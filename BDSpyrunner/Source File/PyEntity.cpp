#include <PyEntity.h>
#include <tool.h>
#include <global.h>
#include <Actor.h>
#include <Tag.h>
#include <ItemStack.h>
#include <NetWork.h>
#include <ScoreBoard.h>

using namespace std;
PyObject* toPyUnicode(const string& str) {
	return PyUnicode_FromStringAndSize(str.c_str(), str.length());
}
Actor* PyEntity_AsActor(PyObject* self) {
	if (reinterpret_cast<PyEntity*>(self)->actor)
		return reinterpret_cast<PyEntity*>(self)->actor;
	else
		Py_RETURN_ERROR("This entity pointer is nullptr");
}
Player* PyEntity_AsPlayer(PyObject* self) {
	if (isPlayer(reinterpret_cast<PyEntity*>(self)->actor))
		return reinterpret_cast<Player*>(reinterpret_cast<PyEntity*>(self)->actor);
	else
		Py_RETURN_ERROR("This entity pointer is nullptr or is not player pointer");
}

//初始化
PyObject* PyEntity_New(PyTypeObject* type, PyObject*, PyObject*) {
	return type->tp_alloc(type, 0);
}

//构造函数
int PyEntity_Init(PyObject* self, PyObject* args, PyObject*) {
	PyEntity* other;
	if (!PyArg_ParseTuple(args, "O", &other)) {
		if (Py_TYPE(other) == &PyEntity_Type) {
			reinterpret_cast<PyEntity*>(self)->actor = other->actor;
			return 0;
		}
	}
	return -1;
}

//回收
void PyEntity_Dealloc(PyObject* obj) {
	Py_TYPE(obj)->tp_free(obj);
}

//转字符串
PyObject* PyEntity_Str(PyObject* self) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return toPyUnicode(a->getNameTag());
}

//哈希
Py_hash_t PyEntity_Hash(PyObject* self) {
	return reinterpret_cast<Py_hash_t>(PyEntity_AsActor(self));
}

//比较
PyObject* PyEntity_RichCompare(PyObject* self, PyObject* other, int op) {
	switch (op) {
		//<
	case Py_LT:break;
		//<=
	case Py_LE:break;
		//==
	case Py_EQ:
		if (PyEntity_AsActor(self) == PyEntity_AsActor(other))
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
PyObject* PyEntity_GetName(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return toPyUnicode(a->getNameTag());
}

int PyEntity_SetName(PyObject* self, PyObject* arg, void*) {
	if (PyUnicode_Check(arg)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return -1;
		p->setNameTag(PyUnicode_AsUTF8(arg));
		return 0;
	}
	return PyErr_BadArgument(), -1;
}

//获取UUID
PyObject* PyEntity_GetUuid(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return toPyUnicode(p->getUuid());
}

//获取XUID
PyObject* PyEntity_GetXuid(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return toPyUnicode(p->getXuid());
}

//获取坐标
PyObject* PyEntity_GetPos(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	Vec3* pos = a->getPos();
	PyObject* x = PyFloat_FromDouble(pos->x);
	PyObject* y = PyFloat_FromDouble(pos->y);
	PyObject* z = PyFloat_FromDouble(pos->z);
	return PyTuple_Pack(3, x, y, z);
}

//获取维度ID
PyObject* PyEntity_GetDimensionId(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getDimensionId());
}

//是否着地
PyObject* PyEntity_GetIsStand(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyBool_FromLong(a->isStand());
}

//是否潜行
PyObject* PyEntity_GetIsSneaking(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyBool_FromLong(a->isSneaking());
}

//获取类型
PyObject* PyEntity_GetTypeID(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getEntityTypeId());
}

//获取类型字符串
PyObject* PyEntity_GetTypeName(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return toPyUnicode(a->getEntityTypeName());
}

//获取nbt数据
PyObject* PyEntity_GetNBTInfo(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return toPyUnicode(CompoundTagtoJson(a->save()).dump(4));
}

//获取生命值
PyObject* PyEntity_GetHealth(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getHealth());
}

int PyEntity_SetHealth(PyObject* self, PyObject* arg, void*) {
	if (PyLong_Check(arg)) {
		Actor* a = PyEntity_AsActor(self);
		if (!a)
			return -1;
		a->setHealth(PyLong_AsLong(arg));
		return 0;
	}
	return PyErr_BadArgument(), -1;
}

//获取最大生命值
PyObject* PyEntity_GetMaxHealth(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getMaxHealth());
}

int PyEntity_SetMaxHealth(PyObject* self, PyObject* arg, void*) {
	if (PyLong_Check(arg)) {
		Actor* a = PyEntity_AsActor(self);
		if (!a)
			return -1;
		a->setMaxHealth(PyLong_AsLong(arg));
		return 0;
	}
	return PyErr_BadArgument(), -1;
}

//获取权限
PyObject* PyEntity_GetPermissions(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return PyLong_FromLong(p->getPermissions());
}

int PyEntity_SetPermissions(PyObject* self, PyObject* arg, void*) {
	if (PyLong_Check(arg)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return -1;
		p->setPermissions((char)PyLong_AsLong(arg));
		return 0;
	}
	return PyErr_BadArgument(), -1;
}

//获取设备id
PyObject* PyEntity_GetDeviceId(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return toPyUnicode(p->getDeviceId());
}

//获取设备类型
PyObject* PyEntity_GetDeviceOS(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return PyLong_FromLong(p->getDeviceOS());
}

//获取IP
PyObject* PyEntity_GetIP(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return toPyUnicode(g_rak_peer->getSystemAddress(p->getClientId()).toString());
}

//获取/设置玩家所有物品
PyObject* PyEntity_GetAllItem(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	Json value;

	Json& inventory = value["Inventory"];
	for (auto& i : p->getInventory()->getSlots()) {
		inventory.push_back(CompoundTagtoJson(i->save()));
	}

	Json& endchest = value["EndChest"];
	for (auto& i : p->getEnderChestContainer()->getSlots()) {
		endchest.push_back(CompoundTagtoJson(i->save()));
	}

	Json& armor = value["Armor"];
	for (auto& i : p->getArmorContainer()->getSlots()) {
		armor.push_back(CompoundTagtoJson(i->save()));
	}

	value["OffHand"] = CompoundTagtoJson(p->getOffHand()->save());
	value["Hand"] = CompoundTagtoJson(p->getSelectedItem()->save());

	return toPyUnicode(value.dump(4));
}

PyObject* PyEntity_SetAllItem(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setAllItem", &x)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		Json value(StringtoJson(x));

		if (value.contains("Inventory")) {
			const vector<ItemStack*>& items = p->getInventory()->getSlots();
			Json& inventory = value["Inventory"];
			for (unsigned i = 0; i < inventory.size(); i++) {
				items[i]->fromJson(inventory[i]);
			}
		}

		if (value.contains("EndChest")) {
			const vector<ItemStack*>& items = p->getEnderChestContainer()->getSlots();
			Json& endchest = value["EndChest"];
			for (unsigned i = 0; i < endchest.size(); i++) {
				items[i]->fromJson(endchest[i]);
			}
		}

		if (value.contains("Armor")) {
			const vector<ItemStack*>& items = p->getArmorContainer()->getSlots();
			Json& armor = value["Armor"];
			for (unsigned i = 0; i < armor.size(); i++) {
				items[i]->fromJson(armor[i]);
			}
		}

		if (value.contains("OffHand")) {
			p->getOffHand()->fromJson(value["OffHand"]);
		}
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SetHand(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setHand", &x)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		Json json(StringtoJson(x));
		p->getSelectedItem()->fromJson(json);
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}

//增加/移除物品
PyObject* PyEntity_AddItem(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:addItem", &x)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		ItemStack i;
		i.fromJson(StringtoJson(x));
		p->addItem(&i);
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveItem(PyObject* self, PyObject* args) {
	int slot, num;
	if (PyArg_ParseTuple(args, "ii:removeItem", &slot, &num)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->getInventory()->clearItem(slot, num);
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}

//传送
PyObject* PyEntity_Teleport(PyObject* self, PyObject* args) {
	Vec3 pos; int did;
	if (PyArg_ParseTuple(args, "fffi:teleport", &pos.x, &pos.y, &pos.z, &did)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->teleport(&pos, did);
	}
	Py_RETURN_NONE;
}

//发送数据包
PyObject* PyEntity_SendTextPacket(PyObject* self, PyObject* args) {
	const char* msg = "";
	int mode = 0;
	if (PyArg_ParseTuple(args, "s|i:sendTextPacket", &msg, &mode)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->sendTextPacket(mode, msg);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SendCommandPacket(PyObject* self, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:sendCommandPacket", &cmd)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->sendCommandRequestPacket(cmd);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_ResendAllChunks(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	p->resendAllChunks();
	Py_RETURN_NONE;
}

PyObject* PyEntity_Disconnect(PyObject* self, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "s:disconnect", &msg)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->sendDisconnectPacket(msg);
	}
	Py_RETURN_NONE;
}

//计分板操作
PyObject* PyEntity_GetScore(PyObject* self, PyObject* args) {
	const char* objname = "";
	if (PyArg_ParseTuple(args, "s:getScore", &objname)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		Objective* testobj = g_scoreboard->getObjective(objname);
		if (testobj) {
			auto id = g_scoreboard->getScoreboardId(p);
			auto score = testobj->getPlayerScore(id);
			return PyLong_FromLong(score->getCount());
		}
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_ModifyScore(PyObject* self, PyObject* args) {
	const char* objname = ""; int count; int mode;
	if (PyArg_ParseTuple(args, "sii:modifyScore", &objname, &count, &mode)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		Objective* testobj = g_scoreboard->getObjective(objname);
		if (testobj) {
			//mode:{set,add,remove}
			g_scoreboard->modifyPlayerScore((ScoreboardId*)g_scoreboard->getScoreboardId(p), testobj, count, mode);
		}
	}
	Py_RETURN_NONE;
}

//增加等级
PyObject* PyEntity_AddLevel(PyObject* self, PyObject* args) {
	int level;
	if (PyArg_ParseTuple(args, "i:addLevel", &level)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->addLevel(level);
	}
	Py_RETURN_NONE;
}

//跨服传送
PyObject* PyEntity_TransferServer(PyObject* self, PyObject* args) {
	const char* address = "";
	unsigned short port;
	if (PyArg_ParseTuple(args, "sH:transferServer", &address, &port)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->sendTransferPacket(address, port);
	}
	Py_RETURN_NONE;
}

//发送表单
PyObject* PyEntity_SendCustomForm(PyObject* self, PyObject* args) {
	const char* str = "";
	if (PyArg_ParseTuple(args, "s:sendCustomForm", &str)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		return PyLong_FromLong(p->sendModalFormRequestPacket(str));
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SendSimpleForm(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* buttons = "";
	if (PyArg_ParseTuple(args, "sss:sendSimpleForm", &title, &content, &buttons)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})", title, content, buttons);
		return PyLong_FromLong(p->sendModalFormRequestPacket(str));
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SendModalForm(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* button1 = "";
	const char* button2 = "";
	if (PyArg_ParseTuple(args, "ssss:sendModalForm", &title, &content, &button1, &button2)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","button1":"%s","button2":"%s","type":"modal"})", title, content, button1, button2);
		return PyLong_FromLong(p->sendModalFormRequestPacket(str));
	}
	Py_RETURN_NONE;
}

//设置侧边栏
PyObject* PyEntity_SetSidebar(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* data = "";
	if (PyArg_ParseTuple(args, "ss:setSidebar", &title, &data)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->sendsetDisplayObjectivePacket(title);
		Json value = StringtoJson(data);
		vector<ScorePacketInfo> info;
		if (value.is_object())
			for (auto& [key, val] : value.items()) {
				ScorePacketInfo o(g_scoreboard->createScoreBoardId(key),
					val.get<int>(), key);
				info.push_back(o);
			}
		p->sendSetScorePacket(0, info);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveSidebar(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	p->sendsetDisplayObjectivePacket("", "");
	Py_RETURN_NONE;
}

//Boss栏
PyObject* PyEntity_SetBossbar(PyObject* self, PyObject* args) {
	const char* name = "";
	float per;
	if (PyArg_ParseTuple(args, "sf:setBossbar", &name, &per)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		p->sendBossEventCodePacket(name, per, 0);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveBossbar(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	p->sendBossEventCodePacket("", 0, 2);
	Py_RETURN_NONE;
}

//标签
PyObject* PyEntity_AddTag(PyObject* self, PyObject* args) {
	const char* tag = "";
	if (PyArg_ParseTuple(args, "s:addTag", &tag)) {
		Actor* a = PyEntity_AsActor(self);
		if (!a)
			return nullptr;
		a->addTag(tag);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveTag(PyObject* self, PyObject* args) {
	const char* tag = "";
	if (PyArg_ParseTuple(args, "s:removeTag", &tag)) {
		Actor* a = PyEntity_AsActor(self);
		if (!a)
			return nullptr;
		a->removeTag(tag);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_GetTags(PyObject* self, PyObject*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	span<string> tags = a->getTags();
	PyObject* list = PyList_New(0);
	for (size_t i = 0; i < tags.size; i++) {
		PyList_Append(list, toPyUnicode(tags.data[i]));
	}
	return list;
}

//崩溃客户端
PyObject* PyEntity_Crash(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	p->crash();
	Py_RETURN_NONE;
}

//杀死实体
PyObject* PyEntity_Kill(PyObject* self, PyObject*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	a->kill();
	Py_RETURN_NONE;
}

PyObject* PyEntity_FromEntity(Actor* ptr) {
	PyObject* obj;
	Py_CALL_BEGIN;
	obj = PyEntity_Type.tp_alloc(&PyEntity_Type, 0);
	Py_CALL_END;
	reinterpret_cast<PyEntity*>(obj)->actor = ptr;
	return obj;
}

//获取属性方法
PyGetSetDef PyEntity_GetSet[]{
	{"name", PyEntity_GetName, PyEntity_SetName, nullptr},
	{"uuid", PyEntity_GetUuid, nullptr, nullptr},
	{"xuid", PyEntity_GetXuid, nullptr, nullptr},
	{"pos", PyEntity_GetPos, nullptr, nullptr},
	{"did", PyEntity_GetDimensionId, nullptr, nullptr},
	{"isstand", PyEntity_GetIsStand, nullptr, nullptr},
	{"issneak", PyEntity_GetIsSneaking, nullptr, nullptr},
	{"typeid", PyEntity_GetTypeID, nullptr, nullptr},
	{"typename", PyEntity_GetTypeName, nullptr, nullptr},
	{"nbt", PyEntity_GetNBTInfo, nullptr, nullptr},
	{"health", PyEntity_GetHealth, PyEntity_SetHealth, nullptr},
	{"maxhealth", PyEntity_GetMaxHealth, PyEntity_SetMaxHealth, nullptr},
	{"perm", PyEntity_GetPermissions, PyEntity_SetPermissions, nullptr},
	{"deviceid", PyEntity_GetDeviceId, nullptr, nullptr},
	{"deviceos", PyEntity_GetDeviceOS, nullptr, nullptr},
	{"ip", PyEntity_GetIP, nullptr, nullptr},
	{nullptr}
};
//Entity方法
PyMethodDef PyEntity_Methods[]{
	{"getAllItem", PyEntity_GetAllItem, METH_VARARGS, nullptr},
	{"setAllItem", PyEntity_SetAllItem, METH_VARARGS, nullptr},
	{"setHand", PyEntity_SetHand, METH_VARARGS, nullptr},
	{"addItem", PyEntity_AddItem, METH_VARARGS, nullptr},
	{"removeItem", PyEntity_RemoveItem, METH_VARARGS, nullptr},
	{"teleport", PyEntity_Teleport, METH_VARARGS, nullptr},
	{"sendTextPacket", PyEntity_SendTextPacket, METH_VARARGS, nullptr},
	{"sendCommandPacket", PyEntity_SendCommandPacket, METH_VARARGS, nullptr},
	{"resendAllChunks", PyEntity_ResendAllChunks, METH_NOARGS, nullptr},
	{"disconnect", PyEntity_Disconnect, METH_VARARGS, nullptr},
	{"getScore", PyEntity_GetScore, METH_VARARGS, nullptr},
	{"modifyScore", PyEntity_ModifyScore, METH_VARARGS, nullptr},
	{"addLevel", PyEntity_AddLevel, METH_VARARGS, nullptr},
	{"transferServer", PyEntity_TransferServer, METH_VARARGS, nullptr},
	{"sendCustomForm", PyEntity_SendCustomForm, METH_VARARGS, nullptr},
	{"sendSimpleForm", PyEntity_SendSimpleForm, METH_VARARGS, nullptr},
	{"sendModalForm", PyEntity_SendModalForm, METH_VARARGS, nullptr},
	{"setSidebar", PyEntity_SetSidebar, METH_VARARGS, nullptr},
	{"removeSidebar", PyEntity_RemoveSidebar, METH_NOARGS, nullptr},
	{"setBossbar", PyEntity_SetBossbar, METH_VARARGS, nullptr},
	{"removeBossbar", PyEntity_RemoveBossbar, METH_NOARGS, nullptr},
	{"addTag", PyEntity_AddTag, METH_VARARGS, nullptr},
	{"removeTag", PyEntity_RemoveTag, METH_VARARGS, nullptr},
	{"getTags", PyEntity_GetTags, METH_NOARGS, nullptr},
	{"crash", PyEntity_Crash, METH_NOARGS, nullptr},
	{"kill", PyEntity_Kill, METH_NOARGS, nullptr},
	{nullptr}
};
//Entity类型
PyTypeObject PyEntity_Type{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Entity",				/* tp_name */
	sizeof(PyEntity),		/* tp_basicsize */
	0,						/* tp_itemsize */
	PyEntity_Dealloc,		/* tp_dealloc */
	nullptr,				/* tp_print */
	nullptr,				/* tp_getattr */
	nullptr,				/* tp_setattr */
	nullptr,				/* tp_reserved */
	nullptr,				/* tp_repr */
	nullptr,				/* tp_as_number */
	nullptr,				/* tp_as_sequence */
	nullptr,				/* tp_as_mapping */
	PyEntity_Hash,			/* tp_hash */
	nullptr,				/* tp_call */
	PyEntity_Str,			/* tp_str */
	nullptr,				/* tp_getattro */
	nullptr,				/* tp_setattro */
	nullptr,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	"Player* or Actor*.",	/* tp_doc */
	nullptr,				/* tp_traverse */
	nullptr,				/* tp_clear */
	PyEntity_RichCompare,	/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	nullptr,				/* tp_iter */
	nullptr,				/* tp_iternext */
	PyEntity_Methods,		/* tp_methods */
	nullptr,				/* tp_members */
	PyEntity_GetSet,		/* tp_getset */
	nullptr,				/* tp_base */
	nullptr,				/* tp_dict */
	nullptr,				/* tp_descr_get */
	nullptr,				/* tp_descr_set */
	0,						/* tp_dictoffset */
	PyEntity_Init,			/* tp_init */
	nullptr,				/* tp_alloc */
	PyEntity_New,			/* tp_new */
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
