#pragma once
#define PY_SSIZE_T_CLEAN
#include "include/Python.h"

#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
#define Py_CALL_BEGIN\
	int _has_gil = PyGILState_Check();\
	PyGILState_STATE _gil_state = PyGILState_LOCKED;\
	if (!_has_gil)_gil_state = PyGILState_Ensure();\
	Py_BEGIN_ALLOW_THREADS;\
	Py_BLOCK_THREADS
#define Py_CALL_END\
	Py_UNBLOCK_THREADS;\
	Py_END_ALLOW_THREADS;\
	if (!_has_gil)PyGILState_Release(_gil_state)

struct Actor;
struct Player;
// 实体类型
struct PyEntity {
	PyObject_HEAD;
	Actor* actor;
};

Actor* PyEntity_AsActor(PyObject* self);
Player* PyEntity_AsPlayer(PyObject* self);

//初始化
PyObject* PyEntity_New(PyTypeObject* type, PyObject*, PyObject*);
//构造函数
int PyEntity_Init(PyObject* self, PyObject* args, PyObject*);
//回收
void PyEntity_Dealloc(PyObject* obj);
//转字符串
PyObject* PyEntity_Str(PyObject* self);
//哈希
Py_hash_t PyEntity_Hash(PyObject* self);
//比较
PyObject* PyEntity_RichCompare(PyObject* self, PyObject* other, int op);

//获取名字
PyObject* PyEntity_GetName(PyObject* self, void*);
int PyEntity_SetName(PyObject* self, PyObject* arg, void*);
//获取UUID
PyObject* PyEntity_GetUuid(PyObject* self, void*);
//获取XUID
PyObject* PyEntity_GetXuid(PyObject* self, void*);
//获取坐标
PyObject* PyEntity_GetPos(PyObject* self, void*);
//获取维度ID
PyObject* PyEntity_GetDimensionId(PyObject* self, void*);
//是否着地
PyObject* PyEntity_GetIsStand(PyObject* self, void*);
//是否潜行
PyObject* PyEntity_GetIsSneaking(PyObject* self, void*);
//获取类型
PyObject* PyEntity_GetTypeID(PyObject* self, void*);
//获取类型字符串
PyObject* PyEntity_GetTypeName(PyObject* self, void*);
//获取nbt数据
PyObject* PyEntity_GetNBTInfo(PyObject* self, void*);
//获取生命值
PyObject* PyEntity_GetHealth(PyObject* self, void*);
int PyEntity_SetHealth(PyObject* self, PyObject* arg, void*);
//获取最大生命值
PyObject* PyEntity_GetMaxHealth(PyObject* self, void*);
int PyEntity_SetMaxHealth(PyObject* self, PyObject* arg, void*);
//获取权限
PyObject* PyEntity_GetPermissions(PyObject* self, void*);
int PyEntity_SetPermissions(PyObject* self, PyObject* arg, void*);
//获取设备id
PyObject* PyEntity_GetDeviceId(PyObject* self, void*);
//获取设备类型
PyObject* PyEntity_GetDeviceOS(PyObject* self, void*);
//获取IP
PyObject* PyEntity_GetIP(PyObject* self, void*);

//获取/设置玩家所有物品
PyObject* PyEntity_GetAllItem(PyObject* self, PyObject*);
PyObject* PyEntity_SetAllItem(PyObject* self, PyObject* args);
PyObject* PyEntity_SetHand(PyObject* self, PyObject* args);
//增加/移除物品
PyObject* PyEntity_AddItem(PyObject* self, PyObject* args);
PyObject* PyEntity_RemoveItem(PyObject* self, PyObject* args);
//传送
PyObject* PyEntity_Teleport(PyObject* self, PyObject* args);
//发送数据包
PyObject* PyEntity_SendTextPacket(PyObject* self, PyObject* args);
PyObject* PyEntity_SendCommandPacket(PyObject* self, PyObject* args);
PyObject* PyEntity_ResendAllChunks(PyObject* self, PyObject*);
PyObject* PyEntity_Disconnect(PyObject* self, PyObject* args);
//计分板操作
PyObject* PyEntity_GetScore(PyObject* self, PyObject* args);
PyObject* PyEntity_ModifyScore(PyObject* self, PyObject* args);
//增加等级
PyObject* PyEntity_AddLevel(PyObject* self, PyObject* args);
//跨服传送
PyObject* PyEntity_TransferServer(PyObject* self, PyObject* args);
//发送表单
PyObject* PyEntity_SendCustomForm(PyObject* self, PyObject* args);
PyObject* PyEntity_SendSimpleForm(PyObject* self, PyObject* args);
PyObject* PyEntity_SendModalForm(PyObject* self, PyObject* args);
//设置侧边栏
PyObject* PyEntity_SetSidebar(PyObject* self, PyObject* args);
PyObject* PyEntity_RemoveSidebar(PyObject* self, PyObject*);
//Boss栏
PyObject* PyEntity_SetBossbar(PyObject* self, PyObject* args);
PyObject* PyEntity_RemoveBossbar(PyObject* self, PyObject*);
//标签
PyObject* PyEntity_AddTag(PyObject* self, PyObject* args);
PyObject* PyEntity_RemoveTag(PyObject* self, PyObject* args);
PyObject* PyEntity_GetTags(PyObject* self, PyObject*);

PyObject* PyEntity_FromEntity(Actor* ptr);

//获取属性方法
extern PyGetSetDef PyEntity_GetSet[];
//Entity方法
extern PyMethodDef PyEntity_Methods[];
//Entity类型
extern PyTypeObject PyEntity_Type;
