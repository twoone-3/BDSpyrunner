#include <Global.hpp>
#include "LoggerAPI.h"
#include "PlayerAPI.h"

LoggerClass::LoggerClass(const string& title) : thiz(title) {}

void LoggerClass::debug(const string& msg) { thiz.debug(msg); }

void LoggerClass::info(const string& msg) { thiz.info(msg); }

void LoggerClass::warn(const string& msg) { thiz.warn(msg); }

void LoggerClass::error(const string& msg) { thiz.error(msg); }

void LoggerClass::fatal(const string& msg) { thiz.fatal(msg); }

void LoggerClass::setTitle(const string& title) { thiz.title = title; }

bool LoggerClass::setFile(const string& file, int level) {
	thiz.setFile(file, std::ios::app);
	thiz.fileLevel = level;
	return thiz.ofs.is_open();
}

bool LoggerClass::setPlayer(const PlayerClass& p, int level) {
	thiz.player = p.thiz;
	thiz.playerLevel = level;
	return true;
}
