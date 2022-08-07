#pragma once
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#define PY_TRY try {
#define PY_CATCH                                                    \
  }                                                                 \
  catch (const std::exception& e) {                                 \
    logger.error("\n{}", e.what());                                 \
  }                                                                 \
  catch (...) {                                                     \
    logger.fatal("An unknown exception was occurred! Function: {}", \
                 __FUNCSIG__);                                      \
  }
#define PLUGIN_PATH "plugins\\py\\"

#define EVENT_BEGIN(code) \
  Callbacker h(code);     \
  PY_TRY;                 \
  h.insert("Event", py::cast(magic_enum::enum_name(code)))
#define EVENT_INSERT(key) h.insert(#key, ev.m##key)
#define EVENT_INSERT_EX(key, value) h.insert(#key, (value))
#define EVENT_END PY_CATCH return h.callback()

namespace py = pybind11;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

enum GameType {
  Survival = 0,
  Creative = 1,
  Adventure = 2,
  Spectator = 6,
};
enum class EventCode;
class Logger;

extern Logger logger;
extern unordered_map<EventCode, vector<py::function>> listeners;
extern unordered_map<string, py::object> player_data;
