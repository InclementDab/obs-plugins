#pragma once

#include <obs.h>
#include <obs-module.h>
#include <UI/obs-frontend-api/obs-frontend-api.h>

#include "obs-simple-counter.h"
#include "obs-scorecard.h"
#include <curl/curl.h>
#include <QWidget>
#include <QMainWindow>
#include <json11.hpp>
#include <spdlog/spdlog.h>
//#include "auth/auth-base.hpp"

using namespace json11;

MODULE_EXPORT class OBSUserVariablesPlugin
{
public:
	//curlpp::options::Url* twitch_url;
	const char* twitch_key;


	static void InitBrowserPanelSafeBlock();

	
	static OBSUserVariablesPlugin* Get();
	static QMainWindow* GetParent();
	static obs_service_t* GetService();
	static std::string GetVersionString();

	//std::shared_ptr<Auth> auth;
	
	config_t* Config() const;

};

static OBSUserVariablesPlugin* plugin_obj;

MODULE_EXTERN void set_plugin(OBSUserVariablesPlugin* plg);

MODULE_EXTERN void on_frontend_event(obs_frontend_event event_type, void* data);
MODULE_EXTERN void tool_menu_item_clicked(void* data);



 
