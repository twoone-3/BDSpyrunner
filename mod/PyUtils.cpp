#include "PyUtils.h"
#include "Common.h"


//打印错误信息
void Py_PrintErrors() {
	if (PyErr_Occurred()) {
		PyErr_Print();
	}
}

//PyObject转string
std::string PyObjectToStr(PyObject* obj) {
	return PyUnicode_AsUTF8(PyObject_Repr(obj));
}

//PyObject转string
std::string PyUnicodeToStr(PyObject* obj) {
	return PyUnicode_AsUTF8(obj);
}

//list转vector
std::vector<std::string> PyListToArray(PyObject* list) {
	std::vector<std::string> arr;
	if (PyList_Check(list)) {
		for (Py_ssize_t i = 0; i < PyList_Size(list); i++) {
			arr.push_back(PyUnicodeToStr(PyList_GetItem(list, i)));
		}
	}
	return arr;
}

PyObject* ToPyObject(PyObject* obj) {
	return obj;
}

PyObject* ToPyObject(std::string_view str) {
	return PyUnicode_FromStringAndSize(str.data(), str.size());
}

//string转Unicode
PyObject* ToPyObject(const std::string& str) {
	return PyUnicode_FromStringAndSize(str.data(), str.size());
}

PyObject* ToPyObject(const char* str) {
	return PyUnicode_FromString(str);
}

//Vec3转list
PyObject* ToPyObject(const Vec3& vec) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(vec.x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(vec.y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(vec.z));
	return list;
}

//方块坐标转list
PyObject* ToPyObject(const BlockPos& bp) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyLong_FromLong(bp.x));
	PyList_SetItem(list, 1, PyLong_FromLong(bp.y));
	PyList_SetItem(list, 2, PyLong_FromLong(bp.z));
	return list;
}

PyObject* ToPyObject(bool b) {
	if (b)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

PyObject* ToPyObject(short s) {
	return PyLong_FromLong(s);
}

PyObject* ToPyObject(int i) {
	return PyLong_FromLong(i);
}

PyObject* ToPyObject(long long l) {
	return PyLong_FromLongLong(l);
}

PyObject* ToPyObject(double d) {
	return PyFloat_FromDouble(d);
}

