#include <Global.hpp>
#include "NBTAPI.h"
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

NBTClass::NBTClass() : thiz(nullptr), is_reference(false) {}

NBTClass::NBTClass(Tag* other) : thiz(other), is_reference(true) {}

NBTClass::NBTClass(const NBTClass& other) : thiz(other.thiz->copy()), is_reference(false) {}

NBTClass::NBTClass(NBTClass&& other) noexcept
	: thiz(move(other.thiz)), is_reference(other.is_reference) {}

NBTClass::~NBTClass() {
	if (is_reference)
		thiz.release();
}

NBTClass NBTClass::newEnd() {
	return EndTag::create();
}

NBTClass NBTClass::newByte(unsigned char value) {
	return ByteTag::create(value);
}

NBTClass NBTClass::newShort(short value) {
	return ShortTag::create(value);
}

NBTClass NBTClass::newInt(int value) {
	return IntTag::create(value);
}

NBTClass NBTClass::newInt64(int64_t value) {
	return Int64Tag::create(value);
}

NBTClass NBTClass::newFloat(float value) {
	return FloatTag::create(value);
}

NBTClass NBTClass::newDouble(double value) {
	return DoubleTag::create(value);
}

NBTClass NBTClass::newString(const string& value) {
	return StringTag::create(value);
}

NBTClass NBTClass::newByteArray(const py::bytearray& value) {
	string str(value);
	return ByteArrayTag::create(str.data(), str.size());
}

NBTClass NBTClass::newList() {
	return ListTag::create();
}

NBTClass NBTClass::newCompound() {
	auto a = CompoundTag::create();
	return a;
}

NBTClass NBTClass::newIntArray() {
	return IntArrayTag::create();
}

NBTClass NBTClass::fromSNBT(const string& snbt) {
	return CompoundTag::fromSNBT(snbt);
}

NBTClass NBTClass::fromBinary(const py::bytes& bytes) {
	return CompoundTag::fromBinaryNBT(bytes);
}

NBTClass NBTClass::__getitem__(int key) {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (thiz->getTagType() != Tag::List)
		throw py::type_error("NBT is not a list");
	return thiz->asListTag()->get(key);
}

NBTClass NBTClass::__getitem__(const string& key) {
	// logger.warn("key:{}", key);
	// logger.warn("val:{}", (void*)thiz->asCompoundTag()->get(key));
	// logger.warn(thiz->asCompoundTag()->toSNBT(4));
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (thiz->getTagType() != Tag::Compound)
		throw py::type_error("NBT is not a compound");
	return thiz->asCompoundTag()->operator[](key);
}

void NBTClass::__setitem__(const string& key, const NBTClass& val) {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (thiz->getTagType() != Tag::Compound)
		throw py::type_error("NBT is not a compound");
	if (!val.thiz)
		throw py::type_error("NBTClass is empty");
	thiz->asCompoundTag()->put(key, val.thiz->copy());
}

unsigned char NBTClass::asByte() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (getType() != Tag::Type::Byte)
		throw py::type_error("NBTClass is not a Byte");
	return thiz->asByteTag()->value();
}

short NBTClass::asShort() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (getType() != Tag::Type::Short)
		throw py::type_error("NBTClass is not a Short");
	return thiz->asShortTag()->value();
}

int NBTClass::asInt() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (getType() != Tag::Type::Int)
		throw py::type_error("NBTClass is not a Int");
	return thiz->asIntTag()->value();
}

int64_t NBTClass::asInt64() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (getType() != Tag::Type::Int64)
		throw py::type_error("NBTClass is not a Int64");
	return thiz->asInt64Tag()->value();
}

float NBTClass::asFloat() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (getType() != Tag::Type::Float)
		throw py::type_error("NBTClass is not a Float");
	return thiz->asFloatTag()->value();
}

double NBTClass::asDouble() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (getType() != Tag::Type::Double)
		throw py::type_error("NBTClass is not a Double");
	return thiz->asDoubleTag()->value();
}

string NBTClass::asString() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (getType() != Tag::Type::String)
		throw py::type_error("NBTClass is not a String");
	return thiz->asStringTag()->value();
}

Tag::Type NBTClass::getType() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	return thiz->getTagType();
}

py::bytes NBTClass::toBinary() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	return thiz->asCompoundTag()->toBinaryNBT();
}

string NBTClass::toJson(int indentatiton) {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	return thiz->toJson(indentatiton);
}

py::object NBTClass::toObject() {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	return py::eval(thiz->toJson(0));
}

string NBTClass::toSNBT(int indent, SnbtFormat snbtFormat) {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	return thiz->asCompoundTag()->toSNBT(indent, snbtFormat);
}

void NBTClass::append(const NBTClass& value) {
	if (!thiz)
		throw py::type_error("NBTClass is empty");
	if (thiz->getTagType() != Tag::List)
		throw py::type_error("NBT is not a list");
	// TODO: find out why it don't work
	thiz->asListTag()->add(value.thiz->copy());
}
