
#include "obs-scorecard.h"


static bool group_toggle_cb(void* _group, obs_properties_t* props, obs_property_t* property, obs_data_t* settings)
{
	auto group = reinterpret_cast<obs_property_t*>(_group);

	auto state = obs_data_get_bool(settings, obs_property_name(property));
	if (obs_property_visible(group) != state) {
		obs_property_set_visible(group, state);
		return true;
	}
	return false;
}




void Scorecard::DefaultData(obs_data_t* settings)
{
	obs_data_t* font = obs_data_create();
	obs_data_set_default_string(font, "face", "Arial");
	obs_data_set_default_int(font, "size", 36);

	obs_data_set_default_obj(settings, N_FONT, font);
	obs_data_set_default_string(settings, N_TEXT, "Record: ${Wins}W / ${Losses}L");
	obs_data_set_default_int(settings, N_FILL_COLOR, 0xFFFFFF);
	obs_data_set_default_int(settings, N_FILL_OPACITY, 100);
	obs_data_set_default_bool(settings, N_OUTLINE, false);
	obs_data_set_default_bool(settings, N_SHADOW, false);


	obs_data_set_default_int(settings, N_OUTLINE_COLOR, 0x000000);
	obs_data_set_default_double(settings, N_OUTLINE_THICKNESS, 6);
	obs_data_set_default_int(settings, N_OUTLINE_OPACITY, 100);

	obs_data_set_default_int(settings, N_SHADOW_ROTATION, 0);
	obs_data_set_default_int(settings, N_SHADOW_COLOR, 0x000000);
	obs_data_set_default_double(settings, N_SHADOW_DISTANCE, 4);
	obs_data_set_default_double(settings, N_SHADOW_SIZE, 1);
	obs_data_set_default_int(settings, N_SHADOW_OPACITY, 100);

	obs_data_release(font);
}

obs_properties_t* Scorecard::GetProperties(void* data)
{

	obs_properties_t* source_properties = obs_properties_create();
	auto font = obs_properties_add_font(source_properties, N_FONT, "Font");
	obs_properties_add_text(source_properties, N_TEXT, "Text", OBS_TEXT_MULTILINE);
	obs_properties_add_button(source_properties, N_VARIABLES_BTN, "Variables", variable_list_button);


	obs_properties_add_color(source_properties, N_FILL_COLOR, "Fill Color");
	obs_properties_add_int_slider(source_properties, N_FILL_OPACITY, "Fill Opacity", 0, 100, 1);


	obs_properties_t* outline_group = obs_properties_create();
	{
		obs_properties_add_int(outline_group, N_OUTLINE_THICKNESS, "Size", 0, 50, 1);
		obs_properties_add_color(outline_group, N_OUTLINE_COLOR, "Color");
		obs_properties_add_int_slider(outline_group, N_OUTLINE_OPACITY, "Opacity", 0, 100, 1);
	}

	auto outline_button = obs_properties_add_bool(source_properties, N_OUTLINE, "Outline");
	obs_property_t* outline_group_prop = obs_properties_add_group(source_properties, N_OUTLINE_GROUP, "Outline Settings", OBS_GROUP_NORMAL, outline_group);
	obs_property_set_visible(outline_group_prop, false);
	obs_property_set_modified_callback2(outline_button, group_toggle_cb, outline_group_prop);


	obs_properties_t* shadow_group = obs_properties_create();
	{
		obs_properties_add_float(shadow_group, N_SHADOW_SIZE, "Size", 1, 10, 0.1);
		obs_properties_add_float_slider(shadow_group, N_SHADOW_ROTATION, "Rotation", -180, 180, 0.5);
		obs_properties_add_float_slider(shadow_group, N_SHADOW_DISTANCE, "Distance", 0, 10, 0.1);
		obs_properties_add_color(shadow_group, N_SHADOW_COLOR, "Color");
		obs_properties_add_int_slider(shadow_group, N_SHADOW_OPACITY, "Opacity", 0, 100, 1);
	}
	auto shadow_button = obs_properties_add_bool(source_properties, N_SHADOW, "Shadow");
	obs_property_t* shadow_group_prop = obs_properties_add_group(source_properties, N_SHADOW_GROUP, "Shadow Settings", OBS_GROUP_NORMAL, shadow_group);
	obs_property_set_visible(shadow_group_prop, false);
	obs_property_set_modified_callback2(shadow_button, group_toggle_cb, shadow_group_prop);
	
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

		wins.text_replace(&wtext);
		draws.text_replace(&wtext);
		losses.text_replace(&wtext);

		spdlog::debug(ToNarrow(wtext.c_str()));
		*text_size = UpdateTextTexture(wtext, update_data, alignment);
		
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


