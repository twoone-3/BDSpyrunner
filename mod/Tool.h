#pragma once
#pragma execution_character_set("utf-8")
#pragma warning(disable:4996)
#include <iostream>
#include <Global.h>
#include <MC/Actor.hpp>
#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Container.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>
#include <MC/Objective.hpp>
#include <MC/Player.hpp>
#include <MC/RakNet.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/ServerNetworkHandler.hpp>
#include <MC/StructureSettings.hpp>
#include <MC/StructureTemplate.hpp>

#include <MC/Tag.hpp>
#include <MC/CompoundTag.hpp>

#undef SymCall
template <typename T>
inline T& Fetch(void* ptr, uintptr_t offset) {
	return *reinterpret_cast<T*>(uintptr_t(ptr) + offset);
}
template <typename T>
inline T& Fetch(uintptr_t ptr, uintptr_t offset) {
	return *reinterpret_cast<T*>(uintptr_t(ptr) + offset);
}
template <typename T>
inline T& Fetch(void* ptr) {
	return *reinterpret_cast<T*>(ptr);
}
template <typename T>
inline T& Fetch(uintptr_t ptr) {
	return *reinterpret_cast<T*>(ptr);
}
#define THOOK(name, ret, sym, ...)		\
struct name {							\
	using func = ret(__VA_ARGS__);		\
	static func _hook;					\
	inline static func* original;		\
	inline static HookRegister _reg =	\
		{ SYM(sym),&original,&_hook };	\
};										\
ret name::_hook(__VA_ARGS__)

// call a virtual function
// _this: this ptr, off: offsetof function
template<typename Return = void, typename... Args>
inline Return VirtualCall(uintptr_t off, void* _this, Args... args) {
	return (*reinterpret_cast<Return(**)(void*, Args...)>(Fetch<uintptr_t>(_this) + off))(_this, args...);
}
// call a function by symbol string
template<typename Return = void, typename... Args>
inline Return SymCall(const char* sym, Args... args) {
	return reinterpret_cast<Return(*)(Args...)>(SYM(sym))(std::forward<Args>(args)...);
}
//全局变量
template <typename T>
inline T* global = nullptr;
//Vec3转list
inline PyObject* ToList(Vec3 vec) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(vec.x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(vec.y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(vec.z));
	return list;
}
//Vec3转list
inline PyObject* ToList(Vec3* vec) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(vec->x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(vec->y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(vec->z));
	return list;
}
//方块坐标转list
inline PyObject* ToList(BlockPos* bp) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyLong_FromLong(bp->x));
	PyList_SetItem(list, 1, PyLong_FromLong(bp->y));
	PyList_SetItem(list, 2, PyLong_FromLong(bp->z));
	return list;
}

bool IsPlayer(Actor* ptr) {
	if (ptr == nullptr)
		return false;
	if (ptr->getEntityTypeId() != 319)
		return false;
	return true;
}