#include "ContainerAPI.h"
#include <Global.hpp>
#include "ItemAPI.h"
#include <MC/ItemStack.hpp>
#include <MC/Container.hpp>
using namespace std;

ContainerClass::ContainerClass(Container* p) : thiz(p) {}

//成员函数
int ContainerClass::getSize() {
	return thiz->getSize();
}

string ContainerClass::getType() {
	return thiz->getTypeName();
}

bool ContainerClass::addItem(const ItemClass& i) {
	ItemStack* item = i.thiz;
	if (!item)
		throw std::invalid_argument("invalid item");
	return thiz->addItem_s(item);
}

bool ContainerClass::addItemToFirstEmptySlot(const ItemClass& i) {
	ItemStack* item = i.thiz;
	if (!item)
		throw std::invalid_argument("invalid item");
	return thiz->addItemToFirstEmptySlot_s(item);
}

bool ContainerClass::hasRoomFor(const ItemClass& i) {
	ItemStack* item = i.thiz;
	if (!item)
		throw std::invalid_argument("invalid item");
	return thiz->hasRoomForItem(*item);
}

bool ContainerClass::removeItem(int slot, int number) {
	return thiz->removeItem_s(slot, number);
}

ItemClass ContainerClass::getItem(int slot) {
	ItemStack* item = (ItemStack*)&thiz->getItem(slot);
	if (!item)
		throw std::domain_error("Fail to get slot from container!");
	return item;
}

bool ContainerClass::setItem(int slot, const ItemClass& i) {
	ItemStack* item = i.thiz;
	if (!item)
		throw std::invalid_argument("invalid item");
	ItemStack* itemOld = (ItemStack*)&thiz->getItem(slot);
	if (!itemOld)
		return false;
	return itemOld->setItem(item);
}

vector<const ItemStack*> ContainerClass::getAllItems() {
	return thiz->getAllSlots();
}

bool ContainerClass::removeAllItems() {
	thiz->removeAllItems();
	return true;
}

bool ContainerClass::isEmpty() {
	return thiz->isEmpty();
}