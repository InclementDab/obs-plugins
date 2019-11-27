#include "counter.h"
#include <locale>
#include <regex>
#include "utils.h"

const char* const Counter::source_id = "counter";
const char* const Counter::source_name = "Counter";

void Counter::GetDefaults(obs_data_t* settings)
{
	obs_data_t* font = obs_data_create();
	obs_data_set_default_string(font, "face", "Arial");
	obs_data_set_default_int(font, "size", 36);

	obs_data_set_default_obj(settings, N_FONT, font);
	obs_data_set_default_string(settings, N_TEXT, "${Value}");

	obs_data_release(font);
}

void Counter::Update(obs_data_t* update_data)
{
	auto t = obs_data_get_string(update_data, N_TEXT);
	//t += '\n';
	std::wstring wtext = ToWide(t);

#pragma region RenderText

	Gdiplus::Status status;
	Gdiplus::RectF bbox;
	Gdiplus::Size size;

	auto text_font = GetTextFont(update_data);
	auto text_format = GetTextFormat();

	spdlog::info("Text: {}", ToNarrow(wtext.c_str()));

	for (text_method m : text_method_list) {
		wtext = std::regex_replace(wtext, m.get_regex(), m.func(this));
	}

	
	GetTextSize(wtext, *text_font, text_format, &size, &bbox);

	std::unique_ptr<uint8_t[]> bits(new uint8_t[size.Width * size.Height * 4]);
	Gdiplus::Bitmap bitmap(size.Width, size.Height, 4 * size.Width, PixelFormat32bppARGB, bits.get());
	Gdiplus::Graphics graphics_bitmap(&bitmap);
	Gdiplus::SolidBrush brush(Gdiplus::Color::White);

	graphics_bitmap.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	graphics_bitmap.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	graphics_bitmap.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	const WCHAR* wtest = wtext.c_str();

	status = graphics_bitmap.Clear(Gdiplus::Color(0));
	if (status) {
		spdlog::error("Error in Clear: {}", status);
		return;
	}
	
	status = graphics_bitmap.DrawString(wtest, wcslen(wtest), text_font, bbox, text_format, &brush);
	if (status) {
		spdlog::error("Error in DrawString: {}", status);
		return;
	}

	if ((LONG)cx != size.Width || (LONG)cy != size.Height) {
		obs_enter_graphics();
		if (texture) gs_texture_destroy(texture);

		const uint8_t* data = (uint8_t*)bits.get();
		texture = gs_texture_create(size.Width, size.Height, GS_BGRA, 1, &data, GS_DYNAMIC);

		obs_leave_graphics();

		cx = size.Width;
		cy = size.Height;

	}
	else if (texture) {
		obs_enter_graphics();
		gs_texture_set_image(texture, bits.get(), size.Width * 4, false);
		obs_leave_graphics();
	}

	delete text_font; delete text_format;

#pragma endregion

	//if (update_data) obs_data_release(update_data);

}

void Counter::Tick(float seconds)
{

}

void Counter::Render(gs_effect_t* effects)
{
	if (!texture) return;

	gs_effect_t* effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
	gs_technique_t* tech = gs_effect_get_technique(effect, "Draw");

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);

	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), texture);
	gs_draw_sprite(texture, 0, cx, cy);

	gs_technique_end_pass(tech);
	gs_technique_end(tech);
}

#pragma region Text_Internals

Gdiplus::StringFormat* Counter::GetTextFormat() const
{
	spdlog::trace("GetTextFormat");
	auto text_format = new Gdiplus::StringFormat(Gdiplus::StringFormat::GenericTypographic());
	text_format->SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
	text_format->SetTrimming(Gdiplus::StringTrimmingWord);

	switch (horizontal_align) {

		case HAlign::Left:
			text_format->SetAlignment(Gdiplus::StringAlignmentNear);
			break;

		case HAlign::Center:
			text_format->SetAlignment(Gdiplus::StringAlignmentCenter);
			break;

		case HAlign::Right:
			text_format->SetAlignment(Gdiplus::StringAlignmentFar);
			break;
	}

	switch (vertical_align) {

		case VAlign::Top:
			text_format->SetLineAlignment(Gdiplus::StringAlignmentNear);
			break;

		case VAlign::Center:
			text_format->SetLineAlignment(Gdiplus::StringAlignmentCenter);
			break;

		case VAlign::Bottom:
			text_format->SetLineAlignment(Gdiplus::StringAlignmentFar);
			break;
	}

	return text_format;
}

Gdiplus::Font* Counter::GetTextFont(obs_data_t* update_data) const
{
	spdlog::trace("GetTextFont");
	obs_data_t* font_data = obs_data_get_obj(update_data, N_FONT);
	std::wstring font_name = ToWide(obs_data_get_string(font_data, "face"));
	long long font_size = obs_data_get_int(font_data, "size");
	int64_t font_flags = obs_data_get_int(font_data, "flags");
	obs_data_release(font_data);

	LOGFONT lf = {};
	std::copy(font_name.begin(), font_name.end(), lf.lfFaceName);
	lf.lfHeight = font_size;
	lf.lfWeight = font_flags & OBS_FONT_BOLD ? FW_BOLD : FW_DONTCARE;
	lf.lfItalic = font_flags & OBS_FONT_ITALIC;
	lf.lfUnderline = font_flags & OBS_FONT_UNDERLINE;
	lf.lfStrikeOut = font_flags & OBS_FONT_STRIKEOUT;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfCharSet = DEFAULT_CHARSET;
	
	return new Gdiplus::Font(text_hdc, &lf);
}

void Counter::GetTextSize(const std::wstring& text, 
						 const Gdiplus::Font& font, 
						 const Gdiplus::StringFormat& format,
						 Gdiplus::Size* text_size,
						 Gdiplus::RectF* bounding_box) const
{
	spdlog::trace("GetTextSize");
	Gdiplus::Graphics graphics(text_hdc);
	Gdiplus::Status status;

	status = graphics.MeasureString(text.c_str(), text.size() + 1, &font, Gdiplus::PointF(0.0f, 0.0f), &format,
									bounding_box);

	if (status) {
		spdlog::error("Error in MeasureString: {}", status);
		return;
	}

	Gdiplus::REAL font_size = font.GetSize(); //obs_data_get_int(nullptr, "size");

	text_size->Height = bounding_box->Height =
		bounding_box->Height < font_size ? font_size : LONG(bounding_box->Height + EPSILON);

	text_size->Width = LONG(bounding_box->Width + EPSILON);

	text_size->Width += text_size->Width % 2;
	text_size->Height += text_size->Height % 2;

	int64_t font_area = int64_t(text_size->Width) * int64_t(text_size->Height);

	/* GPUs typically have texture size limitations */
	Clamp(&text_size->Width, MIN_SIZE, MAX_SIZE);
	Clamp(&text_size->Height, MIN_SIZE, MAX_SIZE);

	if (font_area > MAX_AREA) {

		if (text_size->Width > text_size->Height)
			text_size->Width = MAX_AREA / text_size->Height;
		else
			text_size->Height = MAX_AREA / text_size->Width;
	}

}




#pragma endregion