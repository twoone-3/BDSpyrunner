#include "PyUtils.h"
#include "NBT.h"

#define Py_GET_PLAYER  Py_GET_PLAYER2(nullptr)
#define Py_GET_PLAYER2(ret) auto p = reinterpret_cast<Player*>(reinterpret_cast<PyEntity*>(self)->value);if (p == nullptr)return ret
#define Py_GET_ACTOR Py_GET_ACTOR2(nullptr)
#define Py_GET_ACTOR2(ret) auto a = reinterpret_cast<PyEntity*>(self)->value;if (a == nullptr)return ret

using namespace std;

struct PyEntity {
	PyObject_HEAD;
	Actor* value;

	static int print(PyObject* self, FILE* file, int) {
		Py_GET_ACTOR2(-1);
		fputs(a->getNameTag().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		Py_GET_ACTOR2(ToPyObject(""));
		return ToPyObject(a->getNameTag());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		Py_GET_ACTOR2(ToPyObject(""));
		return ToPyObject(a->getNameTag());
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
		case Py_LT:// <
			break;
		case Py_LE:// <=
			break;
		case Py_EQ:// ==
			return ToPyObject(reinterpret_cast<PyEntity*>(self)->value == reinterpret_cast<PyEntity*>(other)->value);
		case Py_NE:// !=
			return ToPyObject(reinterpret_cast<PyEntity*>(self)->value != reinterpret_cast<PyEntity*>(other)->value);
		case Py_GT:// >
			break;
		case Py_GE:// >=
			break;
		}
		Py_RETURN_NOTIMPLEMENTED;
	}

	//获取名字
	Py_METHOD_DEFINE(getName) {
		Py_GET_ACTOR2(ToPyObject(""));
		return ToPyObject(a->getNameTag());
	}
	Py_METHOD_DEFINE(setName) {
		const char* name = "";
		Py_PARSE("s", &name);
		Py_GET_PLAYER;
		p->setName(name);
		Py_RETURN_NONE;
	}
	//获取UUID
	Py_METHOD_DEFINE(getUuid) {
		Py_GET_PLAYER;
		return ToPyObject(p->getUuid());
	}
	//获取XUID
	Py_METHOD_DEFINE(getXuid) {
		Py_GET_PLAYER;
		return ToPyObject(p->getXuid());
	}
	//获取坐标
	Py_METHOD_DEFINE(getPos) {
		Py_GET_ACTOR;
		return ToPyObject(a->getPos());
	}
	//获取维度ID
	Py_METHOD_DEFINE(getDimensionId) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getDimensionId());
	}
	//是否着地
	Py_METHOD_DEFINE(isStanding) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isStanding());
	}
	//是否潜行
	Py_METHOD_DEFINE(isSneaking) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isSneaking());
	}
	//是否坐下
	Py_METHOD_DEFINE(isSitting) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isSitting());
	}
	//获取类型
	Py_METHOD_DEFINE(getTypeID) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getEntityTypeId());
	}
	//获取类型字符串
	Py_METHOD_DEFINE(getTypeName) {
		Py_GET_ACTOR;
		//旧办法
		//string type;
		//SymCall<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
		//	&type, a->getEntityTypeId());
		return ToPyObject(a->getTypeName());
	}
	//获取nbt数据
	Py_METHOD_DEFINE(getNBT) {
		Py_GET_ACTOR;
		auto t = CompoundTag::create();
		a->save(*t);
		return ToPyObject(CompoundTagToJson(*t).dump(4));
	}
	//获取生命值
	Py_METHOD_DEFINE(getHealth) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getHealth());
	}
	//获取最大生命值
	Py_METHOD_DEFINE(getMaxHealth) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getMaxHealth());
	}
	//获取权限
	Py_METHOD_DEFINE(getPermissions) {
		Py_GET_PLAYER;
		return PyLong_FromLong(static_cast<int>(p->getPlayerPermissionLevel()));
	}
	Py_METHOD_DEFINE(setPermissions) {
		int perm;
		Py_PARSE("i", &perm);
		Py_GET_PLAYER;
		p->setPermissions(static_cast<CommandPermissionLevel>(perm));
		Py_RETURN_NONE;
	}
	//获取设备id
	Py_METHOD_DEFINE(getPlatformOnlineId) {
		Py_GET_PLAYER;
		return ToPyObject(p->getPlatformOnlineId());
	}
	//获取设备类型
	Py_METHOD_DEFINE(getPlatform) {
		Py_GET_PLAYER;
		return PyLong_FromLong(p->getPlatform());
	}
	//获取IP
	Py_METHOD_DEFINE(getIP) {
		Py_GET_PLAYER;
		auto ni = p->getNetworkIdentifier();
		return ToPyObject(Global<RakNet::RakPeer>->getAdr(*ni).ToString(false, ':'));
	}
	//获取玩家所有物品
	Py_METHOD_DEFINE(getAllItem) {
		Py_GET_PLAYER;
		fifo_json items_json = fifo_json::object();
		fifo_json& inventory = items_json["Inventory"];
		for (auto& i : p->getInventory().getSlots()) {
			inventory.push_back(CompoundTagToJson(*i->save()));
		}
		fifo_json& endchest = items_json["EndChest"];
		for (auto& i : p->getEnderChestContainer()->getSlots()) {
			endchest.push_back(CompoundTagToJson(*i->save()));
		}
		fifo_json& armor = items_json["Armor"];
		for (auto& i : p->getArmorContainer().getSlots()) {
			armor.push_back(CompoundTagToJson(*i->save()));
		}
		items_json["OffHand"] = CompoundTagToJson(*p->getOffhandSlot().save());
		items_json["Hand"] = CompoundTagToJson(*p->getSelectedItem().save());
		return ToPyObject(items_json.dump(4));
	}
	//设置玩家所有物品
	Py_METHOD_DEFINE(setAllItem) {
		const char* items_data = "";
		Py_PARSE("s", &items_data);
		Py_GET_PLAYER;
		fifo_json items_json(StrToJson(items_data));
		if (items_json.contains("Inventory")) {
			auto& items = p->getInventory();
			fifo_json& inventory = items_json["Inventory"];
			for (unsigned i = 0; i < inventory.size(); i++) {
				*items.getSlot(i) = LoadItemFromJson(inventory[i]);
			}
		}
		if (items_json.contains("EndChest")) {
			auto items = p->getEnderChestContainer();
			fifo_json& endchest = items_json["EndChest"];
			for (unsigned i = 0; i < endchest.size(); i++) {
				*items->getSlot(i) = LoadItemFromJson(endchest[i]);
			}
		}
		if (items_json.contains("Armor")) {
			auto& items = p->getArmorContainer();
			fifo_json& armor = items_json["Armor"];
			for (unsigned i = 0; i < armor.size(); i++) {
				*items.getSlot(i) = LoadItemFromJson(armor[i]);
			}
		}
		if (items_json.contains("OffHand")) {
			p->setOffhandSlot(LoadItemFromJson(items_json["OffHand"]));
		}
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//设置玩家手上物品
	Py_METHOD_DEFINE(setHand) {
		const char* item_data = "";
		Py_PARSE("s", &item_data);
		Py_GET_PLAYER;
		const_cast<ItemStack&>(p->getSelectedItem()) = LoadItemFromString(item_data);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//增加玩家背包物品
	Py_METHOD_DEFINE(addItem) {
		const char* item_data = "";
		Py_PARSE("s", &item_data);
		Py_GET_PLAYER;
		auto item = LoadItemFromString(item_data);
		p->giveItem(&item);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//移除玩家背包物品
	Py_METHOD_DEFINE(removeItem) {
		int slot, num;
		Py_PARSE("ii", &slot, &num);
		Py_GET_PLAYER;
		p->getInventory().removeItem(slot, num);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//传送
	Py_METHOD_DEFINE(teleport) {
		Vec3 pos;
		int did;
		Py_PARSE("fffi", &pos.x, &pos.y, &pos.z, &did);
		Py_GET_PLAYER;
		p->teleport(pos, did);
		Py_RETURN_NONE;
	}
	//发送文本
	Py_METHOD_DEFINE(sendText) {
		const char* text = "";
		TextType type = TextType::RAW;
		Py_PARSE("s|i", &text, &type);
		Py_GET_PLAYER;
		p->sendTextPacket(text, type);
		Py_RETURN_NONE;
	}
	//模拟玩家执行命令
	Py_METHOD_DEFINE(runCommandAs) {
		const char* cmd = "";
		Py_PARSE("s", &cmd);
		Py_GET_PLAYER;
		p->sendCommandRequestPacket(cmd);
		Py_RETURN_NONE;
	}
	//重新发送所有区块
	Py_METHOD_DEFINE(resendAllChunks) {
		Py_GET_PLAYER;
		p->resendAllChunks();
		Py_RETURN_NONE;
	}
	//断开连接
	Py_METHOD_DEFINE(disconnect) {
		const char* msg = "";
		Py_PARSE("|s", &msg);
		Py_GET_PLAYER;
		p->kick(msg);
		Py_RETURN_NONE;
	}
	//获取玩家分数
	Py_METHOD_DEFINE(getScore) {
		const char* objname = "";
		Py_PARSE("s", &objname);
		Py_GET_PLAYER;
		return PyLong_FromLong(p->getScore(objname));
	}
	//设置玩家分数
	Py_METHOD_DEFINE(setScore) {
		const char* objname = "";
		int count;
		Py_PARSE("si", &objname, &count);
		Py_GET_PLAYER;
		p->setScore(objname, count);
		Py_RETURN_NONE;
	}
	//增加玩家分数
	Py_METHOD_DEFINE(addScore) {
		const char* objname = "";
		int count;
		Py_PARSE("si", &objname, &count);
		Py_GET_PLAYER;
		p->addScore(objname, count);
		Py_RETURN_NONE;
	}
	//减少玩家分数
	Py_METHOD_DEFINE(reduceScore) {
		const char* objname = "";
		int count;
		Py_PARSE("si", &objname, &count);
		Py_GET_PLAYER;
		p->reduceScore(objname, count);
		Py_RETURN_NONE;
	}
	//获取等级
	Py_METHOD_DEFINE(getLevel) {
		Py_GET_PLAYER;
		//p->getex();
		//TODO: get levels from player
		Py_RETURN_NONE;
	}
	//增加等级
	Py_METHOD_DEFINE(addLevel) {
		int level;
		Py_PARSE("i", &level);
		Py_GET_PLAYER;
		p->addLevels(level);
		Py_RETURN_NONE;
	}
	//跨服传送
	Py_METHOD_DEFINE(transferServer) {
		const char* address = "";
		unsigned short port;
		Py_PARSE("sH", &address, &port);
		Py_GET_PLAYER;
		p->sendTransferPacket(address, port);
		Py_RETURN_NONE;
	}
	//发送表单
	Py_METHOD_DEFINE(sendCustomForm) {
		const char* str = "";
		PyObject* callback = nullptr;
		Py_PARSE("sO", &str, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		p->sendCustomFormPacket(str,
			[p, callback](string arg) {
				PyCaller pc;
				pc.call(callback, p, arg);
			}
		);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(sendSimpleForm) {
		const char* title = "";
		const char* content = "";
		PyObject* buttons_list = nullptr;
		PyObject* images_list = nullptr;
		PyObject* callback = nullptr;
		Py_PARSE("ssOOO", &title, &content, &buttons_list, &images_list, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		auto buttons = ListToStrArray(buttons_list);
		auto images = ListToStrArray(images_list);
		if (buttons.size() != images.size())
			Py_RETURN_ERROR("The number of buttons is not equal to the number of images");
		p->sendSimpleFormPacket(title, content, buttons, images,
			[p, callback](int arg) {
				PyCaller pc;
				pc.call(callback, p, arg);
		}
		);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(sendModalForm) {
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
			[p, callback](bool arg) {
				PyCaller pc;
				pc.call(callback, p, arg);
			}
		);
		Py_RETURN_NONE;
	}
	//设置侧边栏
	Py_METHOD_DEFINE(setSidebar) {
		const char* title = "";
		const char* side_data = "";
		ObjectiveSortOrder order = ObjectiveSortOrder::Ascending;
		Py_PARSE("ss|i", &title, &side_data, &order);
		Py_GET_PLAYER;
		vector<pair<string, int>> data;
		fifo_json value = StrToJson(side_data);
		if (value.is_object())
			for (auto& [key, val] : value.items()) {
				data.push_back({ key, val });
			}
		p->setSidebar(title, data, order);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeSidebar) {
		Py_GET_PLAYER;
		p->removeSidebar();
		Py_RETURN_NONE;
	}
	//Boss栏
	Py_METHOD_DEFINE(setBossbar) {
		const char* name = "";
		float per;
		Py_PARSE("sf", &name, &per);
		Py_GET_PLAYER;
		p->sendBossEventPacket(BossEvent::Show, name, per, BossEventColour::Red);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeBossbar) {
		const char* name = "";
		Py_PARSE("s:removeBossbar", &name);
		Py_GET_PLAYER;
		p->sendBossEventPacket(BossEvent::Hide, name, 0, BossEventColour::Red);
		Py_RETURN_NONE;
	}
	//标签
	Py_METHOD_DEFINE(addTag) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		Py_GET_ACTOR;
		a->addTag(tag);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeTag) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		Py_GET_ACTOR;
		a->removeTag(tag);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(getTags) {
		Py_GET_ACTOR;
		auto tags = a->getTags();
		PyObject* list = PyList_New(0);
		for (size_t i = 0; i < tags.size(); i++) {
			PyList_Append(list, ToPyObject(tags[i]));
		}
		return list;
	}
	//杀死实体
	Py_METHOD_DEFINE(kill) {
		Py_GET_ACTOR;
		a->kill();
		Py_RETURN_NONE;
	}
	//获取游戏模式
	Py_METHOD_DEFINE(getGameMode) {
		Py_GET_PLAYER;
		return PyLong_FromLong(p->getPlayerGameType());
	}

	inline static PyMethodDef Methods[]{
		Py_METHOD_NOARGS(getName),
		Py_METHOD_VARARGS(setName),
		Py_METHOD_NOARGS(getUuid),
		Py_METHOD_NOARGS(getXuid),
		Py_METHOD_NOARGS(getPos),
		Py_METHOD_NOARGS(getDimensionId),
		Py_METHOD_NOARGS(isStanding),
		Py_METHOD_NOARGS(isSneaking),
		Py_METHOD_NOARGS(getTypeID),
		Py_METHOD_NOARGS(getTypeName),
		Py_METHOD_NOARGS(getNBT),
		Py_METHOD_NOARGS(getHealth),
		Py_METHOD_NOARGS(getMaxHealth),
		Py_METHOD_NOARGS(getPermissions),
		Py_METHOD_VARARGS(setPermissions),
		Py_METHOD_NOARGS(getPlatformOnlineId),
		Py_METHOD_NOARGS(getPlatform),
		Py_METHOD_NOARGS(getIP),
		Py_METHOD_NOARGS(getAllItem),
		Py_METHOD_VARARGS(setAllItem),
		Py_METHOD_VARARGS(setHand),
		Py_METHOD_VARARGS(addItem),
		Py_METHOD_VARARGS(removeItem),
		Py_METHOD_VARARGS(teleport),
		Py_METHOD_VARARGS(sendText),
		Py_METHOD_VARARGS(runCommandAs),
		Py_METHOD_NOARGS(resendAllChunks),
		Py_METHOD_VARARGS(disconnect),
		Py_METHOD_VARARGS(getScore),
		Py_METHOD_VARARGS(setScore),
		Py_METHOD_VARARGS(addScore),
		Py_METHOD_VARARGS(reduceScore),
		Py_METHOD_NOARGS(getLevel),
		Py_METHOD_VARARGS(addLevel),
		Py_METHOD_VARARGS(transferServer),
		Py_METHOD_VARARGS(sendCustomForm),
		Py_METHOD_VARARGS(sendSimpleForm),
		Py_METHOD_VARARGS(sendModalForm),
		Py_METHOD_VARARGS(setSidebar),
		Py_METHOD_NOARGS(removeSidebar),
		Py_METHOD_VARARGS(setBossbar),
		Py_METHOD_VARARGS(removeBossbar),
		Py_METHOD_VARARGS(addTag),
		Py_METHOD_VARARGS(removeTag),
		Py_METHOD_NOARGS(getTags),
		Py_METHOD_NOARGS(kill),
		Py_METHOD_NOARGS(getGameMode),
		Py_METHOD_END
	};
};

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

PyObject* ToPyObject(Actor* ptr) {
	PyEntity* obj = PyObject_New(PyEntity, &PyEntity_Type);
	obj->value = ptr;
	return reinterpret_cast<PyObject*>(obj);
}
