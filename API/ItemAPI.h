#pragma once
#include <Global.hpp>

class ItemStack;
struct NBTClass;
struct ItemClass {
  ItemStack* thiz;

  ItemClass();
  ItemClass(ItemStack* p);

  string getName();
  string getType();
  short getId();
  int getCount();
  int getAux();

  bool set(const ItemClass&);
  ItemClass clone();
  bool isNull();
  bool setNull();
  bool setAux(short);
  bool setLore(const vector<string>&);
  NBTClass getNbt();
  bool setNbt(const NBTClass&);
};