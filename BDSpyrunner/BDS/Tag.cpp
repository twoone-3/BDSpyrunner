#include "Tag.h"

inline TagMemoryChunk::TagMemoryChunk(size_t size, uint8_t data[]) :capacity(size), size(size), data(data) {}

inline void Tag::put(const string& key, Tag* value) {
	return SymCall("?put@CompoundTag@@QEAAAEAVTag@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$$QEAV2@@Z",
		this, key, value);
}

inline void Tag::putByte(const string& key, uint8_t value) {
	return SymCall("?putByte@CompoundTag@@QEAAAEAEV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@E@Z",
		this, key, value);
}

inline void Tag::putShort(const string& key, short value) {
	return SymCall("?putShort@CompoundTag@@QEAAAEAFV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@F@Z",
		this, key, value);
}

inline void Tag::putString(const string& key, const string& value) {
	return SymCall("?putString@CompoundTag@@QEAAAEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V23@0@Z",
		this, key, value);
}

inline void Tag::putInt(const string& key, int value) {
	return SymCall("?putInt@CompoundTag@@QEAAAEAHV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z",
		this, key, value);
}

inline void Tag::putInt64(const string& key, long long value) {
	return SymCall("?putInt64@CompoundTag@@QEAAAEA_JV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_J@Z",
		this, key, value);
}

inline void Tag::putFloat(const string& key, float value) {
	return SymCall("?putFloat@CompoundTag@@QEAAAEAMV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@M@Z",
		this, key, value);
}

inline void Tag::putByteArray(const string& key, const TagMemoryChunk& value) {
	return SymCall("?putByteArray@CompoundTag@@QEAAAEAUTagMemoryChunk@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U2@@Z",
		this, key, &value);
}

inline void Tag::putCompound(const string& key, const Tag* value) {
	return SymCall("?putCompound@CompoundTag@@QEAAPEAV1@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@3@@Z",
		this, key, &value);
}

inline void Tag::add(Tag* t) {
	SymCall("?add@ListTag@@QEAAXV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@@Z",
		this, &t);
}

inline void Tag::deleteCompound() {
	SymCall("??1CompoundTag@@UEAA@XZ", this);
}

inline void Tag::deleteList() {
	SymCall("??1ListTag@@UEAA@XZ", this);
}

inline TagType Tag::getVariantType() {
	return *((TagType*)this + 40);
}

inline TagType Tag::getListType() {
	return *((TagType*)this + 32);
}

inline auto& Tag::asByte() { return *reinterpret_cast<uint8_t*>(data); }

inline auto& Tag::asShort() { return *reinterpret_cast<short*>(data); }

inline auto& Tag::asInt() { return *reinterpret_cast<int*>(data); }

inline auto& Tag::asInt64() { return *reinterpret_cast<long long*>(data); }

inline auto& Tag::asFloat() { return *reinterpret_cast<float*>(data); }

inline auto& Tag::asDouble() { return *reinterpret_cast<double*>(data); }

inline auto& Tag::asString() { return *reinterpret_cast<string*>(data); }

inline auto& Tag::asByteArray() { return *reinterpret_cast<TagMemoryChunk*>(data); }

inline auto& Tag::asList() { return *reinterpret_cast<vector<Tag*>*>(data); }

inline auto& Tag::asCompound() { return *reinterpret_cast<map<string, Tag>*>(data); }

Tag* newTag(TagType t) {
	Tag* tag;
	SymCall("?newTag@Tag@@SA?AV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@W4Type@1@@Z",
		&tag, t);
	return tag;
}

json ListtoJson(Tag* t) {
	json value(JsonType::array);
	for (auto& c : t->asList()) {
		switch (t->getListType()) {
		case TagType::End:
			break;
		case TagType::Byte:
			value.push_back(c->asByte());
			break;
		case TagType::Short:
			value.push_back(c->asShort());
			break;
		case TagType::Int:
			value.push_back(c->asInt());
			break;
		case TagType::Int64:
			value.push_back(c->asInt64());
			break;
		case TagType::Float:
			value.push_back(c->asFloat());
			break;
		case TagType::Double:
			value.push_back(c->asDouble());
			break;
		case TagType::ByteArray:

			break;
		case TagType::String:
			value.push_back(c->asString());
			break;
		case TagType::List:
			value.push_back(ListtoJson(c));
			break;
		case TagType::Compound:
			value.push_back(CompoundTagtoJson(c));
			break;
		}
	}
	return value;
}

json CompoundTagtoJson(Tag* t) {
	json value;
	for (auto& x : t->asCompound()) {
		TagType type = x.second.getVariantType();
		json& son = value[x.first + std::to_string(static_cast<uint32_t>(type))];
		switch (type) {
		case TagType::End:
			break;
		case TagType::Byte:
			son = x.second.asByte();
			break;
		case TagType::Short:
			son = x.second.asShort();
			break;
		case TagType::Int:
			son = x.second.asInt();
			break;
		case TagType::Int64:
			son = x.second.asInt64();
			break;
		case TagType::Float:
			son = x.second.asFloat();
			break;
		case TagType::Double:
			son = x.second.asDouble();
			break;
		case TagType::ByteArray:
			for (size_t i = 0; i < x.second.asByteArray().size; ++i)
				son.push_back(x.second.asByteArray().data[i]);
			break;
		case TagType::String:
			son = x.second.asString();
			break;
		case TagType::List:
			son = ListtoJson(&x.second);
			break;
		case TagType::Compound:
			son = CompoundTagtoJson(&x.second);
			break;
		case TagType::IntArray:
			break;
		default:
			break;
		}
	}
	return value;
}

Tag* ObjecttoTag(const json& value) {
	Tag* c = newTag(TagType::Compound);
	for (auto& [key, val] : value.items()) {
		string new_key = key;
		char& e = new_key.back();
		TagType type;
		if (*(&e - 1) == '1' && e == '0') {
			type = TagType::Compound;
			new_key.pop_back();
			new_key.pop_back();
		}
		else if (e >= '0' && e <= '9') {
			type = TagType(e - '0');
			new_key.pop_back();
		}
		else continue;
		//cout << key << " - " << type << endl;
		switch (type) {
		case TagType::End:
			break;
		case TagType::Byte:
			c->putByte(new_key, val.get<uint8_t>());
			break;
		case TagType::Short:
			c->putShort(new_key, val.get<short>());
			break;
		case TagType::Int:
			c->putInt(new_key, val.get<int>());
			break;
		case TagType::Int64:
			c->putInt64(new_key, val.get<long long>());
			break;
		case TagType::Float:
			c->putFloat(new_key, val.get<float>());
			break;
		case TagType::Double:
			c->putFloat(new_key, (val.get<float>()));
			break;
		case TagType::ByteArray: {
			size_t size = val.size();
			uint8_t* data = new uint8_t[size];
			for (unsigned i = 0; i < size; ++i)
				data[i] = uint8_t(val[i].get<int>());
			TagMemoryChunk tmc(size, data);
			c->putByteArray(new_key, tmc);
			break;
		}
		case TagType::String:
			c->putString(new_key, val.get<string>());
			break;
		case TagType::List: {
			Tag* list = ArraytoTag(val);
			c->put(new_key, list);
			list->deleteList();
			delete list;
			break;
		}
		case TagType::Compound: {
			Tag* t = ObjecttoTag(val);
			c->putCompound(new_key, t);
			//delete t;
			break;
		}
		case TagType::IntArray:
			break;
		default:
			break;
		}
	}
	return c;
}

Tag* ArraytoTag(const json& value) {
	Tag* list = newTag(TagType::List);
	Tag* tag = nullptr;
	for (auto& x : value) {
		switch (x) {
		case JsonType::null:
			break;
		case JsonType::object:
			tag = ObjecttoTag(x);
			break;
		case JsonType::array:
			tag = ArraytoTag(x);
			break;
		case JsonType::string:
			tag = newTag(TagType::String);
			FETCH(string, tag->data) = x.get<string>();
			break;
		case JsonType::boolean:
			break;
		case JsonType::number_integer:
		case JsonType::number_unsigned:
			tag = newTag(TagType::Int);
			FETCH(int, tag->data) = x.get<int>();
			break;
		case JsonType::number_float:
			tag = newTag(TagType::Double);
			FETCH(double, tag->data) = x.get<double>();
			break;
		case JsonType::binary:
			break;
		case JsonType::discarded:
			break;
		default:
			break;
		}
		list->add(tag);
	}
	return list;
}
