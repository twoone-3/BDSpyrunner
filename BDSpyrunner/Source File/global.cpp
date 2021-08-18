#include <global.h>

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