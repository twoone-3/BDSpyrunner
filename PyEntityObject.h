#pragma once
#include "BDS.hpp"
//#include "include/structmember.h"

static bool isPlayer(const void* ptr);
static void safeCall(const function<void()>& fn);

//玩家指针类型
struct PyEntityObject {
	PyObject_HEAD;
	Actor* ptr_;

	Player* asPlayer() {
		return (Player*)ptr_;
	}
};
//初始化
static PyObject* PyEntity_New(PyTypeObject* type, PyObject*, PyObject*) {
	PyEntityObject* self = (PyEntityObject*)type->tp_alloc(type, 0);
	return (PyObject*)self;
}
//回收
static void PyEntity_Dealloc(PyObject* obj) {
	Py_TYPE(obj)->tp_free(obj);
}
//转字符串
static PyObject* PyEntity_Str(PyEntityObject* self) {
	char str[32];
	sprintf_s(str, 31, "%p", self->ptr_);
	return PyUnicode_FromString(str);
}
//哈希
static Py_hash_t PyEntity_Hash(PyObject* self) {
	return (Py_hash_t)((PyEntityObject*)self)->ptr_;
}
//比较
static PyObject* PyEntity_RichCompare(PyObject* self, PyObject* other, int op) {
	switch (op) {
		//<
	case Py_LT:break;
		//<=
	case Py_LE:break;
		//==
	case Py_EQ:
		if (((PyEntityObject*)self)->ptr_ == ((PyEntityObject*)other)->ptr_)
			Py_RETURN_TRUE;
		else
			Py_RETURN_FALSE;
		break;
		//!=
	case Py_NE:break;
		//>
	case Py_GT:break;
		//>=
	case Py_GE:break;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

//获取名字
static PyObject* PyEntity_GetName(PyEntityObject* self, void*) {
	return PyUnicode_FromString(self->asPlayer()->getNameTag().c_str());
}
static int PyEntity_SetName(PyEntityObject* self, PyObject* arg, void*) {
	if (PyObject_TypeCheck(arg, &PyUnicode_Type)) {
		if (isPlayer(self->ptr_)) {
			self->asPlayer()->setName(PyUnicode_AsUTF8(arg));
			return 0;
		}
		PyErr_Format(PyExc_Exception, "%p isn't Player*", self->ptr_);
	}
	else
		PyErr_BadArgument();
	return -1;
}
//获取UUID
static PyObject* PyEntity_GetUuid(PyEntityObject* self, void*) {
	if (isPlayer(self->ptr_))
		return PyUnicode_FromString(self->asPlayer()->getUuid().c_str());
	PyErr_Format(PyExc_Exception, "%p isn't Player*", self->ptr_);
	return nullptr;
}
//获取XUID
static PyObject* PyEntity_GetXuid(PyEntityObject* self, void*) {
	if (isPlayer(self->ptr_))
		return PyUnicode_FromString(self->asPlayer()->getXuid().c_str());
	PyErr_Format(PyExc_Exception, "%p isn't Player*", self->ptr_);
	return nullptr;
}
//获取坐标
static PyObject* PyEntity_GetPos(PyEntityObject* self, void*) {
	Vec3* pos = self->asPlayer()->getPos();
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(pos->x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(pos->y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(pos->z));
	return list;
}
//获取维度ID
static PyObject* PyEntity_GetDimensionId(PyEntityObject* self, void*) {
	return PyLong_FromLong(self->ptr_->getDimensionId());
}
//是否着地
static PyObject* PyEntity_GetIsStand(PyEntityObject* self, void*) {
	return PyBool_FromLong(self->asPlayer()->isStand());
}
//获取生命值
static PyObject* PyEntity_GetHealth(PyEntityObject* self, void*) {
	return PyLong_FromLong(self->asPlayer()->getHealth());
}
//获取最大生命值
static PyObject* PyEntity_GetMaxHealth(PyEntityObject* self, void*) {
	return PyLong_FromLong(self->asPlayer()->getMaxHealth());
}
//获取权限
static PyObject* PyEntity_GetPermissions(PyEntityObject* self, void*) {
	if (isPlayer(self->ptr_))
		return PyLong_FromLong(self->asPlayer()->getPermissions());
	PyErr_BadArgument();
	return nullptr;
}
static int PyEntity_SetPermissions(PyEntityObject* self, PyObject* arg, void*) {
	if (PyObject_TypeCheck(arg, &PyUnicode_Type)) {
		if (isPlayer(self->ptr_)) {
			self->asPlayer()->setPermissions((char)PyLong_AsLong(arg));
			return 0;
		}
		PyErr_Format(PyExc_Exception, "%p isn't Player*", self->ptr_);
	}
	else
		PyErr_BadArgument();
	return -1;
}

static PyGetSetDef PyEntity_GetSet[]
{
	{"name", (getter)PyEntity_GetName, (setter)PyEntity_SetName, nullptr},
	{"uuid", (getter)PyEntity_GetUuid, nullptr, nullptr},
	{"xuid", (getter)PyEntity_GetXuid, nullptr, nullptr},
	{"pos", (getter)PyEntity_GetPos, nullptr, nullptr},
	{"did", (getter)PyEntity_GetDimensionId, nullptr, nullptr},
	{"isstand", (getter)PyEntity_GetIsStand, nullptr, nullptr},
	{"health", (getter)PyEntity_GetHealth, nullptr, nullptr},
	{"maxhealth", (getter)PyEntity_GetMaxHealth, nullptr, nullptr},
	{"perm", (getter)PyEntity_GetPermissions, (setter)PyEntity_SetPermissions, nullptr},
	{nullptr}
};

extern PyMethodDef PyEntity_Methods[];

static PyTypeObject PyEntity_Type
{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"EntityObject",                 /* tp_name */
	sizeof(PyEntityObject),   /* tp_basicsize */
	0,                       /* tp_itemsize */
	PyEntity_Dealloc,     /* tp_dealloc */
	nullptr,             /* tp_print */
	nullptr,             /* tp_getattr */
	nullptr,             /* tp_setattr */
	nullptr,             /* tp_reserved */
	nullptr,             /* tp_repr */
	nullptr,             /* tp_as_number */
	nullptr,             /* tp_as_sequence */
	nullptr,             /* tp_as_mapping */
	PyEntity_Hash,             /* tp_hash  */
	nullptr,             /* tp_call */
	(reprfunc)PyEntity_Str,             /* tp_str */
	nullptr,             /* tp_getattro */
	nullptr,             /* tp_setattro */
	nullptr,             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,    /* tp_flags */
	"Type of Player* or Actor*.",       /* tp_doc */
	nullptr,             /* tp_traverse */
	nullptr,             /* tp_clear */
	PyEntity_RichCompare,             /* tp_richcompare */
	0,             /* tp_weaklistoffset */
	nullptr,             /* tp_iter */
	nullptr,             /* tp_iternext */
	PyEntity_Methods,          /* tp_methods */
	nullptr,             /* tp_members */
	PyEntity_GetSet,           /* tp_getset */
	nullptr,             /* tp_base */
	nullptr,             /* tp_dict */
	nullptr,             /* tp_descr_get */
	nullptr,             /* tp_descr_set */
	0,                   /* tp_dictoffset */
	nullptr,             /* tp_init */
	nullptr,             /* tp_alloc */
	PyEntity_New         /* tp_new */
};

static PyObject* PyEntity_FromEntity(Actor* ptr) {
	PyObject* obj;
	safeCall([&] {
		if (!PyType_Ready(&PyEntity_Type))
			obj = PyObject_CallFunctionObjArgs((PyObject*)&PyEntity_Type, nullptr);
		}
	);
	((PyEntityObject*)obj)->ptr_ = ptr;
	return obj;
}
