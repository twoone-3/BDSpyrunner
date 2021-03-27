// Jsob.h version 1.3.3 made by twoone3
// Github: https://github.com/twoone-3/Json
#pragma once
#include <iostream>
#include <string>
#include <map>
#define JSON_CHECK(x) if(!x)return false

namespace Json {
using std::move;

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
		std::swap(data_, other.data_);
		std::swap(type_, other.type_);
	};

	~Value() { clear(); }

	Value& operator=(const Value& other) {
		return operator=(Value(other));
	};
	Value& operator=(Value&& other)noexcept {
		std::swap(data_, other.data_);
		std::swap(type_, other.type_);
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
		setObject();
		return data_.o->operator[](i);
	}
	Value& get(const String& i) {
		return operator[](i);
	}
	//取索引对应值
	Value& operator[](const size_t i) {
		setArray();
		return data_.a->operator[](i);
	}
	Value& get(const size_t i) {
		return operator[](i);
	}

	//*(T*)this
	template<typename T>
	T& as() { return *(T*)this; }

	bool& asBool() { return data_.b; }
	int& asInt() { return *(int*)this; }
	unsigned& asUInt() { return *(unsigned*)this; }
	long long& asInt64() { return data_.l; }
	double& asDouble() { return data_.d; }
	String& asString() { return *data_.s; }
	Array& asArray() { return *data_.a; }
	Object& asObject() { return *data_.o; }

	const bool& asBool()const { return data_.b; }
	const int& asInt()const { return *(int*)this; }
	const unsigned& asUInt()const { return *(unsigned*)this; }
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

	//新建一个字符串
	void setString() {
		if (type_ != Type::String) {
			clear();
			data_.s = new String;
			type_ = Type::String;
		}
	}
	//新建一个对象
	void setArray() {
		if (type_ != Type::Array) {
			clear();
			data_.a = new Array;
			type_ = Type::Array;
		}
	}
	//新建一个数组
	void setObject() {
		if (type_ != Type::Object) {
			clear();
			data_.o = new Object;
			type_ = Type::Object;
		}
	}

	//获取类型
	Type type()const { return type_; }

	//移除数组的指定成员
	bool remove(size_t i) {
		if (type_ != Type::Array)
			return false;
		size_t size = data_.a->size();
		if (i >= size)
			return false;
		for (; i < size - 1; ++i) {
			std::swap((*data_.a)[i], (*data_.a)[i + 1]);
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
		setArray();
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

	//转换成紧凑的字符串
	String toString()const;
	//转换成格式化的字符串
	String toStyledString()const;

private:
	Data data_;
	Type type_;
};
//用于解析JSON
class Reader {
	const char* ptr_;
	unsigned line_;
	String err_;
public:
	Reader(const char* s) :ptr_(s), line_(1) {}

	String& getErrorString() { return err_; }
	const char& readChar() { return *ptr_; }
	const char& readNextCharFront() { return *++ptr_; }
	const char& readNextCharBack() { return *ptr_++; }
	void nextChar() { ++ptr_; }
	void nextLine() { ++line_; }
	void addError(const char* s) {
		err_.append(s);
		err_.append(" at Line ");
		err_.append(std::to_string(line_));
	}

	bool readNull() {
		if (readNextCharFront() == 'u' && readNextCharFront() == 'l' && readNextCharFront() == 'l') {
			nextChar();
			return true;
		}
		else {
			addError("Miss 'null'");
			return false;
		}
	}
	bool readTrue(Value& value) {
		if (readNextCharFront() == 'r' && readNextCharFront() == 'u' && readNextCharFront() == 'e') {
			nextChar();
			value = true;
			return true;
		}
		else {
			addError("Miss 'true'");
			return false;
		}
	}
	bool readFalse(Value& value) {
		if (readNextCharFront() == 'a' && readNextCharFront() == 'l' && readNextCharFront() == 's' && readNextCharFront() == 'e') {
			nextChar();
			value = false;
			return true;
		}
		else {
			addError("Miss 'false'");
			return false;
		}
	}
	bool readNumber(Value& value) {
		char* end;
		double num = strtod(ptr_, &end);
		bool is_double = false;
		for (const char* tmp = ptr_; tmp != end; tmp++) {
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
		ptr_ = end;
		return true;
	}
	bool readString(String& s) {
		nextChar();
		char ch;
		while (true) {
			ch = readNextCharBack();
			switch (ch) {
			case'\0':
				addError("Miss '\"'");
				return false;
			case '\"':
				return true;
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
						if (readNextCharBack() != '\\') {
							addError("Invalid character");
							return false;
						}
						if (readNextCharBack() != 'u') {
							addError("Invalid character");
							return false;
						}
						unsigned tmp_u;
						readHex4(tmp_u);
						if (tmp_u < 0xDC00 || tmp_u > 0xDFFF) {
							addError("Invalid character");
							return false;
						}
						u = 0x10000 + (u - 0xD800) * 0x400 + (tmp_u - 0xDC00);
					}
					if (u > 0x10FFFF) {
						addError("Invalid character");
						return false;
					}
					encode_utf8(u, s);
					break;
				}
				default:
					addError("Invalid Escape character");
					return false;
				}
				break;
			default:
				s.push_back(ch);
				break;
			}
		}
		return true;
	}
	bool readString(Value& value) {
		value.setString();
		return readString(value.asString());
	}
	bool readArray(Value& value) {
		nextChar();
		JSON_CHECK(skipBlank());
		value.setArray();
		if (readChar() == ']') {
			nextChar();
			return true;
		}
		while (true) {
			JSON_CHECK(readValue(value.asArray()[value.asArray().size()]));
			JSON_CHECK(skipBlank());
			if (readChar() == ',') {
				nextChar();
			}
			else if (readChar() == ']') {
				nextChar();
				break;
			}
			else {
				addError("Miss ',' or ']'");
				return false;
			}
		}
		return true;
	}
	bool readObject(Value& value) {
		nextChar();
		JSON_CHECK(skipBlank());
		value.setObject();
		if (readChar() == '}') {
			nextChar();
			return true;
		}
		while (true) {
			JSON_CHECK(skipBlank());
			if (readChar() != '"') {
				addError("Miss '\"'");
				return false;
			}
			String key;
			JSON_CHECK(readString(key));
			JSON_CHECK(skipBlank());
			if (readChar() != ':') {
				addError("Miss ':'");
				return false;
			}
			nextChar();
			JSON_CHECK(readValue(value.asObject()[key]));
			JSON_CHECK(skipBlank());
			if (readChar() == ',')
				nextChar();
			else if (readChar() == '}') {
				nextChar();
				return true;
			}
			else {
				addError("Miss ',' or '}'");
				return false;
			}
		}
	}
	bool readValue(Value& value) {
		JSON_CHECK(skipBlank());
		switch (readChar()) {
		case '\0':
			addError("Invalid character");
			return false;
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
					while (readChar() != '*' || *(ptr_ + 1) != '/') {
						if (readChar() == '\0') {
							addError("Miss '*/'");
							return false;
						}
						if (readChar() == '\n')
							nextLine();
						nextChar();

					}
					nextChar();
				}
				else {
					addError("Invalid comment");
					return false;
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
			else {
				addError("Invalid charactor");
				return false;
			}
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
	String out_;
	size_t indent_ = 0;
public:
	String& getString() { return out_; }
	void writeNull() {
		out_.append("null", 4);
	}
	void writeBool(const bool value) {
		if (value)
			out_.append("true", 4);
		else
			out_.append("false", 5);
	}
	void writeInteger(const long long value) {
		out_.append(std::to_string(value));
	}
	void writeDouble(const double value) {
		char buf[16];
		snprintf(buf, 15, "%lf", value);
		String s = buf;
		while (s.back() == '0') {
			if (*(&s.back() - 1) != '.')
				s.pop_back();
			else break;
		}
		out_.append(s);
	}
	void writeString(const String& value) {
		out_.push_back('"');
		out_.append(value);
		out_.push_back('"');
	}
	void writeArray(const Array& value) {
		out_.push_back('[');
		if (!value.empty()) {
			for (auto& i : value) {
				writeValue(i.second);
				out_.push_back(',');
			}
			out_.pop_back();
		}
		out_.push_back(']');
	}
	void writeObject(const Object& value) {
		out_.push_back('{');
		if (!value.empty()) {
			for (auto& i : value) {
				out_.push_back('"');
				out_.append(i.first);
				out_.push_back('"');
				out_.push_back(':');
				writeValue(i.second);
				out_.push_back(',');
			}
			out_.pop_back();
		}
		out_.push_back('}');
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
		for (size_t i = indent_; i--;)
			out_.append("    ", 4);
	}
	void writeNewline() {
		out_.push_back('\n');
	}
	void writeStyledArray(const Array& value) {
		out_.push_back('[');
		if (!value.empty()) {
			writeNewline();
			++indent_;
			for (auto& i : value) {
				writeIndent();
				writeStyledValue(i.second);
				out_.push_back(',');
				writeNewline();
			}
			--indent_;
			out_.pop_back();
			out_.pop_back();
			writeNewline();
			writeIndent();
		}
		out_.push_back(']');
	}
	void writeStyledObject(const Object& value) {
		out_.push_back('{');
		if (!value.empty()) {
			writeNewline();
			++indent_;
			for (auto& i : value) {
				writeIndent();
				out_.push_back('"');
				out_.append(i.first);
				out_.push_back('"');
				out_.push_back(':');
				out_.push_back(' ');
				writeStyledValue(i.second);
				out_.push_back(',');
				writeNewline();
			}
			--indent_;
			out_.pop_back();
			out_.pop_back();
			writeNewline();
			writeIndent();
		}
		out_.push_back('}');
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
Value toJson(const String& str) {
	return toJson(str.c_str());
}
//封装函数-将JSON转字符串
String Value::toString()const {
	Writer w;
	w.writeValue(*this);
	return w.getString();
}
//封装函数-将JSON转格式化字符串
String Value::toStyledString()const {
	Writer w;
	w.writeStyledValue(*this);
	return w.getString();
}

std::ostream& operator<<(std::ostream& o, const Value& v) {
	return o << v.toStyledString();
}
} // namespace Json
