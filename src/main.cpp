#include <LLAPI.h>
#include <LoggerAPI.h>

#include "api/event.h"
#include "api/money.h"
#include "py_utils.h"

constexpr unsigned PYR_VERSION_MAJOR = 2;
constexpr unsigned PYR_VERSION_MINOR = 1;
constexpr unsigned PYR_VERSION_MICRO = 1;
constexpr const char* PYR_VERSION = "v2.1.1";

Logger logger("BDSpyrunner");

void entry() {
  namespace fs = std::filesystem;
  PY_TRY;
  // 初始化LLMoney
  EconomySystem::init();
  // 创建插件目录
  if (!fs::exists(PLUGIN_PATH)) {
    fs::create_directory(PLUGIN_PATH);
    logger.warn("create plugin directory: {}", PLUGIN_PATH);
  }
  // 初始化解释器
  py::initialize_interpreter(true, 0, nullptr, false);
  // 添加PLUGIN_PATH到sys.path
  py::module_::import("sys").attr("path").attr("insert")(0, PLUGIN_PATH);
  // 输出版本号信息
  logger.info("{} loaded.", PYR_VERSION);
#if 0
  auto a = CompoundTag::fromSNBT(R"({
    "a24z": {
        "a4z": {},
        "az": [1f, 1f, 1f],
        "az3": "az2"
    },
    "列表": []
})");
  logger.warn("{}", a->value()["列表"].get()->toJson(4));
#endif
  for (auto& info : fs::directory_iterator(PLUGIN_PATH)) {
    if (info.path().extension() == ".py") {
      string name(info.path().stem().u8string());
      // 忽略以'_'开头的文件
      if (name.front() == '_') {
        logger.debug("Ignoring {}", name);
        continue;
      } else {
        PY_TRY;
        logger.info("Loading {}", name);
        py::module_::import(name.c_str());
        PY_CATCH;
      }
    }
  }
  // 启动子线程前执行，释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
  PyEval_ReleaseThread(PyThreadState_GET());
  PY_CATCH;
}

THook(int, "main", int argc, char* argv[], char* envp[]) {
  entry();
  return original(argc, argv, envp);
}

extern "C" _declspec(dllexport) void onPostInit() {
  std::ios::sync_with_stdio(false);
  LL::registerPlugin("BDSpyrunner", "Loading python in BDS",
                     LL::Version(PYR_VERSION_MAJOR, PYR_VERSION_MINOR,
                                 PYR_VERSION_MICRO, LL::Version::Release),
                     "https://github.com/twoone-3/BDSpyrunner.git", "GPL v3.0");
}
