﻿#pragma once
#pragma execution_character_set("utf-8")
/*LL*/
#include <Global.h>
#include <LoggerAPI.h>
/*MC*/
#include <MC/Actor.hpp>
#include <MC/ActorDamageSource.hpp>
#include <MC/BinaryStream.hpp>
#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockInstance.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockPalette.hpp>
#include <MC/BlockSource.hpp>
#include <MC/CommandParameterData.hpp>
#include <MC/CommandRegistry.hpp>
#include <MC/Common.hpp>
#include <MC/Container.hpp>
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
/*NBT*/
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

inline bool IsPlayer(Actor* ptr) {
	if (ptr == nullptr)
		return false;
	if (ptr->getEntityTypeId() != 319)
		return false;
	return true;
}

inline Logger logger("BDSpyrunner");