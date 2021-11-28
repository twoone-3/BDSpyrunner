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
	inline uint8_t& asByte() { return *reinterpret_cast<uint8_t*>(data); }
	inline short& asShort() { return *reinterpret_cast<short*>(data); }
	inline int& asInt() { return *reinterpret_cast<int*>(data); }
	inline long long& asInt64() { return *reinterpret_cast<long long*>(data); }
	inline float& asFloat() { return *reinterpret_cast<float*>(data); }
	inline double& asDouble() { return *reinterpret_cast<double*>(data); }
	inline std::string& asString() { return *reinterpret_cast<std::string*>(data); }
	inline TagMemoryChunk& asByteArray() { return *reinterpret_cast<TagMemoryChunk*>(data); }
	inline std::vector<Tag*>& asList() { return *reinterpret_cast<std::vector<Tag*>*>(data); }
	inline std::map<std::string, Tag>& asCompound() { return *reinterpret_cast<std::map<std::string, Tag>*>(data); }
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
