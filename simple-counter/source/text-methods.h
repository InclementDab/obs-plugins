#pragma once

#include <string>
#include <regex>

using text_function = std::wstring(*)(void* data);
struct text_method
{
	std::wstring syntax;
	text_function func;

	std::wregex get_regex() const
	{
		std::wstring reg = L"\\$\\{(" + syntax + L")\\}";
		return std::wregex(reg, std::regex_constants::extended);
	}
};
