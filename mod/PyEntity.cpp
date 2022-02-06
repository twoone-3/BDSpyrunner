#include "PyUtils.h"
#include "Main.h"
#include "magic_enum.hpp"

#define PyEntity_Check(self) (Py_TYPE(self) == &PyEntity_Type)
#define PyEntity_RAW(self) (reinterpret_cast<PyEntity*>(self)->value)
#define PyEntity_VALUE auto thiz = PyEntity_RAW(self); if (thiz == nullptr)return nullptr
#define PyEntity_VALUE2 auto thiz = reinterpret_cast<Player*>(PyEntity_RAW(self)); if (thiz == nullptr)return nullptr

using namespace std;

struct PyEntity {
	PyObject_HEAD;
	Actor* value;

	static int print(PyObject* self, FILE* file, int) {
		PyEntity_VALUE, -1;
		fputs(thiz->getNameTag().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		PyEntity_VALUE, ToPyObject("");
		return ToPyObject(thiz->getNameTag());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		PyEntity_VALUE, ToPyObject("");
		return ToPyObject(thiz->getNameTag());
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
		PyEntity_VALUE, ToPyObject("");
		return ToPyObject(thiz->getNameTag());
	}
	//设置名字
	Py_METHOD_DEFINE(setName) {
		const char* name = "";
		Py_PARSE("s", &name);
		PyEntity_VALUE2;
		thiz->setName(name);
		Py_RETURN_NONE;
	}
	//获取UUID
	Py_METHOD_DEFINE(getUuid) {
		PyEntity_VALUE2;
		return ToPyObject(thiz->getUuid());
	}
	//获取XUID
	Py_METHOD_DEFINE(getXuid) {
		PyEntity_VALUE2;
		return ToPyObject(thiz->getXuid());
	}
	//获取坐标
	Py_METHOD_DEFINE(getPos) {
		PyEntity_VALUE;
		return ToPyObject(thiz->getPos());
	}
	//获取维度ID
	Py_METHOD_DEFINE(getDimensionId) {
		PyEntity_VALUE;
		return ToPyObject(thiz->getDimensionId());
	}
	//是否着地
	Py_METHOD_DEFINE(isStanding) {
		PyEntity_VALUE;
		return ToPyObject(thiz->isStanding());
	}
	//是否潜行
	Py_METHOD_DEFINE(isSneaking) {
		PyEntity_VALUE;
		return ToPyObject(thiz->isSneaking());
	}
	//是否坐下
	Py_METHOD_DEFINE(isSitting) {
		PyEntity_VALUE;
		return ToPyObject(thiz->isSitting());
	}
	//获取类型
	Py_METHOD_DEFINE(getTypeId) {
		PyEntity_VALUE;
		return ToPyObject(thiz->getEntityTypeId());
	}
	//获取类型字符串
	Py_METHOD_DEFINE(getTypeName) {
		PyEntity_VALUE;
		return ToPyObject(thiz->getTypeName());
	}
	//获取nbt数据
	Py_METHOD_DEFINE(getNBT) {
		PyEntity_VALUE;
		return ToPyObject(CompoundTag::fromActor(thiz));
	}
	//设置nbt数据
	Py_METHOD_DEFINE(setNBT) {
		PyObject* nbt = nullptr;
		Py_PARSE("O", &nbt);
		PyEntity_VALUE;
		return ToPyObject(thiz->setNbt(static_cast<CompoundTag*>(ToNBT(nbt).get())));
	}
	//获取生命值
	Py_METHOD_DEFINE(getHealth) {
		PyEntity_VALUE;
		return ToPyObject(thiz->getHealth());
	}
	//获取最大生命值
	Py_METHOD_DEFINE(getMaxHealth) {
		PyEntity_VALUE;
		return ToPyObject(thiz->getMaxHealth());
	}
	//获取权限
	Py_METHOD_DEFINE(getPermissions) {
		PyEntity_VALUE2;
		return ToPyObject(static_cast<int>(thiz->getPlayerPermissionLevel()));
	}
	//设置权限
	Py_METHOD_DEFINE(setPermissions) {
		int perm;
		Py_PARSE("i", &perm);
		PyEntity_VALUE2;
		thiz->setPermissions(static_cast<CommandPermissionLevel>(perm));
		Py_RETURN_NONE;
	}
	//获取设备ID
	Py_METHOD_DEFINE(getPlatformOnlineId) {
		PyEntity_VALUE2;
		return ToPyObject(thiz->getPlatformOnlineId());
	}
	//获取设备类型
	Py_METHOD_DEFINE(getPlatform) {
		PyEntity_VALUE2;
		return ToPyObject(thiz->getPlatform());
	}
	//获取IP
	Py_METHOD_DEFINE(getIP) {
		PyEntity_VALUE2;
		auto ni = thiz->getNetworkIdentifier();
		return ToPyObject(Global<RakNet::RakPeer>->getAdr(*ni).ToString(false, ':'));
	}
	//设置玩家手上物品
	Py_METHOD_DEFINE(setHand) {
		PyObject* item_nbt = nullptr;
		Py_PARSE("s", &item_nbt);
		PyEntity_VALUE2;
		const_cast<ItemStack&>(thiz->getSelectedItem()) =
			ItemStack::fromTag(*static_cast<CompoundTag*>(ToNBT(item_nbt).get()));
		thiz->sendInventory(true);
		Py_RETURN_NONE;
	}
	//增加玩家背包物品
	Py_METHOD_DEFINE(addItem) {
		PyObject* item_nbt = nullptr;
		Py_PARSE("s", &item_nbt);
		PyEntity_VALUE2;
		auto item =
			ItemStack::fromTag(*static_cast<CompoundTag*>(ToNBT(item_nbt).get()));
		thiz->giveItem(&item);
		thiz->sendInventory(true);
		Py_RETURN_NONE;
	}
	//移除玩家背包物品
	Py_METHOD_DEFINE(removeItem) {
		int slot, num;
		Py_PARSE("ii", &slot, &num);
		PyEntity_VALUE2;
		thiz->getInventory().removeItem(slot, num);
		thiz->sendInventory(true);
		Py_RETURN_NONE;
	}
	//传送
	Py_METHOD_DEFINE(teleport) {
		Vec3 pos;
		int  dim;
		Py_PARSE("fffi", &pos.x, &pos.y, &pos.z, &dim);
		PyEntity_VALUE2;
		thiz->teleport(pos, dim);
		Py_RETURN_NONE;
	}
	//发送文本
	Py_METHOD_DEFINE(sendText) {
		const char* text = "";
		const char* type = "RAW";
		Py_PARSE("s|i", &text, &type);
		auto t = magic_enum::enum_cast<TextType>(type);
		if (!t)
			Py_RETURN_ERROR_FORMAT("Invalid text type %s", type);
		PyEntity_VALUE2;
		thiz->sendTextPacket(text, t.value());
		Py_RETURN_NONE;
	}
	//模拟玩家执行命令
	Py_METHOD_DEFINE(runCommandAs) {
		const char* cmd = "";
		Py_PARSE("s", &cmd);
		PyEntity_VALUE2;
		thiz->sendCommandRequestPacket(cmd);
		Py_RETURN_NONE;
	}
	//重新发送所有区块
	Py_METHOD_DEFINE(resendAllChunks) {
		PyEntity_VALUE2;
		thiz->resendAllChunks();
		Py_RETURN_NONE;
	}
	//断开连接
	Py_METHOD_DEFINE(disconnect) {
		const char* msg = "";
		Py_PARSE("|s", &msg);
		PyEntity_VALUE2;
		thiz->kick(msg);
		Py_RETURN_NONE;
	}
	//获取玩家分数
	Py_METHOD_DEFINE(getScore) {
		const char* objname = "";
		Py_PARSE("s", &objname);
		PyEntity_VALUE2;
		return ToPyObject(thiz->getScore(objname));
	}
	//设置玩家分数
	Py_METHOD_DEFINE(setScore) {
		const char* objname = "";
		int         count;
		Py_PARSE("si", &objname, &count);
		PyEntity_VALUE2;
		thiz->setScore(objname, count);
		Py_RETURN_NONE;
	}
	//增加玩家分数
	Py_METHOD_DEFINE(addScore) {
		const char* objname = "";
		int         count;
		Py_PARSE("si", &objname, &count);
		PyEntity_VALUE2;
		thiz->addScore(objname, count);
		Py_RETURN_NONE;
	}
	//减少玩家分数
	Py_METHOD_DEFINE(reduceScore) {
		const char* objname = "";
		int         count;
		Py_PARSE("si", &objname, &count);
		PyEntity_VALUE2;
		thiz->reduceScore(objname, count);
		Py_RETURN_NONE;
	}
	//增加等级
	Py_METHOD_DEFINE(addLevel) {
		int level;
		Py_PARSE("i", &level);
		PyEntity_VALUE2;
		thiz->addLevels(level);
		Py_RETURN_NONE;
	}
	//跨服传送
	Py_METHOD_DEFINE(transferServer) {
		const char* address = "";
		unsigned short port;
		Py_PARSE("sH", &address, &port);
		PyEntity_VALUE2;
		thiz->sendTransferPacket(address, port);
		Py_RETURN_NONE;
	}
	//发送表单
	Py_METHOD_DEFINE(sendCustomForm) {
		const char* str = "";
		PyObject* callback = nullptr;
		Py_PARSE("sO", &str, &callback);
		PyEntity_VALUE2;
		if (!PyFunction_Check(callback))
			return nullptr;
		thiz->sendCustomFormPacket(str,
			[thiz, callback] (string arg) {
				PyCaller pc;
				pc.call(callback, thiz, arg);
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
		PyEntity_VALUE2;
		if (!PyFunction_Check(callback))
			return nullptr;
		auto buttons = PyListToArray(buttons_list);
		auto images = PyListToArray(images_list);
		if (buttons.size() != images.size())
			Py_RETURN_ERROR("The number of buttons is not equal to the number of images");
		thiz->sendSimpleFormPacket(title, content, buttons, images,
			[thiz, callback] (int arg) {
				PyCaller pc;
				pc.call(callback, thiz, arg);
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
		PyEntity_VALUE2;
		if (!PyFunction_Check(callback))
			return nullptr;
		thiz->sendModalFormPacket(title, content, button1, button2,
			[thiz, callback] (bool arg) {
				PyCaller pc;
				pc.call(callback, thiz, arg);
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
		PyEntity_VALUE2;
		vector<pair<string, int>> data;
		fifo_json value = StrToJson(side_data);
		if (value.is_object())
			for (auto& [key, val] : value.items()) {
				data.push_back({key, val});
			}
		thiz->setSidebar(title, data, order);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeSidebar) {
		PyEntity_VALUE2;
		thiz->removeSidebar();
		Py_RETURN_NONE;
	}
	//Boss栏
	Py_METHOD_DEFINE(setBossbar) {
		const char* name = "";
		float       per;
		Py_PARSE("sf", &name, &per);
		PyEntity_VALUE2;
		thiz->sendBossEventPacket(BossEvent::Show, name, per, BossEventColour::Red);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeBossbar) {
		const char* name = "";
		Py_PARSE("s:removeBossbar", &name);
		PyEntity_VALUE2;
		thiz->sendBossEventPacket(BossEvent::Hide, name, 0, BossEventColour::Red);
		Py_RETURN_NONE;
	}
	//标签
	Py_METHOD_DEFINE(addTag) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		PyEntity_VALUE;
		thiz->addTag(tag);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeTag) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		PyEntity_VALUE;
		thiz->removeTag(tag);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(getTags) {
		PyEntity_VALUE;
		auto      tags = thiz->getTags();
		PyObject* list = PyList_New(0);
		for (size_t i = 0; i < tags.size(); i++) {
			PyList_Append(list, ToPyObject(tags[i]));
		}
		return list;
	}
	//杀死实体
	Py_METHOD_DEFINE(kill) {
		PyEntity_VALUE;
		thiz->kill();
		Py_RETURN_NONE;
	}
	//获取游戏模式
	Py_METHOD_DEFINE(getGameMode) {
		PyEntity_VALUE2;
		return ToPyObject(thiz->getPlayerGameType());
	}

	inline static PyMethodDef methods[] {
		Py_METHOD_NOARGS(getName),
		Py_METHOD_VARARGS(setName),
		Py_METHOD_NOARGS(getUuid),
		Py_METHOD_NOARGS(getXuid),
		Py_METHOD_NOARGS(getPos),
		Py_METHOD_NOARGS(getDimensionId),
		Py_METHOD_NOARGS(isStanding),
		Py_METHOD_NOARGS(isSneaking),
		Py_METHOD_NOARGS(getTypeId),
		Py_METHOD_NOARGS(getTypeName),
		Py_METHOD_NOARGS(getNBT),
		Py_METHOD_VARARGS(setNBT),
		Py_METHOD_NOARGS(getHealth),
		Py_METHOD_NOARGS(getMaxHealth),
		Py_METHOD_NOARGS(getPermissions),
		Py_METHOD_VARARGS(setPermissions),
		Py_METHOD_NOARGS(getPlatformOnlineId),
		Py_METHOD_NOARGS(getPlatform),
		Py_METHOD_NOARGS(getIP),
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
		Py_METHOD_END};
};

PyTypeObject PyEntity_Type {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Entity",                /* tp_name */
	sizeof(PyEntity),        /* tp_basicsize */
	0,                       /* tp_itemsize */
	nullptr,                 /* tp_dealloc */
	PyEntity::print,         /* tp_print */
	nullptr,                 /* tp_getattr */
	nullptr,                 /* tp_setattr */
	nullptr,                 /* tp_reserved */
	PyEntity::repr,          /* tp_repr */
	nullptr,                 /* tp_as_number */
	nullptr,                 /* tp_as_sequence */
	nullptr,                 /* tp_as_mapping */
	PyEntity::hash,          /* tp_hash */
	nullptr,                 /* tp_call */
	PyEntity::str,           /* tp_str */
	nullptr,                 /* tp_getattro */
	nullptr,                 /* tp_setattro */
	nullptr,                 /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,      /* tp_flags */
	"Entities in Minecraft", /* tp_doc */
	nullptr,                 /* tp_traverse */
	nullptr,                 /* tp_clear */
	PyEntity::rich_compare,  /* tp_richcompare */
	0,                       /* tp_weaklistoffset */
	nullptr,                 /* tp_iter */
	nullptr,                 /* tp_iternext */
	PyEntity::methods,       /* tp_methods */
	nullptr,                 /* tp_members */
	nullptr,                 /* tp_getset */
	nullptr,                 /* tp_base */
	nullptr,                 /* tp_dict */
	nullptr,                 /* tp_descr_get */
	nullptr,                 /* tp_descr_set */
	0,                       /* tp_dictoffset */
	nullptr,                 /* tp_init */
	nullptr,                 /* tp_alloc */
	nullptr,                 /* tp_new */
	nullptr,                 /* tp_free */
	nullptr,                 /* tp_is_gc */
	nullptr,                 /* tp_bases */
	nullptr,                 /* tp_mro */
	nullptr,                 /* tp_cache */
	nullptr,                 /* tp_subclasses */
	nullptr,                 /* tp_weaklist */
	nullptr,                 /* tp_del */
	0,                       /* tp_version_tag */
	nullptr,                 /* tp_finalize */
};

PyObject* ToPyObject(Actor* ptr) {
	PyEntity* obj = PyObject_New(PyEntity, &PyEntity_Type);
	obj->value = ptr;
	return reinterpret_cast<PyObject*>(obj);
}
