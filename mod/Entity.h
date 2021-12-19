#pragma once
#include <string>
#include "CPython.h"
//Entity¿‡–Õ
extern PyTypeObject PyEntity_Type;
PyObject* ToEntity(struct Actor* ptr);
