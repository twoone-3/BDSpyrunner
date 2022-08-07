#include "Global.hpp"

#include <LoggerAPI.h>

Logger logger("BDSpyrunner");

std::unordered_map<enum class EventCode, std::vector<py::function>> listeners;
std::unordered_map<string, py::object> player_data;
