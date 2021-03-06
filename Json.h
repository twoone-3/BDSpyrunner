// Jsob.h version 1.1.0 made by twoone3
// Github: https://github.com/twoone-3/Json
// All development is aimed at learning
#pragma once
#include <iostream>
#include <string>
#include <map>
#pragma warning(disable:4996)
#define JSON_ADD_TAB for (unsigned num = tab; num; num--)s += '\t'
#define JSON_ASSERT(condition) if(!(condition))abort()
#define JSON_CHECK(x) if(!x)return false

using std::string;
using std::to_string;
using std::map;
using std::move;
using std::ostream;

class Json {
public:
	//数组类型定义
	using Array = map<size_t, Json>;
	//对象类型定义
	using Object = map<string, Json>;
	//值类型定义
	union Value {
		bool b;
		int i;
		long long l;
		double d;
		char* s;
		Array* a;
		Object* o;
	};
	//值类型枚举定义
	enum class Type {
		Null,
		Bool,
		Int,
		Int64,
		Double,
		String,
		Array,
		Object
	};
	//解析JSON类
	class Parser {
		const char* end;
		const char* start;
		unsigned line;
		string* err;
	public:
		Parser(const char* s, string* e) :end(s), start(s), line(1), err(e) {}
		const char*& data() { return end; }
		const char& read() { return *end; }
		const char& read_next() { return *++end; }
		const char& read_next_back() { return *end++; }
		void next() { ++end; }
		void add_line() { ++line; }
		//新增错误并返回false
		bool make_error(const char* s) {
			if (err) {
				string content(end - 7, end + 7);
				for (auto& c : content) {
					if (c == '\n' || c == '\t')c = ' ';
				}
				char tmp[256];
				sprintf(tmp, "%s at Line %d\n%s\n       ^~~~", s, line, content.c_str());
				err->assign(tmp);
			}
			return false;
		}
	};

	Json() : type_(Type::Null) { data_.b = 0; }
	Json(const bool b) : type_(Type::Bool) { data_.b = b; }
	Json(const int i) : type_(Type::Int) { data_.i = i; }
	Json(const long long l) : type_(Type::Int64) { data_.l = l; }
	Json(const double d) : type_(Type::Double) { data_.d = d; }
	Json(const char* s) : type_(Type::String) { data_.s = new char[strlen(s) + 1]; strcpy(data_.s, s); }
	Json(const string& s) : type_(Type::String) { data_.s = new char[s.length() + 1]; strcpy(data_.s, s.c_str()); }
	Json(const Array& a) : type_(Type::Array) { data_.a = new Array(a); }
	Json(const Object& o) : type_(Type::Object) { data_.o = new Object(o); }
	Json(const Json& j) {
		clear();
		switch (j.type_) {
		case Type::Null:break;
		case Type::Bool:data_.b = j.data_.b; break;
		case Type::Int:data_.i = j.data_.i; break;
		case Type::Int64:data_.l = j.data_.l; break;
		case Type::Double:data_.d = j.data_.d; break;
		case Type::String:
			data_.s = new char[strlen(j.data_.s) + 1];
			strcpy(data_.s, j.data_.s);
			break;
		case Type::Array:
			data_.a = new Array;
			*data_.a = *j.data_.a;
			break;
		case Type::Object:
			data_.o = new Object;
			*data_.o = *j.data_.o;
			break;
		}
		type_ = j.type_;
	};
	Json(Json&& j)noexcept {
		type_ = j.type_;
		data_.l = j.data_.l;
		j.type_ = Type::Null;
		j.data_.l = 0;
	};
	~Json()noexcept { clear(); }
	Json& operator=(const Json& j) {
		clear();
		switch (j.type_) {
		case Type::Null:break;
		case Type::Bool:data_.b = j.data_.b; break;
		case Type::Int:data_.i = j.data_.i; break;
		case Type::Int64:data_.l = j.data_.l; break;
		case Type::Double:data_.d = j.data_.d; break;
		case Type::String:data_.s = j.data_.s; break;
		case Type::Array:
			data_.a = new Array;
			*data_.a = *j.data_.a;
			break;
		case Type::Object:
			data_.o = new Object;
			*data_.o = *j.data_.o;
			break;
		}
		type_ = j.type_;
		return *this;
	};
	Json& operator=(Json&& j)noexcept {
		type_ = j.type_;
		data_.l = j.data_.l;
		j.type_ = Type::Null;
		j.data_.l = 0;
		return *this;
	};

	bool& asBool() { return data_.b; }
	int& asInt() { return data_.i; }
	long long& asLongLong() { return data_.l; }
	double& asDouble() { return data_.d; }
	char* asString() { return data_.s; }
	Array& asArray() { return *data_.a; }
	Object& asObject() { return *data_.o; }
	const bool& asBool()const { return data_.b; }
	const int& asInt()const { return data_.i; }
	const long long& asLongLong()const { return data_.l; }
	const double& asDouble()const { return data_.d; }
	const char* asString()const { return data_.s; }
	const Array& asArray()const { return *data_.a; }
	const Object& asObject()const { return *data_.o; }

	//转换成紧凑的字符串
	string toString()const {
		switch (type_) {
		case Type::Null:return "null";
		case Type::Bool:return data_.b ? "true" : "false";
		case Type::Int:return to_string(data_.i);
		case Type::Int64:return to_string(data_.l);
		case Type::Double:return double_to_string(data_.d);
		case Type::String:
		{
			string s;
			s += '"';
			s += data_.s;
			s += '"';
			return s;
		}
		case Type::Array:
		{
			string s;
			s += '[';
			if (data_.a->empty())
				return "[]";
			for (auto& i : *data_.a) {
				s += i.second.toString();
				s += ',';
			}
			s.pop_back();
			s += ']';
			return s;
		}
		case Type::Object:
		{
			string s;
			s += '{';
			if (data_.o->empty())
				return "{}";
			for (auto& i : *data_.o) {
				s += '"';
				s += i.first;
				s += '"';
				s += ':';
				s += i.second.toString();
				s += ',';
			}
			s.pop_back();
			s += '}';
			return s;
		}
		}
		return "";
	}
	//转换成格式化的字符串
	string toStyledString()const {
		static unsigned tab = 0;
		switch (type_) {
		case Type::Null:return "null";
		case Type::Bool:return data_.b ? "true" : "false";
		case Type::Int:return to_string(data_.i);
		case Type::Int64:return to_string(data_.l);
		case Type::Double:return double_to_string(data_.d);
		case Type::String:
		{
			string s;
			s += '"';
			s += data_.s;
			s += '"';
			return s;
		}
		case Type::Array:
		{
			string s;
			s += '[';
			if (data_.a->empty()) {
				return "[]";
			}
			s += '\n';
			tab++;
			for (auto& i : *data_.a) {
				JSON_ADD_TAB;
				s += i.second.toStyledString();
				s += ',';
				s += '\n';
			}
			tab--;
			s.resize(s.length() - 2);
			s += '\n';
			JSON_ADD_TAB;
			s += ']';
			return s;
		}
		case Type::Object:
		{
			string s;
			s += '{';
			if (data_.o->empty()) {
				return "{}";
			}
			s += '\n';
			tab++;
			for (auto& i : *data_.o) {
				JSON_ADD_TAB;
				s += '"';
				s += i.first;
				s += '"';
				s += ':';
				s += ' ';
				s += i.second.toStyledString();
				s += ',';
				s += '\n';
			}
			tab--;
			s.resize(s.length() - 2);
			s += '\n';
			JSON_ADD_TAB;
			s += '}';
			return s;
		}
		}
		return "";
	}

	//设置和初始化类型
	void setType(const Type type) {
		type_ = type;
		data_.i = 0;
		switch (type) {
		case Type::Null:break;
		case Type::Bool:break;
		case Type::Int:break;
		case Type::Int64:break;
		case Type::Double:break;
		case Type::String:break;
		case Type::Array:
			data_.a = new Array;
			break;
		case Type::Object:
			data_.o = new Object;
			break;
		}
	}
	//获取类型
	Type getType()const { return type_; }
	//交换内容
	void swap(Json& j) {
		Json tmp = move(j);
		j = move(*this);
		*this = move(tmp);
	}
	//移除数组的指定成员
	bool remove(size_t i) {
		JSON_ASSERT(type_ == Type::Array);
		size_t size = data_.a->size();
		if (i >= size)
			return false;
		for (; i < size - 1; ++i) {
			(*data_.a)[i].swap((*data_.a)[i + 1]);
		}
		data_.a->erase(size - 1);
		return true;
	}
	//移除对象的指定成员
	bool remove(const string& i) {
		JSON_ASSERT(type_ == Type::Object);
		return data_.o->erase(i);
	}
	//向数组追加元素
	void append(const Json& j) {
		JSON_ASSERT(type_ == Type::Array || type_ == Type::Null);
		if (type_ == Type::Null) {
			setType(Type::Array);
		}
		data_.a->emplace(data_.a->size(), j);
	}
	//获取大小
	size_t size()const {
		switch (type_) {
		case Type::Null:break;
		case Type::Bool:break;
		case Type::Int:break;
		case Type::Int64:break;
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
		case Type::Null:return 1;
		case Type::Bool:break;
		case Type::Int:break;
		case Type::Int64:break;
		case Type::Double:break;
		case Type::String:break;
		case Type::Array:return data_.a->empty();
		case Type::Object:return data_.o->empty();
		}
		return 0;
	}
	//是否拥有某个键
	bool has(const string& key)const {
		JSON_ASSERT(type_ == Type::Object);
		return data_.o->find(key) != data_.o->end();
	}
	//清除内容
	void clear() {
		switch (type_) {
		case Type::Null:break;
		case Type::Bool:break;
		case Type::Int:break;
		case Type::Int64:break;
		case Type::Double:break;
		case Type::String:
			delete[] data_.s;
			break;
		case Type::Array:
			delete data_.a;
			break;
		case Type::Object:
			delete data_.o;
			break;
		}
		type_ = Type::Null;
		data_.o = 0;
	}

	//取键对应值
	Json& operator[](const string& i) {
		JSON_ASSERT(type_ == Type::Object || type_ == Type::Null);
		if (type_ == Type::Null) {
			setType(Type::Object);
		}
		return (*data_.o)[i];
	}
	//取索引对应值
	Json& operator[](const size_t i) {
		JSON_ASSERT(type_ == Type::Array || type_ == Type::Null);
		if (type_ == Type::Null) {
			setType(Type::Array);
		}
		return (*data_.a)[i];
	}
	//访问键对应值
	const Json& operator[](const string& i)const {
		JSON_ASSERT(type_ == Type::Object);
		return (*data_.o)[i];
	}
	//访问索引对应值
	const Json& operator[](const size_t i)const {
		JSON_ASSERT(type_ == Type::Array);
		return (*data_.a)[i];
	}

	//从字符串解析值
	bool parse(const string& s, string* err = nullptr) {
		return parse(s.c_str(), err);
	}
	//从字符串解析值
	bool parse(const char* s, string* err = nullptr) {
		clear();
		Parser p(s, err);
		return read_Value(p);
	}
private:
	bool read_Value(Parser& p) {
		JSON_CHECK(skip_blank(p));
		switch (p.read()) {
		case 'n':
			if (p.read_next() == 'u' && p.read_next() == 'l' && p.read_next() == 'l')
				p.next();
			else return p.make_error("Miss 'null'");
			break;
		case 't':
			if (p.read_next() == 'r' && p.read_next() == 'u' && p.read_next() == 'e') {
				p.next();
				*this = true;
			}
			else return p.make_error("Miss 'true'");
			break;
		case 'f':
			if (p.read_next() == 'a' && p.read_next() == 'l' && p.read_next() == 's' && p.read_next() == 'e') {
				p.next();
				*this = false;
			}
			else return p.make_error("Miss 'false'");
			break;
		case '[':return read_Array(p);
		case '{':return read_Object(p);
		case '"':
		{
			string s;
			bool err = read_String(p, s);
			*this = s;
			return err;
		};
		default:
			JSON_CHECK(read_Number(p));
		}
		return true;
	}
	bool read_Number(Parser& p) {
		char* end;
		double num = strtod(p.data(), &end);
		bool is_double = false;
		for (const char* tmp = p.data(); tmp != end; tmp++) {
			switch (*tmp) {
			case'.':
			case'e':
			case'E':
				is_double = true;
			}
		}
		if (is_double)
			*this = num;
		else
			*this = (long long)num;
		p.data() = end;
		return true;
	}
	bool read_Object(Parser& p) {
		p.next();
		JSON_CHECK(skip_blank(p));
		setType(Type::Object);
		if (p.read() == '}') {
			p.next();
			return true;
		}
		while (true) {
			JSON_CHECK(skip_blank(p));
			if (p.read() != '"')
				return p.make_error("Miss '\"'");
			string key;
			JSON_CHECK(read_String(p, key));
			JSON_CHECK(skip_blank(p));
			if (p.read() != ':')
				return p.make_error("Miss ':'");
			p.next();
			JSON_CHECK(skip_blank(p));
			JSON_CHECK((*data_.o)[key].read_Value(p));
			JSON_CHECK(skip_blank(p));
			if (p.read() == ',')
				p.next();
			else if (p.read() == '}') {
				p.next();
				return true;
			}
			else return p.make_error("Miss ',' or '}'");
		}
	}
	bool read_Array(Parser& p) {
		p.next();
		setType(Type::Array);
		JSON_CHECK(skip_blank(p));
		if (p.read() == ']') {
			p.next();
			return true;
		}
		while (true) {
			JSON_CHECK(skip_blank(p));
			JSON_CHECK((*data_.a)[data_.a->size()].read_Value(p));
			JSON_CHECK(skip_blank(p));
			if (p.read() == ',') {
				p.next();
			}
			else if (p.read() == ']') {
				p.next();
				break;
			}
			else return p.make_error("Miss ',' or ']'");
		}
		return true;
	}
	static bool skip_blank(Parser& p) {
		while (true) {
			switch (p.read()) {
			case'\t':break;
			case'\n':p.add_line(); break;
			case'\r':break;
			case' ':break;
			case'/':
				p.next();
				//单行注释
				if (p.read() == '/') {
					p.next();
					while (p.read() != '\n') {
						p.next();
					}
					p.add_line();
				}
				//多行注释
				else if (p.read() == '*') {
					p.next();
					while (p.read() != '*' or *(p.data() + 1) != '/') {
						if (p.read() == '\n')
							p.add_line();
						p.next();

					}
					p.next();
				}
				else {
					return p.make_error("Invalid comment");
				}
				break;
			default:
				return true;
			}
			p.next();
		}
		//return true;
	}
	static inline string double_to_string(const double d) {
		char buf[32];
		snprintf(buf, 31, "%15.15lf", d);
		string s = buf;
		while (s.back() == '0') {
			if (*(&s.back() - 1) != '.')
				s.pop_back();
			else break;
		}
		return s;
	}
	static bool read_Hex4(Parser& p, unsigned& u) {
		u = 0;
		char ch = 0;
		for (int i = 0; i < 4; i++) {
			u <<= 4;
			ch = p.read_next_back();
			if (ch >= '0' && ch <= '9')
				u |= ch - '0';
			else if (ch >= 'a' && ch <= 'f')
				u |= ch - 'a' + 10;
			else if (ch >= 'A' && ch <= 'F')
				u |= ch - 'A' + 10;
			else return p.make_error("Invalid charactor");
		}
		return true;
	}
	static inline void encode_utf8(unsigned& u, string& s) {
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
	static bool read_String(Parser& p, string& s) {
		char ch = 0;
		p.next();
		while (1) {
			ch = p.read_next_back();
			switch (ch) {
			case '\"':return true;
			case '\\':
				switch (p.read_next_back()) {
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
					read_Hex4(p, u);
					if (u >= 0xD800 && u <= 0xDBFF) {
						if (p.read_next_back() != '\\')
							return p.make_error("Invalid character");
						if (p.read_next_back() != 'u')
							return p.make_error("Invalid character");
						unsigned tmp_u;
						read_Hex4(p, tmp_u);
						if (tmp_u < 0xDC00 || tmp_u > 0xDFFF)
							return p.make_error("Invalid character");
						u = 0x10000 + (u - 0xD800) * 0x400 + (tmp_u - 0xDC00);
					}
					if (u > 0x10FFFF)
						return p.make_error("Invalid character");
					encode_utf8(u, s);
					break;
				}
				default:
					return p.make_error("Invalid ESC");
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

	Value data_;
	Type type_;
};

#undef JSON_ADD_TAB
#undef JSON_ASSERT
#undef JSON_CHECK
