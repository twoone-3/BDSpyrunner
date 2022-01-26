#include "Tool.h"
#include <LoggerAPI.h>

extern Logger logger;

//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库
fifo_json ToJson(std::string_view str) {
	try { return fifo_json::parse(str); }
	catch (const std::exception& e) {
		logger.error("Parsing JSON failed! {}", e.what());
		return nullptr;
	}
}

bool IsPlayer(Actor* ptr) {
	if (ptr == nullptr)
		return false;
	if (ptr->getEntityTypeId() != 319)
		return false;
	return true;
}
