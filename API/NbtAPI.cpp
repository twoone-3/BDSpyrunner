#include <Global.hpp>
#include "NbtAPI.h"
#include "ItemAPI.h"
#include "BlockAPI.h"
#include <MC/EndTag.hpp>
#include <MC/ByteTag.hpp>
#include <MC/ShortTag.hpp>
#include <MC/IntTag.hpp>
#include <MC/Int64Tag.hpp>
#include <MC/FloatTag.hpp>
#include <MC/DoubleTag.hpp>
#include <MC/ByteArrayTag.hpp>
#include <MC/StringTag.hpp>
#include <MC/ListTag.hpp>
#include <MC/CompoundTag.hpp>

using namespace std;

NbtClass::NbtClass() {}

NbtClass::NbtClass(unique_ptr<CompoundTag>&& other) { thiz = move(other); }

NbtClass::NbtClass(unique_ptr<Tag>&& other) { thiz = move(other); }

NbtClass::NbtClass(const NbtClass& other) { thiz = other.thiz->copy(); }

NbtClass::NbtClass(NbtClass&& other) noexcept { thiz = move(other.thiz); }

//有三种模式
// 1. a1数据类型 a2数据
//    NBT('Int',3) NBT('Compound')
// 2. SNBT模式 a2填SNBT字符串
//    NBT('SNBT', snbt)
// 3. 二进制模式 a2填bytes
//    NBT('Binary', bytes)

NbtClass::NbtClass(const string& type_str, const py::object& value) {
	if (type_str == "SNBT"sv) {
		thiz = CompoundTag::fromSNBT(py::str(value));
		return;
	}
	else if (type_str == "Binary"sv) {
		thiz = CompoundTag::fromBinaryNBT(py::bytes(value));
		return;
	}
	auto type = magic_enum::enum_cast<Tag::Type>(type_str);
	if (!type)
		throw py::type_error("Invalied NBT type " + type_str);
	switch (type.value()) {
	case Tag::End:
		thiz = EndTag::create();
		break;
	case Tag::Byte:
		thiz = ByteTag::create();
		thiz->asByteTag()->value() = py::int_(value);
		break;
	case Tag::Short:
		thiz = ShortTag::create();
		thiz->asShortTag()->value() = py::int_(value);
		break;
	case Tag::Int:
		thiz = IntTag::create();
		thiz->asIntTag()->value() = py::int_(value);
		break;
	case Tag::Int64:
		thiz = Int64Tag::create();
		thiz->asInt64Tag()->value() = py::int_(value);
		break;
	case Tag::Float:
		thiz = FloatTag::create();
		thiz->asFloatTag()->value() = py::float_(value);
		break;
	case Tag::Double:
		thiz = DoubleTag::create();
		thiz->asDoubleTag()->value() = py::float_(value);
		break;
	case Tag::ByteArray:
		thiz = ByteArrayTag::create();
		// TODO: enable to construct ByteArray
		break;
	case Tag::String:
		thiz = StringTag::create();
		thiz->asStringTag()->value() = py::str(value);
		break;
	case Tag::List:
		thiz = ListTag::create();
		break;
	case Tag::Compound:
		thiz = CompoundTag::create();
		break;
	case Tag::IntArray:
		thiz = IntArrayTag::create();
		break;
	}
}

NbtClass NbtClass::__getitem__(int key) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (thiz->getTagType() != Tag::List)
		throw py::type_error("NBT is not a list");
	return thiz->asListTag()->operator[](key)->copy();
}

NbtClass NbtClass::__getitem__(const string& key) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (thiz->getTagType() != Tag::Compound)
		throw py::type_error("NBT is not a compound");
	return thiz->asCompoundTag()->operator[](key)->copy();
}

void NbtClass::__setitem__(const string& key, const NbtClass& val) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	thiz->asCompoundTag()->put(key, val.thiz->copy());
}

unsigned char NbtClass::asByte() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asByteTag()->value();
}

short NbtClass::asShort() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asShortTag()->value();
}

int NbtClass::asInt() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asIntTag()->value();
}

int64_t NbtClass::asInt64() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asInt64Tag()->value();
}

float NbtClass::asFloat() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asFloatTag()->value();
}

double NbtClass::asDouble() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asDoubleTag()->value();
}

string_view NbtClass::getType() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return magic_enum::enum_name(thiz->getTagType());
}

py::bytes NbtClass::toBinary() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asCompoundTag()->toBinaryNBT();
}

string NbtClass::toJson(int indentatiton) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->toJson(indentatiton);
}

py::object NbtClass::toObject() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return py::eval(thiz->toJson(0));
}

string NbtClass::toSNBT() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asCompoundTag()->toSNBT();
}

void NbtClass::append(const NbtClass& value) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (thiz->getTagType() != Tag::List)
		throw py::type_error("NBT is not a list");
	// TODO: find out why it don't work
	thiz->asListTag()->add(value.thiz->copy());
}
