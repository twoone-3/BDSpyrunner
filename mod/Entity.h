#pragma once
#include <string>
#include "CPython.h"
#include "../mc/Position.h"
//Entity类型
extern PyTypeObject PyEntity_Type;
//Vec3转list
inline PyObject* ToList(Vec3 * vec) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(vec->x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(vec->y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(vec->z));
	return list;
}
//方块坐标转list
inline PyObject* ToList(BlockPos * bp) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyLong_FromLong(bp->x));
	PyList_SetItem(list, 1, PyLong_FromLong(bp->y));
	PyList_SetItem(list, 2, PyLong_FromLong(bp->z));
	return list;
}
PyObject* ToEntity(struct Actor* ptr);
