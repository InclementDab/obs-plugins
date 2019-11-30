
#include <obs-module.h>
#include "obs-simple-counter.h"
#include "obs-scorecard.h"

#include <spdlog/spdlog.h>


// todo Instead of having a Scorecard class, just have 'default setups' in the Counter class. which allows the user to select one of few default variable setups

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
	
	obs_register_source(&OBS_Scorecard_Info());
	obs_register_source(&OBS_SimpleCounter_Info());
	
	spdlog::info("Plugin Initialized");

	return true;
}

void obs_module_unload()
{
	spdlog::warn("Unloading Plugin...");
}