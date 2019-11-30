
#include "obs-scorecard.h"


void Scorecard::DefaultData(obs_data_t* settings)
{
	obs_data_t* font = obs_data_create();
	obs_data_set_default_string(font, "face", "Arial");
	obs_data_set_default_int(font, "size", 36);

	obs_data_set_default_obj(settings, N_FONT, font);
	obs_data_set_default_string(settings, N_TEXT, "Record: ${Wins}W / ${Losses}L");
	obs_data_set_default_int(settings, N_FILL_COLOR, 0xFFFFFF);
	obs_data_set_default_int(settings, N_FILL_OPACITY, 100);

	obs_data_set_default_int(settings, N_OUTLINE_COLOR, 0x000000);
	obs_data_set_default_double(settings, N_OUTLINE_THICKNESS, 3);
	obs_data_set_default_int(settings, N_OUTLINE_OPACITY, 100);

	obs_data_set_default_int(settings, N_SHADOW_COLOR, 0x000000);
	obs_data_set_default_int(settings, N_SHADOW_OPACITY, 100);

	obs_data_release(font);
}

obs_properties_t* Scorecard::GetProperties(void* data)
{
	auto source_properties = obs_properties_create();

	obs_properties_add_font(source_properties, N_FONT, "Font");
	obs_properties_add_text(source_properties, N_TEXT, "Text", OBS_TEXT_MULTILINE);

	obs_properties_add_button(source_properties, N_VARIABLES_BTN, "Variables", variable_list_button);

	obs_properties_t* style_group = obs_properties_create();
	obs_properties_add_color(style_group, N_FILL_COLOR, "Fill Color");
	obs_properties_add_int_slider(style_group, N_FILL_OPACITY, "Fill Opacity", 0, 100, 1);

	// todo create seperate groups for outline and shadow, enable and disable these with bools
	obs_properties_add_bool(style_group, N_OUTLINE, "Outline");
	obs_properties_add_float(style_group, N_OUTLINE_THICKNESS, "Outline Size", 0, 50, 0.25);
	obs_properties_add_color(style_group, N_OUTLINE_COLOR, "Outline Color");
	obs_properties_add_int_slider(style_group, N_OUTLINE_OPACITY, "Outline Opacity", 0, 100, 1);

	obs_properties_add_bool(style_group, N_SHADOW, "Shadow");
	obs_properties_add_color(style_group, N_SHADOW_COLOR, "Shadow Color");
	obs_properties_add_int_slider(style_group, N_SHADOW_OPACITY, "Shadow Opacity", 0, 100, 1);

	obs_properties_add_group(source_properties, N_STYLE, "Style", OBS_GROUP_NORMAL, style_group);


	return source_properties;
}

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


