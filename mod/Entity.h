#pragma once
#include <string>
#define PY_SSIZE_T_CLEAN
#include "../include/Python.h"

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

// 实体类型
//转换std::string为PyUnicode
PyObject* ToPyUnicode(const std::string&);

PyObject* PyEntity_FromEntity(struct Actor* ptr);

//Entity类型
extern PyTypeObject PyEntity_Type;
