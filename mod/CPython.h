#pragma once
#pragma comment(lib,"lib/python37.lib")
#define PY_SSIZE_T_CLEAN
#include "../include/Python.h"

#define Py_PARSE(format,...) if (!PyArg_ParseTuple(args, format ":" __FUNCTION__, __VA_ARGS__))return nullptr
#define Py_KERWORDS_LIST(...) static const char* kwlist[]{ __VA_ARGS__,nullptr }
#define Py_PARSE_WITH_KERWORDS(format,...) if (!PyArg_ParseTupleAndKeywords(args, kwds, format ":" __FUNCTION__, const_cast<char**>(kwlist), __VA_ARGS__))return nullptr

//字符串转Unicode
inline PyObject* ToPyStr(std::string_view str) {
	return PyUnicode_FromStringAndSize(str.data(), str.length());
}
//打印错误信息
inline void PrintPythonError() {
	if (PyErr_Occurred()) {
		PyErr_Print();
	}
}