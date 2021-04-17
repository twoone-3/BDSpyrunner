#pragma once
#include "../pch.h"
#include "NBT.h"
#include "Block.h"
struct Item {};
struct ItemStack {
	VA vtable;
	Item* mItem;
	Tag* mUserData;
	Block* mBlock;
	short mAuxValue;
	char mCount;
	bool mValid;
	VA mPickupTime;
	bool mShowPickUp;
	vector<BlockLegacy*> mCanPlaceOn;
	VA mCanPlaceOnHash;
	vector<BlockLegacy*> mCanDestroy;
	VA mCanDestroyHash;
	VA mBlockingTick;
	ItemStack* mChargedItem;
	VA unk;

	//取物品ID,特殊值,损耗
	short getId() {
		return SYMCALL<short>("?getId@ItemStackBase@@QEBAFXZ", this);
	}
	short getDamageValue() {
		return SYMCALL<short>("?getDamageValue@ItemStackBase@@QEBAFXZ", this);
	}
	//取物品名称
	string getName() {
		string str;
		SYMCALL<string*>("?getRawNameId@ItemStackBase@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this, &str);
		return str;
	}
	//取容器内数量
	int getStackCount() {//IDA ContainerModel::networkUpdateItem
		return FETCH(int, this + 34);
	}
	//判断是否空容器
	bool isNull() {
		return SYMCALL<bool>("?isNull@ItemStackBase@@QEBA_NXZ", this);
	}
	bool isEmptyStack() {
		return FETCH(char, this + 34) == 0;
	}
	Tag* getNetworkUserData() {
		Tag* ct;
		SYMCALL("?getNetworkUserData@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			this, &ct);
		return ct;
	}
	Tag* save() {
		Tag* t = 0;
		SYMCALL("?save@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			this, &t);
		return t;
	}
	ItemStack* fromTag(Tag* t) {
		return SYMCALL<ItemStack*>("?fromTag@ItemStack@@SA?AV1@AEBVCompoundTag@@@Z",
			this, t);
	}
	bool getFromId(short id, short aux, char count) {
		memcpy(this, SYM("?EMPTY_ITEM@ItemStack@@2V1@B"), 0x90);
		bool ret = SYMCALL<bool>("?_setItem@ItemStackBase@@IEAA_NH@Z", this, id);
		mCount = count;
		mAuxValue = aux;
		mValid = true;
		return ret;
	}
	Item* getItem() {
		return SYMCALL<Item*>("?getItem@ItemStackBase@@QEBAPEBVItem@@XZ", this);
	}
	void fromJson(Json::Value j) {
		Tag* t = toTag(j);
		fromTag(t);
		t->deCompound();
		delete t;
	}
};
static_assert(sizeof(ItemStack) == 0x90);
struct Container {
	//获取容器内所有物品
	vector<ItemStack*> getSlots() {
		vector<ItemStack*> s;
		SYMCALL<VA>("?getSlots@Container@@UEBA?BV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ",
			this, &s);
		return move(s);
	}
	void clearItem(int slot, int num) {
		SYMCALL("?removeItem@Container@@UEAAXHH@Z", this, slot, num);
	}
};
#pragma endregion
