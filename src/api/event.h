#pragma once
enum class EventCode {
  /* Player Events */
  onJoin,                 // done
  onPreJoin,              // done
  onLeft,                 // done
  onRespawn,              // done
  onPlayerDie,            // done
  onPlayerCmd,            // done
  onChat,                 // done
  onChangeDim,            // done
  onJump,                 // done
  onSneak,                // done
  onAttackEntity,         // done
  onAttackBlock,          // done
  onUseItem,              // done
  onUseItemOn,            // done
  onTakeItem,             // done
  onDropItem,             // done
  onEat,                  // done
  onConsumeTotem,         // done
  onEffectChanged,        // done
  onStartDestroyBlock,    // done
  onDestroyBlock,         // done
  onPlaceBlock,           // done
  onOpenContainer,        // done
  onCloseContainer,       // done
  onInventoryChange,      // done
  onMove,                 // done
  onChangeSprinting,      // done
  onSetArmor,             // done
  onUseRespawnAnchor,     // done
  onOpenContainerScreen,  // done
  /* Entity Events */
  onMobDie,                // done
  onMobHurt,               // done
  onEntityExplode,         // done
  onProjectileHitEntity,   // done
  onWitherBossDestroy,     // done
  onRide,                  // done
  onStepOnPressurePlate,   // done
  onSpawnProjectile,       // done
  onProjectileCreated,     // done
  onNpcCmd,                // done
  onChangeArmorStand,      // done
  onEntityTransformation,  // done
  /* Block Events */
  onBlockInteracted,     // done
  onBlockChanged,        // done
  onBlockExplode,        // done
  onBlockExploded,       // done
  onFireSpread,          // done
  onCmdBlockExecute,     // done
  onContainerChange,     // done
  onProjectileHitBlock,  // done
  onRedStoneUpdate,      // done
  onHopperSearchItem,    // done
  onHopperPushOut,       // done
  onPistonTryPush,       // done
  onPistonPush,          // done
  onFarmLandDecay,       // done
  onUseFrameBlock,       // done
  onLiquidFlow,          // done
  /* Other Events */
  onScoreChanged,   // done
  onServerStarted,  // done
  onConsoleCmd,     // done
  onConsoleOutput,  // done
  /* Outdated Events */
  onMobSpawn,  // done
};
void EnableEventListener(EventCode eventId);
