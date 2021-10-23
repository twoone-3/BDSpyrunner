//mod.cpp 插件模块

//排除极少使用的Windows API
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include "mc/Actor.h"
#include "mc/Block.h"
#include "mc/ItemStack.h"
#include "mc/Level.h"
#include "mc/NetWork.h"
#include "mc/ScoreBoard.h"
#include "mc/Structure.h"
#include "mc/Tag.h"
#include "mc/tool.h"
#include "mod/Entity.h"
#include "mod/Event.h"
#include "mod/Module.h"
#include "mod/Version.h"
//WinInet library for auto update
#include <WinInet.h>
#pragma comment(lib,"WinInet.lib")

#define PLUGIN_PATH "plugins\\py\\"
#define CACHE_PATH "plugins\\cache\\"
#define BAT_PATH "plugins\\update_pyr.bat"

constexpr size_t BLOCK_SIZE = 0x1000;
constexpr const wchar_t* USER_AGENT = L"Mozilla/5.0 (Windows NT 6.3; Trident/7.0; rv:11.0) like Gecko";

using namespace std;

namespace fs = filesystem;
#pragma region Function
//注入时事件
void Init();
//Dll入口函数
//BOOL WINAPI DllMain(HMODULE, DWORD, LPVOID) {
//	return TRUE;
//}
//GBK转UTF8
static string GbkToUtf8(const char* src_str) {
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}
//UTF8转GBK
static string Utf8ToGbk(const char* src_str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
//转宽字符
static wstring ToWstring(string_view s) {
	string curlLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");
	const char* _Source = s.data();
	size_t _Dsize = s.size() + 1;

	wchar_t* _Dest = new wchar_t[_Dsize];
	size_t i;
	mbstowcs_s(&i, _Dest, _Dsize, _Source, s.size());
	wstring result = _Dest;
	delete[] _Dest;
	setlocale(LC_ALL, curlLocale.c_str());
	return result;
}
//访问url
static Json AccessUrlForJson(const wchar_t* url) {
	string data;
	//非空终止
	char buffer[BLOCK_SIZE];

	HINTERNET hSession = InternetOpen(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hSession)
		return nullptr;
	HINTERNET handle2 = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (!handle2)
		return nullptr;
	DWORD size = 0;
	do {
		InternetReadFile(handle2, buffer, BLOCK_SIZE, &size);
		data.append(buffer, size);
	} while (size);
	InternetCloseHandle(handle2);
	InternetCloseHandle(hSession);
	return StringToJson(data);
}
//访问url
static void AccessUrlForFile(const wchar_t* url, string_view filename) {
	//非空终止
	char buffer[BLOCK_SIZE];

	HINTERNET hSession = InternetOpen(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hSession)
		return;
	HINTERNET handle2 = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (!handle2)
		return;
	DWORD total = 0;
	DWORD size = 0;
	ofstream file(filename, ios::out | ios::binary);
	do {
		total += size;
		cout << "Downloading " << filename << "... " << total << "bytes\r";
		InternetReadFile(handle2, buffer, BLOCK_SIZE, &size);
		file.write(buffer, size);
	} while (size);
	InternetCloseHandle(handle2);
	InternetCloseHandle(hSession);
}
//检查版本
static void CheckPluginVersion() {
	if (!fs::exists(BAT_PATH))
		return;
	cout << "[BDSpyrunner] Checking plugin version..." << endl;
	Json info = AccessUrlForJson(L"https://api.github.com/repos/twoone-3/BDSpyrunner/releases/latest");
	if (info["tag_name"] == PYR_VERSION) {
		cout << "[BDSpyrunner] Your plugin version is latest." << endl;
		return;
	}
	cout << "[BDSpyrunner] Your plugin version isn't latest, auto downloading..." << endl;
	for (auto& asset : info["assets"]) {
		string download_url = asset["browser_download_url"];
		download_url.replace(8, 10, "hub.fastgit.org");
		AccessUrlForFile(ToWstring(download_url).c_str(), CACHE_PATH + string(asset["name"]));
		cout << asset["name"] << " was downloaded successfully, size: " << asset["size"] << endl;
	}
	cout << "[BDSpyrunner] The new version has been downloaded to plugins/download, please restart the server to replace it" << endl;
	system("start /min " BAT_PATH);
	exit(0);
}
//事件回调
template <typename... Args>
static bool EventCallBack(const EventCode e, const char* format, Args... args) {
	bool intercept = true;
	Py_BEGIN_CALL;
	for (PyObject* cb : g_callback_functions[e]) {
		PyObject* result = PyObject_CallFunction(cb, format, args...);
		PrintPythonError();
		if (result == Py_False)
			intercept = false;
	}
	Py_END_CALL;
	return intercept;
}
#pragma endregion
#pragma region Hook List
/*
THOOK(Level_tick, void, "?tick@Level@@UEAAXXZ",
	Level* _this) {
	original(_this);
}
*/
//将Python解释器初始化插入bds主函数
THOOK(BDS_Main, int, "main",
	int argc, char* argv[], char* envp[]) {
	Init();
	return original(argc, argv, envp);
}
//Level的构造函数
THOOK(Level_construct, Level*, "??0Level@@QEAA@AEBV?$not_null@V?$NonOwnerPointer@VSoundPlayerInterface@@@Bedrock@@@gsl@@V?$unique_ptr@VLevelStorage@@U?$default_delete@VLevelStorage@@@std@@@std@@V?$unique_ptr@VLevelLooseFileStorage@@U?$default_delete@VLevelLooseFileStorage@@@std@@@4@AEAVIMinecraftEventing@@_NEAEAVScheduler@@V?$not_null@V?$NonOwnerPointer@VStructureManager@@@Bedrock@@@2@AEAVResourcePackManager@@AEBV?$not_null@V?$NonOwnerPointer@VIEntityRegistryOwner@@@Bedrock@@@2@V?$WeakRefT@UEntityRefTraits@@@@V?$unique_ptr@VBlockComponentFactory@@U?$default_delete@VBlockComponentFactory@@@std@@@4@V?$unique_ptr@VBlockDefinitionGroup@@U?$default_delete@VBlockDefinitionGroup@@@std@@@4@@Z",
	Level* _this, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13) {
	return global<Level> = original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}
//SPSCQueue的构造函数
THOOK(SPSCQueue_construct, SPSCQueue*, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z",
	SPSCQueue* _this) {
	return global<SPSCQueue> = original(_this);
}
//RakPeer的构造函数
THOOK(RakPeer_construct, RakPeer*, "??0RakPeer@RakNet@@QEAA@XZ",
	RakPeer* _this) {
	//会构造两次，取第一次值
	if (global<RakPeer> == nullptr)
		return global<RakPeer> = original(_this);
	return original(_this);
}
//ServerNetworkHandler的构造函数
THOOK(ServerNetworkHandler_construct, uintptr_t, "??0ServerNetworkHandler@@QEAA@AEAVGameCallbacks@@AEBV?$NonOwnerPointer@VILevel@@@Bedrock@@AEAVNetworkHandler@@AEAVPrivateKeyManager@@AEAVServerLocator@@AEAVPacketSender@@AEAVAllowList@@PEAVPermissionsFile@@AEBVUUID@mce@@H_NAEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@HAEAVMinecraftCommands@@AEAVIMinecraftApp@@AEBV?$unordered_map@UPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$hash@UPackIdVersion@@@3@U?$equal_to@UPackIdVersion@@@3@V?$allocator@U?$pair@$$CBUPackIdVersion@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@@3@@std@@AEAVScheduler@@V?$NonOwnerPointer@VTextFilteringProcessor@@@3@@Z",
	ServerNetworkHandler* _this, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16, uintptr_t a17, uintptr_t a18, uintptr_t a19, uintptr_t a20) {
	global<ServerNetworkHandler> = _this;
	return original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
}
//Scoreboard的构造函数
THOOK(ServerScoreboard_construct, Scoreboard*, "??0ServerScoreboard@@QEAA@VCommandSoftEnumRegistry@@PEAVLevelStorage@@@Z",
	Scoreboard* _this, uintptr_t a1, uintptr_t a2) {
	return global<Scoreboard> = original(_this, a1, a2);
}
//改变设置命令的建立，用于注册命令
THOOK(ChangeSettingCommand_setup, void, "?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",
	uintptr_t _this) {
	for (auto& [cmd, des] : g_commands) {
		SymCall("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
			_this, &cmd, des.first.c_str(), 0, 0, 0x80/*CommandFlag*/);
	}
	original(_this);
}
#pragma endregion
#pragma region Listener
//开服完成
THOOK(onServerStarted, void, "?startServerThread@ServerInstance@@QEAAXXZ",
	uintptr_t _this) {
	EventCallBack(EventCode::onServerStarted, nullptr);
	thread(CheckPluginVersion).detach();
	original(_this);
}
//控制台输出，实际上是ostrram::operator<<的底层调用
THOOK(onConsoleOutput, ostream&, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
	ostream& _this, const char* str, uintptr_t size) {
	if (&_this == &cout) {
		if (!EventCallBack(EventCode::onConsoleOutput, "s", str))
			return _this;
	}
	return original(_this, str, size);
}
//控制台输入，实际上是命令队列的底层
THOOK(onConsoleInput, bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	SPSCQueue* _this, string* cmd) {
	static bool debug = false;
	if (*cmd == "pydebug") {
		if (debug) {
			debug = false;
		}
		else {
			debug = true;
			cout << '>';
		}
		return false;
	}
	if (debug) {
		Py_BEGIN_CALL;
		PyRun_SimpleString(cmd->c_str());
		Py_END_CALL;
		cout << '>';
		return false;
	}
	if (EventCallBack(EventCode::onConsoleInput, "O", StringToPyUnicode(*cmd)))
		return original(_this, cmd);
	return false;
}
//玩家加入发包
THOOK(onPlayerJoin, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id,/*SetLocalPlayerAsInitializedPacket*/ uintptr_t pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		EventCallBack(EventCode::onPlayerJoin, "O", ToEntity(p));
	}
	original(_this, id, pkt);
}
//玩家退出
THOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	uintptr_t _this, Player* p, char v3) {
	EventCallBack(EventCode::onPlayerLeft, "O", ToEntity(p));
	return original(_this, p, v3);
}
//使用物品
THOOK(onUseItem, bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	uintptr_t _this, ItemStack* item, BlockPos* bp, char a4, uintptr_t a5, Block* b) {
	Player* p = FETCH(Player*, _this + 8);
	short iid = item->getId();
	short iaux = item->getAuxValue();
	string iname = item->getName();
	BlockLegacy* bl = b->getBlockLegacy();
	short bid = bl->getBlockItemID();
	string bn = bl->getBlockName();
	if (EventCallBack(EventCode::onUseItem,
		"{s:O,s:i,s:i,s:s,s:s,s:i,s:O}",
		"player", ToEntity(p),
		"itemid", static_cast<int>(iid),
		"itemaux", static_cast<int>(iaux),
		"itemname", iname.c_str(),
		"blockname", bn.c_str(),
		"blockid", static_cast<int>(bid),
		"position", ToList(bp)
	))
		return original(_this, item, bp, a4, a5, b);
	return false;
}
//放置方块
THOOK(onPlaceBlock, bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* _this, Block* b, BlockPos* bp, unsigned char a4, Actor* p, bool _bool) {
	if (IsPlayer(p)) {
		BlockLegacy* bl = b->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		if (!EventCallBack(EventCode::onPlaceBlock,
			"{s:O,s:s,s:i,s:O}",
			"player", ToEntity(p),
			"blockname", bn.c_str(),
			"blockid", bid,
			"position", ToList(bp)
		))
			return false;
	}
	return original(_this, b, bp, a4, p, _bool);
}
//破坏方块
THOOK(onDestroyBlock, bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource* _this, Actor* a1, BlockPos* a2, ItemStack* a3, bool a4) {
	if (IsPlayer(a1)) {
		BlockLegacy* bl = _this->getBlock(a2)->getBlockLegacy();
		short bid = bl->getBlockItemID();
		string bn = bl->getBlockName();
		if (!EventCallBack(EventCode::onDestroyBlock,
			"{s:O,s:s,s:i,s:O}",
			"player", ToEntity(a1),
			"blockname", bn.c_str(),
			"blockid", static_cast<int>(bid),
			"position", ToList(a2)
		))
			return false;
	}
	return original(_this, a1, a2, a3, a4);
}
//开箱
THOOK(onOpenChest, bool, "?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	if (EventCallBack(EventCode::onOpenChest,
		"{s:O,s:O}",
		"player", ToEntity(p),
		"position", ToList(bp)
	))
		return original(_this, p, bp);
	return false;
}
//开桶，不能拦截，到方块交互才能拦截，不知道mojang员工在想什么
THOOK(onOpenBarrel, bool, "?use@BarrelBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	if (EventCallBack(EventCode::onOpenBarrel,
		"{s:O,s:O}",
		"player", ToEntity(p),
		"position", ToList(bp)
	))
		return original(_this, p, bp);
	return false;
}
//关箱
THOOK(onCloseChest, void, "?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	auto bp = (BlockPos*)(_this - 196);
	EventCallBack(EventCode::onCloseChest,
		"{s:O,s:O}",
		"player", ToEntity(p),
		"position", ToList(bp)
	);
	original(_this, p);
}
//关桶
THOOK(onCloseBarrel, void, "?stopOpen@BarrelBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	auto bp = (BlockPos*)(_this - 196);
	EventCallBack(EventCode::onCloseBarrel,
		"{s:O,s:O}",
		"player", ToEntity(p),
		"position", ToList(bp)
	);
	original(_this, p);
}
//放入取出物品，实际上是容器改变
THOOK(onContainerChange, void, "?containerContentChanged@LevelContainerModel@@UEAAXH@Z",
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
				"{s:O,s:s,s:i,s:O,s:i,s:i,s:s,s:i,s:i}",
				"player", ToEntity(p),
				"blockname", bl->getBlockName().c_str(),
				"blockid", bid,
				"position", ToList(bp),
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
THOOK(onAttack, bool, "?attack@Player@@UEAA_NAEAVActor@@AEBW4ActorDamageCause@@@Z",
	Player* p, Actor* a, struct ActorDamageCause* c) {
	//if (a) {
	//	a->setNameTag("傻逼");
	//	a->setNameTagVisible(true);
	//}
	if (EventCallBack(EventCode::onPlayerAttack,
		"{s:O,s:O}",
		"player", ToEntity(p),
		"actor", ToEntity(a)
	))
		return original(p, a, c);
	return false;
}
//切换纬度
THOOK(onChangeDimension, bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z",
	uintptr_t _this, Player* p, uintptr_t req) {
	bool result = original(_this, p, req);
	if (result) {
		EventCallBack(EventCode::onChangeDimension, "O", ToEntity(p));
	}
	return result;
}
//生物死亡
THOOK(onMobDie, void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z",
	Mob* _this, uintptr_t dmsg) {
	char v72;
	Actor* sa = _this->getLevel()->fetchEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 64))(dmsg, &v72)));
	bool res = EventCallBack(EventCode::onMobDie,
		"{s:I,s:O,s:O}",
		"dmcase", FETCH(unsigned, dmsg + 8),
		"actor1", ToEntity(_this),
		"actor2", ToEntity(sa)//可能为0
	);
	if (res) original(_this, dmsg);
}
//生物受伤
THOOK(onMobHurt, bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@H_N1@Z",
	Mob* _this, uintptr_t dmsg, int a3, bool a4, bool a5) {
	g_damage = a3;//将生物受伤的值设置为可调整
	char v72;
	Actor* sa = _this->getLevel()->fetchEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 64))(dmsg, &v72)));
	if (EventCallBack(EventCode::onMobHurt,
		"{s:i,s:O,s:O,s:i}",
		"dmcase", FETCH(unsigned, dmsg + 8),
		"actor1", ToEntity(_this),
		"actor2", ToEntity(sa),//可能为0
		"damage", a3
	))
		return original(_this, dmsg, g_damage, a4, a5);
	return false;
}
//玩家重生
THOOK(onRespawn, void, "?respawn@Player@@UEAAXXZ",
	Player* p) {
	EventCallBack(EventCode::onRespawn, "O", ToEntity(p));
	original(p);
}
//聊天，消息title msg w等...
THOOK(onChat, void, "?fireEventPlayerMessage@MinecraftEventing@@AEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@000@Z",
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
THOOK(onInputText, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*TextPacket*/uintptr_t pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	bool res = true;
	if (p) {
		const string& msg = FETCH(string, pkt + 88);
		res = EventCallBack(EventCode::onInputText,
			"{s:O,s:s}",
			"player", ToEntity(p),
			"msg", msg.c_str()
		);
	}
	if (res)original(_this, id, pkt);
}
//玩家输入命令
THOOK(onInputCommand, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*CommandRequestPacket*/uintptr_t pkt) {
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		const string& cmd = FETCH(string, pkt + 48);
		auto data = g_commands.find(cmd.c_str() + 1);
		//如果有这条命令且回调函数不为nullptr
		if (data != g_commands.end() && data->second.second) {
			Py_BEGIN_CALL;
			PyObject_CallFunction(data->second.second, "O", ToEntity(p));
			PrintPythonError();
			Py_END_CALL;
			return;
		}
		bool res = EventCallBack(EventCode::onInputCommand,
			"{s:O,s:s}",
			"player", ToEntity(p),
			"cmd", cmd.c_str()
		);
		if (res)original(_this, id, pkt);
	}
}
//玩家选择表单
THOOK(onSelectForm, void, "?handle@?$PacketHandlerDispatcherInstance@VModalFormResponsePacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
	uintptr_t _this, uintptr_t id, ServerNetworkHandler* handle,/*ModalFormResponsePacket*/uintptr_t* ppkt) {
	uintptr_t pkt = *ppkt;
	Player* p = handle->_getServerPlayer(id, pkt);
	if (p) {
		unsigned fid = FETCH(unsigned, pkt + 48);
		string data = FETCH(string, pkt + 56);
		if (data.back() == '\n')data.pop_back();
		EventCallBack(EventCode::onSelectForm,
			"{s:O,s:s,s:I}",
			"player", ToEntity(p),
			"selected", data.c_str(),
			"formid", fid
		);
	}
	original(_this, id, handle, ppkt);
}
//命令方块更新
THOOK(onCommandBlockUpdate, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*CommandBlockUpdatePacket*/uintptr_t pkt) {
	bool res = true;
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		BlockPos bp = FETCH(BlockPos, pkt + 48);
		unsigned short mode = FETCH(unsigned short, pkt + 60);
		bool condition = FETCH(bool, pkt + 62);
		bool redstone = FETCH(bool, pkt + 63);
		string cmd = FETCH(string, pkt + 72);
		string output = FETCH(string, pkt + 104);
		string rawname = FETCH(string, pkt + 136);
		int delay = FETCH(int, pkt + 168);
		res = EventCallBack(EventCode::onCommandBlockUpdate,
			"{s:O,s:i,s:i,s:i,s:s,s:s,s:s,s:i,s:O}",
			"player", ToEntity(p),
			"mode", mode,
			"condition", condition,
			"redstone", redstone,
			"cmd", cmd.c_str(),
			"output", output.c_str(),
			"rawname", rawname.c_str(),
			"delay", delay,
			"position", ToList(&bp)
		);
	}
	if (res)original(_this, id, pkt);
}
//爆炸
THOOK(onLevelExplode, bool, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* bs, Actor* a3, Vec3 pos, float a5, bool a6, bool a7, float a8, bool a9) {
	if (EventCallBack(EventCode::onLevelExplode,
		"{s:O,s:[f,f,f],s:i,s:f}",
		"actor", ToEntity(a3),
		"position", pos.x, pos.y, pos.z,
		"dimensionid", bs->getDimensionId(),
		"power", a5
	))
		return original(_this, bs, a3, pos, a5, a6, a7, a8, a9);
	return false;
}
//命令方块执行
THOOK(onCommandBlockPerform, bool, "?_execute@CommandBlock@@AEBAXAEAVBlockSource@@AEAVCommandBlockActor@@AEBVBlockPos@@_N@Z",
	uintptr_t _this, BlockSource* a2, uintptr_t a3, BlockPos* bp, bool a5) {
	//脉冲:0,重复:1,链:2
	int mode = SymCall<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z", a3, a2);
	//无条件:0,有条件:1
	bool condition = SymCall<bool>("?getConditionalMode@CommandBlockActor@@QEBA_NAEAVBlockSource@@@Z", a3, a2);
	//SymCall<string&>("?getName@BaseCommandBlock@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
	//	a3 + 200);
	//a3 + 200 BaseCommandBlock
	string cmd = FETCH(string, a3 + 256);
	string rawname = FETCH(string, a3 + 288);
	if (EventCallBack(EventCode::onCommandBlockPerform,
		"{s:i,s:b,s:s,s:s,s:O}",
		"mode", mode,
		"condition", condition,
		"cmd", cmd.c_str(),
		"rawname", rawname.c_str(),
		"position", ToList(bp)
	))
		return original(_this, a2, a3, bp, a5);
	return false;
}
//玩家移动
THOOK(onMove, void, "??0MovePlayerPacket@@QEAA@AEAVPlayer@@W4PositionMode@1@HH@Z",
	uintptr_t _this, Player* p, char a3, int a4, int a5) {
	EventCallBack(EventCode::onMove, "O", ToEntity(p));
	original(_this, p, a3, a4, a5);
}
////玩家移动
//THOOK(onMove, void, "?onMovePlayerPacketNormal@Player@@MEAAXAEBVVec3@@AEBVVec2@@M@Z",
//	Player* _this, Vec3* a1, Vec2* a2, float a3) {
//	EventCallBack(EventCode::onMove, "O", ToEntity(_this));
//	original(_this, a1, a2, a3);
//}
//玩家穿戴
THOOK(onSetArmor, void, "?setArmor@Player@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z",
	Player* p, unsigned slot, ItemStack* i) {
	if (!EventCallBack(EventCode::onSetArmor,
		"{s:O,s:i,s:i,s:s,s:i,s:i}",
		"player", ToEntity(p),
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
THOOK(onScoreChanged, void, "?onScoreChanged@ServerScoreboard@@UEAAXAEBUScoreboardId@@AEBVObjective@@@Z",
	Scoreboard* _this, ScoreboardId* a1, Objective* a2) {
	//创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	//修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	EventCallBack(EventCode::onScoreChanged,
		"{s:i,s:i,s:s,s:s}",
		"scoreboardid", a1->id,
		"playersnum", a2->getPlayerScore(a1)->getCount(),
		"objectivename", a2->getScoreName().c_str(),
		"objectivedisname", a2->getScoreDisplayName().c_str()
	);
	original(_this, a1, a2);
}
//耕地破坏
THOOK(onFallBlockTransform, void, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z",
	uintptr_t _this, BlockSource* a1, BlockPos* a2, Actor* p, uintptr_t a4) {
	if (IsPlayer(p)) {
		if (!EventCallBack(EventCode::onFallBlockTransform,
			"{s:O,s:O,s:i}",
			"player", ToEntity(p),
			"position", ToList(a2),
			"dimensionid", a1->getDimensionId()
		))
			return;
	}
	original(_this, a1, a2, p, a4);
}
//使用重生锚
THOOK(onUseRespawnAnchorBlock, bool, "?trySetSpawn@RespawnAnchorBlock@@CA_NAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* p, BlockPos* a2, BlockSource* a3, Level* a4) {
	if (EventCallBack(EventCode::onUseRespawnAnchorBlock,
		"{s:O,s:O,s:i}",
		"player", ToEntity(p),
		"position", ToList(a2),
		"dimensionid", a3->getDimensionId()
	))
		return original(p, a2, a3, a4);
	return false;
}
//活塞推
THOOK(onPistonPush, bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z",
	BlockActor* _this, BlockSource* bs, BlockPos* bp, unsigned a3, unsigned a4) {
	BlockLegacy* blg = bs->getBlock(bp)->getBlockLegacy();
	string bn = blg->getBlockName();
	short bid = blg->getBlockItemID();
	BlockPos* bp2 = _this->getPosition();
	if (EventCallBack(EventCode::onPistonPush,
		"{s:s,s:i,s:O,s:O,s:i}",
		"blockname", bn.c_str(),
		"blockid", bid,
		"blockpos", ToList(bp),
		"pistonpos", ToList(bp2),
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, bs, bp, a3, a4);
	return false;
}
//末影人随机传送（没人会用吧？）
THOOK(onEndermanRandomTeleport, bool, "?randomTeleport@TeleportComponent@@QEAA_NAEAVActor@@@Z",
	uintptr_t _this, Actor* a1) {
	if (EventCallBack(EventCode::onEndermanRandomTeleport, "O", ToEntity(a1)))
		return original(_this, a1);
	return false;
}
//丢物品
THOOK(onDropItem, bool, "?drop@Player@@UEAA_NAEBVItemStack@@_N@Z",
	Player* _this, ItemStack* a2, bool a3) {
	if (EventCallBack(EventCode::onDropItem,
		"{s:O,s:i,s:i,s:s,s:i}",
		"player", ToEntity(_this),
		"itemid", a2->getId(),
		"itemcount", a2->getCount(),
		"itemname", a2->getName().c_str(),
		"itemaux", a2->getAuxValue()
	))
		return original(_this, a2, a3);
	return false;
}
//拿物品
THOOK(onTakeItem, bool, "?take@Player@@QEAA_NAEAVActor@@HH@Z",
	Player* _this, Actor* actor, int a2, int a3) {
	if (EventCallBack(EventCode::onTakeItem,
		"{s:O,s:O}",
		"player", ToEntity(_this),
		"actor", ToEntity(actor)
	))
		return original(_this, actor, a2, a3);
	return false;
}
//骑
THOOK(onRide, bool, "?canAddPassenger@Actor@@UEBA_NAEAV1@@Z",
	Actor* a1, Actor* a2) {
	if (EventCallBack(EventCode::onRide,
		"{s:O,s:O}",
		"actor1", ToEntity(a1),
		"actor2", ToEntity(a2)
	))
		return original(a1, a2);
	return false;
}*/
//放入取出物品展示框的物品
THOOK(onUseFrameBlock, bool, "?use@ItemFrameBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	if (EventCallBack(EventCode::onUseFrameBlock,
		"{s:O,s:O,s:i}",
		"player", ToEntity(a2),
		"blockpos", ToList(a3),
		"dimensionid", a2->getDimensionId()
	))
		return original(_this, a2, a3);
	return false;
}
//点击物品展示框
THOOK(onUseFrameBlocka, bool, "?attack@ItemFrameBlock@@UEBA_NPEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	if (EventCallBack(EventCode::onUseFrameBlock,
		"{s:O,s:O,s:i}",
		"player", ToEntity(a2),
		"blockpos", ToList(a3),
		"dimensionid", a2->getDimensionId()
	))
		return original(_this, a2, a3);
	return false;
}
//玩家跳跃
THOOK(onJump, void, "?jumpFromGround@Player@@UEAAXXZ",
	Player* _this) {
	if (EventCallBack(EventCode::onJump, "O", ToEntity(_this)))
		return original(_this);
}
//玩家潜行
THOOK(onSneak, void, "?sendActorSneakChanged@ActorEventCoordinator@@QEAAXAEAVActor@@_N@Z",
	uintptr_t _this, Actor* a1, bool a2) {
	if (EventCallBack(EventCode::onSneak, "O", ToEntity(a1)))
		return original(_this, a1, a2);
}
//火势蔓延
THOOK(onFireSpread, bool, "?_trySpawnBlueFire@FireBlock@@AEBA_NAEAVBlockSource@@AEBVBlockPos@@@Z",
	uintptr_t _this, BlockSource* bs, BlockPos* bp) {
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	if (EventCallBack(EventCode::onFireSpread,
		"{s:O,s:s,s:i,s:i}",
		"blockpos", ToList(bp),
		"blockname", bl->getBlockName().c_str(),
		"blockid", bl->getBlockItemID(),
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, bs, bp);
	return false;
}
//方块交互（除箱子，工作台）
THOOK(onBlockInteracted, void, "?onBlockInteractedWith@VanillaServerGameplayEventListener@@UEAA?AW4EventResult@@AEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* pl, BlockPos* bp) {
	BlockSource* bs = global<Level>->getBlockSource(pl->getDimensionId());
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	if (EventCallBack(EventCode::onBlockInteracted,
		"{s:O,s:O,s:s,s:i,s:i}",
		"player", ToEntity(pl),
		"blockpos", ToList(bp),
		"blockname", bl->getBlockName().c_str(),
		"blockid", bl->getBlockItemID(),
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, pl, bp);
}
//方块被爆炸破坏
THOOK(onBlockExploded, void, "?onExploded@Block@@QEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@@Z",
	Block* _this, BlockSource* bs, BlockPos* bp, Actor* actor) {
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	if (EventCallBack(EventCode::onBlockExploded,
		"{s:O,s:O,s:s,s:i,s:i}",
		"actor", ToEntity(actor),
		"blockpos", ToList(bp),
		"blockname", bl->getBlockName().c_str(),
		"blockid", bl->getBlockItemID(),
		"dimensionid", bs->getDimensionId()
	))
		return original(_this, bs, bp, actor);
}
//使用牌子
THOOK(onUseSingBlock, uintptr_t, "?use@SignBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a1, BlockPos* a2) {
	BlockSource* bs = a1->getRegion();
	BlockActor* ba = bs->getBlockEntity(a2);
	string text;
	//获取沉浸式文本内容
	SymCall<string&>("?getImmersiveReaderText@SignBlockActor@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEAVBlockSource@@@Z",
		ba, &text, bs);
	if (EventCallBack(EventCode::onUseSignBlock,
		"{s:O,s:s,s:O}",
		"player", ToEntity(a1),
		"text", text.c_str(),
		"position", ToList(a2)
	))
		return original(_this, a1, a2);
	return 0;
}
#pragma endregion

void Init() {
	//如果目录不存在创建目录
	if (!fs::exists(PLUGIN_PATH))
		fs::create_directory(PLUGIN_PATH);
	if (!fs::exists(CACHE_PATH))
		fs::create_directory(CACHE_PATH);
	//设置模块搜索路径
	Py_SetPath(
		PLUGIN_PATH L";"
		PLUGIN_PATH "Dlls;"
		PLUGIN_PATH "Lib"
	);
#if 0
	//预初始化3.8+
	PyPreConfig cfg;
	PyPreConfig_InitPythonConfig(&cfg);
	cfg.utf8_mode = 1;
	cfg.configure_locale = 0;
	Py_PreInitialize(&cfg);
#endif
	//增加一个模块
	PyImport_AppendInittab("mc", mc_init);
	//初始化解释器
	Py_InitializeEx(0);
	//初始化类型
	if (PyType_Ready(&PyEntity_Type) < 0)
		Py_FatalError("Can't initialize entity type");
	//启用线程支持
	PyEval_InitThreads();
	for (auto& info : fs::directory_iterator(PLUGIN_PATH)) {
		//whether the file is py
		if (info.path().extension() == ".py") {
			string name(info.path().stem().u8string());
			//ignore files starting with '_'
			if (name.front() == '_')
				continue;
			cout << "[BDSpyrunner] Loading " << name << endl;
			PyImport_Import(StringToPyUnicode(name));
			PrintPythonError();
		}
	}
	//释放当前线程
	PyEval_SaveThread();
	//输出版本号信息
	cout << "[BDSpyrunner] " << PYR_VERSION << " loaded." << endl;
}
