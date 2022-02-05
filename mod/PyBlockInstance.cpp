#include "PyUtils.h"

#define PyBlockInstance_Check(self) (Py_TYPE(self) == &PyBlockInstance_Type)
#define PyBlockInstance_RAW(self) (reinterpret_cast<PyBlockInstance*>(self))
#define PyBlockInstance_BLOCK auto block = PyBlockInstance_RAW(self)->block
#define PyBlockInstance_POS auto pos = PyBlockInstance_RAW(self)->pos
#define PyBlockInstance_DIM auto dim = PyBlockInstance_RAW(self)->dim

using namespace std;

struct PyBlockInstance {
	PyObject_HEAD;
	Block* block;
	BlockPos pos;
	int dim;

	static int print(PyObject* self, FILE* file, int) {
		PyBlockInstance_BLOCK;
		fputs(block->getName().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		PyBlockInstance_BLOCK;
		return ToPyObject(block->getName().getString());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		PyBlockInstance_BLOCK;
		return ToPyObject(block->getName().getString());
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
		case Py_LT:// <
			break;
		case Py_LE:// <=
			break;
		case Py_EQ:// ==
			break;
		case Py_NE:// !=
			break;
		case Py_GT:// >
			break;
		case Py_GE:// >=
			break;
		}
		Py_RETURN_NOTIMPLEMENTED;
	}

	Py_METHOD_DEFINE(getName) {
		PyBlockInstance_BLOCK;
		return ToPyObject(block->getName().getString());
	}
	Py_METHOD_DEFINE(getNBT) {
		PyBlockInstance_BLOCK;
		return ToPyObject(CompoundTag::fromBlock(block));
	}
	Py_METHOD_DEFINE(getPos) {
		PyBlockInstance_POS;
		return ToPyObject(pos);
	}
	Py_METHOD_DEFINE(getDimensionId) {
		PyBlockInstance_DIM;
		return ToPyObject(dim);
	}

	inline static PyMethodDef methods[] {
		Py_METHOD_NOARGS(getName),
		Py_METHOD_NOARGS(getPos),
		Py_METHOD_NOARGS(getDimensionId),
		Py_METHOD_END
	};
};

PyTypeObject PyBlockInstance_Type {
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
	PyBlockInstance::methods,		/* tp_methods */
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

PyObject* ToPyObject(BlockInstance* ptr) {
	PyBlockInstance* obj = PyObject_New(PyBlockInstance, &PyBlockInstance_Type);
	obj->block = ptr->getBlock();
	obj->pos = ptr->getPosition();
	obj->dim = ptr->getDimensionId();
	return reinterpret_cast<PyObject*>(obj);
}

PyObject* ToPyObject(BlockInstance& bi) {
	PyBlockInstance* obj = PyObject_New(PyBlockInstance, &PyBlockInstance_Type);
	obj->block = bi.getBlock();
	obj->pos = bi.getPosition();
	obj->dim = bi.getDimensionId();
	return reinterpret_cast<PyObject*>(obj);
}
