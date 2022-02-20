﻿#pragma once
#include "PyUtils.h"
#include "Event.h"

//Py函数表
inline std::unordered_map<EventCode, std::vector<py::function>> g_cb_functions;
//注册命令
inline std::unordered_map<std::string, std::pair<std::string, py::function>> g_commands;
