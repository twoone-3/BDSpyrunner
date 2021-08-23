#include <Block.h>
#include <tool.h>

using namespace std;
string BlockLegacy::getBlockName() {
	return FETCH(string, this + 128);
}

short BlockLegacy::getBlockItemID() {
	return SymCall<short>("?getBlockItemId@BlockLegacy@@QEBAFXZ", this);
	//IDA Item::beginCreativeGroup(,Block*,) 18~22
	//short v3 = FETCH(short, this + 328);
	//if (v3 < 0x100) {
	//	return v3;
	//}
	//return short(255 - v3);
}

BlockLegacy* Block::getBlockLegacy() {
	return FETCH(BlockLegacy*, this + 16);
}

Block* BlockActor::getBlock() {
	return FETCH(Block*, this + 16);
}

BlockPos* BlockActor::getPosition() {//IDA BlockActor::BlockActor 18~20
	return reinterpret_cast<BlockPos*>(this + 44);
}

Block* BlockSource::getBlock(const BlockPos* bp) {
	return SymCall<Block*>("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z",
		this, bp);
}

BlockActor* BlockSource::getBlockEntity(const BlockPos* bp) {
	return SymCall<BlockActor*>("?getBlockEntity@BlockSource@@QEAAPEAVBlockActor@@AEBVBlockPos@@@Z",
		this, bp);
}

bool BlockSource::setBlock(Block* b, BlockPos* bp) {
	return SymCall<bool>("?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HPEBUActorBlockSyncMessage@@@Z",
		this, bp, b, 3, nullptr);
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
	SymCall<int>("?getDimensionId@BlockSource@@QEBA?AV?$AutomaticID@VDimension@@H@@XZ",
		this, &did);
	return did;
	//IDA Dimension::onBlockChanged 42
	//return FETCH(int, FETCH(uintptr_t, this + 32) + 216);
}
