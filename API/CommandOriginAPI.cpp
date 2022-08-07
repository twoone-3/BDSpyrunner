#include "CommandOriginAPI.h"

#include <MC/Command.hpp>
#include <MC/CommandOrigin.hpp>
#include <MC/Dimension.hpp>
#include <magic_enum/magic_enum.hpp>

#include "BlockAPI.h"
#include "ContainerAPI.h"
#include "EntityAPI.h"
#include "ItemAPI.h"
#include "McAPI.h"
#include "NBTAPI.h"
#include "PlayerAPI.h"

CommandOriginClass::CommandOriginClass(CommandOrigin* p) : thiz(p) {}

int CommandOriginClass::getOriginType() { return (int)thiz->getOriginType(); }

string_view CommandOriginClass::getOriginTypeName() {
  return magic_enum::enum_name((OriginType)thiz->getOriginType());
}

string CommandOriginClass::getOriginName() { return thiz->getName(); }

BlockPos CommandOriginClass::getBlockPosition() {
  return thiz->getBlockPosition();
}

Vec3 CommandOriginClass::getPosition() { return thiz->getWorldPosition(); }

int CommandOriginClass::getDim() {
  return thiz->getDimension()->getDimensionId();
}

EntityClass CommandOriginClass::getEntity() { return thiz->getEntity(); }

PlayerClass CommandOriginClass::getPlayer() {
  return reinterpret_cast<Player*>(thiz->getPlayer());
}

NBTClass CommandOriginClass::getNbt() {
  return std::make_unique<CompoundTag>(thiz->serialize());
}

const char* CommandOriginClass::toString() { return "<CommandOrigin>"; }
