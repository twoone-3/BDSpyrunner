#include "BlockAPI.h"
#include "BlockEntityAPI.h"
#include "NbtAPI.h"

BlockEntityClass::BlockEntityClass(BlockActor* be, int dim) : thiz(be), dim(dim) {}

BlockPos BlockEntityClass::getPos() {
	return thiz->getPosition();
}

int BlockEntityClass::getType() {
	return (int)thiz->getType();
}

NbtClass BlockEntityClass::getNbt() {
	return thiz->getNbt();
}

bool BlockEntityClass::setNbt(const NbtClass& nbt) {
	if (!nbt.thiz)
		return false;
	return thiz->setNbt(nbt.thiz->asCompoundTag());
}

BlockClass BlockEntityClass::getBlock() {
	BlockPos bp = thiz->getPosition();
	return Level::getBlockInstance(bp, dim);
}
