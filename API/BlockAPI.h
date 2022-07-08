#pragma once
#include <Global.hpp>
#include <MC/BlockInstance.hpp>

class Block;
struct ContainerClass;
struct BlockEntityClass;
struct NbtClass;
struct BlockClass {
	BlockInstance thiz;

	BlockClass(const BlockInstance& bi);
	BlockClass(BlockInstance&& bi);
	BlockClass(const BlockPos& pos, int dim);

	string getName();
	string getType();
	int getId();
	BlockPos getPos();
	int getDim();
	unsigned short getTileData();

	NbtClass getNbt();
	bool setNbt(const NbtClass&);
	bool hasContainer();
	ContainerClass getContainer();
	bool hasBlockEntity();
	BlockEntityClass getBlockEntity();
	bool removeBlockEntity(const BlockPos& pos);
};
