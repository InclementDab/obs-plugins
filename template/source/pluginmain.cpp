

#include <obs-module.h>
#include <spdlog/spdlog.h>

OBS_DECLARE_MODULE()

bool obs_module_load()
{
#ifdef DEBUG
	AllocConsole();
	spdlog::set_level(spdlog::level::debug);
#elif RELEASE
	spdlog::set_level(spdlog::level::warn);
#endif
	spdlog::info("Initilizing Plugin...");


	spdlog::info("Plugin Initialized");
	return true;
}


void obs_module_unload()
{
	
}