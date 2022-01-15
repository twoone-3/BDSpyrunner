#pragma once
#pragma execution_character_set("utf-8")

#include <MC/Actor.hpp>
#include <MC/ActorDamageSource.hpp>
#include <MC/BinaryStream.hpp>
#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockInstance.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockPalette.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Common.hpp>
#include <MC/CommandRegistry.hpp>
#include <MC/CommandParameterData.hpp>
#include <MC/Container.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>
#include <MC/Objective.hpp>
#include <MC/Player.hpp>
#include <MC/RakNet.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/ServerNetworkHandler.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/SignBlockActor.hpp>
#include <MC/Spawner.hpp>
#include <MC/StructureSettings.hpp>
#include <MC/StructureTemplate.hpp>

#include <MC/Tag.hpp>
#include <MC/ByteTag.hpp>
#include <MC/ShortTag.hpp>
#include <MC/IntTag.hpp>
#include <MC/Int64Tag.hpp>
#include <MC/FloatTag.hpp>
#include <MC/DoubleTag.hpp>
#include <MC/ByteArrayTag.hpp>
#include <MC/StringTag.hpp>
#include <MC/ListTag.hpp>
#include <MC/CompoundTag.hpp>
#include <MC/IntArrayTag.hpp>

#include <third-party/Nlohmann/fifo_json.hpp>

using std::unique_ptr;
using json_t = nlohmann::detail::value_t;
//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库3.10.4版本
inline fifo_json ToJson(std::string_view str) {
	try { return fifo_json::parse(str); }
	catch (const std::exception& e) {
		logger.error("Parsing JSON failed! {}", e.what());
		return nullptr;
	}
}

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
inline bool IsPlayer(Actor* ptr) {
	if (ptr == nullptr)
		return false;
	if (ptr->getEntityTypeId() != 319)
		return false;
	return true;
}
