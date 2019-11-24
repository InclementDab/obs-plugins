#pragma once

using text_function = std::wstring (*)(void* data);
struct text_method
{
	std::wstring syntax;
	text_function func;

	std::wregex get_regex()
	{
		std::wstringstream strstream;
		strstream << L"\\$\\{(" << syntax << L")\\}";
		return std::wregex(strstream.str(), std::regex_constants::extended);
	}
};

inline static std::wstring _SubCount(void* data)
{
	return std::wstring(L"6969");
}

inline static std::wstring _SubGoal(void* data)
{
	return std::wstring(L"1000");
}

static text_method text_method_list[] = {
	{ std::wstring(L"SubCount"), _SubCount },
    { std::wstring(L"SubGoal"), _SubGoal }
};






