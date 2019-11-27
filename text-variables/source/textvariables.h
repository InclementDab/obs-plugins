#pragma once

#include "textvariables_global.h"
#include <obs.h>
#include <obs-module.h>
#include "obs-internal.h"
#include <util/threading.h>
#include <util/util.hpp>
#include <util/platform.h>
#include <iostream>
#include <string>
#include <memory>
#include <windows.h>
#include <gdiplus.h>
#include "OBSTextMethodEditor.h"

#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QTimer>
#include <UI/obs-frontend-api/obs-frontend-api.h>
#include <UI/qt-display.hpp>
#include <UI/qt-wrappers.hpp>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("text-variables", "en-US")
MODULE_EXPORT const char inline* obs_module_description()
{
	return "OBS Custom Text Editor"; // 
}

static ULONG_PTR gdip_token = 0;
VariableEditorUI* variable_editor;

void get_defaults(obs_data_t* settings);
obs_properties_t* get_properties(void* data);
bool hint_clicked(obs_properties_t* props, obs_property_t* property, void* data);

template<typename T, typename T2, BOOL WINAPI deleter(T2)>
class GDIObj
{
	T obj = nullptr;

	GDIObj& Replace(T obj_)
	{
		if (obj)
			deleter(obj);
		obj = obj_;
		return *this;
	}

public:

	GDIObj() = default;
	explicit GDIObj(T obj_) : obj(obj_) {}
	~GDIObj() { deleter(obj); }

	T operator=(T obj_)
	{
		Replace(obj_);
		return obj;
	}

	explicit operator T() const { return obj; }

	bool operator==(T obj_) const { return obj == obj_; }
	bool operator!=(T obj_) const { return obj != obj_; }
};

using HDCObj = GDIObj<HDC, HDC, DeleteDC>;
using HFONTObj = GDIObj<HFONT, HGDIOBJ, DeleteObject>;
using HBITMAPObj = GDIObj<HBITMAP, HGDIOBJ, DeleteObject>;

enum class Align
{
	Left,
	Center,
	Right,
};

enum class VAlign
{
	Top,
	Center,
	Bottom,
};

struct TextSource
{
	obs_source_t* source = nullptr;

	gs_texture_t* tex = nullptr;
	uint32_t cx = 0;
	uint32_t cy = 0;

	HDC hdc;
	Gdiplus::Graphics graphics;
	std::unique_ptr<Gdiplus::Font> font;

	bool read_from_file = false;
	std::string file;
	time_t file_timestamp = 0;
	bool update_file = false;
	float update_time_elapsed = 0.0f;

	std::wstring text;
	std::wstring face;
	int face_size = 0;
	uint32_t color = 0xFFFFFF;
	uint32_t color2 = 0xFFFFFF;
	float gradient_dir = 0;
	uint32_t opacity = 100;
	uint32_t opacity2 = 100;
	uint32_t bk_color = 0;
	uint32_t bk_opacity = 0;
	Align align = Align::Left;
	VAlign valign = VAlign::Top;
	bool gradient = false;
	bool bold = false;
	bool italic = false;
	bool underline = false;
	bool strikeout = false;
	bool vertical = false;

	bool use_outline = false;
	float outline_size = 0.0f;
	uint32_t outline_color = 0;
	uint32_t outline_opacity = 100;

	bool use_extents = false;
	bool wrap = false;
	uint32_t extents_cx = 0;
	uint32_t extents_cy = 0;

	int text_transform = S_TRANSFORM_NONE;

	bool chatlog_mode = false;
	int chatlog_lines = 6;

	TextSource(obs_source_t* source_, obs_data_t* settings)
		: source(source_),
		hdc(CreateCompatibleDC(nullptr)),
		graphics(hdc)
	{
		obs_source_update(source, settings);
	}

	~TextSource()
	{
		if (tex) {
			obs_enter_graphics();
			gs_texture_destroy(tex);
			obs_leave_graphics();
		}
	}

	void UpdateFont();
	void GetStringFormat(Gdiplus::StringFormat& format);
	void RemoveNewlinePadding(const Gdiplus::StringFormat& format, Gdiplus::RectF& box);
	void CalculateTextSizes(const Gdiplus::StringFormat& format, Gdiplus::RectF& bounding_box,
							SIZE& text_size);
	void RenderOutlineText(Gdiplus::Graphics& graphics, const Gdiplus::GraphicsPath& path,
						   const Gdiplus::Brush& brush);
	void RenderText();
	void LoadFileText();
	void TransformText();
	void ReplaceVariables();

	const char* GetMainString(const char* str);

	void Update(obs_data_t* settings);
	void Tick(float seconds);
	void Render();
};

static void clamp(long val, const long& min_val, const long& max_val);
static std::wstring to_wide(const char* utf8);
static uint32_t rgb_to_bgr(uint32_t rgb);
static time_t get_modified_timestamp(const char* filename);
static DWORD get_alpha_val(uint32_t opacity);
static DWORD calc_color(uint32_t color, uint32_t opacity);

#define set_vis(var, val, show)                           \
	do {                                              \
		p = obs_properties_get(props, val);       \
		obs_property_set_visible(p, var == show); \
	} while (false)

static bool use_file_changed(obs_properties_t* props, obs_property_t* p,
							 obs_data_t* s)
{
	bool use_file = obs_data_get_bool(s, S_USE_FILE);

	set_vis(use_file, S_TEXT, false);
	set_vis(use_file, S_FILE, true);
	return true;
}

static bool outline_changed(obs_properties_t* props, obs_property_t* p,
							obs_data_t* s)
{
	bool outline = obs_data_get_bool(s, S_OUTLINE);

	set_vis(outline, S_OUTLINE_SIZE, true);
	set_vis(outline, S_OUTLINE_COLOR, true);
	set_vis(outline, S_OUTLINE_OPACITY, true);
	return true;
}

static bool chatlog_mode_changed(obs_properties_t* props, obs_property_t* p,
								 obs_data_t* s)
{
	bool chatlog_mode = obs_data_get_bool(s, S_CHATLOG_MODE);

	set_vis(chatlog_mode, S_CHATLOG_LINES, true);
	return true;
}

static bool gradient_changed(obs_properties_t* props, obs_property_t* p,
							 obs_data_t* s)
{
	bool gradient = obs_data_get_bool(s, S_GRADIENT);

	set_vis(gradient, S_GRADIENT_COLOR, true);
	set_vis(gradient, S_GRADIENT_OPACITY, true);
	set_vis(gradient, S_GRADIENT_DIR, true);
	return true;
}

static bool extents_modified(obs_properties_t* props, obs_property_t* p,
							 obs_data_t* s)
{
	bool use_extents = obs_data_get_bool(s, S_EXTENTS);

	set_vis(use_extents, S_EXTENTS_WRAP, true);
	set_vis(use_extents, S_EXTENTS_CX, true);
	set_vis(use_extents, S_EXTENTS_CY, true);
	return true;
}

#undef set_vis



inline void QtExecSync(void (*func)(void*), void* const data)
{
	if (QThread::currentThread() == qApp->thread()) {
		func(data);
	}
	else {
		os_event_t* completeEvent;

		os_event_init(&completeEvent, OS_EVENT_TYPE_AUTO);

		QTimer* t = new QTimer();
		t->moveToThread(qApp->thread());
		t->setSingleShot(true);
		QObject::connect(t, &QTimer::timeout, [=]() {
			t->deleteLater();

			func(data);

			os_event_signal(completeEvent);
		});
		QMetaObject::invokeMethod(t, "start", Qt::QueuedConnection,
								  Q_ARG(int, 0));

		QApplication::sendPostedEvents();

		os_event_wait(completeEvent);
		os_event_destroy(completeEvent);
	}
}

inline void QtExecSync(std::function<void()> task)
{
	struct local_context
	{
		std::function<void()> task;
	};

	auto context = new local_context();
	context->task = task;

	QtExecSync([](void* data) {
		local_context* context = (local_context*)data;
		context->task();
		delete context;
	}, context);
}


inline void add_module_ui()
{

	obs_frontend_push_ui_translation(obs_module_get_string);
	QAction* action = (QAction*)obs_frontend_add_tools_menu_qaction(obs_module_text("TextVariables"));
	QMainWindow* window = (QMainWindow*)obs_frontend_get_main_window();
	variable_editor = new VariableEditorUI(window);
	obs_frontend_pop_ui_translation();

	auto cb = []() { variable_editor->ShowHideDialog(); };

	action->connect(action, &QAction::triggered, cb);
}


inline bool obs_module_load()
{

	obs_source_info si = {};
	si.id = "text_variables";
	si.type = OBS_SOURCE_TYPE_INPUT;
	si.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
	si.get_properties = get_properties;
	si.get_defaults = get_defaults;

	si.get_name = [](void*) { return obs_module_text("PluginName"); };
	si.create = [](obs_data_t* settings, obs_source_t* source) {
		return static_cast<void*>(new TextSource(source, settings));
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
	GdiplusStartup(&gdip_token, &gdip_input, nullptr);

	
	// todo run add-module-ui in this callback
	obs_frontend_add_event_callback(obs_frontend_event::OBS_FRONTEND_EVENT_FINISHED_LOADING, nullptr);
	std::function<void()> mod_ui = add_module_ui;
	QtExecSync(mod_ui);

	//QEvent* loaded_event = new QEvent(QEvent::Type::ApplicationActivated);
	//
	//QCoreApplication::notify(QCoreApplication::instance(), loaded_event);

	return true;
}

inline void obs_module_unload()
{
	blog(LOG_INFO, "Unloading {}", obs_module_text("PluginName"));
	Gdiplus::GdiplusShutdown(gdip_token);
}

