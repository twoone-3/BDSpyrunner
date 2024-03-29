﻿#include "block_entity.h"

#include <llapi/mc/BlockActor.hpp>
#include <llapi/mc/Level.hpp>

#include "block.h"
#include "nbt.h"

BlockEntityClass::BlockEntityClass(BlockActor* be, int dim)
    : thiz(be), dim(dim) {}

BlockPos BlockEntityClass::getPos() { return thiz->getPosition(); }

int BlockEntityClass::getType() { return (int)thiz->getType(); }

NBTClass BlockEntityClass::getNbt() { return thiz->getNbt(); }

bool BlockEntityClass::setNbt(const NBTClass& nbt) {
  if (!nbt.thiz) return false;
  return thiz->setNbt(nbt.thiz->asCompoundTag());
}

BlockClass BlockEntityClass::getBlock() {
  BlockPos bp = thiz->getPosition();
  return Level::getBlockInstance(bp, dim);
}
