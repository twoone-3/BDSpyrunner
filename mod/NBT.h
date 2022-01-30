#pragma once
#include "Tool.h"

template <typename T>
class serialize {
public:
	static void write(const std::unique_ptr<T>& val, BinaryStream* stream);
	static std::unique_ptr<T> read(ReadOnlyBinaryStream* stream);
};

fifo_json ListTagToJson(const ListTag& t);
fifo_json CompoundTagToJson(const CompoundTag& t);
std::unique_ptr<ListTag> ToListTag(const fifo_json& value);
std::unique_ptr<CompoundTag> ToCompoundTag(const fifo_json& value);

inline ItemStack LoadItemFromString(std::string_view str) {
	return ItemStack::fromTag(*ToCompoundTag(CompoundTagToJson(str)));
}
inline ItemStack LoadItemFromJson(fifo_json data) {
	return ItemStack::fromTag(*ToCompoundTag(data));
}
