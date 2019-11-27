#pragma once

#include <obs.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>
#include <algorithm>
#include <util/platform.h>
#include "text-methods.h"

#include <spdlog/spdlog.h>

#define MIN_SIZE 2
#define MAX_SIZE 16384
#define MAX_AREA (4096LL * 4096LL)

#define N_TEXT "text"
#define N_FONT "font"
#define N_COUNTER_ADD "add-counter"
#define N_COUNTER_SUB "sub-counter"

#define D_TEXT "Text"
#define D_FONT "Font"
#define D_COUNTER_ADD "Increment Counter"
#define D_COUNTER_SUB "Decrement Counter"

enum struct HAlign
{
	Left,
	Center,
	Right,
};

enum struct VAlign
{
	Top,
	Center,
	Bottom,
};


struct Counter
{
public: // Generic Data
	int value = 0;
	text_method text_method_list[1];
	
public: // OBS Source Data
	static const char* const source_id;
	static const char* const source_name;
	static const obs_source_type source_type = OBS_SOURCE_TYPE_INPUT;
	static const uint32_t source_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;

	obs_data_t* data;
	obs_source_t* source;
	obs_properties_t* source_properties;
	uint32_t cx, cy;

public:	// OBS Text Data
	HDC text_hdc;
	
	HAlign horizontal_align = HAlign::Left;
	VAlign vertical_align = VAlign::Top;

public: // OBS Texture Data
	gs_texture_t* texture = nullptr;
	

	Counter(obs_data_t* _data, obs_source_t* _source) : data(_data), source(_source), source_properties(obs_properties_create()), text_hdc(CreateCompatibleDC(nullptr))
	{
		spdlog::warn("Counter");
		cx = cy = 100;
		text_method_list[0] = { std::wstring(L"Value"), counter_value };

		obs_properties_add_font(source_properties, N_FONT, D_FONT);
		obs_properties_add_text(source_properties, N_TEXT, D_TEXT, OBS_TEXT_DEFAULT);

		obs_hotkey_register_frontend(N_COUNTER_ADD, D_COUNTER_ADD, key_increment_counter, this);
		obs_hotkey_register_frontend(N_COUNTER_SUB, D_COUNTER_SUB, key_decrement_counter, this);
		
		obs_source_update(source, data);
	}

	~Counter()
	{
		spdlog::warn("~Counter");
		if (texture) {
			obs_enter_graphics();
			gs_texture_destroy(texture);
			obs_leave_graphics();
		}
	}


	static void key_increment_counter(void* data, obs_hotkey_id id, obs_hotkey_t* hotkey, bool pressed)
	{
		auto counter = reinterpret_cast<Counter*>(data);
		if (pressed) counter->value++;
		obs_source_update(counter->source, counter->data);
	}

	static void key_decrement_counter(void* data, obs_hotkey_id id, obs_hotkey_t* hotkey, bool pressed)
	{
		auto counter = reinterpret_cast<Counter*>(data);
		if (pressed) counter->value--;
		obs_source_update(counter->source, counter->data);
	}

	static std::wstring counter_value(void* data)
	{
		return std::to_wstring(reinterpret_cast<Counter*>(data)->value);
	}


	void Update(obs_data_t* data);
	void Tick(float seconds);
	void Render(gs_effect_t* effects);
	
	static void GetDefaults(obs_data_t* settings);
	
	Gdiplus::StringFormat* GetTextFormat() const;
	Gdiplus::Font* GetTextFont(obs_data_t* update_data) const;
	void GetTextSize(const std::wstring& text, const Gdiplus::Font& font, const Gdiplus::StringFormat& format, Gdiplus::Size* text_size, Gdiplus::RectF* bounding_box) const;

};
