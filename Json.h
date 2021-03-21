// Jsob.h version 1.3.0 made by twoone3
// Github: https://github.com/twoone-3/Json
// All development is aimed at learning
#pragma once
#include <iostream>
#include <string>
#include <map>
#pragma warning(disable:4996)
//格式化JSON字符串的缩进 可以为字符或字符串
#define JSON_INDENT '\t'
#define JSON_CHECK(x) if(!x)return false

namespace Json {
using std::move;
using std::swap;

//JSON类型枚举
enum class Type {
	Null,
	Bool,
	Integer,
	Double,
	String,
	Array,
	Object
};
//字符串类型
using String = std::string;
//数组类型
using Array = std::map<size_t, class Value>;
//对象类型
using Object = std::map<String, class Value>;
//数据联合
union Data {
	bool b;
	long long l;
	double d;
	String* s;
	Array* a;
	Object* o;
};

//封装函数-将JSON转字符串
String toString(const Value& value);
//封装函数-将JSON转格式化字符串
String toStyledString(const Value& value);

//JSON数据类型
class Value {
public:
	Value() : type_(Type::Null) {}
	Value(const bool b) : type_(Type::Bool) { data_.b = b; }
	Value(const int i) : type_(Type::Integer) { data_.l = i; }
	Value(const long long l) : type_(Type::Integer) { data_.l = l; }
	Value(const double d) : type_(Type::Double) { data_.d = d; }
	Value(const char* s) : type_(Type::String) { data_.s = new String(s); }
	Value(const String& s) : type_(Type::String) { data_.s = new String(s); }
	Value(const Array& a) : type_(Type::Array) { data_.a = new Array(a); }
	Value(const Object& o) : type_(Type::Object) { data_.o = new Object(o); }

	Value(const Value& other) {
		switch (other.type_) {
		case Type::Null:break;
		case Type::Bool:data_.b = other.data_.b; break;
		case Type::Integer:data_.l = other.data_.l; break;
		case Type::Double:data_.d = other.data_.d; break;
		case Type::String:
			data_.s = new String(*other.data_.s);
			break;
		case Type::Array:
			data_.a = new Array(*other.data_.a);
			break;
		case Type::Object:
			data_.o = new Object(*other.data_.o);
			break;
		}
		type_ = other.type_;
	};
	Value(Value&& other) {
		swap(data_, other.data_);
		swap(type_, other.type_);
	};

	~Value() { clear(); }

	Value& operator=(const Value& other) {
		return operator=(Value(other));
	};
	Value& operator=(Value&& other)noexcept {
		swap(data_, other.data_);
		swap(type_, other.type_);
		return *this;
	};

	//比较
	bool compare(const Value& other)const {
		if (type_ != other.type_)
			return false;
		switch (other.type_) {
		case Type::Null:break;
		case Type::Bool:return data_.b == other.data_.b;
		case Type::Integer:return data_.l == other.data_.l;
		case Type::Double:return data_.d == other.data_.d;
		case Type::String:return *data_.s == *other.data_.s;
		case Type::Array:return *data_.a == *other.data_.a;
		case Type::Object:return *data_.o == *other.data_.o;
		}
		return true;
	}
	bool operator==(const Value& other)const {
		return compare(other);
	}

	//取键对应值
	Value& operator[](const String& i) {
		if (type_ != Type::Object) {
			setNewObject();
		}
		return data_.o->operator[](i);
	}
	//取索引对应值
	Value& operator[](const size_t i) {
		if (type_ != Type::Array) {
			setNewArray();
		}
		return data_.a->operator[](i);
	}

	bool& asBool() { return data_.b; }
	int& asInt() { return *(int*)this; }
	long long& asInt64() { return data_.l; }
	double& asDouble() { return data_.d; }
	String& asString() { return *data_.s; }
	Array& asArray() { return *data_.a; }
	Object& asObject() { return *data_.o; }

	const bool& asBool()const { return data_.b; }
	const int& asInt()const { return *(int*)this; }
	const long long& asInt64()const { return data_.l; }
	const double& asDouble()const { return data_.d; }
	const String& asString()const { return *data_.s; }
	const Array& asArray()const { return *data_.a; }
	const Object& asObject()const { return *data_.o; }

	bool isNull()const { return type_ == Type::Null; }
	bool isBool()const { return type_ == Type::Bool; }
	bool isInteger()const { return type_ == Type::Integer; }
	bool isDouble()const { return type_ == Type::Double; }
	bool isNumber()const { return type_ == Type::Integer || type_ == Type::Double; }
	bool isString()const { return type_ == Type::String; }
	bool isArray()const { return type_ == Type::Array; }
	bool isObject()const { return type_ == Type::Object; }

	//转换成紧凑的字符串
	String toString()const {
		return Json::toString(*this);
	}
	//转换成格式化的字符串
	String toStyledString()const {
		return Json::toStyledString(*this);
	}

	//新建一个字符串
	void setNewString() {
		clear();
		data_.s = new String;
		type_ = Type::String;
	}
	//新建一个对象
	void setNewArray() {
		clear();
		data_.a = new Array;
		type_ = Type::Array;
	}
	//新建一个数组
	void setNewObject() {
		clear();
		data_.o = new Object;
		type_ = Type::Object;
	}

	//获取数据
	Data& data() { return data_; }
	const Data& data()const { return data_; }
	//获取类型
	Type& type() { return type_; }
	const Type& type()const { return type_; }

	//移除数组的指定成员
	bool remove(size_t i) {
		if (type_ != Type::Array)
			return false;
		size_t size = data_.a->size();
		if (i >= size)
			return false;
		for (; i < size - 1; ++i) {
			swap((*data_.a)[i], (*data_.a)[i + 1]);
		}
		data_.a->erase(size - 1);
		return true;
	}
	//移除对象的指定成员
	bool remove(const String& i) {
		if (type_ != Type::Object)
			return false;
		return data_.o->erase(i);
	}
	//向数组追加元素
	void append(const Value& j) {
		append(Value(j));
	}
	void append(Value&& j) {
		if (type_ != Type::Array) {
			setNewArray();
		}
		data_.a->emplace(data_.a->size(), j);
	}
	//获取大小
	size_t size()const {
		switch (type_) {
		case Type::Null:break;
		case Type::Bool:break;
		case Type::Integer:break;
		case Type::Double:break;
		case Type::String:break;
		case Type::Array:return data_.a->size();
		case Type::Object:return data_.o->size();
		}
		return 0;
	}
	//是否为空
	bool empty()const {
		switch (type_) {
		case Type::Null:return true;
		case Type::Bool:break;
		case Type::Integer:break;
		case Type::Double:break;
		case Type::String:break;
		case Type::Array:return data_.a->empty();
		case Type::Object:return data_.o->empty();
		}
		return false;
	}
	//是否拥有某个键
	bool has(const String& key)const {
		if (type_ != Type::Object)
			return false;
		return data_.o->find(key) != data_.o->end();
	}
	//清除内容
	void clear() {
		switch (type_) {
		case Type::Null:break;
		case Type::Bool:break;
		case Type::Integer:break;
		case Type::Double:break;
		case Type::String:delete data_.s; break;
		case Type::Array:delete data_.a; break;
		case Type::Object:delete data_.o; break;
		}
		type_ = Type::Null;
		data_.s = 0;
	}

private:
	Data data_;
	Type type_;
};
//用于解析JSON
class Reader {
	const char* ptr;
	unsigned line;
	String err;
public:
	Reader(const char* s) :ptr(s), line(1) {}

	String& getErrorString() { return err; }
	const char& readChar() { return *ptr; }
	const char& readNextCharFront() { return *++ptr; }
	const char& readNextCharBack() { return *ptr++; }
	void nextChar() { ++ptr; }
	void nextLine() { ++line; }
	//新增错误并返回false
	bool addError(const char* s) {
		err.append(s);
		err.append(" at Line ");
		err.append(std::to_string(line));
		return false;
	}

	bool readNull() {
		if (readNextCharFront() == 'u' && readNextCharFront() == 'l' && readNextCharFront() == 'l') {
			nextChar();
			return true;
		}
		else return addError("Miss 'null'");
	}
	bool readTrue(Value& value) {
		if (readNextCharFront() == 'r' && readNextCharFront() == 'u' && readNextCharFront() == 'e') {
			nextChar();
			value = true;
			return true;
		}
		else return addError("Miss 'true'");
	}
	bool readFalse(Value& value) {
		if (readNextCharFront() == 'a' && readNextCharFront() == 'l' && readNextCharFront() == 's' && readNextCharFront() == 'e') {
			nextChar();
			value = false;
			return true;
		}
		else return addError("Miss 'false'");
	}
	bool readNumber(Value& value) {
		char* end;
		double num = strtod(ptr, &end);
		bool is_double = false;
		for (const char* tmp = ptr; tmp != end; tmp++) {
			switch (*tmp) {
			case'.':
			case'e':
			case'E':
				is_double = true;
			}
		}
		if (is_double)
			value = num;
		else
			value = (long long)num;
		ptr = end;
		return true;
	}
	bool readString(String& s) {
		char ch = 0;
		nextChar();
		while (1) {
			ch = readNextCharBack();
			switch (ch) {
			case '\"':return true;
			case '\\':
				switch (readNextCharBack()) {
				case '\"': s += '\"'; break;
				case 'n': s += '\n'; break;
				case 'r': s += '\r'; break;
				case 't': s += '\t'; break;
				case 'f': s += '\f'; break;
				case 'b': s += '\b'; break;
				case '/': s += '/'; break;
				case '\\': s += '\\'; break;
				case 'u':
				{
					unsigned u = 0;
					readHex4(u);
					if (u >= 0xD800 && u <= 0xDBFF) {
						if (readNextCharBack() != '\\')
							return addError("Invalid character");
						if (readNextCharBack() != 'u')
							return addError("Invalid character");
						unsigned tmp_u;
						readHex4(tmp_u);
						if (tmp_u < 0xDC00 || tmp_u > 0xDFFF)
							return addError("Invalid character");
						u = 0x10000 + (u - 0xD800) * 0x400 + (tmp_u - 0xDC00);
					}
					if (u > 0x10FFFF)
						return addError("Invalid character");
					encode_utf8(u, s);
					break;
				}
				default:
					return addError("Invalid ESC");
				}
				break;
			default:
				//if (ch < ' ')return 0;
				s += ch;
				break;
			}
		}
		return true;
	}
	bool readString(Value& value) {
		value.data().s = new String();
		value.type() = Type::String;
		return readString(value.asString());
	}
	bool readArray(Value& value) {
		nextChar();
		value.setNewArray();
		JSON_CHECK(skipBlank());
		if (readChar() == ']') {
			nextChar();
			return true;
		}
		while (true) {
			JSON_CHECK(skipBlank());
			JSON_CHECK(readValue(value.asArray()[value.asArray().size()]));
			JSON_CHECK(skipBlank());
			if (readChar() == ',') {
				nextChar();
			}
			else if (readChar() == ']') {
				nextChar();
				break;
			}
			else return addError("Miss ',' or ']'");
		}
		return true;
	}
	bool readObject(Value& value) {
		nextChar();
		JSON_CHECK(skipBlank());
		value.setNewObject();
		if (readChar() == '}') {
			nextChar();
			return true;
		}
		while (true) {
			JSON_CHECK(skipBlank());
			if (readChar() != '"')
				return addError("Miss '\"'");
			String key;
			JSON_CHECK(readString(key));
			JSON_CHECK(skipBlank());
			if (readChar() != ':')
				return addError("Miss ':'");
			nextChar();
			JSON_CHECK(skipBlank());
			JSON_CHECK(readValue(value.asObject()[key]));
			JSON_CHECK(skipBlank());
			if (readChar() == ',')
				nextChar();
			else if (readChar() == '}') {
				nextChar();
				return true;
			}
			else return addError("Miss ',' or '}'");
		}
	}
	bool readValue(Value& value) {
		JSON_CHECK(skipBlank());
		switch (readChar()) {
		case 'n':return readNull();
		case 't':return readTrue(value);
		case 'f':return readFalse(value);
		case '[':return readArray(value);
		case '{':return readObject(value);
		case '"':return readString(value);
		default:return readNumber(value);
		}
		return true;
	}

	bool skipBlank() {
		while (true) {
			switch (readChar()) {
			case'\t':break;
			case'\n':nextLine(); break;
			case'\r':break;
			case' ':break;
			case'/':
				nextChar();
				//单行注释
				if (readChar() == '/') {
					nextChar();
					while (readChar() != '\n') {
						nextChar();
					}
					nextLine();
				}
				//多行注释
				else if (readChar() == '*') {
					nextChar();
					while (readChar() != '*' || *(ptr + 1) != '/') {
						if (readChar() == '\0')
							return addError("Miss '*/'");
						if (readChar() == '\n')
							nextLine();
						nextChar();

					}
					nextChar();
				}
				else {
					return addError("Invalid comment");
				}
				break;
			default:
				return true;
			}
			nextChar();
		}
		return true;
	}
	bool readHex4(unsigned& u) {
		u = 0;
		char ch = 0;
		for (int i = 0; i < 4; i++) {
			u <<= 4;
			ch = readNextCharBack();
			if (ch >= '0' && ch <= '9')
				u |= ch - '0';
			else if (ch >= 'a' && ch <= 'f')
				u |= ch - 'a' + 10;
			else if (ch >= 'A' && ch <= 'F')
				u |= ch - 'A' + 10;
			else return addError("Invalid charactor");
		}
		return true;
	}
	static void encode_utf8(unsigned& u, String& s) {
		if (u <= 0x7F)
			s += (u & 0xFF);
		else if (u <= 0x7FF) {
			s += (0xC0 | (0xFF & (u >> 6)));
			s += (0x80 | (0x3F & u));
		}
		else if (u <= 0xFFFF) {
			s += (0xE0 | (0xFF & (u >> 12)));
			s += (0x80 | (0x3F & (u >> 6)));
			s += (0x80 | (0x3F & u));
		}
		else {
			s += (0xF0 | (0xFF & (u >> 18)));
			s += (0x80 | (0x3F & (u >> 12)));
			s += (0x80 | (0x3F & (u >> 6)));
			s += (0x80 | (0x3F & u));
		}
	}
};
//用于生成JSON字符串
class Writer {
	String out;
	size_t indent;
public:
	Writer() :indent(0) {}

	String& getString() { return out; }
	void writeNull() {
		out.append("null", 4);
	}
	void writeBool(const bool value) {
		if (value)
			out.append("true", 4);
		else
			out.append("false", 5);
	}
	void writeInteger(const long long value) {
		out.append(std::to_string(value));
	}
	void writeDouble(const double value) {
		char buf[32];
		snprintf(buf, 31, "%15.15lf", value);
		String s = buf;
		while (s.back() == '0') {
			if (*(&s.back() - 1) != '.')
				s.pop_back();
			else break;
		}
		out.append(s);
	}
	void writeString(const String& value) {
		out.push_back('"');
		out.append(value);
		out.push_back('"');
	}
	void writeArray(const Array& value) {
		out.push_back('[');
		if (!value.empty()) {
			for (auto& i : value) {
				writeValue(i.second);
				out.push_back(',');
			}
			out.pop_back();
		}
		out.push_back(']');
	}
	void writeObject(const Object& value) {
		out.push_back('{');
		if (!value.empty()) {
			for (auto& i : value) {
				out.push_back('"');
				out.append(i.first);
				out.push_back('"');
				out.push_back(':');
				writeValue(i.second);
				out.push_back(',');
			}
			out.pop_back();
		}
		out.push_back('}');
	}
	void writeValue(const Value& value) {
		switch (value.type()) {
		case Type::Null:writeNull(); break;
		case Type::Bool:writeBool(value.asBool()); break;
		case Type::Integer: writeInteger(value.asInt64()); break;
		case Type::Double:writeDouble(value.asDouble()); break;
		case Type::String:writeString(value.asString()); break;
		case Type::Array:writeArray(value.asArray()); break;
		case Type::Object:writeObject(value.asObject()); break;
		}
	}
	void writeIndent() {
		for (size_t i = indent; i--;)
			out += (JSON_INDENT);
	}
	void writeNewline() {
		out.push_back('\n');
	}
	void writeStyledArray(const Array& value) {
		out.push_back('[');
		if (!value.empty()) {
			writeNewline();
			++indent;
			for (auto& i : value) {
				writeIndent();
				writeStyledValue(i.second);
				out.push_back(',');
				writeNewline();
			}
			--indent;
			out.pop_back();
			out.pop_back();
			writeNewline();
			writeIndent();
		}
		out.push_back(']');
	}
	void writeStyledObject(const Object& value) {
		out.push_back('{');
		if (!value.empty()) {
			writeNewline();
			++indent;
			for (auto& i : value) {
				writeIndent();
				out.push_back('"');
				out.append(i.first);
				out.push_back('"');
				out.push_back(':');
				out.push_back(' ');
				writeStyledValue(i.second);
				out.push_back(',');
				writeNewline();
			}
			--indent;
			out.pop_back();
			out.pop_back();
			writeNewline();
			writeIndent();
		}
		out.push_back('}');
	}
	void writeStyledValue(const Value& value) {
		switch (value.type()) {
		case Type::Null:writeNull(); break;
		case Type::Bool:writeBool(value.asBool()); break;
		case Type::Integer: writeInteger(value.asInt64()); break;
		case Type::Double:writeDouble(value.asDouble()); break;
		case Type::String:writeString(value.asString()); break;
		case Type::Array:writeStyledArray(value.asArray()); break;
		case Type::Object:writeStyledObject(value.asObject()); break;
		}
	}
};

//封装函数-将字符串转JSON
Value toJson(const char* str) {
	Value v;
	Reader r(str);
	if (!r.readValue(v))
		std::cerr << r.getErrorString() << std::endl;
	return move(v);
}
//封装函数-将JSON转字符串
String toString(const Value& value) {
	Writer w;
	w.writeValue(value);
	return w.getString();
}
//封装函数-将JSON转格式化字符串
String toStyledString(const Value& value) {
	Writer w;
	w.writeStyledValue(value);
	return w.getString();
}

std::ostream& operator<<(std::ostream& o, const Value& v) {
	return o << toStyledString(v);
}
} // namespace Json
