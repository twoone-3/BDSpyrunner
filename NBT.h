#pragma once
#include "pch.h"
enum TagType {
	End, Byte, Short, Int, Int64, Float,
	Double, ByteArray, String, List, Compound,
};
struct Tag {
	VA vftable;
	VA val[3];

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
	for (auto& c : t->asList()) {
		switch (*((char*)t + 32)) {
		case End:
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
			j.append(c->asInt64());
			break;
		case Float:
			j.append(c->asFloat());
			break;
		case Double:
			j.append(c->asDouble());
			break;
		case ByteArray:break;
		case String:
			j.append(c->asString());
			break;
		case List:
			j.append(ListtoJson(c));
			break;
		case Compound:
			j.append(toJson(c));
			break;
		}
	}
	return move(j);
}
Json::Value toJson(Tag* t) {
	Json::Value j;
	for (auto& x : t->asCompound()) {
		switch (x.second.type()) {
		case End:
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
		case ByteArray:break;
		case String:
			j[x.first + to_string(String)] = x.second.asString();
			break;
		case List:
			j[x.first + to_string(List)] = ListtoJson(&x.second);
			break;
		case Compound:
			j[x.first + to_string(Compound)] = toJson(&x.second);
			break;
		}
	}
	return move(j);
}
Tag* toTag(const Json::Value& j) {
	Tag* c = newTag(Compound);
	for (auto& x : j.getMemberNames()) {
		string key = x;
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
		switch (type) {
		case End:break;
		case Byte:
			c->putByte(key, (unsigned char)j[x].asInt());
			break;
		case Short:
			c->putShort(key, (short)j[x].asInt());
			break;
		case Int:
			c->putInt(key, j[x].asInt());
			break;
		case Int64:
			c->putInt64(key, j[x].asInt());
			break;
		case Float:
			c->putFloat(key, j[x].asFloat());
			break;
		case Double:
			c->putFloat(key, (float)j[x].asDouble());
			break;
		case ByteArray:break;
		case String:
			c->putString(key, j[x].asString());
			break;
		case List: {
			Tag* lt = ArraytoTag(j[x]);
			c->put(key, lt);
			lt->deList();
			delete lt;
			break;
		}
		case Compound: {
			Tag* t = toTag(j[x]);
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
	for (auto& x : j) {
		Json::ValueType type = x.type();
		switch (type) {
		case Json::nullValue:break;
		case Json::intValue:
		case Json::uintValue:
		{
			Tag* t = newTag(Int);
			*(int*)t->val = x.asInt();
			l->add(t);
			break;
		}
		case Json::realValue:
		{
			Tag* t = newTag(Double);
			*(double*)t->val = x.asDouble();
			l->add(t);
			break;
		}
		case Json::stringValue:
		{
			Tag* t = newTag(String);
			*(string*)t->val = x.asString();
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
