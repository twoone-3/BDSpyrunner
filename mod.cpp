//mod.cpp 插件模块
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

#define PLUGIN_PATH "plugins\\py\\"

using namespace std;

namespace fs = filesystem;
#pragma region Function
#if 0
//Dll入口函数
BOOL WINAPI DllMain(
	HINSTANCE hinstDLL	/* handle to DLL module */,
	DWORD fdwReason		/* reason for calling function */,
	LPVOID lpReserved	/* reserved */
) {
	// Perform actions based on the reason for calling.
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;
	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;
	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
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
	ofstream file(filename.data(), ios::out | ios::binary);
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
#endif 
//事件回调助手
//创建会申请GIL
class EventCallBackHelper {
public:
	EventCallBackHelper(EventCode t) :
		type_(t), arg_(nullptr), gil_(PyGILState_Ensure()) {
	}
	~EventCallBackHelper() {
		if (arg_) {
			Py_XDECREF(arg_);
		}
		PyGILState_Release(gil_);
	}
	//事件回调
	bool call() {
		bool intercept = true;
		//如果没有则跳过
		auto& cbs = g_callback_functions[type_];
		//Py_BEGIN_CALL;
		//Py_XINCREF(arg_);
		for (auto cb : cbs) {
			PyObject* result = PyObject_CallFunction(cb, "O", arg_);
			PrintPythonError();
			if (result == Py_False)
				intercept = false;
			Py_XDECREF(result);
		}
		//Py_END_CALL;
		return intercept;
	}
	EventCallBackHelper& setArg(PyObject* arg) {
		arg_ = arg;
		return *this;
	}
	EventCallBackHelper& setDict() {
		arg_ = PyDict_New();
		return *this;
	}
	EventCallBackHelper& insert(string_view key, PyObject* item) {
		if (arg_ == nullptr)
			arg_ = PyDict_New();
		PyDict_SetItemString(arg_, key.data(), item);
		Py_DECREF(item);
		//Py_PRINT_REFCOUNT(item);
		return *this;
	}
	EventCallBackHelper& insert(string_view key, string_view item) {
		return insert(key, ToPyStr(item));
	}
	EventCallBackHelper& insert(string_view key, Actor* item) {
		return insert(key, ToEntity(item));
	}
	EventCallBackHelper& insert(string_view key, BlockPos* item) {
		return insert(key, ToList(item));
	}
	EventCallBackHelper& insert(string_view key, Vec3* item) {
		return insert(key, ToList(item));
	}
	EventCallBackHelper& insert(string_view key, short item) {
		return insert(key, PyLong_FromLong(item));
	}
	EventCallBackHelper& insert(string_view key, int item) {
		return insert(key, PyLong_FromLong(item));
	}
	EventCallBackHelper& insert(string_view key, unsigned item) {
		return insert(key, PyLong_FromUnsignedLong(item));
	}
	EventCallBackHelper& insert(string_view key, float item) {
		return insert(key, PyLong_FromDouble(item));
	}
private:
	EventCode type_;
	PyObject* arg_;
	PyGILState_STATE gil_;
};
#pragma endregion
#pragma region Hook List
//将Python解释器初始化插入bds主函数
THOOK(BDS_Main, int, "main",
	int argc, char* argv[], char* envp[]) {
#if 0
	while (true) {
		Tag* t = ObjecttoTag(StringToJson(R"(
	             {
	                "Block10": {
	                    "name8": "minecraft:crafting_table",
	                    "states10": null,
	                    "version3": 17879555
	                },
	                "Count1": 64,
	                "Damage2": 0,
	                "Name8": "minecraft:crafting_table",
	                "WasPickedUp1": 0,
	                "tag10": {
	                    "display10": {
	                        "Lore9": [
	                            "针不戳",
	                            "很不错"
	                        ]
	                    }
	                }
	            }
	)"));
		cout << CompoundTagtoJson(t).dump(4) << endl;
		t->deleteCompound();
		delete t;
	}
#endif
	//如果目录不存在创建目录
	if (!fs::exists(PLUGIN_PATH))
		fs::create_directory(PLUGIN_PATH);
	//设置模块搜索路径
	wstring py_path(PLUGIN_PATH L";"
					PLUGIN_PATH "Dlls;"
					PLUGIN_PATH "Lib;"
					PLUGIN_PATH "Extra;");
	py_path.append(Py_GetPath());
	Py_SetPath(py_path.c_str());
#if 0
	//预初始化3.8+
	PyPreConfig cfg;
	PyPreConfig_InitPythonConfig(&cfg);
	cfg.utf8_mode = 1;
	cfg.configure_locale = 0;
	Py_PreInitialize(&cfg);
#endif
	//增加一个模块
	if((fopen("plugins/py/mc.py", "r")) == NULL)
		PyImport_AppendInittab("mc", mc_init);
	else
		PyImport_AppendInittab("mco", mc_init);
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
			PyImport_ImportModule(name.c_str());
			PrintPythonError();
		}
	}
	// 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
	PyEval_ReleaseThread(PyThreadState_Get());
	//释放当前线程
	//PyEval_SaveThread();
	//输出版本号信息
	cout << "[BDSpyrunner] " << PYR_VERSION << " loaded." << endl;
	return original(argc, argv, envp);
}
//Level的构造函数
THOOK(Level_construct, Level*, "??0Level@@QEAA@AEBV?$not_null@V?$NonOwnerPointer@VSoundPlayerInterface@@@Bedrock@@@gsl@@V?$unique_ptr@VLevelStorage@@U?$default_delete@VLevelStorage@@@std@@@std@@V?$unique_ptr@VLevelLooseFileStorage@@U?$default_delete@VLevelLooseFileStorage@@@std@@@4@AEAVIMinecraftEventing@@_NEAEAVScheduler@@V?$not_null@V?$NonOwnerPointer@VStructureManager@@@Bedrock@@@2@AEAVResourcePackManager@@AEBV?$not_null@V?$NonOwnerPointer@VIEntityRegistryOwner@@@Bedrock@@@2@V?$WeakRefT@UEntityRefTraits@@@@V?$unique_ptr@VBlockComponentFactory@@U?$default_delete@VBlockComponentFactory@@@std@@@4@V?$unique_ptr@VBlockDefinitionGroup@@U?$default_delete@VBlockDefinitionGroup@@@std@@@4@V?$weak_ptr@VItemRegistry@@@4@V?$weak_ptr@VBlockTypeRegistry@@@4@V?$optional@VDimensionDefinitionGroup@@@4@@Z",
	Level* _this, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16) {
	return global<Level> = original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);
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
	if (global<RakPeer> == nullptr) {
		global<RakPeer> = original(_this);
		return global<RakPeer>;
	}
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
// Player构造函数
THOOK(Player_construct, Player*, "??0Player@@QEAA@AEAVLevel@@AEAVPacketSender@@W4GameType@@AEBVNetworkIdentifier@@EVUUID@mce@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCertificate@@U?$default_delete@VCertificate@@@std@@@8@AEAVEntityContext@@55@Z", void* _this, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, void* arg7, void* arg8, void* arg9, void* arg10, void* arg11) {
	//会构造两次，取第一次值
	if (global<std::vector<Player*>> == nullptr)
		global<std::vector<Player*>> = new std::vector<Player*>;
	auto ret = original(_this, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	global<std::vector<Player*>>->push_back(ret);
	return ret;
}
// Player析构函数
THOOK(Player_destruct, Player*, "??1Player@@UEAA@XZ", Player* _this) {
	for (std::vector<Player*>::iterator iter = global<std::vector<Player*>>->begin();
		iter != global<std::vector<Player*>>->end(); iter++) {
		if (*iter == _this) {
			global<std::vector<Player*>>->erase(iter);
			break;
		}
	}
	return original(_this);
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
	EventCallBackHelper h(EventCode::onServerStarted);
	h.setArg(Py_None).call();
	original(_this);
}
//控制台输出，实际上是ostrram::operator<<的底层调用
THOOK(onConsoleOutput, ostream&, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
	ostream& _this, const char* str, uintptr_t size) {
	EventCallBackHelper h(EventCode::onConsoleOutput);
	if (&_this == &cout) {
		h.setArg(ToPyStr(str));
		if (!h.call())
			return _this;
	}
	return original(_this, str, size);
}
//控制台输入，实际上是命令队列的底层
THOOK(onConsoleInput, bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	SPSCQueue* _this, string* cmd) {
	EventCallBackHelper h(EventCode::onConsoleInput);
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
		//Py_BEGIN_CALL;
		PyRun_SimpleString(cmd->c_str());
		//Py_END_CALL;
		cout << '>';
		return false;
	}
	h.setArg(ToPyStr(*cmd));
	if (h.call())
		return original(_this, cmd);
	return false;
}
//玩家加入发包
THOOK(onPlayerJoin, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id,/*SetLocalPlayerAsInitializedPacket*/ uintptr_t pkt) {
	EventCallBackHelper h(EventCode::onPlayerJoin);
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		h.setArg(ToEntity(p)).call();
	}
	original(_this, id, pkt);
}
//玩家退出
THOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	uintptr_t _this, Player* p, char v3) {
	EventCallBackHelper h(EventCode::onPlayerLeft);
	h.setArg(ToEntity(p));
	h.call();
	return original(_this, p, v3);
}
//使用物品
THOOK(onUseItem, bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	uintptr_t _this, ItemStack* item, BlockPos* bp, char a4, uintptr_t a5, Block* b) {
	EventCallBackHelper h(EventCode::onUseItem);
	Player* p = Dereference<Player*>(_this, 8);
	BlockLegacy* bl = b->getBlockLegacy();
	h
		.insert("player", p)
		.insert("itemid", item->getId())
		.insert("itemaux", item->getAuxValue())
		.insert("itemcount", item->getCount())
		.insert("itemname", item->getName())
		.insert("blockname", bl->getBlockName())
		.insert("blockid", bl->getBlockItemID())
		.insert("position", bp);
	if (h.call())
		return original(_this, item, bp, a4, a5, b);
	return false;
}
//放置方块
THOOK(onPlaceBlock, bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* _this, Block* b, BlockPos* bp, unsigned char a4, Actor* p, bool _bool) {
	EventCallBackHelper h(EventCode::onPlaceBlock);
	if (IsPlayer(p)) {
		BlockLegacy* bl = b->getBlockLegacy();
		h
			.insert("player", p)
			.insert("blockname", bl->getBlockName())
			.insert("blockid", bl->getBlockItemID())
			.insert("position", bp);
		if (!h.call())
			return false;
	}
	return original(_this, b, bp, a4, p, _bool);
}
//破坏方块
THOOK(onDestroyBlock, bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource* _this, Actor* a1, BlockPos* a2, ItemStack* a3, bool a4) {
	EventCallBackHelper h(EventCode::onDestroyBlock);
	if (IsPlayer(a1)) {
		BlockLegacy* bl = _this->getBlock(a2)->getBlockLegacy();
		h
			.insert("player", a1)
			.insert("blockname", bl->getBlockName())
			.insert("blockid", bl->getBlockItemID())
			.insert("position", a2);
		if (!h.call())
			return false;
	}
	return original(_this, a1, a2, a3, a4);
}
//开箱
THOOK(onOpenChest, bool, "?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	EventCallBackHelper h(EventCode::onOpenChest);
	h
		.insert("player", p)
		.insert("position", bp);
	if (h.call())
		return original(_this, p, bp);
	return false;
}
//开桶，不能拦截，拦截请看方块交互
THOOK(onOpenBarrel, bool, "?use@BarrelBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	EventCallBackHelper h(EventCode::onOpenBarrel);
	h
		.insert("player", p)
		.insert("position", bp);
	if (h.call())
		return original(_this, p, bp);
	return false;
}
//关箱
THOOK(onCloseChest, void, "?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	EventCallBackHelper h(EventCode::onCloseChest);
	auto bp = (BlockPos*)(_this - 196);
	h
		.insert("player", p)
		.insert("position", bp);
	original(_this, p);
}
//关桶
THOOK(onCloseBarrel, void, "?stopOpen@BarrelBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	EventCallBackHelper h(EventCode::onCloseBarrel);
	auto bp = (BlockPos*)(_this - 196);
	h
		.insert("player", p)
		.insert("position", bp);
	original(_this, p);
}
//放入取出物品，实际上是容器改变
THOOK(onContainerChange, void, "?containerContentChanged@LevelContainerModel@@UEAAXH@Z",
	uintptr_t _this, unsigned slot) {
	EventCallBackHelper h(EventCode::onContainerChange);
	//EventCallBackHelper h(EventCode::onTest);
	Player* p = Dereference<Player*>(_this, 208);//IDA LevelContainerModel::_getContainer line 15 25 v3
	BlockSource* bs = p->getRegion();
	BlockPos* bp = reinterpret_cast<BlockPos*>(_this + 216);
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	short bid = bl->getBlockItemID();
	if (bid == 54 || bid == 130 || bid == 146 || bid == -203 || bid == 205 || bid == 218) {	//非箱子、桶、潜影盒的情况不作处理
		uintptr_t v5 = (*reinterpret_cast<uintptr_t(**)(uintptr_t)>(Dereference<uintptr_t>(_this) + 160))(_this);
		//cout << v5 << endl;
		if (v5) {
			ItemStack* item = (*reinterpret_cast<ItemStack * (**)(uintptr_t, uintptr_t)>(Dereference<uintptr_t>(v5) + 40))(v5, slot);
			h
				.insert("player", p)
				.insert("itemid", item->getId())
				.insert("itemaux", item->getAuxValue())
				.insert("itemcount", item->getCount())
				.insert("itemname", item->getName())
				.insert("blockname", bl->getBlockName())
				.insert("blockid", bid)
				.insert("position", bp)
				.insert("slot", slot);
				//cout << "slot:" << slot << endl;
			h.call();
		}
	}
	original(_this, slot);
}
//玩家攻击
THOOK(onAttack, bool, "?attack@Player@@UEAA_NAEAVActor@@AEBW4ActorDamageCause@@@Z",
	Player* p, Actor* a, struct ActorDamageCause* c) {
	EventCallBackHelper h(EventCode::onPlayerAttack);
	h	
		.insert("player", p)
		.insert("actor", a);
	if (h.call())
		return original(p, a, c);
	return false;
}
//切换纬度
THOOK(onChangeDimension, bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z",
	uintptr_t _this, Player* p, uintptr_t req) {
	EventCallBackHelper h(EventCode::onChangeDimension);
	bool result = original(_this, p, req);
	if (result) {
		h.setArg(ToEntity(p)).call();
	}
	return result;
}
//生物死亡
THOOK(onMobDie, void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z",
	Mob* _this, uintptr_t dmsg) {
	EventCallBackHelper h(EventCode::onMobDie);
	char v72;
	//IDA Mob::die Line163  v16 = (_QWORD *)(*(__int64 (__fastcall **)(const struct ActorDamageSource *, char *))(*(_QWORD *)v2 + 88i64))(v2, & v91);
	Actor* sa = _this->getLevel()->fetchEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 88))(dmsg, &v72)));
	h
		.insert("actor1", _this)
		.insert("actor2", sa)
		.insert("dmcase", Dereference<unsigned>(dmsg, 8))
		;
	if (h.call())
		original(_this, dmsg);
}
//生物受伤
THOOK(onMobHurt, bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@M_N1@Z",
	Mob* _this, uintptr_t dmsg, float a3, bool a4, bool a5) {
	EventCallBackHelper h(EventCode::onMobHurt);
	g_damage = a3;//将生物受伤的值设置为可调整
	char v72;
	// getSourceUniqueId
	Actor* sa = _this->getLevel()->fetchEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 88))(dmsg, &v72)));
	h
		.insert("actor1", _this)
		.insert("actor2", sa)
		.insert("dmcase", Dereference<unsigned>(dmsg, 8))
		.insert("damage", a3)
		;
	if (h.call())
		return original(_this, dmsg, g_damage, a4, a5);
	return false;
}
//玩家重生
THOOK(onRespawn, void, "?respawn@Player@@UEAAXXZ",
	Player* p) {
	EventCallBackHelper h(EventCode::onRespawn);
	h.setArg(ToEntity(p)).call();
	original(p);
}
//聊天，消息title msg w等...
THOOK(onChat, void, "?fireEventPlayerMessage@MinecraftEventing@@AEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@000@Z",
	uintptr_t _this, string& sender, string& target, string& msg, string& style) {
	EventCallBackHelper h(EventCode::onChat);
	h.insert("sender", sender)
		.insert("target", target)
		.insert("msg", msg)
		.insert("style", style);
	h.call();
	original(_this, sender, target, msg, style);
}
//玩家输入文本
THOOK(onInputText, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*TextPacket*/uintptr_t pkt) {
	EventCallBackHelper h(EventCode::onInputText);
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		const string& msg = Dereference<string>(pkt, 88);
		h.insert("player", p)
			.insert("msg", msg);
		if (!h.call())
			return;
	}
	original(_this, id, pkt);
}
//玩家输入命令
THOOK(onInputCommand, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*CommandRequestPacket*/uintptr_t pkt) {
	EventCallBackHelper h(EventCode::onInputCommand);
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		const string& cmd = Dereference<string>(pkt, 48);
		auto data = g_commands.find(cmd.c_str() + 1);
		//如果有这条命令且回调函数不为nullptr
		if (data != g_commands.end() && data->second.second) {
			//Py_BEGIN_CALL;
			PyObject_CallFunction(data->second.second, "O", ToEntity(p));
			PrintPythonError();
			//Py_END_CALL;
			return;
		}
		h.insert("player", p)
			.insert("cmd", cmd);
		if (h.call())
			original(_this, id, pkt);
	}
}
//玩家选择表单
THOOK(onSelectForm, void, "?handle@?$PacketHandlerDispatcherInstance@VModalFormResponsePacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
	uintptr_t _this, uintptr_t id, ServerNetworkHandler* handle,/*ModalFormResponsePacket*/uintptr_t* ppkt) {
	EventCallBackHelper h(EventCode::onSelectForm);
	uintptr_t pkt = *ppkt;
	Player* p = handle->_getServerPlayer(id, pkt);
	if (p) {
		unsigned fid = Dereference<unsigned>(pkt, 48);
		string data = Dereference<string>(pkt, 56);
		if (data.back() == '\n')
			data.pop_back();
		h
			.insert("player", p)
			.insert("selected", data)
			.insert("formid", fid);
		h.call();
	}
	original(_this, id, handle, ppkt);
}
//命令方块更新
THOOK(onCommandBlockUpdate, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
	ServerNetworkHandler* _this, uintptr_t id, /*CommandBlockUpdatePacket*/uintptr_t pkt) {
	EventCallBackHelper h(EventCode::onCommandBlockUpdate);
	Player* p = _this->_getServerPlayer(id, pkt);
	if (p) {
		BlockPos bp = Dereference<BlockPos>(pkt, 48);
		unsigned short mode = Dereference<unsigned short>(pkt, 60);
		bool condition = Dereference<bool>(pkt, 62);
		bool redstone = Dereference<bool>(pkt, 63);
		string cmd = Dereference<string>(pkt, 72);
		string output = Dereference<string>(pkt, 104);
		string rawname = Dereference<string>(pkt, 136);
		int delay = Dereference<int>(pkt, 168);
		h.insert("player", ToEntity(p))
			.insert("mode", mode)
			.insert("condition", condition)
			.insert("redstone", redstone)
			.insert("cmd", cmd)
			.insert("output", output)
			.insert("rawname", rawname)
			.insert("delay", delay)
			.insert("position", &bp);
		if (!h.call())
			return;
	}
	original(_this, id, pkt);
}
//爆炸
THOOK(onLevelExplode, bool, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* bs, Actor* a3, Vec3 pos, float a5, bool a6, bool a7, float a8, bool a9) {
	EventCallBackHelper h(EventCode::onLevelExplode);
	h
		.insert("actor", a3)
		.insert("position", &pos)
		.insert("dimensionid", bs->getDimensionId())
		.insert("power", a5);
	if (h.call())
		return original(_this, bs, a3, pos, a5, a6, a7, a8, a9);
	return false;
}
//命令方块执行
THOOK(onCommandBlockPerform, bool, "?_execute@CommandBlock@@AEBAXAEAVBlockSource@@AEAVCommandBlockActor@@AEBVBlockPos@@_N@Z",
	uintptr_t _this, BlockSource* a2, uintptr_t a3, BlockPos* bp, bool a5) {
	EventCallBackHelper h(EventCode::onCommandBlockPerform);
	//脉冲:0,重复:1,链:2
	int mode = SymCall<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z", a3, a2);
	//无条件:0,有条件:1
	bool condition = SymCall<bool>("?getConditionalMode@CommandBlockActor@@QEBA_NAEAVBlockSource@@@Z", a3, a2);
	//SymCall<string&>("?getName@BaseCommandBlock@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
	//	a3 + 200);
	//a3 + 200 BaseCommandBlock
	string cmd = Dereference<string>(a3, 256);
	string rawname = Dereference<string>(a3, 288);
	h
		.insert("mode", mode)
		.insert("condition", condition)
		.insert("cmd", cmd)
		.insert("rawname", rawname)
		.insert("position", bp);
	if (h.call())
		return original(_this, a2, a3, bp, a5);
	return false;
}
//玩家移动
THOOK(onMove, void*, "??0MovePlayerPacket@@QEAA@AEBVPlayer@@W4PositionMode@1@HH@Z",
	uintptr_t _this, Player* p, char a3, int a4, int a5) {
	/*
	Vec3* deltaPos = p->getPosDelta();
	//Vec3* prevPos = p->getPosPrev();
	//Vec3* pos = p->getPos();
	//cout << "lastPos: " << deltaPos->x << ", " << deltaPos->y << ", " << deltaPos->z << endl;
	//cout << "prevPos: " << prevPos->x << ", " << prevPos->y << ", " << prevPos->z << endl;
	//cout << "CurrPos: " << pos->x << ", " << pos->y << ", " << pos->z << endl;
	if (p->isStanding()) {
		if (deltaPos->x < 0.01 && float(deltaPos->y) == float(-0.0784) && deltaPos->z < 0.01)
			if(deltaPos->x > -0.01 && deltaPos->z > -0.01)
				return original(_this, p, a3, a4, a5);
	}else {
		if (deltaPos->x < 0.01 && deltaPos->y < 0.01 && deltaPos->z < 0.01)
			if (deltaPos->x > -0.01 && deltaPos->y > -0.01 && deltaPos->z > -0.01)
				return original(_this, p, a3, a4, a5);
	}
	*/
	//if (lastPos->x != pos->x || lastPos->y != pos->y || lastPos->z != pos->z) { //最开始的过滤代码，因为getPos()和getPosPrev()获取到的值一样而无效
		EventCallBackHelper h(EventCode::onMove);
		h.setArg(ToEntity(p)).call();
		return original(_this, p, a3, a4, a5);
	//}
	return original(_this, p, a3, a4, a5);
}
//玩家穿戴
THOOK(onSetArmor, void, "?setArmor@Player@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z",
	Player* p, unsigned slot, ItemStack* i) {
	EventCallBackHelper h(EventCode::onSetArmor);
	h
		.insert("player", ToEntity(p))
		.insert("itemid", i->getId())
		.insert("itemcount", i->getCount())
		.insert("itemname", i->getName())
		.insert("itemaux", i->getAuxValue())
		.insert("slot", slot);
	if (h.call())
		return original(p, slot, i);
	return;
}
//计分板改变监听
THOOK(onScoreChanged, void, "?onScoreChanged@ServerScoreboard@@UEAAXAEBUScoreboardId@@AEBVObjective@@@Z",
	Scoreboard* _this, ScoreboardId* a1, Objective* a2) {
	//cout << "ok" << endl;
	// output ok but py plugin no output
	//创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	//修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	EventCallBackHelper h(EventCode::onScoreChanged);
	h.insert("scoreboardid", a1->id)
		.insert("playersnum", a2->getPlayerScore(a1)->getCount())
		.insert("objectivename", a2->getScoreName())
		.insert("objectivedisname", a2->getScoreDisplayName());
	original(_this, a1, a2);
}
//耕地破坏
THOOK(onFallBlockTransform, void, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z",
	uintptr_t _this, BlockSource* a1, BlockPos* a2, Actor* p, uintptr_t a4) {
	EventCallBackHelper h(EventCode::onFallBlockTransform);
	if (IsPlayer(p)) {
		h.insert("player", p)
			.insert("position", a2)
			.insert("dimensionid", a1->getDimensionId());
		if (!h.call())
			return;
	}
	original(_this, a1, a2, p, a4);
}
//使用重生锚
THOOK(onUseRespawnAnchorBlock, bool, "?trySetSpawn@RespawnAnchorBlock@@CA_NAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* p, BlockPos* a2, BlockSource* a3, Level* a4) {
	EventCallBackHelper h(EventCode::onUseRespawnAnchorBlock);
	h.insert("player", ToEntity(p))
		.insert("position", ToList(a2))
		.insert("dimensionid", a3->getDimensionId());
	if (h.call())
		return original(p, a2, a3, a4);
	return false;
}
//活塞推
THOOK(onPistonPush, bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z",
	BlockActor* _this, BlockSource* bs, BlockPos* bp, unsigned a3, unsigned a4) {
	EventCallBackHelper h(EventCode::onPistonPush);
	BlockLegacy* blg = bs->getBlock(bp)->getBlockLegacy();
	string bn = blg->getBlockName();
	short bid = blg->getBlockItemID();
	//BlockPos* bp2 = _this->getPosition();//will cause a crash
	h.insert("blockname", bn)
		.insert("blockid", bid)
		.insert("blockpos", bp)
		//.insert("pistonpos", bp2)//will cause a crash
		.insert("dimensionid", bs->getDimensionId());
	if (h.call())
		return original(_this, bs, bp, a3, a4);
	return false;
}
//末影人随机传送（没人会用吧？）
THOOK(onEndermanRandomTeleport, bool, "?randomTeleport@TeleportComponent@@QEAA_NAEAVActor@@@Z",
	uintptr_t _this, Actor* a1) {
	EventCallBackHelper h(EventCode::onEndermanRandomTeleport);
	h.setArg(ToEntity(a1)).call();
	if (h.call())
		return original(_this, a1);
	return false;
}
//丢物品
THOOK(onDropItem, bool, "?drop@Player@@UEAA_NAEBVItemStack@@_N@Z",
	Player* _this, ItemStack* a2, bool a3) {
	EventCallBackHelper h(EventCode::onDropItem);
	h.insert("player", _this)
		.insert("itemid", a2->getId())
		.insert("itemcount", a2->getCount())
		.insert("itemname", a2->getName())
		.insert("itemaux", a2->getAuxValue());
	if (h.call())
		return original(_this, a2, a3);
	return false;
}
//拿物品
THOOK(onTakeItem, bool, "?take@Player@@QEAA_NAEAVActor@@HH@Z",
	Player* _this, Actor* actor, int a2, int a3) {
	EventCallBackHelper h(EventCode::onTakeItem);
	h.insert("player", _this)
		.insert("actor", actor);
	if (h.call())
		return original(_this, actor, a2, a3);
	return false;
}
//骑
THOOK(onRide, bool, "?canAddPassenger@Actor@@UEBA_NAEAV1@@Z",
	Actor* a1, Actor* a2) {
	EventCallBackHelper h(EventCode::onRide);
	h.insert("actor1", a1)
		.insert("actor2", a2);
	if (h.call())
		return original(a1, a2);
	return false;
}
//放入取出物品展示框的物品
THOOK(onUseFrameBlock, bool, "?use@ItemFrameBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	EventCallBackHelper h(EventCode::onUseFrameBlock);
	h.insert("player", a2)
		.insert("blockpos", a3)
		.insert("dimensionid", a2->getDimensionId());
	if (h.call())
		return original(_this, a2, a3);
	return false;
}
//点击物品展示框
THOOK(onUseFrameBlock2, bool, "?attack@ItemFrameBlock@@UEBA_NPEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	EventCallBackHelper h(EventCode::onUseFrameBlock);
	h.insert("player", a2)
		.insert("blockpos", a3)
		.insert("dimensionid", a2->getDimensionId());
	if (h.call())
		return original(_this, a2, a3);
	return false;
}
//玩家跳跃
THOOK(onJump, void, "?jumpFromGround@Player@@UEAAXXZ",
	Player* _this) {
	EventCallBackHelper h(EventCode::onJump);
	h.setArg(ToEntity(_this));
	if (h.call())
		return original(_this);
}
//玩家潜行
THOOK(onSneak, void, "?sendActorSneakChanged@ActorEventCoordinator@@QEAAXAEAVActor@@_N@Z",
	uintptr_t _this, Actor* a1, bool a2) {
	EventCallBackHelper h(EventCode::onSneak);
	h.setArg(ToEntity(a1));
	if (h.call())
		return original(_this, a1, a2);
}
//火势蔓延
THOOK(onFireSpread, bool, "?_trySpawnBlueFire@FireBlock@@AEBA_NAEAVBlockSource@@AEBVBlockPos@@@Z",
	uintptr_t _this, BlockSource* bs, BlockPos* bp) {
	EventCallBackHelper h(EventCode::onFireSpread);
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	h.insert("blockpos", bp)
		.insert("blockname", bl->getBlockName())
		.insert("blockid", bl->getBlockItemID())
		.insert("dimensionid", bs->getDimensionId());
	if (h.call())
		return original(_this, bs, bp);
	return false;
}
//方块交互（除箱子，工作台）
THOOK(onBlockInteracted, void, "?onBlockInteractedWith@VanillaServerGameplayEventListener@@UEAA?AW4EventResult@@AEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* pl, BlockPos* bp) {
	EventCallBackHelper h(EventCode::onBlockInteracted);
	BlockSource* bs = global<Level>->getBlockSource(pl->getDimensionId());
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	h.insert("player", pl)
		.insert("blockpos", bp)
		.insert("blockname", bl->getBlockName())
		.insert("blockid", bl->getBlockItemID())
		.insert("dimensionid", bs->getDimensionId());
	if (h.call())
		return original(_this, pl, bp);
}
//方块被爆炸破坏
THOOK(onBlockExploded, void, "?onExploded@Block@@QEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@@Z",
	Block* _this, BlockSource* bs, BlockPos* bp, Actor* actor) {
	EventCallBackHelper h(EventCode::onBlockExploded);
	BlockLegacy* bl = bs->getBlock(bp)->getBlockLegacy();
	h.insert("actor", actor)
		.insert("blockpos", bp)
		.insert("blockname", bl->getBlockName())
		.insert("blockid", bl->getBlockItemID())
		.insert("dimensionid", bs->getDimensionId());
	if (h.call())
		return original(_this, bs, bp, actor);
}
//使用牌子
THOOK(onUseSingBlock, uintptr_t, "?use@SignBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a1, BlockPos* a2) {
	EventCallBackHelper h(EventCode::onUseSignBlock);
	BlockSource* bs = a1->getRegion();
	BlockActor* ba = bs->getBlockEntity(a2);
	string text;
	//获取沉浸式文本内容
	SymCall<string&>("?getImmersiveReaderText@SignBlockActor@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEAVBlockSource@@@Z",
		ba, &text, bs);
	h.insert("player", a1)
		.insert("text", text)
		.insert("position", a2);
	if (h.call())
		return original(_this, a1, a2);
	return 0;
}
#pragma endregion
