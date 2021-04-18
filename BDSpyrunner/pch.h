#pragma once
//#pragma warning(disable:4996)
#pragma execution_character_set("utf-8")
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
#include <map>
#include <unordered_map>
#define JSON_USE_EXCEPTION 0
#include "json/json.h"
#define PY_SSIZE_T_CLEAN
#include "include/Python.h"

using namespace std;
using VA = unsigned long long;

#define FETCH(type, ptr) (*(type*)(ptr))
#define SYM(sym) GetServerSymbol(sym)
#define HOOK(name, ret, sym, ...)		\
struct name {							\
	typedef ret(*fn)(__VA_ARGS__);		\
	static ret _hook(__VA_ARGS__);		\
	static fn original;					\
};										\
name::fn name::original = *(name::fn*)HookRegister(sym, name::_hook, &name::original); \
ret name::_hook(__VA_ARGS__)

template<class T>
static void inline print(const T& data) {
	cout << data << endl;
}
template<class T, class... T2>
static void inline print(const T& data, T2... other) {
	cout << data;
	print(other...);
}

extern "C" {
	_declspec(dllimport) int HookFunction(void*, void*, void*);
	_declspec(dllimport) void* GetServerSymbol(const char*);
}
template<typename ret = void, typename... Args>
static ret SYMCALL(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)
		print("Failed to call ", sym);
	return ((ret(*)(Args...))found)(args...);
}
static void* HookRegister(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)
		print("Failed to hook ", sym);
	HookFunction(found, org, hook);
	return org;
}

#include "BDS/Actor.h"
#include "BDS/Block.h"
#include "BDS/Item.h"
#include "BDS/Level.h"
#include "BDS/Math.h"
#include "BDS/NBT.h"
#include "BDS/ScoreBoard.h"
#include "BDS/Structure.h"
