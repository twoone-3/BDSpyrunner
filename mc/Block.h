#pragma once
#include "Position.h"

struct BlockPalette;
struct BlockLegacy {
	std::string getBlockName();
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
	BlockActor* getBlockEntity(const BlockPos* bp);
	void setBlock(const BlockPos*, Block* b);
	void neighborChanged(const BlockPos* pos);
	void updateNeighborsAt(const BlockPos* pos);
	int getDimensionId();
};
