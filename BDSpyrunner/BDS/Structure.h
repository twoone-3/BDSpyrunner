#pragma once
#include "../pch.h"
#include "Math.h"
#include "NBT.h"
#include "Block.h"
struct string_span {
	size_t len;
	const char* str;
	string_span(const char* s) : len(strlen(s)), str(s) {}
	string_span(const std::string& s) : len(s.length()), str(s.c_str()) {}
};
struct StructureSettings {
	char _this[96];
	StructureSettings(BlockPos* size, bool IgnoreEntities, bool IgnoreBlocks) {
		SYMCALL("??0StructureSettings@@QEAA@XZ", this);
		FETCH(bool, _this + 32) = IgnoreEntities;
		FETCH(bool, _this + 34) = IgnoreBlocks;
		FETCH(BlockPos, _this + 36) = *size;
		FETCH(BlockPos, _this + 48) = { 0,0,0 };
	}
	~StructureSettings() {
		((string*)this)->~basic_string();
	}
};
#if 0
struct StructureDataLoadHelper {
	char _this[136];

	StructureDataLoadHelper(BlockPos* bp1, BlockPos* bp2, Vec3* v, VA id, char Rotation, char Mirror, Level* level) {
		SYMCALL("??0StructureDataLoadHelper@@QEAA@AEBVBlockPos@@0AEBVVec3@@UActorUniqueID@@W4Rotation@@W4Mirror@@AEAVLevel@@@Z",
			this, bp1, bp2, v, id, Rotation, Mirror, level);
	}
	~StructureDataLoadHelper() {
		SYMCALL("??1StructureDataLoadHelper@@UEAA@XZ", this);
	}
};
#endif
struct StructureTemplate {
	char _this[216];
	StructureTemplate(const string_span& s) {
		SYMCALL("??0StructureTemplate@@QEAA@V?$basic_string_span@$$CBD$0?0@gsl@@@Z",
			this, s);
	}
	~StructureTemplate() {
		SYMCALL("??1StructureTemplate@@QEAA@XZ", this);
	}
	Tag* save() {
		Tag* t = 0;
		SYMCALL<Tag*>("?save@StructureTemplateData@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			_this + 32, &t);
		return t;
	}
	void load(Tag* t) {
		SYMCALL<bool>("?load@StructureTemplateData@@QEAA_NAEBVCompoundTag@@@Z",
			_this + 32, t);
	}
	void fromJson(const Json::Value& value) {
		Tag* t = toTag(value);
		load(t);
		t->deCompound();
		delete t;
	}
	void fillFromWorld(BlockSource* a2, BlockPos* a3, StructureSettings* a4) {
		SYMCALL("?fillFromWorld@StructureTemplate@@QEAAXAEAVBlockSource@@AEBVBlockPos@@AEBVStructureSettings@@@Z",
			this, a2, a3, a4);
	}
	void placeInWorld(BlockSource* a2, BlockPalette* a3, BlockPos* a4, StructureSettings* a5) {
		SYMCALL("?placeInWorld@StructureTemplate@@QEBAXAEAVBlockSource@@AEBVBlockPalette@@AEBVBlockPos@@AEBVStructureSettings@@PEAVStructureTelemetryServerData@@_N@Z",
			this, a2, a3, a4, a5);
	}
};
#pragma endregion
