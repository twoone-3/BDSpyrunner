#include "Event.h"
#include "Common.h"
#include "Module.h"

using namespace std;
//事件回调，初始化对象将申请GIL
class Callbacker {
public:
	Callbacker(EventCode t) {
		type_ = t;
		arg_ = nullptr;
	}
	~Callbacker() {
	}
	//事件回调
	bool callback() {
		bool pass = true;
		//如果没有则跳过
		auto& cbs = g_callback_functions[type_];
		for (auto cb : cbs) {
			PyCaller pc;
			pass = pc.call(cb, arg_) != Py_False;
		}
		return pass;
	}
	template<typename Arg>
	Callbacker& insert(string_view key, Arg item) {
		if (arg_ == nullptr)
			arg_ = PyDict_New();
		PyObject* obj = ToPyObject(item);
		PyDict_SetItemString(arg_, key.data(), obj);
		Py_DECREF(obj);
		return *this;
	}
private:
	EventCode type_;
	PyObject* arg_;
	PyGILGuard gil_;
};

#define EVENT_BEGIN(evt) evt::subscribe([code](evt e){Callbacker h(code); h.insert("Event",magic_enum::enum_name(code))
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
		EVENT_INSERT2(Cause, magic_enum::enum_name(e.mDamageSource->getCause()));
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
		EVENT_BEGIN(WitherBossDestroyEvent);
		//TODO: AABB and WitherBoss
		EVENT_END;
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
		EVENT_INSERT(BlockInstance);
		//TODO: EVENT_INSERT(Container);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onCloseContainer:
		EVENT_BEGIN(PlayerCloseContainerEvent);
		EVENT_INSERT(BlockInstance);
		//TODO: EVENT_INSERT(Container);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onContainerChange:
		EVENT_BEGIN(ContainerChangeEvent);
		EVENT_INSERT(Actor);
		EVENT_INSERT(BlockInstance);
		//TODO: EVENT_INSERT(Container);
		EVENT_INSERT(NewItemStack);
		EVENT_INSERT(Player);
		EVENT_INSERT(PreviousItemStack);
		EVENT_INSERT(Slot);
		EVENT_END;
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
		EVENT_INSERT2(Cause, magic_enum::enum_name(e.mDamageSource->getCause()));
		EVENT_INSERT2(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onMobHurt:
		EVENT_BEGIN(MobHurtEvent);
		EVENT_INSERT(Mob);
		EVENT_INSERT2(Cause, magic_enum::enum_name(e.mDamageSource->getCause()));
		EVENT_INSERT2(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onCmdBlockExecute:
		EVENT_BEGIN(CmdBlockExecuteEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Command);
		EVENT_INSERT(IsMinecart);
		EVENT_INSERT(Minecart);
		EVENT_END;
		break;
	case EventCode::onRedStoneUpdate:
		EVENT_BEGIN(RedStoneUpdateEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(IsActivated);
		EVENT_INSERT(RedStonePower);
		EVENT_END;
		break;
	case EventCode::onProjectileHitEntity:
		EVENT_BEGIN(ProjectileHitEntityEvent);
		EVENT_INSERT(Source);
		EVENT_INSERT(Target);
		EVENT_END;
		break;
	case EventCode::onProjectileHitBlock:
		EVENT_BEGIN(ProjectileHitBlockEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Source);
		EVENT_END;
		break;
	case EventCode::onBlockInteracted:
		EVENT_BEGIN(BlockInteractedEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onUseRespawnAnchor:
		EVENT_BEGIN(PlayerUseRespawnAnchorEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onFarmLandDecay:
		EVENT_BEGIN(FarmLandDecayEvent);
		EVENT_INSERT(Actor);
		EVENT_INSERT(BlockInstance);
		EVENT_END;
		break;
	case EventCode::onUseFrameBlock:
		EVENT_BEGIN(PlayerUseFrameBlockEvent);
		EVENT_INSERT(BlockInstance);
		EVENT_INSERT(Player);
		EVENT_INSERT2(Type, magic_enum::enum_name(e.mType));
		EVENT_END;
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
		EVENT_BEGIN(FireSpreadEvent);
		EVENT_INSERT(DimensionId);
		EVENT_INSERT(Target);
		EVENT_END;
		break;
	case EventCode::onBlockChanged:
		EVENT_BEGIN(BlockChangedEvent);
		EVENT_INSERT(NewBlockInstance);
		EVENT_INSERT(PreviousBlockInstance);
		EVENT_END;
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
		EVENT_INSERT2(ObjectiveName, e.mObjective->getName());
		EVENT_INSERT(Player);
		EVENT_INSERT(Score);
		//TODO: more info about Objective
		EVENT_END;
		break;
	case EventCode::onServerStarted:
		EVENT_BEGIN(ServerStartedEvent);
		EVENT_END;
		break;
	case EventCode::onConsoleCmd:
		EVENT_BEGIN(ConsoleCmdEvent);
		EVENT_INSERT(Command);
		EVENT_END;
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
	case EventCode::onEffectChanged:
		EVENT_BEGIN(PlayerEffectChangedEvent);
		//TODO: more info about Effect
		EVENT_INSERT2(Effect, e.mEffect->getDisplayName());
		EVENT_INSERT(Player);
		EVENT_INSERT2(Type, magic_enum::enum_name(e.mEventType));
		EVENT_END;
		break;
	default:
		break;
	}
}
