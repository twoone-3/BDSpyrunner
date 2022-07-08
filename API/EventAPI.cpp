#include <Global.hpp>
#include "EventAPI.h"
#include "McAPI.h"
#include "BlockAPI.h"
#include "CommandAPI.h"
#include "ContainerAPI.h"
#include "EntityAPI.h"
#include "ItemAPI.h"
#include "PlayerAPI.h"
#include "ScoreboardAPI.h"
#include <EventAPI.h>
#include <MC/ActorDamageSource.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Mob.hpp>

using namespace std;

class Callbacker {
public:
	Callbacker(EventCode t) : type_(t), arg_() {}
	//事件回调
	bool callback() {
		bool pass = true;
		arg_.inc_ref(); // TODO: 为什么需要增加引用计数？
		for (auto& cb : listeners[type_]) {
			PY_TRY;
			py::gil_scoped_acquire gil_;
			pass = bool(cb(arg_));
			PY_CATCH;
		}
		return pass;
	}
	template <typename T>
	Callbacker& insert(const char* key, const T& item) {
		arg_[key] = item;
		return *this;
	}
	Callbacker& insert(const char* key, const BlockInstance& item) {
		arg_[key] = BlockClass(item);
		return *this;
	}
	Callbacker& insert(const char* key, ItemStack* item) {
		arg_[key] = ItemClass(item);
		return *this;
	}
	Callbacker& insert(const char* key, Player* item) {
		arg_[key] = PlayerClass(item);
		return *this;
	}
	Callbacker& insert(const char* key, Actor* item) {
		arg_[key] = EntityClass(item);
		return *this;
	}
	Callbacker& insert(const char* key, Container* item) {
		arg_[key] = ContainerClass(item);
		return *this;
	}

private:
	EventCode type_;
	py::dict arg_;
	py::gil_scoped_acquire gil_;
};

void EnableEventListener(EventCode event_code) {
	using namespace Event;

	switch (event_code) {
	case EventCode::onJoin:
		Event::PlayerJoinEvent::subscribe([](const PlayerJoinEvent& ev) {
			EVENT_BEGIN(EventCode::onJoin);
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onPreJoin:
		Event::PlayerPreJoinEvent::subscribe([](const PlayerPreJoinEvent& ev) {
			EVENT_BEGIN(EventCode::onPreJoin);
			EVENT_INSERT(IP);
			EVENT_INSERT(Player);
			EVENT_INSERT(XUID);
			EVENT_END;
		});
		break;

	case EventCode::onLeft:
		Event::PlayerLeftEvent::subscribe([](const PlayerLeftEvent& ev) {
			EVENT_BEGIN(EventCode::onLeft);
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onChat:
		Event::PlayerChatEvent::subscribe([](const PlayerChatEvent& ev) {
			EVENT_BEGIN(EventCode::onChat);
			EVENT_INSERT(Player);
			EVENT_INSERT(Message);
			EVENT_END;
		});
		break;

	case EventCode::onChangeDim:
		Event::PlayerChangeDimEvent::subscribe([](const PlayerChangeDimEvent& ev) {
			EVENT_BEGIN(EventCode::onChangeDim);
			EVENT_INSERT(Player);
			EVENT_INSERT(ToDimensionId);
			EVENT_END;
		});
		break;

	case EventCode::onAttackEntity:
		Event::PlayerAttackEvent::subscribe([](const PlayerAttackEvent& ev) {
			EVENT_BEGIN(EventCode::onAttackEntity);
			EVENT_INSERT(Player);
			EVENT_INSERT(Target);
			EVENT_END;
		});
		break;

	case EventCode::onAttackBlock:
		Event::PlayerAttackBlockEvent::subscribe([](const PlayerAttackBlockEvent& ev) {
			EVENT_BEGIN(EventCode::onAttackBlock);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(ItemStack);
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onPlayerDie:
		Event::PlayerDieEvent::subscribe([](const PlayerDieEvent& ev) {
			EVENT_BEGIN(EventCode::onPlayerDie);
			EVENT_INSERT_EX(Cause, magic_enum::enum_name(ev.mDamageSource->getCause()));
			EVENT_INSERT_EX(Entity, EntityClass(ev.mDamageSource->getEntity()));
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onRespawn:
		Event::PlayerRespawnEvent::subscribe([](const PlayerRespawnEvent& ev) {
			EVENT_BEGIN(EventCode::onRespawn);
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onStartDestroyBlock:
		Event::PlayerStartDestroyBlockEvent::subscribe([](const PlayerStartDestroyBlockEvent& ev) {
			EVENT_BEGIN(EventCode::onStartDestroyBlock);
			EVENT_INSERT(Player);
			EVENT_INSERT(BlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onDestroyBlock:
		Event::PlayerDestroyBlockEvent::subscribe([](const PlayerDestroyBlockEvent& ev) {
			EVENT_BEGIN(EventCode::onDestroyBlock);
			EVENT_INSERT(Player);
			EVENT_INSERT(BlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onPlaceBlock:
		Event::PlayerPlaceBlockEvent::subscribe([](const PlayerPlaceBlockEvent& ev) {
			EVENT_BEGIN(EventCode::onPlaceBlock);
			EVENT_INSERT(Player);
			EVENT_INSERT(BlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onMove:
		Event::PlayerMoveEvent::subscribe([](const PlayerMoveEvent& ev) {
			EVENT_BEGIN(EventCode::onMove);
			EVENT_INSERT(Player);
			EVENT_INSERT(Pos);
			EVENT_END;
		});
		break;

	case EventCode::onJump:
		Event::PlayerJumpEvent::subscribe([](const PlayerJumpEvent& ev) {
			EVENT_BEGIN(EventCode::onJump);
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onDropItem:
		Event::PlayerDropItemEvent::subscribe([](const PlayerDropItemEvent& ev) {
			EVENT_BEGIN(EventCode::onDropItem);
			EVENT_INSERT(Player);
			EVENT_INSERT(ItemStack);
			EVENT_END;
		});
		break;

	case EventCode::onTakeItem:
		Event::PlayerPickupItemEvent::subscribe([](const PlayerPickupItemEvent& ev) {
			EVENT_BEGIN(EventCode::onTakeItem);
			EVENT_INSERT(Player);
			EVENT_INSERT(ItemEntity);
			EVENT_INSERT(ItemStack);
			EVENT_END;
		});
		break;

	case EventCode::onOpenContainer:
		Event::PlayerOpenContainerEvent::subscribe([](const PlayerOpenContainerEvent& ev) {
			EVENT_BEGIN(EventCode::onOpenContainer);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Player);
			EVENT_INSERT(Container);
			EVENT_END;
		});
		break;

	case EventCode::onCloseContainer:
		Event::PlayerCloseContainerEvent::subscribe([](const PlayerCloseContainerEvent& ev) {
			EVENT_BEGIN(EventCode::onCloseContainer);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Player);
			EVENT_INSERT(Container);
			EVENT_END;
		});
		break;

	case EventCode::onInventoryChange:
		Event::PlayerInventoryChangeEvent::subscribe([](const PlayerInventoryChangeEvent& ev) {
			EVENT_BEGIN(EventCode::onInventoryChange);
			EVENT_INSERT(NewItemStack);
			EVENT_INSERT(Player);
			EVENT_INSERT(PreviousItemStack);
			EVENT_INSERT(Slot);
			EVENT_END;
		});
		break;

	case EventCode::onUseItem:
		Event::PlayerUseItemEvent::subscribe([](const PlayerUseItemEvent& ev) {
			EVENT_BEGIN(EventCode::onUseItem);
			EVENT_INSERT(Player);
			EVENT_INSERT(ItemStack);
			EVENT_END;
		});
		break;

	case EventCode::onUseItemOn:
		Event::PlayerUseItemOnEvent::subscribe([](const PlayerUseItemOnEvent& ev) {
			EVENT_BEGIN(EventCode::onUseItemOn);
			EVENT_INSERT(Player);
			EVENT_INSERT(ItemStack);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Face);
			EVENT_END;
		});
		break;

	case EventCode::onContainerChange:
		Event::ContainerChangeEvent::subscribe([](const ContainerChangeEvent& ev) {
			EVENT_BEGIN(EventCode::onContainerChange);
			EVENT_INSERT(Player);
			EVENT_INSERT(Actor);
			EVENT_INSERT(Container);
			EVENT_INSERT(Slot);
			EVENT_INSERT(NewItemStack);
			EVENT_INSERT(PreviousItemStack);
			EVENT_END;
		});
		break;

	case EventCode::onChangeArmorStand:
		Event::ArmorStandChangeEvent::subscribe([](const ArmorStandChangeEvent& ev) {
			EVENT_BEGIN(EventCode::onChangeArmorStand);
			EVENT_INSERT(Player);
			EVENT_INSERT_EX(ArmorStand, EntityClass((Actor*)ev.mArmorStand));
			EVENT_INSERT(Slot);
			EVENT_END;
		});
		break;

	case EventCode::onChangeSprinting:
		Event::PlayerSprintEvent::subscribe([](const PlayerSprintEvent& ev) {
			EVENT_BEGIN(EventCode::onChangeSprinting);
			EVENT_INSERT(Player);
			EVENT_INSERT(IsSprinting);
			EVENT_END;
		});
		break;

	case EventCode::onSneak:
		Event::PlayerSneakEvent::subscribe([](const PlayerSneakEvent& ev) {
			EVENT_BEGIN(EventCode::onSneak);
			EVENT_INSERT(Player);
			EVENT_INSERT(IsSneaking);
			EVENT_END;
		});
		break;

	case EventCode::onOpenContainerScreen:
		Event::PlayerOpenContainerScreenEvent::subscribe([](const PlayerOpenContainerScreenEvent& ev) {
			EVENT_BEGIN(EventCode::onOpenContainerScreen);
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onSetArmor:
		Event::PlayerSetArmorEvent::subscribe([](const PlayerSetArmorEvent& ev) {
			EVENT_BEGIN(EventCode::onSetArmor);
			EVENT_INSERT(Player);
			EVENT_INSERT(Slot);
			EVENT_INSERT(ArmorItem);
			EVENT_END;
		});
		break;

	case EventCode::onEat:
		Event::PlayerEatEvent::subscribe([](const PlayerEatEvent& ev) {
			EVENT_BEGIN(EventCode::onEat);
			EVENT_INSERT(Player);
			EVENT_INSERT(FoodItem);
			EVENT_END;
		});
		break;

	case EventCode::onConsumeTotem:
		Event::PlayerConsumeTotemEvent::subscribe([](const PlayerConsumeTotemEvent& ev) {
			EVENT_BEGIN(EventCode::onConsumeTotem);
			EVENT_INSERT(Player);
			EVENT_END;
		});
		break;

	case EventCode::onEffectChanged:
		Event::PlayerEffectChangedEvent::subscribe([](const PlayerEffectChangedEvent& ev) {
			EVENT_BEGIN(EventCode::onEffectChanged);
			EVENT_INSERT(Player);
			EVENT_INSERT(Effect);
			EVENT_INSERT(EventType);
			EVENT_END;
		});
		break;

	case EventCode::onUseRespawnAnchor:
		Event::PlayerUseRespawnAnchorEvent::subscribe([](const PlayerUseRespawnAnchorEvent& ev) {
			EVENT_BEGIN(EventCode::onUseRespawnAnchor);
			EVENT_INSERT(Player);
			EVENT_INSERT(BlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onRide:
		Event::EntityRideEvent::subscribe([](const EntityRideEvent& ev) {
			EVENT_BEGIN(EventCode::onRide);
			EVENT_INSERT(Rider);
			EVENT_INSERT(Vehicle);
			EVENT_END;
		});
		break;

	case EventCode::onEntityExplode:
		Event::EntityExplodeEvent::subscribe([](const EntityExplodeEvent& ev) {
			EVENT_BEGIN(EventCode::onEntityExplode);
			EVENT_INSERT(Actor);
			EVENT_INSERT(Pos);
			EVENT_INSERT_EX(Dim, ev.mDimension->getDimensionId());
			EVENT_INSERT(Radius);
			EVENT_INSERT(MaxResistance);
			EVENT_INSERT(Breaking);
			EVENT_INSERT(Fire);
			EVENT_END;
		});
		break;

	case EventCode::onBlockExplode:
		Event::BlockExplodeEvent::subscribe([](const BlockExplodeEvent& ev) {
			EVENT_BEGIN(EventCode::onBlockExplode);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Radius);
			EVENT_INSERT(MaxResistance);
			EVENT_INSERT(Breaking);
			EVENT_INSERT(Fire);
			EVENT_END;
		});
		break;

	case EventCode::onBlockExploded:
		Event::BlockExplodedEvent::subscribe([](const BlockExplodedEvent& ev) {
			EVENT_BEGIN(EventCode::onBlockExploded);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(ExplodeSource);
			EVENT_END;
		});
		break;

	case EventCode::onCmdBlockExecute:
		Event::CmdBlockExecuteEvent::subscribe([](const CmdBlockExecuteEvent& ev) {
			EVENT_BEGIN(EventCode::onCmdBlockExecute);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Command);
			EVENT_INSERT(IsMinecart);
			EVENT_INSERT(Minecart);
			EVENT_END;
		});
		break;

	case EventCode::onRedStoneUpdate:
		Event::RedStoneUpdateEvent::subscribe([](const RedStoneUpdateEvent& ev) {
			EVENT_BEGIN(EventCode::onRedStoneUpdate);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(RedStonePower);
			EVENT_INSERT(IsActivated);
			EVENT_END;
		});
		break;

	case EventCode::onWitherBossDestroy:
		Event::WitherBossDestroyEvent::subscribe([](const WitherBossDestroyEvent& ev) {
			EVENT_BEGIN(EventCode::onWitherBossDestroy);
			EVENT_INSERT_EX(WitherBoss, EntityClass((Actor*)ev.mWitherBoss));
			EVENT_INSERT_EX(PointA, ev.mDestroyRange.pointA);
			EVENT_INSERT_EX(PointB, ev.mDestroyRange.pointB);
			EVENT_END;
		});
		break;

	case EventCode::onMobHurt:
		Event::MobHurtEvent::subscribe([](const MobHurtEvent& ev) {
			EVENT_BEGIN(EventCode::onMobHurt);
			EVENT_INSERT_EX(Mob, EntityClass(ev.mMob));
			EVENT_INSERT(Damage);
			EVENT_INSERT_EX(Cause, magic_enum::enum_name(ev.mDamageSource->getCause()));
			EVENT_INSERT_EX(Entity, EntityClass(ev.mDamageSource->getEntity()));
			EVENT_END;
		});
		break;

	case EventCode::onStepOnPressurePlate:
		Event::EntityStepOnPressurePlateEvent::subscribe([](const EntityStepOnPressurePlateEvent& ev) {
			EVENT_BEGIN(EventCode::onStepOnPressurePlate);
			EVENT_INSERT(Actor);
			EVENT_INSERT(BlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onMobDie:
		Event::MobDieEvent::subscribe([](const MobDieEvent& ev) {
			EVENT_BEGIN(EventCode::onMobDie);
			EVENT_INSERT_EX(Mob, EntityClass(ev.mMob));
			EVENT_INSERT_EX(Cause, magic_enum::enum_name(ev.mDamageSource->getCause()));
			EVENT_INSERT_EX(Entity, EntityClass(ev.mDamageSource->getEntity()));
			EVENT_END;
		});
		break;

	case EventCode::onNpcCmd:
		Event::NpcCmdEvent::subscribe([](const NpcCmdEvent& ev) {
			EVENT_BEGIN(EventCode::onNpcCmd);
			EVENT_INSERT(Npc);
			EVENT_INSERT(Player);
			EVENT_INSERT(Command);
			EVENT_END;
		});
		break;

	case EventCode::onSpawnProjectile:
		Event::ProjectileSpawnEvent::subscribe([](const ProjectileSpawnEvent& ev) {
			EVENT_BEGIN(EventCode::onSpawnProjectile);
			EVENT_INSERT(Shooter);
			// idf...
			EVENT_INSERT(Type);
			EVENT_END;
		});
		break;

	case EventCode::onProjectileCreated:
		Event::ProjectileCreatedEvent::subscribe([](const ProjectileCreatedEvent& ev) {
			EVENT_BEGIN(EventCode::onProjectileCreated);
			EVENT_INSERT(Shooter);
			EVENT_INSERT(Projectile);
			EVENT_END;
		});
		break;

	case EventCode::onProjectileHitEntity:
		Event::ProjectileHitEntityEvent::subscribe([](const ProjectileHitEntityEvent& ev) {
			EVENT_BEGIN(EventCode::onProjectileHitEntity);
			EVENT_INSERT(Target);
			EVENT_INSERT(Source);
			EVENT_END;
		});
		break;

	case EventCode::onEntityTransformation:
		Event::EntityTransformEvent::subscribe([](const EntityTransformEvent& ev) {
			EVENT_BEGIN(EventCode::onEntityTransformation);
			EVENT_INSERT_EX(EntityId, to_string(ev.mBeforeEntityUniqueId->id));
			EVENT_INSERT(AfterEntity);
			EVENT_END;
		});
		break;

	case EventCode::onProjectileHitBlock:
		Event::ProjectileHitBlockEvent::subscribe([](const ProjectileHitBlockEvent& ev) {
			EVENT_BEGIN(EventCode::onProjectileHitBlock);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Source);
			EVENT_END;
		});
		break;

	case EventCode::onLiquidFlow:
		Event::LiquidSpreadEvent::subscribe([](const LiquidSpreadEvent& ev) {
			EVENT_BEGIN(EventCode::onLiquidFlow);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Target);
			EVENT_INSERT(DimensionId);
			EVENT_END;
		});
		break;

	case EventCode::onUseFrameBlock:
		Event::PlayerUseFrameBlockEvent::subscribe([](const PlayerUseFrameBlockEvent& ev) {
			EVENT_BEGIN(EventCode::onUseFrameBlock);
			EVENT_INSERT(Player);
			EVENT_INSERT(BlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onBlockInteracted:
		Event::BlockInteractedEvent::subscribe([](const BlockInteractedEvent& ev) {
			EVENT_BEGIN(EventCode::onBlockInteracted);
			EVENT_INSERT(Player);
			EVENT_INSERT(BlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onFarmLandDecay:
		Event::FarmLandDecayEvent::subscribe([](const FarmLandDecayEvent& ev) {
			EVENT_BEGIN(EventCode::onFarmLandDecay);
			EVENT_INSERT(BlockInstance);
			EVENT_INSERT(Actor);
			EVENT_END;
		});
		break;

	case EventCode::onPistonTryPush:
		Event::PistonTryPushEvent::subscribe([](const PistonTryPushEvent& ev) {
			EVENT_BEGIN(EventCode::onPistonTryPush);
			EVENT_INSERT(PistonBlockInstance);
			EVENT_INSERT(TargetBlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onPistonPush:
		Event::PistonPushEvent::subscribe([](const PistonPushEvent& ev) {
			EVENT_BEGIN(EventCode::onPistonPush);
			EVENT_INSERT(PistonBlockInstance);
			EVENT_INSERT(TargetBlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onHopperSearchItem:
		Event::HopperSearchItemEvent::subscribe([](const HopperSearchItemEvent& ev) {
			EVENT_BEGIN(EventCode::onHopperSearchItem);
			EVENT_INSERT_EX(isMinecart, ev.isMinecart);
			EVENT_INSERT(HopperBlock);
			EVENT_INSERT(MinecartPos);
			EVENT_INSERT(DimensionId);
			EVENT_END;
		});
		break;

	case EventCode::onHopperPushOut:
		Event::HopperPushOutEvent::subscribe([](const HopperPushOutEvent& ev) {
			EVENT_BEGIN(EventCode::onHopperPushOut);
			EVENT_INSERT(Pos);
			EVENT_INSERT(DimensionId);
			EVENT_END;
		});
		break;

	case EventCode::onFireSpread:
		Event::FireSpreadEvent::subscribe([](const FireSpreadEvent& ev) {
			EVENT_BEGIN(EventCode::onFireSpread);
			EVENT_INSERT(Target);
			EVENT_INSERT(DimensionId);
			EVENT_END;
		});
		break;

	case EventCode::onBlockChanged:
		Event::BlockChangedEvent::subscribe([](const BlockChangedEvent& ev) {
			EVENT_BEGIN(EventCode::onBlockChanged);
			EVENT_INSERT(PreviousBlockInstance);
			EVENT_INSERT(NewBlockInstance);
			EVENT_END;
		});
		break;

	case EventCode::onScoreChanged:
		Event::PlayerScoreChangedEvent::subscribe([](const PlayerScoreChangedEvent& ev) {
			EVENT_BEGIN(EventCode::onScoreChanged);
			EVENT_INSERT(Player);
			EVENT_INSERT(Score);
			EVENT_INSERT_EX(Objective, ObjectiveClass(ev.mObjective));
			// ScoreboardId
			EVENT_END;
		});
		break;

	case EventCode::onConsoleOutput:
		Event::ConsoleOutputEvent::subscribe([](const ConsoleOutputEvent& ev) {
			EVENT_BEGIN(EventCode::onConsoleOutput);
			EVENT_INSERT(Output);
			EVENT_END;
		});
		break;
	case EventCode::onMobSpawn:
		Event::MobSpawnEvent::subscribe([](const MobSpawnEvent& ev) {
			EVENT_BEGIN(EventCode::onMobSpawn);
			EVENT_INSERT(TypeName);
			EVENT_INSERT(Pos);
			EVENT_INSERT(DimensionId);
			EVENT_END;
		});
		break;
	default:
		break;
	}
}
