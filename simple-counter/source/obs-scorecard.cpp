
#include "obs-scorecard.h"


void Scorecard::Update(obs_data_t* update_data)
{
	std::string text = obs_data_get_string(update_data, N_TEXT);
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
		text_texture = GetTextTexture(wtext, update_data, text_size, alignment);
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


