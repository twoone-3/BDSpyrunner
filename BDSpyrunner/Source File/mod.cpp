//mod.cpp 插件模块
//排除极少使用的Windows API
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <tool.h>
#include <PyEntity.h>
#include <global.h>
#include <Actor.h>
#include <Block.h>
#include <ItemStack.h>
#include <Level.h>
#include <NetWork.h>
#include <ScoreBoard.h>
#include <Structure.h>
#include <Tag.h>
#include <Event.h>

constexpr auto VERSION_1 = 1;
constexpr auto VERSION_2 = 7;
constexpr auto VERSION_3 = 3;
constexpr auto PLUGIN_PATH = L"plugins/py";
constexpr auto MODULE_NAME = "mc";

using namespace std;

//Py函数表
static unordered_map<EventCode, vector<PyObject*>> g_callback_functions;
//注册命令
static unordered_map<string, pair<string, PyObject*>> g_commands;
//伤害
static int g_damage = 0;
#pragma region Function Define
//注入时事件
static void init();
//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库3.10.0版本
Json StringtoJson(string_view str) {
	try {
		return Json::parse(str);
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
		return nullptr;
	}
}
//Dll入口函数
BOOL WINAPI DllMain(HMODULE, DWORD, LPVOID) {
	return TRUE;
}
//检查版本
string getBDSVersion() {
	string version;
	SymCall<string&>("?getServerVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		&version);
	return version;
}
//转宽字符
static wstring CharToWchar(const string& str) {
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
	wstring wstr;
	wstr.resize(len + 1);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), wstr.data(), len);
	return wstr;
}
//事件回调
template <typename... Args>
static bool EventCallBack(EventCode e, const char* format, Args... args) {
	bool intercept = true;
	SEH_BEGIN;
	Py_CALL_BEGIN;
	for (PyObject* cb : g_callback_functions[e]) {
		PyObject* result = PyObject_CallFunction(cb, format, args...);
		PyErr_Print();
		if (result == Py_False)
			intercept = false;
	}
	Py_CALL_END;
	SEH_END;
	return intercept;
}
#pragma endregion
#pragma region Hook List
#if 0
HOOK(Actor_load, bool, "?load@Actor@@UEAA_NAEBVCompoundTag@@AEAVDataLoadHelper@@@Z",
	Actor* _this, Tag* tag, struct DataLoadHelper* data) {
	//cout << CompoundTagtoJson(tag).dump(4) << endl;
	//cout << data << endl;

	return original(_this, tag, data);
}
HOOK(Level_tick, void, "?tick@Level@@UEAAXXZ",
	Level* _this) {
	original(_this);
}
#endif
//将Python解释器初始化插入bds主函数
HOOK(BDS_Main, int, "main",
	int argc, char* argv[], char* envp[]) {
	init();
	// 执行 main 函数
	return original(argc, argv, envp);
}
//Level的构造函数
HOOK(Level_construct, Level*, "??0Level@@QEAA@AEBV?$not_null@V?$NonOwnerPointer@VSoundPlayerInterface@@@Bedrock@@@gsl@@V?$unique_ptr@VLevelStorage@@U?$default_delete@VLevelStorage@@@std@@@std@@V?$unique_ptr@VLevelLooseFileStorage@@U?$default_delete@VLevelLooseFileStorage@@@std@@@4@AEAVIMinecraftEventing@@_NEAEAVScheduler@@V?$not_null@V?$NonOwnerPointer@VStructureManager@@@Bedrock@@@2@AEAVResourcePackManager@@AEBV?$not_null@V?$NonOwnerPointer@VIEntityRegistryOwner@@@Bedrock@@@2@V?$WeakRefT@UEntityRefTraits@@@@V?$unique_ptr@VBlockComponentFactory@@U?$default_delete@VBlockComponentFactory@@@std@@@4@V?$unique_ptr@VBlockDefinitionGroup@@U?$default_delete@VBlockDefinitionGroup@@@std@@@4@@Z",
	Level* _this, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13) {
	return g_level = original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}
//SPSCQueue的构造函数
HOOK(SPSCQueue_construct, SPSCQueue*, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z",
	SPSCQueue* _this) {
	return g_command_queue = original(_this);
}
//RakPeer的构造函数
HOOK(RakPeer_construct, RakPeer*, "??0RakPeer@RakNet@@QEAA@XZ",
	RakPeer* _this) {
	//会构造两次，取第一次值
	if (g_rak_peer == nullptr)
		return g_rak_peer = original(_this);
	return original(_this);
}
//ServerNetworkHandler的构造函数
HOOK(ServerNetworkHandler_construct, uintptr_t, "??0ServerNetworkHandler@@QEAA@AEAVGameCallbacks@@AEBV?$NonOwnerPointer@VILevel@@@Bedrock@@AEAVNetworkHandler@@AEAVPrivateKeyManager@@AEAVServerLocator@@AEAVPacketSender@@AEAVAllowList@@PEAVPermissionsFile@@AEBVUUID@mce@@H_NAEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@HAEAVMinecraftCommands@@AEAVIMinecraftApp@@AEBV?$unordered_map@UPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$hash@UPackIdVersion@@@3@U?$equal_to@UPackIdVersion@@@3@V?$allocator@U?$pair@$$CBUPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@@3@@std@@AEAVScheduler@@V?$NonOwnerPointer@VTextFilteringProcessor@@@3@@Z",
	ServerNetworkHandler* _this, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16, uintptr_t a17, uintptr_t a18, uintptr_t a19, uintptr_t a20) {
	g_server_network_handler = _this;
	return original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
}
//Scoreboard的构造函数
HOOK(ServerScoreboard_construct, Scoreboard*, "??0ServerScoreboard@@QEAA@VCommandSoftEnumRegistry@@PEAVLevelStorage@@@Z",
	Scoreboard* _this, uintptr_t a2, uintptr_t a3) {
	return g_scoreboard = original(_this, a2, a3);
}
//改变设置命令的建立，用于注册命令
HOOK(ChangeSettingCommand_setup, void, "?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",
	uintptr_t _this) {
	for (auto& [cmd, des] : g_commands) {
		SymCall("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
			_this, &cmd, des.first.c_str(), 0, 0, 0x80);
	}
	original(_this);
}
//控制台输出，实际上是cout<<xxx的底层调用
HOOK(onConsoleOutput, ostream&, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
	ostream& _this, const char* str, uintptr_t size) {
	if (&_this == &cout) {
		wstring wstr = CharToWchar(str);
		bool res = EventCallBack(EventCode::onConsoleOutput, "u#", wstr.c_str(), wstr.length());
		if (!res)return _this;
	}
	return original(_this, str, size);
}
//控制台输入，实际上是命令队列的底层
HOOK(onConsoleInput, bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	SPSCQueue* _this, const string& cmd) {
	//debug模式（不推荐使用）
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
		Py_CALL_BEGIN;
		PyRun_SimpleString(cmd.c_str());
		Py_CALL_END;
		cout << '>';
		return 0;
	}
	wstring wstr = CharToWchar(cmd);
	if (EventCallBack(EventCode::onConsoleInput, "u#", wstr.c_str(), wstr.length()))
		return original(_this, cmd);
	else
		return false;
}
//玩家加入发包
HOOK(onPlayerJoin, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id,/*SetLocalPlayerAsInitializedPacket*/ uintptr_t pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		EventCallBack(EventCode::onPlayerJoin, "O", PyEntity_FromEntity(p));
	}
	original(_this, id, pkt);
}
//玩家退出
HOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	uintptr_t _this, Player* p, char v3) {
	EventCallBack(EventCode::onPlayerLeft, "O", PyEntity_FromEntity(p));
	return original(_this, p, v3);
}
//使用物品
HOOK(onUseItem, bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	uintptr_t _this, ItemStack* item, BlockPos* bp, char a4, uintptr_t a5, Block* b) {
	Player* p = FETCH(Player*, _this + 8);
	short iid = item->getId();
	short iaux = item->getAuxValue();
	string iname = item->getName();
	BlockLegacy* bl = b->getBlockLegacy();
	short bid = bl->getBlockItemID();
	string bn = bl->getBlockName();
	if (EventCallBack(EventCode::onUseItem,
		"{s:O,s:i,s:i,s:s,s:s,s:i,s:[i,i,i]}",
		"player", PyEntity_FromEntity(p),
		"itemid", static_cast<int>(iid),
		"itemaux", static_cast<int>(iaux),
		"itemname", iname.c_str(),
		"blockname", bn.c_str(),
		"blockid", static_cast<int>(bid),
		"position", bp->x, bp->y, bp->z
	))
		return original(_this, item, bp, a4, a5, b);
	else
		return false;
}
//放置方块
HOOK(onPlaceBlock, bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* _this, Block* b, BlockPos* bp, unsigned char a4, Actor* p, bool _bool) {
	if (isPlayer(p)) {
		BlockLegacy* bl = b->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		if (!EventCallBack(EventCode::onPlaceBlock,
			"{s:O,s:s,s:i,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"blockname", bn.c_str(),
			"blockid", bid,
			"position", bp->x, bp->y, bp->z
		))
			return false;
	}
	return original(_this, b, bp, a4, p, _bool);
}
//破坏方块
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
	st2.placeInWorld(_this, g_level->getBlockPalette(), bp, &ss);
	//st.placeInWorld(_this, g_level->getBlockPalette(), bp, &ss);
#endif
	if (isPlayer(p)) {
		BlockLegacy* bl = _this->getBlock(bp)->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		if (!EventCallBack(EventCode::onDestroyBlock,
			"{s:O,s:s,s:i,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"blockname", bn.c_str(),
			"blockid", static_cast<int>(bid),
			"position", bp->x, bp->y, bp->z
		))
			return false;
	}
	return original(_this, p, bp, a3, a4);
}
//开箱
HOOK(onOpenChest, bool, "?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	if (EventCallBack(EventCode::onOpenChest,
		"{s:O,s:[i,i,i]}",
		"player", PyEntity_FromEntity(p),
		"position", bp->x, bp->y, bp->z
	))
		return original(_this, p, bp);
	return false;
}
//开桶，不能拦截，到方块交互才能拦截，不知道mojang员工在想什么
HOOK(onOpenBarrel, bool, "?use@BarrelBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	if (EventCallBack(EventCode::onOpenBarrel,
		"{s:O,s:[i,i,i]}",
		"player", PyEntity_FromEntity(p),
		"position", bp->x, bp->y, bp->z
	))
		return original(_this, p, bp);
	return false;
}
//关箱
HOOK(onCloseChest, void, "?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	auto bp = (BlockPos*)(_this - 204);
	EventCallBack(EventCode::onCloseChest,
		"{s:O,s:[i,i,i]}",
		"player", PyEntity_FromEntity(p),
		"position", bp->x, bp->y, bp->z
	);
	original(_this, p);
}
//关桶
HOOK(onCloseBarrel, void, "?stopOpen@BarrelBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	auto bp = (BlockPos*)(_this - 204);
	EventCallBack(EventCode::onCloseBarrel,
		"{s:O,s:[i,i,i]}",
		"player", PyEntity_FromEntity(p),
		"position", bp->x, bp->y, bp->z
	);
	original(_this, p);
}
//放入取出物品，实际上是容器改变
HOOK(onContainerChange, void, "?containerContentChanged@LevelContainerModel@@UEAAXH@Z",
	uintptr_t _this, unsigned slot) {
	Player* p = FETCH(Player*, _this + 208);//IDA LevelContainerModel::_getContainer line 15 25 v3
	BlockSource* bs = p->getRegion();
	BlockPos* bp = reinterpret_cast<BlockPos*>(_this + 216);
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	short bid = bl->getBlockItemID();
	if (bid == 54 || bid == 130 || bid == 146 || bid == -203 || bid == 205 || bid == 218) {	//非箱子、桶、潜影盒的情况不作处理
		uintptr_t v5 = (*reinterpret_cast<uintptr_t(**)(uintptr_t)>(FETCH(uintptr_t, _this) + 160))(_this);
		if (v5) {
			ItemStack* i = (*reinterpret_cast<ItemStack * (**)(uintptr_t, uintptr_t)>(FETCH(uintptr_t, v5) + 40))(v5, slot);
			EventCallBack(EventCode::onContainerChange,
				"{s:O,s:s,s:i,s:[i,i,i],s:i,s:i,s:s,s:i,s:i}",
				"player", PyEntity_FromEntity(p),
				"blockname", bl->getBlockName().c_str(),
				"blockid", bid,
				"position", bp->x, bp->y, bp->z,
				"itemid", i->getId(),
				"itemcount", i->getCount(),
				"itemname", i->getName().c_str(),
				"itemaux", i->getAuxValue(),
				"slot", slot
			);
		}
	}
	original(_this, slot);
}
//玩家攻击
HOOK(onAttack, bool, "?attack@Player@@UEAA_NAEAVActor@@AEBW4ActorDamageCause@@@Z",
	Player* p, Actor* a, struct ActorDamageCause* c) {
	if (a) {
		a->setNameTag("傻逼");
		a->setNameTagVisible(true);
	}
	if (EventCallBack(EventCode::onPlayerAttack,
		"{s:O,s:O}",
		"player", PyEntity_FromEntity(p),
		"actor", PyEntity_FromEntity(a)
	))
		return original(p, a, c);
	return false;
}
//切换纬度
HOOK(onChangeDimension, bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z",
	uintptr_t _this, Player* p, uintptr_t req) {
	bool result = original(_this, p, req);
	if (result) {
		EventCallBack(EventCode::onChangeDimension, "O", PyEntity_FromEntity(p));
	}
	return result;
}
//生物死亡
HOOK(onMobDie, void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z",
	Mob* _this, uintptr_t dmsg) {
	char v72;
	Actor* sa = _this->getLevel()->fetchEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 64))(dmsg, &v72)));
	bool res = EventCallBack(EventCode::onMobDie,
		"{s:I,s:O,s:O}",
		"dmcase", FETCH(unsigned, dmsg + 8),
		"actor1", PyEntity_FromEntity(_this),
		"actor2", PyEntity_FromEntity(sa)//可能为0
	);
	if (res) original(_this, dmsg);
}
//生物受伤
HOOK(onMobHurt, bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@H_N1@Z",
	Mob* _this, uintptr_t dmsg, int a3, bool a4, bool a5) {
	g_damage = a3;//将生物受伤的值设置为可调整
	char v72;
	Actor* sa = _this->getLevel()->fetchEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 64))(dmsg, &v72)));
	if (EventCallBack(EventCode::onMobHurt,
		"{s:i,s:O,s:O,s:i}",
		"dmcase", FETCH(unsigned, dmsg + 8),
		"actor1", PyEntity_FromEntity(_this),
		"actor2", PyEntity_FromEntity(sa),//可能为0
		"damage", a3
	))
		return original(_this, dmsg, g_damage, a4, a5);
	return false;
}
//玩家重生
HOOK(onRespawn, void, "?respawn@Player@@UEAAXXZ",
	Player* p) {
	EventCallBack(EventCode::onRespawn, "O", PyEntity_FromEntity(p));
	original(p);
}
//聊天，消息title msg w等...
HOOK(onChat, void, "?fireEventPlayerMessage@MinecraftEventing@@AEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@000@Z",
	uintptr_t _this, string& sender, string& target, string& msg, string& style) {
	EventCallBack(EventCode::onChat,
		"{s:s,s:s,s:s,s:s}",
		"sender", sender.c_str(),
		"target", target.c_str(),
		"msg", msg.c_str(),
		"style", style.c_str()
	);
	original(_this, sender, target, msg, style);
}
//玩家输入文本
HOOK(onInputText, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*TextPacket*/uintptr_t pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	bool res = true;
	if (p) {
		const string& msg = FETCH(string, pkt + 88);
		res = EventCallBack(EventCode::onInputText,
			"{s:O,s:s}",
			"player", PyEntity_FromEntity(p),
			"msg", msg.c_str()
		);
	}
	if (res)original(_this, id, pkt);
}
//玩家输入命令
HOOK(onInputCommand, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*CommandRequestPacket*/uintptr_t pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		const string& cmd = FETCH(string, pkt + 48);
		auto data = g_commands.find(cmd.c_str() + 1);
		//如果有这条命令且回调函数不为nullptr
		if (data != g_commands.end() && data->second.second) {
			Py_CALL_BEGIN;
			PyObject_CallFunction(data->second.second, "O", PyEntity_FromEntity(p));
			PyErr_Print();
			Py_CALL_END;
			return;
		}
		bool res = EventCallBack(EventCode::onInputCommand,
			"{s:O,s:s}",
			"player", PyEntity_FromEntity(p),
			"cmd", cmd.c_str()
		);
		if (res)original(_this, id, pkt);
	}
}
//玩家选择表单
HOOK(onSelectForm, void, "?handle@?$PacketHandlerDispatcherInstance@VModalFormResponsePacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
	uintptr_t _this, uintptr_t id, ServerNetworkHandler* handle,/*ModalFormResponsePacket*/uintptr_t* ppkt) {
	uintptr_t pkt = *ppkt;
	Player* p = handle->_getServerPlayer(id, pkt);
	if (p) {
		unsigned fid = FETCH(unsigned, pkt + 48);
		string data = FETCH(string, pkt + 56);
		if (data.back() == '\n')data.pop_back();
		EventCallBack(EventCode::onSelectForm,
			"{s:O,s:s,s:I}",
			"player", PyEntity_FromEntity(p),
			"selected", data.c_str(),
			"formid", fid
		);
	}
	original(_this, id, handle, ppkt);
}
//命令方块更新
HOOK(onCommandBlockUpdate, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*CommandBlockUpdatePacket*/uintptr_t pkt) {
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
			"{s:O,s:i,s:i,s:i,s:s,s:s,s:s,s:i,s:[i,i,i]}",
			"player", PyEntity_FromEntity(p),
			"mode", mode,
			"condition", condition,
			"redstone", redstone,
			"cmd", cmd.c_str(),
			"output", output.c_str(),
			"rawname", rawname.c_str(),
			"delay", delay,
			"position", bp.x, bp.y, bp.z
		);
	}
	if (res)original(_this, id, pkt);
}
//爆炸
HOOK(onLevelExplode, bool, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* bs, Actor* a3, Vec3 pos, float a5, bool a6, bool a7, float a8, bool a9) {
	if (EventCallBack(EventCode::onLevelExplode,
		"{s:O,s:[f,f,f],s:i,s:f}",
		"actor", PyEntity_FromEntity(a3),
		"position", pos.x, pos.y, pos.z,
		"dimensionid", bs->getDimensionId(),
		"power", a5
	))
		return original(_this, bs, a3, pos, a5, a6, a7, a8, a9);
	return false;
}
//命令方块执行
HOOK(onCommandBlockPerform, bool, "?_execute@CommandBlock@@AEBAXAEAVBlockSource@@AEAVCommandBlockActor@@AEBVBlockPos@@_N@Z",
	uintptr_t _this, BlockSource* a2, uintptr_t a3, BlockPos* bp, bool a5) {
	//脉冲:0,重复:1,链:2
	int mode = SymCall<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z", a3, a2);
	//无条件:0,有条件:1
	bool condition = SymCall<bool>("?getConditionalMode@CommandBlockActor@@QEBA_NAEAVBlockSource@@@Z", a3, a2);
	string cmd = FETCH(string, a3 + 264);
	string rawname = FETCH(string, a3 + 296);
	if (EventCallBack(EventCode::onCommandBlockPerform,
		"{s:i,s:b,s:s,s:s,s:[i,i,i]}",
		"mode", mode,
		"condition", condition,
		"cmd", cmd.c_str(),
		"rawname", rawname.c_str(),
		"position", bp->x, bp->y, bp->z
	))
		return original(_this, a2, a3, bp, a5);
	return false;
}
//玩家移动
HOOK(onMove, void, "??0MovePlayerPacket@@QEAA@AEAVPlayer@@W4PositionMode@1@HH@Z",
	uintptr_t _this, Player* p, char a3, int a4, int a5) {
	EventCallBack(EventCode::onMove, "O", PyEntity_FromEntity(p));
	original(_this, p, a3, a4, a5);
}
//玩家穿戴
HOOK(onSetArmor, void, "?setArmor@Player@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z",
	Player* p, unsigned slot, ItemStack* i) {
	if (!EventCallBack(EventCode::onSetArmor,
		"{s:O,s:i,s:i,s:s,s:i,s:i}",
		"player", PyEntity_FromEntity(p),
		"itemid", i->getId(),
		"itemcount", i->getCount(),
		"itemname", i->getName().c_str(),
		"itemaux", i->getAuxValue(),
		"slot", slot
	))
		return;
	return original(p, slot, i);
}
//计分板改变监听
HOOK(onScoreChanged, void, "?onScoreChanged@ServerScoreboard@@UEAAXAEBUScoreboardId@@AEBVObjective@@@Z",
	Scoreboard* _this, ScoreboardId* a1, Objective* a2) {
	/*
	原命令：
	创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	*/
	int scoreboardid = a1->id;
	EventCallBack(EventCode::onScoreChanged,
		"{s:i,s:i,s:s,s:s}",
		"scoreboardid", scoreboardid,
		"playersnum", a2->getPlayerScore(a1)->getCount(),
		"objectivename", a2->getScoreName().c_str(),
		"objectivedisname", a2->getScoreDisplayName().c_str()
	);
	original(_this, a1, a2);
}
//耕地破坏
HOOK(onFallBlockTransform, void, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z",
	uintptr_t _this, BlockSource* a1, BlockPos* a2, Actor* p, uintptr_t a4) {
	if (isPlayer(p)) {
		if (!EventCallBack(EventCode::onFallBlockTransform,
			"{s:O,s:[i,i,i],s:i}",
			"player", PyEntity_FromEntity(p),
			"position", a2->x, a2->y, a2->z,
			"dimensionid", a1->getDimensionId()
		))
			return;
	}
	original(_this, a1, a2, p, a4);
}
//使用重生锚
HOOK(onUseRespawnAnchorBlock, bool, "?trySetSpawn@RespawnAnchorBlock@@CA_NAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* p, BlockPos* a2, BlockSource* a3, Level* a4) {
	if (isPlayer(p)) {
		if (!EventCallBack(EventCode::onUseRespawnAnchorBlock,
			"{s:O,s:[i,i,i],s:i}",
			"player", PyEntity_FromEntity(p),
			"position", a2->x, a2->y, a2->z,
			"dimensionid", a3->getDimensionId()
		))
			return false;
	}
	return original(p, a2, a3, a4);
}
//活塞推
HOOK(onPistonPush, bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z",
	BlockActor* _this, BlockSource* bs, BlockPos* bp, unsigned a3, unsigned a4) {
	BlockLegacy* blg = bs->getBlock(bp)->getBlockLegacy();
	string bn = blg->getBlockName();
	short bid = blg->getBlockItemID();
	BlockPos* bp2 = _this->getPosition();
	if (EventCallBack(EventCode::onPistonPush,
		"{s:s,s:i,s:[i,i,i],s:[i,i,i],s:i}",
		"blockname", bn.c_str(),
		"blockid", bid,
		"blockpos", bp->x, bp->y, bp->z,
		"pistonpos", bp2->x, bp2->y, bp2->z,
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, bs, bp, a3, a4);
	return false;
}
//末影人随机传送（没人会用吧？）
HOOK(onEndermanRandomTeleport, bool, "?randomTeleport@TeleportComponent@@QEAA_NAEAVActor@@@Z",
	uintptr_t _this, Actor* a1) {
	if (EventCallBack(EventCode::onEndermanRandomTeleport, "O", PyEntity_FromEntity(a1)))
		return original(_this, a1);
	return false;
}
//服务器开完
HOOK(onServerStarted, void, "?startServerThread@ServerInstance@@QEAAXXZ",
	uintptr_t _this) {
	EventCallBack(EventCode::onServerStarted, nullptr);
	original(_this);
}
//丢物品
HOOK(onDropItem, bool, "?drop@Player@@UEAA_NAEBVItemStack@@_N@Z",
	Player* _this, ItemStack* a2, bool a3) {
	if (EventCallBack(EventCode::onDropItem,
		"{s:O,s:i,s:i,s:s,s:i}",
		"player", PyEntity_FromEntity(_this),
		"itemid", a2->getId(),
		"itemcount", a2->getCount(),
		"itemname", a2->getName().c_str(),
		"itemaux", a2->getAuxValue()
	))
		return original(_this, a2, a3);
	return false;
}
//拿物品
HOOK(onTakeItem, bool, "?take@Player@@QEAA_NAEAVActor@@HH@Z",
	Player* _this, Actor* actor, int a2, int a3) {
	if (EventCallBack(EventCode::onTakeItem,
		"{s:O,s:O}",
		"player", PyEntity_FromEntity(_this),
		"actor", PyEntity_FromEntity(actor)
	))
		return original(_this, actor, a2, a3);
	return false;
}
//骑
HOOK(onRide, bool, "?canAddRider@Actor@@UEBA_NAEAV1@@Z",
	Actor* a1, Actor* a2) {
	if (EventCallBack(EventCode::onRide,
		"{s:O,s:O}",
		"actor1", PyEntity_FromEntity(a1),
		"actor2", PyEntity_FromEntity(a2)
	))
		return original(a1, a2);
	return false;
}
//放入取出物品展示框的物品（未测试）
HOOK(onUseFrameBlock, bool, "?use@ItemFrameBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	if (EventCallBack(EventCode::onUseFrameBlock,
		"{s:O,s:[i,i,i],s:i}",
		"player", PyEntity_FromEntity(a2),
		"blockpos", a3->x, a3->y, a3->z,
		"dimensionid", a2->getDimensionId()
	))
		return original(_this, a2, a3);
	return false;
}
//点击物品展示框（未测试）
HOOK(onUseFrameBlocka, bool, "?attack@ItemFrameBlock@@UEBA_NPEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	if (EventCallBack(EventCode::onUseFrameBlock,
		"{s:O,s:[i,i,i],s:i}",
		"player", PyEntity_FromEntity(a2),
		"blockpos", a3->x, a3->y, a3->z,
		"dimensionid", a2->getDimensionId()
	))
		return original(_this, a2, a3);
	return false;
}
//玩家跳跃
HOOK(onJump, void, "?jumpFromGround@Player@@UEAAXXZ",
	Player* _this) {
	if (EventCallBack(EventCode::onJump, "O", PyEntity_FromEntity(_this)))
		return original(_this);
}
//玩家潜行
HOOK(onSneak, void, "?sendActorSneakChanged@ActorEventCoordinator@@QEAAXAEAVActor@@_N@Z",
	uintptr_t _this, Actor* a1, bool a2) {
	if (EventCallBack(EventCode::onSneak, "O", PyEntity_FromEntity(a1)))
		return original(_this, a1, a2);
}
//火势蔓延（未测试）
HOOK(onFireSpread, bool, "?_trySpawnBlueFire@FireBlock@@AEBA_NAEAVBlockSource@@AEBVBlockPos@@@Z",
	uintptr_t _this, BlockSource* bs, BlockPos* bp) {
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	if (EventCallBack(EventCode::onFireSpread,
		"{s:[i,i,i],s:s,s:i,s:i}",
		"blockpos", bp->x, bp->y, bp->z,
		"blockname", bl->getBlockName().c_str(),
		"blockid", bl->getBlockItemID(),
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, bs, bp);
	return false;
}
//方块交互（除箱子，工作台）
HOOK(onBlockInteracted, void, "?onBlockInteractedWith@VanillaServerGameplayEventListener@@UEAA?AW4EventResult@@AEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* pl, BlockPos* bp) {
	BlockSource* bs = g_level->getBlockSource(pl->getDimensionId());
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	if (EventCallBack(EventCode::onBlockInteracted,
		"{s:O,s:[i,i,i],s:s,s:i,s:i}",
		"player", PyEntity_FromEntity(pl),
		"blockpos", bp->x, bp->y, bp->z,
		"blockname", bl->getBlockName().c_str(),
		"blockid", bl->getBlockItemID(),
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, pl, bp);
}
//方块被爆炸破坏（未测试）
HOOK(onBlockExploded, void, "?onExploded@Block@@QEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@@Z",
	Block* _this, BlockSource* bs, BlockPos* bp, Actor* actor) {
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	if (EventCallBack(EventCode::onBlockExploded,
		"{s:O,s:[i,i,i],s:s,s:i,s:i}",
		"actor", PyEntity_FromEntity(actor),
		"blockpos", bp->x, bp->y, bp->z,
		"blockname", bl->getBlockName().c_str(),
		"blockid", bl->getBlockItemID(),
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, bs, bp, actor);
}
//方块侧面放触发，点击牌子触发
HOOK(onUseSingBlock, uintptr_t, "?use@SignBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a1, BlockPos* a2) {
	BlockSource* bs = a1->getRegion();
	BlockActor* ba = bs->getBlockEntity(a2);
	string text;
	//获取沉浸式文本内容
	SymCall<string&>("?getImmersiveReaderText@SignBlockActor@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEAVBlockSource@@@Z",
		ba, &text, bs);
	if (EventCallBack(EventCode::onUseSignBlock,
		"{s:O,s:s,s:[i,i,i]}",
		"player", PyEntity_FromEntity(a1),
		"text", text.c_str(),
		"pos", a2->x, a2->y, a2->z
	))
		return original(_this, a1, a2);
	return 0;
}
#pragma endregion
#pragma region API Function
//最小版本要求
static PyObject* PyAPI_minVersionRequire(PyObject*, PyObject* args) {
	int v1, v2, v3;
	if (PyArg_ParseTuple(args, "iii:minVersionRequire", &v1, &v2, &v3)) {
		if (v1 > VERSION_1)
			Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
		if (v2 > VERSION_2)
			Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
		if (v3 > VERSION_3)
			Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
	}
	Py_RETURN_NONE;
}
//获取版本（摒弃）
static PyObject* PyAPI_getBDSVersion(PyObject*, PyObject*) {
	return ToPyUnicode(getBDSVersion());
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
		if (!g_command_queue)
			Py_RETURN_ERROR("Command queue is not initialized");
		onConsoleInput::original(g_command_queue, cmd);
	}
	Py_RETURN_NONE;
}
//设置监听
static PyObject* PyAPI_setListener(PyObject*, PyObject* args) {
	const char* name = ""; PyObject* func;
	if (PyArg_ParseTuple(args, "sO:setListener", &name, &func)) {
		auto it = events.find(name);
		if (!PyFunction_Check(func)) {
			Py_RETURN_ERROR("Parameter 2 is not callable");
		}
		if (it == events.end()) {
			Py_RETURN_ERROR("Invalid Listener key words");
		}
		g_callback_functions[it->second].push_back(func);
	}
	Py_RETURN_NONE;
}
//设置指令说明
static PyObject* PyAPI_setCommandDescription(PyObject*, PyObject* args) {
	const char* cmd = "";
	const char* des = "";
	PyObject* callback = nullptr;
	if (PyArg_ParseTuple(args, "ss|O:setCommandDescription", &cmd, &des, &callback)) {
		g_commands[cmd] = { des, callback };
	}
	Py_RETURN_NONE;
}
//获取玩家
static PyObject* PyAPI_getPlayerByXuid(PyObject*, PyObject* args) {
	const char* xuid = "";
	if (PyArg_ParseTuple(args, "s:getPlayerByXuid", &xuid)) {
		Player* p = g_level->getPlayerByXuid(xuid);
		if (!p)
			Py_RETURN_ERROR("Failed to find player");
		return PyEntity_FromEntity(p);
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_getPlayerList(PyObject*, PyObject*) {
	PyObject* list = PyList_New(0);
	if (!g_level)
		Py_RETURN_ERROR("Level is not set");
	for (Player* p : g_level->getAllPlayers()) {
		PyList_Append(list, PyEntity_FromEntity(p));
	}
	return list;
}
//修改生物受伤的伤害值
static PyObject* PyAPI_setDamage(PyObject*, PyObject* args) {
	PyArg_ParseTuple(args, "i:setDamage", &g_damage);
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setServerMotd(PyObject*, PyObject* args) {
	const char* name = "";
	if (PyArg_ParseTuple(args, "s:setServerMotd", &name)) {
		if (g_server_network_handler)
			SymCall<uintptr_t, ServerNetworkHandler*, const string&, bool>("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
				g_server_network_handler, name, true);
		else
			Py_RETURN_ERROR("Server did not finish loading");
	}
	Py_RETURN_NONE;
}
//根据坐标设置方块
static PyObject* PyAPI_getBlock(PyObject*, PyObject* args) {
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "iiii:getBlock", &bp.x, &bp.y, &bp.z, &did)) {
		BlockSource* bs = g_level->getBlockSource(did);
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
		BlockSource* bs = g_level->getBlockSource(did);
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
		BlockSource* bs = g_level->getBlockSource(did);
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

		return ToPyUnicode(CompoundTagtoJson(st.save()).dump(4));
	}
	Py_RETURN_NONE;
}
static PyObject* PyAPI_setStructure(PyObject*, PyObject* args) {
	const char* data = "";
	BlockPos pos; int did;
	if (PyArg_ParseTuple(args, "siiii:setStructure",
		&data, &pos.x, &pos.y, &pos.z, &did)) {
		BlockSource* bs = g_level->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		Json value = StringtoJson(data);
		Json& arr = value["size9"];
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
		st.placeInWorld(bs, g_level->getBlockPalette(), &pos, &ss);
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
//产生爆炸
static PyObject* PyAPI_explode(PyObject*, PyObject* args) {
	Vec3 pos; int did;
	float power; bool destroy;
	float range; bool fire;
	if (PyArg_ParseTuple(args, "fffifbfb:explode",
		&pos.x, &pos.y, &pos.z, &did, &power, &destroy, &range, &fire)) {
		BlockSource* bs = g_level->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		onLevelExplode::original(g_level, bs, nullptr, pos, power, fire, destroy, range, true);
	}
	Py_RETURN_NONE;
}
//生成物品
static PyObject* PyAPI_spawnItem(PyObject*, PyObject* args) {
	const char* data = "";
	Vec3 pos; int did;
	if (PyArg_ParseTuple(args, "sfffi:spawnItem", &data, &pos.x, &pos.y, &pos.z, &did)) {
		BlockSource* bs = g_level->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		ItemStack item(StringtoJson(data));
		g_level->getSpawner()->spawnItem(bs, &item, &pos);
		cout << pos.toString() << endl;
	}
	Py_RETURN_NONE;
}
//模块方法列表
static PyMethodDef PyAPI_Methods[]{
	{"minVersionRequire", PyAPI_minVersionRequire, METH_VARARGS, nullptr},
	{"getBDSVersion", PyAPI_getBDSVersion, METH_NOARGS, nullptr},
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
	{"explode", PyAPI_explode, METH_VARARGS, nullptr},
	{"spawnItem", PyAPI_spawnItem, METH_VARARGS, nullptr},
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
	using namespace filesystem;
	cout << "[BDSpyrunner] " << VERSION_1
		<< '.' << VERSION_2 << '.' << VERSION_3 << " loaded." << endl;
	//如果目录不存在创建目录
	if (!exists(PLUGIN_PATH))
		create_directories(PLUGIN_PATH);
	if (getBDSVersion() != "1.17.11.01") {
		cerr << "[BDSpyrunner] 服务端版本非最新版，继续使用可能出现未知问题" << endl;
		cerr << "[BDSpyrunner] The server version is not the latest version, unknown problems may occur if you continue to use it" << endl;
	}
	//将plugins/py加入模块搜索路径
	Py_SetPath((wstring(PLUGIN_PATH) + L';' + Py_GetPath()).c_str());
#if 0
#pragma region 预初始化3.8+
	PyPreConfig cfg;
	PyPreConfig_InitPythonConfig(&cfg);
	cfg.utf8_mode = 1;
	cfg.configure_locale = 0;
	Py_PreInitialize(&cfg);
#pragma endregion
#endif
	//增加一个模块
	PyImport_AppendInittab(MODULE_NAME, PyAPI_init);
	//初始化解释器
	Py_Initialize();
	if (PyType_Ready(&PyEntity_Type) < 0)
		cerr << "Falid to prepare class 'Entity'" << endl;
	//启用线程支持
	PyEval_InitThreads();
	for (auto& info : directory_iterator(PLUGIN_PATH)) {
		//whether the file is py
		if (info.path().extension() == ".py") {
			const string& name = info.path().stem().u8string();
			//ignore files starting with '_'
			if (name.front() == '_')
				continue;
			cout << "[BDSpyrunner] loading " << name << endl;
			PyImport_Import(ToPyUnicode(name));
			PyErr_Print();
		}
	}
	//释放当前线程
	PyEval_SaveThread();
}