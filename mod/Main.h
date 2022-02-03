#pragma once
#include <third-party/Nlohmann/fifo_json.hpp>

using json_t = nlohmann::detail::value_t;

//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库
fifo_json StrToJson(std::string_view str);
