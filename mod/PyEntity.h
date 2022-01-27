#pragma once
#include "CPython.h"

extern PyTypeObject PyEntity_Type;
PyObject* ToPyEntity(class Actor* ptr);
