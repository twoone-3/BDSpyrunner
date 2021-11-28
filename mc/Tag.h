#pragma once
#include "json_tool.h"
#include "DataIO.h"



enum class TagType : uint8_t {
	End, Byte, Short, Int, Int64, Float, Double,
	ByteArray, String, List, Compound, IntArray
};
struct TagMemoryChunk {
	size_t capacity = 0;
	size_t size = 0;
	std::unique_ptr<uint8_t[]> data;

	TagMemoryChunk(size_t size, uint8_t data[]);
};
//所有Tag的合体
struct Tag {
	void* vtable;
	char data[32];

	void put(const std::string& key, Tag* value);
	void putByte(const std::string& key, uint8_t value);
	void putShort(const std::string& key, short value);
	void putString(const std::string& key, const std::string& value);
	void putInt(const std::string& key, int value);
	void putInt64(const std::string& key, long long value);
	void putFloat(const std::string& key, float value);
	void putByteArray(const std::string& key, const TagMemoryChunk& value);
	void putCompound(const std::string& key, const Tag* value);

	void add(Tag* t);
	void deleteCompound();
	void deleteList();
	void deleteString();

	TagType getVariantType();
	TagType getListType();
	auto& asByte();
	auto& asShort();
	auto& asInt();
	auto& asInt64();
	auto& asFloat();
	auto& asDouble();
	std::string& asString();
	auto& asByteArray();
	auto& asList();
	auto& asCompound();
};
typedef Tag CompoundTag;

Tag* newTag(TagType t);
Json ListtoJson(Tag* t);
Json CompoundTagtoJson(Tag* t);
Tag* ObjecttoTag(const Json& value);
Tag* ArraytoTag(const Json& value);

template<> class serialize<CompoundTag>
{
public:
	static void write(const CompoundTag* val, BinaryStream* stream);
	static CompoundTag* read(ReadOnlyBinaryStream* stream);
};
