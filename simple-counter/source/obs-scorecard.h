#pragma once
#include <obs.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include <vector>

#include "common.h"
#include "counter.h"
#include "gdiplus/GdiPlusOBS.h"
#include <spdlog/spdlog.h>

#define SCORECARD_ID "scorecard"
#define SCORECARD_NAME "Scorecard"




struct Scorecard : GdiPlusOBS
{

public: // Generic Data

	Counter wins, draws, losses;
	std::vector<text_method> text_method_list = {};

public: // OBS Source Data

	obs_data_t* data;
	obs_source_t* source;
	Gdiplus::Size* text_size;


private:	// Members that MUST be updated in Update Callback

	Alignment alignment;
	gs_texture_t* text_texture = nullptr;

public:

	Scorecard(obs_data_t* _data, obs_source_t* _source) : 
		wins(_source, _data, "wins", "Wins"),
		draws(_source, _data,"draws", "Draws"),
		losses(_source, _data, "losses", "Losses"),
		data(_data), source(_source), text_size(new Gdiplus::Size(0,0))
	{
		spdlog::warn("Scorecard");

		text_method_list.push_back(wins.get_text_method());
		text_method_list.push_back(draws.get_text_method());
		text_method_list.push_back(losses.get_text_method());
		
		obs_source_update(source, data);
	}

	~Scorecard()
	{
		spdlog::warn("~Scorecard");

		if (text_texture) {
			obs_enter_graphics();
			gs_texture_destroy(text_texture);
			obs_leave_graphics();
		}

		delete text_size;
	}

	static bool variable_list_button(obs_properties_t* props, obs_property_t* property, void* data)
	{
		spdlog::info("button_variables_list");
		return true;
	}

	static void DefaultData(obs_data_t* settings)
	{
		obs_data_t* font = obs_data_create();
		obs_data_set_default_string(font, "face", "Arial");
		obs_data_set_default_int(font, "size", 36);

		obs_data_set_default_obj(settings, N_FONT, font);
		obs_data_set_default_string(settings, N_TEXT, "Record: ${Wins}W / ${Losses}L");
		obs_data_set_default_int(settings, N_COLOR, 0xFFFFFF);
		obs_data_set_default_int(settings, N_OPACITY, 100);


		obs_data_release(font);
	}
	
	static obs_properties_t* GetProperties(void* data)
	{
		auto source_properties = obs_properties_create();
		obs_properties_t* prop_list = obs_properties_create();

		obs_properties_add_font(source_properties, N_FONT, "Font");
		obs_properties_add_text(source_properties, N_TEXT, "Text", OBS_TEXT_MULTILINE);

		obs_properties_add_button(source_properties, N_VARIABLES_BTN, "Variables", variable_list_button);

		obs_properties_add_color(source_properties, N_COLOR, "Fill Color");
		obs_properties_add_int_slider(source_properties, N_OPACITY, "Fill Opacity", 0, 100, 1);

		return source_properties;
	}

	void Update(obs_data_t* data);
	void Tick(float seconds);
	void Render(gs_effect_t* effects);
};


struct OBS_Scorecard_Info : obs_source_info
{
	OBS_Scorecard_Info() : obs_source_info()
	{
		id = SCORECARD_ID;
		type = OBS_SOURCE_TYPE_INPUT;
		output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
		get_defaults = Scorecard::DefaultData;
		get_properties = Scorecard::GetProperties;

		create = [](obs_data_t* data, obs_source_t* source) {
			spdlog::info("OBS_Scorecard_Info.create");
			return static_cast<void*>(new Scorecard(data, source));
		};
		destroy = [](void* data) {
			spdlog::info("OBS_Scorecard_Info.destroy");
			delete reinterpret_cast<Scorecard*>(data);
		};
		get_name = [](void*) {
			spdlog::info("OBS_Scorecard_Info.get_name");
			return SCORECARD_NAME;
		};
		update = [](void* data, obs_data_t* settings) {
			spdlog::debug("OBS_Scorecard_Info.update");
			return reinterpret_cast<Scorecard*>(data)->Update(settings);
		};
		get_width = [](void* data) -> uint32_t {
			spdlog::trace("OBS_Scorecard_Info.get_width");
			return reinterpret_cast<Scorecard*>(data)->text_size->Width;
		};
		get_height = [](void* data) -> uint32_t {
			spdlog::trace("OBS_Scorecard_Info.get_height");
			return reinterpret_cast<Scorecard*>(data)->text_size->Height;
		};
		video_tick = [](void* data, float seconds) {
			spdlog::trace("OBS_Scorecard_Info.video_tick {}s", seconds);
			return reinterpret_cast<Scorecard*>(data)->Tick(seconds);
		};
		video_render = [](void* data, gs_effect_t* effects) {
			spdlog::trace("OBS_Scorecard_Info.video_render");
			return reinterpret_cast<Scorecard*>(data)->Render(effects);
		};
	}
};


