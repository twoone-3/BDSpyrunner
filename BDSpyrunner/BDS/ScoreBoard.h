#pragma once
#include "../tool.h"

struct Player;
struct PlayerScore {
	VA getScore();
};
//scoreboardcmd list; objective::objective; scoreboard getscores
//int scores    +12 this[12]
//string displayname  +96
//string name +64
struct ScoreInfo {
	int getCount();
};
struct ScoreboardId {
	int id;
	VA null;
};
struct ScorePacketInfo {
	ScoreboardId sid;
	string obj_name = "name";
	unsigned score;
	enum Type : char { Invalid = 0, Player = 1, Actor = 2, Fake = 3 };
	Type type = Fake;
	VA pid;
	VA aid;
	string fake_name;

	ScorePacketInfo(ScoreboardId* s, unsigned num, const string& fake);
};
struct Objective {
	//获取计分板名称
	string getScoreName();
	//获取计分板展示名称
	string getScoreDisplayName();
	auto createScoreboardId(Player* player);
	ScoreInfo* getPlayerScore(ScoreboardId* a2);
};
struct Scoreboard {
	Objective* getObjective(string str);
	ScoreboardId* getScoreboardId(const string& str);
	vector<Objective*> getObjectives();
	auto getDisplayInfoFiltered(string* str);
	auto getTrackedIds();
	ScoreboardId* getScoreboardId(Player* a2);
	//更改玩家分数
	int modifyPlayerScore(ScoreboardId* a3, Objective* a4, int count, int mode);
	ScoreboardId* createScoreBoardId(const string& s);
	ScoreboardId* createScoreBoardId(Player* player);
};
