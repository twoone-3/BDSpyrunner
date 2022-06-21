#include "CommandOriginAPI.h"
#include "NbtAPI.h"
#include "EntityAPI.h"
#include <Global.hpp>
#include "BlockAPI.h"
#include "EntityAPI.h"
#include "ItemAPI.h"
#include "PlayerAPI.h"
#include "McAPI.h"
#include "ContainerAPI.h"
#include "NbtAPI.h"
#include <MC/Actor.hpp>
#include <MC/Level.hpp>
#include <MC/ItemActor.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/CompoundTag.hpp>
#include <MC/Mob.hpp>
#include <MC/Dimension.hpp>
#include <MC/Command.hpp>

CommandOriginClass::CommandOriginClass(CommandOrigin* p) : thiz(p) {}

int CommandOriginClass::getOriginType() {
	return (int)thiz->getOriginType();
}

string_view CommandOriginClass::getOriginTypeName() { // TODO: check string_view can transform to pyobject
	return magic_enum::enum_name((OriginType)thiz->getOriginType());
}

string CommandOriginClass::getOriginName() {
	return thiz->getName();
}

BlockPos CommandOriginClass::getBlockPosition() {
	return thiz->getBlockPosition();
}

Vec3 CommandOriginClass::getPosition() {
	return thiz->getWorldPosition();
}

int CommandOriginClass::getDim() {
	return thiz->getDimension()->getDimensionId();
}

EntityClass CommandOriginClass::getEntity() {
	return thiz->getEntity();
}

PlayerClass CommandOriginClass::getPlayer() {
	return reinterpret_cast<Player*>(thiz->getPlayer());
}

NbtClass CommandOriginClass::getNbt() {
	return std::make_unique<CompoundTag>(thiz->serialize());
}

const char* CommandOriginClass::toString() {
	return "<CommandOrigin>";
}
