#include <Structure.h>
#include <Tag.h>
#include <tool.h>

using namespace std;
StructureSettings::StructureSettings(BlockPos* size, bool IgnoreEntities, bool IgnoreBlocks) {
	SymCall("??0StructureSettings@@QEAA@XZ", this);
	FETCH(bool, _this + 32) = IgnoreEntities;
	FETCH(bool, _this + 34) = IgnoreBlocks;
	FETCH(BlockPos, _this + 36) = *size;
	FETCH(BlockPos, _this + 48) = { 0,0,0 };
}

StructureSettings::~StructureSettings() {
	reinterpret_cast<string*>(this)->~basic_string();
}

StructureTemplate::StructureTemplate(const span<char>& s) {
	SymCall("??0StructureTemplate@@QEAA@V?$basic_string_span@$$CBD$0?0@gsl@@@Z",
		this, s);
}

StructureTemplate::~StructureTemplate() {
	SymCall("??1StructureTemplate@@QEAA@XZ", this);
}

Tag* StructureTemplate::save() {
	Tag* t = 0;
	SymCall<Tag*>("?save@StructureTemplateData@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
		_this + 32, &t);
	return t;
}

void StructureTemplate::load(Tag* t) {
	SymCall<bool>("?load@StructureTemplateData@@QEAA_NAEBVCompoundTag@@@Z",
		_this + 32, t);
}

void StructureTemplate::fromJson(const Json& value) {
	Tag* t = ObjecttoTag(value);
	load(t);
	t->deleteCompound();
	delete t;
}

void StructureTemplate::fillFromWorld(BlockSource* a2, BlockPos* a3, StructureSettings* a4) {
	SymCall("?fillFromWorld@StructureTemplate@@QEAAXAEAVBlockSource@@AEBVBlockPos@@AEBVStructureSettings@@@Z",
		this, a2, a3, a4);
}

void StructureTemplate::placeInWorld(BlockSource* a2, BlockPalette* a3, BlockPos* a4, StructureSettings* a5) {
	SymCall("?placeInWorld@StructureTemplate@@QEBAXAEAVBlockSource@@AEBVBlockPalette@@AEBVBlockPos@@AEBVStructureSettings@@PEAVStructureTelemetryServerData@@_N@Z",
		this, a2, a3, a4, a5);
}
