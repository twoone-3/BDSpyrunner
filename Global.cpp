#include "Global.hpp"

//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库
fifo_json StrToJson(std::string_view str) {
	try {
		return fifo_json::parse(str);
	}
	catch (const std::exception& e) {
		logger.error("\nParsing JSON failed! {}", e.what());
		return nullptr;
	}
}
//全局变量
bool isCmdRegisterEnabled = false;

Logger logger("BDSpyrunner");

std::unordered_map<enum class EventCode, std::vector<py::function>> listeners;
std::unordered_map<string, py::object> player_data;