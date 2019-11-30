#pragma once
#include <obs.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include "source/common.h"
#include <memory>

#include <spdlog/spdlog.h>

#define MIN_SIZE 2
#define MAX_SIZE 16384
#define MAX_AREA (4096LL * 4096LL)

struct Alignment
{
	enum struct H
	{
		Left,
		Middle,
		Right,
	} HAlign;

	enum struct V
	{
		Top,
		Center,
		Bottom,
	} VAlign;

	Alignment() : HAlign(H::Left), VAlign(V::Top) {}
	Alignment(H h, V v) : HAlign(h), VAlign(v) {}
};



struct GdiPlusOBS
{
	HDC text_hdc = CreateCompatibleDC(nullptr);	
	gs_texture_t* GetTextTexture(std::wstring& wtext, obs_data_t* update_data, Gdiplus::Size* text_size, const Alignment& alignment);
};