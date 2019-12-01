
#include "GdiPlusOBS.h"

static Gdiplus::Size MeasureText(Gdiplus::Graphics* graphics, std::wstring wtext, Gdiplus::Font* text_font, Gdiplus::StringFormat text_format)
{
	spdlog::trace("GetTextSize");
	Gdiplus::Size text_size;
	Gdiplus::RectF bounding_box;
	auto status = graphics->MeasureString(wtext.c_str(), wtext.length(), text_font, Gdiplus::PointF(0, 0), &text_format, &bounding_box); GdiPlusOBS::CheckStatus(status, "MeasureString");

	text_size.Height = bounding_box.Height = bounding_box.Height < text_font->GetSize() ? text_font->GetSize() : LONG(bounding_box.Height + EPSILON);
	text_size.Width = LONG(bounding_box.Width + EPSILON);

	text_size.Height += text_size.Height % 2;	text_size.Width += text_size.Width % 2;
	Clamp(&text_size.Height, MIN_SIZE, MAX_SIZE); Clamp(&text_size.Width, MIN_SIZE, MAX_SIZE);

	int64_t font_area = int64_t(text_size.Height) * int64_t(text_size.Width);
	if (font_area > MAX_AREA) {
		if (text_size.Height > text_size.Width)	text_size.Height = MAX_AREA / text_size.Width;
		else text_size.Width = MAX_AREA / text_size.Height;
	}
	return text_size;
}

Gdiplus::Size GdiPlusOBS::UpdateTextTexture(std::wstring& wtext, obs_data_t* update_data, const Alignment& alignment)
{
	spdlog::trace("GetTextTexture");
	Gdiplus::Size text_size;
	
	Gdiplus::Status status;
	Gdiplus::Graphics graphics(text_hdc);
	

#pragma region OBSGetData

	// Font
	obs_data_t* font_data = obs_data_get_obj(update_data, N_FONT);
	std::wstring font_name = ToWide(obs_data_get_string(font_data, "face"));
	auto font_size = obs_data_get_int(font_data, "size");
	auto font_flags = obs_data_get_int(font_data, "flags");
	obs_data_release(font_data);

	auto fill_color = obs_data_get_int(update_data, N_FILL_COLOR);
	auto fill_opacity = obs_data_get_int(update_data, N_FILL_OPACITY);

	auto outline_enable = obs_data_get_bool(update_data, N_OUTLINE);
	auto outline_color = obs_data_get_int(update_data, N_OUTLINE_COLOR);
	auto outline_thickness = obs_data_get_double(update_data, N_OUTLINE_THICKNESS);
	auto outline_opacity = obs_data_get_int(update_data, N_OUTLINE_COLOR);

	auto shadow_enable = obs_data_get_bool(update_data, N_SHADOW);
	auto shadow_color = obs_data_get_int(update_data, N_SHADOW_COLOR);
	auto shadow_opacity = obs_data_get_int(update_data, N_SHADOW_COLOR);
	auto shadow_angle = obs_data_get_double(update_data, N_SHADOW_ROTATION);
	auto shadow_distance = obs_data_get_double(update_data, N_SHADOW_DISTANCE);
	auto shadow_size = obs_data_get_double(update_data, N_SHADOW_SIZE);


#pragma endregion

#pragma region GetFont

	spdlog::trace("GetTextFont");
	LOGFONT lf;
	std::copy(font_name.begin(), font_name.end(), lf.lfFaceName);
	lf.lfHeight = font_size;
	lf.lfWeight = font_flags & OBS_FONT_BOLD ? FW_BOLD : FW_DONTCARE;
	lf.lfItalic = font_flags & OBS_FONT_ITALIC;
	lf.lfUnderline = font_flags & OBS_FONT_UNDERLINE;
	lf.lfStrikeOut = font_flags & OBS_FONT_STRIKEOUT;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfCharSet = DEFAULT_CHARSET;

	auto text_font = new Gdiplus::Font(text_hdc, &lf);

	Gdiplus::FontFamily font_family;
	text_font->GetFamily(&font_family);
	
#pragma endregion

#pragma region StringFormat
	spdlog::trace("StringFormat");

	auto text_format = new Gdiplus::StringFormat(Gdiplus::StringFormat::GenericTypographic());
	text_format->SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
	text_format->SetTrimming(Gdiplus::StringTrimmingWord);

	switch (alignment.HAlign) {

		case Alignment::H::Left:
			text_format->SetAlignment(Gdiplus::StringAlignmentNear);
			break;

		case Alignment::H::Middle:
			text_format->SetAlignment(Gdiplus::StringAlignmentCenter);
			break;

		case Alignment::H::Right:
			text_format->SetAlignment(Gdiplus::StringAlignmentFar);
			break;
	}

	switch (alignment.VAlign) {

		case Alignment::V::Top:
			text_format->SetLineAlignment(Gdiplus::StringAlignmentNear);
			break;

		case Alignment::V::Center:
			text_format->SetLineAlignment(Gdiplus::StringAlignmentCenter);
			break;

		case Alignment::V::Bottom:
			text_format->SetLineAlignment(Gdiplus::StringAlignmentFar);
			break;
	}

#pragma endregion

#pragma region TextSize
	

#pragma endregion

#pragma region DrawToGraphics


	text_size = MeasureText(&graphics, wtext, text_font, text_format);
	
	Gdiplus::GraphicsPath graphics_path;
	
	if (outline_enable) {
		Gdiplus::SolidBrush outline_brush(GetColorValue(outline_color, outline_opacity));
		Gdiplus::Pen outline_pen(&outline_brush, outline_thickness);
		status = outline_pen.SetLineJoin(Gdiplus::LineJoinRound); CheckStatus(status, "SetLineJoin");	
	}

	Gdiplus::SolidBrush shadow_brush(GetColorValue(shadow_color, shadow_opacity));
	Gdiplus::PointF shadow_point(cos(shadow_angle * 3.1415 / 180) * shadow_distance,
								 sin(shadow_angle * 3.1415 / 180) * shadow_distance);
	
	if (shadow_enable) text_size.Width += shadow_point.X*2; text_size.Height += shadow_point.Y*2;

	auto scan0 = std::make_unique<uint8_t[]>(text_size.Width * text_size.Height * 4);
	Gdiplus::Bitmap bitmap(text_size.Width, text_size.Height, 4 * text_size.Width, PixelFormat32bppARGB, scan0.get());
	Gdiplus::Graphics graphics_bitmap(&bitmap);
	status = graphics_bitmap.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias); CheckStatus(status, "SetTextRenderingHint");
	status = graphics_bitmap.SetCompositingMode(Gdiplus::CompositingModeSourceOver); CheckStatus(status, "SetCompositingMode");
	status = graphics_bitmap.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias); CheckStatus(status, "SetSmoothingMode");
	status = graphics_bitmap.Clear(Gdiplus::Color(0)); CheckStatus(status, "Clear");

	Gdiplus::SolidBrush fill_brush(GetColorValue(fill_color, fill_opacity));
	//status = graphics_bitmap.FillPath(&shadow_brush, &shadow_path); CheckStatus(status, "Shadow.FillPath");
	//status = graphics_bitmap.FillPath(&fill_brush, &graphics_path); CheckStatus(status, "Main.FillPath");

	if (shadow_enable) {
		status = graphics_bitmap.DrawString(wtext.c_str(), wtext.length(), text_font, shadow_point, text_format, &shadow_brush); CheckStatus(status, "Shadow.DrawString");
	}
	
	status = graphics_bitmap.DrawString(wtext.c_str(), wtext.length(), text_font, Gdiplus::PointF(0, 0), text_format, &fill_brush); CheckStatus(status, "Main.DrawString");
	
	obs_enter_graphics();
	const uint8_t* data = static_cast<uint8_t*>(scan0.get());
	text_texture = gs_texture_create(text_size.Width, text_size.Height, GS_BGRA, 1, &data, GS_DYNAMIC);
	obs_leave_graphics();

	//text_size.Height = graphics_rect.Height; text_size.Width = graphics_rect.Width;
#pragma endregion
	delete text_font;
	return text_size;
}




