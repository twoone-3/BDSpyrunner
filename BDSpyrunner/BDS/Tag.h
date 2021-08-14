#pragma once
#include "../tool.h"

enum class TagType : uint8_t {
	End, Byte, Short, Int, Int64, Float, Double,
	ByteArray, String, List, Compound, IntArray
};
struct TagMemoryChunk {
	size_t capacity = 0;
	size_t size = 0;
	unique_ptr<uint8_t[]> data;

	TagMemoryChunk(size_t size, uint8_t data[]);
};
//所有Tag的合体
struct Tag {
	void* vtable;
	char data[32];

	void put(const string& key, Tag* value);
	void putByte(const string& key, uint8_t value);
	void putShort(const string& key, short value);
	void putString(const string& key, const string& value);
	void putInt(const string& key, int value);
	void putInt64(const string& key, long long value);
	void putFloat(const string& key, float value);
	void putByteArray(const string& key, const TagMemoryChunk& value);
	void putCompound(const string& key, const Tag* value);

	void add(Tag* t);
	void deleteCompound();
	void deleteList();

	TagType getVariantType();
	TagType getListType();
	auto& asByte();
	auto& asShort();
	auto& asInt();
	auto& asInt64();
	auto& asFloat();
	auto& asDouble();
	auto& asString();
	auto& asByteArray();
	auto& asList();
	auto& asCompound();
};

Tag* newTag(TagType t);
json ListtoJson(Tag* t);
json CompoundTagtoJson(Tag* t);
Tag* ObjecttoTag(const json& value);
Tag* ArraytoTag(const json& value);
