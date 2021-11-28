#include "DataIO.h"
#include "tool.h"

BinaryStream::BinaryStream() {
	SymCall("??0BinaryStream@@QEAA@XZ",
		this);
}

BinaryStream::BinaryStream(std::string* buffer, bool copy) {
	SymCall("??0BinaryStream@@QEAA@AEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		this, buffer, copy);
}

std::string* BinaryStream::GetAndReleaseData() {
	return SymCall<std::string*>("?getAndReleaseData@BinaryStream@@QEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this);
}

void BinaryStream::writeUnsignedInt(unsigned int num)
{
	SymCall("?writeUnsignedInt@BinaryStream@@QEAAXI@Z", this, num);
}

BinaryStream::~BinaryStream() {
	SymCall("??1BinaryStream@@UEAA@XZ",
		this);
}


