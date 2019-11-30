#include "obs-simple-counter.h"


obs_properties_t* SimpleCounter::GetProperties(void* data)
{
	auto source_properties = obs_properties_create();

	obs_properties_add_font(source_properties, N_FONT, "Font");
	obs_properties_add_text(source_properties, N_TEXT, "Text", OBS_TEXT_DEFAULT);

	return source_properties;
}


void SimpleCounter::DefaultData(obs_data_t* settings)
{
	obs_data_t* font = obs_data_create();
	obs_data_set_default_string(font, "face", "Arial");
	obs_data_set_default_int(font, "size", 36);

	obs_data_set_default_obj(settings, N_FONT, font);
	obs_data_set_default_string(settings, N_TEXT, "Counter Value: ${Value}");

	obs_data_release(font);
}

void SimpleCounter::Update(obs_data_t* update_data)
{
	std::string text = obs_data_get_string(update_data, N_TEXT);
	spdlog::debug(text);
	//text_font = GdiPlusOBS::GetTextFont(update_data, text_hdc);
	
	if (text.empty()) {
		text_texture = nullptr;
		
	} else {
		std::wstring wtext = ToWide(text.c_str());
		
		for (text_method m : text_method_list) {
			wtext = std::regex_replace(wtext, m.get_regex(), m.func(this));
		}
		
		//text_texture = GdiPlusOBS::GetTextTexture(wtext, alignment);
	}
}

void SimpleCounter::Tick(float seconds)
{

}

void SimpleCounter::Render(gs_effect_t* effects)
{
	if (!text_texture) return;

	gs_technique_t* tech = gs_effect_get_technique(effects, "Draw");

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);

	gs_effect_set_texture(gs_effect_get_param_by_name(effects, "image"), text_texture);
	gs_draw_sprite(text_texture, 0, size.cx, size.cy);

	gs_technique_end_pass(tech);
	gs_technique_end(tech);
}


