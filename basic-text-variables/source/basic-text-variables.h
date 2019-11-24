#pragma once

#include "common.h"

#pragma region extrashit
using namespace std;
using namespace Gdiplus;

template<typename T, typename T2, BOOL WINAPI deleter(T2)> class GDIObj
{
	T obj = nullptr;

	inline GDIObj& Replace(T obj_)
	{
		if (obj)
			deleter(obj);
		obj = obj_;
		return *this;
	}

public:
	inline GDIObj() {}
	inline GDIObj(T obj_) : obj(obj_) {}
	inline ~GDIObj() { deleter(obj); }

	inline T operator=(T obj_)
	{
		Replace(obj_);
		return obj;
	}

	inline operator T() const { return obj; }

	inline bool operator==(T obj_) const { return obj == obj_; }
	inline bool operator!=(T obj_) const { return obj != obj_; }
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

	HDCObj hdc;
	Graphics graphics;

	HFONTObj hfont;
	unique_ptr<Font> font;

	bool read_from_file = false;
	string file;
	time_t file_timestamp = 0;
	bool update_file = false;
	float update_time_elapsed = 0.0f;

	wstring text;
	wstring face;
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

	inline TextSource(obs_source_t* source_, obs_data_t* settings)
		: source(source_),
		hdc(CreateCompatibleDC(nullptr)),
		graphics(hdc)
	{
		obs_source_update(source, settings);
	}

	inline ~TextSource()
	{
		if (tex) {
			obs_enter_graphics();
			gs_texture_destroy(tex);
			obs_leave_graphics();
		}
	}

	void UpdateFont();
	void GetStringFormat(StringFormat& format);
	void RemoveNewlinePadding(const StringFormat& format, RectF& box);
	void CalculateTextSizes(const StringFormat& format, RectF& bounding_box,
							SIZE& text_size);
	void RenderOutlineText(Graphics& graphics, const GraphicsPath& path,
						   const Brush& brush);
	void RenderText();
	void LoadFileText();
	void TransformText();
	void ReplaceVariables();

	const char* GetMainString(const char* str);

	void Update(obs_data_t* settings);
	void Tick(float seconds);
	void Render();
};




#pragma endregion

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-text-variables", "en-US")
MODULE_EXPORT const char* obs_module_description()
{
	return "Windows GDI+ text source";
}

static ULONG_PTR gdip_token = 0;

void get_defaults(obs_data_t* settings);
obs_properties_t* get_properties(void* data);
bool hint_clicked(obs_properties_t* props, obs_property_t* property, void* data);




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