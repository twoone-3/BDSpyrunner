#include "PyUtils.h"
#include "magic_enum.hpp"

#define PyNBT_Check(self) (Py_TYPE(self) == &PyNBT_Type)
#define PyNBT_RAW(self) (reinterpret_cast<PyNBT*>(self)->value)
#define PyNBT_VALUE auto thiz = PyNBT_RAW(self).get(); if (thiz == nullptr)return nullptr

using namespace std;

struct PyNBT {
	PyObject_HEAD;
	unique_ptr<Tag> value;

	static int print(PyObject* self, FILE* file, int) {
		PyNBT_VALUE, -1;
		fputs(thiz->toJson(4).c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		PyNBT_VALUE, ToPyObject("");
		return ToPyObject(thiz->toJson(4));
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		PyNBT_VALUE, ToPyObject("");
		return ToPyObject(thiz->toJson(4));
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
		case Py_LT:// <
			break;
		case Py_LE:// <=
			break;
		case Py_EQ:// ==
			return ToPyObject(PyNBT_RAW(self) == PyNBT_RAW(other));
		case Py_NE:// !=
			return ToPyObject(PyNBT_RAW(self) != PyNBT_RAW(other));
		case Py_GT:// >
			break;
		case Py_GE:// >=
			break;
		}
		Py_RETURN_NOTIMPLEMENTED;
	}
	static int init(PyObject* self, PyObject* args, PyObject* kwds) {
		const char* type_str = "";
		PyObject* value = nullptr;
		Py_KEYWORDS_LIST("type", "value");
		//此处可能晦涩难懂 return nullptr,-1;
		Py_PARSE_WITH_KEYWORDS("s|O", &type_str, &value), -1;
		auto type = magic_enum::enum_cast<Tag::Type>(type_str);
		if (!type)
			Py_RETURN_ERROR_FORMAT("Invalied NBT type %s", type_str), -1;
		switch (type.value()) {
		case Tag::End:
			PyNBT_RAW(self) = EndTag::create();
			break;
		case Tag::Byte:
			PyNBT_RAW(self) = ByteTag::create();
			PyNBT_RAW(self)->asByteTag()->value() = PyLong_AsLong(value);
			break;
		case Tag::Short:
			PyNBT_RAW(self) = ShortTag::create();
			PyNBT_RAW(self)->asShortTag()->value() = PyLong_AsLong(value);
			break;
		case Tag::Int:
			PyNBT_RAW(self) = IntTag::create();
			PyNBT_RAW(self)->asIntTag()->value() = PyLong_AsLong(value);
			break;
		case Tag::Int64:
			PyNBT_RAW(self) = Int64Tag::create();
			PyNBT_RAW(self)->asInt64Tag()->value() = PyLong_AsLongLong(value);
			break;
		case Tag::Float:
			PyNBT_RAW(self) = FloatTag::create();
			PyNBT_RAW(self)->asFloatTag()->value() = PyFloat_AsDouble(value);
			break;
		case Tag::Double:
			PyNBT_RAW(self) = DoubleTag::create();
			PyNBT_RAW(self)->asDoubleTag()->value() = PyFloat_AsDouble(value);
			break;
		case Tag::ByteArray:
			PyNBT_RAW(self) = ByteArrayTag::create();
			//PyNBT_RAW(self)->asByteArrayTag()->value() = PyLong_AsLong(value);
			break;
		case Tag::String:
			PyNBT_RAW(self) = StringTag::create();
			PyNBT_RAW(self)->asStringTag()->value() = PyUnicodeToStr(value);
			break;
		case Tag::List:
			PyNBT_RAW(self) = ListTag::create();
			break;
		case Tag::Compound:
			PyNBT_RAW(self) = CompoundTag::create();
			break;
		case Tag::IntArray:
			PyNBT_RAW(self) = IntArrayTag::create();
			break;
		}
		return 0;
	}
	static PyObject* _new(PyTypeObject* subtype, PyObject* args, PyObject* kwds) {
		return subtype->tp_alloc(&PyNBT_Type, 0);
	}
	static void dealloc(PyObject* self) {

		PyNBT_RAW(self)
			.~unique_ptr();
		Py_TYPE(self)->tp_free(self);
	}
	static Py_ssize_t length(PyObject* self) {
		PyNBT_VALUE, 0;
		if (thiz->getTagType() == Tag::List)
			return thiz->asListTag()->size();
		if (thiz->getTagType() == Tag::Compound)
			return thiz->asCompoundTag()->value().size();
		return 0;
	}
	static PyObject* subscript(PyObject* self, PyObject* key) {
		PyNBT_VALUE;
		if (PyLong_Check(key)) {
			if (thiz->getTagType() != Tag::List)
				Py_RETURN_ERROR("Type of tag must be list");
			auto i = PyLong_AsLong(key);
			if (i >= thiz->asListTag()->size())
				Py_RETURN_ERROR("Out of range");
			return ToPyObject(thiz->asListTag()->get(i));
		}
		else if (PyUnicode_Check(key)) {
			if (thiz->getTagType() != Tag::Compound)
				Py_RETURN_ERROR("Type of tag must be compound");
			auto s = PyUnicodeToStr(key);
			if (!thiz->asCompoundTag()->contains(s))
				Py_RETURN_ERROR_FORMAT("Missing key %s", s);
			return ToPyObject(thiz->asCompoundTag()->get(s)->copy());
		}
		return ToPyObject(0);
	}
	static int ass_subscript(PyObject* self, PyObject* key, PyObject* value) {
		PyNBT_VALUE, -1;
		if (!PyNBT_Check(value))
			Py_RETURN_ERROR("value must be PyNBT"), -1;
		if (PyLong_Check(key)) {
			if (thiz->getTagType() != Tag::List)
				Py_RETURN_ERROR("Type of tag must be list"), -1;
			auto i = PyLong_AsLong(key);
			if (i >= thiz->asListTag()->size())
				Py_RETURN_ERROR("Out of range"), -1;
			thiz->asListTag()->value()[i] = PyNBT_RAW(value).get();
		}
		else if (PyUnicode_Check(key)) {
			if (thiz->getTagType() != Tag::Compound)
				Py_RETURN_ERROR("Type of tag must be compound"), -1;
			auto s = PyUnicodeToStr(key);
			thiz->asCompoundTag()->put(s, PyNBT_RAW(value)->copy());
		}
		return 0;
	}

	Py_METHOD_DEFINE(getType) {
		PyNBT_VALUE;
		return ToPyObject(magic_enum::enum_name(thiz->getTagType()));
	}
	Py_METHOD_DEFINE(toBinary) {
		bool is_little_endian = true;
		Py_PARSE("|b", &is_little_endian);
		PyNBT_VALUE;
		return ToPyObject(thiz->asCompoundTag()->toBinaryNBT(is_little_endian));
	}
	Py_METHOD_DEFINE(toJson) {
		int indentatiton = 4;
		Py_PARSE("|i", &indentatiton);
		PyNBT_VALUE;
		return ToPyObject(thiz->toJson(indentatiton));
	}
	Py_METHOD_DEFINE(toSNBT) {
		PyNBT_VALUE;
		return ToPyObject(thiz->asCompoundTag()->toSNBT());
	}
	Py_METHOD_DEFINE(append) {
		PyObject* value = nullptr;
		Py_PARSE("O", &value);
		PyNBT_VALUE;
		if (thiz->getTagType() != Tag::List)
			Py_RETURN_ERROR("Type of tag must be list");
		if (!PyNBT_Check(value))
			Py_RETURN_ERROR("a1 must be PyNBT");
		//TODO: find out why it don't work
		puts("sss");
		thiz->asListTag()->addString("sss");
		thiz->asListTag()->add(PyNBT_RAW(value)->copy());
		Py_RETURN_NONE;
	}

	inline static PyMethodDef methods[] {
		Py_METHOD_NOARGS(getType),
		Py_METHOD_VARARGS(toBinary),
		Py_METHOD_VARARGS(toJson),
		Py_METHOD_NOARGS(toSNBT),
		Py_METHOD_VARARGS(append),
		Py_METHOD_END
	};
	inline static PyMappingMethods as_mapping {
		length,				/*mp_length*/
		subscript,			/*mp_subscript*/
		ass_subscript		/*mp_ass_subscript*/
	};
};

PyTypeObject PyNBT_Type {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"NBT",					/* tp_name */
	sizeof(PyNBT),			/* tp_basicsize */
	0,						/* tp_itemsize */
	PyNBT::dealloc,			/* tp_dealloc */
	PyNBT::print,			/* tp_print */
	nullptr,				/* tp_getattr */
	nullptr,				/* tp_setattr */
	nullptr,				/* tp_reserved */
	PyNBT::repr,			/* tp_repr */
	nullptr,				/* tp_as_number */
	nullptr,				/* tp_as_sequence */
	&PyNBT::as_mapping,		/* tp_as_mapping */
	PyNBT::hash,			/* tp_hash */
	nullptr,				/* tp_call */
	PyNBT::str,				/* tp_str */
	nullptr,				/* tp_getattro */
	nullptr,				/* tp_setattro */
	nullptr,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	nullptr,				/* tp_doc */
	nullptr,				/* tp_traverse */
	nullptr,				/* tp_clear */
	PyNBT::rich_compare,	/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	nullptr,				/* tp_iter */
	nullptr,				/* tp_iternext */
	PyNBT::methods,			/* tp_methods */
	nullptr,				/* tp_members */
	nullptr,				/* tp_getset */
	nullptr,				/* tp_base */
	nullptr,				/* tp_dict */
	nullptr,				/* tp_descr_get */
	nullptr,				/* tp_descr_set */
	0,						/* tp_dictoffset */
	PyNBT::init,			/* tp_init */
	nullptr,				/* tp_alloc */
	PyNBT::_new,			/* tp_new */
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

PyObject* ToPyObject(unique_ptr<Tag>&& ptr) {
	PyNBT* obj = PyObject_New(PyNBT, &PyNBT_Type);
	new (&obj->value) unique_ptr<Tag>(move(ptr));
	return reinterpret_cast<PyObject*>(obj);
}

std::unique_ptr<Tag> ToNBT(PyObject* nbt) {
	return PyNBT_RAW(nbt)->copy();
}
