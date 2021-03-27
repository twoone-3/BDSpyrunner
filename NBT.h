#pragma once
#include "pch.h"
enum TagType {
	End, Byte, Short, Int, Int64, Float,
	Double, ByteArray, String, List, Compound,
};
struct Tag {
	void* vftable;
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

	char type() {
		return *((char*)this + 40);
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
	Tag* tag = 0;
	SYMCALL("?newTag@Tag@@SA?AV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@W4Type@1@@Z",
		&tag, t);
	return tag;
}
Json::Value ListtoJson(Tag* t) {
	Json::Value j;
	j.setArray();
	for (auto& c : t->asList()) {
		switch (*((char*)t + 32)) {
		case End:
			cout << "End" << endl;
			break;
		case Byte:
			j.append(c->asByte());
			break;
		case Short:
			j.append(c->asShort());
			break;
		case Int:
			j.append(c->asInt());
			break;
		case Int64:
			cout << "Int64" << endl;
			j.append(c->asInt64());
			break;
		case Float:
			j.append(c->asFloat());
			break;
		case Double:
			j.append(c->asDouble());
			break;
		case ByteArray:
			cout << "BtyeArray" << endl;
			break;
		case String:
			j.append(c->asString());
			break;
		case List:
			j.append(ListtoJson(c));
			break;
		case Compound:
			j.append(toJson(c));
			break;
		default:
			cout << "default" << endl;
		}
	}
	return move(j);
}
Json::Value toJson(Tag* t) {
	Json::Value j;
	j.setObject();
	for (auto& x : t->asCompound()) {
		switch (x.second.type()) {
		case End:
			cout << x.first << endl;
			break;
		case Byte:
			j[x.first + to_string(Byte)] = x.second.asByte();
			break;
		case Short:
			j[x.first + to_string(Short)] = x.second.asShort();
			break;
		case Int:
			j[x.first + to_string(Int)] = x.second.asInt();
			break;
		case Int64:
			j[x.first + to_string(Int64)] = x.second.asInt64();
			break;
		case Float:
			j[x.first + to_string(Float)] = x.second.asFloat();
			break;
		case Double:
			j[x.first + to_string(Double)] = x.second.asDouble();
			break;
		case ByteArray:
			cout << x.first << endl;
			break;
		case String:
			j[x.first + to_string(String)] = x.second.asString();
			break;
		case List:
			j[x.first + to_string(List)] = ListtoJson(&x.second);
			break;
		case Compound:
			j[x.first + to_string(Compound)] = toJson(&x.second);
			break;
		default:
			cout << x.first << endl;
		}
	}
	return move(j);
}
Tag* toTag(const Json::Value& j) {
	Tag* c = newTag(Compound);
	if (!j.isObject())
		return c;
	for (auto& x : j.asObject()) {
		string key = x.first;
		char& e = key.back();
		int type = 0;
		if (*(&e - 1) == '1' && e == '0') {
			type = 10;
			key.pop_back();
			key.pop_back();
		}
		else if (e >= '0' && e <= '9') {
			type = e - '0';
			key.pop_back();
		}
		else continue;
		switch (type) {
		case End:break;
		case Byte:
			c->putByte(key, (unsigned char)x.second.asInt64());
			break;
		case Short:
			c->putShort(key, (short)x.second.asInt64());
			break;
		case Int:
			c->putInt(key, x.second.asInt());
			break;
		case Int64:
			c->putInt64(key, x.second.asInt64());
			break;
		case Float:
			c->putFloat(key, (float)x.second.asDouble());
			break;
		case Double:
			c->putFloat(key, (float)x.second.asDouble());
			break;
		case ByteArray:break;
		case String:
			c->putString(key, x.second.asString());
			break;
		case List:
		{
			Tag* lt = ArraytoTag(x.second);
			c->put(key, lt);
			lt->deList();
			delete lt;
			break;
		}
		case Compound:
		{
			Tag* t = toTag(x.second);
			c->putCompound(key, t);
			//delete t;
			break;
		}
		}
	}
	return c;
}
Tag* ArraytoTag(const Json::Value& j) {
	Tag* l = newTag(List);
	if (!j.isArray())
		return l;
	for (auto& x : j.asArray()) {
		switch (x.second.type()) {
		case Json::Type::Null:break;
		case Json::Type::Integer:
		{
			Tag* t = newTag(Int);
			*(int*)t->_this = x.second.asInt();
			l->add(t);
			break;
		}
		case Json::Type::Double:
		{
			Tag* t = newTag(Double);
			*(double*)t->_this = x.second.asDouble();
			l->add(t);
			break;
		}
		case Json::Type::String:
		{
			Tag* t = newTag(String);
			*(string*)t->_this = x.second.asString();
			l->add(t);
			break;
		}
		case Json::Type::Array:
		{
			Tag* t = ArraytoTag(x.second);
			l->add(t);
			break;
		}
		case Json::Type::Object:
		{
			Tag* t = toTag(x.second);
			l->add(t);
			break;
		}
		}
	}
	return l;
}
