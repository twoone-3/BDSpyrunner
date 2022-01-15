#pragma once
#include "CPython.h"
//Entity类型
extern PyTypeObject PyItem_Type;
PyObject* ToItem(class ItemStack* ptr);
