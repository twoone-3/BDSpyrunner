#pragma once
#define PY_SSIZE_T_CLEAN
#include "../include/Python.h"
#include <Global.h>

#define Py_PARSE(format,...) if (!PyArg_ParseTuple(args, format ":" __FUNCTION__, __VA_ARGS__))return nullptr
#define Py_KERWORDS_LIST(...) static const char* kwlist[]{ __VA_ARGS__,nullptr }
#define Py_PARSE_WITH_KERWORDS(format,...) if (!PyArg_ParseTupleAndKeywords(args, kwds, format ":" __FUNCTION__, const_cast<char**>(kwlist), __VA_ARGS__))return nullptr

#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
#define Py_RETURN_ERROR_FORMAT(str,...) return PyErr_Format(PyExc_Exception, str, __VA_ARGS__), nullptr
#define Py_PRINT_REFCOUNT(obj) logger.info(#obj " 的引用计数 : {}",  obj->ob_refcnt)

#define Py_METHOD_VARARGS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_VARARGS, nullptr}
#define Py_METHOD_NOARGS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_NOARGS, nullptr}
#define Py_METHOD_KEYWORDS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_VARARGS | METH_KEYWORDS, nullptr}
#define Py_METHOD_END {nullptr, nullptr, 0, nullptr}
#define Py_METHOD_DEFINE(name) static PyObject* name(PyObject * self, PyObject* args)

//#define Py_BEGIN_CALL\
//	int _has_gil = PyGILState_Check();\
//	PyGILState_STATE _gil_state = PyGILState_LOCKED;\
//	if (!_has_gil)_gil_state = PyGILState_Ensure();\
//	Py_BEGIN_ALLOW_THREADS;\
//	Py_BLOCK_THREADS
//#define Py_END_CALL\
//	Py_UNBLOCK_THREADS\
//	Py_END_ALLOW_THREADS;\
//	if (!_has_gil)PyGILState_Release(_gil_state)

class PyGILGuard {
public:
	PyGILGuard();
	~PyGILGuard();
private:
	PyGILState_STATE gil_;
};

std::string PyObjectToStr(PyObject* obj);
std::string PyUnicodeToStr(PyObject* obj);
//字符串转Unicode
PyObject* StrToPyUnicode(std::string_view str);
//list转vector
std::vector<std::string> ToStrArray(PyObject* list);
//Vec3转list
PyObject* ToList(const Vec3& vec);
//方块坐标转list
PyObject* ToList(BlockPos* bp);
//打印错误信息
void PrintPythonError();