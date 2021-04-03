#include "pch.h"
#include "BDS.hpp"
#include "Event.h"
#include "PyEntityObject.h"
#pragma region Macro
#define PyAPIFunction(name) static PyObject* api_##name(PyObject* , PyObject* args)
#define CheckResult(...) if (!res) return 0; return original(__VA_ARGS__)
//#define isPlayer(ptr) _PlayerList[(Player*)ptr]
#pragma endregion
#pragma region Global variable
static VA _cmdqueue;//指令队列
static ServerNetworkHandler* _Handle;
static Level* _level;
static Scoreboard* _scoreboard;//计分板
static unsigned _formid = 1;//表单ID
static unordered_map<Event, vector<PyObject*>> _PyFuncs;//Py函数表
//static unordered_map<Player*, bool> _PlayerList;//玩家列表
static vector<pair<string, string>> _Command;//注册命令
static unordered_map<string, PyObject*> ShareData;//共享数据
static int _Damage;//伤害值
static unordered_map<PyObject*, unsigned[2]> tick;//执行队列
static queue<function<void()>> _todos;
#pragma endregion
#pragma region Function Define
template<class T>void inline print(const T& data) {
	cout << data << endl;
}
template<class T, class... T2>void inline print(const T& data, T2... other) {
	cout << data;
	print(other...);
}
static inline VA createPacket(const int type) {
	VA pkt[2];
	SYMCALL("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		pkt, type);
	return *pkt;
}
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
static bool EventCall(Event e, PyObject* val) {
	bool result = true;
	safeCall([&] {
		auto& List = _PyFuncs[e];
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
static unsigned ModalFormRequestPacket(Player* p, const string& str) {
	unsigned fid = _formid++;
	if (isPlayer(p)) {
		VA pkt = createPacket(100);
		FETCH(unsigned, pkt + 48) = fid;
		FETCH(string, pkt + 56) = str;
		p->sendPacket(pkt);
	}
	return fid;
}
static bool TransferPacket(Player* p, const string& address, short port) {
	if (isPlayer(p)) {
		VA pkt = createPacket(85);
		FETCH(string, pkt + 48) = address;
		FETCH(short, pkt + 80) = port;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool TextPacket(Player* p, int mode, const string& msg) {
	if (isPlayer(p)) {
		VA pkt = createPacket(9);
		FETCH(int, pkt + 48) = mode;
		FETCH(string, pkt + 56) = p->getNameTag();
		FETCH(string, pkt + 88) = msg;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool CommandRequestPacket(Player* p, const string& cmd) {
	if (isPlayer(p)) {
		VA pkt = createPacket(77);
		FETCH(string, pkt + 48) = cmd;
		SYMCALL<VA>("?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
			_Handle, p->getNetId(), pkt);
		//p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool BossEventPacket(Player* p, string name, float per, int eventtype) {
	if (isPlayer(p)) {
		VA pkt = createPacket(74);
		FETCH(VA, pkt + 56) = FETCH(VA, pkt + 64) = FETCH(VA, p->getUniqueID());
		FETCH(int, pkt + 72) = eventtype;//0显示,1更新,2隐藏,
		FETCH(string, pkt + 80) = name;
		FETCH(float, pkt + 112) = per;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool setDisplayObjectivePacket(Player* p, const string& title, const string& name = "name") {
	if (isPlayer(p)) {
		VA pkt = createPacket(107);
		FETCH(string, pkt + 48) = "sidebar";
		FETCH(string, pkt + 80) = name;
		FETCH(string, pkt + 112) = title;
		FETCH(string, pkt + 144) = "dummy";
		FETCH(char, pkt + 176) = 0;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
static bool SetScorePacket(Player* p, char type, const vector<ScorePacketInfo>& slot) {
	if (isPlayer(p)) {
		VA pkt = createPacket(108);
		FETCH(char, pkt + 48) = type;//{set,remove}
		FETCH(vector<ScorePacketInfo>, pkt + 56) = slot;
		p->sendPacket(pkt);
		return true;
	}
	return false;
}
#pragma endregion
#pragma region Hook List
HOOK(Level_tick, void, "?tick@Level@@UEAAXXZ",
	Level* _this) {
	original(_this);
	//执行todos函数
	if (!_todos.empty()) {
		for (int i = 0; i < _todos.size(); i++) {
			_todos.front()();
			_todos.pop();
		}
	}
	if (!tick.empty()) {
		safeCall([] {
			for (auto& i : tick) {
				if (!i.second[0]) {
					i.second[0] = i.second[1];
					PyObject_CallFunction(i.first, nullptr);
				}
				else i.second[0]--;
			}
			}
		);
	}
}
HOOK(SPSCQueue, VA, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z",
	VA _this) {
	_cmdqueue = original(_this);
	return _cmdqueue;
}
HOOK(Level_Level, Level*, "??0Level@@QEAA@AEBV?$not_null@V?$NonOwnerPointer@VSoundPlayerInterface@@@Bedrock@@@gsl@@V?$unique_ptr@VLevelStorage@@U?$default_delete@VLevelStorage@@@std@@@std@@V?$unique_ptr@VLevelLooseFileStorage@@U?$default_delete@VLevelLooseFileStorage@@@std@@@4@AEAVIMinecraftEventing@@_NEAEAVScheduler@@V?$not_null@V?$NonOwnerPointer@VStructureManager@@@Bedrock@@@2@AEAVResourcePackManager@@AEAVIEntityRegistryOwner@@V?$unique_ptr@VBlockComponentFactory@@U?$default_delete@VBlockComponentFactory@@@std@@@4@V?$unique_ptr@VBlockDefinitionGroup@@U?$default_delete@VBlockDefinitionGroup@@@std@@@4@@Z",
	VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7, VA a8, VA a9, VA a10, VA a11, VA a12, VA a13) {
	_level = original(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
	return _level;
}
HOOK(ServerNetworkHandler_ServerNetworkHandler, VA, "??0ServerNetworkHandler@@QEAA@AEAVGameCallbacks@@AEAVLevel@@AEAVNetworkHandler@@AEAVPrivateKeyManager@@AEAVServerLocator@@AEAVPacketSender@@AEAVAllowList@@PEAVPermissionsFile@@AEBVUUID@mce@@H_NAEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@HAEAVMinecraftCommands@@AEAVIMinecraftApp@@AEBV?$unordered_map@UPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$hash@UPackIdVersion@@@3@U?$equal_to@UPackIdVersion@@@3@V?$allocator@U?$pair@$$CBUPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@@3@@std@@AEAVScheduler@@V?$NonOwnerPointer@VTextFilteringProcessor@@@Bedrock@@@Z",
	ServerNetworkHandler* _this, VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7, VA a8, VA a9, VA a10, VA a11, VA a12, VA a13, VA a14, VA a15, VA a16, VA a17, VA a18, VA a19) {
	_Handle = _this;
	return original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19);
}
HOOK(ChangeSettingCommand_setup, void, "?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",//"?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",?setup@KillCommand@@SAXAEAVCommandRegistry@@@Z
	VA _this) {
	for (auto& cmd : _Command) {
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
		bool res = EventCall(Event::onConsoleOutput, PyUnicode_FromStringAndSize(str, size));
		if (!res)return _this;
	}
	return original(_this, str, size);
}
//HOOK(onConsoleOutput2, int, "printf",
//	const char* format, va_list va) {
//	bool res = EventCall(Event::onConsoleOutput, PyUnicode_FromString(va));
//	CheckResult(format, va);
//}
HOOK(onConsoleInput, bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	VA _this, const string& cmd) {
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
		//_PlayerList[p] = true;
		//EventCall(Event::onPlayerJoin, PyLong_FromUnsignedLongLong((VA)p));
		EventCall(Event::onPlayerJoin, PyEntity_FromEntity(p));
	}
	original(_this, id, pkt);
}
HOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	VA _this, Player* p, char v3) {
	EventCall(Event::onPlayerLeft, PyEntity_FromEntity(p));
	//_PlayerList.erase(p);
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
	bool res = EventCall(Event::onUseItem,
		Py_BuildValue("{s:O,s:i,s:i,s:s,s:s,s:i,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"itemid", iid,
			"itemaux", iaux,
			"itemname", iname.c_str(),
			"blockname", bn.c_str(),
			"blockid", bid,
			"position", bp->x, bp->y, bp->z
		)
	);
	CheckResult(_this, item, bp, a4, a5, b);
}
HOOK(onPlaceBlock, bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* _this, Block* b, BlockPos* bp, unsigned char a4, Actor* p, bool _bool) {
	bool res = true;
	if (isPlayer(p)) {
		BlockLegacy* bl = b->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		res = EventCall(Event::onPlaceBlock,
			Py_BuildValue("{s:O,s:s,s:i,s:[i,i,i]}",
				"player", PyEntity_FromEntity(p),
				"blockname", bn.c_str(),
				"blockid", bid,
				"position", bp->x, bp->y, bp->z
			)
		);
	}
	CheckResult(_this, b, bp, a4, p, _bool);
}
HOOK(onDestroyBlock, bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource* _this, Actor* p, BlockPos* bp, ItemStack* a3, bool a4) {
	//BlockPos size = { 5,5,5 };
	//StructureSettings ss(&size, 0, 0);
	//StructureTemplate st("tmp");
	//StructureTemplate st2("tmp2");
	//st.fillFromWorld(_this, bp, &ss);
	//ofstream fout("s.json");
	//fout.clear();
	//Json::Value v(toJson(st.save()));
	//fout << v << endl;
	//st2.fromJson(v);
	//Json::Value v2(toJson(st2.save()));
	//fout << v2 << endl;
	//cout << (v2 == v) << endl;
	//st2.placeInWorld(_this, _level->getBlockPalette(), bp, &ss);
	//st.placeInWorld(_this, _level->getBlockPalette(), bp, &ss);
	BlockLegacy* bl = _this->getBlock(bp)->getBlockLegacy();
	short bid = bl->getBlockItemID();
	string bn = bl->getBlockName();
	bool res = EventCall(Event::onDestroyBlock,
		Py_BuildValue("{s:O,s:s,s:i,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"blockname", bn.c_str(),
			"blockid", (int)bid,
			"position", bp->x, bp->y, bp->z
		)
	);
	CheckResult(_this, p, bp, a3, a4);
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
		EventCall(Event::onChangeDimension, PyLong_FromUnsignedLongLong((VA)p));
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
	_Damage = a3;//将生物受伤的值设置为可调整
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
	CheckResult(_this, dmsg, _Damage, a4, a5);
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
		Py_BuildValue("{s:O,s:[f,f,f],s:i,s:i}",
			"actor", a3,
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
	if (!i->getId())return original(p, slot, i);
	bool res = EventCall(Event::onSetArmor,
		Py_BuildValue("{s:O,s:i,s:i,s:s,s:i,s:i}",
			"player", PyEntity_FromEntity(p),
			"itemid", i->getId(),
			"itemcount", i->mCount,
			"itemname", i->getName().c_str(),
			"itemaux", i->mAuxValue,
			"slot", slot
		)
	);
	if (res)original(p, slot, i);
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
	VA _this, BlockSource* a1, BlockPos* a2, Player* p, VA a4) {
	bool res = true;
	if (isPlayer(p)) {
		res = EventCall(Event::onFallBlockTransform,
			Py_BuildValue("{s:O,s:[i,i,i],s:i}",
				"player", PyEntity_FromEntity(p),
				"position", a2->x, a2->y, a2->z,
				"dimensionid", a1->getDimensionId()
			)
		);
	}
	if (res)original(_this, a1, a2, p, a4);
}
HOOK(onUseRespawnAnchorBlock, bool, "?trySetSpawn@RespawnAnchorBlock@@CA_NAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* p, BlockPos* a2, BlockSource* a3, void* a4) {
	bool res = true;
	if (isPlayer(p)) {
		res = EventCall(Event::onUseRespawnAnchorBlock,
			Py_BuildValue("{s:O,s:[i,i,i],s:i}",
				"player", PyEntity_FromEntity(p),
				"position", a2->x, a2->y, a2->z,
				"dimensionid", a3->getDimensionId()
			)
		);
	}
	CheckResult(p, a2, a3, a4);
}
HOOK(onPistonPush, bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z",
	BlockActor* _this, BlockSource* bs, BlockPos* bp, unsigned a3, unsigned a4) {
	auto blg = bs->getBlock(bp)->getBlockLegacy();
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
static PyObject* api_getVersion(PyObject*, PyObject*) {
	return PyLong_FromLong(130);
}
//指令输出
static PyObject* api_logout(PyObject*, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "s:logout", &msg)) {
		SYMCALL<ostream&>("??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
			&cout, msg, strlen(msg));
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//执行指令
static PyObject* api_runcmd(PyObject*, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:runcmd", &cmd)) {
		onConsoleInput::original(_cmdqueue, cmd);
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//计时器
static PyObject* api_setTimer(PyObject*, PyObject* args) {
	unsigned time; PyObject* func;
	if (PyArg_ParseTuple(args, "OI:setTimer", &func, &time)) {
		if (!PyCallable_Check(func))
			Py_RETURN_NONE;
		tick[func][0] = time;
		tick[func][1] = time;
	}
	Py_RETURN_NONE;
}
static PyObject* api_removeTimer(PyObject*, PyObject* args) {
	PyObject* func;
	if (PyArg_ParseTuple(args, "O:removeTimer", &func)) {
		tick.erase(func);
	}
	Py_RETURN_NONE;
}
//延时执行一次函数（x）
#if 0
static PyObject* api_setTimeout(PyObject*, PyObject* args) {
	PyObject* func; unsigned time;
	if (PyArg_ParseTuple(args, "OI:setTimeout", &func, &time)) {
		safeCall([&func, &time] {
			thread([&] {
				Sleep(time);
				PyObject_CallFunction(func, nullptr);
				}
			).detach();
			}
		);
	}
	Py_RETURN_NONE;
}
#endif
//设置监听
static PyObject* api_setListener(PyObject*, PyObject* args) {
	const char* name = ""; PyObject* fn;
	if (PyArg_ParseTuple(args, "sO:setListener", &name, &fn)) {
		try {
			Event e = toEvent(name);
			_PyFuncs[e].push_back(fn);
			Py_RETURN_NONE;
		}
		catch (const std::out_of_range&) {
			cerr << "无效的监听:" << name << endl;
		}
	}
	Py_RETURN_NONE;
}
//共享数据
static PyObject* api_setShareData(PyObject*, PyObject* args) {
	const char* index = ""; PyObject* data;
	if (PyArg_ParseTuple(args, "sO:setShareData", &index, &data)) {
		ShareData.emplace(index, data);
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
static PyObject* api_getShareData(PyObject*, PyObject* args) {
	const char* index = "";
	if (PyArg_ParseTuple(args, "s:getShareData", &index)) {
		return ShareData[index];
	}
	Py_RETURN_NONE;
}
//设置指令说明
static PyObject* api_setCommandDescription(PyObject*, PyObject* args) {
	const char* cmd = "", * des = "";
	if (PyArg_ParseTuple(args, "ss:setCommandDescribe", &cmd, &des)) {
		_Command.push_back({ cmd,des });
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
static PyObject* api_getPlayerList(PyObject*, PyObject*) {
	PyObject* list = PyList_New(0);
	for (auto& p : _level->getAllPlayers()) {
		PyList_Append(list, PyLong_FromUnsignedLongLong((VA)p));
	}
	return list;
}
//发送表单
static PyObject* api_sendCustomForm(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	const char* str = "";
	if (PyArg_ParseTuple(args, "Os:sendCustomForm", &obj, &str)) {
		return PyLong_FromLong(ModalFormRequestPacket(obj->asPlayer(), str));
	}
	return PyLong_FromLong(0);
}
static PyObject* api_sendSimpleForm(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	const char* title = "", * content = "", * buttons = "";
	if (PyArg_ParseTuple(args, "Osss:sendSimpleForm", &obj, &title, &content, &buttons)) {
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})", title, content, buttons);
		return PyLong_FromLong(ModalFormRequestPacket(obj->asPlayer(), str));
	}
	Py_RETURN_NONE;
}
static PyObject* api_sendModalForm(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	const char* title = "", * content = "", * button1 = "", * button2 = "";
	if (PyArg_ParseTuple(args, "Ossss:sendModalForm", &obj, &title, &content, &button1, &button2)) {
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","button1":"%s","button2":"%s","type":"modal"})", title, content, button1, button2);
		return PyLong_FromLong(ModalFormRequestPacket(obj->asPlayer(), str));
	}
	Py_RETURN_NONE;
}
//跨服传送
static PyObject* api_transferServer(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	const char* address = ""; short port;
	if (PyArg_ParseTuple(args, "Osh:transferServer", &obj, &address, &port)) {
		if (TransferPacket(obj->asPlayer(), address, port))
			Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//获取玩家信息
static PyObject* api_getPlayerInfo(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:getPlayerInfo", &obj)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
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
static PyObject* api_getActorInfo(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:getActorInfo", &obj)) {
		Actor* a = obj->asPlayer();
		if (a) {
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
	}
	Py_RETURN_NONE;
}
static PyObject* api_getActorInfoEx(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:getActorInfoEx", &obj)) {
		Actor* a = obj->asPlayer();
		if (a) {
			Tag* t = a->save();
			string info = toJson(t).toStyledString();
			t->deCompound();
			delete t;
			return PyUnicode_FromStringAndSize(info.c_str(), info.length());
		}
	}
	Py_RETURN_NONE;
}
//玩家权限
static PyObject* api_getPlayerPerm(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:getPlayerPerm", &obj)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			return PyLong_FromLong(p->getPermissions());
		}
	}
	Py_RETURN_NONE;
}
static PyObject* api_setPlayerPerm(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	unsigned char lv;
	if (PyArg_ParseTuple(args, "Ob:setPlayerPerm", &obj, &lv)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			p->setPermissionLevel(lv);
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
//增加玩家等级
static PyObject* api_addLevel(PyObject*, PyObject* args) {
	PyEntityObject* obj;
	int lv;
	if (PyArg_ParseTuple(args, "Oi:addLevel", &obj, &lv)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			p->addLevel(lv);
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
//设置玩家名字
PyAPIFunction(setName) {
	PyEntityObject* obj;
	const char* name = "";
	if (PyArg_ParseTuple(args, "Os:setName", &obj, &name)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			p->setName(name);
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
//玩家分数
PyAPIFunction(getPlayerScore) {
	PyEntityObject* obj;
	const char* objname = "";
	if (PyArg_ParseTuple(args, "Os:getPlayerScore", &obj, &objname)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			Objective* testobj = _scoreboard->getObjective(objname);
			if (testobj) {
				auto id = _scoreboard->getScoreboardId(p);
				auto score = testobj->getPlayerScore(id);
				return PyLong_FromLong(score->getCount());
			}
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(modifyPlayerScore) {
	PyEntityObject* obj;
	const char* objname = ""; int count; int mode;
	if (PyArg_ParseTuple(args, "Osii:modifyPlayerScore", &obj, &obj, &count, &mode)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p) && mode >= 0 && mode <= 3) {
			Objective* testobj = _scoreboard->getObjective(objname);
			if (testobj) {
				//mode:{set,add,remove}
				_scoreboard->modifyPlayerScore((ScoreboardId*)_scoreboard->getScoreboardId(p), testobj, count, mode);
				Py_RETURN_NONE;
			}
		}
	}
	Py_RETURN_NONE;
}
//模拟玩家发送文本
PyAPIFunction(talkAs) {
	PyEntityObject* obj;
	const char* msg = "";
	if (PyArg_ParseTuple(args, "Os:talkAs", &obj, &msg)) {
		Player* p = obj->asPlayer();
		if (TextPacket(p, 1, msg))
			Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//模拟玩家执行指令
PyAPIFunction(runcmdAs) {
	PyEntityObject* obj;
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "Os:runcmdAs", &obj, &cmd)) {
		Player* p = obj->asPlayer();
		if (CommandRequestPacket(p, cmd))
			Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//传送玩家
PyAPIFunction(teleport) {
	PyEntityObject* obj;
	float x, y, z; int did;
	if (PyArg_ParseTuple(args, "Offfi:teleport", &obj, &x, &y, &z, &did)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			p->teleport({ x,y,z }, did);
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
//原始输出
PyAPIFunction(tellraw) {
	PyEntityObject* obj;
	const char* msg = "";
	if (PyArg_ParseTuple(args, "Os:tellraw", &obj, &msg)) {
		Player* p = obj->asPlayer();
		if (TextPacket(p, 0, msg))
			Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
PyAPIFunction(tellrawEx) {
	const char* msg = "";
	PyEntityObject* obj;
	int mode;
	if (PyArg_ParseTuple(args, "Osi:tellrawEx", &obj, &msg, &mode)) {
		Player* p = obj->asPlayer();
		if (TextPacket(p, mode, msg))
			Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//boss栏
PyAPIFunction(setBossBar) {
	PyEntityObject* obj;
	const char* name = ""; float per;
	if (PyArg_ParseTuple(args, "Osf:", &obj, &name, &per)) {
		Player* p = obj->asPlayer();
		if (BossEventPacket(p, name, per, 0))
			Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
PyAPIFunction(removeBossBar) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:removeBossBar", &obj)) {
		Player* p = obj->asPlayer();
		if (BossEventPacket(p, "", 0.0, 2))
			Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//修改生物受伤的伤害值!
PyAPIFunction(setDamage) {
	int a;
	if (PyArg_ParseTuple(args, "i:setDamage", &a)) {
		_Damage = a;
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
PyAPIFunction(setServerMotd) {
	const char* n;
	if (PyArg_ParseTuple(args, "s:setServerMotd", &n)) {
		_todos.push([n] {
			if (_Handle) {
				string name(n);
				SYMCALL<VA>("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
					_Handle, &name, true);
			}
			});
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//玩家背包相关操作
PyAPIFunction(getPlayerItems) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:getPlayerItems", &obj)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			Json::Value j;
			for (auto& i : p->getContainer()->getSlots()) {
				j.append(toJson(i->save()));
			}
			string str = j.toStyledString();
			return PyUnicode_FromStringAndSize(str.c_str(), str.length());
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(setPlayerItems) {
	PyEntityObject* obj;
	const char* x = "";
	if (PyArg_ParseTuple(args, "Os:setPlayerItems", &obj, &x)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			Json::Value j = toJson(x);
			if (j.isArray()) {
				vector<ItemStack*> is = p->getContainer()->getSlots();
				for (unsigned i = 0; i < j.size(); i++) {
					is[i]->fromJson(j[i]);
				}
				p->sendInventroy();
				Py_RETURN_NONE;
			}
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(getPlayerHand) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:getPlayerHand", &obj)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			ItemStack* item = p->getSelectedItem();
			string str = toJson(item->save()).toStyledString();
			return PyUnicode_FromStringAndSize(str.c_str(), str.length());
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(setPlayerHand) {
	PyEntityObject* obj;
	const char* x = "";
	if (PyArg_ParseTuple(args, "Os:setPlayerHand", &obj, &x)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			p->getSelectedItem()->fromJson(toJson(x));
			p->sendInventroy();
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(getPlayerArmor) {
	PyEntityObject* obj;
	int slot;
	if (PyArg_ParseTuple(args, "Oi:getPlayerArmor", &obj, &slot)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p) && slot >= 0 && slot <= 4) {
			ItemStack* item = p->getArmor(slot);
			string str = toJson(item->save()).toStyledString();
			return PyUnicode_FromStringAndSize(str.c_str(), str.length());
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(setPlayerArmor) {
	PyEntityObject* obj;
	int slot; const char* x = "";
	if (PyArg_ParseTuple(args, "Ois:setPlayerArmor", &obj, &slot, &x)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p) && slot >= 0 && slot <= 4) {
			p->getArmor(slot)->fromJson(toJson(x));
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(getPlayerItem) {
	PyEntityObject* obj;
	int slot;
	if (PyArg_ParseTuple(args, "Oi:getPlayerItem", &obj, &slot)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			ItemStack* item = p->getInventoryItem(slot);
			string str = toJson(item->save()).toStyledString();
			return PyUnicode_FromStringAndSize(str.c_str(), str.length());
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(setPlayerItem) {
	PyEntityObject* obj;
	int slot; const char* x = "";
	if (PyArg_ParseTuple(args, "Ois:setPlayerItem", &obj, &slot, &x)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p) && slot >= 0 && slot <= 36) {
			p->getInventoryItem(slot)->fromJson(toJson(x));
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(getPlayerEnderChests) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:getPlayerEnderChests", &obj)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			Json::Value j;
			vector<ItemStack*> is = p->getEnderChestContainer()->getSlots();
			for (auto i : is) {
				j.append(toJson(i->save()));
			}
			string str = j.toStyledString();
			return PyUnicode_FromStringAndSize(str.c_str(), str.length());
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(setPlayerEnderChests) {
	PyEntityObject* obj;
	const char* x = "";
	if (PyArg_ParseTuple(args, "Os:setPlayerEnderChests", &obj, &x)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			Json::Value j = toJson(x);
			if (j.isArray()) {
				vector<ItemStack*> is = p->getEnderChestContainer()->getSlots();
				for (unsigned i = 0; i < j.size(); i++) {
					is[i]->fromJson(j[i]);
				}
				p->sendInventroy();
				Py_RETURN_NONE;
			}
		}
	}
	Py_RETURN_NONE;
}
//增加/移除物品
PyAPIFunction(addItemEx) {
	PyEntityObject* obj;
	const char* x = "";
	if (PyArg_ParseTuple(args, "Os:addItemEx", &obj, &x)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			ItemStack i;
			i.fromJson(toJson(x));
			p->addItem(&i);
			p->sendInventroy();
			Py_RETURN_NONE;
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(removeItem) {
	PyEntityObject* obj;
	int slot, num;
	if (PyArg_ParseTuple(args, "Oii:removeItem", &obj, &slot, &num)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			p->getContainer()->clearItem(slot, num);
			p->sendInventroy();
		}
	}
	Py_RETURN_NONE;
}
//设置玩家侧边栏✓
PyAPIFunction(setSidebar) {
	PyEntityObject* obj;
	const char* title = "", * data = "";
	if (PyArg_ParseTuple(args, "Oss:setSidebar", &obj, &title, &data)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			setDisplayObjectivePacket(p, title);
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
				SetScorePacket(p, 0, info);
				Py_RETURN_NONE;
			}
		}
	}
	Py_RETURN_NONE;
}
PyAPIFunction(removeSidebar) {
	PyEntityObject* obj;
	if (PyArg_ParseTuple(args, "O:removeSidebar", &obj)) {
		Player* p = obj->asPlayer();
		if (isPlayer(p)) {
			setDisplayObjectivePacket(p, "", "");
		}
	}
	Py_RETURN_NONE;
}
//根据坐标设置方块
static PyObject* api_getBlock(PyObject*, PyObject* args) {
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "iiii:getBlock", &bp.x, &bp.y, &bp.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			cerr << "未知纬度ID:" << did << endl;
			Py_RETURN_NONE;
		}
		auto bl = bs->getBlock(&bp)->getBlockLegacy();
		return Py_BuildValue("{s:s:s:i}",
			"blockname", bl->getBlockName().c_str(),
			"blockid", (int)bl->getBlockItemID()
		);
	}
	Py_RETURN_NONE;
}
static PyObject* api_setBlock(PyObject*, PyObject* args) {
	const char* name = "";
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "siiii:setBlock", &name, &bp.x, &bp.y, &bp.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			cerr << "未知纬度ID:" << did << endl;
			Py_RETURN_NONE;
		}
		bs->setBlock(name, bp);
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
//获取一个结构
static PyObject* api_getStructure(PyObject*, PyObject* args) {
	BlockPos pos1, pos2; int did;
	if (PyArg_ParseTuple(args, "iiiiiii:getStructure",
		&pos1.x, &pos1.y, &pos1.z,
		&pos2.x, &pos2.y, &pos2.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			cerr << "未知纬度ID:" << did << endl;
			Py_RETURN_NONE;
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
static PyObject* api_setStructure(PyObject*, PyObject* args) {
	const char* data = "";
	BlockPos pos; int did;
	if (PyArg_ParseTuple(args, "siiii:setStructure",
		&data, &pos.x, &pos.y, &pos.z, &did)) {
		auto bs = _level->getBlockSource(did);
		if (!bs) {
			cerr << "未知纬度ID:" << did << endl;
			Py_RETURN_NONE;
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
		Py_RETURN_NONE;
	}
	Py_RETURN_NONE;
}
#pragma endregion
#pragma region New API
//获取/设置玩家所有物品
static PyObject* PyEntity_GetAllItem(PyEntityObject* self, PyObject*) {
	Player* p = self->asPlayer();
	if (isPlayer(p)) {
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
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SetAllItem(PyEntityObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:setAllItem", &x)) {
		Player* p = self->asPlayer();
		if (isPlayer(p)) {
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
	}
	Py_RETURN_NONE;
}
//增加/移除物品
static PyObject* PyEntity_AddItem(PyEntityObject* self, PyObject* args) {
	const char* x = "";
	if (PyArg_ParseTuple(args, "s:addItem", &x)) {
		Player* p = self->asPlayer();
		if (isPlayer(p)) {
			ItemStack i;
			i.fromJson(toJson(x));
			p->addItem(&i);
			p->sendInventroy();
		}
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveItem(PyEntityObject* self, PyObject* args) {
	int slot, num;
	if (PyArg_ParseTuple(args, "ii:removeItem", &slot, &num)) {
		Player* p = self->asPlayer();
		if (isPlayer(p)) {
			p->getContainer()->clearItem(slot, num);
			p->sendInventroy();
		}
	}
	Py_RETURN_NONE;
}
//传送
static PyObject* PyEntity_Teleport(PyEntityObject* self, PyObject* args) {
	Vec3 pos; int did;
	if (PyArg_ParseTuple(args, "fffi:teleport", &pos.x, &pos.y, &pos.z, &did)) {
		Player* p = self->asPlayer();
		if (isPlayer(p)) {
			p->teleport(pos, did);
		}
	}
	Py_RETURN_NONE;
}
//发送数据包
static PyObject* PyEntity_SendTextPacket(PyEntityObject* self, PyObject* args) {
	const char* msg = "";
	int mode;
	if (PyArg_ParseTuple(args, "si:sendTextPacket", &msg, &mode)) {
		TextPacket(self->asPlayer(), mode, msg);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendCommandPacket(PyEntityObject* self, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:sendCommandPacket", &cmd)) {
		CommandRequestPacket(self->asPlayer(), cmd);
	}
	Py_RETURN_NONE;
}
//计分板操作
static PyObject* PyEntity_GetScore(PyEntityObject* self, PyObject* args) {
	const char* objname = "";
	if (PyArg_ParseTuple(args, "s:getScore", &objname)) {
		Player* p = self->asPlayer();
		if (isPlayer(p)) {
			Objective* testobj = _scoreboard->getObjective(objname);
			if (testobj) {
				auto id = _scoreboard->getScoreboardId(p);
				auto score = testobj->getPlayerScore(id);
				return PyLong_FromLong(score->getCount());
			}
		}
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_ModifyScore(PyEntityObject* self, PyObject* args) {
	const char* objname = ""; int count; int mode;
	if (PyArg_ParseTuple(args, "sii:modifyScore", &objname, &count, &mode)) {
		Player* p = self->asPlayer();
		if (isPlayer(p) && mode >= 0 && mode <= 3) {
			Objective* testobj = _scoreboard->getObjective(objname);
			if (testobj) {
				//mode:{set,add,remove}
				_scoreboard->modifyPlayerScore((ScoreboardId*)_scoreboard->getScoreboardId(p), testobj, count, mode);
			}
		}
	}
	Py_RETURN_NONE;
}
//增加等级
static PyObject* PyEntity_AddLevel(PyEntityObject* self, PyObject* args) {
	int level;
	if (PyArg_ParseTuple(args, "i:addLevel", &level)) {
		Player* p = self->asPlayer();
		if (isPlayer(p)) {
			p->addLevel(level);
		}
	}
	Py_RETURN_NONE;
}
//跨服传送
static PyObject* PyEntity_TransferServer(PyEntityObject* self, PyObject* args) {
	const char* address = "";
	short port;
	if (PyArg_ParseTuple(args, "sh:transferServer", &address, &port)) {
		TransferPacket(self->asPlayer(), address, port);
	}
	Py_RETURN_NONE;
}
//发送表单
static PyObject* PyEntity_SendCustomForm(PyEntityObject* self, PyObject* args) {
	const char* str = "";
	if (PyArg_ParseTuple(args, "s:sendCustomForm", &str)) {
		return PyLong_FromLong(ModalFormRequestPacket(self->asPlayer(), str));
	}
	return PyLong_FromLong(0);
}
static PyObject* PyEntity_SendSimpleForm(PyEntityObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* buttons = "";
	if (PyArg_ParseTuple(args, "sss:sendSimpleForm", &title, &content, &buttons)) {
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})", title, content, buttons);
		return PyLong_FromLong(ModalFormRequestPacket(self->asPlayer(), str));
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_SendModalForm(PyEntityObject* self, PyObject* args) {
	const char* title = "";
	const char* content = "";
	const char* button1 = "";
	const char* button2 = "";
	if (PyArg_ParseTuple(args, "ssss:sendModalForm", &title, &content, &button1, &button2)) {
		char str[4096];
		sprintf_s(str, 4096, R"({"title":"%s","content":"%s","button1":"%s","button2":"%s","type":"modal"})", title, content, button1, button2);
		return PyLong_FromLong(ModalFormRequestPacket(self->asPlayer(), str));
	}
	Py_RETURN_NONE;
}
//设置侧边栏
static PyObject* PyEntity_SetSidebar(PyEntityObject* self, PyObject* args) {
	const char* title = "";
	const char* data = "";
	if (PyArg_ParseTuple(args, "ss:setSidebar", &title, &data)) {
		setDisplayObjectivePacket(self->asPlayer(), title);
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
			SetScorePacket(self->asPlayer(), 0, info);
		}
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveSidebar(PyEntityObject* self, PyObject*) {
	setDisplayObjectivePacket(self->asPlayer(), "", "");
	Py_RETURN_NONE;
}
//Boss栏
static PyObject* PyEntity_SetBossbar(PyEntityObject* self, PyObject* args) {
	const char* name = ""; float per;
	if (PyArg_ParseTuple(args, "sf:setBossBar", &name, &per)) {
		BossEventPacket(self->asPlayer(), name, per, 0);
	}
	Py_RETURN_NONE;
}
static PyObject* PyEntity_RemoveBossbar(PyEntityObject* self, PyObject*) {
	BossEventPacket(self->asPlayer(), "", 0, 2);
	Py_RETURN_NONE;
}
//玩家方法
PyMethodDef PyEntity_Methods[]
{
	{"getAllItem", (PyCFunction)PyEntity_GetAllItem, 4, nullptr},
	{"setAllItem", (PyCFunction)PyEntity_SetAllItem, 1, nullptr},
	{"addItem", (PyCFunction)PyEntity_AddItem, 1, nullptr},
	{"removeItem", (PyCFunction)PyEntity_RemoveItem, 1, nullptr},
	{"teleport", (PyCFunction)PyEntity_Teleport, 1, nullptr},
	{"sendTextPacket", (PyCFunction)PyEntity_SendTextPacket, 1, nullptr},
	{"sendCommandPacket", (PyCFunction)PyEntity_SendCommandPacket, 1, nullptr},
	{"getScore", (PyCFunction)PyEntity_GetScore, 1, nullptr},
	{"modifyScore", (PyCFunction)PyEntity_ModifyScore, 1, nullptr},
	{"addLevel", (PyCFunction)PyEntity_AddLevel, 1, nullptr},
	{"transferServer", (PyCFunction)PyEntity_TransferServer, 1, nullptr},
	{"sendCustomForm", (PyCFunction)PyEntity_SendCustomForm, 1, nullptr},
	{"sendCustomForm", (PyCFunction)PyEntity_SendCustomForm, 1, nullptr},
	{"sendSimpleForm", (PyCFunction)PyEntity_SendSimpleForm, 1, nullptr},
	{"sendModalForm", (PyCFunction)PyEntity_SendModalForm, 1, nullptr},
	{"setSidebar", (PyCFunction)PyEntity_SetSidebar, 1, nullptr},
	{"removeSidebar", (PyCFunction)PyEntity_RemoveSidebar, 4, nullptr},
	{"setBossbar", (PyCFunction)PyEntity_SetBossbar, 1, nullptr},
	{"removeBossbar", (PyCFunction)PyEntity_RemoveBossbar, 4, nullptr},
	{nullptr}
};
#pragma endregion
#pragma region Function List
static PyMethodDef api_list[]
{
	{"getVersion", api_getVersion, 4, 0},
	{"logout", api_logout, 1, 0},
	{"runcmd", api_runcmd, 1, 0},
	{"setTimer", api_setTimer, 1, 0},
	{"removeTimer", api_removeTimer, 1, 0},
	//{"setTimeout", api_setTimeout, 1, 0},
	{"setListener", api_setListener, 1, 0},
	{"setShareData", api_setShareData, 1, 0},
	{"getShareData", api_getShareData, 1, 0},
	{"setCommandDescription", api_setCommandDescription, 1, 0},
	{"getPlayerList", api_getPlayerList, 4, 0},
	{"sendSimpleForm", api_sendSimpleForm, 1, 0},
	{"sendModalForm", api_sendModalForm, 1, 0},
	{"sendCustomForm", api_sendCustomForm, 1, 0},
	{"transferServer", api_transferServer, 1, 0},
	{"getPlayerInfo", api_getPlayerInfo, 1, 0},
	{"getActorInfo", api_getActorInfo, 1, 0},
	{"getActorInfoEx", api_getActorInfoEx, 1, 0},
	{"getPlayerPerm", api_getPlayerPerm, 1, 0},
	{"setPlayerPerm", api_setPlayerPerm, 1, 0},
	{"addLevel", api_addLevel, 1, 0},
	{"setName", api_setName, 1, 0},
	{"getPlayerScore", api_getPlayerScore, 1, 0},
	{"modifyPlayerScore", api_modifyPlayerScore, 1, 0},
	{"talkAs", api_talkAs, 1, 0},
	{"runcmdAs", api_runcmdAs, 1, 0},
	{"teleport", api_teleport, 1, 0},
	{"tellraw", api_tellraw, 1, 0},
	{"tellrawEx", api_tellrawEx, 1, 0},
	{"setBossBar", api_setBossBar, 1, 0},
	{"removeBossBar", api_removeBossBar, 1, 0},
	{"setDamage", api_setDamage, 1, 0},
	{"setServerMotd", api_setServerMotd, 1, 0},
	{"getPlayerItems", api_getPlayerItems, 1, 0},
	{"setPlayerItems", api_setPlayerItems, 1, 0},
	{"getPlayerHand", api_getPlayerHand, 1, 0},
	{"setPlayerHand", api_setPlayerHand, 1, 0},
	{"getPlayerArmor", api_getPlayerArmor, 1, 0},
	{"setPlayerArmor", api_setPlayerArmor, 1, 0},
	{"getPlayerItem", api_getPlayerItem, 1, 0},
	{"setPlayerItem", api_setPlayerItem, 1, 0},
	{"getPlayerEnderChests", api_getPlayerEnderChests, 1, 0},
	{"setPlayerEnderChests", api_setPlayerEnderChests, 1, 0},
	{"addItemEx", api_addItemEx, 1, 0},
	{"removeItem", api_removeItem, 1, 0},
	{"setSidebar", api_setSidebar, 1, 0},
	{"removeSidebar", api_removeSidebar, 1, 0},
	{"getBlock", api_getBlock, 1, 0},
	{"setBlock", api_setBlock, 1, 0},
	{"getStructure", api_getStructure, 1, 0},
	{"setStructure", api_setStructure, 1, 0},
	{0,0,0,0}
};
static PyModuleDef api_module
{
	PyModuleDef_HEAD_INIT,
	"mc",
	0,
	-1,
	api_list,
	0,
	0,
	0,
	0
};
static PyObject* api_init() {
	return PyModule_Create(&api_module);
}
#pragma endregion
static void init() {
	//解释器初始化
	//PyPreConfig cfg;
	//PyPreConfig_InitPythonConfig(&cfg);
	//cfg.utf8_mode = 1;
	//cfg.configure_locale = 0;
	//Py_PreInitialize(&cfg);
	PyImport_AppendInittab("mc", api_init); //增加一个模块
	Py_Initialize();
	//PyEval_InitThreads();
	filesystem::directory_iterator files("py");
	for (auto& info : files) {
		auto& path = info.path();
		if (path.extension() == ".py" || path.extension() == ".pyd") {
			const string& name = path.stem().u8string();
			print("[BDSpyrunner] loading ", name);
			PyImport_ImportModule(("py." + name).c_str());
			PyErr_Print();
		}
	}
	//PyEval_SaveThread();
}
BOOL WINAPI DllMain(HMODULE, DWORD reason, LPVOID) {
	if (reason == 1) {
		//while (1) {
		//	Tag* t = toTag(toJson(R"({})"));
		//	cout << toJson(t).toStyledString() << endl;
		//	t->deCompound();
		//	delete t;
		//}
		//sizeof(Json::Value);
		if (!filesystem::exists("py"))
			filesystem::create_directory("py");
		init();
		puts("[BDSpyrunner] 1.3.1 loaded. 感谢小枫云 http://ipyvps.com 的赞助.");
	}
	return 1;
}