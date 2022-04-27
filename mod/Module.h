#pragma once
#include <unordered_map>
#include <vector>
#include "Entity.h"
#include "Event.h"

//Pyº¯Êı±í
inline std::unordered_map<EventCode, std::vector<PyObject*>> g_callback_functions;
//×¢²áÃüÁî
inline std::unordered_map<std::string, std::pair<std::string, PyObject*>> g_commands;
//ÉËº¦
inline static float g_damage = 0;

extern "C" PyObject * mc_init();