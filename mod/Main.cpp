#include "Common.h"

#define PLUGIN_PATH "plugins\\py\\"

using namespace std;

namespace fs = filesystem;

bool IsPlayer(Actor* ptr) {
	if (ptr == nullptr)
		return false;
	if (ptr->getEntityTypeId() != 319)
		return false;
	return true;
}
//字符串转JSON，本插件采用 https://json.nlohmann.me 的JSON库
fifo_json StrToJson(std::string_view str) {
	try {
		return fifo_json::parse(str);
	}
	catch (const std::exception& e) {
		logger.error("Parsing JSON failed! {}", e.what());
		return nullptr;
	}
}
//将Python解释器初始化插入bds主函数
THook(int, "main", int argc, char* argv[], char* envp[]) {
	//如果目录不存在创建目录
	if (!fs::exists(PLUGIN_PATH))
		fs::create_directory(PLUGIN_PATH);
	//设置模块搜索路径
	wstring plugins_path = PLUGIN_PATH L";";
	plugins_path.append(Py_GetPath());
	Py_SetPath(plugins_path.c_str());
#if 0
	//预初始化3.8+
	PyPreConfig cfg;
	PyPreConfig_InitPythonConfig(&cfg);
	cfg.utf8_mode = 1;
	cfg.configure_locale = 0;
	Py_PreInitialize(&cfg);
#endif
	//增加一个模块
	//PyImport_AppendInittab("mc", McInit);
	//初始化解释器
	py::initialize_interpreter();
	//输出版本号信息
	logger.info("{} loaded.", PYR_VERSION);
	//初始化类型
	//PyClassInit();
	//启用线程支持
	PyEval_InitThreads();
	try {
		for (auto& info : fs::directory_iterator(PLUGIN_PATH)) {
			if (info.path().extension() == ".py") {
				string name(info.path().stem().u8string());
				//忽略以'_'开头的文件
				if (name.front() == '_') {
					logger.info("Ignoring {}", name);
					continue;
				}
				else {
					logger.info("Loading {}", name);
					auto m = py::module_::import(name.c_str());
				}
			}
		}
	}
	catch (const std::exception& e) {
		logger.error(e.what());
	}
	//启动子线程前执行，释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
	PyEval_ReleaseThread(PyThreadState_Get());
	//注册命令监听
	Event::RegCmdEvent::subscribe(
		[] (Event::RegCmdEvent e) {
		for (auto& [cmd, des] : g_commands) {
			e.mCommandRegistry->registerCommand(cmd, des.first.c_str(),
				CommandPermissionLevel::Any, {CommandFlagValue::None},
				{static_cast<CommandFlagValue>(0x80)});
		}
		return true;
	}
	);
	//命令监听
	Event::PlayerCmdEvent::subscribe(
		[] (Event::PlayerCmdEvent e) {
		for (auto& [cmd, data] : g_commands) {
			if (e.mCommand._Starts_with(cmd)) {
				data.second(e.mPlayer, e.mCommand);
				return false;
			}
		}
		return true;
	}
	);
	return original(argc, argv, envp);
}
//Dll主函数
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	hinstDLL;
	lpReserved;
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		//Initialize once for each new process.
		//Return FALSE to fail DLL load.
		LL::registerPlugin(
			"BDSpyrunner", "For .py plugins' loading",
			LL::Version(PYR_VERSION_MAJOR, PYR_VERSION_MINOR, PYR_VERSION_MICRO, LL::Version::Release),
			{{"Author", "twoone3"}}
		);
		break;
	case DLL_THREAD_ATTACH:
		//Do thread-specific initialization.
		break;
	case DLL_THREAD_DETACH:
		//Do thread-specific cleanup.
		break;
	case DLL_PROCESS_DETACH:
		//Perform any necessary cleanup.
		break;
	}
	return TRUE;//Successful DLL_PROCESS_ATTACH.
}

#if 0
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
	}
	while (size);
	InternetCloseHandle(handle2);
	InternetCloseHandle(hSession);
	return StrToJson(data);
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
	}
	while (size);
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
#if 0
class PyGILGuard {
public:
	PyGILGuard() { gil_ = PyGILState_Ensure(); }
	~PyGILGuard() { PyGILState_Release(gil_); }

private:
	PyGILState_STATE gil_;
};
#endif
