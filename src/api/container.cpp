#include "container.h"

#include <llapi/mc/Container.hpp>
#include <llapi/mc/ItemStack.hpp>

#include "item.h"

ContainerClass::ContainerClass(Container* p) : thiz(p) {}

//成员函数
int ContainerClass::getSize() {
  if (thiz == nullptr) return 0;
  return thiz->getSize();
}

string ContainerClass::getType() {
  if (thiz == nullptr) return "";
  return thiz->getTypeName();
}

bool ContainerClass::addItem(const ItemClass& i) {
  if (thiz == nullptr) return false;
  ItemStack* item = i.thiz;
  if (!item) throw std::invalid_argument("invalid item");
  return thiz->addItem_s(item);
}

bool ContainerClass::addItemToFirstEmptySlot(const ItemClass& i) {
  if (thiz == nullptr) return false;
  ItemStack* item = i.thiz;
  if (!item) throw std::invalid_argument("invalid item");
  return thiz->addItemToFirstEmptySlot_s(item);
}

bool ContainerClass::hasRoomFor(const ItemClass& i) {
  if (thiz == nullptr) return false;
  ItemStack* item = i.thiz;
  if (!item) throw std::invalid_argument("invalid item");
  return thiz->hasRoomForItem(*item);
}

bool ContainerClass::removeItem(int slot, int number) {
  if (thiz == nullptr) return false;
  return thiz->removeItem_s(slot, number);
}

ItemClass ContainerClass::getItem(int slot) {
  if (thiz == nullptr) return nullptr;
  ItemStack* item = (ItemStack*)&thiz->getItem(slot);
  if (!item) throw std::domain_error("Fail to get slot from container!");
  return item;
}

bool ContainerClass::setItem(int slot, const ItemClass& i) {
  if (thiz == nullptr) return false;
  ItemStack* item = i.thiz;
  if (!item) throw std::invalid_argument("invalid item");
  ItemStack* itemOld = const_cast<ItemStack*>(&thiz->getItem(slot));
  if (!itemOld) return false;
  return itemOld->setItem(item);
}

vector<ItemClass> ContainerClass::getAllItems() {
  if (thiz == nullptr) return {};
  vector<ItemClass> res;
  for (auto i : thiz->getAllSlots()) {
    res.push_back(ItemClass(const_cast<ItemStack*>(i)));
  }
  return res;
}

bool ContainerClass::removeAllItems() {
  if (thiz == nullptr) return false;
  thiz->removeAllItems();
  return true;
}

bool ContainerClass::isEmpty() {
  if (thiz == nullptr) return true;
  return thiz->isEmpty();
}