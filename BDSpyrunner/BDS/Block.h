#pragma once
#include "../tool.h"
#include "Math.h"

struct BlockPalette {};
struct BlockLegacy {
	string getBlockName();
	short getBlockItemID();
};
struct Block {
	BlockLegacy* getBlockLegacy();
};
struct BlockActor {
	Block* getBlock();
	BlockPos* getPosition();
};
struct BlockSource {
	Block* getBlock(const BlockPos* bp);
	bool setBlock(Block* b, BlockPos* bp);
	void neighborChanged(const BlockPos* pos);
	void updateNeighborsAt(const BlockPos* pos);
	int getDimensionId();
};
