#pragma once
#define PY_SSIZE_T_CLEAN
#include "../include/Python.h"
#include "Common.h"

#define Py_PARSE(format,...) if (!PyArg_ParseTuple(args, format ":" __FUNCTION__, __VA_ARGS__))return nullptr
#define Py_KEYWORDS_LIST(...) static const char* kwlist[]{ __VA_ARGS__,nullptr }
#define Py_PARSE_WITH_KEYWORDS(format,...) if (!PyArg_ParseTupleAndKeywords(args, kwds, format ":" __FUNCTION__, const_cast<char**>(kwlist), __VA_ARGS__))return nullptr

#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
#define Py_RETURN_ERROR_FORMAT(str,...) return PyErr_Format(PyExc_Exception, str, __VA_ARGS__), nullptr
#define Py_PRINT_REFCOUNT(obj) logger.info(#obj " 的引用计数 : {}",  obj->ob_refcnt)

#define Py_METHOD_VARARGS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_VARARGS, nullptr}
#define Py_METHOD_NOARGS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_NOARGS, nullptr}
#define Py_METHOD_ONEARG(name) {#name, reinterpret_cast<PyCFunction>(name), METH_O, nullptr}
#define Py_METHOD_KEYWORDS(name) {#name, reinterpret_cast<PyCFunction>(name), METH_VARARGS | METH_KEYWORDS, nullptr}
#define Py_METHOD_CUSTOM(name, flags) {#name, reinterpret_cast<PyCFunction>(name), flags, nullptr}
#define Py_METHOD_END {nullptr, nullptr, 0, nullptr}
#define Py_METHOD_DEFINE(name) static PyObject* name(PyObject * self, PyObject* args, PyObject* kwds)

//打印错误信息
void Py_PrintErrors();
// PyObject转string
std::string PyObjectToStr(PyObject* obj);
// PyUnicode转string
std::string PyUnicodeToStr(PyObject* obj);
// list转vector
std::vector<std::string> PyListToArray(PyObject* list);
// PyObject转PyObject
PyObject* ToPyObject(PyObject* obj);
// string转PyUnicode
PyObject* ToPyObject(std::string_view str);
// string转PyUnicode
PyObject* ToPyObject(const std::string& str);
// string转PyUnicode
PyObject* ToPyObject(const char* str);
// Vec3转list
PyObject* ToPyObject(const Vec3& vec);
// BlockPos转list
PyObject* ToPyObject(const BlockPos& bp);
// bool转PyObject
PyObject* ToPyObject(bool b);
// short转PyObject
PyObject* ToPyObject(short s);
// int转PyObject
PyObject* ToPyObject(int i);
// long long转PyObject
PyObject* ToPyObject(long long l);
// double转PyObject
PyObject* ToPyObject(double d);

extern PyTypeObject PyBlockInstance_Type;
PyObject* ToPyObject(BlockInstance* ptr);
PyObject* ToPyObject(BlockInstance& bi);

extern PyTypeObject PyEntity_Type;
PyObject* ToPyObject(Actor* ptr);

extern PyTypeObject PyItemStack_Type;
PyObject* ToPyObject(ItemStack* ptr);

extern PyTypeObject PyNBT_Type;
PyObject* ToPyObject(std::unique_ptr<Tag>&& ptr);

class PyGILGuard {
public:
	PyGILGuard() {
		gil_ = PyGILState_Ensure();
	}
	~PyGILGuard() {
		PyGILState_Release(gil_);
	}

private:
	PyGILState_STATE gil_;
};

class PyCaller {
public:
	PyCaller() {
	}
	~PyCaller() {
		Py_DECREF(result_);
	}
	PyObject* callVector(PyObject* func, const vector<PyObject*>& args) {
		return result_ = _PyObject_FastCall(func, args.data(), args.size());
	}
	template <typename... Args>
	PyObject* call(PyObject* func, Args... args) {
		vector<PyObject*> vargs;
		(vargs.push_back(ToPyObject(args)), ...);
		callVector(func, vargs);
		Py_PrintErrors();
		for (auto arg : vargs) {
			Py_DECREF(arg);
		}
		return result_;
	}

private:
	PyGILGuard gil_;
	vector<PyObject*> args_;
	PyObject* result_;
};
