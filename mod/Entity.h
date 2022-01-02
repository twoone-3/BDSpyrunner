#pragma once
#include "CPython.h"
//Entity类型
extern PyTypeObject PyEntity_Type;
PyObject* ToEntity(class Actor* ptr);
