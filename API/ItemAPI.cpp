#include "ItemAPI.h"

#include <Global.hpp>
#include <MC/ItemStack.hpp>

#include "EntityAPI.h"
#include "McAPI.h"
#include "NBTAPI.h"

ItemClass::ItemClass() : thiz(ItemStack::create()) {}

ItemClass::ItemClass(ItemStack* p) : thiz(p) {}

string ItemClass::getName() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->getName();
}

string ItemClass::getType() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->getTypeName();
}

short ItemClass::getId() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->getId();
}

int ItemClass::getCount() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->getCount();
}

int ItemClass::getAux() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->getAux();
}

bool ItemClass::set(const ItemClass& i) {
  ItemStack* item = i.thiz;
  if (!item) throw std::invalid_argument("invalid item");
  return thiz->setItem(item);
}

ItemClass ItemClass::clone() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return ItemClass(thiz->clone_s());
}

bool ItemClass::isNull() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->isNull();
}

bool ItemClass::setNull() {
  if (!thiz) throw std::invalid_argument("invalid item");
  thiz->setNull();
  return true;
}

bool ItemClass::setAux(short aux) {
  if (!thiz) throw std::invalid_argument("invalid item");
  thiz->setAuxValue(aux);
  return true;
}

bool ItemClass::setLore(const vector<string>& lores) {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->setLore(lores);
}

NBTClass ItemClass::getNbt() {
  if (!thiz) throw std::invalid_argument("invalid item");
  return thiz->getNbt();
}

bool ItemClass::setNbt(const NBTClass& nbt) {
  return thiz->setNbt(nbt.thiz->asCompoundTag());
}
