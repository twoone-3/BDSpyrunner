#include "Block.h"
#include "tool.h"

using namespace std;
string BlockLegacy::getBlockName() {
	return Dereference<string>(this , 128);
}

short BlockLegacy::getBlockItemID() {
	//IDA Item::beginCreativeGroup(,Block*,) 18~22
	//short v3 = Dereference<short, this + 328);
	//if (v3 < 0x100) {
	//	return v3;
	//}
	//return short(255 - v3);
	return SymCall<short>("?getBlockItemId@BlockLegacy@@QEBAFXZ", this);
}

BlockLegacy* Block::getBlockLegacy() {
	return Dereference<BlockLegacy*>(this , 16);
}

Block* BlockActor::getBlock() {
	return Dereference<Block*>(this , 16);
}

BlockPos* BlockActor::getPosition() {//IDA BlockActor::BlockActor 18~20
	return Dereference<BlockPos*>(this , 44);
}

void BlockActor::setChanged() {
	SymCall("?setChanged@BlockActor@@QEAAXXZ", this);
}

Block* BlockSource::getBlock(const BlockPos* bp) {
	return SymCall<Block*>("?getBlock@BlockSource@@UEBAAEBVBlock@@AEBVBlockPos@@@Z",
		this, bp);
}

BlockActor* BlockSource::getBlockEntity(const BlockPos* bp) {
	return SymCall<BlockActor*>("?getBlockEntity@BlockSource@@QEAAPEAVBlockActor@@AEBVBlockPos@@@Z",
		this, bp);
}

void BlockSource::setBlock(const BlockPos* bp, Block* b) {
	SymCall<uintptr_t>("?setBlock@BlockSource@@QEAA_NHHHAEBVBlock@@HPEAVActor@@@Z",
		this, bp->x, bp->y, bp->z, b, 3, nullptr);
	//SymCall<bool>("?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HV?$shared_ptr@VBlockActor@@@std@@PEBUActorBlockSyncMessage@@PEAVActor@@@Z",
	//this, bp, b, 3, nullptr, nullptr, nullptr);
}

void BlockSource::neighborChanged(const BlockPos* pos) {
	SymCall("?neighborChanged@BlockSource@@QEAAXAEBVBlockPos@@0@Z",
		this, pos, pos);
}

void BlockSource::updateNeighborsAt(const BlockPos* pos) {
	SymCall("?updateNeighborsAt@BlockSource@@QEAAXAEBVBlockPos@@@Z",
		this, pos);
}

int BlockSource::getDimensionId() {
	int did;
	SymCall<int&>("?getDimensionId@BlockSource@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ",
		this, &did);
	return did;
}
