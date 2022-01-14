#pragma once
#include <vector>
#include "Entity.h"
#include "Event.h"

//Py函数表
inline std::unordered_map<EventCode, std::vector<PyObject*>> g_callback_functions;
//注册命令
inline std::unordered_map<std::string, std::pair<std::string, PyObject*>> g_commands;
//伤害
inline static int g_damage = 0;

extern "C" PyObject * mc_init();