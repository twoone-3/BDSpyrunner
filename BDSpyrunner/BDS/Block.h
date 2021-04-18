#pragma once
#include "Math.h"
struct BlockPalette {};
struct BlockLegacy {
	string getBlockName() {
		return FETCH(string, this + 128);
	}
	short getBlockItemID() {//IDA Item::beginCreativeGroup(,Block*,) 18~22
		short v3 = FETCH(short, this + 328);
		if (v3 < 0x100) {
			return v3;
		}
		return (short)(255 - v3);
	}
};
struct Block {
	BlockLegacy* getBlockLegacy() {
		return FETCH(BlockLegacy*, this + 16);
	}
};
struct BlockActor {
	Block* getBlock() {
		return FETCH(Block*, this + 16);
	}
	BlockPos* getPosition() {//IDA BlockActor::BlockActor 18~20
		return (BlockPos*)(this + 44);
	}
};
struct BlockSource {
	Block* getBlock(const BlockPos* bp) {
		return SYMCALL<Block*>("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z",
			this, bp);
	}
	bool setBlock(const string& name, const BlockPos& bp) {
		return SYMCALL<bool>("?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HPEBUActorBlockSyncMessage@@@Z",
			this, &bp, *(Block**)GetServerSymbol(("?m" + name + "@VanillaBlocks@@3PEBVBlock@@EB").c_str()), 3, nullptr);
	}
	void neighborChanged(const BlockPos* pos) {
		SYMCALL("?neighborChanged@BlockSource@@QEAAXAEBVBlockPos@@0@Z",
			this, pos, pos);
	}
	void updateNeighborsAt(const BlockPos* pos) {
		SYMCALL("?updateNeighborsAt@BlockSource@@QEAAXAEBVBlockPos@@@Z",
			this, pos);
	}
	int getDimensionId() {	//IDA Dimension::onBlockChanged 42
		return FETCH(int, FETCH(VA, this + 32) + 216);
	}
};
