#include "BlockAPI.h"
#include "McAPI.h"
#include "ContainerAPI.h"
#include "BlockEntityAPI.h"
#include "NbtAPI.h"

BlockClass::BlockClass(const BlockInstance& bi) : thiz(bi) {}

BlockClass::BlockClass(BlockInstance&& bi) : thiz(std::move(bi)) {}

BlockClass::BlockClass(const BlockPos& pos, int dim) : thiz(Level::getBlockInstance(pos, dim)) {}

string BlockClass::getName() {
	return thiz.getBlock()->getName().getString();
}

string BlockClass::getType() {
	return thiz.getBlock()->getTypeName();
}

int BlockClass::getId() {
	return thiz.getBlock()->getId();
}

BlockPos BlockClass::getPos() {
	return thiz.getPosition();
}

int BlockClass::getDim() {
	return thiz.getDimensionId();
}

unsigned short BlockClass::getTileData() {
	return thiz.getBlock()->getTileData();
}

NbtClass BlockClass::getNbt() {
	return thiz.getBlock()->getNbt();
}

bool BlockClass::setNbt(const NbtClass& nbt) {
	return Level::setBlock(getPos(), getDim(), nbt.thiz->asCompoundTag());
}

bool BlockClass::hasContainer() {
	return thiz.hasContainer();
}

ContainerClass BlockClass::getContainer() {
	return ContainerClass(thiz.getContainer());
}

bool BlockClass::hasBlockEntity() {
	return thiz.getBlock()->hasBlockEntity();
}

BlockEntityClass BlockClass::getBlockEntity() {
	return {thiz.getBlockEntity(), getDim()};
}

bool BlockClass::removeBlockEntity(const BlockPos& pos) {
	thiz.getBlockSource()->removeBlockEntity(pos);
	return true;
}