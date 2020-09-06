#include "pluginmain.hpp"

#include <QMainWindow>
//#include <window-user-variables-editor.h>
//#include "auth/auth-twitch.hpp"

OBS_DECLARE_MODULE()

bool obs_module_load()
{
#ifdef DEBUG
	AllocConsole();
	spdlog::set_level(spdlog::level::debug);
#elif RELEASE
	spdlog::set_level(spdlog::level::warn);
#endif
	spdlog::info("Loading Modules...");


	// Plugin not loading?
	// Do you require the 'this' keyword in a context called directly by this?
	obs_register_source(&OBS_Scorecard_Info());
	obs_register_source(&OBS_SimpleCounter_Info());


	spdlog::info("Modules Loaded");
	return true;
}

void obs_module_post_load()
{
	spdlog::info("Initializing Plugin...");


	static OBSUserVariablesPlugin* plg = new OBSUserVariablesPlugin();
	spdlog::warn("plugin");

	//counter_plugin::plugin::initialize();
	//plg->twitch_url = new curlpp::options::Url(obs_service_get_url(service));

	obs_frontend_add_event_callback(on_frontend_event, plg);
	obs_frontend_add_tools_menu_item("User Variables", tool_menu_item_clicked, plg);
	set_plugin(plg);

	spdlog::info("Plugin Initialized");
}

void obs_module_unload()
{
	spdlog::warn("Unloading Plugin...");

	delete plugin_obj;
}


void on_frontend_event(obs_frontend_event event_type, void* data)
{
	auto plugin_data = static_cast<OBSUserVariablesPlugin*>(data);


	if (event_type == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
		spdlog::info("OBS Loading Complete!");


	}
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	auto count = size * nmemb;
	static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
	return size * nmemb;
}

#define TWITCH_AUTH_URL "https://obsproject.com/app-auth/twitch?action=redirect"
#define TWITCH_TOKEN_URL "https://obsproject.com/app-auth/twitch-token"
#define ACCEPT_HEADER "Accept: application/vnd.twitchtv.v5+json"

#define TWITCH_SCOPE_VERSION 1

static class TwitchProvider
{
public:
	TwitchProvider()
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	~TwitchProvider()
	{
		curl_global_cleanup();
	}


	const std::string client_id                 = "gyh8jdvtfe5k6qdft9co39ym194bnn";
	const std::string client_secret             = "6y42yke0hj6i69slzzjbav8sbzk24w";
	const std::string redirect_uri              = "http://localhost:8080";
	const std::vector<std::string> client_scope = {
		"user:edit",
		"user:edit:broadcast",
		"channel:read:subscriptions"
	};

	const std::string auth_uri  = "https://id.twitch.tv/oauth2/authorize/";
	const std::string token_uri = "https://id.twitch.tv/oauth2/token";
	const std::string user_uri  = "https://api.twitch.tv/helix/users/";

	struct token_response
	{
		long expires_in;
		std::string access_token;
		std::string token_type;

		token_response(Json in)
		{
			expires_in   = in["expires_in"].int_value();
			access_token = in["access_token"].string_value();
			token_type   = in["token_type"].string_value();
		}

		bool empty() const
		{
			return access_token.empty();
		}
	};


	token_response GetAccessToken() const
	{
		std::string post_uri = token_uri;

		post_uri += "?client_id=" + client_id;
		post_uri += "&client_secret=" + client_secret;
		post_uri += "&grant_type=client_credentials";

		std::string token;

		CURL* curl    = curl_easy_init();
		CURLcode code = CURLE_FAILED_INIT;
		std::string read_buffer;

		spdlog::info(post_uri);
		curl_easy_setopt(curl, CURLOPT_URL, post_uri.c_str());
		curl_easy_setopt(curl, CURLOPT_POST);
		curl_easy_setopt(curl, CURLOPT_VERBOSE);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

		code = curl_easy_perform(curl);
		spdlog::info("AUTH RESULT: {}", read_buffer);

		if (code) spdlog::error("AUTH {}", curl_easy_strerror(code));
		curl_easy_cleanup(curl);
		


		std::string err;
		token_response response(Json::parse(read_buffer,err));

		if (!err.empty())
			spdlog::error(err);


		return response;
	}
} TwitchProvider;

void tool_menu_item_clicked(void* data)
{
	auto token = TwitchProvider.GetAccessToken();

	//QMainWindow* window = (QMainWindow*)obs_frontend_get_main_window();
	//OAuthLogin login_page(window, TWITCH_AUTH_URL, false);
	//
	//if (login_page.exec() == QDialog::Rejected) {
	//	spdlog::warn("Login Rejected!");
	//	return;
	//}

	//auto mwindow = static_cast<QMainWindow*>(obs_frontend_get_main_window());
	//auto veditor = new UserVariablesEditor(mwindow);
	//veditor->show();
	

	auto service = obs_frontend_get_streaming_service();
	if (!service) {
		spdlog::error("Service Failed! User Not Logged In?");
		return;
	}
	// 6 1 6 4
	auto props = obs_get_service_properties(obs_service_get_id(service));
	auto prop  = obs_properties_first(props);
	spdlog::info(obs_property_get_type(prop));
	while (obs_property_next(&prop)) {
		//spdlog::info(obs_property_get_type(prop));
	}

	//std::string token = obs_service_get_key(service);
}

void OBSUserVariablesPlugin::InitBrowserPanelSafeBlock()
{
#ifdef BROWSER_AVAILABLE
	if (!cef)
		return;
	if (cef->init_browser()) {
		InitPanelCookieManager();
		return;
	}

	ExecThreadedWithoutBlocking([] { cef->wait_for_browser_init(); },
								QTStr("BrowserPanelInit.Title"),
								QTStr("BrowserPanelInit.Text"));
	InitPanelCookieManager();
#endif
}



OBSUserVariablesPlugin* OBSUserVariablesPlugin::Get()
{
	return plugin_obj;
}

QMainWindow* OBSUserVariablesPlugin::GetParent()
{
	return static_cast<QMainWindow*>(obs_frontend_get_main_window());
}

config_t* OBSUserVariablesPlugin::Config() const
{
	config_t* config = nullptr;
	//int i = config_open_defaults(config, "user-variables.cfg");
	//if (i) {
	//	spdlog::warn("Config {}", i);
	//	config = config_create("user-variables.cfg");
	//}

	return config;
}

obs_service_t* OBSUserVariablesPlugin::GetService()
{
	return obs_frontend_get_streaming_service();
}

std::string OBSUserVariablesPlugin::GetVersionString()
{
	return "1.0.0.0";
}

void set_plugin(OBSUserVariablesPlugin* plg)
{
	plugin_obj = plg;
}
