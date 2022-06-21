#pragma once
#include <Global.hpp>

struct PlayerClass;
struct LoggerClass {
	Logger thiz;

	LoggerClass(const string& title);

	void debug(const string& msg);
	void info(const string& msg);
	void warn(const string& msg);
	void error(const string& msg);
	void fatal(const string& msg);

	void setTitle(const string& title);
	bool setFile(const string& file, int level = 4);
	bool setPlayer(const PlayerClass& p, int level = 4);
};
