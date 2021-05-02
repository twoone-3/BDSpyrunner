#include "pch.h"
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
		}
	}
	return move(value);
}
Json::Value toJson(Tag* t) {
	Json::Value value(Json::objectValue);
	for (auto& x : t->asCompound()) {
		TagType type = x.second.getVariantType();
		Json::Value& son = value[x.first + to_string(type)];
		switch (type) {
		case End:
			break;
		case Byte:
			son = x.second.asByte();
			break;
		case Short:
			son = x.second.asShort();
			break;
		case Int:
			son = x.second.asInt();
			break;
		case Int64:
			son = x.second.asInt64();
			break;
		case Float:
			son = x.second.asFloat();
			break;
		case Double:
			son = x.second.asDouble();
			break;
		case ByteArray:
			son = Json::arrayValue;
			for (size_t i = 0; i < x.second.asByteArray().size_; ++i)
				son.append(x.second.asByteArray().data_[i]);
			break;
		case String:
			son = x.second.asString();
			break;
		case List:
			son = ListtoJson(&x.second);
			break;
		case Compound:
			son = toJson(&x.second);
			break;
		case IntArray:
			print("IntArrayTag");
			break;
		}
	}
	return move(value);
}
Tag* toTag(const Json::Value& value) {
	Tag* c = newTag(Compound);
	auto begin = value.begin();
	while (begin != value.end()) {
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
		case ByteArray:
		{
			size_t size = begin->size();
			unsigned char* data = new unsigned char[size];
			for (unsigned i = 0; i < size; ++i)
				data[i] = (unsigned char)begin->operator[](i).asInt();
			TagMemoryChunk tmc(size, data);
			c->putByteArray(key, tmc);
			break;
		}
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
