#pragma once
#include "Actor.h"
#include "Block.h"
#include "ItemStack.h"
#include "Level.h"
#include "NetWork.h"
#include "ScoreBoard.h"
#include "Structure.h"
#include "Tag.h"
#include "Event.h"
typedef struct _object PyObject;
//指令队列
extern SPSCQueue* g_command_queue;
//网络处理
extern ServerNetworkHandler* g_server_network_handler;
//世界
extern Level* g_level;
//网络
extern RakPeer* g_rak_peer;
//计分板
extern Scoreboard* g_scoreboard;
//Py函数表
extern unordered_map<EventCode, std::vector<PyObject*>> g_callback_functions;
//注册命令
extern unordered_map<string, std::pair<const char*, PyObject*>> g_commands;
//伤害
extern int g_damage;
