#include "PyBlockInstance.h"
#include "Tool.h"
#include "NBT.h"

#define Py_GET_BLOCKINSTANCE Py_GET_BLOCKINSTANCE2(nullptr)
#define Py_GET_BLOCKINSTANCE2(ret) BlockInstance* bi = getBlock(self);if (bi == nullptr)return ret

using namespace std;

struct PyBlockInstance {
	PyObject_HEAD;
	BlockInstance* value;

	static BlockInstance* getBlock(PyObject* self) {
		if (reinterpret_cast<PyBlockInstance*>(self)->value)
			return reinterpret_cast<PyBlockInstance*>(self)->value;
		else
			Py_RETURN_ERROR("This value pointer is nullptr");
	}
	static int print(PyObject* self, FILE* file, int) {
		Py_GET_BLOCKINSTANCE2(-1);
		fputs(bi->getBlock()->getName().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		Py_GET_BLOCKINSTANCE2(StrToPyUnicode(""));
		return StrToPyUnicode(bi->getBlock()->getName().getString());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		Py_GET_BLOCKINSTANCE2(StrToPyUnicode(""));
		return StrToPyUnicode(bi->getBlock()->getName().getString());
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
			//<
		case Py_LT:break;
			//<=
		case Py_LE:break;
			//==
		case Py_EQ:
			if (getBlock(self) == getBlock(other))
				Py_RETURN_TRUE;
			else
				Py_RETURN_FALSE;
			break;
			//!=
		case Py_NE:
			if (getBlock(self) != getBlock(other))
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
		Py_GET_BLOCKINSTANCE;
		return StrToPyUnicode(bi->getBlock()->getName().getString());
	}

	inline static PyMethodDef Methods[]{
		{ "getName", getName, METH_NOARGS, nullptr },
		{ nullptr }
	};
};

PyTypeObject PyBlockInstance_Type{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"BlockInstance",				/* tp_name */
	sizeof(PyBlockInstance),		/* tp_basicsize */
	0,						/* tp_blocksize */
	nullptr,				/* tp_dealloc */
	PyBlockInstance::print,			/* tp_print */
	nullptr,				/* tp_getattr */
	nullptr,				/* tp_setattr */
	nullptr,				/* tp_reserved */
	PyBlockInstance::repr,			/* tp_repr */
	nullptr,				/* tp_as_number */
	nullptr,				/* tp_as_sequence */
	nullptr,				/* tp_as_mapping */
	PyBlockInstance::hash,			/* tp_hash */
	nullptr,				/* tp_call */
	PyBlockInstance::str,			/* tp_str */
	nullptr,				/* tp_getattro */
	nullptr,				/* tp_setattro */
	nullptr,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	nullptr,				/* tp_doc */
	nullptr,				/* tp_traverse */
	nullptr,				/* tp_clear */
	PyBlockInstance::rich_compare,	/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	nullptr,				/* tp_iter */
	nullptr,				/* tp_iternext */
	PyBlockInstance::Methods,		/* tp_methods */
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

PyObject* ToPyBlockInstance(BlockInstance* ptr) {
	PyBlockInstance* obj = PyObject_New(PyBlockInstance, &PyBlockInstance_Type);
	obj->value = ptr;
	return reinterpret_cast<PyObject*>(obj);
}
