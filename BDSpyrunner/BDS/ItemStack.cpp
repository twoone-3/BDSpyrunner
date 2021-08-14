#include "ItemStack.h"

//取物品ID,特殊值,损耗

inline short ItemStack::getId() {
	return SymCall<short>("?getId@ItemStackBase@@QEBAFXZ", this);
}

inline short ItemStack::getDamageValue() {
	return SymCall<short>("?getDamageValue@ItemStackBase@@QEBAFXZ", this);
}

//取物品名称

inline string ItemStack::getName() {
	string str;
	SymCall<string*>("?getRawNameId@ItemStackBase@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &str);
	return str;
}

//取容器内数量

inline int ItemStack::getStackCount() {//IDA ContainerModel::networkUpdateItem
	return FETCH(int, this + 34);
}

//判断是否空容器

inline bool ItemStack::isNull() {
	return SymCall<bool>("?isNull@ItemStackBase@@QEBA_NXZ", this);
}

inline bool ItemStack::isEmptyStack() {
	return FETCH(char, this + 34) == 0;
}

inline Tag* ItemStack::getNetworkUserData() {
	Tag* t = nullptr;
	SymCall("?getNetworkUserData@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
		this, &t);
	return t;
}

inline Tag* ItemStack::save() {
	Tag* t = nullptr;
	SymCall("?save@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
		this, &t);
	return t;
}

inline ItemStack* ItemStack::fromTag(Tag* t) {
	return SymCall<ItemStack*>("?fromTag@ItemStack@@SA?AV1@AEBVCompoundTag@@@Z",
		this, t);
}

inline bool ItemStack::getFromId(short id, short aux, char count) {
	memcpy(this, SYM("?EMPTY_ITEM@ItemStack@@2V1@B"), sizeof(ItemStack));
	bool ret = SymCall<bool>("?_setItem@ItemStackBase@@IEAA_NH@Z", this, id);
	mCount = count;
	mAuxjson = aux;
	mValid = true;
	return ret;
}

inline Item* ItemStack::getItem() {
	return SymCall<Item*>("?getItem@ItemStackBase@@QEBAPEBVItem@@XZ", this);
}

inline void ItemStack::fromJson(const json& value) {
	Tag* t = ObjecttoTag(value);
	fromTag(t);
	t->deleteCompound();
	delete t;
}

//获取容器内所有物品

inline vector<ItemStack*> Container::getSlots() {
	vector<ItemStack*> s;
	SymCall<VA>("?getSlots@Container@@UEBA?BV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ",
		this, &s);
	return s;
}

inline void Container::clearItem(int slot, int num) {
	SymCall("?removeItem@Container@@UEAAXHH@Z", this, slot, num);
}
