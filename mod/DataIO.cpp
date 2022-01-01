#include "DataIO.h"
#include "Tool.h"
#include "NBT.h"

void serialize<CompoundTag>::write(CompoundTag* item, BinaryStream* stream) {
	return SymCall("?write@?$serialize@VCompoundTag@@@@SAXAEBVCompoundTag@@AEAVBinaryStream@@@Z",
		item, stream);
}

std::unique_ptr<CompoundTag> serialize<CompoundTag>::read(BinaryStream* stream) {
	auto tag = CompoundTag::create();
	SymCall("?read@?$serialize@VCompoundTag@@@@SA?AVCompoundTag@@AEAVReadOnlyBinaryStream@@@Z",
		tag.get(), stream);
	return tag;
}

BinaryStream::BinaryStream() {
	SymCall("??0BinaryStream@@QEAA@XZ",
		this);
}

BinaryStream::BinaryStream(std::string* buffer, bool copy) {
	SymCall("??0BinaryStream@@QEAA@AEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		this, buffer, copy);
}

std::string BinaryStream::getAndReleaseData() {
	std::string data;
	SymCall("?getAndReleaseData@BinaryStream@@QEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &data);
	return data;
}

void BinaryStream::writeUnsignedInt(unsigned int num) {
	SymCall("?writeUnsignedInt@BinaryStream@@QEAAXI@Z", this, num);
}

BinaryStream::~BinaryStream() {
	SymCall("??1BinaryStream@@UEAA@XZ",
		this);
}

ReadOnlyBinaryStream::ReadOnlyBinaryStream() {}

ReadOnlyBinaryStream::ReadOnlyBinaryStream(std::string* buffer) {
	SymCall("??0ReadOnlyBinaryStream@@QEAA@$$QEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, buffer);
}

ReadOnlyBinaryStream::ReadOnlyBinaryStream(const std::string* buffer, bool copyBuffer) {
	SymCall("??0ReadOnlyBinaryStream@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		this, buffer, copyBuffer);
}

ReadOnlyBinaryStream::~ReadOnlyBinaryStream() {
	SymCall("??1ReadOnlyBinaryStream@@UEAA@XZ", this);
}
