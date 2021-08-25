#include "Level.h"
#include "tool.h"

using namespace std;
//获取方块源 没这个维度返回空指针

BlockSource* Level::getBlockSource(int did) {
	uintptr_t d = SymCall<uintptr_t>("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
		this, did);
	if (!d)
		return nullptr;
	return FETCH(BlockSource*, d + 96);//IDA Level::tickEntities 120
}

Scoreboard* Level::getScoreBoard() {
	return FETCH(Scoreboard*, this + 8600);//IDA Level::getScoreboard
}

unsigned Level::getSeed() {
	return SymCall<unsigned>("?getSeed@Level@@UEAAIXZ", this);
}

string Level::getPlayerNames() {
	string s;
	SymCall<string&>("?getPlayerNames@Level@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &s);
	return s;
}

Actor* Level::fetchEntity(uintptr_t id) {
	return SymCall<Actor*>("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z",
		this, id, false);
}

Player* Level::getPlayerByXuid(const string& xuid) {
	return SymCall<Player*>("?getPlayerByXuid@Level@@UEBAPEAVPlayer@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, &xuid);
}

vector<Player*> Level::getAllPlayers() {
	return FETCH(vector<Player*>, this + 112);//IDA Level::forEachPlayer
}

BlockPalette* Level::getBlockPalette() {
	return FETCH(BlockPalette*, this + 2120);
}

Spawner* Level::getSpawner() {
	return SymCall<Spawner*>("?getSpawner@Level@@UEBAAEAVSpawner@@XZ",
		this);
}

ItemActor* Spawner::spawnItem(BlockSource* bs, ItemStack* item, Vec3* pos) {
	return SymCall<ItemActor*>("?spawnItem@Spawner@@QEAAPEAVItemActor@@AEAVBlockSource@@AEBVItemStack@@PEAVActor@@AEBVVec3@@H@Z",
		this, bs, item, nullptr, pos, 0);
}
