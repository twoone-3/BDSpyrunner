#pragma once
#pragma execution_character_set("utf-8")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include "json.hpp"

using std::string;
using std::wstring;
using std::string_view;
using std::vector;
using std::pair;
using std::map;
using std::unordered_map;
using std::function;
using std::unique_ptr;
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using nlohmann::json;
using JsonType = json::value_t;
using VA = unsigned long long;

#define INFO(str) cout << "[BDSpyrunner] " << str << endl
#define ERR(str) cerr << "[BDSpyrunner] " << str << endl
#define FETCH(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYM GetServerSymbol
// make a hook
#define HOOK(name, ret, sym, ...)		\
struct name {							\
	using func = ret(__VA_ARGS__);		\
	static func _hook;					\
	static func* original;				\
};										\
name::func* name::original = *reinterpret_cast<name::func**>(SymHook(sym, name::_hook, &name::original)); \
ret name::_hook(__VA_ARGS__)
// provide Detours API
extern "C" _declspec(dllimport)
int HookFunction(void*, void*, void*);
// get address from symbol string
extern "C" _declspec(dllimport)
void* GetServerSymbol(const char*);
// call a virtual function
// _this: this ptr, off: offsetof function
template<typename ret = void, typename... Args>
static ret VirtualCall(VA off, void* _this, Args... args) {
	return (*reinterpret_cast<ret(**)(void*, Args...)>(*reinterpret_cast<VA*>(_this) + off))(_this, args...);
}
// call a function by symbol string
template<typename ret = void, typename... Args>
static ret SymCall(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)
		ERR("Failed to call " << sym);
	return reinterpret_cast<ret(*)(Args...)>(found)(args...);
}
static void* SymHook(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)
		ERR("Failed to hook " << sym);
	HookFunction(found, org, hook);
	return org;
}
constexpr size_t Hash(const char* s) {
	unsigned h = 0;
	for (; *s; ++s)
		h = 5 * h + *s;
	return static_cast<size_t>(h);
}
static json StringtoJson(string_view str) {
	try {
		return json::parse(str);
	}
	catch (const json::exception& e) {
		ERR(e.what());
		return nullptr;
	}
}
