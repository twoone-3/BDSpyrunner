#pragma once
#include "json.hpp"
#include "Position.h"
#include "span.h"

struct Tag;
struct BlockSource;
struct BlockPalette;
struct StructureSettings {
	char _this[96];
	StructureSettings(BlockPos* size, bool IgnoreEntities, bool IgnoreBlocks);
	~StructureSettings();
};
#if 0
struct StructureDataLoadHelper {
	char _this[136];

	StructureDataLoadHelper(BlockPos* bp1, BlockPos* bp2, Vec3* v, uintptr_t id, char Rotation, char Mirror, Level* level) {
		SymCall("??0StructureDataLoadHelper@@QEAA@AEBVBlockPos@@0AEBVVec3@@UActorUniqueID@@W4Rotation@@W4Mirror@@AEAVLevel@@@Z",
			this, bp1, bp2, v, id, Rotation, Mirror, level);
	}
	~StructureDataLoadHelper() {
		SymCall("??1StructureDataLoadHelper@@UEAA@XZ", this);
	}
};
#endif
struct StructureTemplate {
	char _this[216];
	StructureTemplate(const span<char>& s);
	~StructureTemplate();
	Tag* save();
	void load(Tag* t);
	void fromJson(const Json& value);
	void fillFromWorld(BlockSource* a2, BlockPos* a3, StructureSettings* a4);
	void placeInWorld(BlockSource* a2, BlockPalette* a3, BlockPos* a4, StructureSettings* a5);
};
