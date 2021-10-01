#pragma once
#pragma comment(lib,"lib/python37.lib")
#define PY_SSIZE_T_CLEAN
#include "../include/Python.h"

//字符串转Unicode
inline PyObject* StringToPyUnicode(std::string_view str) {
	return PyUnicode_FromStringAndSize(str.data(), str.length());
}
//打印错误信息
inline void PrintPythonError() {
	if (PyErr_Occurred()) {
		PyErr_Print();
	}
}