#pragma once
#include "../tool.h"

struct Actor;
struct Player;
struct BlockSource;
struct Scoreboard;
struct BlockPalette;
struct SPSCQueue;
struct Level {
	//获取方块源 没这个维度返回空指针
	BlockSource* getBlockSource(int did);
	Scoreboard* getScoreBoard();
	unsigned getSeed();
	string getPlayerNames();
	Actor* fetchEntity(VA id);
	Player* getPlayerByXuid(const string& xuid);
	vector<Player*> getAllPlayers();
	BlockPalette* getBlockPalette();
};
