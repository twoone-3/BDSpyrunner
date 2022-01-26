#pragma once
#pragma comment(lib,"lib/python37.lib")
#define PY_SSIZE_T_CLEAN
#include "../include/Python.h"

#define Py_PARSE(format,...) if (!PyArg_ParseTuple(args, format ":" __FUNCTION__, __VA_ARGS__))return nullptr
#define Py_KERWORDS_LIST(...) static const char* kwlist[]{ __VA_ARGS__,nullptr }
#define Py_PARSE_WITH_KERWORDS(format,...) if (!PyArg_ParseTupleAndKeywords(args, kwds, format ":" __FUNCTION__, const_cast<char**>(kwlist), __VA_ARGS__))return nullptr

#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
#define Py_PRINT_REFCOUNT(obj) cout << "引用计数：" << obj->ob_refcnt << endl
//#define Py_BEGIN_CALL
//	int _has_gil = PyGILState_Check();
//	PyGILState_STATE _gil_state = PyGILState_LOCKED;
//	if (!_has_gil)_gil_state = PyGILState_Ensure();
//	Py_BEGIN_ALLOW_THREADS;
//	Py_BLOCK_THREADS
//#define Py_END_CALL
//	Py_UNBLOCK_THREADS
//	Py_END_ALLOW_THREADS;
//	if (!_has_gil)PyGILState_Release(_gil_state)

//字符串转Unicode
inline PyObject* ToPyStr(std::string_view str) {
	return PyUnicode_InternFromString(str.data());
	//return PyUnicode_FromStringAndSize(str.data(), str.length());
}
//打印错误信息
inline void PrintPythonError() {
	if (PyErr_Occurred()) {
		PyErr_Print();
	}
}
