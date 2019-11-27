
#include <obs-module.h>
#include "counter.h"
#include <spdlog/spdlog.h>

OBS_DECLARE_MODULE()

bool obs_module_load()
{
	AllocConsole();
	spdlog::set_level(spdlog::level::debug);
	spdlog::info("Initializing Plugin...");
	
	obs_source_info si = {};
	si.id = Counter::source_id;
	si.type = Counter::source_type;
	si.output_flags = Counter::source_flags;

	si.create = [](obs_data_t* data, obs_source_t* source) {
		spdlog::info("source.create");
		return static_cast<void*>(new Counter(data, source));
	};
	si.destroy = [](void* data) {
		spdlog::info("source.destroy");
		delete reinterpret_cast<Counter*>(data);
	};
	si.get_name = [](void* data) {
		spdlog::debug("source.get_name");
		return Counter::source_name;
	};
	si.get_properties = [](void* data) {
		spdlog::debug("source.get_properties");
		return reinterpret_cast<Counter*>(data)->GetProperties();
	};
	si.get_defaults = [](obs_data_t* data) {
		spdlog::debug("source.get_defaults");
		Counter::GetDefaults(data);
	};
	si.free_type_data = [](void* type_data) {
		spdlog::debug("source.free_type_data");
		spdlog::info(sizeof(type_data));
	};
	si.update = [](void* data, obs_data_t* settings) {
		spdlog::debug("source.update");
		return reinterpret_cast<Counter*>(data)->Update(settings);
	};
	
	si.get_width = [](void* data) {
		spdlog::trace("source.get_width");
		return reinterpret_cast<Counter*>(data)->cx;
	};
	si.get_height = [](void* data) {
		spdlog::trace("source.get_height");
		return reinterpret_cast<Counter*>(data)->cy;
	};
	si.video_tick = [](void* data, float seconds) {
		spdlog::trace("source.video_tick {}ms", seconds / 1000);
		return reinterpret_cast<Counter*>(data)->Tick(seconds);
	};
	si.video_render = [](void* data, gs_effect_t* effects) {
		spdlog::trace("source.video_render");
		return reinterpret_cast<Counter*>(data)->Render(effects);
	};

	obs_register_source(&si);
	
	spdlog::info("Plugin Initialized");

	return true;
}

void obs_module_unload()
{
	spdlog::warn("Unloading Plugin...");
}