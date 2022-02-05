#include "PyUtils.h"

#define PyItemStack_Check(self) (Py_TYPE(self) == &PyItemStack_Type)
#define PyItemStack_RAW(self) (reinterpret_cast<PyItemStack*>(self)->value)
#define PyItemStack_VALUE auto thiz = PyItemStack_RAW(self); if (thiz == nullptr)return nullptr

using namespace std;

struct PyItemStack {
	PyObject_HEAD;
	ItemStack* value;

	static int print(PyObject* self, FILE* file, int) {
		PyItemStack_VALUE, -1;
		fputs(thiz->getName().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		PyItemStack_VALUE, ToPyObject("");
		return ToPyObject(thiz->getName());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		PyItemStack_VALUE, ToPyObject("");
		return ToPyObject(thiz->getName());
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
		case Py_LT:// <
			break;
		case Py_LE:// <=
			break;
		case Py_EQ:// ==
			return ToPyObject(PyItemStack_RAW(self) == PyItemStack_RAW(other));
		case Py_NE:// !=
			return ToPyObject(PyItemStack_RAW(self) != PyItemStack_RAW(other));
		case Py_GT:// >
			break;
		case Py_GE:// >=
			break;
		}
		Py_RETURN_NOTIMPLEMENTED;
	}

	Py_METHOD_DEFINE(getName) {
		PyItemStack_VALUE;
		return ToPyObject(thiz->getName());
	}
	Py_METHOD_DEFINE(getNBT) {
		PyItemStack_VALUE;
		return ToPyObject(thiz->getNbt());
	}

	inline static PyMethodDef methods[] {
		Py_METHOD_NOARGS(getName),
		Py_METHOD_NOARGS(getNBT),
		Py_METHOD_END
	};
};

PyTypeObject PyItemStack_Type {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Item",					/* tp_name */
	sizeof(PyItemStack),	/* tp_basicsize */
	0,						/* tp_itemsize */
	nullptr,				/* tp_dealloc */
	PyItemStack::print,		/* tp_print */
	nullptr,				/* tp_getattr */
	nullptr,				/* tp_setattr */
	nullptr,				/* tp_reserved */
	PyItemStack::repr,		/* tp_repr */
	nullptr,				/* tp_as_number */
	nullptr,				/* tp_as_sequence */
	nullptr,				/* tp_as_mapping */
	PyItemStack::hash,		/* tp_hash */
	nullptr,				/* tp_call */
	PyItemStack::str,		/* tp_str */
	nullptr,				/* tp_getattro */
	nullptr,				/* tp_setattro */
	nullptr,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	nullptr,				/* tp_doc */
	nullptr,				/* tp_traverse */
	nullptr,				/* tp_clear */
	PyItemStack::rich_compare,/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	nullptr,				/* tp_iter */
	nullptr,				/* tp_iternext */
	PyItemStack::methods,	/* tp_methods */
	nullptr,				/* tp_members */
	nullptr,				/* tp_getset */
	nullptr,				/* tp_base */
	nullptr,				/* tp_dict */
	nullptr,				/* tp_descr_get */
	nullptr,				/* tp_descr_set */
	0,						/* tp_dictoffset */
	nullptr,				/* tp_init */
	nullptr,				/* tp_alloc */
	nullptr,				/* tp_new */
	nullptr,				/* tp_free */
	nullptr,				/* tp_is_gc */
	nullptr,				/* tp_bases */
	nullptr,				/* tp_mro */
	nullptr,				/* tp_cache */
	nullptr,				/* tp_subclasses */
	nullptr,				/* tp_weaklist */
	nullptr,				/* tp_del */
	0,						/* tp_version_tag */
	nullptr,				/* tp_finalize */
};

PyObject* ToPyObject(ItemStack* ptr) {
	PyItemStack* obj = PyObject_New(PyItemStack, &PyItemStack_Type);
	obj->value = ptr;
	return reinterpret_cast<PyObject*>(obj);
}
