#include "pch.h"
#define PY_SSIZE_T_CLEAN
#include "include/Python.h"

#define VERSION_STRING "1.6.3"
#define VERSION_NUMBER 204
#define PLUGIN_PATH "plugins/py"
#define MODULE_NAME "mc"
#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr

constexpr size_t Hash(const char* s) {
	unsigned h = 0;
	for (; *s; ++s)
		h = 5 * h + *s;
	return size_t(h);
}

#pragma region EventCode
enum class EventCode {
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
	onPistonPush,
	onEndermanRandomTeleport,
};
//不推荐中文！
static const unordered_map<string, EventCode> events{
{"后台输入",EventCode::onConsoleInput},
{"后台输出",EventCode::onConsoleOutput},
{"选择表单",EventCode::onSelectForm},
{"使用物品",EventCode::onUseItem},
{"放置方块",EventCode::onPlaceBlock},
{"破坏方块",EventCode::onDestroyBlock},
{"打开箱子",EventCode::onOpenChest},
{"打开木桶",EventCode::onOpenBarrel},
{"关闭箱子",EventCode::onCloseChest},
{"关闭木桶",EventCode::onCloseBarrel},
{"放入取出",EventCode::onContainerChange},
{"切换维度",EventCode::onChangeDimension},
{"生物死亡",EventCode::onMobDie},
{"生物受伤",EventCode::onMobHurt},
{"玩家重生",EventCode::onRespawn},
{"聊天消息",EventCode::onChat},
{"输入文本",EventCode::onInputText},
{"更新命令方块",EventCode::onCommandBlockUpdate},
{"输入指令",EventCode::onInputCommand},
{"命令方块执行",EventCode::onCommandBlockPerform},
{"加入游戏",EventCode::onPlayerJoin},
{"离开游戏",EventCode::onPlayerLeft},
{"玩家攻击",EventCode::onPlayerAttack},
{"世界爆炸",EventCode::onLevelExplode},
{"玩家穿戴",EventCode::onSetArmor},
{"耕地破坏",EventCode::onFallBlockTransform},
{"使用重生锚",EventCode::onUseRespawnAnchorBlock},
{"计分板改变",EventCode::onScoreChanged},
{"玩家移动",EventCode::onMove},
{"活塞推动",EventCode::onPistonPush},
{"onConsoleInput",EventCode::onConsoleInput},
{"onConsoleOutput",EventCode::onConsoleOutput},
{"onSelectForm",EventCode::onSelectForm},
{"onUseItem",EventCode::onUseItem},
{"onPlaceBlock",EventCode::onPlaceBlock},
{"onDestroyBlock",EventCode::onDestroyBlock},
{"onOpenChest",EventCode::onOpenChest},
{"onOpenBarrel",EventCode::onOpenBarrel},
{"onCloseChest",EventCode::onCloseChest},
{"onCloseBarrel",EventCode::onCloseBarrel},
{"onContainerChange",EventCode::onContainerChange},
{"onChangeDimension",EventCode::onChangeDimension},
{"onMobDie",EventCode::onMobDie},
{"onMobHurt",EventCode::onMobHurt},
{"onRespawn",EventCode::onRespawn},
{"onChat",EventCode::onChat},
{"onInputText",EventCode::onInputText},
{"onCommandBlockUpdate",EventCode::onCommandBlockUpdate},
{"onInputCommand",EventCode::onInputCommand},
{"onCommandBlockPerform",EventCode::onCommandBlockPerform},
{"onPlayerJoin",EventCode::onPlayerJoin},
{"onPlayerLeft",EventCode::onPlayerLeft},
{"onPlayerAttack",EventCode::onPlayerAttack},
{"onLevelExplode",EventCode::onLevelExplode},
{"onSetArmor",EventCode::onSetArmor},
{"onFallBlockTransform",EventCode::onFallBlockTransform},
{"onUseRespawnAnchorBlock",EventCode::onUseRespawnAnchorBlock},
{"onScoreChanged",EventCode::onScoreChanged},
{"onMove",EventCode::onMove},
{"onPistonPush",EventCode::onPistonPush},
{"onEndermanRandomTeleport",EventCode::onEndermanRandomTeleport},
};
#pragma endregion
#pragma region Global variable
namespace {
//指令队列
static SPSCQueue* _command_queue = nullptr;
//网络处理
static ServerNetworkHandler* _server_network_handler = nullptr;
//世界
static Level* _level = nullptr;
//网络
static RakPeer* _rak_peer = nullptr;
//计分板
static Scoreboard* _scoreboard = nullptr;
//Py函数表
static unordered_map<EventCode, vector<PyObject*>> _functions;
//注册命令
static vector<pair<string, string>> _commands;
//伤害
static int _damage;
}
#pragma endregion
#pragma region Function Define
//注入时事件
static void init();
BOOL WINAPI DllMain(HMODULE, DWORD, LPVOID) {
	return TRUE;
}
//检查版本
bool checkBDSVersion(const char* str) {
	string version;
	SymCall<string&>("?getServerVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		&version);
	//cout << version << endl;
	return version == str;
}
//创建包
static VA createPacket(int type) {
	VA pkt[2];
	SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		pkt, type);
	return *pkt;
}
//是否为玩家
static bool isPlayer(void* ptr) {
	for (auto p : _level->getAllPlayers()) {
		if (ptr == p)
			return true;
	}
	return false;
}
//转宽字符
static wstring CharToWchar(const string& str) {
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
	wstring wstr;
	wstr.resize(len + 1);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), wstr.data(), len);
	return wstr;
}
//锁GIL调用函数
static void safeCall(const function<void()>& fn) {
	int nHold = PyGILState_Check();   //检测当前线程是否拥有GIL
	PyGILState_STATE gstate = PyGILState_LOCKED;
	if (!nHold)
		gstate = PyGILState_Ensure();//如果没有GIL，则申请获取GIL
	Py_BEGIN_ALLOW_THREADS;
	Py_BLOCK_THREADS;
	fn();
	Py_UNBLOCK_THREADS;
	Py_END_ALLOW_THREADS;
	if (!nHold)
		PyGILState_Release(gstate);//释放当前线程的GIL
}
//事件回调
static bool EventCallBack(EventCode e, PyObject* val) {
	bool result = true;
	safeCall([&] {
		vector<PyObject*>& List = _functions[e];
		if (!List.empty()) {
			for (PyObject* fn : List) {
				//if (val) {
				if (PyObject_CallFunction(fn, "O", val) == Py_False)
					result = false;
				//}
				//else {
				//	if (_PyObject_CallNoArg(fn) == Py_False)
				//		result = false;
				//}
				PyErr_Print();
			}
		}
		Py_CLEAR(val);
		}
	);
	return result;
}
//Packet相关封装
static unsigned sendModalFormRequestPacket(Player* p, const string& str) {
	static unsigned id = 0;
	VA pkt = createPacket(100);
	FETCH(unsigned, pkt + 48) = ++id;
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
	SymCall<VA>("?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
		_server_network_handler, p->getClientId(), pkt);
	//p->sendPacket(pkt);
}
static void sendBossEventCodePacket(Player* p, string name, float per, int eventtype) {
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
#pragma region PyEntity
//玩家指针类型
struct PyEntity {
	PyObject_HEAD;
	Actor* actor;
};
extern PyTypeObject PyEntity_Type;
static Actor* PyEntity_AsActor(PyObject* self) {
	if (reinterpret_cast<PyEntity*>(self)->actor)
		return reinterpret_cast<PyEntity*>(self)->actor;
	else
		Py_RETURN_ERROR("This entity is not available");
}
static Player* PyEntity_AsPlayer(PyObject* self) {
	if (isPlayer(reinterpret_cast<PyEntity*>(self)->actor))
		return reinterpret_cast<Player*>(reinterpret_cast<PyEntity*>(self)->actor);
	else
		Py_RETURN_ERROR("This entity is not player");
}

//初始化
static PyObject* PyEntity_New(PyTypeObject* type, PyObject*, PyObject*) {
	return type->tp_alloc(type, 0);
}
//构造函数
static int PyEntity_Init(PyObject* self, PyObject* args, PyObject*) {
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
static void PyEntity_Dealloc(PyObject* obj) {
	Py_TYPE(obj)->tp_free(obj);
}
//转字符串
static PyObject* PyEntity_Str(PyObject* self) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	string name = a->getNameTag();
	return PyUnicode_FromStringAndSize(name.c_str(), name.length());
}
//哈希
static Py_hash_t PyEntity_Hash(PyObject* self) {
	return reinterpret_cast<Py_hash_t>(PyEntity_AsActor(self));
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
static PyObject* PyEntity_GetName(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	string name = a->getNameTag();
	return PyUnicode_FromStringAndSize(name.c_str(), name.length());
}
static int PyEntity_SetName(PyObject* self, PyObject* arg, void*) {
	if (PyUnicode_Check(arg)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return -1;
		p->setName(PyUnicode_AsUTF8(arg));
		return 0;
	}
	return PyErr_BadArgument(), -1;
}
//获取UUID
static PyObject* PyEntity_GetUuid(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return PyUnicode_FromString(p->getUuid().c_str());
}
//获取XUID
static PyObject* PyEntity_GetXuid(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return PyUnicode_FromString(p->getXuid().c_str());
}
//获取坐标
static PyObject* PyEntity_GetPos(PyObject* self, void*) {
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
static PyObject* PyEntity_GetDimensionId(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getDimensionId());
}
//是否着地
static PyObject* PyEntity_GetIsStand(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyBool_FromLong(a->isStand());
}
//是否潜行
static PyObject* PyEntity_GetIsSneaking(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyBool_FromLong(a->isSneaking());
}
//获取类型
static PyObject* PyEntity_GetTypeID(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getEntityTypeId());
}
//获取类型字符串
static PyObject* PyEntity_GetTypeName(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyUnicode_FromString(a->getEntityTypeName().c_str());
}
//获取nbt数据
static PyObject* PyEntity_GetNBTInfo(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	string str = CompoundTagtoJson(a->save()).dump(4);
	return PyUnicode_FromStringAndSize(str.c_str(), str.length());
}
//获取生命值
static PyObject* PyEntity_GetHealth(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getHealth());
}
//获取最大生命值
static PyObject* PyEntity_GetMaxHealth(PyObject* self, void*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	return PyLong_FromLong(a->getMaxHealth());
}
//获取权限
static PyObject* PyEntity_GetPermissions(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return PyLong_FromLong(p->getPermissions());
}
static int PyEntity_SetPermissions(PyObject* self, PyObject* arg, void*) {
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
static PyObject* PyEntity_GetDeviceId(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	string str = p->getDeviceId();
	return PyUnicode_FromStringAndSize(str.c_str(), str.length());
}
//获取设备类型
static PyObject* PyEntity_GetDeviceOS(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	return PyLong_FromLong(p->getDeviceOS());
}
//获取IP
static PyObject* PyEntity_GetIP(PyObject* self, void*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	string str = _rak_peer->getSystemAddress(p->getClientId()).toString();
	return PyUnicode_FromStringAndSize(str.c_str(), str.length());
}

//获取属性方法
static PyGetSetDef PyEntity_GetSet[]{
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
	{"health", PyEntity_GetHealth, nullptr, nullptr},
	{"maxhealth", PyEntity_GetMaxHealth, nullptr, nullptr},
	{"perm", PyEntity_GetPermissions, PyEntity_SetPermissions, nullptr},
	{"deviceid", PyEntity_GetDeviceId, nullptr, nullptr},
	{"deviceos", PyEntity_GetDeviceOS, nullptr, nullptr},
	{"ip", PyEntity_GetIP, nullptr, nullptr},
	{nullptr}
};

//获取/设置玩家所有物品
static PyObject* PyEntity_GetAllItem(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	json value;

	json& inventory = value["Inventory"];
	for (auto& i : p->getInventory()->getSlots()) {
		inventory.push_back(CompoundTagtoJson(i->save()));
	}

	json& endchest = value["EndChest"];
	for (auto& i : p->getEnderChestContainer()->getSlots()) {
		endchest.push_back(CompoundTagtoJson(i->save()));
	}

	json& armor = value["Armor"];
	for (auto& i : p->getArmorContainer()->getSlots()) {
		armor.push_back(CompoundTagtoJson(i->save()));
	}

	value["OffHand"] = CompoundTagtoJson(p->getOffHand()->save());
	value["Hand"] = CompoundTagtoJson(p->getSelectedItem()->save());

	string str = value.dump(4);
	return PyUnicode_FromStringAndSize(str.c_str(), str.length());
}
static PyObject* PyEntity_SetAllItem(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setAllItem", &x)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		json value(StringtoJson(x));

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
			p->getOffHand()->fromJson(value["OffHand"]);
		}
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SetHand(PyObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setHand", &x)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		json json(StringtoJson(x));
		p->getSelectedItem()->fromJson(json);
		p->sendInventroy();
	}
	Py_RETURN_NONE;
}
//增加/移除物品
static PyObject* PyEntity_AddItem(PyObject* self, PyObject* args) {
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
static PyObject* PyEntity_RemoveItem(PyObject* self, PyObject* args) {
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
static PyObject* PyEntity_Teleport(PyObject* self, PyObject* args) {
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
static PyObject* PyEntity_SendTextPacket(PyObject* self, PyObject* args) {
	const char* msg = "";
	int mode = 0;
	if (PyArg_ParseTuple(args, "s|i:sendTextPacket", &msg, &mode)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		sendTextPacket(p, mode, msg);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendCommandPacket(PyObject* self, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:sendCommandPacket", &cmd)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		sendCommandRequestPacket(p, cmd);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_ResendAllChunks(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	p->resendAllChunks();
	Py_RETURN_NONE;
}
static PyObject* PyEntity_Disconnect(PyObject* self, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "s:disconnect", &msg)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		sendDisconnectPacket(p, msg);
	}
	Py_RETURN_NONE;
}
//计分板操作
static PyObject* PyEntity_GetScore(PyObject* self, PyObject* args) {
	const char* objname = "";
	if (PyArg_ParseTuple(args, "s:getScore", &objname)) {
		Player* p = PyEntity_AsPlayer(self);
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
static PyObject* PyEntity_ModifyScore(PyObject* self, PyObject* args) {
	const char* objname = ""; int count; int mode;
	if (PyArg_ParseTuple(args, "sii:modifyScore", &objname, &count, &mode)) {
		Player* p = PyEntity_AsPlayer(self);
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
static PyObject* PyEntity_AddLevel(PyObject* self, PyObject* args) {
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
static PyObject* PyEntity_TransferServer(PyObject* self, PyObject* args) {
	const char* address = "";
	short port;
	if (PyArg_ParseTuple(args, "sh:transferServer", &address, &port)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		sendTransferPacket(p, address, port);
	}
	Py_RETURN_NONE;
}
//发送表单
static PyObject* PyEntity_SendCustomForm(PyObject* self, PyObject* args) {
	const char* str = "";
	if (PyArg_ParseTuple(args, "s:sendCustomForm", &str)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		return PyLong_FromLong(sendModalFormRequestPacket(p, str));
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendSimpleForm(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* buttons = "";
	if (PyArg_ParseTuple(args, "sss:sendSimpleForm", &title, &content, &buttons)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})", title, content, buttons);
		return PyLong_FromLong(sendModalFormRequestPacket(p, str));
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendModalForm(PyObject* self, PyObject* args) {
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
		return PyLong_FromLong(sendModalFormRequestPacket(p, str));
	}
	Py_RETURN_NONE;
}
//设置侧边栏
static PyObject* PyEntity_SetSidebar(PyObject* self, PyObject* args) {
	const char* title = "";
	const char* data = "";
	if (PyArg_ParseTuple(args, "ss:setSidebar", &title, &data)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		sendsetDisplayObjectivePacket(p, title);
		json value = StringtoJson(data);
		vector<ScorePacketInfo> info;
		if (value.is_object())
			for (auto& [key, val] : value.items()) {
				ScorePacketInfo o(_scoreboard->createScoreBoardId(key),
					val.get<int>(), key);
				info.push_back(o);
			}
		sendSetScorePacket(p, 0, info);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveSidebar(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	sendsetDisplayObjectivePacket(p, "", "");
	Py_RETURN_NONE;
}
//Boss栏
static PyObject* PyEntity_SetBossbar(PyObject* self, PyObject* args) {
	const char* name = "";
	float per;
	if (PyArg_ParseTuple(args, "sf:setBossBar", &name, &per)) {
		Player* p = PyEntity_AsPlayer(self);
		if (!p)
			return nullptr;
		sendBossEventCodePacket(p, name, per, 0);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveBossbar(PyObject* self, PyObject*) {
	Player* p = PyEntity_AsPlayer(self);
	if (!p)
		return nullptr;
	sendBossEventCodePacket(p, "", 0, 2);
	Py_RETURN_NONE;
}
//标签
static PyObject* PyEntity_AddTag(PyObject* self, PyObject* args) {
	const char* tag = "";
	if (PyArg_ParseTuple(args, "s:addTag", &tag)) {
		Actor* a = PyEntity_AsActor(self);
		if (!a)
			return nullptr;
		a->addTag(tag);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveTag(PyObject* self, PyObject* args) {
	const char* tag = "";
	if (PyArg_ParseTuple(args, "s:removeTag", &tag)) {
		Actor* a = PyEntity_AsActor(self);
		if (!a)
			return nullptr;
		a->removeTag(tag);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_GetTags(PyObject* self, PyObject*) {
	Actor* a = PyEntity_AsActor(self);
	if (!a)
		return nullptr;
	span<string> tags = a->getTags();
	PyObject* list = PyList_New(0);
	for (size_t i = 0; i < tags.size; i++) {
		PyList_Append(list, PyUnicode_FromString(tags.data[i].c_str()));
	}
	return list;
}

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

static PyObject* PyEntity_FromEntity(Actor* ptr) {
	PyObject* obj;
	safeCall([&obj] {obj = PyEntity_Type.tp_alloc(&PyEntity_Type, 0); });
	reinterpret_cast<PyEntity*>(obj)->actor = ptr;
	return obj;
}
#pragma endregion
#pragma region Hook List
#if 0
HOOK(Level_tick, void, "?tick@Level@@UEAAXXZ",
	Level* _this) {
	original(_this);
}
#endif
HOOK(BDS_Main, int, "main",
	int argc, char* argv[], char* envp[]) {
	init();
	// 执行 main 函数
	return original(argc, argv, envp);
}
HOOK(Level_construct, Level*, "??0Level@@QEAA@AEBV?$not_null@V?$NonOwnerPointer@VSoundPlayerInterface@@@Bedrock@@@gsl@@V?$unique_ptr@VLevelStorage@@U?$default_delete@VLevelStorage@@@std@@@std@@V?$unique_ptr@VLevelLooseFileStorage@@U?$default_delete@VLevelLooseFileStorage@@@std@@@4@AEAVIMinecraftEventing@@_NEAEAVScheduler@@V?$not_null@V?$NonOwnerPointer@VStructureManager@@@Bedrock@@@2@AEAVResourcePackManager@@AEBV?$not_null@V?$NonOwnerPointer@VIEntityRegistryOwner@@@Bedrock@@@2@V?$WeakRefT@UEntityRefTraits@@@@V?$unique_ptr@VBlockComponentFactory@@U?$default_delete@VBlockComponentFactory@@@std@@@4@V?$unique_ptr@VBlockDefinitionGroup@@U?$default_delete@VBlockDefinitionGroup@@@std@@@4@@Z",
	Level* _this, VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7, VA a8, VA a9, VA a10, VA a11, VA a12, VA a13) {
	return _level = original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}
HOOK(SPSCQueue_construct, SPSCQueue*, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z",
	SPSCQueue* _this) {
	return _command_queue = original(_this);
}
HOOK(RakPeer_construct, RakPeer*, "??0RakPeer@RakNet@@QEAA@XZ",
	RakPeer* _this) {
	return _rak_peer = original(_this);
}
HOOK(ServerNetworkHandler_construct, VA, "??0ServerNetworkHandler@@QEAA@AEAVGameCallbacks@@AEBV?$NonOwnerPointer@VILevel@@@Bedrock@@AEAVNetworkHandler@@AEAVPrivateKeyManager@@AEAVServerLocator@@AEAVPacketSender@@AEAVAllowList@@PEAVPermissionsFile@@AEBVUUID@mce@@H_NAEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@HAEAVMinecraftCommands@@AEAVIMinecraftApp@@AEBV?$unordered_map@UPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$hash@UPackIdVersion@@@3@U?$equal_to@UPackIdVersion@@@3@V?$allocator@U?$pair@$$CBUPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@@3@@std@@AEAVScheduler@@V?$NonOwnerPointer@VTextFilteringProcessor@@@3@@Z",
	ServerNetworkHandler* _this, VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7, VA a8, VA a9, VA a10, VA a11, VA a12, VA a13, VA a14, VA a15, VA a16, VA a17, VA a18, VA a19, VA a20) {
	_server_network_handler = _this;
	return original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
}
HOOK(ServerScoreboard_construct, Scoreboard*, "??0ServerScoreboard@@QEAA@VCommandSoftEnumRegistry@@PEAVLevelStorage@@@Z",
	VA _this, VA a2, VA a3) {
	return _scoreboard = original(_this, a2, a3);
}
HOOK(ChangeSettingCommand_setup, void, "?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",
	VA _this) {
	for (auto& [cmd, des] : _commands) {
		SymCall("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
			_this, &cmd, des.c_str(), 0, 0, 0x80);
	}
	original(_this);
}
HOOK(onConsoleOutput, ostream&, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
	ostream& _this, const char* str, VA size) {
	if (&_this == &cout) {
		wstring wstr = CharToWchar(str);
		bool res = EventCallBack(EventCode::onConsoleOutput, PyUnicode_FromWideChar(wstr.c_str(), wstr.length()));
		if (!res)return _this;
	}
	return original(_this, str, size);
}
HOOK(onConsoleInput, bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	SPSCQueue* _this, const string& cmd) {
	//debug模式（不推荐使用）
	static bool debug = false;
	if (debug) {
		safeCall([&cmd] {PyRun_SimpleString(cmd.c_str()); });
		cout << '>';
		return 0;
	}
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
	wstring wstr = CharToWchar(cmd);
	if (EventCallBack(EventCode::onConsoleInput, PyUnicode_FromWideChar(wstr.c_str(), wstr.length())))
		return original(_this, cmd);
	else
		return false;
}
HOOK(onPlayerJoin, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
	ServerNetworkHandler* _this, VA id,/*SetLocalPlayerAsInitializedPacket*/ VA pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		EventCallBack(EventCode::onPlayerJoin, PyEntity_FromEntity(p));
	}
	original(_this, id, pkt);
}
HOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	VA _this, Player* p, char v3) {
	EventCallBack(EventCode::onPlayerLeft, PyEntity_FromEntity(p));
	return original(_this, p, v3);
}
HOOK(onUseItem, bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	VA _this, ItemStack* item, BlockPos* bp, char a4, VA a5, Block* b) {
	Player* p = FETCH(Player*, _this + 8);
	short iid = item->getId();
	short iaux = item->mAuxjson;
	string iname = item->getName();
	BlockLegacy* bl = b->getBlockLegacy();
	short bid = bl->getBlockItemID();
	string bn = bl->getBlockName();
	if (EventCallBack(EventCode::onUseItem,
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
		if (!EventCallBack(EventCode::onPlaceBlock,
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
#if 0//测试获取结构
	BlockPos size = { 5,5,5 };
	StructureSettings ss(&size, 0, 0);
	StructureTemplate st("tmp");
	StructureTemplate st2("tmp2");
	st.fillFromWorld(_this, bp, &ss);
	json v(toJson(st.save()));
	st2.fromJson(v);
	json v2(toJson(st2.save()));
	//print(v2);
	bp->y++;
	st2.placeInWorld(_this, _level->getBlockPalette(), bp, &ss);
	//st.placeInWorld(_this, _level->getBlockPalette(), bp, &ss);
#endif
	if (isPlayer(p)) {
		BlockLegacy* bl = _this->getBlock(bp)->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		if (!EventCallBack(EventCode::onDestroyBlock,
			Py_BuildValue("{s:O,s:s,s:i,s:[i,i,i]}",
				"player", PyEntity_FromEntity(p),
				"blockname", bn.c_str(),
				"blockid", static_cast<int>(bid),
				"position", bp->x, bp->y, bp->z
			)
		))
			return false;
	}
	return original(_this, p, bp, a3, a4);
}
HOOK(onOpenChest, bool, "?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	VA _this, Player* p, BlockPos* bp) {
	if (EventCallBack(EventCode::onOpenChest,
		Py_BuildValue("{s:O,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"position", bp->x, bp->y, bp->z
		)
	))
		return original(_this, p, bp);
	return false;
}
HOOK(onOpenBarrel, bool, "?use@BarrelBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	VA _this, Player* p, BlockPos* bp) {
	if (EventCallBack(EventCode::onOpenBarrel,
		Py_BuildValue("{s:O,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"position", bp->x, bp->y, bp->z
		)
	))
		return original(_this, p, bp);
	return false;
}
HOOK(onCloseChest, void, "?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z",
	VA _this, Player* p) {
	auto bp = (BlockPos*)(_this - 204);
	EventCallBack(EventCode::onCloseChest,
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
	EventCallBack(EventCode::onCloseBarrel,
		Py_BuildValue("{s:O,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"position", bp->x, bp->y, bp->z
		)
	);
	original(_this, p);
}
HOOK(onContainerChange, void, "?containerContentChanged@LevelContainerModel@@UEAAXH@Z",
	VA _this, unsigned slot) {
	Player* p = FETCH(Player*, _this + 208);//IDA LevelContainerModel::_getContainer line 15 25 v3
	BlockSource* bs = p->getRegion();
	BlockPos* bp = reinterpret_cast<BlockPos*>(_this + 216);
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	short bid = bl->getBlockItemID();
	if (bid == 54 || bid == 130 || bid == 146 || bid == -203 || bid == 205 || bid == 218) {	//非箱子、桶、潜影盒的情况不作处理
		VA v5 = (*reinterpret_cast<VA(**)(VA)>(FETCH(VA, _this) + 160))(_this);
		if (v5) {
			ItemStack* i = (*reinterpret_cast<ItemStack * (**)(VA, VA)>(FETCH(VA, v5) + 40))(v5, slot);
			EventCallBack(EventCode::onContainerChange,
				Py_BuildValue("{s:O,s:s,s:i,s:[i,i,i],s:i,s:i,s:s,s:i,s:i}",
					"player", PyEntity_FromEntity(p),
					"blockname", bl->getBlockName().c_str(),
					"blockid", bid,
					"position", bp->x, bp->y, bp->z,
					"itemid", i->getId(),
					"itemcount", i->mCount,
					"itemname", i->getName().c_str(),
					"itemaux", i->mAuxjson,
					"slot", slot
				)
			);
		}
	}
	original(_this, slot);
}
HOOK(onAttack, bool, "?attack@Player@@UEAA_NAEAVActor@@AEBW4ActorDamageCause@@@Z",
	Player* p, Actor* a, struct ActorDamageCause* c) {
	if (EventCallBack(EventCode::onPlayerAttack,
		Py_BuildValue("{s:O,s:O}",
			"player", PyEntity_FromEntity(p),
			"actor", PyEntity_FromEntity(a)
		)
	))
		return original(p, a, c);
	return false;
}
HOOK(onChangeDimension, bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z",
	VA _this, Player* p, VA req) {
	bool result = original(_this, p, req);
	if (result) {
		EventCallBack(EventCode::onChangeDimension, PyEntity_FromEntity(p));
	}
	return result;
}
HOOK(onMobDie, void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z",
	Mob* _this, VA dmsg) {
	char v72;
	Actor* sa = _this->getLevel()->fetchEntity(*(VA*)((*(VA(__fastcall**)(VA, char*))(*(VA*)dmsg + 64))(dmsg, &v72)));
	bool res = EventCallBack(EventCode::onMobDie,
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
	if (EventCallBack(EventCode::onMobHurt,
		Py_BuildValue("{s:i,s:O,s:O,s:i}",
			"dmcase", FETCH(unsigned, dmsg + 8),
			"actor1", PyEntity_FromEntity(_this),
			"actor2", PyEntity_FromEntity(sa),//可能为0
			"damage", a3
		)
	))
		return original(_this, dmsg, _damage, a4, a5);
	return false;
}
HOOK(onRespawn, void, "?respawn@Player@@UEAAXXZ",
	Player* p) {
	EventCallBack(EventCode::onRespawn, PyEntity_FromEntity(p));
	original(p);
}
HOOK(onChat, void, "?fireEventPlayerMessage@MinecraftEventing@@AEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@000@Z",
	VA _this, string& sender, string& target, string& msg, string& style) {
	EventCallBack(EventCode::onChat,
		Py_BuildValue("{s:s,s:s,s:s,s:s}",
			"sender", sender.c_str(),
			"target", target.c_str(),
			"msg", msg.c_str(),
			"style", style.c_str()
		)
	);
	original(_this, sender, target, msg, style);
}
HOOK(onInputText, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	ServerNetworkHandler* _this, VA id, /*TextPacket*/VA pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	bool res = true;
	if (p) {
		const string& msg = FETCH(string, pkt + 88);
		res = EventCallBack(EventCode::onInputText,
			Py_BuildValue("{s:O,s:s}",
				"player", PyEntity_FromEntity(p),
				"msg", msg.c_str()
			)
		);
	}
	if (res)original(_this, id, pkt);
}
HOOK(onInputCommand, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	ServerNetworkHandler* _this, VA id, /*CommandRequestPacket*/VA pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		const string& cmd = FETCH(string, pkt + 48);
		bool res = EventCallBack(EventCode::onInputCommand,
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
		EventCallBack(EventCode::onSelectForm,
			Py_BuildValue("{s:O,s:s,s:I}",
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
		res = EventCallBack(EventCode::onCommandBlockUpdate,
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
	if (EventCallBack(EventCode::onLevelExplode,
		Py_BuildValue("{s:O,s:[f,f,f],s:i,s:f}",
			"actor", PyEntity_FromEntity(a3),
			"position", pos.x, pos.y, pos.z,
			"dimensionid", bs->getDimensionId(),
			"power", a5
		)
	))
		return original(_this, bs, a3, pos, a5, a6, a7, a8, a9);
	return false;
}
HOOK(onCommandBlockPerform, bool, "?_execute@CommandBlock@@AEBAXAEAVBlockSource@@AEAVCommandBlockActor@@AEBVBlockPos@@_N@Z",
	VA _this, BlockSource* a2, VA a3, BlockPos* bp, bool a5) {
	//脉冲:0,重复:1,链:2
	int mode = SymCall<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z", a3, a2);
	//无条件:0,有条件:1
	bool condition = SymCall<bool>("?getConditionalMode@CommandBlockActor@@QEBA_NAEAVBlockSource@@@Z", a3, a2);
	string cmd = FETCH(string, a3 + 264);
	string rawname = FETCH(string, a3 + 296);
	if (EventCallBack(EventCode::onCommandBlockPerform,
		Py_BuildValue("{s:i,s:b,s:s,s:s,s:[i,i,i]}",
			"mode", mode,
			"condition", condition,
			"cmd", cmd.c_str(),
			"rawname", rawname.c_str(),
			"position", bp->x, bp->y, bp->z
		)
	))
		return original(_this, a2, a3, bp, a5);
	return false;
}
HOOK(onMove, void, "??0MovePlayerPacket@@QEAA@AEAVPlayer@@W4PositionMode@1@HH@Z",
	VA _this, Player* p, char a3, int a4, int a5) {
	EventCallBack(EventCode::onMove, PyEntity_FromEntity(p));
	original(_this, p, a3, a4, a5);
}
HOOK(onSetArmor, void, "?setArmor@Player@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z",
	Player* p, unsigned slot, ItemStack* i) {
	if (!EventCallBack(EventCode::onSetArmor,
		Py_BuildValue("{s:O,s:i,s:i,s:s,s:i,s:i}",
			"player", PyEntity_FromEntity(p),
			"itemid", i->getId(),
			"itemcount", i->mCount,
			"itemname", i->getName().c_str(),
			"itemaux", i->mAuxjson,
			"slot", slot
		)
	))
		return;
	return original(p, slot, i);
}
HOOK(onScoreChanged, void, "?onScoreChanged@ServerScoreboard@@UEAAXAEBUScoreboardId@@AEBVObjective@@@Z",
	Scoreboard* _this, ScoreboardId* a1, Objective* a2) {
	/*
	原命令：
	创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	*/
	int scoreboardid = a1->id;
	EventCallBack(EventCode::onScoreChanged,
		Py_BuildValue("{s:i,s:i,s:s,s:s}",
			"scoreboardid", scoreboardid,
			"playersnum", a2->getPlayerScore(a1)->getCount(),
			"objectivename", a2->getScoreName().c_str(),
			"objectivedisname", a2->getScoreDisplayName().c_str()
		)
	);
	original(_this, a1, a2);
}
HOOK(onFallBlockTransform, void, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z",
	VA _this, BlockSource* a1, BlockPos* a2, Actor* p, VA a4) {
	if (isPlayer(p)) {
		if (!EventCallBack(EventCode::onFallBlockTransform,
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
		if (!EventCallBack(EventCode::onUseRespawnAnchorBlock,
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
	if (EventCallBack(EventCode::onPistonPush,
		Py_BuildValue("{s:s,s:i,s:[i,i,i],s:[i,i,i],s:i}",
			"blockname", bn.c_str(),
			"blockid", bid,
			"blockpos", bp->x, bp->y, bp->z,
			"pistonpos", bp2->x, bp2->y, bp2->z,
			"dimensionid", bs->getDimensionId()
		)
	))
		return original(_this, bs, bp, a3, a4);
	return false;
}
HOOK(onEndermanRandomTeleport, bool, "?randomTeleport@TeleportComponent@@QEAA_NAEAVActor@@@Z",
	VA _this, Actor* a1) {
	if (EventCallBack(EventCode::onEndermanRandomTeleport,
		PyEntity_FromEntity(a1)
	))
		return original(_this, a1);
	return false;
}
#pragma endregion
#pragma region API Function
//获取版本
static PyObject* PyAPI_getVersion(PyObject*, PyObject*) {
	return PyLong_FromLong(VERSION_NUMBER);
}
//指令输出
static PyObject* PyAPI_logout(PyObject*, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "s:logout", &msg)) {
		SymCall<ostream&>("??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
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
//设置指令说明
static PyObject* PyAPI_setCommandDescription(PyObject*, PyObject* args) {
	const char* cmd = "";
	const char* des = "";
	if (PyArg_ParseTuple(args, "ss:setCommandDescription", &cmd, &des)) {
		_commands.push_back({ cmd, des });
	}
	Py_RETURN_NONE;
}
//获取玩家
static PyObject* PyAPI_getPlayerByXuid(PyObject*, PyObject* args) {
	const char* xuid = "";
	if (PyArg_ParseTuple(args, "s:getPlayerByXuid", &xuid)) {
		Player* p = _level->getPlayerByXuid(xuid);
		if (!p)
			Py_RETURN_ERROR("Failed to find player");
		return PyEntity_FromEntity(p);
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_getPlayerList(PyObject*, PyObject*) {
	PyObject* list = PyList_New(0);
	if (!_level)
		Py_RETURN_ERROR("Level is not set");
	for (Player* p : _level->getAllPlayers()) {
		PyList_Append(list, PyEntity_FromEntity(p));
	}
	return list;
}
//修改生物受伤的伤害值
static PyObject* PyAPI_setDamage(PyObject*, PyObject* args) {
	if (PyArg_ParseTuple(args, "i:setDamage", &_damage)) {
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setServerMotd(PyObject*, PyObject* args) {
	const char* name = "";
	if (PyArg_ParseTuple(args, "s:setServerMotd", &name)) {
		if (_server_network_handler)
			SymCall<VA, ServerNetworkHandler*, const string&, bool>("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
				_server_network_handler, name, true);
		else
			Py_RETURN_ERROR("Server did not finish loading");
	}
	Py_RETURN_NONE;
}
//根据坐标设置方块
static PyObject* PyAPI_getBlock(PyObject*, PyObject* args) {
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "iiii:getBlock", &bp.x, &bp.y, &bp.z, &did)) {
		BlockSource* bs = _level->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		BlockLegacy* bl = bs->getBlock(&bp)->getBlockLegacy();
		return Py_BuildValue("{s:s:s:i}",
			"blockname", bl->getBlockName().c_str(),
			"blockid", bl->getBlockItemID()
		);
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setBlock(PyObject*, PyObject* args) {
	const char* name = "";
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "siiii:setBlock", &name, &bp.x, &bp.y, &bp.z, &did)) {
		BlockSource* bs = _level->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		Block* b = *reinterpret_cast<Block**>(SYM((string("?m") + name + "@VanillaBlocks@@3PEBVBlock@@EB").c_str()));
		if (!b)
			Py_RETURN_ERROR("Unknown Block");
		bs->setBlock(b, &bp);
	}
	Py_RETURN_NONE;
}
//获取一个结构
static PyObject* PyAPI_getStructure(PyObject*, PyObject* args) {
	BlockPos pos1, pos2; int did;
	if (PyArg_ParseTuple(args, "iiiiiii:getStructure",
		&pos1.x, &pos1.y, &pos1.z,
		&pos2.x, &pos2.y, &pos2.z, &did)) {
		BlockSource* bs = _level->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		BlockPos start{
			min(pos1.x,pos2.x),
			min(pos1.y,pos2.y),
			min(pos1.z,pos2.z)
		};
		BlockPos size{
			max(pos1.x,pos2.x) - start.x,
			max(pos1.y,pos2.y) - start.y,
			max(pos1.z,pos2.z) - start.z
		};
		StructureSettings ss(&size, false, false);
		StructureTemplate st("tmp");
		st.fillFromWorld(bs, &start, &ss);

		string str = CompoundTagtoJson(st.save()).dump(4);
		return PyUnicode_FromStringAndSize(str.c_str(), str.length());
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setStructure(PyObject*, PyObject* args) {
	const char* data = "";
	BlockPos pos; int did;
	if (PyArg_ParseTuple(args, "siiii:setStructure",
		&data, &pos.x, &pos.y, &pos.z, &did)) {
		BlockSource* bs = _level->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		json value = StringtoJson(data);
		json& arr = value["size9"];
		if (!arr.is_array())
			Py_RETURN_ERROR("Invalid json string");
		BlockPos size{
			arr[0].get<int>(),
			arr[1].get<int>(),
			arr[2].get<int>()
		};
		StructureSettings ss(&size, true, false);
		StructureTemplate st("tmp");
		st.fromJson(value);
		st.placeInWorld(bs, _level->getBlockPalette(), &pos, &ss);
		for (int x = 0; x != size.x; ++x) {
			for (int y = 0; y != size.y; ++y) {
				for (int z = 0; z != size.z; ++z) {
					BlockPos bp{ x,y,z };
					bs->neighborChanged(&bp);
				}
			}
		}
	}
	Py_RETURN_NONE;
}
//模块方法列表
static PyMethodDef PyAPI_Methods[]{
	{"getVersion", PyAPI_getVersion, METH_NOARGS, nullptr},
	{"logout", PyAPI_logout, METH_VARARGS, nullptr},
	{"runcmd", PyAPI_runcmd, METH_VARARGS, nullptr},
	{"setListener", PyAPI_setListener, METH_VARARGS, nullptr},
	{"setCommandDescription", PyAPI_setCommandDescription, METH_VARARGS, nullptr},
	{"getPlayerByXuid", PyAPI_getPlayerByXuid, METH_VARARGS, nullptr},
	{"getPlayerList", PyAPI_getPlayerList, METH_NOARGS, nullptr},
	{"setDamage", PyAPI_setDamage, METH_VARARGS, nullptr},
	{"setServerMotd", PyAPI_setServerMotd, METH_VARARGS, nullptr},
	{"getBlock", PyAPI_getBlock, METH_VARARGS, nullptr},
	{"setBlock", PyAPI_setBlock, METH_VARARGS, nullptr},
	{"getStructure", PyAPI_getStructure, METH_VARARGS, nullptr},
	{"setStructure", PyAPI_setStructure, METH_VARARGS, nullptr},
	{nullptr}
};
//模块定义
static PyModuleDef PyAPI_Module{
	PyModuleDef_HEAD_INIT,
	MODULE_NAME,
	"API functions",
	-1,
	PyAPI_Methods,
	nullptr,
	nullptr,
	nullptr,
	nullptr
};

//模块初始化
static PyObject* PyAPI_init() {
	PyObject* module = PyModule_Create(&PyAPI_Module);
	PyModule_AddObject(module, "Entity", reinterpret_cast<PyObject*>(&PyEntity_Type));
	return module;
}
#pragma endregion

void init() {
	using namespace std::filesystem;
	INFO(VERSION_STRING " loaded.");
	//如果目录不存在创建目录
	if (!exists(PLUGIN_PATH))
		create_directories(PLUGIN_PATH);
	//将plugins/py加入模块搜索路径
	Py_SetPath((PLUGIN_PATH L";" + wstring(Py_GetPath())).c_str());
#pragma region 预初始化3.8+
	//PyPreConfig cfg;
	//PyPreConfig_InitPythonConfig(&cfg);
	//cfg.utf8_mode = 1;
	//cfg.configure_locale = 0;
	//Py_PreInitialize(&cfg);
#pragma endregion
	PyImport_AppendInittab(MODULE_NAME, PyAPI_init);//增加一个模块
	Py_Initialize();//初始化解释器
	if (PyType_Ready(&PyEntity_Type) < 0)
		ERR("Falid to prepare class 'Entity'");
	PyEval_InitThreads();//启用线程支持
	for (const directory_entry& info : directory_iterator(PLUGIN_PATH)) {
		const path& path = info;
		if (path.extension() == ".py" || path.extension() == ".pyd") {
			const string& name = path.stem().u8string();
			INFO("loading " << name);
			PyImport_ImportModule(name.c_str());
			PyErr_Print();
		}
	}
	PyEval_SaveThread();//释放当前线程
}