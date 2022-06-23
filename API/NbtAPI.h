#pragma once
#include <Global.hpp>

class Tag;
struct NbtClass {
	unique_ptr<Tag> thiz;
	bool is_reference;

	NbtClass();
	NbtClass(Tag* other);
	template<typename T>
	NbtClass(unique_ptr<T>&& other) {
		thiz = std::move(other);
		is_reference = false;
	}
	NbtClass(const NbtClass& other);
	NbtClass(NbtClass&& other) noexcept;
	~NbtClass();
	
	static NbtClass newEnd();
	static NbtClass newByte(unsigned char value);
	static NbtClass newShort(short value);
	static NbtClass newInt(int value);
	static NbtClass newInt64(int64_t value);
	static NbtClass newFloat(float value);
	static NbtClass newDouble(double value);
	static NbtClass newString(const string& value);
	static NbtClass newByteArray(const py::bytearray& value);
	static NbtClass newList();
	static NbtClass newCompound();
	static NbtClass newIntArray();
	static NbtClass fromSNBT(const string& snbt);
	static NbtClass fromBinary(const py::bytes& bytes);

	NbtClass __getitem__(int key);
	NbtClass __getitem__(const string& key);
	void __setitem__(const string& key, const NbtClass& val);
	unsigned char asByte();
	short asShort();
	int asInt();
	int64_t asInt64();
	float asFloat();
	double asDouble();
	string_view getType();
	py::bytes toBinary();
	string toJson(int indentatiton = 4);
	py::object toObject();
	string toSNBT(int indent, SnbtFormat snbtFormat = SnbtFormat::PartialNewLine);
	void append(const NbtClass& value);
};
