#pragma once
#include <vector>
#include <string>
#include "Position.h"

struct Actor;
struct Player;
struct BlockSource;
struct Scoreboard;
struct BlockPalette;
struct ItemActor;
struct ItemStack;
struct SPSCQueue;
struct Spawner {
	ItemActor* spawnItem(BlockSource* bs, ItemStack* item, Vec3* pos);
};

struct Level {
	//获取方块源 没这个维度返回空指针
	BlockSource* getBlockSource(int did);
	Scoreboard* getScoreBoard();
	unsigned getSeed();
	std::string getPlayerNames();
	Actor* fetchEntity(uintptr_t id);
	Player* getPlayerByXuid(const std::string& xuid);
	std::vector<Player*> getAllPlayers();
	BlockPalette* getBlockPalette();
	Spawner* getSpawner();
};
