#include <global.h>

using namespace std;
//指令队列
SPSCQueue* g_command_queue = nullptr;
//网络处理
ServerNetworkHandler* g_server_network_handler = nullptr;
//世界
Level* g_level = nullptr;
//网络
RakPeer* g_rak_peer = nullptr;
//计分板
Scoreboard* g_scoreboard = nullptr;
//Py函数表
unordered_map<EventCode, vector<PyObject*>> g_callback_functions;
//注册命令
unordered_map<string, pair<string, PyObject*>> g_commands;
//伤害
int g_damage = 0;
