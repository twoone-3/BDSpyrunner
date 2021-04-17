#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <filesystem>
#include <unordered_map>
#define JSON_USE_EXCEPTION 0
#include "json/json.h"
#define PY_SSIZE_T_CLEAN
#include "include/Python.h"
using namespace std;
#define FETCH(type, ptr) (*reinterpret_cast<type*>(ptr))
//#pragma warning(disable:4996)
#pragma execution_character_set("utf-8")

#define SYM(sym) GetServerSymbol(sym)
using VA = unsigned long long;
extern "C" {
	_declspec(dllimport) int HookFunction(void*, void*, void*);
	_declspec(dllimport) void* GetServerSymbol(const char*);
}
template<typename ret = void, typename... Args>
static ret SYMCALL(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)std::cerr << "Failed to call " << sym << std::endl;
	return ((ret(*)(Args...))found)(args...);
}
static void* HookRegister(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)std::cerr << "Failed to hook " << sym << std::endl;
	HookFunction(found, org, hook);
	return org;
}
#define HOOK(name, ret, sym, ...)		\
struct name {							\
	typedef ret(*fn)(__VA_ARGS__);		\
	static ret _hook(__VA_ARGS__);		\
	static fn original;					\
};										\
name::fn name::original = *(name::fn*)HookRegister(sym, name::_hook, &name::original); \
ret name::_hook(__VA_ARGS__)

#include "BDS/Actor.h"
#include "BDS/Block.h"
#include "BDS/Item.h"
#include "BDS/Level.h"
#include "BDS/Math.h"
#include "BDS/NBT.h"
#include "BDS/ScoreBoard.h"
#include "BDS/Structure.h"
