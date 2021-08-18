#pragma once
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
