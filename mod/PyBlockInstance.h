﻿#pragma once
#include "CPython.h"

extern PyTypeObject PyBlockInstance_Type;
PyObject* ToBlockInstance(class BlockInstance* ptr);