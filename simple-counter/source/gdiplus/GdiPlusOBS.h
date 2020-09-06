#pragma once
#include <obs.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include "common.h"
#include <spdlog/spdlog.h>
#include <string>

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
protected:
	HDC text_hdc = CreateCompatibleDC(nullptr);
	gs_texture_t* text_texture = nullptr;
		
public:
	Gdiplus::Font* text_font = new Gdiplus::Font(text_hdc);
	
	static void CheckStatus(Gdiplus::Status status, const char* const source = "Generic")
	{
		if (status) {
			spdlog::error("Error in {}: {}", source, status);
			OutputDebugString((LPCWSTR)L"Error In CheckStatus");
			OutputDebugString((LPCWSTR)status);
		}
	}

	Gdiplus::Size UpdateTextTexture(std::wstring& wtext, obs_data_t* update_data, const Alignment& alignment);
};