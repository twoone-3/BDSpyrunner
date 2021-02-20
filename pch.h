#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include "json/json.h"
#include "include/Python.h"
#define f(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYMAS(ret, fn, ...) (reinterpret_cast<ret(*)(__VA_ARGS__)>(SYM(fn)))
#define SYM(sym) GetServerSymbol(sym)
using VA = unsigned long long;
extern "C" {
	_declspec(dllimport) int HookFunction(void*, void*, void*);
	_declspec(dllimport) void* GetServerSymbol(const char*);
}
template<typename ret = void, typename... Args>
static ret SYMCALL(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)printf("Failed to call %s\n", sym);
	return ((ret(*)(Args...))found)(args...);
}
static void* HookRegister(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)printf("Failed to hook %s\n", sym);
	HookFunction(found, org, hook);
	return org;
}
#define Hook(name, ret, sym, ...)		\
struct name {							\
	static ret(*original)(__VA_ARGS__);	\
	static ret _hook(__VA_ARGS__);		\
};										\
ret(*name::original)(__VA_ARGS__)=*(ret(**)(__VA_ARGS__))HookRegister(sym, &name::_hook,&name::original);\
ret name::_hook(__VA_ARGS__)