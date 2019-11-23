#pragma once

#include <obs-module.h>
#include <util/base.h>
#include <windows.h>
#include <gdiplus.h>

#include "TextSource.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-text-variables", "en-US")
MODULE_EXPORT const char* obs_module_description()
{
	return "Windows GDI+ text source";
}

#define PLUGIN_ID "text_variables"
#define PLUGIN_NAME "basic-text-variables"

static ULONG_PTR gdip_token = 0;


void get_defaults(obs_data_t* settings);
obs_properties_t* get_properties(void* data);

bool obs_module_load()
{
	blog(LOG_INFO, "Loading {}", PLUGIN_NAME);

	obs_source_info si = {};
	si.id = "text_variables";
	si.type = OBS_SOURCE_TYPE_INPUT;
	si.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
	si.get_properties = get_properties;
	si.get_defaults = get_defaults;

	si.get_name = [](void*) { return obs_module_text(PLUGIN_NAME); };
	si.create = [](obs_data_t* settings, obs_source_t* source) {
		return (void*)new TextSource(source, settings);
	};
	si.destroy = [](void* data) {
		delete reinterpret_cast<TextSource*>(data);
	};
	si.get_width = [](void* data) {
		return reinterpret_cast<TextSource*>(data)->cx;
	};
	si.get_height = [](void* data) {
		return reinterpret_cast<TextSource*>(data)->cy;
	};
	si.update = [](void* data, obs_data_t* settings) {
		reinterpret_cast<TextSource*>(data)->Update(settings);
	};
	si.video_tick = [](void* data, float seconds) {
		reinterpret_cast<TextSource*>(data)->Tick(seconds);
	};
	si.video_render = [](void* data, gs_effect_t*) {
		reinterpret_cast<TextSource*>(data)->Render();
	};

	

	obs_register_source(&si);
	const Gdiplus::GdiplusStartupInput gdip_input;
	Gdiplus::GdiplusStartup(&gdip_token, &gdip_input, nullptr);

	return true;
}

void obs_module_unload()
{
	blog(LOG_INFO, "Unloading {}", PLUGIN_NAME);
	Gdiplus::GdiplusShutdown(gdip_token);
}