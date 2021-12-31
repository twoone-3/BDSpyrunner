#include "Entity.h"
#include "Tool.h"
#include <MC/Scoreboard.hpp>

using namespace std;
struct PyEntity {
	PyObject_HEAD;
	Actor* actor;

	static Actor* asActor(PyObject* self) {
		if (reinterpret_cast<PyEntity*>(self)->actor)
			return reinterpret_cast<PyEntity*>(self)->actor;
		else
			Py_RETURN_ERROR("This entity pointer is nullptr");
	}
	static Player* asPlayer(PyObject* self) {
		if (IsPlayer(reinterpret_cast<PyEntity*>(self)->actor))
			return reinterpret_cast<Player*>(reinterpret_cast<PyEntity*>(self)->actor);
		else
			Py_RETURN_ERROR("This entity pointer is nullptr or is not player pointer");
	}
	static int print(PyObject* self, FILE* file, int) {
		Actor* a = asActor(self);
		if (!a)
			return -1;
		fputs(a->getNameTag().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		Actor* a = asActor(self);
		if (!a)
			return ToPyStr("");
		return ToPyStr(a->getNameTag());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(asActor(self));
	}
	static PyObject* str(PyObject* self) {
		Actor* a = asActor(self);
		if (!a)
			return ToPyStr("");
		return ToPyStr(a->getNameTag());
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
			//<
		case Py_LT:break;
			//<=
		case Py_LE:break;
			//==
		case Py_EQ:
			if (PyEntity::asActor(self) == PyEntity::asActor(other))
				Py_RETURN_TRUE;
			else
				Py_RETURN_FALSE;
			break;
			//!=
		case Py_NE:
			if (PyEntity::asActor(self) != PyEntity::asActor(other))
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

	//获取名字
	static PyObject* getName(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return ToPyStr(a->getNameTag());
	}
	static int setName(PyObject* self, PyObject* arg, void*) {
		if (PyUnicode_Check(arg)) {
			Player* p = PyEntity::asPlayer(self);
			if (!p)
				return -1;
			p->setName(PyUnicode_AsUTF8(arg));
			return 0;
		}
		return PyErr_BadArgument(), -1;
	}
	//获取UUID
	static PyObject* getUuid(PyObject* self, void*) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		return ToPyStr(p->getUuid());
	}
	//获取XUID
	static PyObject* getXuid(PyObject* self, void*) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		return ToPyStr(p->getXuid());
	}
	//获取坐标
	static PyObject* getPos(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return ToList(a->getPos());
	}
	//获取维度ID
	static PyObject* getDimensionId(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return PyLong_FromLong(a->getDimensionId());
	}
	//是否着地
	static PyObject* getIsStand(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return PyBool_FromLong(a->isStanding());
	}
	//是否潜行
	static PyObject* getIsSneaking(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return PyBool_FromLong(a->isSneaking());
	}
	//获取类型
	static PyObject* getTypeID(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return PyLong_FromLong(a->getEntityTypeId());
	}
	//获取类型字符串
	static PyObject* getTypeName(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		string type;
		SymCall<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
			&type, a->getEntityTypeId());
		return ToPyStr(type);
	}
	//获取nbt数据
	static PyObject* getNBTInfo(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		CompoundTag t;
		a->save(t);
		return ToPyStr(t.toSNBT());
	}
	//获取生命值
	static PyObject* getHealth(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return PyLong_FromLong(a->getHealth());
	}
	static int setHealth(PyObject* self, PyObject* arg, void*) {
		if (PyLong_Check(arg)) {
			Actor* a = PyEntity::asActor(self);
			if (!a)
				return -1;
			//不知行不行
			a->serializationSetHealth(PyLong_AsLong(arg));
			return 0;
		}
		return PyErr_BadArgument(), -1;
	}
	//获取最大生命值
	static PyObject* getMaxHealth(PyObject* self, void*) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		return PyLong_FromLong(a->getMaxHealth());
	}
	static int setMaxHealth(PyObject* self, PyObject* arg, void*) {
		if (PyLong_Check(arg)) {
			Actor* a = PyEntity::asActor(self);
			if (!a)
				return -1;
			logger.error(__FILE__, __LINE__, "此函数目前无法使用");
			//a->setMaxHealth(PyLong_AsLong(arg));
			return 0;
		}
		return PyErr_BadArgument(), -1;
	}
	//获取权限
	static PyObject* getPermissions(PyObject* self, void*) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		return PyLong_FromLong(static_cast<int>(p->getPlayerPermissionLevel()));
	}
	static int setPermissions(PyObject* self, PyObject* arg, void*) {
		if (PyLong_Check(arg)) {
			Player* p = PyEntity::asPlayer(self);
			if (!p)
				return -1;
			p->setPermissions((CommandPermissionLevel)PyLong_AsLong(arg));
			return 0;
		}
		return PyErr_BadArgument(), -1;
	}
	//获取设备id
	static PyObject* getPlatformOnlineId(PyObject* self, void*) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		return ToPyStr(p->getPlatformOnlineId());
	}
	//获取设备类型
	static PyObject* getPlatform(PyObject* self, void*) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		return PyLong_FromLong(p->getPlatform());
	}
	//获取IP
	static PyObject* getIP(PyObject* self, void*) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		auto& ni = *p->getNetworkIdentifier();
		return ToPyStr(global<RakNet::RakPeer>->getAdr(ni).ToString(false,':'));
	}
};

//获取/设置玩家所有物品
PyObject* PyEntity_GetAllItem(PyObject* self, PyObject*) {
	Player* p = PyEntity::asPlayer(self);
	if (!p)
		return nullptr;
	json value;

	json& inventory = value["Inventory"];
	for (auto& i : p->getInventory().getSlots()) {
		inventory.push_back(i->save()->toSNBT());
	}

	json& endchest = value["EndChest"];
	for (auto& i : p->getEnderChestContainer()->getSlots()) {
		endchest.push_back(i->save()->toSNBT());
	}

	json& armor = value["Armor"];
	for (auto& i : p->getArmorContainer()->getSlots()) {
		armor.push_back(i->save()->toSNBT());
	}

	value["OffHand"] = CompoundTagtoJson(p->getOffhandSlot()->save());
	value["Hand"] = CompoundTagtoJson(p->getSelectedItem()->save());

	return ToPyStr(value.dump(4));
}

PyObject* PyEntity_SetAllItem(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setAllItem", &x)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		json value(StringToJson(x));

		if (value.contains("Inventory")) {
			const vector<ItemStack*>& items = p->getInventory()->getSlots();
			json& inventory = value["Inventory"];
			for (unsigned i = 0; i < inventory.size(); i++) {
				items[i]->fromJson(inventory[i]);
			}
		}

		if (value.contains("EndChest")) {
			const vector<ItemStack*>& items = p->getEnderChestContainer()->getSlots();
			json& endchest = value["EndChest"];
			for (unsigned i = 0; i < endchest.size(); i++) {
				items[i]->fromJson(endchest[i]);
			}
		}

		if (value.contains("Armor")) {
			const vector<ItemStack*>& items = p->getArmorContainer()->getSlots();
			json& armor = value["Armor"];
			for (unsigned i = 0; i < armor.size(); i++) {
				items[i]->fromJson(armor[i]);
			}
		}

		if (value.contains("OffHand")) {
			p->getOffhandSlot()->fromJson(value["OffHand"]);
		}
		p->sendInventory(true);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SetHand(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setHand", &x)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		json json(StringToJson(x));
		p->getSelectedItem()->fromJson(json);
		p->sendInventory(true);
	}
	Py_RETURN_NONE;
}

//增加/移除物品
PyObject* PyEntity_AddItem(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:addItem", &x)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		ItemStack i;
		i.fromJson(StringToJson(x));
		p->giveItem(i);
		p->sendInventory(true);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveItem(PyObject* self, PyObject* args) {
	int slot, num;
	if (PyArg_ParseTuple(args, "ii:removeItem", &slot, &num)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->getInventory()->clearItem(slot, num);
		p->sendInventory(true); //idk what the true means
	}
	Py_RETURN_NONE;
}

//传送
PyObject* PyEntity_Teleport(PyObject* self, PyObject* args) {
	Vec3 pos; int did;
	if (PyArg_ParseTuple(args, "fffi:teleport", &pos.x, &pos.y, &pos.z, &did)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->teleport(pos, did);
	}
	Py_RETURN_NONE;
}

//发送数据包
PyObject* PyEntity_SendTextPacket(PyObject* self, PyObject* args) {
	const char* msg = "";
	int mode = 0;
	if (PyArg_ParseTuple(args, "s|i:sendTextPacket", &msg, &mode)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->sendTextPacket(msg, (TextType)mode);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SendCommandPacket(PyObject* self, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:sendCommandPacket", &cmd)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->sendCommandRequestPacket(cmd);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_ResendAllChunks(PyObject* self, PyObject*) {
	Player* p = PyEntity::asPlayer(self);
	if (!p)
		return nullptr;
	p->resendAllChunks();
	Py_RETURN_NONE;
}

PyObject* PyEntity_Disconnect(PyObject* self, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "|s:disconnect", &msg)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->kick(msg);
	}
	Py_RETURN_NONE;
}

//计分板操作
PyObject* PyEntity_GetScore(PyObject* self, PyObject* args) {
	const char* objname = "";
	if (PyArg_ParseTuple(args, "s:getScore", &objname)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		Objective* testobj = Global<Scoreboard>->getObjective(objname);
		if (testobj) {
			auto id = Global<Scoreboard>->getScoreboardId(*p);
			auto score = testobj->getPlayerScore(id);
			return PyLong_FromLong(score.getCount());
		}
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_ModifyScore(PyObject* self, PyObject* args) {
	const char* objname = ""; int count; int mode;
	if (PyArg_ParseTuple(args, "sii:modifyScore", &objname, &count, &mode)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		Objective* testobj = Global<Scoreboard>->getObjective(objname);
		if (testobj) {
			//mode:{set,add,remove}
			Global<Scoreboard>->modifyPlayerScore(Global<Scoreboard>->getScoreboardId(*p), testobj, count, mode); // Todo
		}
	}
	Py_RETURN_NONE;
}

//增加等级
PyObject* PyEntity_AddLevel(PyObject* self, PyObject* args) {
	int level;
	if (PyArg_ParseTuple(args, "i:addLevel", &level)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->addLevels(level);
	}
	Py_RETURN_NONE;
}

//跨服传送
PyObject* PyEntity_TransferServer(PyObject* self, PyObject* args) {
	const char* address = "";
	unsigned short port;
	if (PyArg_ParseTuple(args, "sH:transferServer", &address, &port)) {
		Player* p = PyEntity::asPlayer(self);
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
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		return PyLong_FromLong(p->sendModalFormPacket(str)); // Todo
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SendSimpleForm(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* buttons = "";
	if (PyArg_ParseTuple(args, "sss:sendSimpleForm", &title, &content, &buttons)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})", title, content, buttons);
		return PyLong_FromLong(p->sendModalFormPacket(str)); // Todo
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_SendModalForm(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* button1 = "";
	const char* button2 = "";
	if (PyArg_ParseTuple(args, "ssss:sendModalForm", &title, &content, &button1, &button2)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","button1":"%s","button2":"%s","type":"modal"})", title, content, button1, button2);
		return PyLong_FromLong(p->sendModalFormPacket(str)); // Todo
	}
	Py_RETURN_NONE;
}

//设置侧边栏
PyObject* PyEntity_SetSidebar(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* data = "";
	if (PyArg_ParseTuple(args, "ss:setSidebar", &title, &data)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->sendSetDisplayObjectivePacket(title); // Todo
		json value = StringToJson(data);
		vector<ScorePacketInfo> info;
		if (value.is_object())
			for (auto& [key, val] : value.items()) {
				ScorePacketInfo o(Global<Scoreboard>->createScoreboardId(key)); // Todo
					val.get<int>(), key);
				info.push_back(o);
			}
		p->sendSetScorePacket(0, info);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveSidebar(PyObject* self, PyObject*) {
	Player* p = PyEntity::asPlayer(self);
	if (!p)
		return nullptr;
	p->sendSetDisplayObjectivePacket("", "");
	Py_RETURN_NONE;
}

//Boss栏
PyObject* PyEntity_SetBossbar(PyObject* self, PyObject* args) {
	const char* name = "";
	float per;
	if (PyArg_ParseTuple(args, "sf:setBossbar", &name, &per)) {
		Player* p = PyEntity::asPlayer(self);
		if (!p)
			return nullptr;
		p->sendBossEventPacket(name, per, 0); // Todo
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveBossbar(PyObject* self, PyObject*) {
	Player* p = PyEntity::asPlayer(self);
	if (!p)
		return nullptr;
	p->sendBossEventPacket("", 0, 2); // Todo
	Py_RETURN_NONE;
}

//标签
PyObject* PyEntity_AddTag(PyObject* self, PyObject* args) {
	const char* tag = "";
	if (PyArg_ParseTuple(args, "s:addTag", &tag)) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		a->addTag(tag);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_RemoveTag(PyObject* self, PyObject* args) {
	const char* tag = "";
	if (PyArg_ParseTuple(args, "s:removeTag", &tag)) {
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		a->removeTag(tag);
	}
	Py_RETURN_NONE;
}

PyObject* PyEntity_GetTags(PyObject* self, PyObject*) {
	Actor* a = PyEntity::asActor(self);
	if (!a)
		return nullptr;
	span<string> tags = a->getTags();
	PyObject* list = PyList_New(0);
	for (size_t i = 0; i < tags.size; i++) {
		PyList_Append(list, ToPyStr(tags.data[i]));
	}
	return list;
}

//杀死实体
PyObject* PyEntity_Kill(PyObject* self, PyObject*) {
	Actor* a = PyEntity::asActor(self);
	if (!a)
		return nullptr;
	a->kill();
	Py_RETURN_NONE;
}

//获取属性方法
PyGetSetDef PyEntity_GetSet[]{
	{ "name", PyEntity::getName, PyEntity::setName, nullptr },
	{ "uuid", PyEntity::getUuid, nullptr, nullptr },
	{ "xuid", PyEntity::getXuid, nullptr, nullptr },
	{ "pos", PyEntity::getPos, nullptr, nullptr },
	{ "did", PyEntity::getDimensionId, nullptr, nullptr },
	{ "is_standing", PyEntity::getIsStand, nullptr, nullptr },
	/*已弃用*/{ "isstand", PyEntity::getIsStand, nullptr, nullptr },
	{ "is_sneaking", PyEntity::getIsSneaking, nullptr, nullptr },
	/*已弃用*/{ "issneak", PyEntity::getIsSneaking, nullptr, nullptr },
	{ "typeid", PyEntity::getTypeID, nullptr, nullptr },
	{ "typename", PyEntity::getTypeName, nullptr, nullptr },
	{ "NBT", PyEntity::getNBTInfo, nullptr, nullptr },
	/*已弃用*/{ "nbt", PyEntity::getNBTInfo, nullptr, nullptr },
	{ "health", PyEntity::getHealth, PyEntity::setHealth, nullptr },
	{ "maxhealth", PyEntity::getMaxHealth, PyEntity::setMaxHealth, nullptr },
	{ "perm", PyEntity::getPermissions, PyEntity::setPermissions, nullptr },
	{ "platform_online_id", PyEntity::getPlatformOnlineId, nullptr, nullptr },
	/*已弃用*/{ "deviceid", PyEntity::getPlatformOnlineId, nullptr, nullptr },
	{ "platform", PyEntity::getPlatform, nullptr, nullptr },
	/*已弃用*/{ "deviceos", PyEntity::getPlatform, nullptr, nullptr },
	{ "IP", PyEntity::getIP, nullptr, nullptr },
	/*已弃用*/{ "ip", PyEntity::getIP, nullptr, nullptr },
	{ nullptr }
};
//Entity方法
PyMethodDef PyEntity_Methods[]{
	//{"getItem", (PyCFunction)PyEntity_GetItem, METH_VARARGS | METH_KEYWORDS, nullptr},
	{ "getAllItem", PyEntity_GetAllItem, METH_VARARGS, nullptr },
	{ "setAllItem", PyEntity_SetAllItem, METH_VARARGS, nullptr },
	{ "setHand", PyEntity_SetHand, METH_VARARGS, nullptr },
	{ "addItem", PyEntity_AddItem, METH_VARARGS, nullptr },
	{ "removeItem", PyEntity_RemoveItem, METH_VARARGS, nullptr },
	{ "teleport", PyEntity_Teleport, METH_VARARGS, nullptr },
	{ "sendTextPacket", PyEntity_SendTextPacket, METH_VARARGS, nullptr },
	{ "sendCommandPacket", PyEntity_SendCommandPacket, METH_VARARGS, nullptr },
	{ "resendAllChunks", PyEntity_ResendAllChunks, METH_NOARGS, nullptr },
	{ "disconnect", PyEntity_Disconnect, METH_VARARGS, nullptr },
	{ "getScore", PyEntity_GetScore, METH_VARARGS, nullptr },
	{ "modifyScore", PyEntity_ModifyScore, METH_VARARGS, nullptr },
	{ "addLevel", PyEntity_AddLevel, METH_VARARGS, nullptr },
	{ "transferServer", PyEntity_TransferServer, METH_VARARGS, nullptr },
	{ "sendCustomForm", PyEntity_SendCustomForm, METH_VARARGS, nullptr },
	{ "sendSimpleForm", PyEntity_SendSimpleForm, METH_VARARGS, nullptr },
	{ "sendModalForm", PyEntity_SendModalForm, METH_VARARGS, nullptr },
	{ "setSidebar", PyEntity_SetSidebar, METH_VARARGS, nullptr },
	{ "removeSidebar", PyEntity_RemoveSidebar, METH_NOARGS, nullptr },
	{ "setBossbar", PyEntity_SetBossbar, METH_VARARGS, nullptr },
	{ "removeBossbar", PyEntity_RemoveBossbar, METH_NOARGS, nullptr },
	{ "addTag", PyEntity_AddTag, METH_VARARGS, nullptr },
	{ "removeTag", PyEntity_RemoveTag, METH_VARARGS, nullptr },
	{ "getTags", PyEntity_GetTags, METH_NOARGS, nullptr },
	{ "kill", PyEntity_Kill, METH_NOARGS, nullptr },
	{ nullptr }
};
//Entity类型
PyTypeObject PyEntity_Type{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Entity",				/* tp_name */
	sizeof(PyEntity),		/* tp_basicsize */
	0,						/* tp_itemsize */
	nullptr,				/* tp_dealloc */
	PyEntity::print,		/* tp_print */
	nullptr,				/* tp_getattr */
	nullptr,				/* tp_setattr */
	nullptr,				/* tp_reserved */
	PyEntity::repr,			/* tp_repr */
	nullptr,				/* tp_as_number */
	nullptr,				/* tp_as_sequence */
	nullptr,				/* tp_as_mapping */
	PyEntity::hash,			/* tp_hash */
	nullptr,				/* tp_call */
	PyEntity::str,			/* tp_str */
	nullptr,				/* tp_getattro */
	nullptr,				/* tp_setattro */
	nullptr,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	"Entities in Minecraft",/* tp_doc */
	nullptr,				/* tp_traverse */
	nullptr,				/* tp_clear */
	PyEntity::rich_compare,	/* tp_richcompare */
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

PyObject* ToEntity(Actor* ptr) {
	PyEntity* obj = nullptr;
	//Py_BEGIN_CALL;
	obj = PyObject_New(PyEntity, &PyEntity_Type);
	//Py_END_CALL;
	obj->actor = ptr;
	return reinterpret_cast<PyObject*>(obj);
}
