from mc import *

logger = Logger("test")


def onAllEvent(e):
    logger.debug(str(e))


events = [
    "onPreJoin",
    "onJoin",
    "onLeft",
    "onRespawn",
    "onPlayerDie",
    "onPlayerCmd",
    "onChat",
    "onChangeDim",
    "onJump",
    "onSneak",
    "onAttackEntity",
    "onAttackBlock",
    "onUseItem",
    "onUseItemOn",
    "onTakeItem",
    "onDropItem",
    "onEat",
    "onConsumeTotem",
    "onEffectChanged",
    "onStartDestroyBlock",
    "onDestroyBlock",
    "onPlaceBlock",
    "onOpenContainer",
    "onCloseContainer",
    "onInventoryChange",
    "onMove",
    "onChangeSprinting",
    "onSetArmor",
    "onUseRespawnAnchor",
    "onOpenContainerScreen",
    "onMobDie",
    "onMobHurt",
    "onEntityExplode",
    "onProjectileHitEntity",
    "onWitherBossDestroy",
    "onRide",
    "onStepOnPressurePlate",
    "onSpawnProjectile",
    "onProjectileCreated",
    "onNpcCmd",
    "onChangeArmorStand",
    "onEntityTransformation",
    "onBlockInteracted",
    "onBlockChanged",
    "onBlockExplode",
    "onBlockExploded",
    "onFireSpread",
    "onCmdBlockExecute",
    "onContainerChange",
    "onProjectileHitBlock",
    "onRedStoneUpdate",
    "onHopperSearchItem",
    "onHopperPushOut",
    "onPistonTryPush",
    "onPistonPush",
    "onFarmLandDecay",
    "onUseFrameBlock",
    "onLiquidFlow",
    "onScoreChanged",
    "onTick",
    "onServerStarted",
    "onConsoleCmd",
    "onConsoleOutput",
    "onMoneyAdd",
    "onMoneyReduce",
    "onMoneyTrans",
    "onMoneySet",
    "beforeMoneyAdd",
    "beforeMoneyReduce",
    "beforeMoneyTrans",
    "beforeMoneySet",
    "onMobSpawn",
]

# for n in events:
# 	setListener(n,onAllEvent)

# nbt = NBT.newCompound()
# nbt['list'] = NBT.newList()
# nbt['string'] = NBT.newString('I am string')
# nbt['list'] = NBT.newList()
# nbt['a24z'] = NBT.fromSNBT('{"a4z":{},"az":[1f,1f,1],"az3":"az2"}')
# logger.warn(str(nbt['a24z']))
# nbt['a24z']['a4z'] = NBT.newByte(2)
# b = nbt.toBinary()
# nbt['二进制'] = NBT.fromBinary(b)
# logger.warn(nbt.toSNBT(4))
