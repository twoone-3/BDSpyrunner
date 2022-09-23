#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/ServerAPI.h>

#include <llapi/mc/Level.hpp>

#include "api/block.h"
#include "api/block_entity.h"
#include "api/command.h"
#include "api/command_origin.h"
#include "api/command_output.h"
#include "api/container.h"
#include "api/entity.h"
#include "api/item.h"
#include "api/money.h"
#include "api/nbt.h"
#include "api/player.h"
#include "api/scoreboard.h"
#include "api/utils.h"
#include "py_utils.h"

#define DEF_ENUM(name, type)                           \
  {                                                    \
    auto&& entries = magic_enum::enum_entries<type>(); \
    auto&& e = py::enum_<type>(mc_module, name);       \
    for (auto& [val, n] : entries) {                   \
      e.value(n.data(), val);                          \
    }                                                  \
  }
#define DEF_ENUM_SIMPLE(type) DEF_ENUM(#type, type)

#define def_copy(c) def("__copy__", [](const c& self) { return c(self); })

PYBIND11_EMBEDDED_MODULE(mc, mc_module) {
  using py::literals::operator""_a;
  PY_TRY;
#pragma region Enums
  DEF_ENUM_SIMPLE(TitleType);
  DEF_ENUM_SIMPLE(TextType);
  DEF_ENUM_SIMPLE(Mirror);
  DEF_ENUM_SIMPLE(Rotation);
  DEF_ENUM_SIMPLE(CommandPermissionLevel);
  DEF_ENUM("ParameterType", DynamicCommand::ParameterType);
  DEF_ENUM("EffectChangedType", Event::PlayerEffectChangedEvent::EventType);
  DEF_ENUM_SIMPLE(CommandParameterOption);
  DEF_ENUM_SIMPLE(GameType);
  DEF_ENUM_SIMPLE(SnbtFormat);
  DEF_ENUM_SIMPLE(ObjectiveSortOrder);
  DEF_ENUM_SIMPLE(BossEventColour);
  DEF_ENUM("ScriptFacing", ScriptModuleMinecraft::ScriptFacing);
  DEF_ENUM("TagType", Tag::Type);

#pragma endregion
#pragma region Classes
  py::class_<Vec2>(mc_module, "Vec2")
      .def(py::init<float, float>())

      .def_property(
          "x", [](const Vec2& pos) { return pos.x; },
          [](Vec2& pos, float val) { pos.x = val; })
      .def_property(
          "y", [](const Vec2& pos) { return pos.y; },
          [](Vec2& pos, float val) { pos.y = val; })
      .def("__repr__", &Vec2::toString);

  py::class_<Vec3>(mc_module, "Vec3")
      .def(py::init<float, float, float>())

      .def_property(
          "x", [](const Vec3& pos) { return pos.x; },
          [](Vec3& pos, float val) { pos.x = val; })
      .def_property(
          "y", [](const Vec3& pos) { return pos.y; },
          [](Vec3& pos, float val) { pos.y = val; })
      .def_property(
          "z", [](const Vec3& pos) { return pos.z; },
          [](Vec3& pos, float val) { pos.z = val; })
      .def("__repr__", &Vec3::toString);

  py::class_<BlockPos>(mc_module, "BlockPos")
      .def(py::init<int, int, int>())

      .def_property(
          "x", [](const BlockPos& pos) { return pos.x; },
          [](BlockPos& pos, int val) { pos.x = val; })
      .def_property(
          "y", [](const BlockPos& pos) { return pos.y; },
          [](BlockPos& pos, int val) { pos.y = val; })
      .def_property(
          "z", [](const BlockPos& pos) { return pos.z; },
          [](BlockPos& pos, int val) { pos.z = val; })
      .def("__repr__", &BlockPos::toString);

  py::class_<BlockClass>(mc_module, "Block")
      .def_property("name", &BlockClass::getName, nullptr)
      .def_property("type", &BlockClass::getType, nullptr)
      .def_property("id", &BlockClass::getId, nullptr)
      .def_property("pos", &BlockClass::getPos, nullptr)
      .def_property("dim", &BlockClass::getDim, nullptr)
      .def_property("tile_data", &BlockClass::getTileData, nullptr)

      .def("setNbt", &BlockClass::setNbt)
      .def("getNbt", &BlockClass::getNbt)
      .def("hasContainer", &BlockClass::hasContainer)
      .def("getContainer", &BlockClass::getContainer)
      .def("hasBlockEntity", &BlockClass::hasBlockEntity)
      .def("getBlockEntity", &BlockClass::getBlockEntity)
      .def("removeBlockEntity", &BlockClass::removeBlockEntity);

  py::class_<ContainerClass>(mc_module, "Container")
      .def_property("size", &ContainerClass::getSize, nullptr)
      .def_property("type", &ContainerClass::getType, nullptr)

      .def("addItem", &ContainerClass::addItem)
      .def("addItemToFirstEmptySlot", &ContainerClass::addItemToFirstEmptySlot)
      .def("hasRoomFor", &ContainerClass::hasRoomFor)
      .def("removeItem", &ContainerClass::removeItem)
      .def("getItem", &ContainerClass::getItem)
      .def("setItem", &ContainerClass::setItem)
      .def("getAllItems", &ContainerClass::getAllItems)
      .def("removeAllItems", &ContainerClass::removeAllItems)
      .def("isEmpty", &ContainerClass::isEmpty);

  py::class_<ItemClass>(mc_module, "Item")
      .def(py::init())

      .def_property("name", &ItemClass::getName, nullptr)
      .def_property("type", &ItemClass::getType, nullptr)
      .def_property("id", &ItemClass::getId, nullptr)
      .def_property("count", &ItemClass::getCount, nullptr)
      .def_property("aux", &ItemClass::getAux, nullptr)

      .def("set", &ItemClass::set)
      .def("clone", &ItemClass::clone)
      .def("isNull", &ItemClass::isNull)
      .def("setNull", &ItemClass::setNull)
      .def("setAux", &ItemClass::setAux)
      .def("setLore", &ItemClass::setLore)
      .def("setNbt", &ItemClass::setNbt)
      .def("getNbt", &ItemClass::getNbt);

  py::class_<CommandClass>(mc_module, "Command")
      .def(py::init<string, string, CommandPermissionLevel>())
      .def_property("name", &CommandClass::getName, nullptr)

      .def_static("getSoftEnumValues",
                  &DynamicCommandInstance::getSoftEnumValues)
      .def_static("getSoftEnumNames", &DynamicCommandInstance::getSoftEnumNames)

      .def("setEnum", &CommandClass::setEnum)
      .def("setAlias", &CommandClass::setAlias)
      .def("setSoftEnum", &CommandClass::setSoftEnum)
      .def("addSoftEnumValues", &CommandClass::addSoftEnumValues)
      .def("removeSoftEnumValues", &CommandClass::removeSoftEnumValues)
      .def("mandatory2",
           py::overload_cast<const string&, DynamicCommand::ParameterType,
                             string, string, CommandParameterOption>(
               &CommandClass::mandatory),
           "name"_a, "type"_a, "description"_a = "", "identifier"_a = "",
           "option"_a = CommandParameterOption::None)
      .def("mandatory",
           py::overload_cast<const string&, DynamicCommand::ParameterType,
                             string, CommandParameterOption>(
               &CommandClass::mandatory),
           "name"_a, "type"_a, "description"_a = "", "option"_a = CommandParameterOption::None)
      .def("optional2",
           py::overload_cast<const string&, DynamicCommand::ParameterType,
                             string, string, CommandParameterOption>(
               &CommandClass::optional),
           "name"_a, "type"_a, "description"_a = "", "identifier"_a = "",
           "option"_a = CommandParameterOption::None)
      .def("optional",
           py::overload_cast<const string&, DynamicCommand::ParameterType,
                             string, CommandParameterOption>(
               &CommandClass::optional),
           "name"_a, "type"_a, "description"_a = "", "option"_a = CommandParameterOption::None)
      .def("overload", py::overload_cast<>(&CommandClass::addOverload))
      .def("overload",
           py::overload_cast<const vector<size_t>&>(&CommandClass::addOverload))
      .def("overload",
           py::overload_cast<vector<string>&&>(&CommandClass::addOverload))
      .def("setCallback", &CommandClass::setCallback)
      .def("setup", &CommandClass::setup);

  py::class_<CommandOriginClass>(mc_module, "CommandOrigin")
      .def_property("type", &CommandOriginClass::getOriginType, nullptr)
      .def_property("type_name", &CommandOriginClass::getOriginTypeName,
                    nullptr)
      .def_property("name", &CommandOriginClass::getOriginName, nullptr)
      .def_property("pos", &CommandOriginClass::getPosition, nullptr)
      .def_property("blockpos", &CommandOriginClass::getBlockPosition, nullptr)
      .def_property("dim", &CommandOriginClass::getDim, nullptr)
      .def_property("entity", &CommandOriginClass::getEntity, nullptr)
      .def_property("player", &CommandOriginClass::getPlayer, nullptr)

      .def("getNbt", &CommandOriginClass::getNbt)
      .def("toString", &CommandOriginClass::toString);

  py::class_<CommandOutputClass>(mc_module, "CommandOutput")
      .def_property("empty", &CommandOutputClass::empty, nullptr)
      .def_property("success_count", &CommandOutputClass::getSuccessCount,
                    nullptr)

      .def("success", py::overload_cast<>(&CommandOutputClass::success))
      .def("success",
           py::overload_cast<const string&>(&CommandOutputClass::success))
      .def("error", &CommandOutputClass::error)
      .def("addMessage", &CommandOutputClass::addMessage)
      .def("toString", &CommandOutputClass::toString);

  py::class_<DynamicCommand::Result>(mc_module, "CommandResult")
      .def("__repr__",
           [](const DynamicCommand::Result& thiz) {
             return py::str(CastResult(thiz));
           })
      .def("toObject",
           [](const DynamicCommand::Result& thiz) { return CastResult(thiz); });

  py::class_<NBTClass>(mc_module, "NBT")
      .def_property("type", &NBTClass::getType, nullptr)
      .def_static("newByte", &NBTClass::newByte)
      .def_static("newShort", &NBTClass::newShort)
      .def_static("newInt", &NBTClass::newInt)
      .def_static("newLong", &NBTClass::newInt64)
      .def_static("newFloat", &NBTClass::newFloat)
      .def_static("newDouble", &NBTClass::newDouble)
      .def_static("newByteArray", &NBTClass::newByteArray)
      .def_static("newString", &NBTClass::newString)
      .def_static("newIntArray", &NBTClass::newIntArray)
      .def_static("newList", &NBTClass::newList)
      .def_static("newCompound", &NBTClass::newCompound)
      .def_static("fromSNBT", &NBTClass::fromSNBT)
      .def_static("fromBinary", &NBTClass::fromBinary)

      .def("__getitem__", py::overload_cast<int>(&NBTClass::__getitem__))
      .def("__getitem__",
           py::overload_cast<const string&>(&NBTClass::__getitem__))
      .def("__setitem__", &NBTClass::__setitem__)
      .def("asByte", &NBTClass::asByte)
      .def("asShort", &NBTClass::asShort)
      .def("asInt", &NBTClass::asInt)
      .def("asInt64", &NBTClass::asInt64)
      .def("asFloat", &NBTClass::asFloat)
      .def("asDouble", &NBTClass::asDouble)
      .def("asString", &NBTClass::asString)
      .def("toBinary", &NBTClass::toBinary)
      .def("toJson", &NBTClass::toJson, "indent"_a = 4)
      .def("toSNBT", &NBTClass::toSNBT, "indent"_a = 4,
           "format"_a = SnbtFormat::PartialNewLine)
      .def("append", &NBTClass::append);

  py::class_<Logger>(mc_module, "Logger")
      .def(py::init<string>(), "title"_a)
      .def("debug", [](Logger& l, const string& msg) { l.debug(msg); })
      .def("info", [](Logger& l, const string& msg) { l.info(msg); })
      .def("warn", [](Logger& l, const string& msg) { l.warn(msg); })
      .def("error", [](Logger& l, const string& msg) { l.error(msg); })
      .def("fatal", [](Logger& l, const string& msg) { l.fatal(msg); })

      .def("setTitle", [](Logger& l, const string& title) { l.title = title; })
      .def("setFile",
           [](Logger& l, const string& file, int level) {
             l.setFile(file, std::ios::app);
             l.fileLevel = level;
             return l.ofs.is_open();
           })
      .def("setPlayer", [](Logger& l, const PlayerClass& player, int level) {
        l.player = player.thiz;
        l.playerLevel = level;
      });

  py::class_<PlayerClass>(mc_module, "Player")
      .def_property("name", &PlayerClass::getName, nullptr)
      .def_property("pos", &PlayerClass::getPos, nullptr)
      .def_property("blockpos", &PlayerClass::getBlockPos, nullptr)
      .def_property("dim", &PlayerClass::getDim, nullptr)
      .def_property("real_name", &PlayerClass::getRealName, nullptr)
      .def_property("xuid", &PlayerClass::getXuid, nullptr)
      .def_property("uuid", &PlayerClass::getUuid, nullptr)
      .def_property("perm_level", &PlayerClass::getPermLevel, nullptr)
      .def_property("game_mode", &PlayerClass::getGameMode, nullptr)
      .def_property("max_health", &PlayerClass::getMaxHealth, nullptr)
      .def_property("health", &PlayerClass::getHealth, nullptr)
      .def_property("in_air", &PlayerClass::getInAir, nullptr)
      .def_property("in_water", &PlayerClass::getInWater, nullptr)
      .def_property("sneaking", &PlayerClass::getSneaking, nullptr)
      .def_property("speed", &PlayerClass::getSpeed, nullptr)
      .def_property("direction", &PlayerClass::getDirection, nullptr)
      .def_property("unique_id", &PlayerClass::getUniqueID, nullptr)
      .def_property("lang_code", &PlayerClass::getLangCode, nullptr)

      .def_copy(PlayerClass)
      .def("__repr__",
           [](PlayerClass& self) {
             return "Player(\"" + self.getName() + "\")";
           })
      .def_property("ip", &PlayerClass::getIP, nullptr)
      .def_property("avg_ping", &PlayerClass::getAvgPing, nullptr)
      .def_property("avg_packet_loss", &PlayerClass::getAvgPacketLoss, nullptr)
      .def_property("os", &PlayerClass::getOs, nullptr)
      .def_property("client_id", &PlayerClass::getClientId, nullptr)

      .def("isOP", &PlayerClass::isOP)
      .def("setPermLevel", &PlayerClass::setPermLevel)
      .def("setGameMode", &PlayerClass::setGameMode)

      .def("runcmd", &PlayerClass::runcmd)
      .def("teleport", &PlayerClass::teleport)
      .def("kill", &PlayerClass::kill)
      .def("kick", &PlayerClass::kick,
           "msg"_a = "Disconnecting from the server")
      .def("sendText", &PlayerClass::sendText, "msg"_a = "",
           "type"_a = TextType::RAW)
      .def("talkAs", &PlayerClass::talkAs)
      .def("rename", &PlayerClass::rename)
      .def("setOnFire", &PlayerClass::setOnFire)
      .def("transServer", &PlayerClass::transServer, "ip"_a, "port"_a = 19132)
      .def("crash", &PlayerClass::crash)
      .def("hurt", &PlayerClass::hurt)
      .def("refreshChunks", &PlayerClass::refreshChunks)
      .def("giveItem", &PlayerClass::giveItem)
      .def("clearItem", &PlayerClass::clearItem)
      .def("isSprinting", &PlayerClass::isSprinting)
      .def("setSprinting", &PlayerClass::setSprinting)

      .def("getBlockStandingOn", &PlayerClass::getBlockStandingOn)
      .def("getHand", &PlayerClass::getHand)
      .def("getOffHand", &PlayerClass::getOffHand)
      .def("getInventory", &PlayerClass::getInventory)
      .def("getArmor", &PlayerClass::getArmor)
      .def("getEnderChest", &PlayerClass::getEnderChest)
      .def("getRespawnPosition", &PlayerClass::getRespawnPosition)
      .def("setRespawnPosition", &PlayerClass::setRespawnPosition)
      .def("refreshItems", &PlayerClass::refreshItems)
      .def("getScore", &PlayerClass::getScore)
      .def("setScore", &PlayerClass::setScore)
      .def("addScore", &PlayerClass::addScore)
      .def("reduceScore", &PlayerClass::reduceScore)
      .def("deleteScore", &PlayerClass::deleteScore)
      .def("setSidebar", &PlayerClass::setSidebar)
      .def("removeSidebar", &PlayerClass::removeSidebar)
      .def("setBossBar", &PlayerClass::setBossBar)
      .def("removeBossBar", py::overload_cast<>(&PlayerClass::removeBossBar))
      .def("removeBossBar",
           py::overload_cast<int64_t>(&PlayerClass::removeBossBar))
      .def("addLevel", &PlayerClass::addLevel)
      .def("getLevel", &PlayerClass::getLevel)
      .def("resetLevel", &PlayerClass::resetLevel)
      .def("getXpNeededForNextLevel", &PlayerClass::getXpNeededForNextLevel)
      .def("addExperience", &PlayerClass::addExperience)

      .def("sendSimpleForm", &PlayerClass::sendSimpleForm)
      .def("sendModalForm", &PlayerClass::sendModalForm)
      .def("sendCustomForm", &PlayerClass::sendCustomForm)

      .def("setExtraData", &PlayerClass::setExtraData)
      .def("getExtraData", &PlayerClass::getExtraData)
      .def("delExtraData", &PlayerClass::delExtraData)

      .def("setNbt", &PlayerClass::setNbt)
      .def("getNbt", &PlayerClass::getNbt)
      .def("addTag", &PlayerClass::addTag)
      .def("removeTag", &PlayerClass::removeTag)
      .def("hasTag", &PlayerClass::hasTag)
      .def("getAllTags", &PlayerClass::getAllTags)
      .def("getEntityFromViewVector", &PlayerClass::getEntityFromViewVector,
           "max_distance"_a = 5.25f)
      .def("getBlockFromViewVector", &PlayerClass::getBlockFromViewVector,
           "includeLiquid"_a = false, "solidOnly"_a = false,
           "maxDistance"_a = 5.25f, "ignoreBorderBlocks"_a = true,
           "fullOnly"_a = false)

      .def("removeItem", &PlayerClass::removeItem)
      .def("setHeadShow", &PlayerClass::setHeadShow)

      // SimulatedPlayer API
      .def("isSimulatedPlayer", &PlayerClass::isSimulatedPlayer)
      .def("simulateSneak", &PlayerClass::simulateSneak)
      .def("simulateAttack", py::overload_cast<>(&PlayerClass::simulateAttack))
      .def("simulateAttack",
           py::overload_cast<const EntityClass&>(&PlayerClass::simulateAttack))
      .def("simulateDestory",
           py::overload_cast<>(&PlayerClass::simulateDestory))
      .def("simulateDestory",
           py::overload_cast<const BlockPos&,
                             ScriptModuleMinecraft::ScriptFacing>(
               &PlayerClass::simulateDestory))
      .def("simulateDisconnect", &PlayerClass::simulateDisconnect)
      .def("simulateInteract",
           py::overload_cast<>(&PlayerClass::simulateInteract))
      .def("simulateInteractEntity", py::overload_cast<const EntityClass&>(
                                         &PlayerClass::simulateInteract))
      .def("simulateInteractBlock",
           py::overload_cast<const BlockPos&,
                             ScriptModuleMinecraft::ScriptFacing>(
               &PlayerClass::simulateInteract))
      .def("simulateJump", &PlayerClass::simulateJump)
      .def("simulateLocalMove", &PlayerClass::simulateLocalMove, "target"_a,
           "speed"_a = 1.0f)
      .def("simulateWorldMove", &PlayerClass::simulateWorldMove, "target"_a,
           "speed"_a = 1.0f)
      .def("simulateMoveTo", &PlayerClass::simulateMoveTo, "target"_a,
           "speed"_a = 1.0f)
      .def("simulateLookAt",
           py::overload_cast<const EntityClass&>(&PlayerClass::simulateLookAt))
      .def("simulateLookAt",
           py::overload_cast<const Vec3&>(&PlayerClass::simulateLookAt))
      .def("simulateLookAt",
           py::overload_cast<const BlockPos&>(&PlayerClass::simulateLookAt))
      .def("simulateSetBodyRotation", &PlayerClass::simulateSetBodyRotation)
      .def("simulateNavigateTo",
           py::overload_cast<vector<Vec3>, float>(
               &PlayerClass::simulateNavigateTo),
           "path"_a, "speed"_a = 1.0f)
      .def("simulateNavigateTo",
           py::overload_cast<const Vec3&, float>(
               &PlayerClass::simulateNavigateTo),
           "pos"_a, "speed"_a = 1.0f)
      .def("simulateNavigateTo",
           py::overload_cast<const EntityClass&, float>(
               &PlayerClass::simulateNavigateTo),
           "target"_a, "speed"_a = 1.0f)
      .def("simulateUseItem",
           py::overload_cast<>(&PlayerClass::simulateUseItem))
      .def("simulateUseItem2",
           py::overload_cast<int>(&PlayerClass::simulateUseItem))
      .def("simulateUseItem3",
           py::overload_cast<const ItemClass&>(&PlayerClass::simulateUseItem))
      .def("simulateUseItem4",
           py::overload_cast<const ItemClass&, const BlockPos&,
                             ScriptModuleMinecraft::ScriptFacing, const Vec3&>(
               &PlayerClass::simulateUseItem),
           "item"_a, "pos"_a, "face"_a, "relativePos"_a = Vec3(0.5, 0.5, 0.5))
      .def("simulateUseItem5",
           py::overload_cast<int, const BlockPos&,
                             ScriptModuleMinecraft::ScriptFacing, const Vec3&>(
               &PlayerClass::simulateUseItem),
           "slot"_a, "pos"_a, "face"_a, "relativePos"_a = Vec3(0.5, 0.5, 0.5))
      .def("simulateStopDestroyingBlock",
           &PlayerClass::simulateStopDestroyingBlock)
      .def("simulateStopInteracting", &PlayerClass::simulateStopInteracting)
      .def("simulateStopMoving", &PlayerClass::simulateStopMoving)
      .def("simulateStopUsingItem", &PlayerClass::simulateStopUsingItem)
      .def("simulateStopSneaking", &PlayerClass::simulateStopSneaking);

  py::class_<EntityClass>(mc_module, "Entity")
      .def_property("name", &EntityClass::getName, nullptr)
      .def_property("type", &EntityClass::getType, nullptr)
      .def_property("id", &EntityClass::getId, nullptr)
      .def_property("pos", &EntityClass::getPos, nullptr)
      .def_property("blockpos", &EntityClass::getBlockPos, nullptr)
      .def_property("dim", &EntityClass::getDim, nullptr)
      .def_property("max_health", &EntityClass::getMaxHealth, nullptr)
      .def_property("health", &EntityClass::getHealth, nullptr)
      .def_property("in_air", &EntityClass::getInAir, nullptr)
      .def_property("in_water", &EntityClass::getInWater, nullptr)
      .def_property("speed", &EntityClass::getSpeed, nullptr)
      .def_property("direction", &EntityClass::getDirection, nullptr)
      .def_property("unique_id", &EntityClass::getUniqueID, nullptr)

      .def_copy(EntityClass)
      .def("__repr__",
           [](EntityClass& self) { return "Entity(" + self.getName() + ")"; })
      .def("teleport", &EntityClass::teleport)
      .def("kill", &EntityClass::kill)
      .def("hurt", &EntityClass::hurt)
      .def("setOnFire", &EntityClass::setOnFire)
      .def("isPlayer", &EntityClass::isPlayer)
      .def("toPlayer", &EntityClass::toPlayer)
      .def("isItemEntity", &EntityClass::isItemEntity)
      .def("toItem", &EntityClass::toItem)
      .def("getBlockStandingOn", &EntityClass::getBlockStandingOn)
      .def("getArmor", &EntityClass::getArmor)
      .def("hasContainer", &EntityClass::hasContainer)
      .def("getContainer", &EntityClass::getContainer)
      .def("refreshItems", &EntityClass::refreshItems)
      .def("setNbt", &EntityClass::setNbt)
      .def("getNbt", &EntityClass::getNbt)
      .def("addTag", &EntityClass::addTag)
      .def("removeTag", &EntityClass::removeTag)
      .def("hasTag", &EntityClass::hasTag)
      .def("getAllTags", &EntityClass::getAllTags)
      .def("getEntityFromViewVector", &EntityClass::getEntityFromViewVector)
      .def("getBlockFromViewVector", &EntityClass::getBlockFromViewVector);

  py::class_<BlockEntityClass>(mc_module, "BlockEntity")
      .def_property("pos", &BlockEntityClass::getPos, nullptr)
      .def_property("type", &BlockEntityClass::getType, nullptr)

      .def("setNbt", &BlockEntityClass::setNbt)
      .def("getNbt", &BlockEntityClass::getNbt)
      .def("getBlock", &BlockEntityClass::getBlock);

  py::class_<ObjectiveClass>(mc_module, "Objective")
      .def_property("name", &ObjectiveClass::getName, nullptr)
      .def_property("display_name", &ObjectiveClass::getDisplayName, nullptr)

      .def("setDisplay", &ObjectiveClass::setDisplay, "slot"_a, "sort"_a = 0)
      .def("setScore", &ObjectiveClass::setScore)
      .def("addScore", &ObjectiveClass::addScore)
      .def("reduceScore", &ObjectiveClass::reduceScore)
      .def("deleteScore", &ObjectiveClass::deleteScore)
      .def("getScore", &ObjectiveClass::getScore);

#pragma endregion
#pragma region Functions
  mc_module.def("getBbsVersion", &LL::getBdsVersion)
      .def("getServerProtocolVersion", &LL::getServerProtocolVersion)
      .def("runCommand", &Level::runcmd)
      .def("runCommandEx", &Level::runcmdEx)
      .def("setListener", &mc::setListener)
      .def("registerCommand", &mc::registerCommand, "name"_a, "desc"_a,
           "callback"_a, "perm"_a = CommandPermissionLevel::GameMasters)
      .def("getPlayer", &mc::getPlayer)
      .def("getOnlinePlayers", &mc::getOnlinePlayers)
      .def("getAllEntities", &mc::getAllEntities)
      .def("setServerMotd", &LL::setServerMotd)
      .def("broadcastText", &Level::broadcastText, "text"_a,
           "type"_a = TextType::RAW)
      .def("broadcastTitle", &Level::broadcastTitle, "text"_a,
           "type"_a = TitleType::SetTitle, "fade_in_duration"_a = 100,
           "remain_duration"_a = 500, "fade_out_duration"_a = 100)
      .def("getBlock", &mc::getBlock)
      .def("setBlock",
           py::overload_cast<const BlockPos&, int, const string&, int>(
               &mc::setBlock))
      .def("setBlock",
           py::overload_cast<const BlockPos&, int, const BlockClass&>(
               &mc::setBlock))
      .def("getStructure", &mc::getStructure, "pos1"_a, "pos2"_a, "dim"_a,
           "ignore_entities"_a = true, "ignore_blocks"_a = false)
      .def("setStructure", &mc::setStructure, "nbt"_a, "pos"_a, "dim"_a,
           "mir"_a = Mirror::None_15, "rot"_a = Rotation::None_14)
      .def("explode", &mc::explode)
      .def("spawnItem", &mc::spawnItem)
      .def("setSignBlockMessage", &mc::setSignBlockMessage)
      .def("isSlimeChunk", &mc::IsSlimeChunk)

      .def("spawnMob", &mc::spawnMob)
      .def("cloneMob", &mc::cloneMob)
      .def("spawnSimulatedPlayer", &mc::spawnSimulatedPlayer)
      .def("spawnParticle", &mc::spawnParticle)
      .def("setMotd", &LL::setServerMotd)
      .def("sendCmdOutput", &mc::sendCmdOutput)
      .def("getDisplayObjective", &mc::getDisplayObjective)
      .def("clearDisplayObjective", &mc::clearDisplayObjective)
      .def("getScoreObjective", &mc::getScoreObjective)
      .def("newScoreObjective", &mc::newScoreObjective)
      .def("removeScoreObjective", &mc::removeScoreObjective)
      .def("getAllScoreObjectives", &mc::getAllScoreObjectives)

      .def("setMaxPlayers", &mc::setMaxNumPlayers)

      .def("getMoney", &EconomySystem::getMoney)
      .def("setMoney", &EconomySystem::setMoney)
      .def("addMoney", &EconomySystem::addMoney)
      .def("reduceMoney", &EconomySystem::reduceMoney)
      .def("transMoney", &EconomySystem::transMoney)
      .def("getMoneyHist", &EconomySystem::getMoneyHist)
      .def("clearMoneyHist", &EconomySystem::clearMoneyHist);
#pragma endregion
  PY_CATCH;
}