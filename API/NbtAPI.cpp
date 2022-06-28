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
#include <MC/IntArrayTag.hpp>

NbtClass::NbtClass() : thiz(nullptr), is_reference(false) {}

NbtClass::NbtClass(Tag* other) : thiz(other), is_reference(true) {}

NbtClass::NbtClass(const NbtClass& other) : thiz(other.thiz->copy()), is_reference(false) {}

NbtClass::NbtClass(NbtClass&& other) noexcept
	: thiz(move(other.thiz)), is_reference(other.is_reference) {}

NbtClass::~NbtClass() {
	// logger.warn("析构：{}", (void*)thiz.get());
	if (is_reference)
		thiz.release();
}

NbtClass NbtClass::newEnd() {
	return EndTag::create();
}

NbtClass NbtClass::newByte(unsigned char value) {
	return ByteTag::create(value);
}

NbtClass NbtClass::newShort(short value) {
	return ShortTag::create(value);
}

NbtClass NbtClass::newInt(int value) {
	return IntTag::create(value);
}

NbtClass NbtClass::newInt64(int64_t value) {
	return Int64Tag::create(value);
}

NbtClass NbtClass::newFloat(float value) {
	return FloatTag::create(value);
}

NbtClass NbtClass::newDouble(double value) {
	return DoubleTag::create(value);
}

NbtClass NbtClass::newString(const string& value) {
	return StringTag::create(value);
}

NbtClass NbtClass::newByteArray(const py::bytearray& value) {
	string str(value);
	return ByteArrayTag::create(str.data(), str.size());
}

NbtClass NbtClass::newList() {
	return ListTag::create();
}

NbtClass NbtClass::newCompound() {
	auto a = CompoundTag::create();
	return a;
}

NbtClass NbtClass::newIntArray() {
	return IntArrayTag::create();
}

NbtClass NbtClass::fromSNBT(const string& snbt) {
	return CompoundTag::fromSNBT(snbt);
}

NbtClass NbtClass::fromBinary(const py::bytes& bytes) {
	return CompoundTag::fromBinaryNBT(bytes);
}

NbtClass NbtClass::__getitem__(int key) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (thiz->getTagType() != Tag::List)
		throw py::type_error("NBT is not a list");
	return thiz->asListTag()->get(key);
}

NbtClass NbtClass::__getitem__(const string& key) {
	// logger.warn("key:{}", key);
	// logger.warn("val:{}", (void*)thiz->asCompoundTag()->get(key));
	// logger.warn(thiz->asCompoundTag()->toSNBT(4));
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (thiz->getTagType() != Tag::Compound)
		throw py::type_error("NBT is not a compound");
	return thiz->asCompoundTag()->operator[](key);
}

void NbtClass::__setitem__(const string& key, const NbtClass& val) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (thiz->getTagType() != Tag::Compound)
		throw py::type_error("NBT is not a compound");
	if (!val.thiz)
		throw py::type_error("NbtClass is empty");
	thiz->asCompoundTag()->put(key, val.thiz->copy());
}

unsigned char NbtClass::asByte() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (getType() != Tag::Type::Byte)
		throw py::type_error("NbtClass is not a Byte");
	return thiz->asByteTag()->value();
}

short NbtClass::asShort() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (getType() != Tag::Type::Short)
		throw py::type_error("NbtClass is not a Short");
	return thiz->asShortTag()->value();
}

int NbtClass::asInt() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (getType() != Tag::Type::Int)
		throw py::type_error("NbtClass is not a Int");
	return thiz->asIntTag()->value();
}

int64_t NbtClass::asInt64() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (getType() != Tag::Type::Int64)
		throw py::type_error("NbtClass is not a Int64");
	return thiz->asInt64Tag()->value();
}

float NbtClass::asFloat() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (getType() != Tag::Type::Float)
		throw py::type_error("NbtClass is not a Float");
	return thiz->asFloatTag()->value();
}

double NbtClass::asDouble() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (getType() != Tag::Type::Double)
		throw py::type_error("NbtClass is not a Double");
	return thiz->asDoubleTag()->value();
}

string NbtClass::asString() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (getType() != Tag::Type::String)
		throw py::type_error("NbtClass is not a String");
	return thiz->asStringTag()->value();
}

Tag::Type NbtClass::getType() {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->getTagType();
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

string NbtClass::toSNBT(int indent, SnbtFormat snbtFormat) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	return thiz->asCompoundTag()->toSNBT(indent, snbtFormat);
}

void NbtClass::append(const NbtClass& value) {
	if (!thiz)
		throw py::type_error("NbtClass is empty");
	if (thiz->getTagType() != Tag::List)
		throw py::type_error("NBT is not a list");
	// TODO: find out why it don't work
	thiz->asListTag()->add(value.thiz->copy());
}
