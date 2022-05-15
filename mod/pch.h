#pragma once
#pragma execution_character_set("utf-8")
#pragma region LL
#include <DynamicCommandAPI.h>
#include <FormUI.h>
#include <LoggerAPI.h>
#include <EventAPI.h>
#include <ServerAPI.h>
#pragma endregion
#pragma region third_party
#include <third-party/magic_enum/magic_enum.hpp>
#include <third-party/Nlohmann/fifo_json.hpp>
#pragma endregion
#pragma region MC
#include <MC/Actor.hpp>
#include <MC/ActorDamageSource.hpp>
#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockPalette.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Common.hpp>
#include <MC/Container.hpp>
//#include <MC/ItemInstance.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>
#include <MC/MobEffect.hpp>
#include <MC/Objective.hpp>
#include <MC/Player.hpp>
#include <MC/RakNet.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/ServerNetworkHandler.hpp>
#include <MC/SignBlockActor.hpp>
//#include <MC/SimpleContainer.hpp>
#include <MC/Spawner.hpp>
#include <MC/StructureSettings.hpp>
#include <MC/StructureTemplate.hpp>
#pragma endregion
#pragma region NBT
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
#pragma endregion
#pragma region Python
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#pragma endregion

#define PY_TRY try {
#define PY_CATCH } catch (const std::exception& e) {logger.error("\n{}", e.what());}
#define PLUGIN_PATH "plugins\\py\\"

#define EVENT_BEGIN(evt) evt::subscribe([code](evt e){Callbacker h(code); PY_TRY; h.insert("Event", py::cast(magic_enum::enum_name(code)))
#define EVENT_INSERT(key) h.insert(#key, e.m##key)
#define EVENT_INSERT_EX(key, value) h.insert(#key, (value))
#define EVENT_END PY_CATCH return h.callback();})

#define ADD_ENUM(type) 	{auto entries = magic_enum::enum_entries<type>(); auto e = py::enum_<type>(m, #type); for (auto& [val, name] : entries) {e.value(name.data(), val); }}

namespace py = pybind11;
using json_t = nlohmann::detail::value_t;
