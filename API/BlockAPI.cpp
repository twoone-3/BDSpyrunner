#include "BlockAPI.h"
#include "McAPI.h"
#include "ContainerAPI.h"
#include "EntityAPI.h"
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

//公用API
//auto McClass::getBlock() {
//	CHECK_ARGS_COUNT(args, 1);
//
//	IntVec4 pos;
//	if (args.size() == 1) {
//		// IntPos
//		if (IsInstanceOf<IntPos>(args[0])) {
//			// IntPos
//			IntPos* posObj = IntPos::extractPos(args[0]);
//			if (posObj->dim < 0)
//				return false;
//			else {
//				pos = *posObj;
//			}
//		}
//		else if (IsInstanceOf<FloatPos>(args[0])) {
//			// FloatPos
//			FloatPos* posObj = FloatPos::extractPos(args[0]);
//			if (posObj->dim < 0)
//				return false;
//			else {
//				pos = posObj->toIntVec4();
//			}
//		}
//		else {
//			LOG_WRONG_ARG_TYPE();
//			return {};
//		}
//	}
//	else if (args.size() == 4) {
//		// Number Pos
//		CHECK_ARG_TYPE(args[0], ValueKind::kNumber);
//		CHECK_ARG_TYPE(args[1], ValueKind::kNumber);
//		CHECK_ARG_TYPE(args[2], ValueKind::kNumber);
//		CHECK_ARG_TYPE(args[3], ValueKind::kNumber);
//		pos = {args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()};
//	}
//	else {
//		LOG_WRONG_ARGS_COUNT();
//		return {};
//	}
//
//	auto block = Level::getBlock(pos.getBlockPos(), pos.dim);
//	if (!block) {
//		LOG_WRONG_ARG_TYPE();
//		return {};
//	}
//	else {
//		BlockPos bp{pos.x, pos.y, pos.z};
//		return BlockClass::newBlock(block, &bp, pos.dim);
//	}
//	("Fail in GetBlock!")
//}
//
//auto McClass::setBlock() {
//	CHECK_ARGS_COUNT(args, 2);
//
//	IntVec4 pos;
//	auto block;
//	unsigned short tileData = 0;
//	if (args.size() == 2 || args.size() == 3) {
//		if (args.size() == 3) {
//			CHECK_ARG_TYPE(args[1], ValueKind::kString);
//			CHECK_ARG_TYPE(args[2], ValueKind::kNumber);
//			tileData = args[2].toInt();
//		}
//		if (IsInstanceOf<IntPos>(args[0])) {
//			// IntPos
//			IntPos* posObj = IntPos::extractPos(args[0]);
//			if (posObj->dim < 0)
//				return false;
//			else {
//				pos = *posObj;
//				block = args[1];
//			}
//		}
//		else if (IsInstanceOf<FloatPos>(args[0])) {
//			// FloatPos
//			FloatPos* posObj = FloatPos::extractPos(args[0]);
//			if (posObj->dim < 0)
//				return false;
//			else {
//				pos = posObj->toIntVec4();
//				block = args[1];
//			}
//		}
//		else {
//			LOG_WRONG_ARG_TYPE();
//			return {};
//		}
//	}
//	else if (args.size() == 5 || args.size() == 6) {
//		// Number Pos
//		CHECK_ARG_TYPE(args[0], ValueKind::kNumber);
//		CHECK_ARG_TYPE(args[1], ValueKind::kNumber);
//		CHECK_ARG_TYPE(args[2], ValueKind::kNumber);
//		CHECK_ARG_TYPE(args[3], ValueKind::kNumber);
//		pos = {args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()};
//		block = args[4];
//		if (args.size() == 6) {
//			CHECK_ARG_TYPE(args[4], ValueKind::kString);
//			CHECK_ARG_TYPE(args[5], ValueKind::kNumber);
//			tileData = args[5].toInt();
//		}
//	}
//	else {
//		LOG_WRONG_ARGS_COUNT();
//		return {};
//	}
//
//
//	if (block.isString()) {
//		//方块名
//		return Level::setBlock(pos.getBlockPos(), pos.dim, block.toStr(), tileData);
//	}
//	else if (IsInstanceOf<NbtClass>(block)) {
//		// Nbt
//		Tag* nbt = NbtClass::extract(block);
//		return Level::setBlock(pos.getBlockPos(), pos.dim, (CompoundTag*)nbt);
//	}
//	else {
//		//其他方块对象
//		Block* bl = BlockClass::extract(block);
//		if (!bl) {
//			LOG_WRONG_ARG_TYPE();
//			return {};
//		}
//		return Level::setBlock(pos.getBlockPos(), pos.dim, bl);
//	}
//	("Fail in SetBlock!")
//}
//
