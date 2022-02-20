#pragma once
#include <pybind11/embed.h>

//#define Py_PARSE(format,...) if (!PyArg_ParseTuple(args, format ":" __FUNCTION__, __VA_ARGS__))return nullptr
//#define Py_KEYWORDS_LIST(...) static const char* kwlist[] {__VA_ARGS__,nullptr}
//#define Py_PARSE_WITH_KEYWORDS(format,...) if (!PyArg_ParseTupleAndKeywords(args, kwds, format ":" __FUNCTION__, const_cast<char**>(kwlist), __VA_ARGS__))return nullptr
//
//#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
//#define Py_RETURN_ERROR_FORMAT(str,...) return PyErr_Format(PyExc_Exception, str, __VA_ARGS__), nullptr
//#define Py_PRINT_REFCOUNT(obj) logger.info(#obj "->ref_count = {}",  obj->ob_refcnt)
//
//#define Py_METHOD_VARARGS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_VARARGS, nullptr}
//#define Py_METHOD_NOARGS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_NOARGS, nullptr}
//#define Py_METHOD_ONEARG(name) {#name, reinterpret_cast<PyCFunction>(name), METH_O, nullptr}
//#define Py_METHOD_KEYWORDS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_VARARGS | METH_KEYWORDS, nullptr}
//#define Py_METHOD_CUSTOM(name, flags) {#name, reinterpret_cast<PyCFunction>(name), flags, nullptr}
//#define Py_METHOD_END {nullptr, nullptr, 0, nullptr}
//#define Py_METHOD_DEFINE(name) static PyObject* name(PyObject * self, PyObject* args, PyObject* kwds)

namespace py = pybind11;

#if 1
class PyGILGuard {
public:
	PyGILGuard() { gil_ = PyGILState_Ensure(); }
	~PyGILGuard() { PyGILState_Release(gil_); }

private:
	PyGILState_STATE gil_;
};
#endif
