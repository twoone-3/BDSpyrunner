#include "CPython.h"
#include "Tool.h"

std::string PyObjectToStr(PyObject* obj) {
	return PyUnicode_AsUTF8(PyObject_Repr(obj));
}

std::string PyUnicodeToStr(PyObject* obj) {
	return PyUnicode_AsUTF8(obj);
}

PyObject* StrToPyUnicode(std::string_view str) {
	return PyUnicode_InternFromString(str.data());
}

//list转vector
std::vector<std::string> ToStrArray(PyObject* list) {
	std::vector<std::string> arr;
	if (PyList_Check(list)) {
		for (Py_ssize_t i = 0; i < PyList_Size(list); i++) {
			arr.push_back(PyUnicode_AsUTF8(PyList_GetItem(list, i)));
		}
	}
	return arr;
}

//Vec3转list
PyObject* ToList(const Vec3& vec) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(vec.x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(vec.y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(vec.z));
	return list;
}

//方块坐标转list
PyObject* ToList(BlockPos* bp) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyLong_FromLong(bp->x));
	PyList_SetItem(list, 1, PyLong_FromLong(bp->y));
	PyList_SetItem(list, 2, PyLong_FromLong(bp->z));
	return list;
}

//打印错误信息
void Py_PrintErrors() {
	if (PyErr_Occurred()) {
		//PyObject* type;
		//PyObject* value;
		//PyObject* traceback;
		//PyErr_Fetch(&type, &value, &traceback);
		//if (PyUnicode_Check(value)) {
		//	logger.error("{}", PyUnicodeToStr(value));
		//}
		//else if (PyTuple_Check(value)) {
		//	PyObject* info = reinterpret_cast<PyBaseExceptionObject*>(value)->args;
		//	auto size = PyTuple_Size(info);
		//	if (size == 1) {
		//		logger.error("{}: {}", Py_TYPE(value)->tp_name, PyUnicodeToStr(PyTuple_GetItem(info, 0)));
		//	}
		//	else if (size == 2) {
		//		logger.error("{}: {}", Py_TYPE(value)->tp_name, PyUnicodeToStr(PyTuple_GetItem(info, 0)));
		//		PyObject* location = PyTuple_GetItem(info, 1);
		//		logger.error("File: {} Line: {} Column: {}",
		//			PyUnicodeToStr(PyTuple_GetItem(location, 0)),
		//			PyObjectToStr(PyTuple_GetItem(location, 1)),
		//			PyObjectToStr(PyTuple_GetItem(location, 2))
		//		);
		//		logger.error("    {}", PyObjectToStr(PyTuple_GetItem(location, 3)));
		//	}
		//}
		//logger.info("{}", PyObjectToStr(traceback));
		//PyObject_Print(type, stdout, 0);
		//PyObject_Print(value, stdout, 0);
		//PyErr_Restore(type, value, traceback);
		PyErr_Print();
	}
}

PyGILGuard::PyGILGuard() {
	gil_ = PyGILState_Ensure();
}

PyGILGuard::~PyGILGuard() {
	PyGILState_Release(gil_);
}
