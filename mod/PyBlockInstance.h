#pragma once
#include "CPython.h"

extern PyTypeObject PyBlockInstance_Type;
PyObject* ToPyBlockInstance(class BlockInstance* ptr);
