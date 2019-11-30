
#include "GdiPlusOBS.h"

gs_texture_t* GdiPlusOBS::GetTextTexture(std::wstring& wtext, obs_data_t* update_data, Gdiplus::Size* text_size, const Alignment& alignment)
{
	spdlog::trace("GetTextTexture");

	Gdiplus::Graphics graphics(text_hdc);
	Gdiplus::RectF bounding_box;

#pragma region OBSGetData

	// Font
	obs_data_t* font_data = obs_data_get_obj(update_data, N_FONT);
	std::wstring font_name = ToWide(obs_data_get_string(font_data, "face"));
	long long font_size = obs_data_get_int(font_data, "size");
	int64_t font_flags = obs_data_get_int(font_data, "flags");
	obs_data_release(font_data);

	uint32_t text_color = (uint32_t)obs_data_get_int(update_data, N_COLOR);
	int text_opacity = obs_data_get_int(update_data, N_OPACITY);
	
#pragma endregion
	
#pragma region GetFont
	
	spdlog::trace("GetTextFont");
	LOGFONT* lf = new LOGFONT();
	std::copy(font_name.begin(), font_name.end(), lf->lfFaceName);
	lf->lfHeight = font_size;
	lf->lfWeight = font_flags & OBS_FONT_BOLD ? FW_BOLD : FW_DONTCARE;
	lf->lfItalic = font_flags & OBS_FONT_ITALIC;
	lf->lfUnderline = font_flags & OBS_FONT_UNDERLINE;
	lf->lfStrikeOut = font_flags & OBS_FONT_STRIKEOUT;
	lf->lfQuality = ANTIALIASED_QUALITY;
	lf->lfCharSet = DEFAULT_CHARSET;

	auto text_font = new Gdiplus::Font(text_hdc, lf);
#pragma endregion
	
#pragma region StringFormat
	spdlog::trace("StringFormat");
	gs_texture_t* texture = nullptr;
	Gdiplus::Status status;

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
	spdlog::trace("GetTextSize");

	const WCHAR* wtest = wtext.c_str();
	status = graphics.MeasureString(wtest, wcslen(wtest), text_font, Gdiplus::PointF(0.0f, 0.0f), text_format, &bounding_box);
	if (status) {
		spdlog::error("Error in MeasureString: {}", status);
		return nullptr;
	}

	text_size->Height = bounding_box.Height = bounding_box.Height < text_font->GetSize() ? text_font->GetSize() : LONG(bounding_box.Height + EPSILON);
	text_size->Width = LONG(bounding_box.Width + EPSILON);

	text_size->Height += text_size->Height % 2;	text_size->Width += text_size->Width % 2;
	Clamp(&text_size->Height, MIN_SIZE, MAX_SIZE); Clamp(&text_size->Width, MIN_SIZE, MAX_SIZE);

	int64_t font_area = int64_t(text_size->Height) * int64_t(text_size->Width);
	if (font_area > MAX_AREA) {
		if (text_size->Height > text_size->Width)	text_size->Height = MAX_AREA / text_size->Width;
		else text_size->Width = MAX_AREA / text_size->Height;
	}

#pragma endregion

	std::unique_ptr<uint8_t[]> bits(new uint8_t[text_size->Height * text_size->Width * 4]);
	Gdiplus::Bitmap bitmap(text_size->Width, text_size->Height, 4 * text_size->Width, PixelFormat32bppARGB, bits.get());
	Gdiplus::Graphics graphics_bitmap(&bitmap);	
	Gdiplus::SolidBrush brush(GetColorValue(text_color, text_opacity));
	
	
#pragma region DrawToGraphics

	graphics_bitmap.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	graphics_bitmap.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	graphics_bitmap.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	status = graphics_bitmap.Clear(Gdiplus::Color(0));
	if (status) {
		spdlog::error("Error in Clear: {}", status);
		return nullptr;
	}

	status = graphics_bitmap.DrawString(wtest, wcslen(wtest), text_font, bounding_box, text_format, &brush);
	if (status) {
		spdlog::error("Error in DrawString: {}", status);
		return nullptr;
	}

	obs_enter_graphics();

	if (!texture) {
		const uint8_t* data = static_cast<uint8_t*>(bits.get());
		texture = gs_texture_create(text_size->Width, text_size->Height, GS_BGRA, 1, &data, GS_DYNAMIC);
	}
	else {
		gs_texture_set_image(texture, bits.get(), text_size->Width * 4, false);
	}

	obs_leave_graphics();
#pragma endregion
	
	delete text_font;
	return texture;
}




