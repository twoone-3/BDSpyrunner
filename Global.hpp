#pragma once
#pragma execution_character_set("utf-8")
#include <DynamicCommandAPI.h>
#include <FormUI.h>
#include <LoggerAPI.h>
#include <EventAPI.h>
#include <ServerAPI.h>
#include <I18nAPI.h>

#include <third-party/magic_enum/magic_enum.hpp>
#include <third-party/Nlohmann/fifo_json.hpp>

#include <MC/Actor.hpp>
#include <MC/ActorDamageSource.hpp>
#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockPalette.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Common.hpp>
#include <MC/Container.hpp>
#include <MC/Dimension.hpp>
//#include <MC/GameTypeConv.hpp>
#include <MC/ItemInstance.hpp>
#include <MC/ItemStack.hpp>
#include <MC/JsonHelpers.hpp>
#include <MC/Level.hpp>
#include <MC/MobEffect.hpp>
#include <MC/Objective.hpp>
#include <MC/Player.hpp>
#include <MC/RakNet.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/ServerNetworkHandler.hpp>
#include <MC/SignBlockActor.hpp>
#include <MC/Spawner.hpp>
#include <MC/StructureSettings.hpp>
#include <MC/StructureTemplate.hpp>

#include <MC/ByteArrayTag.hpp>
#include <MC/ByteTag.hpp>
#include <MC/CompoundTag.hpp>
#include <MC/DoubleTag.hpp>
#include <MC/EndTag.hpp>
#include <MC/FloatTag.hpp>
#include <MC/Int64Tag.hpp>
#include <MC/IntArrayTag.hpp>
#include <MC/IntTag.hpp>
#include <MC/ListTag.hpp>
#include <MC/ShortTag.hpp>
#include <MC/StringTag.hpp>
#include <MC/Tag.hpp>

#include <pybind11/embed.h>
#include <pybind11/stl.h>
// clang-format off
#define PY_TRY try {
#define PY_CATCH } catch (const std::exception& e) {logger.error("\n{}", e.what());}
#define PLUGIN_PATH "plugins\\py\\"

#define EVENT_BEGIN(code) Callbacker h(code); PY_TRY; h.insert("Event", py::cast(magic_enum::enum_name(code)))
#define EVENT_INSERT(key) h.insert(#key, ev.m##key)
#define EVENT_INSERT_EX(key, value) h.insert(#key, (value))
#define EVENT_END PY_CATCH return h.callback()
// clang-format on

namespace py = pybind11;
using json_t = nlohmann::detail::value_t;
using std::unique_ptr;

enum GameType {
	Survival = 0,
	Creative = 1,
	Adventure = 2,
	Spectator = 6,
};

//全局变量
extern bool isCmdRegisterEnabled;
extern Logger logger;
extern std::unordered_map<enum class EventCode, std::vector<py::function>> listeners;
extern std::unordered_map<string, py::object> player_data;

std::string DimId2Name(int dimid);
//输出错误信息
void PrintError(const std::exception&);
//字符串转JSON
fifo_json StrToJson(std::string_view str);
//安全地调用py
template <typename... Args>
py::object call(const py::object& obj, Args&&... args) {
	PY_TRY;
	py::gil_scoped_acquire gil_;
	return obj(std::forward<Args>(args)...);
	PY_CATCH;
	return py::none();
}
