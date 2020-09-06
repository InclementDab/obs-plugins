#include "common.h"
#include "auth-twitch.hpp"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "remote-text.hpp"
#include <qt-wrappers.hpp>

#include <json11.hpp>

#include "ui-config.h"
#include "UI/obf.h"
#include <obs-module.h>
#include "pluginmain.hpp"
#include <util/config-file.h>

using namespace json11;

/* ------------------------------------------------------------------------- */

#define TWITCH_AUTH_URL "https://obsproject.com/app-auth/twitch?action=redirect"
#define TWITCH_TOKEN_URL "https://obsproject.com/app-auth/twitch-token"
#define ACCEPT_HEADER "Accept: application/vnd.twitchtv.v5+json"

#define TWITCH_SCOPE_VERSION 1

static Auth::Def twitchDef = {"Twitch", Auth::Type::OAuth_StreamKey};

/* ------------------------------------------------------------------------- */

QCef* cef = nullptr;
QCefCookieManager* panel_cookies = nullptr;

TwitchAuth::TwitchAuth(const Def &d) : OAuthStreamKey(d)
{
	if (!cef)
		return;

	cef->add_popup_whitelist_url(
		"https://twitch.tv/popout/frankerfacez/chat?ffz-settings",
		this);
	uiLoadTimer.setSingleShot(true);
	uiLoadTimer.setInterval(500);
	connect(&uiLoadTimer, &QTimer::timeout, this,
		&TwitchAuth::TryLoadSecondaryUIPanes);
}

bool TwitchAuth::GetChannelInfo()
try {
	std::string client_id = TWITCH_CLIENTID;
	deobfuscate_str(&client_id[0], TWITCH_HASH);

	if (!GetToken(TWITCH_TOKEN_URL, client_id, TWITCH_SCOPE_VERSION))
		return false;
	if (token.empty())
		return false;
	if (!key_.empty())
		return true;

	std::string auth;
	auth += "Authorization: OAuth ";
	auth += token;

	std::vector<std::string> headers;
	headers.push_back(std::string("Client-ID: ") + client_id);
	headers.push_back(ACCEPT_HEADER);
	headers.push_back(std::move(auth));

	std::string output;
	std::string error;
	long error_code = 0;

	bool success = false;

	auto func = [&]() {
		success = GetRemoteFile("https://api.twitch.tv/kraken/channel",
					output, error, &error_code,
					"application/json", nullptr, headers,
					nullptr, 5);
	};

	ExecThreadedWithoutBlocking(
		func, QTStr("Auth.LoadingChannel.Title"),
		QTStr("Auth.LoadingChannel.Text").arg(service()));
	if (error_code == 403) {
		OBSMessageBox::warning(OBSUserVariablesPlugin::GetParent(),
				       Str("TwitchAuth.TwoFactorFail.Title"),
				       Str("TwitchAuth.TwoFactorFail.Text"),
				       true);
		blog(LOG_WARNING, "%s: %s", __FUNCTION__,
		     "Got 403 from Twitch, user probably does not "
		     "have two-factor authentication enabled on "
		     "their account");
		return false;
	}

	if (!success || output.empty())
		throw ErrorInfo("Failed to get text from remote", error);

	Json json = Json::parse(output, error);
	if (!error.empty())
		throw ErrorInfo("Failed to parse json", error);

	error = json["error"].string_value();
	if (!error.empty()) {
		if (error == "Unauthorized") {
			if (RetryLogin()) {
				return GetChannelInfo();
			}
			throw ErrorInfo(error, json["message"].string_value());
		}
		throw ErrorInfo(error,
				json["error_description"].string_value());
	}

	name = json["name"].string_value();
	key_ = json["stream_key"].string_value();

	return true;
} catch (ErrorInfo info) {
	QString title = QTStr("Auth.ChannelFailure.Title");
	QString text = QTStr("Auth.ChannelFailure.Text")
			       .arg(service(), info.message.c_str(),
				    info.error.c_str());

	QMessageBox::warning(OBSUserVariablesPlugin::GetParent(), title, text);

	blog(LOG_WARNING, "%s: %s: %s", __FUNCTION__, info.message.c_str(),
	     info.error.c_str());
	return false;
}

void TwitchAuth::SaveInternal()
{
	OBSUserVariablesPlugin* main = OBSUserVariablesPlugin::Get();
	config_set_string(main->Config(), service(), "Name", name.c_str());
	if (uiLoaded) {
		//config_set_string(main->Config(), service(), "DockState",
		//		  main->saveState().toBase64().constData());
	}
	OAuthStreamKey::SaveInternal();
}

static inline std::string get_config_str(OBSUserVariablesPlugin* main, const char *section,
					 const char *name)
{
	const char *val = config_get_string(main->Config(), section, name);
	return val ? val : "";
}

bool TwitchAuth::LoadInternal()
{
	if (!cef)
		return false;

	OBSUserVariablesPlugin* main = OBSUserVariablesPlugin::Get();
	name = get_config_str(main, service(), "Name");
	firstLoad = false;
	return OAuthStreamKey::LoadInternal();
}

bool TwitchAuth::RetryLogin()
{
	OAuthLogin login(OBSUserVariablesPlugin::GetParent(), TWITCH_AUTH_URL, false);
	if (login.exec() == QDialog::Rejected) {
		return false;
	}

	std::shared_ptr<TwitchAuth> auth =
		std::make_shared<TwitchAuth>(twitchDef);
	std::string client_id = TWITCH_CLIENTID;
	deobfuscate_str(&client_id[0], TWITCH_HASH);

	return GetToken(TWITCH_TOKEN_URL, client_id, TWITCH_SCOPE_VERSION,
			QT_TO_UTF8(login.GetCode()), true);
}

std::shared_ptr<Auth> TwitchAuth::Login(QWidget *parent)
{
	OAuthLogin login(parent, TWITCH_AUTH_URL, false);
	if (login.exec() == QDialog::Rejected) {
		return nullptr;
	}

	std::shared_ptr<TwitchAuth> auth =
		std::make_shared<TwitchAuth>(twitchDef);

	std::string client_id = TWITCH_CLIENTID;
	deobfuscate_str(&client_id[0], TWITCH_HASH);

	if (!auth->GetToken(TWITCH_TOKEN_URL, client_id, TWITCH_SCOPE_VERSION,
			    QT_TO_UTF8(login.GetCode()))) {
		return nullptr;
	}

	std::string error;
	if (auth->GetChannelInfo()) {
		return auth;
	}

	return nullptr;
}

static std::shared_ptr<Auth> CreateTwitchAuth()
{
	return std::make_shared<TwitchAuth>(twitchDef);
}

static void DeleteCookies()
{
	if (panel_cookies)
		panel_cookies->DeleteCookies("twitch.tv", std::string());
}

void RegisterTwitchAuth()
{
	OAuth::RegisterOAuth(twitchDef, CreateTwitchAuth, TwitchAuth::Login,
			     DeleteCookies);
}
