#pragma once

#include <obs.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <util/platform.h>
#include "common.h"

#include <spdlog/spdlog.h>
#include "gdiplus/GdiPlusOBS.h"
#include "counter.h"


#define COUNTER_ID "simple-counter"
#define COUNTER_NAME "Simple Counter"
#define N_TEXT "text"
#define N_FONT "font"
#define N_COUNTER_ADD "add-counter"
#define N_COUNTER_SUB "sub-counter"

#define D_TEXT "Text"
#define D_FONT "Font"
#define D_COUNTER_ADD "Increment Counter"
#define D_COUNTER_SUB "Decrement Counter"



struct SimpleCounter
{

public: // Generic Data

	Counter counter;
	std::vector<text_method> text_method_list = {};

public: // OBS Source Data

	obs_data_t* data;
	obs_source_t* source;
	SIZE size = { 10, 10 };
	Gdiplus::Font* text_font = nullptr;

private:	// Members that MUST be updated in Update Callback

	HDC text_hdc;
	Alignment alignment;
	gs_texture_t* text_texture = nullptr;

public:

	SimpleCounter(obs_data_t* _data, obs_source_t* _source) :
		counter(_source, _data, "counter", "Counter"),
		data(_data), source(_source),
		text_hdc(CreateCompatibleDC(nullptr))
	{
		spdlog::warn("SimpleCounter");

		obs_source_update(source, data);
	}

	~SimpleCounter()
	{
		spdlog::warn("~SimpleCounter");

		if (text_texture) {
			obs_enter_graphics();
			gs_texture_destroy(text_texture);
			obs_leave_graphics();
		}

		delete text_font;
	}

	static void DefaultData(obs_data_t* settings);
	static obs_properties_t* GetProperties(void* data);

	void Update(obs_data_t* data);
	void Tick(float seconds);
	void Render(gs_effect_t* effects);

};


struct OBS_SimpleCounter_Info : obs_source_info
{
	OBS_SimpleCounter_Info() : obs_source_info()
	{
		id = COUNTER_ID;
		type = OBS_SOURCE_TYPE_INPUT;
		output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
		get_defaults = SimpleCounter::DefaultData;
		get_properties = SimpleCounter::GetProperties;

		create = [](obs_data_t* data, obs_source_t* source) {
			spdlog::info("OBS_Scorecard_Info.create");
			return static_cast<void*>(new SimpleCounter(data, source));
		};
		destroy = [](void* data) {
			spdlog::info("OBS_Scorecard_Info.destroy");
			delete reinterpret_cast<SimpleCounter*>(data);
		};
		get_name = [](void*) {
			spdlog::info("OBS_Scorecard_Info.get_name");
			return COUNTER_NAME;
		};
		update = [](void* data, obs_data_t* settings) {
			spdlog::debug("OBS_Scorecard_Info.update");
			return reinterpret_cast<SimpleCounter*>(data)->Update(settings);
		};
		get_width = [](void* data) -> uint32_t {
			spdlog::trace("OBS_Scorecard_Info.get_width");
			return reinterpret_cast<SimpleCounter*>(data)->size.cx;
		};
		get_height = [](void* data) -> uint32_t {
			spdlog::trace("OBS_Scorecard_Info.get_height");
			return reinterpret_cast<SimpleCounter*>(data)->size.cy;
		};
		video_tick = [](void* data, float seconds) {
			spdlog::trace("OBS_Scorecard_Info.video_tick {}s", seconds);
			return reinterpret_cast<SimpleCounter*>(data)->Tick(seconds);
		};
		video_render = [](void* data, gs_effect_t* effects) {
			spdlog::trace("OBS_Scorecard_Info.video_render");
			return reinterpret_cast<SimpleCounter*>(data)->Render(effects);
		};
	}
};

