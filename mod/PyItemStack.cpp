#include "PyItemStack.h"
#include "Tool.h"
#include "NBT.h"

#define Py_GET_ITEMSTACK Py_GET_ITEMSTACK2(nullptr)
#define Py_GET_ITEMSTACK2(ret) ItemStack* i = getItemStack(self);if (i == nullptr)return ret

using namespace std;

struct PyItemStack {
	PyObject_HEAD;
	ItemStack* value;

	static ItemStack* getItemStack(PyObject* self) {
		if (reinterpret_cast<PyItemStack*>(self)->value)
			return reinterpret_cast<PyItemStack*>(self)->value;
		else
			Py_RETURN_ERROR("This value pointer is nullptr");
	}
	static int print(PyObject* self, FILE* file, int) {
		Py_GET_ITEMSTACK2(-1);
		fputs(i->getName().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		Py_GET_ITEMSTACK2(StrToPyUnicode(""));
		return StrToPyUnicode(i->getName());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		Py_GET_ITEMSTACK2(StrToPyUnicode(""));
		return StrToPyUnicode(i->getName());
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
			//<
		case Py_LT:break;
			//<=
		case Py_LE:break;
			//==
		case Py_EQ:
			if (getItemStack(self) == getItemStack(other))
				Py_RETURN_TRUE;
			else
				Py_RETURN_FALSE;
			break;
			//!=
		case Py_NE:
			if (getItemStack(self) != getItemStack(other))
				Py_RETURN_TRUE;
			else
				Py_RETURN_FALSE;
			break;
			//>
		case Py_GT:break;
			//>=
		case Py_GE:break;
		}
		Py_RETURN_NOTIMPLEMENTED;
	}

	static PyObject* getName(PyObject* self, PyObject*) {
		Py_GET_ITEMSTACK;
		return StrToPyUnicode(i->getName());
	}

	inline static PyMethodDef Methods[]{
		{ "getName", getName, METH_NOARGS, nullptr },
		{ nullptr }
	};
};

PyTypeObject PyItemStack_Type{
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
	PyItemStack::Methods,	/* tp_methods */
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

PyObject* ToPyItemStack(ItemStack* ptr) {
	PyItemStack* obj = PyObject_New(PyItemStack, &PyItemStack_Type);
	obj->value = ptr;
	return reinterpret_cast<PyObject*>(obj);
}
