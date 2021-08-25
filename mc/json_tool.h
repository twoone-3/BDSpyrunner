#pragma once
#include <iostream>
#include "json.hpp"

using Json = nlohmann::basic_json<>;
using JsonType = nlohmann::detail::value_t;
//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库3.10.0版本
inline Json StringToJson(std::string_view str) {
	try {
		return Json::parse(str);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return nullptr;
	}
}
