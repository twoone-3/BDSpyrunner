#pragma once
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "global.h"

#define PY_TRY try {
#define PY_CATCH                    \
  }                                 \
  catch (const std::exception& e) { \
    logger.error("\n{}", e.what()); \
  }
namespace py = pybind11;

inline unordered_map<EventCode, vector<py::function>> listeners;
inline unordered_map<string, py::object> player_data;
