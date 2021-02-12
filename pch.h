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
#include "Detours/include/detours.h"
#define f(type, ptr) (*(type*)(ptr))
#define SYMAS(ret, fn, ...) ((ret(*)(__VA_ARGS__))SYM(fn))
#define SYM(rva) (void*)(_handle + rva)
using VA = unsigned long long;
using RVA = unsigned int;
#include "RVA.h"
void* HookFunction(void* oldfunc, void* newfunc) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&oldfunc, newfunc);
	DetourTransactionCommit();
	return oldfunc;
}
static VA _handle = (VA)GetModuleHandle(0);
template<typename ret = void, typename... Args>
inline ret SYMCALL(RVA off, Args... args) {
	return ((ret(*)(Args...))SYM(off))(args...);
}
#define Hook(name, ret, rva, ...)		\
struct name {							\
	static ret(*original)(__VA_ARGS__);	\
	static ret _hook(__VA_ARGS__);		\
};										\
ret(*name::original)(__VA_ARGS__)=(ret(*)(__VA_ARGS__))HookFunction(SYM(rva), &name::_hook);\
ret name::_hook(__VA_ARGS__)