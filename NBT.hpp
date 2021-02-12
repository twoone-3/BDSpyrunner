#pragma once
#include "pch.h"
using namespace std;
enum TagType {
	End, Byte, Short, Int, Int64, Float,
	Double, ByteArray, String, List, Compound,
};
struct Tag {
	VA vftable;
	VA value[3];

	void put(const string& key, const Tag* value) {
		return SYMCALL(MSSYM_MD5_9185d61c497a96e9aa3e623cf3538e67,
			this, key, value);
	}
	void putByte(const string& key, const unsigned char value) {
		return SYMCALL(MSSYM_MD5_63cca7b57ad5118628b227fccd59a565,
			this, key, value);
	}
	void putShort(const string& key, const short value) {
		return SYMCALL(MSSYM_MD5_bac65ee588cb8573315172fb38b08236,
			this, key, value);
	}
	void putString(const string& key, const string& value) {
		return SYMCALL(MSSYM_MD5_5ca6e1c0087e756cb8fcbe37e98c5803,
			this, key, value);
	}
	void putInt(const string& key, const int& value) {
		return SYMCALL(MSSYM_MD5_644cc4dc2599f254165ee0e19c8054bd,
			this, key, value);
	}
	void putInt64(const string& key, const int64_t& value) {
		return SYMCALL(MSSYM_MD5_f69ea9befa2d8e1552b0e095dda6aa81,
			this, key, value);
	}
	void putFloat(const string& key, const float& value) {
		return SYMCALL(MSSYM_MD5_9659be8cc21e43b24ed3ca14ef7ad990,
			this, key, value);
	}
	void putBoolean(const string& key, const bool& value) {
		return SYMCALL(MSSYM_MD5_6e2786a1395292da975f5a53150e04dc,
			this, key, value);
	}
	void putCompound(const string& key, const Tag* value) {
		return SYMCALL(MSSYM_MD5_38ac198002a6675928bd29ecb9022e72,
			this, key, &value);
	}

	void add(Tag* t) {
		SYMCALL(MSSYM_B1QA3addB1AA7ListTagB2AAA6QEAAXVB2QDA6uniqueB1UA3ptrB1AA4VTagB2AAA1UB2QDA7defaultB1UA6deleteB1AA4VTagB3AAAA3stdB3AAAA3stdB3AAAA1Z,
			this, &t);
	}
	void deCompound() {
		SYMCALL(MSSYM_B2QQE121CompoundTagB2AAA4UEAAB1AA2XZ, this);
	}
	void deList() {
		SYMCALL(MSSYM_B2QQA81ListTagB2AAA4UEAAB1AA2XZ, this);
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
Tag* newTag(TagType t);
Json::Value ListtoJson(Tag* t);
Json::Value toJson(Tag* t);
Tag* toTag(const Json::Value& j);
Tag* ArraytoTag(const Json::Value& j);
Tag* newTag(TagType t) {
	Tag* tag = 0;
	SYMCALL(MSSYM_B1QA6newTagB1AA3TagB2AAA2SAB1QA2AVB2QDA6uniqueB1UA3ptrB1AA4VTagB2AAA1UB2QDA7defaultB1UA6deleteB1AA4VTagB3AAAA3stdB3AAAA3stdB2AAA6W4TypeB1AA11B2AAA1Z,
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
			j[x.first + to_string(List)] = ListtoJson((Tag*)&x.second);
			break;
		case Compound:
			j[x.first + to_string(Compound)] = toJson((Tag*)&x.second);
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
			c->putByte(key, j[x].asInt());
			break;
		case Short:
			c->putShort(key, j[x].asInt());
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
		case List:
		{
			Tag* lt = ArraytoTag(j[x]);
			c->put(key, lt);
			lt->deList();
			delete lt;
		}
		break;
		case Compound:
		{
			Tag* ccc = toTag(j[x]);
			c->putCompound(key, ccc);
		}
		break;
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
			*(int*)t->value = x.asInt();
			l->add(t);
			break;
		}
		case Json::realValue:
		{
			Tag* t = newTag(Double);
			*(double*)t->value = x.asDouble();
			l->add(t);
			break;
		}
		case Json::stringValue:
		{
			Tag* t = newTag(String);
			*(string*)t->value = x.asString();
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
			Tag* ccc = toTag(x);
			l->add(ccc);
			break;
		}
		}
	}
	return l;
}
