#pragma once
#pragma execution_character_set("utf-8")
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

//using std::string;
//using std::wstring;
//using std::string_view;
//using std::vector;
//using std::pair;
//using std::map;
//using std::unordered_map;
//using std::function;
//using std::unique_ptr;
//using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using VA = unsigned long long;

#define FETCH(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYM GetServerSymbol
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
template<typename ReturnType = void, typename... Args>
ReturnType VirtualCall(VA off, void* _this, Args... args) {
	return (*reinterpret_cast<ReturnType(**)(void*, Args...)>(*reinterpret_cast<VA*>(_this) + off))(_this, args...);
}
// call a function by symbol string
template<typename ReturnType = void, typename... Args>
ReturnType SymCall(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)
		cerr << "Failed to call " << sym << endl;
	return reinterpret_cast<ReturnType(*)(Args...)>(found)(args...);
}
// replace the function
void* SymHook(const char* sym, void* hook, void* org);
constexpr size_t Hash(const char* s);
//´´½¨°ü
VA createPacket(int type);
bool isPlayer(void*);
