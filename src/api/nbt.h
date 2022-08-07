#pragma once
#include <MC/CompoundTag.hpp>
#include <MC/Tag.hpp>

#include "../py_utils.h"

class Tag;
struct NBTClass {
  unique_ptr<Tag> thiz;
  bool is_reference;

  NBTClass();
  NBTClass(Tag* other);
  template <typename T>
  NBTClass(unique_ptr<T>&& other) {
    thiz = move(other);
    is_reference = false;
  }
  NBTClass(const NBTClass& other);
  NBTClass(NBTClass&& other) noexcept;
  ~NBTClass();

  static NBTClass newEnd();
  static NBTClass newByte(unsigned char value);
  static NBTClass newShort(short value);
  static NBTClass newInt(int value);
  static NBTClass newInt64(int64_t value);
  static NBTClass newFloat(float value);
  static NBTClass newDouble(double value);
  static NBTClass newString(const string& value);
  static NBTClass newByteArray(const py::bytearray& value);
  static NBTClass newList();
  static NBTClass newCompound();
  static NBTClass newIntArray();
  static NBTClass fromSNBT(const string& snbt);
  static NBTClass fromBinary(const py::bytes& bytes);

  NBTClass __getitem__(int key);
  NBTClass __getitem__(const string& key);
  void __setitem__(const string& key, const NBTClass& val);
  unsigned char asByte();
  short asShort();
  int asInt();
  int64_t asInt64();
  float asFloat();
  double asDouble();
  string asString();
  Tag::Type getType();
  py::bytes toBinary();
  string toJson(int indentatiton = 4);
  py::object toObject();
  string toSNBT(int indent, SnbtFormat snbtFormat = SnbtFormat::PartialNewLine);
  void append(const NBTClass& value);
};
