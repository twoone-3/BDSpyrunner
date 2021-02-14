#include "pch.h"
#include "BDS.hpp"
#pragma warning(disable:4996)
#pragma region 宏定义
#define api_method(name) {#name, api_##name, 1, 0}
#define api_function(name) static PyObject* api_##name(PyObject*, PyObject*args)
#define check_ret(...) if (!res) return 0;return original(__VA_ARGS__)
#define PlayerCheck(ptr)  PlayerList[(Player*)ptr]
#pragma endregion
#pragma region 全局变量
static VA _cmdqueue = 0, _ServerNetworkHandle = 0;
static Level* _level = 0;
static const VA STD_COUT_HANDLE = f(VA, SYM(MSSYM_B2UUA3impB2UQA4coutB1AA3stdB2AAA23VB2QDA5basicB1UA7ostreamB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB3AAAA11B1AA1A));
static Scoreboard* _scoreboard;//储存计分板名称
static unsigned _formid = 1;//表单ID
static unordered_map<char, vector<PyObject*>> PyFuncs;//Py函数
static unordered_map<Player*, bool> PlayerList;//玩家列表
static vector<pair<string, string>> Command;//注册命令
static unordered_map<string, PyObject*> ShareData;//共享数据
static int _Damage;//伤害值
static unordered_map<PyObject*, pair<unsigned, unsigned>> tick;//执行队列
static queue<function<void()>> todos;
#pragma endregion
#pragma region 函数定义
static Json::Value toJson(const string& s) {
	Json::Value j;
	Json::CharReaderBuilder rb;
	string errs;
	Json::CharReader* r(rb.newCharReader());
	bool res = r->parse(s.c_str(), s.c_str() + s.length(), &j, &errs);
	if (!res || !errs.empty()) {
		cerr << u8"[Jsoncpp]" << errs << endl;
	}
	delete r;
	return move(j);
}
static inline VA createPacket(int type) {
	VA pkt;
	SYMCALL(MSSYM_B1QE12createPacketB1AE16MinecraftPacketsB2AAA2SAB1QA2AVB2QDA6sharedB1UA3ptrB1AA7VPacketB3AAAA3stdB2AAE20W4MinecraftPacketIdsB3AAAA1Z,
		&pkt, type);
	return pkt;
}
static char stoe(const string& s) {
	if (s == u8"后台输入")return 0;
	else if (s == u8"后台输出")return 1;
	else if (s == u8"选择表单")return 2;
	else if (s == u8"使用物品")return 3;
	else if (s == u8"放置方块")return 4;
	else if (s == u8"破坏方块")return 5;
	else if (s == u8"打开箱子")return 6;
	else if (s == u8"打开木桶")return 7;
	else if (s == u8"关闭箱子")return 8;
	else if (s == u8"关闭木桶")return 9;
	else if (s == u8"放入取出")return 10;
	else if (s == u8"切换维度")return 11;
	else if (s == u8"生物死亡")return 12;
	else if (s == u8"生物受伤")return 13;
	else if (s == u8"玩家重生")return 14;
	else if (s == u8"聊天消息")return 15;
	else if (s == u8"输入文本")return 16;
	else if (s == u8"更新命令方块")return 17;
	else if (s == u8"输入指令")return 18;
	else if (s == u8"命令方块执行")return 19;
	else if (s == u8"加入游戏")return 20;
	else if (s == u8"离开游戏")return 21;
	else if (s == u8"玩家攻击")return 22;
	else if (s == u8"世界爆炸")return 23;
	else if (s == u8"玩家穿戴")return 24;
	else if (s == u8"耕地破坏")return 25;
	else if (s == u8"使用重生锚")return 26;
	else if (s == u8"计分板改变")return 27;
	else return-1;
}
static bool callpy(const char* type, PyObject* val) {
	bool result = true;
	int nHold = PyGILState_Check();   //检测当前线程是否拥有GIL
	PyGILState_STATE gstate;
	if (!nHold) {
		gstate = PyGILState_Ensure();   //如果没有GIL，则申请获取GIL
	}
	Py_BEGIN_ALLOW_THREADS;
	Py_BLOCK_THREADS;
	for (PyObject* fn : PyFuncs[stoe(type)]) {
		if (PyObject_CallFunction(fn, "O", val) == Py_False)
			result = false;
	}
	PyErr_Print();
	Py_UNBLOCK_THREADS;
	Py_END_ALLOW_THREADS;
	if (!nHold) {
		PyGILState_Release(gstate);    //释放当前线程的GIL
	}
	return result;
}
static unsigned ModalFormRequestPacket(Player* p, string str) {
	unsigned fid = _formid++;
	if (PlayerCheck(p)) {
		VA pkt = createPacket(100);
		f(unsigned, pkt + 40) = fid;
		f(string, pkt + 48) = str;
		p->sendPacket(pkt);
	}
	return fid;
}
static bool TransferPacket(Player* p, const string& address, short port) {
	if (PlayerCheck(p)) {
		VA pkt = createPacket(85);
		f(string, pkt + 40) = address;
		f(short, pkt + 72) = port;
		//Sleep(10);
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool TextPacket(Player* p, char mode, const string& msg) {
	if (PlayerCheck(p)) {
		VA pkt = createPacket(9);
		f(char, pkt + 40) = mode;
		f(string, pkt + 48) = p->getNameTag();
		f(string, pkt + 80) = msg;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool CommandRequestPacket(Player* p, const string& cmd) {
	if (PlayerCheck(p)) {
		VA pkt = createPacket(77);
		f(string, pkt + 40) = cmd;
		SYMCALL<VA>(MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE24AEBVCommandRequestPacketB3AAAA1Z,
			_ServerNetworkHandle, p->getNetId(), pkt);
		//p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool BossEventPacket(Player* p, string name, float per, int eventtype) {
	if (PlayerCheck(p)) {
		VA pkt = createPacket(74);
		f(VA, pkt + 48) = f(VA, pkt + 56) = f(VA, p->getUniqueID());
		f(int, pkt + 64) = eventtype;//0显示,1更新,2隐藏,
		f(string, pkt + 72) = name;
		f(float, pkt + 104) = per;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool setDisplayObjectivePacket(Player* p, const string& title, const string& name = "name") {
	if (PlayerCheck(p)) {
		VA pkt = createPacket(107);
		f(string, pkt + 40) = "sidebar";
		f(string, pkt + 72) = name;
		f(string, pkt + 104) = title;
		f(string, pkt + 136) = "dummy";
		f(char, pkt + 168) = 0;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool SetScorePacket(Player* p, char type, const vector<ScorePacketInfo>& slot) {
	if (PlayerCheck(p)) {
		VA pkt = createPacket(108);
		f(char, pkt + 40) = type;//{set,remove}
		f(vector<ScorePacketInfo>, pkt + 48) = slot;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
#pragma endregion
#pragma region API函数
// 指令输出
api_function(logout) {
	const char* msg;
	if (PyArg_ParseTuple(args, "s:logout", &msg)) {
		SYMCALL<VA>(MSSYM_MD5_b5f2f0a753fc527db19ac8199ae8f740,
			STD_COUT_HANDLE, msg, strlen(msg));
		return Py_True;
	}
	return Py_False;
}
// 执行指令
api_function(runcmd) {
	const char* cmd;
	if (PyArg_ParseTuple(args, "s:runcmd", &cmd)) {
		SYMCALL<bool>(MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e,
			_cmdqueue, (string)cmd);
		return Py_True;
	}
	return Py_False;
}
// 计时器
api_function(setTimer) {
	unsigned time; PyObject* func;
	if (PyArg_ParseTuple(args, "OI:setTimer", &func, &time)) {
		if (!PyCallable_Check(func))
			return Py_False;
		tick[func] = { time,time };
	}
	return Py_True;
}
api_function(removeTimer) {
	PyObject* func;
	if (PyArg_ParseTuple(args, "O:removeTimer", &func, &time)) {
		tick.erase(func);
	}
	return Py_True;
}
// 设置监听
api_function(setListener) {
	const char* e; PyObject* fn;
	if (PyArg_ParseTuple(args, "sO:setListener", &e, &fn)) {
		char event = stoe(e);
		if (event != -1) {
			PyFuncs[stoe(e)].push_back(fn);
			return Py_True;
		}
		else cout << u8"无效的监听:" << e << endl;
	}
	return Py_False;
}
// 共享数据
api_function(setShareData) {
	const char* index; PyObject* data;
	if (PyArg_ParseTuple(args, "sO:setShareData", &index, &data)) {
		ShareData[index] = data;
		return Py_True;
	}
	return Py_False;
}
api_function(getShareData) {
	const char* index;
	if (PyArg_ParseTuple(args, "s:getShareData", &index)) {
		return ShareData[index];
	}
	return Py_False;
}
// 设置指令说明
api_function(setCommandDescription) {
	const char* cmd, * des;
	if (PyArg_ParseTuple(args, "ss:setCommandDescribe", &cmd, &des)) {
		Command.push_back({ cmd,des });
		return Py_True;
	}
	return Py_False;
}
api_function(getPlayerList) {
	auto list = PyList_New(0);
	PyArg_ParseTuple(args, ":getPlayerList");
	for (auto& p : PlayerList) {
		if (p.second)
			PyList_Append(list, PyLong_FromUnsignedLongLong((VA)p.first));
	}
	return list;
}
// 发送表单
api_function(sendCustomForm) {
	Player* p; const char* str;
	if (PyArg_ParseTuple(args, "Ks:sendCustomForm", &p, &str)) {
		return PyLong_FromLong(ModalFormRequestPacket(p, str));
	}
	return PyLong_FromLong(0);
}
api_function(sendSimpleForm) {
	Player* p; const char* title, * content, * buttons;
	if (PyArg_ParseTuple(args, "Ksss:sendSimpleForm", &p, &title, &content, &buttons)) {
		char str[0x400];
		sprintf(str, R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})", title, content, buttons);
		return PyLong_FromLong(ModalFormRequestPacket(p, str));
	}
	return Py_False;
}
api_function(sendModalForm) {
	Player* p; const char* title, * content, * button1, * button2;
	if (PyArg_ParseTuple(args, "Kssss:sendModalForm", &p, &title, &content, &button1, &button2)) {
		char str[0x400];
		sprintf(str, R"({"title":"%s","content":"%s","button1":"%s","button2":"%s","type":"modal"})", title, content, button1, button2);
		return PyLong_FromLong(ModalFormRequestPacket(p, str));
	}
	return Py_False;
}
// 跨服传送
api_function(transferServer) {
	Player* p; const char* address; int port;
	if (PyArg_ParseTuple(args, "Ksi:transferServer", &p, &address, &port)) {
		if (TransferPacket(p, address, port))
			return Py_True;
	}
	return Py_False;
}
// 获取玩家信息
api_function(getPlayerInfo) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:getPlayerInfo", &p)) {
		if (PlayerCheck(p)) {
			Vec3* pp = p->getPos();
			return Py_BuildValue("{s:s,s:[f,f,f],s:i,s:b,s:i,s:i,s:s,s:s}",
				"playername", p->getNameTag().c_str(),
				"XYZ", pp->x, pp->y, pp->z,
				"dimensionid", p->getDimensionId(),
				"isstand", p->isStand(),
				"health", p->getHealth(),
				"maxhealth", p->getMaxHealth(),
				"xuid", p->getXuid().c_str(),
				"uuid", p->getUuid().c_str()
			);
		}
	}
	return PyDict_New();
}
api_function(getActorInfo) {
	Actor* a;
	if (PyArg_ParseTuple(args, "K:getActorInfo", &a)) {
		if (!a)
			return Py_False;
		Vec3* pp = a->getPos();
		return Py_BuildValue("{s:s,s:i,s:[f,f,f],s:i,s:b,s:i,s:i}",
			"entityname", a->getEntityTypeName().c_str(),
			"entityid", a->getEntityTypeId(),
			"XYZ", pp->x, pp->y, pp->z,
			"dimensionid", a->getDimensionId(),
			"isstand", a->isStand(),
			"health", a->getHealth(),
			"maxhealth", a->getMaxHealth()
		);
	}
	return Py_False;
}
api_function(getActorInfoEx) {
	Actor* a;
	if (PyArg_ParseTuple(args, "K:getActorInfoEx", &a)) {
		if (!a)
			return Py_False;
		Tag* t = a->save();
		string info = toJson(t).toStyledString();
		t->deCompound();
		delete t;
		return PyUnicode_FromString(info.c_str());
	}
	return Py_False;
}
// 玩家权限
api_function(getPlayerPerm) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:getPlayerPerm", &p)) {
		if (PlayerCheck(p)) {
			return PyLong_FromLong(p->getPermission());
		}
	}
	return Py_False;
}
api_function(setPlayerPerm) {
	Player* p; unsigned char lv;
	if (PyArg_ParseTuple(args, "Kb:setPlayerPerm", &p, &lv)) {
		if (PlayerCheck(p)) {
			p->setPermissionLevel(lv);
			return Py_True;
		}
	}
	return Py_False;
}
// 增加玩家等级
api_function(addLevel) {
	Player* p; int lv;
	if (PyArg_ParseTuple(args, "Ki::addLevel", &p, &lv)) {
		if (PlayerCheck(p)) {
			SYMCALL(MSSYM_B1QA9addLevelsB1AA6PlayerB2AAA6UEAAXHB1AA1Z, p, lv);
			return Py_True;
		}
	}
	return Py_False;
}
// 设置玩家名字
api_function(setName) {
	Player* p; const char* name;
	if (PyArg_ParseTuple(args, "Ks:setName", &p, &name)) {
		if (PlayerCheck(p)) {
			p->setName(name);
			return Py_True;
		}
	}
	return Py_False;
}
// 玩家分数
api_function(getPlayerScore) {
	Player* p; const char* obj;
	if (PyArg_ParseTuple(args, "Ks:getPlayerScore", &p, &obj)) {
		if (PlayerCheck(p)) {
			Objective* testobj = _scoreboard->getObjective(obj);
			if (testobj) {
				auto id = _scoreboard->getScoreboardId(p);
				auto score = testobj->getPlayerScore(id);
				return PyLong_FromLong(score->getCount());
			}
			else cout << "bad objective: " << obj << endl;
		}
	}
	return Py_False;
}
api_function(modifyPlayerScore) {
	Player* p; const char* obj; int count; int mode;
	if (PyArg_ParseTuple(args, "Ksii:modifyPlayerScore", &p, &obj, &count, &mode)) {
		if (PlayerCheck(p)) {
			Objective* testobj = _scoreboard->getObjective(obj);
			if (testobj) {
				//mode:{set,add,remove}
				_scoreboard->modifyPlayerScore((ScoreboardId*)_scoreboard->getScoreboardId(p), testobj, count, mode);
				return Py_True;
			}
			else cout << "bad objective: " << obj << endl;
		}
	}
	return Py_False;
}
// 模拟玩家发送文本
api_function(talkAs) {
	Player* p; const char* msg;
	if (PyArg_ParseTuple(args, "Ks:talkAs", &p, &msg)) {
		if (TextPacket(p, 1, msg))
			return Py_True;
	}
	return Py_False;
}
// 模拟玩家执行指令
api_function(runcmdAs) {
	Player* p; const char* cmd;
	if (PyArg_ParseTuple(args, "Ks:runcmdAs", &p, &cmd)) {
		if (CommandRequestPacket(p, cmd))
			return Py_True;
	}
	return Py_False;
}
// 传送玩家
api_function(teleport) {
	Player* p; float x, y, z; int did;
	if (PyArg_ParseTuple(args, "Kfffi:teleport", &p, &x, &y, &z, &did)) {
		p->teleport({ x,y,z }, did);
		return Py_True;
	}
	return Py_False;
}
// 原始输出
api_function(tellraw) {
	const char* msg;
	Player* p;
	if (PyArg_ParseTuple(args, "Ks:tellraw", &p, &msg)) {
		if (TextPacket(p, 0, msg))
			return Py_True;
	}
	return Py_False;
}
api_function(tellrawEx) {
	const char* msg;
	Player* p;
	int mode;
	if (PyArg_ParseTuple(args, "Ksi:tellrawEx", &p, &msg, &mode)) {
		if (TextPacket(p, mode, msg))
			return Py_True;
	}
	return Py_False;
}
// boss栏
api_function(setBossBar) {
	Player* p; const char* name; float per;
	if (PyArg_ParseTuple(args, "Ksf:", &p, &name, &per)) {
		if (BossEventPacket(p, name, per, 0))
			return Py_True;
	}
	return Py_False;
}
api_function(removeBossBar) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:removeBossBar", &p)) {
		if (BossEventPacket(p, "", 0.0, 2))
			return Py_True;
	}
	return Py_False;
}
// 通过玩家指针获取计分板id
api_function(getScoreBoardId) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:getScoreBoardId", &p)) {
		if (PlayerCheck(p)) {
			return Py_BuildValue("{s:i}",
				"scoreboardid", _scoreboard->getScoreboardId(p)
			);
		}
	}
	return Py_False;
}
api_function(createScoreBoardId) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:createScoreBoardId", &p)) {
		if (PlayerCheck(p)) {
			_scoreboard->createScoreBoardId(p);
			return Py_True;
		}
	}
	return Py_False;
}
// 修改生物受伤的伤害值!
api_function(setDamage) {
	int a;
	if (PyArg_ParseTuple(args, "i:setDamage", &a)) {
		_Damage = a;
		return Py_True;
	}
	return Py_False;
}
api_function(setServerMotd) {
	const char* n;
	if (PyArg_ParseTuple(args, "s:setServerMotd", &n) && _ServerNetworkHandle) {
		string name = n;
		SYMCALL<VA>(MSSYM_MD5_21204897709106ba1d290df17fead479,
			_ServerNetworkHandle, &name, true);
		return Py_True;
	}
	return Py_False;
}
// 玩家背包相关操作
api_function(getPlayerItems) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:getPlayerItems", &p)) {
		if (PlayerCheck(p)) {
			Json::Value j;
			for (auto& i : p->getContainer()->getSlots()) {
				j.append(toJson(i->save()));
			}
			return PyUnicode_FromString(j.toStyledString().c_str());
		}
	}
	return Py_False;
}
api_function(setPlayerItems) {
	Player* p;
	const char* x;
	if (PyArg_ParseTuple(args, "Ks:setPlayerItems", &p, &x)) {
		if (PlayerCheck(p)) {
			Json::Value j = toJson(x);
			if (j.isArray()) {
				vector<ItemStack*> is = p->getContainer()->getSlots();
				for (unsigned i = 0; i < j.size(); i++) {
					Tag* t = toTag(j[i]);
					is[i]->fromTag(t);
					t->deCompound();
					delete t;
				}
				p->sendInventroy();
			}
		}
		return Py_True;
	}
	return Py_False;
}
api_function(getPlayerArmor) {
	Player* p; int slot;
	if (PyArg_ParseTuple(args, "Ki:getPlayerArmor", &p, &slot)) {
		if (PlayerCheck(p)) {
			ItemStack* item = p->getArmor(slot);
			return PyUnicode_FromString(toJson(item->save()).toStyledString().c_str());
		}
	}
	return Py_False;
}
api_function(setPlayerArmor) {
	Player* p; int slot; const char* x;
	if (PyArg_ParseTuple(args, "Kis:setPlayerArmor", &p, &slot, &x)) {
		if (PlayerCheck(p)) {
			Json::Value j = toJson(x);
			Tag* t = toTag(j);
			p->getArmor(slot)->fromTag(t);
			t->deCompound();
			p->sendInventroy();
			delete t;
			return Py_True;
		}
	}
	return Py_False;
}
api_function(getPlayerHand) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:getPlayerHand", &p)) {
		if (PlayerCheck(p)) {
			ItemStack* item = p->getSelectedItem();
			return PyUnicode_FromString(toJson(item->save()).toStyledString().c_str());
		}
	}
	return Py_False;
}
api_function(setPlayerHand) {
	Player* p;
	const char* x;
	if (PyArg_ParseTuple(args, "Ks:setPlayerHand", &p, &x)) {
		if (PlayerCheck(p)) {
			Json::Value j = toJson(x);
			Tag* t = toTag(j);
			p->getSelectedItem()->fromTag(t);
			t->deCompound();
			p->sendInventroy();
			delete t;
			return Py_True;
		}
	}
	return Py_False;
}
api_function(getPlayerItem) {
	Player* p; int slot;
	if (PyArg_ParseTuple(args, "Ki:getPlayerItem", &p, &slot)) {
		if (PlayerCheck(p)) {
			ItemStack* item = p->getInventoryItem(slot);
			return PyUnicode_FromString(toJson(item->save()).toStyledString().c_str());
		}
	}
	return Py_False;
}
api_function(setPlayerItem) {
	Player* p; int slot; const char* x;
	if (PyArg_ParseTuple(args, "Kis:setPlayerItem", &p, &slot, &x)) {
		if (PlayerCheck(p)) {
			Tag* t = toTag(toJson(x));
			p->getInventoryItem(slot)->fromTag(t);
			p->sendInventroy();
			t->deCompound();
			delete t;
			return Py_True;
		}
	}
	return Py_False;
}
api_function(getPlayerEnderChests) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:getPlayerEnderChests", &p)) {
		if (PlayerCheck(p)) {
			Json::Value j;
			vector<ItemStack*> is = p->getEnderChestContainer()->getSlots();
			for (auto i : is) {
				j.append(toJson(i->save()));
			}
			return Py_True;
		}
	}
	return Py_False;
}
api_function(setPlayerEnderChests) {
	Player* p;
	const char* x;
	if (PyArg_ParseTuple(args, "Ks:setPlayerEnderChests", &p, &x)) {
		if (PlayerCheck(p)) {
			Json::Value j = toJson(x);
			if (j.isArray()) {
				vector<ItemStack*> is = p->getEnderChestContainer()->getSlots();
				for (unsigned i = 0; i < j.size(); i++) {
					Tag* t = toTag(j[i]);
					is[i]->fromTag(t);
					t->deCompound();
					delete t;
				}
				p->sendInventroy();
				return Py_True;
			}
		}
	}
	return Py_False;
}
// 增加/移除物品
api_function(addItemEx) {
	Player* p;
	const char* x;
	if (PyArg_ParseTuple(args, "Ks:addItemEx", &p, &x)) {
		if (PlayerCheck(p)) {
			Tag* t = toTag(toJson(x));
			ItemStack i;
			i.fromTag(t);
			p->addItem(&i);
			p->sendInventroy();
			t->deCompound();
			delete t;
			return Py_True;
		}
	}
	return Py_False;
}
api_function(removeItem) {
	Player* p; int slot, num;
	if (PyArg_ParseTuple(args, "Kii:removeItem", &p, &slot, &num)) {
		if (PlayerCheck(p)) {
			p->getContainer()->clearItem(slot, num);
			p->sendInventroy();
		}
	}
	return Py_None;
}
// 设置玩家侧边栏
api_function(setSidebar) {
	Player* p;
	const char* title;
	const char* data;
	if (PyArg_ParseTuple(args, "Kss:setSidebar", &p, &title, &data)) {
		if (PlayerCheck(p)) {
			Json::Value j = toJson(data);
			setDisplayObjectivePacket(p, title);
			if (j.isObject()) {
				vector<ScorePacketInfo> info;
				for (auto& x : j.getMemberNames()) {
					ScorePacketInfo o(_scoreboard->createScoreBoardId(x),
						j[x].asInt(), x);
					info.push_back(o);
				}
				SetScorePacket(p, 0, info);
				return Py_True;
			}
		}
	}
	return Py_False;
}
api_function(removeSidebar) {
	Player* p;
	if (PyArg_ParseTuple(args, "K:removeSidebar", &p)) {
		if (PlayerCheck(p)) {
			setDisplayObjectivePacket(p, "", "");
		}
	}
	return Py_None;
}
PyMethodDef api_list[] = {
api_method(logout),
api_method(runcmd),
api_method(setTimer),
api_method(removeTimer),
api_method(setListener),
api_method(setShareData),
api_method(getShareData),
api_method(setCommandDescription),
api_method(getPlayerList),
api_method(sendSimpleForm),
api_method(sendModalForm),
api_method(sendCustomForm),
api_method(transferServer),
api_method(getPlayerInfo),
api_method(getActorInfo),
api_method(getActorInfoEx),
api_method(getPlayerPerm),
api_method(setPlayerPerm),
api_method(addLevel),
api_method(setName),
api_method(getPlayerScore),
api_method(modifyPlayerScore),
api_method(talkAs),
api_method(runcmdAs),
api_method(teleport),
api_method(tellraw),
api_method(tellrawEx),
api_method(setBossBar),
api_method(removeBossBar),
api_method(getScoreBoardId),
api_method(createScoreBoardId),
api_method(setDamage),
api_method(setServerMotd),
api_method(getPlayerItems),
api_method(setPlayerItems),
api_method(getPlayerArmor),
api_method(setPlayerArmor),
api_method(getPlayerHand),
api_method(setPlayerHand),
api_method(getPlayerItem),
api_method(setPlayerItem),
api_method(getPlayerEnderChests),
api_method(setPlayerEnderChests),
api_method(addItemEx),
api_method(removeItem),
api_method(setSidebar),
api_method(removeSidebar),
{}
};
// 模块声明
PyModuleDef api_module = { PyModuleDef_HEAD_INIT, "mc", 0, -1, api_list, 0, 0, 0, 0 };
extern "C" PyObject * mc_init() {
	return PyModule_Create(&api_module);
}
#pragma endregion
#pragma region Hook
Hook(世界Tick, void, MSSYM_B1QA4tickB1AA5LevelB2AAA7UEAAXXZ,
	VA a1, VA a2, VA a3, VA a4) {
	original(a1, a2, a3, a4);
	size_t l = todos.size();
	if (l > 0) {
		for (int i = 0; i < l; i++) {
			todos.front()();
			todos.pop();
		}
	}
	int nHold = PyGILState_Check();   //检测当前线程是否拥有GIL
	PyGILState_STATE gstate;
	if (!nHold)
	{
		gstate = PyGILState_Ensure();   //如果没有GIL，则申请获取GIL
	}
	Py_BEGIN_ALLOW_THREADS;
	Py_BLOCK_THREADS;
	/**************************以下加入需要调用的python脚本代码  Begin***********************/
	if (tick.empty())
		return;
	for (auto& i : tick) {
		if (!i.second.first) {
			i.second.first = i.second.second;
			PyObject_CallFunction(i.first, 0);
		}
		else i.second.first--;
	}
	/**************************以下加入需要调用的python脚本代码  End***********************/
	Py_UNBLOCK_THREADS;
	Py_END_ALLOW_THREADS;
	if (!nHold)
	{
		PyGILState_Release(gstate);    //释放当前线程的GIL
	}
}
Hook(获取指令队列, VA, MSSYM_MD5_3b8fb7204bf8294ee636ba7272eec000,
	VA _this) {
	_cmdqueue = original(_this);
	return _cmdqueue;
}
Hook(获取地图初始化信息, Level*, MSSYM_MD5_96d831b409d1a1984d6ac116f2bd4a55,
	VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7, VA a8, VA a9, VA a10, VA a11, VA a12, VA a13) {
	_level = original(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
	return _level;
}
Hook(获取游戏初始化信息, VA, MSSYM_MD5_9f3b3524a8d04242c33d9c188831f836,
	VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7) {
	_ServerNetworkHandle = f(VA, a3);
	return original(a1, a2, a3, a4, a5, a6, a7);
}
Hook(命令注册, void, MSSYM_B1QA5setupB1AE20ChangeSettingCommandB2AAE22SAXAEAVCommandRegistryB3AAAA1Z,
	VA _this) {
	for (auto& cmd : Command) {
		SYMCALL(MSSYM_MD5_8574de98358ff66b5a913417f44dd706,
			_this, cmd.first.c_str(), cmd.second.c_str(), 0, 0, 0x40);
	}
	original(_this);
}
Hook(计分板, Scoreboard*, MSSYM_B2QQE170ServerScoreboardB2AAA4QEAAB1AE24VCommandSoftEnumRegistryB2AAE16PEAVLevelStorageB3AAAA1Z,
	VA _this, VA a2, VA a3) {
	_scoreboard = (Scoreboard*)original(_this, a2, a3);
	return _scoreboard;
}
Hook(后台输出, VA, MSSYM_MD5_b5f2f0a753fc527db19ac8199ae8f740,
	VA handle, const char* str, VA size) {
	if (handle == STD_COUT_HANDLE) {
		bool res = callpy(u8"后台输出", PyUnicode_FromString(str));
		if (!res)return 0;
	}
	return original(handle, str, size);
}
Hook(后台输入, bool, MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e,
	VA _this, string& cmd) {
	bool res = true;
	/*if (cmd == "pyreload") {
		PyFuncs.clear();
		Py_Finalize();
		void init();
		init();
		return 0;
	}*/
	res = callpy(u8"后台输入", PyUnicode_FromString(cmd.c_str()));
	check_ret(_this, cmd);
}
Hook(加入游戏, void, MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE37AEBVSetLocalPlayerAsInitializedPacketB3AAAA1Z,
	VA _this, VA a2, VA a3) {
	Player* p = SYMCALL<Player*>(MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, a2, *(VA*)(a3 + 16));
	PlayerList[p] = true;
	callpy(u8"加入游戏", PyLong_FromUnsignedLongLong((VA)p));
	original(_this, a2, a3);
}
Hook(离开游戏, void, MSSYM_B2QUE12onPlayerLeftB1AE20ServerNetworkHandlerB2AAE21AEAAXPEAVServerPlayerB3AAUA1NB1AA1Z,
	VA _this, Player* p, char v3) {
	callpy(u8"离开游戏", PyLong_FromUnsignedLongLong((VA)p));
	PlayerList[p] = false;
	PlayerList.erase(p);
	return original(_this, p, v3);
}
Hook(使用物品, bool, MSSYM_B1QA9useItemOnB1AA8GameModeB2AAA4UEAAB1UE14NAEAVItemStackB2AAE12AEBVBlockPosB2AAA9EAEBVVec3B2AAA9PEBVBlockB3AAAA1Z,
	VA _this, ItemStack* item, BlockPos* bp, unsigned __int8 a4, VA a5, Block* b) {
	Player* p = f(Player*, _this + 8);
	short iid = item->getId();
	short iaux = item->mAuxValue;
	string iname = item->getName();
	BlockLegacy* bl = b->getBlockLegacy();
	short bid = bl->getBlockItemID();
	string bn = bl->getBlockName();
	bool res = callpy(u8"使用物品", Py_BuildValue("{s:K,s:i,s:i,s:s,s:s,s:i,s:[i,i,i]}",
		"player", p,
		"itemid", iid,
		"itemaux", iaux,
		"itemname", iname.c_str(),
		"blockname", bn.c_str(),
		"blockid", bid,
		"position", bp->x, bp->y, bp->z
	));
	check_ret(_this, item, bp, a4, a5, b);
}
Hook(放置方块, bool, MSSYM_B1QA8mayPlaceB1AE11BlockSourceB2AAA4QEAAB1UE10NAEBVBlockB2AAE12AEBVBlockPosB2AAE10EPEAVActorB3AAUA1NB1AA1Z,
	BlockSource* _this, Block* b, BlockPos* bp, unsigned __int8 a4, Actor* p, bool _bool) {
	bool res = true;
	if (PlayerCheck(p)) {
		BlockLegacy* bl = b->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		res = callpy(u8"放置方块", Py_BuildValue("{s:K,s:s,s:i,s:[i,i,i]}",
			"player", p,
			"blockname", bn.c_str(),
			"blockid", bid,
			"position", bp->x, bp->y, bp->z
		));
	}
	check_ret(_this, b, bp, a4, p, _bool);
}
Hook(破坏方块, bool, MSSYM_B2QUE20destroyBlockInternalB1AA8GameModeB2AAA4AEAAB1UE13NAEBVBlockPosB2AAA1EB1AA1Z,
	VA _this, BlockPos* bp) {
	Player* p = f(Player*, _this + 8);
	BlockSource* bs = f(BlockSource*, f(VA, _this + 8) + 840);
	Block* b = bs->getBlock(bp);
	BlockLegacy* bl = b->getBlockLegacy();
	short bid = bl->getBlockItemID();
	string bn = bl->getBlockName();
	bool res = true;
	res = callpy(u8"破坏方块", Py_BuildValue("{s:K,s:s,s:i,s:[i,i,i]}",
		"player", p,
		"blockname", bn.c_str(),
		"blockid", bid,
		"position", bp->x, bp->y, bp->z
	));
	check_ret(_this, bp);
}
Hook(打开箱子, bool, MSSYM_B1QA3useB1AE10ChestBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB2AAA1EB1AA1Z,
	VA _this, Player* p, BlockPos* bp) {
	bool res = callpy(u8"打开箱子", Py_BuildValue("{s:K,s:[i,i,i]}",
		"player", p,
		"position", bp->x, bp->y, bp->z
	));
	check_ret(_this, p, bp);
}
Hook(打开木桶, bool, MSSYM_B1QA3useB1AE11BarrelBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB2AAA1EB1AA1Z,
	VA _this, Player* p, BlockPos* bp) {
	bool res = callpy(u8"打开木桶", Py_BuildValue("{s:K,s:[i,i,i]}",
		"player", p,
		"position", bp->x, bp->y, bp->z
	));
	check_ret(_this, p, bp);
}
Hook(关闭箱子, void, MSSYM_B1QA8stopOpenB1AE15ChestBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	VA _this, Player* p) {
	auto bp = (BlockPos*)(_this - 204);
	callpy(u8"关闭箱子", Py_BuildValue("{s:K,s:[i,i,i]}",
		"player", p,
		"position", bp->x, bp->y, bp->z
	));
	original(_this, p);
}
Hook(关闭木桶, void, MSSYM_B1QA8stopOpenB1AE16BarrelBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	VA _this, Player* p) {
	auto bp = (BlockPos*)(_this - 204);
	callpy(u8"关闭木桶", Py_BuildValue("{s:K,s:[i,i,i]}",
		"player", p,
		"position", bp->x, bp->y, bp->z
	));
	original(_this, p);
}
Hook(放入取出, void, MSSYM_B1QE23containerContentChangedB1AE19LevelContainerModelB2AAA6UEAAXHB1AA1Z,
	VA a1, VA slot) {
	VA v3 = f(VA, a1 + 208);// IDA LevelContainerModel::_getContainer line 15 25
	BlockSource* bs = f(BlockSource*, f(VA, v3 + 848) + 88);
	BlockPos* bp = (BlockPos*)(a1 + 216);
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	short bid = bl->getBlockItemID();
	if (bid == 54 || bid == 130 || bid == 146 || bid == -203 || bid == 205 || bid == 218) {	// 非箱子、桶、潜影盒的情况不作处理
		VA v5 = (*(VA(**)(VA))(*(VA*)a1 + 160))(a1);
		if (v5) {
			ItemStack* i = (ItemStack*)(*(VA(**)(VA, VA))(*(VA*)v5 + 40))(v5, slot);
			callpy(u8"放入取出", Py_BuildValue("{s:K,s:s,s:i,s:[i,i,i],s:i,s:i,s:s,s:i,s:i}",
				"player", f(Player*, a1 + 208),
				"blockname", bl->getBlockName().c_str(),
				"blockid", bid,
				"position", bp->x, bp->y, bp->z,
				"itemid", i->getId(),
				"itemcount", i->mCount,
				"itemname", i->getName().c_str(),
				"itemaux", i->mAuxValue,
				"slot", slot
			));
		}
	}
	original(a1, slot);
}
Hook(玩家攻击, bool, MSSYM_B1QA6attackB1AA6PlayerB2AAA4UEAAB1UE10NAEAVActorB3AAAA1Z,
	Player* p, Actor* a) {
	bool res = callpy(u8"玩家攻击", Py_BuildValue("{s:K,s:K}",
		"player", p,
		"actor", a
	));
	check_ret(p, a);
}
Hook(切换维度, bool, MSSYM_B2QUE21playerChangeDimensionB1AA5LevelB2AAA4AEAAB1UE11NPEAVPlayerB2AAE26AEAVChangeDimensionRequestB3AAAA1Z,
	VA _this, Player* p, VA req) {
	bool result = original(_this, p, req);
	if (result) {
		callpy(u8"切换维度", PyLong_FromUnsignedLongLong((VA)p));
	}
	return result;
}
Hook(生物死亡, void, MSSYM_B1QA3dieB1AA3MobB2AAE26UEAAXAEBVActorDamageSourceB3AAAA1Z,
	Mob* _this, VA dmsg) {
	char v72;
	Actor* sa = f(Level*, _this + 856)->fetchEntity(*(VA*)((*(VA(__fastcall**)(VA, char*))(*(VA*)dmsg + 64))(dmsg, &v72)));
	bool res = callpy(u8"生物死亡", Py_BuildValue("{s:I,s:K,s:K}",
		"dmcase", f(unsigned, dmsg + 8),
		"actor1", _this,
		"actor2", sa//可能为0
	));
	if (res) original(_this, dmsg);
}
Hook(生物受伤, bool, MSSYM_B2QUA4hurtB1AA3MobB2AAA4MEAAB1UE22NAEBVActorDamageSourceB2AAA1HB1UA2N1B1AA1Z,
	Mob* _this, VA dmsg, int a3, bool a4, bool a5) {
	_Damage = a3;//将生物受伤的值设置为可调整
	char v72;
	Actor* sa = f(Level*, _this + 856)->fetchEntity(*(VA*)((*(VA(__fastcall**)(VA, char*))(*(VA*)dmsg + 64))(dmsg, &v72)));
	bool res = callpy(u8"生物受伤", Py_BuildValue("{s:i,s:K,s:K,s:i}",
		"dmcase", f(unsigned, dmsg + 8),
		"actor1", _this,
		"actor2", sa,//可能为0
		"damage", a3
	));
	check_ret(_this, dmsg, _Damage, a4, a5);
}
Hook(玩家重生, void, MSSYM_B1QA7respawnB1AA6PlayerB2AAA7UEAAXXZ,
	Player* p) {
	callpy(u8"玩家重生", PyLong_FromUnsignedLongLong((VA)p));
	original(p);
}
Hook(聊天消息, void, MSSYM_MD5_ad251f2fd8c27eb22c0c01209e8df83c,
	VA _this, string& sender, string& target, string& msg, string& style) {
	callpy(u8"聊天消息", Py_BuildValue("{s:s,s:s,s:s,s:s}",
		"sender", sender.c_str(),
		"target", target.c_str(),
		"msg", msg.c_str(),
		"style", style.c_str()
	));
	original(_this, sender, target, msg, style);
}
Hook(输入文本, void, MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE14AEBVTextPacketB3AAAA1Z,
	VA _this, VA id, /*(TextPacket*)*/VA pkt) {
	Player* p = SYMCALL<Player*>(MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, f(char, pkt + 16));
	if (p) {
		string msg = f(string, pkt + 80);
		bool res = callpy(u8"输入文本", Py_BuildValue("{s:K,s:s}",
			"player", p,
			"msg", msg.c_str()
		));
		if (res)original(_this, id, pkt);
	}
}
Hook(输入指令, void, MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE24AEBVCommandRequestPacketB3AAAA1Z,
	VA _this, VA id, /*(CommandRequestPacket*)*/VA pkt) {
	Player* p = SYMCALL<Player*>(MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, f(char, pkt + 16));
	if (p) {
		string cmd = f(string, pkt + 40);
		bool res = callpy(u8"输入指令", Py_BuildValue("{s:K,s:s}",
			"player", p,
			"cmd", cmd.c_str()
		));
		if (res)original(_this, id, pkt);
	}
}
Hook(选择表单, void, MSSYM_MD5_8b7f7560f9f8353e6e9b16449ca999d2,
	VA _this, VA id, VA handle,/*(ModalFormResponsePacket**)*/VA* ppkt) {
	VA pkt = *ppkt;
	Player* p = SYMCALL<Player*>(MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		handle, id, f(char, pkt + 16));
	if (PlayerCheck(p)) {
		unsigned fid = f(unsigned, pkt + 40);
		string data = f(string, pkt + 48);
		if (data.back() == '\n')data.pop_back();
		callpy(u8"选择表单", Py_BuildValue("{s:K,s:s,s:i}",
			"player", p,
			"selected", data.c_str(),
			"formid", fid
		));
	}
	original(_this, id, handle, ppkt);
}
Hook(更新命令方块, void, MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE28AEBVCommandBlockUpdatePacketB3AAAA1Z,
	VA _this, VA id, /*(CommandBlockUpdatePacket*)*/VA pkt) {
	bool res = true;
	Player* p = SYMCALL<Player*>(MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, *((char*)pkt + 16));
	if (PlayerCheck(p)) {
		auto bp = f(BlockPos, pkt + 40);
		auto mode = f(unsigned short, pkt + 52);
		auto condition = f(bool, pkt + 54);
		auto redstone = f(bool, pkt + 55);
		auto cmd = f(string, pkt + 64);
		auto output = f(string, pkt + 96);
		auto rawname = f(string, pkt + 128);
		auto delay = f(int, pkt + 160);
		res = callpy(u8"命令方块更新", Py_BuildValue("{s:K,s:i,s:i,s:i,s:s,s:s,s:s,s:i,s:[i,i,i]}",
			"player", p,
			"mode", mode,
			"condition", condition,
			"redstone", redstone,
			"cmd", cmd.c_str(),
			"output", output.c_str(),
			"rawname", rawname.c_str(),
			"delay", delay,
			"position", bp.x, bp.y, bp.z
		));
	}
	if (res)original(_this, id, pkt);
}
Hook(世界爆炸, bool, MSSYM_B1QA7explodeB1AA5LevelB2AAE20QEAAXAEAVBlockSourceB2AAA9PEAVActorB2AAA8AEBVVec3B2AAA1MB1UA4N3M3B1AA1Z,
	Level* _this, BlockSource* bs, Actor* a3, Vec3 pos, float a5, bool a6, bool a7, float a8, bool a9) {
	bool res = callpy(u8"世界爆炸", Py_BuildValue("{s:K,s:[f,f,f],s:i,s:i}",
		"actor", a3,
		"position", pos.x, pos.y, pos.z,
		"dimensionid", bs->getDimensionId(),
		"power", a5
	));
	check_ret(_this, bs, a3, pos, a5, a6, a7, a8, a9);
}
Hook(命令方块执行, bool, MSSYM_B1QE14performCommandB1AE17CommandBlockActorB2AAA4QEAAB1UE16NAEAVBlockSourceB3AAAA1Z,
	VA _this, BlockSource* a2) {
	/*//脉冲:0,重复:1,链:2
	int mode = SYMCALL<int>(MSSYM_B1QA7getModeB1AE17CommandBlockActorB2AAA4QEBAB1QE19AW4CommandBlockModeB2AAE15AEAVBlockSourceB3AAAA1Z,
		_this, a2);
	//无条件:0,有条件:1
	bool condition = SYMCALL<bool>(MSSYM_B1QE18getConditionalModeB1AE17CommandBlockActorB2AAA4QEBAB1UE16NAEAVBlockSourceB3AAAA1Z,
		_this, a2);
	string cmd = f(string, _this + 264);
	string rawname = f(string, _this + 296);
	BlockPos bp = f(BlockPos, _this + 44);*/
	Tag* t = newTag(Compound);
	SYMCALL<bool>(MSSYM_B1QA4saveB1AE17CommandBlockActorB2AAA4UEBAB1UE16NAEAVCompoundTagB3AAAA1Z,
		_this, t);
	string data = toJson(t).toStyledString();
	t->deCompound();
	delete t;
	bool res = callpy(u8"命令方块执行", PyUnicode_FromString(data.c_str()));
	check_ret(_this, a2);
}
Hook(玩家穿戴, void, MSSYM_B1QA8setArmorB1AA6PlayerB2AAE16UEAAXW4ArmorSlotB2AAE13AEBVItemStackB3AAAA1Z,
	Player* p, unsigned slot, ItemStack* i) {
	if (!i->getId())return original(p, slot, i);
	bool res = callpy(u8"玩家穿戴", Py_BuildValue("{s:K,s:i,s:i,s:s,s:i,s:i}",
		"player", p,
		"itemid", i->getId(),
		"itemcount", i->mCount,
		"itemname", i->getName().c_str(),
		"itemaux", i->mAuxValue,
		"slot", slot
	));
	if (res)original(p, slot, i);
}
Hook(计分板改变, void, MSSYM_B1QE14onScoreChangedB1AE16ServerScoreboardB2AAE21UEAAXAEBUScoreboardIdB2AAE13AEBVObjectiveB3AAAA1Z,
	const Scoreboard* _this, ScoreboardId* a2, Objective* a3) {
	/*
	原命令：
	创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	*/
	int scoreboardid = a2->id;
	callpy(u8"计分板改变", Py_BuildValue("{s:i,s:i,s:s,s:s}",
		"scoreboardid", scoreboardid,
		"playersnum", a3->getPlayerScore(a2)->getCount(),
		"objectivename", a3->getScoreName().c_str(),
		"objectivedisname", a3->getScoreDisplayName().c_str()
	));
	original(_this, a2, a3);
}
Hook(耕地破坏, void, MSSYM_B1QE15transformOnFallB1AA9FarmBlockB2AAE20UEBAXAEAVBlockSourceB2AAE12AEBVBlockPosB2AAA9PEAVActorB2AAA1MB1AA1Z,
	VA _this, BlockSource* a1, BlockPos* a2, Player* p, VA a4) {
	bool res = true;
	if (PlayerCheck(p)) {
		res = callpy(u8"耕地破坏", Py_BuildValue("{s:K,s:[i,i,i],s:i}",
			"player", p,
			"position", a2->x, a2->y, a2->z,
			"dimensionid", a1->getDimensionId()
		));
	}
	if (res)original(_this, a1, a2, p, a4);
}
Hook(使用重生锚, bool, MSSYM_B1QE11trySetSpawnB1AE18RespawnAnchorBlockB2AAA2CAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB2AAE15AEAVBlockSourceB2AAA9AEAVLevelB3AAAA1Z,
	Player* p, BlockPos* a2, BlockSource* a3, void* a4) {
	bool res = true;
	if (PlayerCheck(p)) {
		res = callpy(u8"使用重生锚", Py_BuildValue("{s:K,s:[i,i,i],s:i}",
			"player", p,
			"position", a2->x, a2->y, a2->z,
			"dimensionid", a3->getDimensionId()
		));
	}
	check_ret(p, a2, a3, a4);
}
#pragma endregion
void init() {
	//PyPreConfig cfg;
	//PyPreConfig_InitPythonConfig(&cfg);
	//cfg.utf8_mode = 1;
	//cfg.configure_locale = 0;
	//Py_PreInitialize(&cfg);
	PyImport_AppendInittab("mc", mc_init); //增加一个模块
	Py_Initialize();
	PyEval_InitThreads();
	if (!filesystem::exists("py"))
		filesystem::create_directory("py");
	filesystem::directory_iterator files("py");
	for (auto& info : files) {
		auto& path = info.path();
		if (path.extension() == ".py") {
			const string& name = path.stem().u8string();
			cout << "[BDSpyrunner] loading " << name << endl;
			PyImport_ImportModule(("py." + name).c_str());
			PyErr_Print();
		}
	}
	PyEval_SaveThread();
}
int DllMain(VA, int dwReason, VA) {
	if (dwReason == 1) {
		//while (1) {
		//	Tag* t = toTag(toJson(R"({"a9":["啊这","就这？"]})"));
		//	cout << toJson(t) << endl;
		//	t->deCompound();
		//	delete t;
		//}
		//StructureTemplate st("test");
		//StructureSettings ss;
		//cout << toJson(st.save()) << endl;
		init();
		puts(u8"[BDSpyrunner] v0.2.10 loaded.\n"
			"感谢小枫云(ipyvps.com)对此项目的大力支持\n"
			"Thanks for ipyvps.com strong support for this project");
	}
	return 1;
}