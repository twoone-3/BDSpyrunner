﻿#pragma once
#include "../global.h"

class Container;
struct ItemClass;
struct ContainerClass {
  Container* thiz;

  ContainerClass(Container* p);

  int getSize();
  string getType();

  bool addItem(const ItemClass& i);
  bool addItemToFirstEmptySlot(const ItemClass& i);
  bool hasRoomFor(const ItemClass& i);
  bool removeItem(int slot, int number);
  ItemClass getItem(int slot);
  bool setItem(int slot, const ItemClass& i);
  vector<ItemClass> getAllItems();
  bool removeAllItems();
  bool isEmpty();
};