#pragma once
#include "PyEntity.h"
#include "PyItemStack.h"
#include "PyBlockInstance.h"
#include "Event.h"

//Py函数表
inline std::unordered_map<EventCode, std::vector<PyObject*>> g_callback_functions;
//注册命令
inline std::unordered_map<std::string, std::pair<std::string, PyObject*>> g_commands;

extern "C" PyObject * McInit();