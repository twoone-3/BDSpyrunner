#include <Global.hpp>
#include "BlockAPI.h"
#include "EntityAPI.h"
#include "PlayerAPI.h"
#include "ItemAPI.h"
#include "PlayerAPI.h"
#include "McAPI.h"
#include "ContainerAPI.h"
#include "NbtAPI.h"
#include <MC/ServerPlayer.hpp>
#include <MC/Level.hpp>
#include <MC/ItemActor.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/CompoundTag.hpp>
#include <MC/Mob.hpp>
#include <MC/CommandUtils.hpp>

EntityClass::EntityClass(Actor* a) : thiz(a) {}

string EntityClass::getName() {
	if (!thiz)
		return {};
	return CommandUtils::getActorName(*thiz);
}

string EntityClass::getType() {
	if (!thiz)
		return {};
	return thiz->getTypeName();
}

int EntityClass::getId() {
	if (!thiz)
		return {};
	return (int)thiz->getEntityTypeId();
}

Vec3 EntityClass::getPos() {
	if (!thiz)
		return {};
	return thiz->getPosition();
}

BlockPos EntityClass::getBlockPos() {
	if (!thiz)
		return {};
	return thiz->getBlockPos();
}

int EntityClass::getDim() {
	if (!thiz)
		return {};
	return thiz->getDimensionId();
}

int EntityClass::getMaxHealth() {
	if (!thiz)
		return {};
	return thiz->getMaxHealth();
}

int EntityClass::getHealth() {
	if (!thiz)
		return {};
	return thiz->getHealth();
}

bool EntityClass::getInAir() {
	if (!thiz)
		return {};
	return !thiz->isOnGround() && !thiz->isInWater();
}

bool EntityClass::getInWater() {
	if (!thiz)
		return {};
	return thiz->isInWater();
}

float EntityClass::getSpeed() {
	if (!thiz)
		return {};
	return thiz->getSpeedInMetersPerSecond();
}

Vec2 EntityClass::getDirection() {
	if (!thiz)
		return {0, 0};
	return *thiz->getDirection();
}

string EntityClass::getUniqueID() {
	if (!thiz)
		return {};
	return std::to_string(thiz->getUniqueID().id);
}

bool EntityClass::teleport(const Vec3& pos, int dim) {
	if (!thiz)
		return {};
	return thiz->teleport(pos, dim);
}

bool EntityClass::kill() {
	if (!thiz)
		return {};
	thiz->kill();
	return true;
}

bool EntityClass::hurt(float damage) {
	if (!thiz)
		return {};
	return thiz->hurtEntity(damage);
}

bool EntityClass::setOnFire(int time) {
	if (!thiz)
		return {};
	return thiz->setOnFire(time, true);
}

bool EntityClass::isPlayer() {
	if (!thiz)
		return {};
	return thiz->isPlayer();
}

PlayerClass EntityClass::toPlayer() {
	if (!thiz || !thiz->isPlayer())
		return nullptr;
	auto pl = (Player*)thiz;
	if (!pl)
		return nullptr;
	else
		return pl;
}

bool EntityClass::isItemEntity() {
	if (!thiz)
		return {};
	return thiz->isItemActor();
}

EntityClass EntityClass::toItem() {
	if (!thiz || !thiz->isItemActor())
		return nullptr;
	auto it = (ItemActor*)thiz;
	if (!it)
		return nullptr;
	return reinterpret_cast<Actor*>(it->getItemStack());
}

BlockClass EntityClass::getBlockStandingOn() {
	if (!thiz)
		return BlockInstance::Null;
	return Level::getBlockInstance(thiz->getBlockPosCurrentlyStandingOn(nullptr), getDim());
}

ContainerClass EntityClass::getArmor() {
	if (!thiz)
		return nullptr;
	return &thiz->getArmorContainer();
}

bool EntityClass::hasContainer() {
	if (!thiz)
		return {};
	return Level::hasContainer(thiz->getPosition(), thiz->getDimensionId());
}

ContainerClass EntityClass::getContainer() {
	if (!thiz)
		return nullptr;
	return Level::getContainer(thiz->getPosition(), thiz->getDimensionId());
}

bool EntityClass::refreshItems() {
	if (!thiz)
		return {};
	return reinterpret_cast<Mob*>(thiz)->refreshInventory();
}

NbtClass EntityClass::getNbt() {
	if (!thiz)
		return {};
	return thiz->getNbt();
}

bool EntityClass::setNbt(const NbtClass& nbt) {
	if (!thiz)
		return {};
	if (!nbt.thiz)
		return {};
	return thiz->setNbt(nbt.thiz->asCompoundTag());
}

bool EntityClass::addTag(const string& tag) {
	if (!thiz)
		return {};
	return thiz->addTag(tag);
}

bool EntityClass::removeTag(const string& tag) {
	if (!thiz)
		return {};
	return thiz->removeTag(tag);
}

bool EntityClass::hasTag(const string& tag) {
	if (!thiz)
		return {};
	return thiz->hasTag(tag);
}

vector<string> EntityClass::getAllTags() {
	if (!thiz)
		return {};
	return thiz->getAllTags();
}

EntityClass EntityClass::getEntityFromViewVector(float maxDistance) {
	if (!thiz)
		return nullptr;
	auto a = thiz->getActorFromViewVector(maxDistance);
	if (a)
		return a;
	return nullptr;
}

BlockClass EntityClass::getBlockFromViewVector(bool includeLiquid, bool solidOnly, float maxDistance, bool ignoreBorderBlocks, bool fullOnly) {
	if (!thiz)
		return BlockInstance::Null;
	return thiz->getBlockFromViewVector(includeLiquid, solidOnly, maxDistance, ignoreBorderBlocks, fullOnly);
}
