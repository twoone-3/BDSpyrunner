#include "NBT.h"
#include "Tool.h"


using namespace std;
json ToJson(ListTag& t) {
	json value(json_t::array);
	for (auto& c : t.value()) {
		switch (t.type) {
		case Tag::Type::End:
			break;
		case Tag::Type::Byte:
			value.push_back(c->asByteTag()->value());
			break;
		case Tag::Type::Short:
			value.push_back(c->asShortTag()->value());
			break;
		case Tag::Type::Int:
			value.push_back(c->asIntTag()->value());
			break;
		case Tag::Type::Int64:
			value.push_back(c->asInt64Tag()->value());
			break;
		case Tag::Type::Float:
			value.push_back(c->asFloatTag()->value());
			break;
		case Tag::Type::Double:
			value.push_back(c->asDoubleTag()->value());
			break;
		case Tag::Type::ByteArray:

			break;
		case Tag::Type::String:
			value.push_back(c->asStringTag()->value());
			break;
		case Tag::Type::List:
			value.push_back(ToJson(*c->asListTag()));
			break;
		case Tag::Type::Compound:
			value.push_back(ToJson(*c->asCompoundTag()));
			break;
		}
	}
	return value;
}

json ToJson(CompoundTag& t) {
	json value;
	for (auto& x : t.value()) {
		Tag::Type type = x.second.getTagType();
		json& son = value[x.first + to_string(static_cast<uint32_t>(type))];
		switch (type) {
		case Tag::Type::End:
			break;
		case Tag::Type::Byte:
			son = x.second.asByteTag()->value();
			break;
		case Tag::Type::Short:
			son = x.second.asShortTag()->value();
			break;
		case Tag::Type::Int:
			son = x.second.asIntTag()->value();
			break;
		case Tag::Type::Int64:
			son = x.second.asInt64Tag()->value();
			break;
		case Tag::Type::Float:
			son = x.second.asFloatTag()->value();
			break;
		case Tag::Type::Double:
			son = x.second.asDoubleTag()->value();
			break;
		case Tag::Type::ByteArray:
			for (size_t i = 0; i < x.second.asByteArrayTag()->value().size; ++i) {
				auto c = *x.second.asByteArrayTag()->value().data.get();
				son.push_back(c[i]);
			}
			break;
		case Tag::Type::String:
			son = x.second.asStringTag()->value();
			break;
		case Tag::Type::List:
			son = ToJson(*x.second.asListTag());
			break;
		case Tag::Type::Compound:
			son = ToJson(*x.second.asCompoundTag());
			break;
		case Tag::Type::IntArray:
			break;
		default:
			break;
		}
	}
	return value;
}

ListTag* ToListTag(const json& value) {
	ListTag* list = new ListTag;
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

CompoundTag* ToCompoundTag(const json& value) {
	CompoundTag* c = new CompoundTag;
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
		{
			ListTag* list = ToListTag(val);
			c->put(new_key, list->copyList());
			delete& list;
			break;
		}
		case Tag::Type::Compound:
		{
			CompoundTag* t = ToCompoundTag(val);
			c->put(new_key, t->copy());
			delete t;
			break;
		}
		case Tag::Type::IntArray:
			break;
		default:
			break;
		}
	}
	return c;
}
