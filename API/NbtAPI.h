#pragma once
#include <Global.hpp>
#include <MC/Tag.hpp>
#include <MC/ByteTag.hpp>
#include <MC/EndTag.hpp>
#include <MC/ShortTag.hpp>
#include <MC/IntTag.hpp>
#include <MC/Int64Tag.hpp>
#include <MC/FloatTag.hpp>
#include <MC/DoubleTag.hpp>
#include <MC/StringTag.hpp>
#include <MC/ByteArrayTag.hpp>
#include <MC/ListTag.hpp>
#include <MC/CompoundTag.hpp>

struct NbtClass {
	unique_ptr<Tag> thiz;

	NbtClass();
	NbtClass(unique_ptr<CompoundTag>&& other);
	NbtClass(unique_ptr<Tag>&& other);
	NbtClass(const NbtClass& other);
	NbtClass(NbtClass&& other) noexcept;
	//有三种模式
	// 1. a1数据类型 a2数据
	//    NBT('Int',3) NBT('Compound')
	// 2. SNBT模式 a2填SNBT字符串
	//    NBT('SNBT', snbt)
	// 3. 二进制模式 a2填bytes
	//    NBT('Binary', bytes)
	NbtClass(const string& type_str, const py::object& value);

	NbtClass __getitem__(int key);
	NbtClass __getitem__(const string& key);
	void __setitem__(const string& key, const NbtClass& val);
	unsigned char asByte();
	short asShort();
	int asInt();
	int64_t asInt64();
	float asFloat();
	double asDouble();
	string_view getType();
	py::bytes toBinary();
	string toJson(int indentatiton = 4);
	py::object toObject();
	string toSNBT();
	void append(const NbtClass& value);
};
