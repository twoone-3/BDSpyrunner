#include "NBT.h"

void serialize<CompoundTag>::write(const std::unique_ptr<CompoundTag>& item, BinaryStream* stream) {
	SymCall("?write@?$serialize@VCompoundTag@@@@SAXAEBVCompoundTag@@AEAVBinaryStream@@@Z",
		void, CompoundTag*, BinaryStream*)(item.get(), stream);
}
std::unique_ptr<CompoundTag> serialize<CompoundTag>::read(ReadOnlyBinaryStream* stream) {
	auto tag = CompoundTag::create();
	SymCall("?read@?$serialize@VCompoundTag@@@@SA?AVCompoundTag@@AEAVReadOnlyBinaryStream@@@Z",
		void, CompoundTag*, ReadOnlyBinaryStream*)(tag.get(), stream);
	return tag;
}
fifo_json ToJson(const ListTag& t) {
	fifo_json json = fifo_json::array();
	for (auto& c : t) {
		switch (t.getElementType()) {
		case Tag::Type::End:
			logger.error("EndTag in ListTag");
			break;
		case Tag::Type::Byte:
			json.push_back(c->asByteTag()->value());
			break;
		case Tag::Type::Short:
			json.push_back(c->asShortTag()->value());
			break;
		case Tag::Type::Int:
			json.push_back(c->asIntTag()->value());
			break;
		case Tag::Type::Int64:
			json.push_back(c->asInt64Tag()->value());
			break;
		case Tag::Type::Float:
			json.push_back(c->asFloatTag()->value());
			break;
		case Tag::Type::Double:
			json.push_back(c->asDoubleTag()->value());
			break;
		case Tag::Type::ByteArray:
			logger.error("ByteArrayTag in ListTag");
			break;
		case Tag::Type::String:
			json.push_back(c->asStringTag()->value());
			break;
		case Tag::Type::List:
			json.push_back(ToJson(*c->asListTag()));
			break;
		case Tag::Type::Compound:
			json.push_back(ToJson(*c->asCompoundTag()));
			break;
		}
	}
	return json;
}
fifo_json ToJson(const CompoundTag& t) {
	fifo_json json = fifo_json::object();
	for (auto& [key, val] : t) {
		Tag* tag = const_cast<Tag*>(val.get());
		Tag::Type type = tag->getTagType();
		fifo_json& son = json[key + std::to_string(static_cast<uint32_t>(type))];
		switch (type) {
		case Tag::Type::End:
			logger.error("EndTag in CompoundTag");
			break;
		case Tag::Type::Byte:
			son = tag->asByteTag()->value();
			break;
		case Tag::Type::Short:
			son = tag->asShortTag()->value();
			break;
		case Tag::Type::Int:
			son = tag->asIntTag()->value();
			break;
		case Tag::Type::Int64:
			son = tag->asInt64Tag()->value();
			break;
		case Tag::Type::Float:
			son = tag->asFloatTag()->value();
			break;
		case Tag::Type::Double:
			son = tag->asDoubleTag()->value();
			break;
		case Tag::Type::ByteArray:
			for (size_t i = 0; i < tag->asByteArrayTag()->value().size; ++i) {
				auto c = tag->asByteArrayTag()->value().data[i];
				son.push_back(c);
			}
			break;
		case Tag::Type::String:
			son = tag->asStringTag()->value();
			break;
		case Tag::Type::List:
			son = ToJson(*tag->asListTag());
			break;
		case Tag::Type::Compound:
			son = ToJson(*tag->asCompoundTag());
			break;
		case Tag::Type::IntArray:
			logger.error("IntArrayTag in CompoundTag");
			break;
		default:
			break;
		}
	}
	return json;
}
unique_ptr<ListTag> ToListTag(const fifo_json& value) {
	auto list = ListTag::create();
	for (auto& x : value) {
		switch (x) {
		case json_t::null:
			break;
		case json_t::object:
			list->add(ToCompoundTag(x));
			break;
		case json_t::array:
			list->add(ToListTag(x));
			break;
		case json_t::string:
			list->addString(x.get<string>());
			break;
		case json_t::boolean:
			list->addByte(x.get<bool>());
			break;
		case json_t::number_integer:
		case json_t::number_unsigned:
			list->addInt(x.get<int>());
			break;
		case json_t::number_float:
			list->addDouble(x.get<double>());
			break;
		case json_t::binary:
			break;
		case json_t::discarded:
			break;
		default:
			break;
		}
	}
	return list;
}
unique_ptr<CompoundTag> ToCompoundTag(const fifo_json& value) {
	unique_ptr<CompoundTag> c = CompoundTag::create();
	for (auto& [key, val] : value.items()) {
		string new_key = key;
		char& e = new_key.back();
		Tag::Type type;
		if (*(&e - 1) == '1' && e == '0') {
			type = Tag::Type::Compound;
			new_key.pop_back();
			new_key.pop_back();
		}
		else if (e >= '0' && e <= '9') {
			type = Tag::Type(e - '0');
			new_key.pop_back();
		}
		else continue;
		//cout << key << " - " << type << endl;
		switch (type) {
		case Tag::Type::End:
			break;
		case Tag::Type::Byte:
			c->putByte(new_key, val.get<uint8_t>());
			break;
		case Tag::Type::Short:
			c->putShort(new_key, val.get<short>());
			break;
		case Tag::Type::Int:
			c->putInt(new_key, val.get<int>());
			break;
		case Tag::Type::Int64:
			c->putInt64(new_key, val.get<long long>());
			break;
		case Tag::Type::Float:
			c->putFloat(new_key, val.get<float>());
			break;
		case Tag::Type::Double:
			c->putFloat(new_key, val.get<float>());
			break;
		case Tag::Type::ByteArray:
		{
			size_t size = val.size();
			char* data = new char[size];
			for (unsigned i = 0; i < size; ++i) {
				data[i] = val[i].get<char>();
			}
			TagMemoryChunk tmc(data, size);
			c->putByteArray(new_key, tmc);
			break;
		}
		case Tag::Type::String:
			c->putString(new_key, val.get<string>());
			break;
		case Tag::Type::List:
			c->put(new_key, ToListTag(val));
			break;
		case Tag::Type::Compound:
			c->put(new_key, ToCompoundTag(val));
			break;
		case Tag::Type::IntArray:
			break;
		default:
			break;
		}
	}
	return c;
}
