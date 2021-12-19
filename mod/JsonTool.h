#pragma once
#include <iostream>
#include "json.hpp"

using json = nlohmann::basic_json<>;
using json_t = nlohmann::detail::value_t;
//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库3.10.4版本
inline json StringToJson(std::string_view str) {
	try { return json::parse(str); }
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return nullptr;
	}
}
