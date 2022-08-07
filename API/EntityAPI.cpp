#include "EntityAPI.h"

#include <MC/CommandUtils.hpp>
#include <MC/ItemActor.hpp>
#include <MC/Level.hpp>
#include <MC/Mob.hpp>
#include <MC/SimpleContainer.hpp>

#include "BlockAPI.h"
#include "ContainerAPI.h"
#include "ItemAPI.h"
#include "McAPI.h"
#include "NBTAPI.h"
#include "PlayerAPI.h"

EntityClass::EntityClass(Actor* a) : thiz(a) {}

string EntityClass::getName() {
  if (thiz == nullptr) return "";
  return CommandUtils::getActorName(*thiz);
}

string EntityClass::getType() {
  if (thiz == nullptr) return "";
  return thiz->getTypeName();
}

int EntityClass::getId() {
  if (thiz == nullptr) return -1;
  return (int)thiz->getEntityTypeId();
}

Vec3 EntityClass::getPos() {
  if (thiz == nullptr) return Vec3();
  return thiz->getPosition();
}

BlockPos EntityClass::getBlockPos() {
  if (thiz == nullptr) return BlockPos();
  return thiz->getBlockPos();
}

int EntityClass::getDim() {
  if (thiz == nullptr) return -1;
  return thiz->getDimensionId();
}

int EntityClass::getMaxHealth() {
  if (thiz == nullptr) return -1;
  return thiz->getMaxHealth();
}

int EntityClass::getHealth() {
  if (thiz == nullptr) return -1;
  return thiz->getHealth();
}

bool EntityClass::getInAir() {
  if (thiz == nullptr) return false;
  return !thiz->isOnGround() && !thiz->isInWater();
}

bool EntityClass::getInWater() {
  if (thiz == nullptr) return false;
  return thiz->isInWater();
}

float EntityClass::getSpeed() {
  if (thiz == nullptr) return 0.0f;
  return thiz->getSpeedInMetersPerSecond();
}

Vec2 EntityClass::getDirection() {
  if (thiz == nullptr) return Vec2(0.f, 0.f);
  return *thiz->getDirection();
}

string EntityClass::getUniqueID() {
  if (thiz == nullptr) return "";
  return std::to_string(thiz->getUniqueID().id);
}

bool EntityClass::teleport(const Vec3& pos, int dim) {
  if (thiz == nullptr) return false;
  return thiz->teleport(pos, dim);
}

bool EntityClass::kill() {
  if (thiz == nullptr) return false;
  thiz->kill();
  return true;
}

bool EntityClass::hurt(float damage) {
  if (thiz == nullptr) return false;
  return thiz->hurtEntity(damage);
}

bool EntityClass::setOnFire(int time) {
  if (thiz == nullptr) return false;
  return thiz->setOnFire(time, true);
}

bool EntityClass::isPlayer() {
  if (thiz == nullptr) return false;
  return thiz->isPlayer();
}

PlayerClass EntityClass::toPlayer() {
  if (thiz == nullptr) return nullptr;
  if (!thiz->isPlayer()) return nullptr;
  return (Player*)thiz;
}

bool EntityClass::isItemEntity() {
  if (thiz == nullptr) return false;
  return thiz->isItemActor();
}

ItemClass EntityClass::toItem() {
  if (thiz == nullptr) return nullptr;
  if (!thiz->isItemActor()) return nullptr;
  return reinterpret_cast<ItemActor*>(thiz)->getItemStack();
}

BlockClass EntityClass::getBlockStandingOn() {
  if (thiz == nullptr) return BlockInstance::Null;
  return Level::getBlockInstance(thiz->getBlockPosCurrentlyStandingOn(nullptr),
                                 getDim());
}

ContainerClass EntityClass::getArmor() {
  if (thiz == nullptr) return nullptr;
  return &thiz->getArmorContainer();
}

bool EntityClass::hasContainer() {
  if (thiz == nullptr) return false;
  return Level::hasContainer(thiz->getPosition(), thiz->getDimensionId());
}

ContainerClass EntityClass::getContainer() {
  if (thiz == nullptr) return nullptr;
  return Level::getContainer(thiz->getPosition(), thiz->getDimensionId());
}

bool EntityClass::refreshItems() {
  if (thiz == nullptr) return false;
  return reinterpret_cast<Mob*>(thiz)->refreshInventory();
}

NBTClass EntityClass::getNbt() {
  if (thiz == nullptr) return nullptr;
  return thiz->getNbt();
}

bool EntityClass::setNbt(const NBTClass& nbt) {
  if (thiz == nullptr) return false;
  if (!nbt.thiz) return false;
  return thiz->setNbt(nbt.thiz->asCompoundTag());
}

bool EntityClass::addTag(const string& tag) {
  if (thiz == nullptr) return false;
  return thiz->addTag(tag);
}

bool EntityClass::removeTag(const string& tag) {
  if (thiz == nullptr) return false;
  return thiz->removeTag(tag);
}

bool EntityClass::hasTag(const string& tag) {
  if (thiz == nullptr) return false;
  return thiz->hasTag(tag);
}

vector<string> EntityClass::getAllTags() {
  if (thiz == nullptr) return vector<string>();
  return thiz->getAllTags();
}

EntityClass EntityClass::getEntityFromViewVector(float maxDistance) {
  if (thiz == nullptr) return nullptr;
  return thiz->getActorFromViewVector(maxDistance);
}

BlockClass EntityClass::getBlockFromViewVector(bool includeLiquid,
                                               bool solidOnly,
                                               float maxDistance,
                                               bool ignoreBorderBlocks,
                                               bool fullOnly) {
  if (thiz == nullptr) return BlockInstance::Null;
  return thiz->getBlockFromViewVector(includeLiquid, solidOnly, maxDistance,
                                      ignoreBorderBlocks, fullOnly);
}
