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
#define f(type, ptr) (*(type*)(ptr))
#define SYMAS(ret, fn, ...) ((ret(*)(__VA_ARGS__))SYM(fn))
#define SYM GetServerSymbol
using VA = unsigned long long;
void* HookFunction(void*, void*);
void* GetServerSymbol(const char*);
template<typename ret = void, typename... Args>
inline ret SYMCALL(const char* sym, Args... args) {
	return ((ret(*)(Args...))SYM(sym))(args...);
}
#define Hook(name, ret, sym, ...)		\
struct name {							\
	static ret(*original)(__VA_ARGS__);	\
	static ret _hook(__VA_ARGS__);		\
};										\
ret(*name::original)(__VA_ARGS__)=(ret(*)(__VA_ARGS__))HookFunction(SYM(sym), &name::_hook);\
ret name::_hook(__VA_ARGS__)