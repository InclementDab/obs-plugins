#pragma once

#include <obs.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>
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





struct Alignment
{
	enum struct H
	{
		Left,
		Middle,
		Right,
	} HAlign;
	
	enum struct V
	{
		Top,
		Center,
		Bottom,
	} VAlign;

	Alignment() : HAlign(H::Left), VAlign(V::Top) {}
	Alignment(H h, V v) : HAlign(h), VAlign(v) {}
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

	std::vector<obs_hotkey_id> hotkey_list = {};
	obs_data_t* data;
	obs_source_t* source;
	uint32_t cx, cy;

private:	// Members that MUST be updated in Update Callback
	HDC text_hdc;
	Alignment alignment;
	gs_texture_t* text_texture = nullptr;

public: // OBS Texture Data
	

	Counter(obs_data_t* _data, obs_source_t* _source) : data(_data), source(_source), text_hdc(CreateCompatibleDC(nullptr))
	{
		spdlog::warn("Counter");
		cx = cy = 100;
		text_method_list[0] = { std::wstring(L"Value"), counter_value };

		hotkey_list.push_back(obs_hotkey_register_source(source, N_COUNTER_ADD, D_COUNTER_ADD, key_increment_counter, this));
		hotkey_list.push_back(obs_hotkey_register_source(source, N_COUNTER_SUB, D_COUNTER_SUB, key_decrement_counter, this));
		
		obs_source_update(source, data);
	}

	~Counter()
	{
		spdlog::warn("~Counter");
		for (obs_hotkey_id id : hotkey_list) {
			obs_hotkey_unregister(id);
		}
		
		if (text_texture) {
			obs_enter_graphics();
			gs_texture_destroy(text_texture);
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

	gs_texture_t* GetTextTexture(std::wstring& wtext, const Gdiplus::Font& text_font);
	
	static Gdiplus::Font* GetTextFont(obs_data_t* update_data, const HDC& hdc);
	static void GetDefaults(obs_data_t* settings);
	static obs_properties_t* GetProperties();
	static Gdiplus::StringFormat* GetTextFormat(const Alignment& alignment);
	static void GetTextSize(const std::wstring& text,
							const Gdiplus::Font& font,
							const Gdiplus::StringFormat& format,
							const HDC& text_hdc,
							Gdiplus::Size* text_size,
							Gdiplus::RectF* bounding_box);

};
