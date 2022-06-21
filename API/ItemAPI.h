#pragma once
#include <Global.hpp>

class ItemStack;
struct NbtClass;
struct ItemClass {
	ItemStack* thiz;

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
	NbtClass getNbt();
	bool setNbt(const NbtClass&);
};