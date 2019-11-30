
#include "obs-scorecard.h"


void Scorecard::Update(obs_data_t* update_data)
{
	std::string text = obs_data_get_string(update_data, N_SCORECARD_TEXT);
	obs_data_t* font_data = obs_data_get_obj(update_data, N_SCORECARD_FONT);

	if (text.empty()) {
		text_texture = nullptr;
	}
	else {
		std::wstring wtext = ToWide(text.c_str());

		for (text_method m : text_method_list) {
			//wtext = std::regex_replace(wtext, m.get_regex(), m.func(this));
		}

		wtext = wins.text_replace(wtext);
		wtext = draws.text_replace(wtext);
		wtext = losses.text_replace(wtext);

		spdlog::debug(ToNarrow(wtext.c_str()));
		text_texture = GetTextTexture(wtext, font_data, text_size, alignment);
	}

	
}

void Scorecard::Tick(float seconds)
{
}

void Scorecard::Render(gs_effect_t* effects)
{
	if (!text_texture) return;

	gs_effect_t* effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
	gs_technique_t* tech = gs_effect_get_technique(effect, "Draw");

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);

	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), text_texture);
	gs_draw_sprite(text_texture, 0, text_size->Width, text_size->Height);

	gs_technique_end_pass(tech);
	gs_technique_end(tech);
}

void Scorecard::DefaultData(obs_data_t* settings)
{
	obs_data_t* font = obs_data_create();
	obs_data_set_default_string(font, "face", "Arial");
	obs_data_set_default_int(font, "size", 36);

	obs_data_set_default_obj(settings, N_SCORECARD_FONT, font);
	obs_data_set_default_string(settings, N_SCORECARD_TEXT, "Record: ${Wins}W / ${Losses}L");

	obs_data_release(font);
}

obs_properties_t* Scorecard::GetProperties(void* data)
{
	auto source_properties = obs_properties_create();
	obs_properties_t* prop_list = obs_properties_create();
	
	obs_properties_add_font(source_properties, N_SCORECARD_FONT, D_SCORECARD_FONT);
	obs_properties_add_text(source_properties, N_SCORECARD_TEXT, D_SCORECARD_TEXT, OBS_TEXT_MULTILINE);

	obs_properties_add_button(source_properties, N_SCORECARD_VARIABLES_BTN, D_SCORECARD_VARIABLES_BTN, variable_list_button);
	
	
	return source_properties;
}
