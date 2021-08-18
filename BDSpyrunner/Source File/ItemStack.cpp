#include <ItemStack.h>
#include <tool.h>
#include <Tag.h>

using namespace std;
ItemStack::ItemStack() {
	SymCall("??0ItemStack@@QEAA@XZ", this);
}

ItemStack::ItemStack(const Json& value) {
	SymCall("??0ItemStack@@QEAA@XZ", this);
	fromJson(value);
}

ItemStack::~ItemStack() {
	SymCall("??1ItemStack@@UEAA@XZ", this);
}

short ItemStack::getAuxValue() {
	return FETCH(short, _this + 32);
}

unsigned char ItemStack::getCount() {
	return FETCH(unsigned char, _this + 34);
}

short ItemStack::getId() {
	return SymCall<short>("?getId@ItemStackBase@@QEBAFXZ", this);
}

short ItemStack::getDamageValue() {
	return SymCall<short>("?getDamageValue@ItemStackBase@@QEBAFXZ", this);
}

//取物品名称

string ItemStack::getName() {
	string str;
	SymCall<string&>("?getRawNameId@ItemStackBase@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &str);
	return str;
}

//取容器内数量

int ItemStack::getStackCount() {//IDA ContainerModel::networkUpdateItem
	return FETCH(int, this + 34);
}

//判断是否空容器

bool ItemStack::isNull() {
	return SymCall<bool>("?isNull@ItemStackBase@@QEBA_NXZ", this);
}

bool ItemStack::isEmptyStack() {
	return FETCH(char, this + 34) == 0;
}

Tag* ItemStack::getNetworkUserData() {
	Tag* t = nullptr;
	SymCall("?getNetworkUserData@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
		this, &t);
	return t;
}

Tag* ItemStack::save() {
	Tag* t = nullptr;
	SymCall("?save@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
		this, &t);
	return t;
}

ItemStack* ItemStack::fromTag(Tag* t) {
	return SymCall<ItemStack*>("?fromTag@ItemStack@@SA?AV1@AEBVCompoundTag@@@Z",
		this, t);
}

//bool ItemStack::getFromId(short id, short aux, char count) {
//	memcpy(this, SYM("?EMPTY_ITEM@ItemStack@@2V1@B"), sizeof(ItemStack));
//	bool ret = SymCall<bool>("?_setItem@ItemStackBase@@IEAA_NH@Z", this, id);
//	mCount = count;
//	mAuxValue = aux;
//	mValid = true;
//	return ret;
//}

Item* ItemStack::getItem() {
	return SymCall<Item*>("?getItem@ItemStackBase@@QEBAPEBVItem@@XZ", this);
}

void ItemStack::fromJson(const Json& value) {
	Tag* t = ObjecttoTag(value);
	fromTag(t);
	t->deleteCompound();
	delete t;
}

//获取容器内所有物品

vector<ItemStack*> Container::getSlots() {
	vector<ItemStack*> s;
	SymCall<uintptr_t>("?getSlots@Container@@UEBA?BV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ",
		this, &s);
	return s;
}

void Container::clearItem(int slot, int num) {
	SymCall("?removeItem@Container@@UEAAXHH@Z", this, slot, num);
}
