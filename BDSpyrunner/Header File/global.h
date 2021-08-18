#pragma once
#include <unordered_map>
#include <vector>
#include <string>

typedef struct _object PyObject;
//指令队列
extern struct SPSCQueue* g_command_queue;
//网络处理
extern struct ServerNetworkHandler* g_server_network_handler;
//世界
extern struct Level* g_level;
//网络
extern struct RakPeer* g_rak_peer;
//计分板
extern struct Scoreboard* g_scoreboard;
//Py函数表
extern std::unordered_map<enum class EventCode, std::vector<PyObject*>> g_callback_functions;
//注册命令
extern std::unordered_map<std::string, std::pair<std::string, PyObject*>> g_commands;
//伤害
extern int g_damage;
