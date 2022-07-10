#include "BlockAPI.h"
#include "BlockEntityAPI.h"
#include "NBTAPI.h"
#include <MC/BlockActor.hpp>
#include <MC/Level.hpp>

BlockEntityClass::BlockEntityClass(BlockActor* be, int dim) : thiz(be), dim(dim) {}

BlockPos BlockEntityClass::getPos() {
	return thiz->getPosition();
}

int BlockEntityClass::getType() {
	return (int)thiz->getType();
}

NBTClass BlockEntityClass::getNbt() {
	return thiz->getNbt();
}

bool BlockEntityClass::setNbt(const NBTClass& nbt) {
	if (!nbt.thiz)
		return false;
	return thiz->setNbt(nbt.thiz->asCompoundTag());
}

BlockClass BlockEntityClass::getBlock() {
	BlockPos bp = thiz->getPosition();
	return Level::getBlockInstance(bp, dim);
}
