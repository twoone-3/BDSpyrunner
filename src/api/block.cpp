#include "block.h"

#include <llapi/mc/Block.hpp>
#include <llapi/mc/BlockSource.hpp>
#include <llapi/mc/HashedString.hpp>
#include <llapi/mc/Level.hpp>

#include "block_entity.h"
#include "container.h"
#include "nbt.h"
#include "utils.h"

BlockClass::BlockClass(const BlockInstance& bi) : thiz(bi) {}

BlockClass::BlockClass(BlockInstance&& bi) : thiz(std::move(bi)) {}

BlockClass::BlockClass(const BlockPos& pos, int dim)
    : thiz(Level::getBlockInstance(pos, dim)) {}

string BlockClass::getName() { return thiz.getBlock()->getName().getString(); }

string BlockClass::getType() { return thiz.getBlock()->getTypeName(); }

int BlockClass::getId() { return thiz.getBlock()->getId(); }

BlockPos BlockClass::getPos() { return thiz.getPosition(); }

int BlockClass::getDim() { return thiz.getDimensionId(); }

unsigned short BlockClass::getTileData() {
  return thiz.getBlock()->getTileData();
}

NBTClass BlockClass::getNbt() { return thiz.getBlock()->getNbt(); }

bool BlockClass::setNbt(const NBTClass& nbt) {
  return Level::setBlock(getPos(), getDim(), nbt.thiz->asCompoundTag());
}

bool BlockClass::hasContainer() { return thiz.hasContainer(); }

ContainerClass BlockClass::getContainer() {
  return ContainerClass(thiz.getContainer());
}

bool BlockClass::hasBlockEntity() { return thiz.getBlock()->hasBlockEntity(); }

BlockEntityClass BlockClass::getBlockEntity() {
  return {thiz.getBlockEntity(), getDim()};
}

bool BlockClass::removeBlockEntity(const BlockPos& pos) {
  thiz.getBlockSource()->removeBlockEntity(pos);
  return true;
}