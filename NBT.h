#pragma once
#include "pch.h"
enum TagType : char {
	End, Byte, Short, Int, Int64, Float, Double,
	ByteArray, String, List, Compound, IntArray
};
struct Tag {
	void* _vftable;
	char _this[24];

	void put(const string& key, const Tag* value) {
		return SYMCALL("?put@CompoundTag@@QEAAAEAVTag@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$$QEAV2@@Z",
			this, key, value);
	}
	void putByte(const string& key, const unsigned char value) {
		return SYMCALL("?putByte@CompoundTag@@QEAAAEAEV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@E@Z",
			this, key, value);
	}
	void putShort(const string& key, const short value) {
		return SYMCALL("?putShort@CompoundTag@@QEAAAEAFV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@F@Z",
			this, key, value);
	}
	void putString(const string& key, const string& value) {
		return SYMCALL("?putString@CompoundTag@@QEAAAEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V23@0@Z",
			this, key, value);
	}
	void putInt(const string& key, const int& value) {
		return SYMCALL("?putInt@CompoundTag@@QEAAAEAHV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z",
			this, key, value);
	}
	void putInt64(const string& key, const int64_t& value) {
		return SYMCALL("?putInt64@CompoundTag@@QEAAAEA_JV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_J@Z",
			this, key, value);
	}
	void putFloat(const string& key, const float& value) {
		return SYMCALL("?putFloat@CompoundTag@@QEAAAEAMV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@M@Z",
			this, key, value);
	}
	void putCompound(const string& key, const Tag* value) {
		return SYMCALL("?putCompound@CompoundTag@@QEAAPEAV1@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@3@@Z",
			this, key, &value);
	}

	void add(Tag* t) {
		SYMCALL("?add@ListTag@@QEAAXV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@@Z",
			this, &t);
	}
	void deCompound() {
		SYMCALL("??1CompoundTag@@UEAA@XZ", this);
	}
	void deList() {
		SYMCALL("??1ListTag@@UEAA@XZ", this);
	}

	TagType getVariantType() {
		return *((TagType*)this + 40);
	}
	TagType getListType() {
		return *((TagType*)this + 32);
	}
	auto& asByte() { return *(unsigned char*)((VA)this + 8); }
	auto& asShort() { return *(short*)((VA)this + 8); }
	auto& asInt() { return *(int*)((VA)this + 8); }
	auto& asInt64() { return *(long long*)((VA)this + 8); }
	auto& asFloat() { return *(float*)((VA)this + 8); }
	auto& asDouble() { return *(double*)((VA)this + 8); }
	auto& asString() { return *(string*)((VA)this + 8); }
	auto asListTag() { return (Tag*)this; }
	auto& asList() { return *(vector<Tag*>*)((VA)this + 8); }
	auto& asCompound() { return *(map<string, Tag>*)((VA)this + 8); }
};

Json::Value ListtoJson(Tag* t);
Json::Value toJson(Tag* t);
Tag* toTag(const Json::Value& j);
Tag* ArraytoTag(const Json::Value& j);

Tag* newTag(TagType t) {
	Tag* tag;
	SYMCALL("?newTag@Tag@@SA?AV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@W4Type@1@@Z",
		&tag, t);
	return tag;
}
Json::Value ListtoJson(Tag* t) {
	Json::Value value(Json::arrayValue);
	for (auto& c : t->asList()) {
		switch (t->getListType()) {
		case End:
			break;
		case Byte:
			value.append(c->asByte());
			break;
		case Short:
			value.append(c->asShort());
			break;
		case Int:
			value.append(c->asInt());
			break;
		case Int64:
			value.append(c->asInt64());
			break;
		case Float:
			value.append(c->asFloat());
			break;
		case Double:
			value.append(c->asDouble());
			break;
		case ByteArray:break;
		case String:
			value.append(c->asString());
			break;
		case List:
			value.append(ListtoJson(c));
			break;
		case Compound:
			value.append(toJson(c));
			break;
		default:
			puts("default");
		}
	}
	return move(value);
}
Json::Value toJson(Tag* t) {
	Json::Value value(Json::objectValue);
	for (auto& x : t->asCompound()) {
		TagType type = x.second.getVariantType();
		string key(x.first + to_string(type));
		switch (type) {
		case End:
			break;
		case Byte:
			value[key] = x.second.asByte();
			break;
		case Short:
			value[key] = x.second.asShort();
			break;
		case Int:
			value[key] = x.second.asInt();
			break;
		case Int64:
			value[key] = x.second.asInt64();
			break;
		case Float:
			value[key] = x.second.asFloat();
			break;
		case Double:
			value[key] = x.second.asDouble();
			break;
		case ByteArray:break;
		case String:
			value[key] = x.second.asString();
			break;
		case List:
			value[key] = ListtoJson(&x.second);
			break;
		case Compound:
			value[key] = toJson(&x.second);
			break;
		}
	}
	return move(value);
}
Tag* toTag(const Json::Value& value) {
	Tag* c = newTag(Compound);
	auto begin = value.begin();
	auto end = value.end();
	while (begin != end) {
		string key = begin.name();
		char& e = key.back();
		int type = 0;
		if (*(&e - 1) == '1' && e == '0') {
			type = 10;
			key.resize(key.length() - 2);
		}
		else if (e >= '0' && e <= '9') {
			type = e - '0';
			key.pop_back();
		}
		else continue;
		//cout << key << " - " << type << endl;
		switch (type) {
		case End:break;
		case Byte:
			c->putByte(key, (unsigned char)begin->asInt());
			break;
		case Short:
			c->putShort(key, (short)begin->asInt());
			break;
		case Int:
			c->putInt(key, begin->asInt());
			break;
		case Int64:
			c->putInt64(key, begin->asInt64());
			break;
		case Float:
			c->putFloat(key, begin->asFloat());
			break;
		case Double:
			c->putFloat(key, (float)begin->asDouble());
			break;
		case ByteArray:break;
		case String:
			c->putString(key, begin->asString());
			break;
		case List:
		{
			Tag* lt = ArraytoTag(*begin);
			c->put(key, lt);
			lt->deList();
			delete lt;
			break;
		}
		case Compound:
		{
			Tag* t = toTag(*begin);
			c->putCompound(key, t);
			//delete t;
			break;
		}
		}
		begin++;
	}
	return c;
}
Tag* ArraytoTag(const Json::Value& value) {
	Tag* l = newTag(List);
	for (auto& x : value) {
		switch (x.type()) {
		case Json::nullValue:break;
		case Json::intValue:
		case Json::uintValue:
		{
			Tag* t = newTag(Int);
			*(int*)t->_this = x.asInt();
			l->add(t);
			break;
		}
		case Json::realValue:
		{
			Tag* t = newTag(Double);
			*(double*)t->_this = x.asDouble();
			l->add(t);
			break;
		}
		case Json::stringValue:
		{
			Tag* t = newTag(String);
			*(string*)t->_this = x.asString();
			l->add(t);
			break;
		}
		case Json::arrayValue:
		{
			Tag* t = ArraytoTag(x);
			l->add(t);
			break;
		}
		case Json::objectValue:
		{
			Tag* t = toTag(x);
			l->add(t);
			break;
		}
		}
	}
	return l;
}
