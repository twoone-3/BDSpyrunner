#include "Entity.h"
#include "Tool.h"
#include "NBT.h"

#define Py_GET_PLAYER Player* p = asPlayer(self);if (p == nullptr)return nullptr
#define Py_GET_ACTOR Actor* a = asActor(self);if (a == nullptr)return nullptr

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
		if (a == nullptr)
			return -1;
		fputs(a->getNameTag().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		Actor* a = asActor(self);
		if (a == nullptr)
			return ToPyStr("");
		return ToPyStr(a->getNameTag());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		Actor* a = asActor(self);
		if (a == nullptr)
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
		Py_GET_ACTOR;
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
		Py_GET_PLAYER;
		return ToPyStr(p->getUuid());
	}
	//获取XUID
	static PyObject* getXuid(PyObject* self, void*) {
		Py_GET_PLAYER;
		return ToPyStr(p->getXuid());
	}
	//获取坐标
	static PyObject* getPos(PyObject* self, void*) {
		Py_GET_ACTOR;
		return ToList(a->getPos());
	}
	//获取维度ID
	static PyObject* getDimensionId(PyObject* self, void*) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getDimensionId());
	}
	//是否着地
	static PyObject* getIsStand(PyObject* self, void*) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isStanding());
	}
	//是否潜行
	static PyObject* getIsSneaking(PyObject* self, void*) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isSneaking());
	}
	//获取类型
	static PyObject* getTypeID(PyObject* self, void*) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getEntityTypeId());
	}
	//获取类型字符串
	static PyObject* getTypeName(PyObject* self, void*) {
		Py_GET_ACTOR;
		string type;
		SymCall<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
			&type, a->getEntityTypeId());
		return ToPyStr(type);
	}
	//获取nbt数据
	static PyObject* getNBTInfo(PyObject* self, void*) {
		Py_GET_ACTOR;
		unique_ptr<CompoundTag> t = CompoundTag::create();
		a->save(*t);
		return ToPyStr(ToJson(move(t)));
	}
	//获取生命值
	static PyObject* getHealth(PyObject* self, void*) {
		Py_GET_ACTOR;
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
		Py_GET_ACTOR;
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
		Py_GET_PLAYER;
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
		Py_GET_PLAYER;
		return ToPyStr(p->getPlatformOnlineId());
	}
	//获取设备类型
	static PyObject* getPlatform(PyObject* self, void*) {
		Py_GET_PLAYER;
		return PyLong_FromLong(p->getPlatform());
	}
	//获取IP
	static PyObject* getIP(PyObject* self, void*) {
		Py_GET_PLAYER;
		auto& ni = *p->getNetworkIdentifier();
		return ToPyStr(Global<RakNet::RakPeer>->getAdr(ni).ToString(false, ':'));
	}

	//获取玩家所有物品
	static PyObject* getAllItem(PyObject* self, PyObject*) {
		Py_GET_PLAYER;
		fifo_json value;

		fifo_json& inventory = value["Inventory"];
		for (auto& i : p->getInventory().getSlots()) {
			inventory.push_back(ToJson(i->save()));
		}

		fifo_json& endchest = value["EndChest"];
		for (auto& i : p->getEnderChestContainer()->getSlots()) {
			endchest.push_back(ToJson(i->save()));
		}

		fifo_json& armor = value["Armor"];
		for (auto& i : p->getArmorContainer().getSlots()) {
			armor.push_back(ToJson(i->save()));
		}

		value["OffHand"] = ToJson(p->getOffhandSlot().save());
		value["Hand"] = ToJson(p->getSelectedItem().save());

		return ToPyStr(value.dump(4));
	}
	//设置玩家所有物品
	static PyObject* setAllItem(PyObject* self, PyObject* args) {
		const char* x = "";
		Py_PARSE("s", &x);
		Py_GET_PLAYER;
		fifo_json value(StringToJson(x));

		if (value.contains("Inventory")) {
			auto& items = p->getInventory();
			fifo_json& inventory = value["Inventory"];
			for (unsigned i = 0; i < inventory.size(); i++) {
				*items.getSlot(i) = LoadItemFromJson(inventory[i]);
			}
		}

		if (value.contains("EndChest")) {
			auto items = p->getEnderChestContainer();
			fifo_json& endchest = value["EndChest"];
			for (unsigned i = 0; i < endchest.size(); i++) {
				*items->getSlot(i) = LoadItemFromJson(endchest[i]);
			}
		}

		if (value.contains("Armor")) {
			auto& items = p->getArmorContainer();
			fifo_json& armor = value["Armor"];
			for (unsigned i = 0; i < armor.size(); i++) {
				*items.getSlot(i) = LoadItemFromJson(armor[i]);
			}
		}

		if (value.contains("OffHand")) {
			p->setOffhandSlot(LoadItemFromJson(value["OffHand"]));
		}
		p->sendInventory(true);

		Py_RETURN_NONE;
	}
	//设置玩家手上物品
	static PyObject* setHand(PyObject* self, PyObject* args) {
		const char* x = "";
		Py_PARSE("s", &x);
		Py_GET_PLAYER;
		const_cast<ItemStack&>(p->getSelectedItem()) = LoadItemFromString(x);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//增加玩家背包物品
	static PyObject* addItem(PyObject* self, PyObject* args) {
		const char* x = "";
		Py_PARSE("s", &x);
		Py_GET_PLAYER;
		auto item = LoadItemFromString(x);
		p->giveItem(&item);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//移除玩家背包物品
	static PyObject* removeItem(PyObject* self, PyObject* args) {
		int slot, num;
		Py_PARSE("ii", &slot, &num);
		Py_GET_PLAYER;
		p->getInventory().removeItem(slot, num);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//传送
	static PyObject* teleport(PyObject* self, PyObject* args) {
		Vec3 pos; int did;
		Py_PARSE("fffi", &pos.x, &pos.y, &pos.z, &did);
		Py_GET_PLAYER;
		p->teleport(pos, did);
		Py_RETURN_NONE;
	}
	//发送数据包
	static PyObject* sendTextPacket(PyObject* self, PyObject* args) {
		const char* msg = "";
		int mode = 0;
		Py_PARSE("s|i", &msg, &mode);
		Py_GET_PLAYER;
		p->sendTextPacket(msg, TextType(mode));
		Py_RETURN_NONE;
	}
	static PyObject* sendCommandPacket(PyObject* self, PyObject* args) {
		const char* cmd = "";
		Py_PARSE("s", &cmd);
		Py_GET_PLAYER;
		p->sendCommandRequestPacket(cmd);
		Py_RETURN_NONE;
	}
	//重新发送所有区块
	static PyObject* resendAllChunks(PyObject* self, PyObject*) {
		Py_GET_PLAYER;
		p->resendAllChunks();
		Py_RETURN_NONE;
	}
	//断开连接
	static PyObject* disconnect(PyObject* self, PyObject* args) {
		const char* msg = "";
		Py_PARSE("|s", &msg);
		Py_GET_PLAYER;
		p->kick(msg);
		Py_RETURN_NONE;
	}
	//计分板操作
	static PyObject* getScore(PyObject* self, PyObject* args) {
		const char* objname = "";
		Py_PARSE("s", &objname);
		Py_GET_PLAYER;
		return PyLong_FromLong(p->getScore(objname));
	}
	//todo add reduce set
	static PyObject* modifyScore(PyObject* self, PyObject* args) {
		const char* objname = "";
		int count; PlayerScoreSetFunction mode;
		Py_PARSE("sii", &objname, &count, &mode);
		Py_GET_PLAYER;
		Py_RETURN_NONE;
	}
	//增加等级
	static PyObject* addLevel(PyObject* self, PyObject* args) {
		int level;
		Py_PARSE("i", &level);
		Py_GET_PLAYER;
		p->addLevels(level);
		Py_RETURN_NONE;
	}
	//跨服传送
	static PyObject* transferServer(PyObject* self, PyObject* args) {
		const char* address = "";
		unsigned short port;
		Py_PARSE("sH", &address, &port);
		Py_GET_PLAYER;
		p->sendTransferPacket(address, port);
		Py_RETURN_NONE;
	}
	//发送表单
	static PyObject* sendCustomForm(PyObject* self, PyObject* args) {
		const char* str = "";
		PyObject* callback = nullptr;
		Py_PARSE("sO", &str, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		p->sendCustomFormPacket(str,
			[callback](string arg) {
				PyObject* result = PyObject_CallFunction(callback, "s", arg.c_str());
				PrintPythonError();
				Py_XDECREF(result);
			}
		);
		Py_RETURN_NONE;
	}
	static PyObject* sendSimpleForm(PyObject* self, PyObject* args) {
		const char* title = "";
		const char* content = "";
		PyObject* buttons = nullptr;
		PyObject* images = nullptr;
		PyObject* callback = nullptr;
		Py_PARSE("ssOOO", &title, &content, &buttons, &images, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		p->sendSimpleFormPacket(title, content, ToStrArray(buttons), ToStrArray(images),
			[callback](int arg) {
				PyObject* result = PyObject_CallFunction(callback, "i", arg);
				PrintPythonError();
				Py_XDECREF(result);
			}
		);
		Py_RETURN_NONE;
	}
	static PyObject* sendModalForm(PyObject* self, PyObject* args) {
		const char* title = "";
		const char* content = "";
		const char* button1 = "";
		const char* button2 = "";
		PyObject* callback = nullptr;
		Py_PARSE("ssssO", &title, &content, &button1, &button2, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		p->sendModalFormPacket(title, content, button1, button2,
			[callback](bool arg) {
				PyObject* result = PyObject_CallFunction(callback, "O", arg ? Py_True : Py_False);
				PrintPythonError();
				Py_XDECREF(result);
			}
		);
		Py_RETURN_NONE;
	}
	//设置侧边栏
	static PyObject* setSidebar(PyObject* self, PyObject* args) {
		const char* title = "";
		const char* side_data = "";
		ObjectiveSortOrder order = ObjectiveSortOrder::Ascending;
		Py_PARSE("ss|i", &title, &side_data, &order);
		Py_GET_PLAYER;
		vector<pair<string, int>> data;
		fifo_json value = StringToJson(side_data);
		if (value.is_object())
			for (auto& [key, val] : value.items()) {
				data.push_back({ key, val });
			}
		p->setSidebar(title, data, order);
		Py_RETURN_NONE;
	}
	static PyObject* removeSidebar(PyObject* self, PyObject*) {
		Py_GET_PLAYER;
		p->removeSidebar();
		Py_RETURN_NONE;
	}
	//Boss栏
	static PyObject* setBossbar(PyObject* self, PyObject* args) {
		const char* name = "";
		float per;
		Py_PARSE("sf", &name, &per);
		Py_GET_PLAYER;
		p->sendBossEventPacket(BossEvent::Show, name, per, BossEventColour::Red); // Todo
		Py_RETURN_NONE;
	}
	static PyObject* removeBossbar(PyObject* self, PyObject* args) {
		const char* name = "";
		Py_PARSE("s:removeBossbar", &name);
		Py_GET_PLAYER;
		p->sendBossEventPacket(BossEvent::Hide, name, 0, BossEventColour::Red); // Todo
		Py_RETURN_NONE;
	}
	//标签
	static PyObject* addTag(PyObject* self, PyObject* args) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		a->addTag(tag);
		Py_RETURN_NONE;
	}
	static PyObject* removeTag(PyObject* self, PyObject* args) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		Actor* a = PyEntity::asActor(self);
		if (!a)
			return nullptr;
		a->removeTag(tag);
		Py_RETURN_NONE;
	}
	static PyObject* getTags(PyObject* self, PyObject*) {
		Py_GET_ACTOR;
		auto tags = a->getTags();
		PyObject* list = PyList_New(0);
		for (size_t i = 0; i < tags.size(); i++) {
			PyList_Append(list, ToPyStr(tags[i]));
		}
		return list;
	}
	//杀死实体
	static PyObject* kill(PyObject* self, PyObject*) {
		Py_GET_ACTOR;
		a->kill();
		Py_RETURN_NONE;
	}

	//获取属性方法
	inline static PyGetSetDef GetSet[]{
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
	inline static PyMethodDef Methods[]{
		//{"getItem", (PyCFunction)GetItem, METH_VARARGS | METH_KEYWORDS, nullptr},
		{ "getAllItem", getAllItem, METH_VARARGS, nullptr },
		{ "setAllItem", setAllItem, METH_VARARGS, nullptr },
		{ "setHand", setHand, METH_VARARGS, nullptr },
		{ "addItem", addItem, METH_VARARGS, nullptr },
		{ "removeItem", removeItem, METH_VARARGS, nullptr },
		{ "teleport", teleport, METH_VARARGS, nullptr },
		{ "sendTextPacket", sendTextPacket, METH_VARARGS, nullptr },
		{ "sendCommandPacket", sendCommandPacket, METH_VARARGS, nullptr },
		{ "resendAllChunks", resendAllChunks, METH_NOARGS, nullptr },
		{ "disconnect", disconnect, METH_VARARGS, nullptr },
		{ "getScore", getScore, METH_VARARGS, nullptr },
		{ "modifyScore", modifyScore, METH_VARARGS, nullptr },
		{ "addLevel", addLevel, METH_VARARGS, nullptr },
		{ "transferServer", transferServer, METH_VARARGS, nullptr },
		{ "sendCustomForm", sendCustomForm, METH_VARARGS, nullptr },
		{ "sendSimpleForm", sendSimpleForm, METH_VARARGS, nullptr },
		{ "sendModalForm", sendModalForm, METH_VARARGS, nullptr },
		{ "setSidebar", setSidebar, METH_VARARGS, nullptr },
		{ "removeSidebar", removeSidebar, METH_NOARGS, nullptr },
		{ "setBossbar", setBossbar, METH_VARARGS, nullptr },
		{ "removeBossbar", removeBossbar, METH_NOARGS, nullptr },
		{ "addTag", addTag, METH_VARARGS, nullptr },
		{ "removeTag", removeTag, METH_VARARGS, nullptr },
		{ "getTags", getTags, METH_NOARGS, nullptr },
		{ "kill", kill, METH_NOARGS, nullptr },
		{ nullptr }
	};
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
	PyEntity::Methods,		/* tp_methods */
	nullptr,				/* tp_members */
	PyEntity::GetSet,		/* tp_getset */
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
