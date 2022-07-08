#pragma once
#pragma execution_character_set("utf-8")
#include <LoggerAPI.h>

#include <third-party/magic_enum/magic_enum.hpp>
#include <third-party/Nlohmann/fifo_json.hpp>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

// clang-format off
#define PY_TRY try {
#define PY_CATCH } catch (const std::exception& e) { logger.error("\n{}", e.what()); } catch (...) { logger.fatal("An unknown exception was occurred! Function: {}", __FUNCSIG__); }
#define PLUGIN_PATH "plugins\\py\\"

#define EVENT_BEGIN(code) Callbacker h(code); PY_TRY; h.insert("Event", py::cast(magic_enum::enum_name(code)))
#define EVENT_INSERT(key) h.insert(#key, ev.m##key)
#define EVENT_INSERT_EX(key, value) h.insert(#key, (value))
#define EVENT_END PY_CATCH return h.callback()
// clang-format on

namespace py = pybind11;
using std::unique_ptr;

enum GameType {
	Survival = 0,
	Creative = 1,
	Adventure = 2,
	Spectator = 6,
};

// 全局变量

extern Logger logger;
extern std::unordered_map<enum class EventCode, std::vector<py::function>> listeners;
extern std::unordered_map<string, py::object> player_data;

// 字符串转JSON
fifo_json StrToJson(std::string_view str);
