#pragma once
#include <Global.hpp>

//////////////////// Classes ////////////////////
class BlockActor;
struct BlockClass;
struct NbtClass;
struct BlockEntityClass {
	BlockActor* thiz;
	int dim;

	BlockEntityClass(BlockActor* be,int dim);

	BlockPos getPos();
	int getType();

	NbtClass getNbt();
	bool setNbt(const NbtClass& nbt);
	BlockClass getBlock();
};
