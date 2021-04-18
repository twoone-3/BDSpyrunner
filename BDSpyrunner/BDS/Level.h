#pragma once
#include "../pch.h"
#include "Block.h"
#include "ScoreBoard.h"
struct Actor;
struct Level {
	//获取方块源 没这个维度返回空指针
	BlockSource* getBlockSource(int did) {
		VA d = SYMCALL<VA>("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
			this, did);
		if (!d)return 0;
		return FETCH(BlockSource*, d + 96);//IDA Level::tickEntities 120
	}
	Scoreboard* getScoreBoard() {//IDA Level::removeEntityReferences
		return FETCH(Scoreboard*, this + 8464);
	}
	unsigned getSeed() {
		return SYMCALL<unsigned>("?getSeed@Level@@UEAAIXZ", this);
	}
	string getPlayerNames() {
		string s;
		SYMCALL<string&>("?getPlayerNames@Level@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this, &s);
		return s;
	}
	Actor* fetchEntity(VA id) {
		return SYMCALL<Actor*>("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z",
			this, id, false);
	}
	const vector<Player*>& getAllPlayers() {
		return FETCH(vector<Player*>, this + 112);
	}
	BlockPalette* getBlockPalette() {
		return FETCH(BlockPalette*, this + 2072);
	}
};
struct ServerNetworkHandler {
	Player* _getServerPlayer(VA id, VA pkt) {
		return SYMCALL<Player*>("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
			this, id, FETCH(char, pkt + 16));
	}
};