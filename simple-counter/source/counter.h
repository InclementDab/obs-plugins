#pragma once

#include <obs.h>
#include <spdlog/spdlog.h>

struct Counter
{
public:
	obs_source_t* source;
	obs_data_t* settings;

public:
	int value = 0; // todo template this?
	const char* name;
	const char* id;

	obs_hotkey_id increment_hotkey, decrement_hotkey;

	Counter(obs_source_t* _source, obs_data_t* _settings, const char* const _id, const char* const _name) : source(_source), settings(_settings), name(_name), id(_id)
	{
		spdlog::info("Creating Counter {}", name);

		auto increment_func = [](void* data, obs_hotkey_id id, obs_hotkey_t* hotkey, bool pressed)
		{
			Counter* c = reinterpret_cast<Counter*>(data);
			if (pressed) c->value++;
			obs_source_update(c->source, c->settings);
		};
		auto decrement_func = [](void* data, obs_hotkey_id id, obs_hotkey_t* hotkey, bool pressed)
		{
			Counter* c = reinterpret_cast<Counter*>(data);
			if (pressed) c->value--;
			obs_source_update(c->source, c->settings);
		};

		char buff[100];
		snprintf(buff, sizeof buff, "Increment %s", name);
		increment_hotkey = obs_hotkey_register_source(source, id, buff, increment_func, this);
		snprintf(buff, sizeof buff, "Decrement %s", name);
		decrement_hotkey = obs_hotkey_register_source(source, id, buff, decrement_func, this);
	}

	~Counter()
	{
		obs_hotkey_unregister(increment_hotkey);
		obs_hotkey_unregister(decrement_hotkey);
	}

	explicit operator text_method() const
	{
		return text_method(name, [](void* data)
		{
			return std::to_wstring(reinterpret_cast<Counter*>(data)->value);
		});
	}

	text_method get_text_method() const
	{
		return text_method(name, [](void* data) {
			return std::to_wstring(reinterpret_cast<Counter*>(data)->value);
		});
	}

	std::wstring text_replace(std::wstring wtext)
	{
		std::wstring syntax = ToWide(name);
		std::wstring regex = L"\\$\\{(" + syntax + L")\\}";
		return std::regex_replace(wtext, std::wregex(regex), std::to_wstring(value));
	}

	explicit operator int() const
	{
		return value;
	}
	
	bool operator==(const Counter& compare) const
	{
		return  (value == compare.value) &&
				(name == compare.name) &&
				(id == compare.id);
	}

	Counter& operator=(const Counter&) = default;
	Counter& operator=(Counter&&) = default;
};
