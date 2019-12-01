#pragma once

#include <obs-internal.h>
#include <util/platform.h>
#include <string>
#include <regex>
#include <utility>
#include <spdlog/spdlog.h>

#define N_TEXT "text"
#define N_FONT "font"

#define N_FILL_COLOR "fill_color"
#define N_FILL_OPACITY "fill_opacity"

#define N_OUTLINE "outline"
#define N_OUTLINE_GROUP "outline_group"
#define N_OUTLINE_COLOR "outline_color"
#define N_OUTLINE_OPACITY "outline_opacity"
#define N_OUTLINE_THICKNESS "outline_thickness"



#define N_SHADOW "shadow"
#define N_SHADOW_GROUP "shadow_group"

#define N_SHADOW_ROTATION "shadow_rotation"
#define N_SHADOW_SIZE "shadow_size"
#define N_SHADOW_COLOR "shadow_color"
#define N_SHADOW_OPACITY "shadow_opacity"
#define N_SHADOW_DISTANCE "shadow_distance"


#define N_STYLE "style"

#define N_VARIABLES_BTN "variables"

static const char* ToNarrow(const wchar_t* wcs)
{
	size_t len = wcslen(wcs);
	char* text = new char[len];
	os_wcs_to_utf8(wcs, 0, text, len);

	return text;
}

static const wchar_t* ToWide(const char* utf8)
{
	size_t len = strlen(utf8);
	wchar_t* text = new wchar_t[len];
	os_utf8_to_wcs(utf8, 0, text, len);

	return text;
}


template <typename T1, typename T2>
static void Clamp(T1* num, const T2& min, const T2& max)
{
	assert(!(max < min));
	*num = (*num < min) ? min : (max < *num) ? max : *num;
}

template <typename T1, typename T2>
static T1 Clamp(const T1& num, const T2& min, const T2& max)
{
	assert(!(max < min));
	return (num < min) ? min : (max < num) ? max : num;
}

static Gdiplus::Color GetColorValue(uint32_t color, uint32_t opacity)
{
	uint32_t _color = (color & 0xFF) << 16 | color & 0xFF00 | (color & 0xFF0000) >> 16;
	auto rgba = _color & 0xFFFFFF | (opacity * 255 / 100 & 0xFF) << 24;
	Gdiplus::Color c(rgba);
	return c;
}

using text_function = std::wstring(*)(void* data);
struct text_method
{
	std::wstring syntax;
	text_function func;

	text_method(std::wstring _syntax, const text_function& _func) : syntax(std::move(_syntax)), func(_func) {}
	text_method(const char* const _syntax, const text_function& _func) : syntax(ToWide(_syntax)), func(_func) {}

	std::wregex get_regex() const
	{
		std::wstring reg = L"\\$\\{(" + syntax + L")\\}";
		return std::wregex(reg, std::regex_constants::extended);
	}
};
