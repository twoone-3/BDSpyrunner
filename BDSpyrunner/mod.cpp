#include "pch.h"
#pragma region Macro
#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
#define CheckResult(...) if (!res) return 0; return original(__VA_ARGS__)
#pragma endregion
#pragma region Event
enum class Event : char {
	None,
	onConsoleInput,
	onConsoleOutput,
	onSelectForm,
	onUseItem,
	onPlaceBlock,
	onDestroyBlock,
	onOpenChest,
	onOpenBarrel,
	onCloseChest,
	onCloseBarrel,
	onContainerChange,
	onChangeDimension,
	onMobDie,
	onMobHurt,
	onRespawn,
	onChat,
	onInputText,
	onCommandBlockUpdate,
	onInputCommand,
	onCommandBlockPerform,
	onPlayerJoin,
	onPlayerLeft,
	onPlayerAttack,
	onLevelExplode,
	onSetArmor,
	onFallBlockTransform,
	onUseRespawnAnchorBlock,
	onScoreChanged,
	onMove,
	onPistonPush
};
static const unordered_map<string, Event> events{
{"后台输入",Event::onConsoleInput},
{"后台输出",Event::onConsoleOutput},
{"选择表单",Event::onSelectForm},
{"使用物品",Event::onUseItem},
{"放置方块",Event::onPlaceBlock},
{"破坏方块",Event::onDestroyBlock},
{"打开箱子",Event::onOpenChest},
{"打开木桶",Event::onOpenBarrel},
{"关闭箱子",Event::onCloseChest},
{"关闭木桶",Event::onCloseBarrel},
{"放入取出",Event::onContainerChange},
{"切换维度",Event::onChangeDimension},
{"生物死亡",Event::onMobDie},
{"生物受伤",Event::onMobHurt},
{"玩家重生",Event::onRespawn},
{"聊天消息",Event::onChat},
{"输入文本",Event::onInputText},
{"更新命令方块",Event::onCommandBlockUpdate},
{"输入指令",Event::onInputCommand},
{"命令方块执行",Event::onCommandBlockPerform},
{"加入游戏",Event::onPlayerJoin},
{"离开游戏",Event::onPlayerLeft},
{"玩家攻击",Event::onPlayerAttack},
{"世界爆炸",Event::onLevelExplode},
{"玩家穿戴",Event::onSetArmor},
{"耕地破坏",Event::onFallBlockTransform},
{"使用重生锚",Event::onUseRespawnAnchorBlock},
{"计分板改变",Event::onScoreChanged},
{"玩家移动",Event::onMove},
{"活塞推动",Event::onPistonPush},
{"onConsoleInput",Event::onConsoleInput},
{"onConsoleOutput",Event::onConsoleOutput},
{"onSelectForm",Event::onSelectForm},
{"onUseItem",Event::onUseItem},
{"onPlaceBlock",Event::onPlaceBlock},
{"onDestroyBlock",Event::onDestroyBlock},
{"onOpenChest",Event::onOpenChest},
{"onOpenBarrel",Event::onOpenBarrel},
{"onCloseChest",Event::onCloseChest},
{"onCloseBarrel",Event::onCloseBarrel},
{"onContainerChange",Event::onContainerChange},
{"onChangeDimension",Event::onChangeDimension},
{"onMobDie",Event::onMobDie},
{"onMobHurt",Event::onMobHurt},
{"onRespawn",Event::onRespawn},
{"onChat",Event::onChat},
{"onInputText",Event::onInputText},
{"onCommandBlockUpdate",Event::onCommandBlockUpdate},
{"onInputCommand",Event::onInputCommand},
{"onCommandBlockPerform",Event::onCommandBlockPerform},
{"onPlayerJoin",Event::onPlayerJoin},
{"onPlayerLeft",Event::onPlayerLeft},
{"onPlayerAttack",Event::onPlayerAttack},
{"onLevelExplode",Event::onLevelExplode},
{"onSetArmor",Event::onSetArmor},
{"onFallBlockTransform",Event::onFallBlockTransform},
{"onUseRespawnAnchorBlock",Event::onUseRespawnAnchorBlock},
{"onScoreChanged",Event::onScoreChanged},
{"onMove",Event::onMove},
{"onPistonPush",Event::onPistonPush}
};
#pragma endregion
#pragma region Global variable
static struct SPSCQueue* _command_queue = nullptr;//指令队列
static ServerNetworkHandler* _server_network_handler = nullptr;
static Level* _level = nullptr;
static Scoreboard* _scoreboard = nullptr;//计分板
static unordered_map<Event, vector<PyObject*>> _functions;//Py函数表
static map<string, string> _commands;//注册命令
static unordered_map<string, PyObject*> _share_data;//共享数据
static int _damage;//伤害值
#pragma endregion
#pragma region Function Define
//创建包
static inline VA createPacket(const int type) {
	VA pkt[2];
	SYMCALL("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		pkt, type);
	return *pkt;
}
//是否为玩家
static bool isPlayer(const void* ptr) {
	for (auto& p : _level->getAllPlayers()) {
		if (ptr == p)
			return true;
	}
	return false;
}
//锁GIL调用函数
static void safeCall(const function<void()>& fn) {
	int nHold = PyGILState_Check();   //检测当前线程是否拥有GIL
	PyGILState_STATE gstate = PyGILState_LOCKED;
	if (!nHold)
		gstate = PyGILState_Ensure();   //如果没有GIL，则申请获取GIL
	Py_BEGIN_ALLOW_THREADS;
	Py_BLOCK_THREADS;
	fn();
	Py_UNBLOCK_THREADS;
	Py_END_ALLOW_THREADS;
	if (!nHold)
		PyGILState_Release(gstate);    //释放当前线程的GIL
}
//字符串转JSON
static Json::Value toJson(const string& s) {
	Json::Value j;
	Json::CharReaderBuilder rb;
	string errs;
	Json::CharReader* r(rb.newCharReader());
	bool res = r->parse(s.c_str(), s.c_str() + s.length(), &j, &errs);
	if (!res || !errs.empty()) {
		cerr << errs << endl;;
	}
	delete r;
	return std::move(j);
}
//事件回调
static bool EventCall(Event e, PyObject* val) {
	bool result = true;
	safeCall([&] {
		auto& List = _functions[e];
		if (!List.empty()) {
			for (PyObject* fn : List) {
				if (PyObject_CallFunction(fn, "O", val) == Py_False)
					result = false;
				PyErr_Print();
			}
		}
		Py_CLEAR(val);
		}
	);
	return result;
}
//Packet相关封装
static int sendModalFormRequestPacket(Player* p, const string& str) {
	static int id = 0;
	id = 255 + id * 127;
	VA pkt = createPacket(100);
	FETCH(unsigned, pkt + 48) = id;
	FETCH(string, pkt + 56) = str;
	p->sendPacket(pkt);
	return id;
}
static void sendTransferPacket(Player* p, const string& address, short port) {
	VA pkt = createPacket(85);
	FETCH(string, pkt + 48) = address;
	FETCH(short, pkt + 80) = port;
	p->sendPacket(pkt);
}
static void sendDisconnectPacket(Player* p, const string& msg) {
	VA pkt = createPacket(5);
	FETCH(string, pkt + 56) = msg;
	p->sendPacket(pkt);
}
static void sendTextPacket(Player* p, int mode, const string& msg) {
	VA pkt = createPacket(9);
	FETCH(int, pkt + 48) = mode;
	FETCH(string, pkt + 56) = p->getNameTag();
	FETCH(string, pkt + 88) = msg;
	p->sendPacket(pkt);
}
static void sendCommandRequestPacket(Player* p, const string& cmd) {
	VA pkt = createPacket(77);
	FETCH(string, pkt + 48) = cmd;
	SYMCALL<VA>("?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
		_server_network_handler, p->getNetId(), pkt);
	//p->sendPacket(pkt);
}
static void sendBossEventPacket(Player* p, string name, float per, int eventtype) {
	VA pkt = createPacket(74);
	FETCH(VA, pkt + 56) = FETCH(VA, pkt + 64) = FETCH(VA, p->getUniqueID());
	FETCH(int, pkt + 72) = eventtype;//0显示,1更新,2隐藏,
	FETCH(string, pkt + 80) = name;
	FETCH(float, pkt + 112) = per;
	p->sendPacket(pkt);
}
static void sendsetDisplayObjectivePacket(Player* p, const string& title, const string& name = "name") {
	VA pkt = createPacket(107);
	FETCH(string, pkt + 48) = "sidebar";
	FETCH(string, pkt + 80) = name;
	FETCH(string, pkt + 112) = title;
	FETCH(string, pkt + 144) = "dummy";
	FETCH(char, pkt + 176) = 0;
	p->sendPacket(pkt);
}
static void sendSetScorePacket(Player* p, char type, const vector<ScorePacketInfo>& slot) {
	VA pkt = createPacket(108);
	FETCH(char, pkt + 48) = type;//{set,remove}
	FETCH(vector<ScorePacketInfo>, pkt + 56) = slot;
	p->sendPacket(pkt);
}
#pragma endregion
#pragma region PyEntityObject
//玩家指针类型
struct PyEntityObject {
	PyObject_HEAD;
	Actor* actor_;

	Player* asPlayer() {
		if (isPlayer(actor_))
			return (Player*)actor_;
		else
			Py_RETURN_ERROR("This entity is not player");
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
//哈希
static Py_hash_t PyEntity_Hash(PyObject* self) {
	return (Py_hash_t)((PyEntityObject*)self)->actor_;
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
		if (((PyEntityObject*)self)->actor_ == ((PyEntityObject*)other)->actor_)
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
	return PyUnicode_FromString(self->actor_->getNameTag().c_str());
}
static int PyEntity_SetName(PyEntityObject* self, PyObject* arg, void*) {
	if (PyUnicode_Check(arg)) {
		Player* p = self->asPlayer();
		if (!p)
			return -1;
		p->setName(PyUnicode_AsUTF8(arg));
		return 0;
	}
	return PyErr_BadArgument(), -1;
}
//获取UUID
static PyObject* PyEntity_GetUuid(PyEntityObject* self, void*) {
	Player* p = self->asPlayer();
	if (!p)
		return nullptr;
	return PyUnicode_FromString(p->getUuid().c_str());
}
//获取XUID
static PyObject* PyEntity_GetXuid(PyEntityObject* self, void*) {
	Player* p = self->asPlayer();
	if (!p)
		return nullptr;
	return PyUnicode_FromString(p->getXuid().c_str());
}
//获取坐标
static PyObject* PyEntity_GetPos(PyEntityObject* self, void*) {
	Vec3* pos = self->actor_->getPos();
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(pos->x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(pos->y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(pos->z));
	return list;
}
//获取维度ID
static PyObject* PyEntity_GetDimensionId(PyEntityObject* self, void*) {
	return PyLong_FromLong(self->actor_->getDimensionId());
}
//是否着地
static PyObject* PyEntity_GetIsStand(PyEntityObject* self, void*) {
	return PyBool_FromLong(self->actor_->isStand());
}
//获取类型
static PyObject* PyEntity_GetTypeID(PyEntityObject* self, void*) {
	return PyLong_FromLong(self->actor_->getEntityTypeId());
}
//获取类型字符串
static PyObject* PyEntity_GetTypeName(PyEntityObject* self, void*) {
	return PyUnicode_FromString(self->actor_->getEntityTypeName().c_str());
}
//获取nbt数据
static PyObject* PyEntity_GetNBTInfo(PyEntityObject* self, void*) {
	return PyUnicode_FromString(toJson(self->actor_->save()).toStyledString().c_str());
}
//获取生命值
static PyObject* PyEntity_GetHealth(PyEntityObject* self, void*) {
	return PyLong_FromLong(self->actor_->getHealth());
}
//获取最大生命值
static PyObject* PyEntity_GetMaxHealth(PyEntityObject* self, void*) {
	return PyLong_FromLong(self->actor_->getMaxHealth());
}
//获取权限
static PyObject* PyEntity_GetPermissions(PyEntityObject* self, void*) {
	Player* p = self->asPlayer();
	if (!p)
		return nullptr;
	return PyLong_FromLong(p->getPermissions());
}
static int PyEntity_SetPermissions(PyEntityObject* self, PyObject* arg, void*) {
	if (PyLong_Check(arg)) {
		Player* p = self->asPlayer();
		if (!p)
			return -1;
		p->setPermissions((char)PyLong_AsLong(arg));
		return 0;
	}
	return PyErr_BadArgument(), -1;
}

//获取属性方法
static PyGetSetDef PyEntity_GetSet[]{
	{"name", (getter)PyEntity_GetName, (setter)PyEntity_SetName, nullptr},
	{"uuid", (getter)PyEntity_GetUuid, nullptr, nullptr},
	{"xuid", (getter)PyEntity_GetXuid, nullptr, nullptr},
	{"pos", (getter)PyEntity_GetPos, nullptr, nullptr},
	{"did", (getter)PyEntity_GetDimensionId, nullptr, nullptr},
	{"isstand", (getter)PyEntity_GetIsStand, nullptr, nullptr},
	{"typeid", (getter)PyEntity_GetTypeID, nullptr, nullptr},
	{"typename", (getter)PyEntity_GetTypeName, nullptr, nullptr},
	{"nbt", (getter)PyEntity_GetNBTInfo, nullptr, nullptr},
	{"health", (getter)PyEntity_GetHealth, nullptr, nullptr},
	{"maxhealth", (getter)PyEntity_GetMaxHealth, nullptr, nullptr},
	{"perm", (getter)PyEntity_GetPermissions, (setter)PyEntity_SetPermissions, nullptr},
	{nullptr}
};

//获取/设置玩家所有物品
static PyObject* PyEntity_GetAllItem(PyEntityObject* self, PyObject*) {
	Player* p = self->asPlayer();
	if (!p)
		return nullptr;
	Json::Value json(Json::objectValue);

	Json::Value& inventory = json["Inventory"];
	for (auto& i : p->getContainer()->getSlots()) {
		inventory.append(toJson(i->save()));
	}

	Json::Value& endchest = json["EndChest"];
	for (auto& i : p->getEnderChestContainer()->getSlots()) {
		endchest.append(toJson(i->save()));
	}

	Json::Value& armor = json["Armor"];
	for (auto& i : p->getArmorContainer()->getSlots()) {
		armor.append(toJson(i->save()));
	}

	json["OffHand"] = toJson(p->getOffHand()->save());
	json["Hand"] = toJson(p->getSelectedItem()->save());

	const string& str = json.toStyledString();
	return PyUnicode_FromStringAndSize(str.c_str(), str.length());
}
static PyObject* PyEntity_SetAllItem(PyEntityObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setAllItem", &x)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		Json::Value json = toJson(x);

		if (json.isMember("Inventory")) {
			const vector<ItemStack*>& items = p->getContainer()->getSlots();
			Json::Value& inventory = json["Inventory"];
			for (unsigned i = 0; i < inventory.size(); i++) {
				items[i]->fromJson(inventory[i]);
			}
		}

		if (json.isMember("EndChest")) {
			const vector<ItemStack*>& items = p->getEnderChestContainer()->getSlots();
			Json::Value& endchest = json["EndChest"];
			for (unsigned i = 0; i < endchest.size(); i++) {
				items[i]->fromJson(endchest[i]);
			}
		}

		if (json.isMember("Armor")) {
			const vector<ItemStack*>& items = p->getArmorContainer()->getSlots();
			Json::Value& armor = json["Armor"];
			for (unsigned i = 0; i < armor.size(); i++) {
				items[i]->fromJson(armor[i]);
			}
		}

		if (json.isMember("OffHand")) {
			p->getOffHand()->fromJson(json["OffHand"]);
		}

		if (json.isMember("Hand")) {
			p->getSelectedItem()->fromJson(json["Hand"]);
		}
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}
//增加/移除物品
static PyObject* PyEntity_AddItem(PyEntityObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:addItem", &x)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		ItemStack i;
		i.fromJson(toJson(x));
		p->addItem(&i);
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveItem(PyEntityObject* self, PyObject* args) {
	int slot, num;
	if (PyArg_ParseTuple(args, "ii:removeItem", &slot, &num)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		p->getContainer()->clearItem(slot, num);
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}
//传送
static PyObject* PyEntity_Teleport(PyEntityObject* self, PyObject* args) {
	Vec3 pos; int did;
	if (PyArg_ParseTuple(args, "fffi:teleport", &pos.x, &pos.y, &pos.z, &did)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		p->teleport(&pos, did);
	}
	Py_RETURN_NONE;
}
//发送数据包
static PyObject* PyEntity_SendTextPacket(PyEntityObject* self, PyObject* args) {
	const char* msg = "";
	int mode = 0;
	if (PyArg_ParseTuple(args, "s|i:sendTextPacket", &msg, &mode)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		sendTextPacket(p, mode, msg);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendCommandPacket(PyEntityObject* self, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:sendCommandPacket", &cmd)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		sendCommandRequestPacket(p, cmd);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_ResendAllChunks(PyEntityObject* self, PyObject*) {
	Player* p = self->asPlayer();
	if (!p)
		return nullptr;
	p->resendAllChunks();
	Py_RETURN_NONE;
}
static PyObject* PyEntity_Disconnect(PyEntityObject* self, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "s:disconnect", &msg)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		sendDisconnectPacket(p, msg);
	}
	Py_RETURN_NONE;
}
//计分板操作
static PyObject* PyEntity_GetScore(PyEntityObject* self, PyObject* args) {
	const char* objname = "";
	if (PyArg_ParseTuple(args, "s:getScore", &objname)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		Objective* testobj = _scoreboard->getObjective(objname);
		if (testobj) {
			auto id = _scoreboard->getScoreboardId(p);
			auto score = testobj->getPlayerScore(id);
			return PyLong_FromLong(score->getCount());
		}
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_ModifyScore(PyEntityObject* self, PyObject* args) {
	const char* objname = ""; int count; int mode;
	if (PyArg_ParseTuple(args, "sii:modifyScore", &objname, &count, &mode)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		Objective* testobj = _scoreboard->getObjective(objname);
		if (testobj) {
			//mode:{set,add,remove}
			_scoreboard->modifyPlayerScore((ScoreboardId*)_scoreboard->getScoreboardId(p), testobj, count, mode);
		}
	}
	Py_RETURN_NONE;
}
//增加等级
static PyObject* PyEntity_AddLevel(PyEntityObject* self, PyObject* args) {
	int level;
	if (PyArg_ParseTuple(args, "i:addLevel", &level)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		p->addLevel(level);
	}
	Py_RETURN_NONE;
}
//跨服传送
static PyObject* PyEntity_TransferServer(PyEntityObject* self, PyObject* args) {
	const char* address = "";
	short port;
	if (PyArg_ParseTuple(args, "sh:transferServer", &address, &port)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		sendTransferPacket(p, address, port);
	}
	Py_RETURN_NONE;
}
//发送表单
static PyObject* PyEntity_SendCustomForm(PyEntityObject* self, PyObject* args) {
	const char* str = "";
	if (PyArg_ParseTuple(args, "s:sendCustomForm", &str)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		return PyLong_FromLong(sendModalFormRequestPacket(p, str));
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendSimpleForm(PyEntityObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* buttons = "";
	if (PyArg_ParseTuple(args, "sss:sendSimpleForm", &title, &content, &buttons)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})", title, content, buttons);
		return PyLong_FromLong(sendModalFormRequestPacket(p, str));
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendModalForm(PyEntityObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* button1 = "";
	const char* button2 = "";
	if (PyArg_ParseTuple(args, "ssss:sendModalForm", &title, &content, &button1, &button2)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","button1":"%s","button2":"%s","type":"modal"})", title, content, button1, button2);
		return PyLong_FromLong(sendModalFormRequestPacket(p, str));
	}
	Py_RETURN_NONE;
}
//设置侧边栏
static PyObject* PyEntity_SetSidebar(PyEntityObject* self, PyObject* args) {
	const char* title = "";
	const char* data = "";
	if (PyArg_ParseTuple(args, "ss:setSidebar", &title, &data)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		sendsetDisplayObjectivePacket(p, title);
		Json::Value value = toJson(data);
		if (value.isObject()) {
			vector<ScorePacketInfo> info;
			auto begin = value.begin();
			auto end = value.end();
			while (begin != end) {
				ScorePacketInfo o(_scoreboard->createScoreBoardId(begin.name()),
					begin->asInt(), begin.name());
				info.push_back(o);
				begin++;
			}
			sendSetScorePacket(p, 0, info);
		}
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveSidebar(PyEntityObject* self, PyObject*) {
	Player* p = self->asPlayer();
	if (!p)
		return nullptr;
	sendsetDisplayObjectivePacket(p, "", "");
	Py_RETURN_NONE;
}
//Boss栏
static PyObject* PyEntity_SetBossbar(PyEntityObject* self, PyObject* args) {
	const char* name = "";
	float per;
	if (PyArg_ParseTuple(args, "sf:setBossBar", &name, &per)) {
		Player* p = self->asPlayer();
		if (!p)
			return nullptr;
		sendBossEventPacket(p, name, per, 0);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveBossbar(PyEntityObject* self, PyObject*) {
	Player* p = self->asPlayer();
	if (!p)
		return nullptr;
	sendBossEventPacket(p, "", 0, 2);
	Py_RETURN_NONE;
}

//Entity方法
PyMethodDef PyEntity_Methods[]{
	{"getAllItem", (PyCFunction)PyEntity_GetAllItem, 4, nullptr},
	{"setAllItem", (PyCFunction)PyEntity_SetAllItem, 1, nullptr},
	{"addItem", (PyCFunction)PyEntity_AddItem, 1, nullptr},
	{"removeItem", (PyCFunction)PyEntity_RemoveItem, 1, nullptr},
	{"teleport", (PyCFunction)PyEntity_Teleport, 1, nullptr},
	{"sendTextPacket", (PyCFunction)PyEntity_SendTextPacket, 1, nullptr},
	{"sendCommandPacket", (PyCFunction)PyEntity_SendCommandPacket, 1, nullptr},
	{"resendAllChunks", (PyCFunction)PyEntity_ResendAllChunks, 4, nullptr},
	{"disconnect", (PyCFunction)PyEntity_Disconnect, 1, nullptr},
	{"getScore", (PyCFunction)PyEntity_GetScore, 1, nullptr},
	{"modifyScore", (PyCFunction)PyEntity_ModifyScore, 1, nullptr},
	{"addLevel", (PyCFunction)PyEntity_AddLevel, 1, nullptr},
	{"transferServer", (PyCFunction)PyEntity_TransferServer, 1, nullptr},
	{"sendCustomForm", (PyCFunction)PyEntity_SendCustomForm, 1, nullptr},
	{"sendSimpleForm", (PyCFunction)PyEntity_SendSimpleForm, 1, nullptr},
	{"sendModalForm", (PyCFunction)PyEntity_SendModalForm, 1, nullptr},
	{"setSidebar", (PyCFunction)PyEntity_SetSidebar, 1, nullptr},
	{"removeSidebar", (PyCFunction)PyEntity_RemoveSidebar, 4, nullptr},
	{"setBossbar", (PyCFunction)PyEntity_SetBossbar, 1, nullptr},
	{"removeBossbar", (PyCFunction)PyEntity_RemoveBossbar, 4, nullptr},
	{nullptr}
};
//Entity类型
static PyTypeObject PyEntity_Type{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"EntityObject",			/* tp_name */
	sizeof(PyEntityObject),	/* tp_basicsize */
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
	nullptr,				/* tp_str */
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
	nullptr,				/* tp_init */
	nullptr,				/* tp_alloc */
	PyEntity_New			/* tp_new */
};

static PyObject* PyEntity_FromEntity(Actor* ptr) {
	PyObject* obj;
	safeCall([&] {
		if (!PyType_Ready(&PyEntity_Type))
			obj = _PyObject_CallNoArg((PyObject*)&PyEntity_Type);
		}
	);
	((PyEntityObject*)obj)->actor_ = ptr;
	return obj;
}
#pragma endregion
#pragma region Hook List
HOOK(Level_tick, void, "?tick@Level@@UEAAXXZ",
	Level* _this) {
	if (!_level)
		_level = _this;
	original(_this);
	////执行todos函数
	//if (!_todos.empty()) {
	//	for (int i = 0; i < _todos.size(); i++) {
	//		_todos.front()();
	//		_todos.pop();
	//	}
	//}
}
HOOK(SPSCQueue, SPSCQueue*, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z",
	SPSCQueue* _this) {
	_command_queue = original(_this);
	return _command_queue;
}
HOOK(ServerNetworkHandler_ServerNetworkHandler, VA, "??0ServerNetworkHandler@@QEAA@AEAVGameCallbacks@@AEAVLevel@@AEAVNetworkHandler@@AEAVPrivateKeyManager@@AEAVServerLocator@@AEAVPacketSender@@AEAVAllowList@@PEAVPermissionsFile@@AEBVUUID@mce@@H_NAEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@HAEAVMinecraftCommands@@AEAVIMinecraftApp@@AEBV?$unordered_map@UPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$hash@UPackIdVersion@@@3@U?$equal_to@UPackIdVersion@@@3@V?$allocator@U?$pair@$$CBUPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@@3@@std@@AEAVScheduler@@V?$NonOwnerPointer@VTextFilteringProcessor@@@Bedrock@@@Z",
	ServerNetworkHandler* _this, VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7, VA a8, VA a9, VA a10, VA a11, VA a12, VA a13, VA a14, VA a15, VA a16, VA a17, VA a18, VA a19) {
	_server_network_handler = _this;
	return original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19);
}
HOOK(ChangeSettingCommand_setup, void, "?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",//"?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",?setup@KillCommand@@SAXAEAVCommandRegistry@@@Z
	VA _this) {
	for (auto& cmd : _commands) {
		SYMCALL("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
			_this, cmd.first.c_str(), cmd.second.c_str(), 0, 0, 0x40);
	}
	original(_this);
}
HOOK(ServerScoreboard_, Scoreboard*, "??0ServerScoreboard@@QEAA@VCommandSoftEnumRegistry@@PEAVLevelStorage@@@Z",
	VA _this, VA a2, VA a3) {
	_scoreboard = (Scoreboard*)original(_this, a2, a3);
	return _scoreboard;
}
HOOK(onConsoleOutput, ostream&, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
	ostream& _this, const char* str, VA size) {
	if (&_this == &cout) {
		wchar_t* wstr = new wchar_t[size];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstr, (int)size);
		bool res = EventCall(Event::onConsoleOutput, PyUnicode_FromWideChar(wstr, size));
		delete wstr;
		if (!res)return _this;
	}
	return original(_this, str, size);
}
HOOK(onConsoleInput, bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	SPSCQueue* _this, const string& cmd) {
	//是否开启debug模式
	static bool debug = false;
	if (cmd == "pydebug") {
		if (debug) {
			debug = false;
		}
		else {
			debug = true;
			cout << '>';
		}
		return 0;
	}
	if (debug) {
		safeCall([&cmd] {
			PyRun_SimpleString(cmd.c_str());
			}
		);
		cout << '>';
		return 0;
	}
	bool res = EventCall(Event::onConsoleInput, PyUnicode_FromStringAndSize(cmd.c_str(), cmd.length()));
	CheckResult(_this, cmd);
}
HOOK(onPlayerJoin, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
	ServerNetworkHandler* _this, VA id,/*SetLocalPlayerAsInitializedPacket*/ VA pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		EventCall(Event::onPlayerJoin, PyEntity_FromEntity(p));
	}
	original(_this, id, pkt);
}
HOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	VA _this, Player* p, char v3) {
	EventCall(Event::onPlayerLeft, PyEntity_FromEntity(p));
	return original(_this, p, v3);
}
HOOK(onUseItem, bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	VA _this, ItemStack* item, BlockPos* bp, char a4, VA a5, Block* b) {
	Player* p = FETCH(Player*, _this + 8);
	short iid = item->getId();
	short iaux = item->mAuxValue;
	string iname = item->getName();
	BlockLegacy* bl = b->getBlockLegacy();
	short bid = bl->getBlockItemID();
	string bn = bl->getBlockName();
	if (EventCall(Event::onUseItem,
		Py_BuildValue("{s:O,s:i,s:i,s:s,s:s,s:i,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"itemid", iid,
			"itemaux", iaux,
			"itemname", iname.c_str(),
			"blockname", bn.c_str(),
			"blockid", bid,
			"position", bp->x, bp->y, bp->z
		)
	))
		return original(_this, item, bp, a4, a5, b);
	else
		return false;
}
HOOK(onPlaceBlock, bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* _this, Block* b, BlockPos* bp, unsigned char a4, Actor* p, bool _bool) {
	if (isPlayer(p)) {
		BlockLegacy* bl = b->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		if (!EventCall(Event::onPlaceBlock,
			Py_BuildValue("{s:O,s:s,s:i,s:[i,i,i]}",
				"player", PyEntity_FromEntity(p),
				"blockname", bn.c_str(),
				"blockid", bid,
				"position", bp->x, bp->y, bp->z
			)
		))
			return false;
	}
	return original(_this, b, bp, a4, p, _bool);
}
HOOK(onDestroyBlock, bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource* _this, Actor* p, BlockPos* bp, ItemStack* a3, bool a4) {
	//BlockPos size = { 5,5,5 };
	//StructureSettings ss(&size, 0, 0);
	//StructureTemplate st("tmp");
	//StructureTemplate st2("tmp2");
	//st.fillFromWorld(_this, bp, &ss);
	//Json::Value v(toJson(st.save()));
	//print(v);
	//st2.fromJson(v);
	//Json::Value v2(toJson(st2.save()));
	//print(v2);
	//st2.placeInWorld(_this, _level->getBlockPalette(), bp, &ss);
	////st.placeInWorld(_this, _level->getBlockPalette(), bp, &ss);
	if (isPlayer(p)) {
		BlockLegacy* bl = _this->getBlock(bp)->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		if (!EventCall(Event::onDestroyBlock,
			Py_BuildValue("{s:O,s:s,s:i,s:[i,i,i]}",
				"player", PyEntity_FromEntity(p),
				"blockname", bn.c_str(),
				"blockid", (int)bid,
				"position", bp->x, bp->y, bp->z
			)
		))
			return false;
	}
	return original(_this, p, bp, a3, a4);
}
HOOK(onOpenChest, bool, "?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	VA _this, Player* p, BlockPos* bp) {
	bool res = EventCall(Event::onOpenChest,
		Py_BuildValue("{s:O,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"position", bp->x, bp->y, bp->z
		)
	);
	CheckResult(_this, p, bp);
}
HOOK(onOpenBarrel, bool, "?use@BarrelBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	VA _this, Player* p, BlockPos* bp) {
	bool res = EventCall(Event::onOpenBarrel,
		Py_BuildValue("{s:O,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"position", bp->x, bp->y, bp->z
		)
	);
	CheckResult(_this, p, bp);
}
HOOK(onCloseChest, void, "?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z",
	VA _this, Player* p) {
	auto bp = (BlockPos*)(_this - 204);
	EventCall(Event::onCloseChest,
		Py_BuildValue("{s:O,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"position", bp->x, bp->y, bp->z
		)
	);
	original(_this, p);
}
HOOK(onCloseBarrel, void, "?stopOpen@BarrelBlockActor@@UEAAXAEAVPlayer@@@Z",
	VA _this, Player* p) {
	auto bp = (BlockPos*)(_this - 204);
	EventCall(Event::onCloseBarrel,
		Py_BuildValue("{s:O,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"position", bp->x, bp->y, bp->z
		)
	);
	original(_this, p);
}
HOOK(onContainerChange, void, "?containerContentChanged@LevelContainerModel@@UEAAXH@Z",
	VA a1, VA slot) {
	Actor* v3 = FETCH(Actor*, a1 + 208);//IDA LevelContainerModel::_getContainer line 15 25
	BlockSource* bs = v3->getBlockSource();
	BlockPos* bp = (BlockPos*)(a1 + 216);
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	short bid = bl->getBlockItemID();
	if (bid == 54 || bid == 130 || bid == 146 || bid == -203 || bid == 205 || bid == 218) {	//非箱子、桶、潜影盒的情况不作处理
		VA v5 = (*(VA(**)(VA))(*(VA*)a1 + 160))(a1);
		if (v5) {
			ItemStack* i = (ItemStack*)(*(VA(**)(VA, VA))(*(VA*)v5 + 40))(v5, slot);
			Player* p = FETCH(Player*, a1 + 208);
			EventCall(Event::onContainerChange,
				Py_BuildValue("{s:O,s:s,s:i,s:[i,i,i],s:i,s:i,s:s,s:i,s:i}",
					"player", PyEntity_FromEntity(p),
					"blockname", bl->getBlockName().c_str(),
					"blockid", bid,
					"position", bp->x, bp->y, bp->z,
					"itemid", i->getId(),
					"itemcount", i->mCount,
					"itemname", i->getName().c_str(),
					"itemaux", i->mAuxValue,
					"slot", slot
				)
			);
		}
	}
	original(a1, slot);
}
HOOK(onAttack, bool, "?attack@Player@@UEAA_NAEAVActor@@@Z",
	Player* p, Actor* a) {
	bool res = EventCall(Event::onPlayerAttack,
		Py_BuildValue("{s:O,s:O}",
			"player", PyEntity_FromEntity(p),
			"actor", PyEntity_FromEntity(a)
		)
	);
	CheckResult(p, a);
}
HOOK(onChangeDimension, bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z",
	VA _this, Player* p, VA req) {
	bool result = original(_this, p, req);
	if (result) {
		EventCall(Event::onChangeDimension, PyEntity_FromEntity(p));
	}
	return result;
}
HOOK(onMobDie, void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z",
	Mob* _this, VA dmsg) {
	char v72;
	Actor* sa = _this->getLevel()->fetchEntity(*(VA*)((*(VA(__fastcall**)(VA, char*))(*(VA*)dmsg + 64))(dmsg, &v72)));
	bool res = EventCall(Event::onMobDie,
		Py_BuildValue("{s:I,s:O,s:O}",
			"dmcase", FETCH(unsigned, dmsg + 8),
			"actor1", PyEntity_FromEntity(_this),
			"actor2", PyEntity_FromEntity(sa)//可能为0
		)
	);
	if (res) original(_this, dmsg);
}
HOOK(onMobHurt, bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@H_N1@Z",
	Mob* _this, VA dmsg, int a3, bool a4, bool a5) {
	_damage = a3;//将生物受伤的值设置为可调整
	char v72;
	Actor* sa = _this->getLevel()->fetchEntity(*(VA*)((*(VA(__fastcall**)(VA, char*))(*(VA*)dmsg + 64))(dmsg, &v72)));
	bool res = EventCall(Event::onMobHurt,
		Py_BuildValue("{s:i,s:O,s:O,s:i}",
			"dmcase", FETCH(unsigned, dmsg + 8),
			"actor1", PyEntity_FromEntity(_this),
			"actor2", PyEntity_FromEntity(sa),//可能为0
			"damage", a3
		)
	);
	CheckResult(_this, dmsg, _damage, a4, a5);
}
HOOK(onRespawn, void, "?respawn@Player@@UEAAXXZ",
	Player* p) {
	EventCall(Event::onRespawn, PyEntity_FromEntity(p));
	original(p);
}
HOOK(onChat, void, "?fireEventPlayerMessage@MinecraftEventing@@AEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@000@Z",
	VA _this, string& sender, string& target, string& msg, string& style) {
	EventCall(Event::onChat, Py_BuildValue("{s:s,s:s,s:s,s:s}",
		"sender", sender.c_str(),
		"target", target.c_str(),
		"msg", msg.c_str(),
		"style", style.c_str()
	));
	original(_this, sender, target, msg, style);
}
HOOK(onInputText, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	ServerNetworkHandler* _this, VA id, /*TextPacket*/VA pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		const string& msg = FETCH(string, pkt + 88);
		bool res = EventCall(Event::onInputText,
			Py_BuildValue("{s:O,s:s}",
				"player", PyEntity_FromEntity(p),
				"msg", msg.c_str()
			)
		);
		if (res)original(_this, id, pkt);
	}
}
HOOK(onInputCommand, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	ServerNetworkHandler* _this, VA id, /*CommandRequestPacket*/VA pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		const string& cmd = FETCH(string, pkt + 48);
		bool res = EventCall(Event::onInputCommand,
			Py_BuildValue("{s:O,s:s}",
				"player", PyEntity_FromEntity(p),
				"cmd", cmd.c_str()
			)
		);
		if (res)original(_this, id, pkt);
	}
}
HOOK(onSelectForm, void, "?handle@?$PacketHandlerDispatcherInstance@VModalFormResponsePacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
	VA _this, VA id, ServerNetworkHandler* handle,/*ModalFormResponsePacket*/VA* ppkt) {
	VA pkt = *ppkt;
	Player* p = handle->_getServerPlayer(id, pkt);
	if (p) {
		unsigned fid = FETCH(unsigned, pkt + 48);
		string data = FETCH(string, pkt + 56);
		if (data.back() == '\n')data.pop_back();
		EventCall(Event::onSelectForm,
			Py_BuildValue("{s:O,s:s,s:i}",
				"player", PyEntity_FromEntity(p),
				"selected", data.c_str(),
				"formid", fid
			)
		);
	}
	original(_this, id, handle, ppkt);
}
HOOK(onCommandBlockUpdate, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
	ServerNetworkHandler* _this, VA id, /*CommandBlockUpdatePacket*/VA pkt) {
	bool res = true;
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		auto bp = FETCH(BlockPos, pkt + 48);
		auto mode = FETCH(unsigned short, pkt + 60);
		auto condition = FETCH(bool, pkt + 62);
		auto redstone = FETCH(bool, pkt + 63);
		auto cmd = FETCH(string, pkt + 72);
		auto output = FETCH(string, pkt + 104);
		auto rawname = FETCH(string, pkt + 136);
		auto delay = FETCH(int, pkt + 168);
		res = EventCall(Event::onCommandBlockUpdate,
			Py_BuildValue("{s:O,s:i,s:i,s:i,s:s,s:s,s:s,s:i,s:[i,i,i]}",
				"player", PyEntity_FromEntity(p),
				"mode", mode,
				"condition", condition,
				"redstone", redstone,
				"cmd", cmd.c_str(),
				"output", output.c_str(),
				"rawname", rawname.c_str(),
				"delay", delay,
				"position", bp.x, bp.y, bp.z
			)
		);
	}
	if (res)original(_this, id, pkt);
}
HOOK(onLevelExplode, bool, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* bs, Actor* a3, Vec3 pos, float a5, bool a6, bool a7, float a8, bool a9) {
	bool res = EventCall(Event::onLevelExplode,
		Py_BuildValue("{s:O,s:[f,f,f],s:i,s:f}",
			"actor", PyEntity_FromEntity(a3),
			"position", pos.x, pos.y, pos.z,
			"dimensionid", bs->getDimensionId(),
			"power", a5
		)
	);
	CheckResult(_this, bs, a3, pos, a5, a6, a7, a8, a9);
}
HOOK(onCommandBlockPerform, bool, "?_execute@CommandBlock@@AEBAXAEAVBlockSource@@AEAVCommandBlockActor@@AEBVBlockPos@@_N@Z",
	VA _this, BlockSource* a2, VA a3, BlockPos* bp, bool a5) {
	//脉冲:0,重复:1,链:2
	int mode = SYMCALL<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z", a3, a2);
	//无条件:0,有条件:1
	bool condition = SYMCALL<bool>("?getConditionalMode@CommandBlockActor@@QEBA_NAEAVBlockSource@@@Z", a3, a2);
	string cmd = FETCH(string, a3 + 264);
	string rawname = FETCH(string, a3 + 296);
	bool res = EventCall(Event::onCommandBlockPerform,
		Py_BuildValue("{s:i,s:b,s:s,s:s,s:[i,i,i]}",
			"mode", mode,
			"condition", condition,
			"cmd", cmd.c_str(),
			"rawname", rawname.c_str(),
			"position", bp->x, bp->y, bp->z
		)
	);
	CheckResult(_this, a2, a3, bp, a5);
}
HOOK(onMove, void, "??0MovePlayerPacket@@QEAA@AEAVPlayer@@W4PositionMode@1@HH@Z",
	VA _this, Player* p, char a3, int a4, int a5) {
	EventCall(Event::onMove, PyEntity_FromEntity(p));
	original(_this, p, a3, a4, a5);
}
HOOK(onSetArmor, void, "?setArmor@Player@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z",
	Player* p, unsigned slot, ItemStack* i) {
	if (!EventCall(Event::onSetArmor,
		Py_BuildValue("{s:O,s:i,s:i,s:s,s:i,s:i}",
			"player", PyEntity_FromEntity(p),
			"itemid", i->getId(),
			"itemcount", i->mCount,
			"itemname", i->getName().c_str(),
			"itemaux", i->mAuxValue,
			"slot", slot
		)
	))
		return;
	return original(p, slot, i);
}
HOOK(onScoreChanged, void, "?onScoreChanged@ServerScoreboard@@UEAAXAEBUScoreboardId@@AEBVObjective@@@Z",
	const Scoreboard* _this, ScoreboardId* a2, Objective* a3) {
	/*
	原命令：
	创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	*/
	int scoreboardid = a2->id;
	EventCall(Event::onScoreChanged,
		Py_BuildValue("{s:i,s:i,s:s,s:s}",
			"scoreboardid", scoreboardid,
			"playersnum", a3->getPlayerScore(a2)->getCount(),
			"objectivename", a3->getScoreName().c_str(),
			"objectivedisname", a3->getScoreDisplayName().c_str()
		)
	);
	original(_this, a2, a3);
}
HOOK(onFallBlockTransform, void, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z",
	VA _this, BlockSource* a1, BlockPos* a2, Actor* p, VA a4) {
	if (isPlayer(p)) {
		if (!EventCall(Event::onFallBlockTransform,
			Py_BuildValue("{s:O,s:[i,i,i],s:i}",
				"player", PyEntity_FromEntity(p),
				"position", a2->x, a2->y, a2->z,
				"dimensionid", a1->getDimensionId()
			)
		))
			return;
	}
	original(_this, a1, a2, p, a4);
}
HOOK(onUseRespawnAnchorBlock, bool, "?trySetSpawn@RespawnAnchorBlock@@CA_NAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* p, BlockPos* a2, BlockSource* a3, Level* a4) {
	if (isPlayer(p)) {
		if (!EventCall(Event::onUseRespawnAnchorBlock,
			Py_BuildValue("{s:O,s:[i,i,i],s:i}",
				"player", PyEntity_FromEntity(p),
				"position", a2->x, a2->y, a2->z,
				"dimensionid", a3->getDimensionId()
			)
		))
			return false;
	}
	return original(p, a2, a3, a4);
}
HOOK(onPistonPush, bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z",
	BlockActor* _this, BlockSource* bs, BlockPos* bp, unsigned a3, unsigned a4) {
	BlockLegacy* blg = bs->getBlock(bp)->getBlockLegacy();
	string bn = blg->getBlockName();
	short bid = blg->getBlockItemID();
	BlockPos* bp2 = _this->getPosition();
	bool res = EventCall(Event::onPistonPush,
		Py_BuildValue("{s:s,s:i,s:[i,i,i],s:[i,i,i],s:i}",
			"blockname", bn.c_str(),
			"blockid", bid,
			"blockpos", bp->x, bp->y, bp->z,
			"pistonpos", bp2->x, bp2->y, bp2->z,
			"dimensionid", bs->getDimensionId()
		)
	);
	CheckResult(_this, bs, bp, a3, a4);
}
#pragma endregion
#pragma region API Function
//获取版本
static PyObject* PyAPI_getVersion(PyObject*, PyObject*) {
	return PyLong_FromLong(141);
}
//指令输出
static PyObject* PyAPI_logout(PyObject*, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "s:logout", &msg)) {
		SYMCALL<ostream&>("??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
			&cout, msg, strlen(msg));
	}
	Py_RETURN_NONE;
}
//执行指令
static PyObject* PyAPI_runcmd(PyObject*, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:runcmd", &cmd)) {
		if (_command_queue)
			onConsoleInput::original(_command_queue, cmd);
		else
			Py_RETURN_ERROR("Command queue is not initialized");
	}
	Py_RETURN_NONE;
}
//多线程
static PyObject* PyAPI_newThread(PyObject*, PyObject* args) {
	PyObject* func;
	if (PyArg_ParseTuple(args, "O:newThread", &func)) {
		if (!PyFunction_Check(func)) {
			Py_RETURN_ERROR("Parameter 1 is not callable");
		}
		thread([&] {
			safeCall([&] {
				_PyObject_CallNoArg(func);
				});
			}
		).detach();
	}
	Py_RETURN_NONE;
}
//设置监听
static PyObject* PyAPI_setListener(PyObject*, PyObject* args) {
	const char* name = ""; PyObject* func;
	if (PyArg_ParseTuple(args, "sO:setListener", &name, &func)) {
		auto found = events.find(name);
		if (!PyFunction_Check(func)) {
			Py_RETURN_ERROR("Parameter 2 is not callable");
		}
		if (found == events.end()) {
			Py_RETURN_ERROR("Invalid Listener key words");
		}
		_functions[found->second].push_back(func);
	}
	Py_RETURN_NONE;
}
//共享数据
static PyObject* PyAPI_setShareData(PyObject*, PyObject* args) {
	const char* index = ""; PyObject* data;
	if (PyArg_ParseTuple(args, "sO:setShareData", &index, &data)) {
		_share_data.emplace(index, data);
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_getShareData(PyObject*, PyObject* args) {
	const char* index = "";
	if (PyArg_ParseTuple(args, "s:getShareData", &index)) {
		auto found = _share_data.find(index);
		if (found == _share_data.end())
			Py_RETURN_ERROR("This data does not exist");
		else
			return found->second;
	}
	Py_RETURN_NONE;
}
//设置指令说明
static PyObject* PyAPI_setCommandDescription(PyObject*, PyObject* args) {
	const char* cmd = "";
	const char* des = "";
	if (PyArg_ParseTuple(args, "ss:setCommandDescription", &cmd, &des)) {
		_commands.emplace(cmd, des);
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_getPlayerList(PyObject*, PyObject*) {
	PyObject* list = PyList_New(0);
	for (auto& p : _level->getAllPlayers()) {
		PyList_Append(list, PyEntity_FromEntity(p));
	}
	return list;
}
//修改生物受伤的伤害值!
static PyObject* PyAPI_setDamage(PyObject*, PyObject* args) {
	int real;
	if (PyArg_ParseTuple(args, "i:setDamage", &real)) {
		_damage = real;
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setServerMotd(PyObject*, PyObject* args) {
	const char* n;
	if (PyArg_ParseTuple(args, "s:setServerMotd", &n)) {
		if (_server_network_handler) {
			string name(n);
			SYMCALL<VA>("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
				_server_network_handler, &name, true);
		}
		else
			Py_RETURN_ERROR("Server did not finish loading");
	}
	Py_RETURN_NONE;
}
//根据坐标设置方块
static PyObject* PyAPI_getBlock(PyObject*, PyObject* args) {
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "iiii:getBlock", &bp.x, &bp.y, &bp.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			Py_RETURN_ERROR("Unknown dimension ID");
		}
		auto bl = bs->getBlock(&bp)->getBlockLegacy();
		return Py_BuildValue("{s:s:s:i}",
			"blockname", bl->getBlockName().c_str(),
			"blockid", (int)bl->getBlockItemID()
		);
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setBlock(PyObject*, PyObject* args) {
	const char* name = "";
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "siiii:setBlock", &name, &bp.x, &bp.y, &bp.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			Py_RETURN_ERROR("Unknown dimension ID");
		}
		bs->setBlock(name, bp);
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//获取一个结构
static PyObject* PyAPI_getStructure(PyObject*, PyObject* args) {
	BlockPos pos1, pos2; int did;
	if (PyArg_ParseTuple(args, "iiiiiii:getStructure",
		&pos1.x, &pos1.y, &pos1.z,
		&pos2.x, &pos2.y, &pos2.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			Py_RETURN_ERROR("Unknown dimension ID");
		}
		BlockPos start = {
			min(pos1.x,pos2.x),
			min(pos1.y,pos2.y),
			min(pos1.z,pos2.z) };
		BlockPos size = {
			max(pos1.x,pos2.x) - start.x,
			max(pos1.y,pos2.y) - start.y,
			max(pos1.z,pos2.z) - start.z
		};
		StructureSettings ss(&size, true, false);
		StructureTemplate st("tmp");
		st.fillFromWorld(bs, &start, &ss);
		string str = toJson(st.save()).toStyledString();
		return PyUnicode_FromStringAndSize(str.c_str(), str.length());
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setStructure(PyObject*, PyObject* args) {
	const char* data = "";
	BlockPos pos; int did;
	if (PyArg_ParseTuple(args, "siiii:setStructure",
		&data, &pos.x, &pos.y, &pos.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			Py_RETURN_ERROR("Unknown dimension ID");
		}
		Json::Value value = toJson(data);
		if (!value["size9"].isArray()) {
			cerr << "结构JSON错误" << endl;
			Py_RETURN_NONE;
		}
		BlockPos size = {
			value["size9"][0].asInt(),
			value["size9"][1].asInt(),
			value["size9"][2].asInt()
		};
		StructureSettings ss(&size, true, false);
		StructureTemplate st("tmp");
		st.fromJson(value);
		st.placeInWorld(bs, _level->getBlockPalette(), &pos, &ss);
		for (int x = 0; x != size.x; x++) {
			for (int y = 0; y != size.y; y++) {
				for (int z = 0; z != size.z; z++) {
					BlockPos bp = { x,y,z };
					bs->neighborChanged(&bp);
				}
			}

		}
	}
	Py_RETURN_NONE;
}
//模块方法列表
static PyMethodDef PyAPI_Methods[]{
	{"getVersion", PyAPI_getVersion, 4, nullptr},
	{"logout", PyAPI_logout, 1, nullptr},
	{"runcmd", PyAPI_runcmd, 1, nullptr},
	{"newThread", PyAPI_newThread, 1, nullptr},
	{"setListener", PyAPI_setListener, 1, nullptr},
	{"setShareData", PyAPI_setShareData, 1, nullptr},
	{"getShareData", PyAPI_getShareData, 1, nullptr},
	{"setCommandDescription", PyAPI_setCommandDescription, 1, nullptr},
	{"getPlayerList", PyAPI_getPlayerList, 4, nullptr},
	{"setDamage", PyAPI_setDamage, 1, nullptr},
	{"setServerMotd", PyAPI_setServerMotd, 1, nullptr},
	{"getBlock", PyAPI_getBlock, 1, nullptr},
	{"setBlock", PyAPI_setBlock, 1, nullptr},
	{"getStructure", PyAPI_getStructure, 1, nullptr},
	{"setStructure", PyAPI_setStructure, 1, nullptr},
	{nullptr}
};
//模块定义
static PyModuleDef PyAPI_Module{
	PyModuleDef_HEAD_INIT,
	"mc",
	nullptr,
	-1,
	PyAPI_Methods,
	nullptr,
	nullptr,
	nullptr,
	nullptr
};
//模块初始化
static PyObject* PyAPI_init() {
	return PyModule_Create(&PyAPI_Module);
}
#pragma endregion
BOOL WINAPI DllMain(HMODULE, DWORD reason, LPVOID) {
	if (reason == 1) {
		//while (1) {
		//	Tag* t = toTag(toJson(R"({})"));
		//	cout << toJson(t).toStyledString() << endl;
		//	t->deCompound();
		//	delete t;
		//}
		//sizeof(Json::Value);
		ios::sync_with_stdio(false);
		if (!filesystem::exists("plugins")) {
			filesystem::create_directory("plugins");
		}
		if (!filesystem::exists("plugins/py")) {
			filesystem::create_directory("plugins/py");
		}
		wstring path(L"plugins/py;");
		path.append(Py_GetPath());
		Py_SetPath(path.c_str());
		//解释器初始化
		//PyPreConfig cfg;
		//PyPreConfig_InitPythonConfig(&cfg);
		//cfg.utf8_mode = 1;
		//cfg.configure_locale = 0;
		//Py_PreInitialize(&cfg);
		PyImport_AppendInittab("mc", PyAPI_init); //增加一个模块
		Py_Initialize();//初始化解释器
		PyEval_InitThreads();//启用线程支持
		filesystem::directory_iterator files("plugins/py");
		for (auto& info : files) {
			auto& path = info.path();
			if (path.extension() == ".py" || path.extension() == ".pyd") {
				const string& name = path.stem().u8string();
				print("[BDSpyrunner] loading ", name);
				PyImport_ImportModule(name.c_str());
				PyErr_Print();
			}
		}
		PyEval_SaveThread();//释放当前线程
		print("[BDSpyrunner] 1.4.1 loaded. \n感谢小枫云 http://ipyvps.com 的赞助.");
	}
	return TRUE;
}