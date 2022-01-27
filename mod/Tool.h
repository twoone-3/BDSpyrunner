#pragma once
#pragma execution_character_set("utf-8")

#include <Global.h>
#include <LoggerAPI.h>

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

constexpr const char* PYR_VERSION = "v1.9.1";

//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库
fifo_json StrToJson(std::string_view str);

bool IsPlayer(Actor* ptr);

inline Logger logger("BDSpyrunner");

