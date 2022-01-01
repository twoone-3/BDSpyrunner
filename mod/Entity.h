#pragma once
#include <string>
#include "CPython.h"
//Entity类型
extern PyTypeObject PyEntity_Type;
PyObject* ToEntity(class Actor* ptr);
