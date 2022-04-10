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
#include <MC/BlockLegacy.hpp>
#include <MC/BlockPalette.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Common.hpp>
#include <MC/Container.hpp>
#include <MC/ItemInstance.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>
#include <MC/MobEffect.hpp>
#include <MC/Objective.hpp>
#include <MC/Player.hpp>
#include <MC/RakNet.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/ServerNetworkHandler.hpp>
#include <MC/SignBlockActor.hpp>
#include <MC/SimpleContainer.hpp>
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

namespace py = pybind11;
using json_t = nlohmann::detail::value_t;

#pragma region Version
constexpr unsigned PYR_VERSION_MAJOR = 1;
constexpr unsigned PYR_VERSION_MINOR = 9;
constexpr unsigned PYR_VERSION_MICRO = 10;
constexpr const char* PYR_VERSION = "v1.9.10";
#pragma endregion
//全局Logger
inline Logger logger("BDSpyrunner");
//输出错误信息
void PrintError(const std::exception&);
//判断指针是否为player
bool IsPlayer(Actor* ptr);
//字符串转JSON
fifo_json StrToJson(std::string_view str);
//事件代码
enum class EventCode {
	onPreJoin,
	onJoin,
	onLeft,
	onPlayerCmd,
	onChat,
	onPlayerDie,
	onRespawn,
	onChangeDim,
	onJump,
	onSneak,
	onAttack,
	onEat,
	onMove,
	onChangeSprinting,
	onSpawnProjectile,
	onFireworkShootWithCrossbow,
	onSetArmor,
	onRide,
	onStepOnPressurePlate,
	onUseItem,
	onPickupItem,
	onDropItem,
	onUseItemOn,
	onInventoryChange,
	onChangeArmorStand,
	onStartDestroyBlock,
	onDestroyBlock,
	onWitherBossDestroy,
	onPlaceBlock,
	onLiquidFlow,
	onOpenContainer,
	onCloseContainer,
	onContainerChange,
	onOpenContainerScreen,
	onExplode,
	onBlockExploded,
	onBedExplode,
	onRespawnAnchorExplode,
	onEntityExplode,
	onBlockExplode,
	onMobDie,
	onMobHurt,
	onCmdBlockExecute,
	onRedStoneUpdate,
	onProjectileHitEntity,
	onProjectileHitBlock,
	onBlockInteracted,
	onUseRespawnAnchor,
	onFarmLandDecay,
	onUseFrameBlock,
	onPistonPush,
	onHopperSearchItem,
	onHopperPushOut,
	onFireSpread,
	onBlockChanged,
	onNpcCmd,
	onScoreChanged,
	onServerStarted,
	onConsoleCmd,
	onConsoleOutput,
	onConsumeTotem,
	onEffectChanged,
};
//启用监听器
void EnableEventListener(EventCode e);
//Py函数表
inline std::unordered_map<EventCode, std::vector<py::function>> g_cb_functions;
//注册命令
inline std::unordered_map<std::string, std::pair<std::string, py::function>> g_commands;
//class PyGILGuard {
//public:
//	PyGILGuard() { gil_ = PyGILState_Ensure(); }
//	~PyGILGuard() { PyGILState_Release(gil_); }
//
//private:
//	PyGILState_STATE gil_;
//};
