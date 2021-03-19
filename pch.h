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
#include "json/json.h"
#include "include/Python.h"

#define f(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYM(sym) GetServerSymbol(sym)

#pragma warning(disable:4996)
#pragma execution_character_set("utf-8")
using namespace std;
using VA = unsigned long long;
extern "C" {
	_declspec(dllimport) int HookFunction(void*, void*, void*);
	_declspec(dllimport) void* GetServerSymbol(const char*);
}
template<typename ret = void, typename... Args>
static inline ret SYMCALL(const char* sym, Args... args) {
	return ((ret(*)(Args...))SYM(sym))(args...);
}
static void* HookRegister(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)cerr << "Failed to hook " << sym << endl;
	HookFunction(found, org, hook);
	return org;
}
#define Hook(name, ret, sym, ...)		\
namespace name {						\
	typedef ret(*fn)(__VA_ARGS__);		\
	static ret _hook(__VA_ARGS__);		\
	static fn original = *(fn*)HookRegister(sym, name::_hook, &name::original);\
};										\
ret name::_hook(__VA_ARGS__)