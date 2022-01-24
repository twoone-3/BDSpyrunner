#pragma once
#include "CPython.h"

extern PyTypeObject PyEntity_Type;
PyObject* ToEntity(class Actor* ptr);
