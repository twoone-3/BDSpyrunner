#pragma once
#include <Global.hpp>

class Objective;
struct ObjectiveClass {
	Objective* thiz;

	ObjectiveClass(Objective* obj);

	string getName();
	string getDisplayName();
	bool setDisplay(const string& slot, int sort = 0);
	std::optional<int> setScore(const string& id, int score);
	std::optional<int> addScore(const string& id, int score);
	std::optional<int> reduceScore(const string& id, int score);
	bool deleteScore(const string& id);
	int getScore(const string& id);
};
