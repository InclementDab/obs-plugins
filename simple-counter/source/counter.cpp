#include "counter.h"
#include <locale>
#include <regex>
#include "utils.h"

const char* const Counter::source_id = "counter";
const char* const Counter::source_name = "Counter";

obs_properties_t* Counter::GetProperties()
{
	auto source_properties = obs_properties_create();

	obs_properties_add_font(source_properties, N_FONT, D_FONT);
	obs_properties_add_text(source_properties, N_TEXT, D_TEXT, OBS_TEXT_DEFAULT);

	return source_properties;
}

void Counter::GetDefaults(obs_data_t* settings)
{
	obs_data_t* font = obs_data_create();
	obs_data_set_default_string(font, "face", "Arial");
	obs_data_set_default_int(font, "size", 36);

	obs_data_set_default_obj(settings, N_FONT, font);
	obs_data_set_default_string(settings, N_TEXT, "Counter Value: ${Value}");

	obs_data_release(font);
}

void Counter::Update(obs_data_t* update_data)
{
	std::string text = obs_data_get_string(update_data, N_TEXT);
	
	if (text.empty()) {
		text_texture = nullptr;
		
	} else {
		Gdiplus::Font* font = GetTextFont(update_data, text_hdc);
		std::wstring wtext = ToWide(text.c_str());
		
		for (text_method m : text_method_list) {
			wtext = std::regex_replace(wtext, m.get_regex(), m.func(this));
		}
		
		spdlog::debug(ToNarrow(wtext.c_str()));
		text_texture = GetTextTexture(wtext, *font);
		delete font;
	}
	
	//if (update_data) obs_data_release(update_data);

}

void Counter::Tick(float seconds)
{

}

void Counter::Render(gs_effect_t* effects)
{
	if (!text_texture) return;

	gs_effect_t* effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
	gs_technique_t* tech = gs_effect_get_technique(effect, "Draw");

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);

	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), text_texture);
	gs_draw_sprite(text_texture, 0, cx, cy);

	gs_technique_end_pass(tech);
	gs_technique_end(tech);
}

gs_texture_t* Counter::GetTextTexture(std::wstring& wtext, const Gdiplus::Font& text_font)
{
	gs_texture_t* texture = nullptr;
	Gdiplus::Status status;
	Gdiplus::RectF bbox;
	Gdiplus::Size size;


	auto text_format = GetTextFormat(alignment);
	GetTextSize(wtext, text_font, text_format, text_hdc, &size, &bbox);

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
		return nullptr;
	}

	status = graphics_bitmap.DrawString(wtest, wcslen(wtest), &text_font, bbox, text_format, &brush);
	if (status) {
		spdlog::error("Error in DrawString: {}", status);
		return nullptr;
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

	return texture;
}

#pragma region Text_Internals

Gdiplus::StringFormat* Counter::GetTextFormat(const Alignment& alignment)
{
	spdlog::trace("GetTextFormat");
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

	return text_format;
}

Gdiplus::Font* Counter::GetTextFont(obs_data_t* update_data, const HDC& hdc)
{
	spdlog::trace("GetTextFont");
	obs_data_t* font_data = obs_data_get_obj(update_data, N_FONT);
	std::wstring font_name = ToWide(obs_data_get_string(font_data, "face"));
	long long font_size = obs_data_get_int(font_data, "size");
	int64_t font_flags = obs_data_get_int(font_data, "flags");
	obs_data_release(font_data);

	LOGFONT* lf = new LOGFONT();
	std::copy(font_name.begin(), font_name.end(), lf->lfFaceName);
	lf->lfHeight = font_size;
	lf->lfWeight = font_flags & OBS_FONT_BOLD ? FW_BOLD : FW_DONTCARE;
	lf->lfItalic = font_flags & OBS_FONT_ITALIC;
	lf->lfUnderline = font_flags & OBS_FONT_UNDERLINE;
	lf->lfStrikeOut = font_flags & OBS_FONT_STRIKEOUT;
	lf->lfQuality = ANTIALIASED_QUALITY;
	lf->lfCharSet = DEFAULT_CHARSET;

	return Gdiplus::Font(hdc, lf).Clone();
}

void Counter::GetTextSize(const std::wstring& text, 
						 const Gdiplus::Font& font, 
						 const Gdiplus::StringFormat& format,
						 const HDC& text_hdc,
						 Gdiplus::Size* text_size,
						 Gdiplus::RectF* bounding_box)
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