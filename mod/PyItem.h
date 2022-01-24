#pragma once
#include "CPython.h"

extern PyTypeObject PyItem_Type;
PyObject* ToItem(class ItemStack* ptr);
