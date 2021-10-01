#pragma once
#include <string>
#include "CPython.h"
#include "../mc/Position.h"

#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
#define Py_BEGIN_CALL\
	int _has_gil = PyGILState_Check();\
	PyGILState_STATE _gil_state = PyGILState_LOCKED;\
	if (!_has_gil)_gil_state = PyGILState_Ensure();\
	Py_BEGIN_ALLOW_THREADS;\
	Py_BLOCK_THREADS
#define Py_END_CALL\
	Py_UNBLOCK_THREADS;\
	Py_END_ALLOW_THREADS;\
	if (!_has_gil)PyGILState_Release(_gil_state)

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
