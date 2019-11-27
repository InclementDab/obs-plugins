#pragma once

#include <obs-internal.h>

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


template <typename T>
static void Clamp(T* num, const T& min, const T& max)
{
	assert(!(max < min));
	*num = (*num < min) ? min : (max < *num) ? max : *num;
}

template <typename T>
static T Clamp(const T& num, const T& min, const T& max)
{
	assert(!(max < min));
	return (num < min) ? min : (max < num) ? max : num;
}

static bool DeleteCounters(void* data, obs_source_t* source)
{
	if (obs_source_get_id(source) == Counter::source_id) {
		obs_source_remove(source);
	}
	return true;
}
