#pragma once
#pragma execution_character_set("utf-8")
#include <iostream>

#define FETCH(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYM GetServerSymbol
#define HOOK(name, ret, sym, ...)	\
struct name {						\
	using func = ret(__VA_ARGS__);	\
	static func _hook;				\
	static func* original;			\
};									\
name::func* name::original =		\
*reinterpret_cast<name::func**>		\
(SymHook(sym, name::_hook, &name::original)); \
ret name::_hook(__VA_ARGS__)

extern "C" {
	// provide Detours API
	_declspec(dllimport) int HookFunction(void*, void*, void*);
	// get address from symbol string
	_declspec(dllimport) void* GetServerSymbol(const char*);
}
// call a virtual function
// _this: this ptr, off: offsetof function
template<typename ReturnType = void, typename... Args>
ReturnType VirtualCall(uintptr_t off, void* _this, Args... args) {
	return (*reinterpret_cast<ReturnType(**)(void*, Args...)>(*reinterpret_cast<uintptr_t*>(_this) + off))(_this, args...);
}
// call a function by symbol string
template<typename ReturnType = void, typename... Args>
ReturnType SymCall(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)
		std::cerr << "Failed to call " << sym << std::endl;
	return reinterpret_cast<ReturnType(*)(Args...)>(found)(args...);
}
// replace the function
void* SymHook(const char* sym, void* hook, void* org);
constexpr size_t Hash(const char* s);
//´´½¨°ü
uintptr_t createPacket(int type);
bool isPlayer(void*);
