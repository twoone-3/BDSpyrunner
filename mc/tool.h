#pragma once
#pragma execution_character_set("utf-8")
#pragma warning(disable:4996)
#include <iostream>
#include "Level.h"
#include "NetWork.h"

#define FETCH(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYM GetServerSymbol
#define HOOK(name, ret, sym, ...)	\
struct name {						\
	using func = ret(__VA_ARGS__);	\
	static func _hook;				\
	static func* original;			\
};									\
name::func* name::original = *reinterpret_cast<name::func**>(SymHook(sym, name::_hook, &name::original));\
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
inline void* SymHook(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)
		std::cerr << "Failed to hook " << sym << std::endl;
	HookFunction(found, org, hook);
	return org;
}
//全局变量
template <typename T>
struct Global {
	inline static T* data = nullptr;
};
//SPSCQueue* Global<SPSCQueue>::data = nullptr;
//RakPeer* Global<RakPeer>::data = nullptr;
//ServerNetworkHandler* Global<ServerNetworkHandler>::data = nullptr;
//Level* Global<Level>::data = nullptr;
//Scoreboard* Global<Scoreboard>::data = nullptr;

//字符串哈希
inline constexpr size_t Hash(const char* s) {
	unsigned h = 0;
	for (; *s; ++s)
		h = 5 * h + *s;
	return static_cast<size_t>(h);
}

//创建包
inline uintptr_t createPacket(int type) {
	uintptr_t pkt[2];
	SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		pkt, type);
	return *pkt;
}

//是否为玩家
inline bool isPlayer(void* ptr) {
	for (auto p : Global<Level>::data->getAllPlayers()) {
		if (ptr == p)
			return true;
	}
	return false;
}
