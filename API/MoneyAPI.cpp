#include "MoneyAPI.h"
#include "API/EventAPI.h"
#include <Global.hpp>
#include <LLAPI.h>
#include <LoggerAPI.h>
#include <I18nAPI.h>

typedef money_t (*LLMoneyGet_T)(xuid_t);
typedef string (*LLMoneyGetHist_T)(xuid_t, int);
typedef bool (*LLMoneyTrans_T)(xuid_t, xuid_t, money_t, string const&);
typedef bool (*LLMoneySet_T)(xuid_t, money_t);
typedef bool (*LLMoneyAdd_T)(xuid_t, money_t);
typedef bool (*LLMoneyReduce_T)(xuid_t, money_t);
typedef void (*LLMoneyClearHist_T)(int);

namespace money {
LLMoneyGet_T get = nullptr;
LLMoneySet_T set = nullptr;
LLMoneyAdd_T add = nullptr;
LLMoneyReduce_T reduce = nullptr;
LLMoneyTrans_T transfer = nullptr;
LLMoneyGetHist_T getHistory = nullptr;
LLMoneyClearHist_T clearHIstory = nullptr;
};


bool EconomySystem::init() {
	auto llmoney = LL::getPlugin("LLMoney");
	if (!llmoney) {
		logger.warn(tr("init.llMoney.noFound"));
		return false;
	}

	HMODULE h = llmoney->handle;

	money::get = (LLMoneyGet_T)GetProcAddress(h, "LLMoneyGet");
	if (!money::get)
		logger.warn("Fail to load API money::getMoney!");

	money::set = (LLMoneySet_T)GetProcAddress(h, "LLMoneySet");
	if (!money::set)
		logger.warn("Fail to load API money::setMoney!");

	money::add = (LLMoneyAdd_T)GetProcAddress(h, "LLMoneyAdd");
	if (!money::add)
		logger.warn("Fail to load API money::addMoney!");

	money::reduce = (LLMoneyReduce_T)GetProcAddress(h, "LLMoneyReduce");
	if (!money::reduce)
		logger.warn("Fail to load API money::reduceMoney!");

	money::transfer = (LLMoneyTrans_T)GetProcAddress(h, "LLMoneyTrans");
	if (!money::transfer)
		logger.warn("Fail to load API money::transMoney!");

	money::getHistory = (LLMoneyGetHist_T)GetProcAddress(h, "LLMoneyGetHist");
	if (!money::getHistory)
		logger.warn("Fail to load API money::getTransHist!");

	money::clearHIstory = (LLMoneyClearHist_T)GetProcAddress(h, "LLMoneyClearHist");
	if (!money::clearHIstory)
		logger.warn("Fail to load API money::clearHist!");

	return true;
}

money_t EconomySystem::getMoney(xuid_t player) {
	if (money::get)
		return money::get(player);
	else {
		logger.error("API money::getMoney have not been loaded!");
		return 0;
	}
}

bool EconomySystem::setMoney(xuid_t player, money_t money) {
	if (money::set)
		return money::set(player, money);
	else {
		logger.error("API money::setMoney have not been loaded!");
		return false;
	}
}

bool EconomySystem::addMoney(xuid_t player, money_t money) {
	if (money::add)
		return money::add(player, money);
	else {
		logger.error("API money::addMoney have not been loaded!");
		return false;
	}
}

bool EconomySystem::reduceMoney(xuid_t player, money_t money) {
	if (money::reduce)
		return money::reduce(player, money);
	else {
		logger.error("API money::reduceMoney have not been loaded!");
		return false;
	}
}

bool EconomySystem::transMoney(xuid_t player1, xuid_t player2, money_t money, string const& notes) {
	if (money::transfer)
		return money::transfer(player1, player2, money, notes);
	else {
		logger.error("API money::transMoney have not been loaded!");
		return false;
	}
}

std::string EconomySystem::getMoneyHist(xuid_t player, int time) {
	if (money::getHistory)
		return money::getHistory(player, time);
	else {
		logger.error("API money::getTransHist have not been loaded!");
		return "";
	}
}

bool EconomySystem::clearMoneyHist(int time) {
	if (money::clearHIstory) {
		money::clearHIstory(time);
		return true;
	}
	else {
		logger.error("API money::clearHist have not been loaded!");
		return false;
	}
}