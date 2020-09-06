#pragma once

#include <QObject>
#include <QMetaObject>
#include <QDialog>
#include <QTimer>
#include <functional>
#include <memory>
#include <string>



class Auth : public QObject
{
	Q_OBJECT

protected:
	virtual void SaveInternal() = 0;
	virtual bool LoadInternal() = 0;

	bool firstLoad = true;

	struct ErrorInfo
	{
		std::string message;
		std::string error;

		ErrorInfo(std::string message_, std::string error_)
			: message(message_), error(error_)
		{}
	};

public:
	enum class Type
	{
		None,
		OAuth_StreamKey,
	};

	struct Def
	{
		std::string service;
		Type type;
	};

	typedef std::function<std::shared_ptr<Auth>()> create_cb;

	inline Auth(const Def& d) : def(d) {}
	virtual ~Auth() {}

	inline Type type() const { return def.type; }
	inline const char* service() const { return def.service.c_str(); }

	virtual void LoadUI() {}

	virtual void OnStreamConfig() {}

	static std::shared_ptr<Auth> Create(const std::string& service);
	static Type AuthType(const std::string& service);
	static void Load();
	static void Save();

protected:
	static void RegisterAuth(const Def& d, create_cb create);

private:
	Def def;
};


class QCefWidget;

class OAuthLogin : public QDialog
{
	Q_OBJECT

		QCefWidget* cefWidget = nullptr;
	QString code;
	bool get_token = false;
	bool fail = false;

public:
	OAuthLogin(QWidget* parent, const std::string& url, bool token);
	~OAuthLogin();

	inline QString GetCode() const { return code; }
	inline bool LoadFail() const { return fail; }

	virtual int exec() override;

public slots:
	void urlChanged(const QString& url);
};

class OAuth : public Auth
{
	Q_OBJECT

public:
	inline OAuth(const Def& d) : Auth(d) {}

	typedef std::function<std::shared_ptr<Auth>(QWidget*)> login_cb;
	typedef std::function<void()> delete_cookies_cb;

	static std::shared_ptr<Auth> Login(QWidget* parent,
									   const std::string& service);
	static void DeleteCookies(const std::string& service);

	static void RegisterOAuth(const Def& d, create_cb create,
							  login_cb login,
							  delete_cookies_cb delete_cookies);

protected:
	std::string refresh_token;
	std::string token;
	bool implicit = false;
	uint64_t expire_time = 0;
	int currentScopeVer = 0;

	virtual void SaveInternal() override;
	virtual bool LoadInternal() override;

	virtual bool RetryLogin() = 0;
	bool TokenExpired();
	bool GetToken(const char* url, const std::string& client_id,
				  int scope_ver,
				  const std::string& auth_code = std::string(),
				  bool retry = false);
};

class OAuthStreamKey : public OAuth
{
	Q_OBJECT

protected:
	std::string key_;

public:
	inline OAuthStreamKey(const Def& d) : OAuth(d) {}

	inline const std::string& key() const { return key_; }

	virtual void OnStreamConfig() override;
};


class BrowserDock;

class TwitchAuth : public OAuthStreamKey
{
	Q_OBJECT

		friend class TwitchLogin;

	QSharedPointer<BrowserDock> chat;
	QSharedPointer<BrowserDock> info;
	QSharedPointer<BrowserDock> stat;
	QSharedPointer<BrowserDock> feed;
	QSharedPointer<QAction> chatMenu;
	QSharedPointer<QAction> infoMenu;
	QSharedPointer<QAction> statMenu;
	QSharedPointer<QAction> feedMenu;
	bool uiLoaded = false;

	std::string name;

	virtual bool RetryLogin() override;

	virtual void SaveInternal() override;
	virtual bool LoadInternal() override;

	bool GetChannelInfo();

	virtual void LoadUI() override;

public:
	TwitchAuth(const Def& d);

	static std::shared_ptr<Auth> Login(QWidget* parent);

	QTimer uiLoadTimer;

public slots:
	void TryLoadSecondaryUIPanes();
	void LoadSecondaryUIPanes();
};