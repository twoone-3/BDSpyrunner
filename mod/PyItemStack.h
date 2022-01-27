#pragma once
#include "CPython.h"

extern PyTypeObject PyItemStack_Type;
PyObject* ToPyItemStack(class ItemStack* ptr);
