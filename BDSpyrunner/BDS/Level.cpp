#include "Level.h"

//获取方块源 没这个维度返回空指针

inline BlockSource* Level::getBlockSource(int did) {
	VA d = SymCall<VA>("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
		this, did);
	if (!d)
		return nullptr;
	return FETCH(BlockSource*, d + 96);//IDA Level::tickEntities 120
}

inline Scoreboard* Level::getScoreBoard() {
	return FETCH(Scoreboard*, this + 8600);//IDA Level::getScoreboard
}

inline unsigned Level::getSeed() {
	return SymCall<unsigned>("?getSeed@Level@@UEAAIXZ", this);
}

inline string Level::getPlayerNames() {
	string s;
	SymCall<string&>("?getPlayerNames@Level@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &s);
	return s;
}

inline Actor* Level::fetchEntity(VA id) {
	return SymCall<Actor*>("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z",
		this, id, false);
}

inline Player* Level::getPlayerByXuid(const string& xuid) {
	return SymCall<Player*>("?getPlayerByXuid@Level@@UEBAPEAVPlayer@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, &xuid);
}

inline vector<Player*> Level::getAllPlayers() {
	return FETCH(vector<Player*>, this + 112);//IDA Level::forEachPlayer
}

inline BlockPalette* Level::getBlockPalette() {
	return FETCH(BlockPalette*, this + 2120);
}
