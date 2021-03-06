#pragma once
#include <obs.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include <vector>

#include "common.h"
#include "counter.h"
#include "gdiplus/GdiPlusOBS.h"
#include <spdlog/spdlog.h>

#include <chrono>

#define SCORECARD_ID "scorecard"
#define SCORECARD_NAME "Scorecard"



// todo Instead of having a Scorecard class, just have 'default setups' in the Counter class. which allows the user to select one of few default variable setups
struct Scorecard : GdiPlusOBS
{	
	
public: // Generic Data

	Counter wins, draws, losses;

public: // OBS Source Data

	obs_data_t* data;
	obs_source_t* source;
	Gdiplus::Size* text_size;
	Alignment alignment;

public:

	Scorecard(obs_data_t* _data, obs_source_t* _source) :
		wins(_source, _data, "wins", "Wins"),
		draws(_source, _data,"draws", "Draws"),
		losses(_source, _data, "losses", "Losses"),
		data(_data), source(_source), text_size(new Gdiplus::Size(0,0))
	{
		spdlog::warn("Scorecard");		
		obs_source_update(source, data);

		t1 = std::chrono::high_resolution_clock::now();
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

	static void DefaultData(obs_data_t* settings);

	static obs_properties_t* GetProperties(void* data);

	void Update(obs_data_t* data);
	void Tick(float seconds);
	void Render(gs_effect_t* effects);

	std::chrono::high_resolution_clock clock;
	
	
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


