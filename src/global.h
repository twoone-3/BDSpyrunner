#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#define PLUGIN_PATH "./plugins/py/"

using std::pair;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

enum class EventCode;

extern class Logger logger;
