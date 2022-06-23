#include <API/EventAPI.h>
#include <Global.hpp>
#include <LoggerAPI.h>

using namespace std;

constexpr unsigned PYR_VERSION_MAJOR = 2;
constexpr unsigned PYR_VERSION_MINOR = 0;
constexpr unsigned PYR_VERSION_MICRO = 0;
constexpr const char* PYR_VERSION = "v2.0.0";

void entry() {
	PY_TRY;
	namespace fs = filesystem;
	//如果目录不存在创建目录
	if (!fs::exists(PLUGIN_PATH))
		fs::create_directory(PLUGIN_PATH);
	//设置模块搜索路径
	wstring plugins_path = PLUGIN_PATH L";";
	plugins_path.append(Py_GetPath());
	Py_SetPath(plugins_path.c_str());
	//初始化解释器
	py::initialize_interpreter();
	//输出版本号信息
	logger.info("{} loaded.", PYR_VERSION);
	//启用线程支持
	PyEval_InitThreads();
#if 0
	auto a = CompoundTag::fromSNBT(R"({
    "a24z": {
        "a4z": {},
        "az": [1f, 1f, 1f],
        "az3": "az2"
    },
    "列表": []
})");
	logger.warn("{}", a->get("列表")->toJson(4));
#endif
	for (auto& info : fs::directory_iterator(PLUGIN_PATH)) {
		if (info.path().extension() == ".py") {
			string name(info.path().stem().u8string());
			//忽略以'_'开头的文件
			if (name.front() == '_') {
				logger.debug("Ignoring {}", name);
				continue;
			}
			else {
				PY_TRY;
				logger.info("Loading {}", name);
				py::module_::import(name.c_str());
				PY_CATCH;
			}
		}
	}
	//启动子线程前执行，释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
	PyEval_ReleaseThread(PyThreadState_Get());
	PY_CATCH;
}

THook(int, "main", int argc, char* argv[], char* envp[]) {
	entry();
	return original(argc, argv, envp);
}

extern "C" _declspec(dllexport) void onPostInit() {
	std::ios::sync_with_stdio(false);
	LL::registerPlugin(
		"BDSpyrunner",
		"Loading python in BDS",
		LL::Version(PYR_VERSION_MAJOR,
			PYR_VERSION_MINOR,
			PYR_VERSION_MICRO,
			LL::Version::Release),
		{{"Author", "twoone3"}});
}
