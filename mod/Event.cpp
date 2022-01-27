#include "Event.h"
#include "Tool.h"
#include "Module.h"

using namespace std;
//事件回调助手，初始化对象将申请GIL
class Callbacker {
public:
	Callbacker(EventCode t) :
		type_(t), arg_(nullptr), gil_() {
	}
	~Callbacker() {
		Py_XDECREF(arg_);
	}
	//事件回调
	bool callback() {
		bool intercept = true;
		//如果没有则跳过
		auto& cbs = g_callback_functions[type_];
		for (auto cb : cbs) {
			PyObject* result = _PyObject_FastCall(cb, &arg_, 1);
			PrintPythonError();
			if (result == Py_False)
				intercept = false;
			Py_XDECREF(result);
		}
		return intercept;
	}
	Callbacker& setArg(PyObject* arg) {
		arg_ = arg;
		return *this;
	}
	Callbacker& insert(string_view key, PyObject* item) {
		if (arg_ == nullptr)
			arg_ = PyDict_New();
		PyDict_SetItemString(arg_, key.data(), item);
		Py_DECREF(item);
		return *this;
	}
	Callbacker& insert(string_view key, string_view item) {
		return insert(key, StrToPyUnicode(item));
	}
	Callbacker& insert(string_view key, Actor* item) {
		return insert(key, ToPyEntity(item));
	}
	Callbacker& insert(string_view key, ItemStack* item) {
		return insert(key, ToPyItemStack(item));
	}
	Callbacker& insert(string_view key, BlockPos* item) {
		return insert(key, ToList(item));
	}
	Callbacker& insert(string_view key, BlockPos& item) {
		return insert(key, ToList(&item));
	}
	Callbacker& insert(string_view key, BlockInstance& item) {
		return insert(key, ToBlockInstance(&item));
	}
	Callbacker& insert(string_view key, const Vec3& item) {
		return insert(key, ToList(item));
	}
	Callbacker& insert(string_view key, short item) {
		return insert(key, PyLong_FromLong(item));
	}
	Callbacker& insert(string_view key, int item) {
		return insert(key, PyLong_FromLong(item));
	}
	Callbacker& insert(string_view key, unsigned item) {
		return insert(key, PyLong_FromUnsignedLong(item));
	}
	Callbacker& insert(string_view key, long long item) {
		return insert(key, PyLong_FromLongLong(item));
	}
	Callbacker& insert(string_view key, unsigned long long item) {
		return insert(key, PyLong_FromUnsignedLongLong(item));
	}
	Callbacker& insert(string_view key, float item) {
		return insert(key, PyLong_FromDouble(item));
	}
private:
	EventCode type_;
	PyObject* arg_;
	PyGILGuard gil_;
};

static const std::unordered_map<string, EventCode> g_events{
	{ "onPreJoin", EventCode::onPreJoin },
	{ "onJoin", EventCode::onJoin },
	{ "onLeft", EventCode::onLeft },
	{ "onPlayerCmd", EventCode::onPlayerCmd },
	{ "onChat", EventCode::onChat },
	{ "onPlayerDie", EventCode::onPlayerDie },
	{ "onRespawn", EventCode::onRespawn },
	{ "onChangeDim", EventCode::onChangeDim },
	{ "onJump", EventCode::onJump },
	{ "onSneak", EventCode::onSneak },
	{ "onAttack", EventCode::onAttack },
	{ "onEat", EventCode::onEat },
	{ "onMove", EventCode::onMove },
	{ "onChangeSprinting", EventCode::onChangeSprinting },
	{ "onSpawnProjectile", EventCode::onSpawnProjectile },
	{ "onFireworkShootWithCrossbow", EventCode::onFireworkShootWithCrossbow },
	{ "onSetArmor", EventCode::onSetArmor },
	{ "onRide", EventCode::onRide },
	{ "onStepOnPressurePlate", EventCode::onStepOnPressurePlate },
	{ "onMobDie", EventCode::onMobDie },
	{ "onMobHurt", EventCode::onMobHurt },
	{ "onUseItem", EventCode::onUseItem },
	{ "onTakeItem", EventCode::onTakeItem },
	{ "onDropItem", EventCode::onDropItem },
	{ "onUseItemOn", EventCode::onUseItemOn },
	{ "onInventoryChange", EventCode::onInventoryChange },
	{ "onChangeArmorStand", EventCode::onChangeArmorStand },
	{ "onStartDestroyBlock", EventCode::onStartDestroyBlock },
	{ "onDestroyBlock", EventCode::onDestroyBlock },
	{ "onWitherBossDestroy", EventCode::onWitherBossDestroy },
	{ "onPlaceBlock", EventCode::onPlaceBlock },
	{ "onExplode", EventCode::onExplode },
	{ "onBedExplode", EventCode::onBedExplode },
	{ "onRespawnAnchorExplode", EventCode::onRespawnAnchorExplode },
	{ "onBlockExploded", EventCode::onBlockExploded },
	{ "onEntityExplode", EventCode::onEntityExplode },
	{ "onBlockExplode", EventCode::onBlockExplode },
	{ "onLiquidFlow", EventCode::onLiquidFlow },
	{ "onOpenContainer", EventCode::onOpenContainer },
	{ "onCloseContainer", EventCode::onCloseContainer },
	{ "onContainerChangeSlot", EventCode::onContainerChange },
	{ "onContainerChange", EventCode::onContainerChange },
	{ "onOpenContainerScreen", EventCode::onOpenContainerScreen },
	{ "onCmdBlockExecute", EventCode::onCmdBlockExecute },
	{ "onRedStoneUpdate", EventCode::onRedStoneUpdate },
	{ "onProjectileHitBlock", EventCode::onProjectileHitBlock },
	{ "onProjectileHitEntity", EventCode::onProjectileHitEntity },
	{ "onBlockInteracted", EventCode::onBlockInteracted },
	{ "onUseRespawnAnchor", EventCode::onUseRespawnAnchor },
	{ "onFarmLandDecay", EventCode::onFarmLandDecay },
	{ "onUseFrameBlock", EventCode::onUseFrameBlock },
	{ "onPistonPush", EventCode::onPistonPush },
	{ "onHopperSearchItem", EventCode::onHopperSearchItem },
	{ "onHopperPushOut", EventCode::onHopperPushOut },
	{ "onFireSpread", EventCode::onFireSpread },
	{ "onBlockChanged", EventCode::onBlockChanged },
	{ "onNpcCmd", EventCode::onNpcCmd },
	{ "onScoreChanged", EventCode::onScoreChanged },
	{ "onServerStarted", EventCode::onServerStarted },
	{ "onConsoleCmd", EventCode::onConsoleCmd },
	{ "onConsoleOutput", EventCode::onConsoleOutput },
	{ "onTick", EventCode::onTick },
	{ "onMoneyAdd", EventCode::onMoneyAdd },
	{ "onMoneyReduce", EventCode::onMoneyReduce },
	{ "onMoneyTrans", EventCode::onMoneyTrans },
	{ "onMoneySet", EventCode::onMoneySet },
	{ "onFormSelected", EventCode::onFormSelected },
	{ "onConsumeTotem", EventCode::onConsumeTotem },
	{ "onEffectAdded", EventCode::onEffectAdded },
	{ "onEffectRemoved", EventCode::onEffectRemoved },
	{ "onEffectUpdated", EventCode::onEffectUpdated }
};

std::optional<EventCode> StringToEventCode(const std::string& s) {
	auto x = g_events.find(s);
	if (x == g_events.end())
		return nullopt;
	else
		return x->second;
}

#define EVENT_BEGIN(evt) if(!evt::hasListener())evt::subscribe([code](evt e){Callbacker h(code); h.insert("Event",#evt)
#define EVENT_INSERT(key) h.insert(#key, e.m##key)
#define EVENT_INSERT2(key,value) h.insert(#key, value)
#define EVENT_END return h.callback();})

void EnableEventListener(EventCode code) {
	using namespace Event;
	switch (code) {
	case EventCode::onPreJoin:
		EVENT_BEGIN(PlayerPreJoinEvent);
		EVENT_INSERT(IP);
		EVENT_INSERT(Player);
		EVENT_INSERT(XUID);
		EVENT_END;
		break;
	case EventCode::onJoin:
		EVENT_BEGIN(PlayerJoinEvent);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onLeft:
		EVENT_BEGIN(PlayerLeftEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(XUID);
		EVENT_END;
		break;
	case EventCode::onPlayerCmd:
		EVENT_BEGIN(PlayerCmdEvent);
		EVENT_INSERT(Command);
		EVENT_INSERT(Player);
		EVENT_INSERT2(isSuccess, e.mResult->isSuccess());
		EVENT_END;
		break;
	case EventCode::onChat:
		EVENT_BEGIN(PlayerChatEvent);
		EVENT_INSERT(Message);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onPlayerDie:
		EVENT_BEGIN(PlayerDieEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT2(Cause, int(e.mDamageSource->getCause()));
		EVENT_INSERT2(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onRespawn:
		EVENT_BEGIN(PlayerRespawnEvent);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onChangeDim:
		EVENT_BEGIN(PlayerChangeDimEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(ToDimensionId);
		EVENT_END;
		break;
	case EventCode::onJump:
		EVENT_BEGIN(PlayerJumpEvent);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onSneak:
		EVENT_BEGIN(PlayerSneakEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(IsSneaking);
		EVENT_END;
		break;
	case EventCode::onAttack:
		EVENT_BEGIN(PlayerAttackEvent);
		EVENT_INSERT(AttackDamage);
		EVENT_INSERT(Player);
		EVENT_INSERT(Target);
		EVENT_END;
		break;
	case EventCode::onEat:
		EVENT_BEGIN(PlayerEatEvent);
		EVENT_INSERT(FoodItem);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onMove:
		EVENT_BEGIN(PlayerMoveEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(Pos);
		EVENT_END;
		break;
	case EventCode::onChangeSprinting:
		EVENT_BEGIN(PlayerSprintEvent);
		EVENT_INSERT(IsSprinting);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onSpawnProjectile:
		EVENT_BEGIN(ProjectileSpawnEvent);
		//EVENT_INSERT(Identifier);
		EVENT_INSERT(Shooter);
		EVENT_INSERT(Type);
		EVENT_END;
		break;
	case EventCode::onFireworkShootWithCrossbow:
		break;
	case EventCode::onSetArmor:
		EVENT_BEGIN(PlayerSetArmorEvent);
		EVENT_INSERT(ArmorItem);
		EVENT_INSERT(Player);
		EVENT_INSERT(Slot);
		EVENT_END;
		break;
	case EventCode::onRide:
		EVENT_BEGIN(EntityRideEvent);
		EVENT_INSERT(Rider);
		EVENT_INSERT(Vehicle);
		EVENT_END;
		break;
	case EventCode::onStepOnPressurePlate:
		break;
	case EventCode::onUseItem:
		EVENT_BEGIN(PlayerUseItemEvent);
		EVENT_INSERT(ItemStack);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onTakeItem:
		EVENT_BEGIN(PlayerTakeItemEvent);
		EVENT_INSERT(ItemEntity);
		EVENT_INSERT(ItemStack);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onDropItem:
		EVENT_BEGIN(PlayerDropItemEvent);
		EVENT_INSERT(ItemStack);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onUseItemOn:
		EVENT_BEGIN(PlayerUseItemOnEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(ItemStack);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onInventoryChange:
		EVENT_BEGIN(PlayerInventoryChangeEvent);
		EVENT_INSERT(NewItemStack);
		EVENT_INSERT(Player);
		EVENT_INSERT(PreviousItemStack);
		EVENT_INSERT(Slot);
		EVENT_END;
		break;
	case EventCode::onChangeArmorStand:
		EVENT_BEGIN(ArmorStandChangeEvent);
		//EVENT_INSERT(ArmorStand); todo
		EVENT_INSERT(Player);
		EVENT_INSERT(Slot);
		EVENT_END;
		break;
	case EventCode::onStartDestroyBlock:
		EVENT_BEGIN(PlayerStartDestroyBlockEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onDestroyBlock:
		EVENT_BEGIN(PlayerDestroyBlockEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onWitherBossDestroy:
		break;
	case EventCode::onPlaceBlock:
		EVENT_BEGIN(PlayerPlaceBlockEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onLiquidFlow:
		break;
	case EventCode::onOpenContainer:
		EVENT_BEGIN(PlayerOpenContainerEvent);
		// todo container
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onCloseContainer:
		EVENT_BEGIN(PlayerCloseContainerEvent);
		//todo
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onContainerChange:
		break;
	case EventCode::onOpenContainerScreen:
		break;
	case EventCode::onExplode:
		break;
	case EventCode::onBlockExploded:
		break;
	case EventCode::onBedExplode:
		break;
	case EventCode::onRespawnAnchorExplode:
		break;
	case EventCode::onEntityExplode:
		break;
	case EventCode::onBlockExplode:
		EVENT_BEGIN(BlockExplodeEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Breaking);
		EVENT_INSERT(Fire);
		EVENT_INSERT(MaxResistance);
		EVENT_INSERT(Radius);
		EVENT_END;
		break;
	case EventCode::onMobDie:
		EVENT_BEGIN(MobDieEvent);
		EVENT_INSERT(Mob);
		EVENT_INSERT2(Cause, int(e.mDamageSource->getCause()));
		EVENT_INSERT2(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onMobHurt:
		EVENT_BEGIN(MobDieEvent);
		EVENT_INSERT(Mob);
		EVENT_INSERT2(Cause, int(e.mDamageSource->getCause()));
		EVENT_INSERT2(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onCmdBlockExecute:
		break;
	case EventCode::onRedStoneUpdate:
		break;
	case EventCode::onProjectileHitEntity:
		break;
	case EventCode::onProjectileHitBlock:
		break;
	case EventCode::onBlockInteracted:
		break;
	case EventCode::onUseRespawnAnchor:
		break;
	case EventCode::onFarmLandDecay:
		break;
	case EventCode::onUseFrameBlock:
		break;
	case EventCode::onPistonPush:
		EVENT_BEGIN(PistonPushEvent);
		EVENT_INSERT(PistonBlockInstance);
		EVENT_INSERT(TargetBlockInstance);
		EVENT_END;
		break;
	case EventCode::onHopperSearchItem:
		EVENT_BEGIN(HopperSearchItemEvent);
		EVENT_INSERT(DimensionId);
		EVENT_INSERT(HopperBlock);
		EVENT_INSERT(MinecartPos);
		EVENT_END;
		break;
	case EventCode::onHopperPushOut:
		EVENT_BEGIN(HopperPushOutEvent);
		EVENT_INSERT(DimensionId);
		EVENT_INSERT(Pos);
		EVENT_END;
		break;
	case EventCode::onFireSpread:
		break;
	case EventCode::onBlockChanged:
		break;
	case EventCode::onNpcCmd:
		EVENT_BEGIN(NpcCmdEvent);
		EVENT_INSERT(Command);
		EVENT_INSERT(Npc);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onScoreChanged:
		EVENT_BEGIN(PlayerScoreChangedEvent);
		EVENT_INSERT(Player);
		//todo
		EVENT_END;
		break;
	case EventCode::onServerStarted:
		EVENT_BEGIN(ServerStartedEvent);
		e;
		EVENT_END;
		break;
	case EventCode::onConsoleCmd:
		EVENT_BEGIN(ConsoleCmdEvent);
		EVENT_INSERT(Command);
		EVENT_END;
		break;
	case EventCode::onFormSelected:
		break;
	case EventCode::onConsoleOutput:
		EVENT_BEGIN(ConsoleOutputEvent);
		EVENT_INSERT(Output);
		EVENT_END;
		break;
	case EventCode::onTick:
		break;
	case EventCode::onMoneyAdd:
		break;
	case EventCode::onMoneyReduce:
		break;
	case EventCode::onMoneyTrans:
		break;
	case EventCode::onMoneySet:
		break;
	case EventCode::onConsumeTotem:
		break;
	case EventCode::onEffectAdded:
		break;
	case EventCode::onEffectUpdated:
		break;
	case EventCode::onEffectRemoved:
		break;
	case EventCode::EVENT_COUNT:
		break;
	default:
		break;
	}
}
