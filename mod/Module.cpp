#include "Module.h"
#include "Version.h"
#include "../mc/tool.h"
#include "../mc/Actor.h"
#include "../mc/Block.h"
#include "../mc/ItemStack.h"
#include "../mc/ScoreBoard.h"
#include "../mc/Structure.h"
#include "../mc/Tag.h"

using namespace std;
//是否为史莱姆区块
constexpr int IsSlimeChunk(unsigned x, unsigned z) {
	unsigned mt0 = (x * 0x1F1F1F1F) ^ z;
	unsigned mt1 = (1812433253u * (mt0 ^ (mt0 >> 30u)) + 1);
	unsigned mt2 = mt1;
	for (unsigned i = 2; i < 398; ++i)
		mt2 = (1812433253u * (mt2 ^ (mt2 >> 30u)) + i);
	unsigned k = (mt0 & 0x80000000u) + (mt1 & 0x7FFFFFFFU);
	mt0 = mt2 ^ (k >> 1u);
	if (k & 1)
		mt0 ^= 2567483615u;
	mt0 ^= (mt0 >> 11u);
	mt0 ^= (mt0 << 7u) & 0x9D2C5680u;
	mt0 ^= (mt0 << 15u) & 0xEFC60000u;
	mt0 ^= (mt0 >> 18u);
	return !(mt0 % 10);
}
//最小版本要求
static PyObject* minVersionRequire(PyObject*, PyObject* args) {
	int major, minor, micro;
	Py_PARSE("iii", &major, &minor, &micro);
	if (major > PYR_MAJOR_VERSION)
		Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
	if (minor > PYR_MINOR_VERSION)
		Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
	if (micro > PYR_MICRO_VERSION)
		Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
	Py_RETURN_NONE;
}
//获取BDS版本
static PyObject* getBDSVersion(PyObject*, PyObject* args) {
	Py_PARSE("");
	return StringToPyUnicode(SymCall<std::string>("?getServerVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ"));
}
//指令输出
static PyObject* logout(PyObject*, PyObject* args) {
	const char* msg = "";
	Py_PARSE("s", &msg);
	SymCall<ostream&>("??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
		&cout, msg, strlen(msg));
	Py_RETURN_NONE;
}
//执行指令
static PyObject* runCommand(PyObject*, PyObject* args) {
	const char* cmd = "";
	Py_PARSE("s", &cmd);
	if (global<SPSCQueue> == nullptr)
		Py_RETURN_ERROR("Command queue is not initialized");
	SymCall<bool, SPSCQueue*, const string&>("??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		global<SPSCQueue>, cmd);
	Py_RETURN_NONE;
}
//设置监听
static PyObject* setListener(PyObject*, PyObject* args) {
	const char* name = ""; PyObject* func;
	Py_PARSE("sO", &name, &func);
	auto it = events.find(name);
	if (!PyFunction_Check(func))
		Py_RETURN_ERROR("Parameter 2 is not callable");
	if (it == events.end())
		Py_RETURN_ERROR("Invalid Listener key words");
	g_callback_functions[it->second].push_back(func);
	Py_RETURN_NONE;
}
//设置指令说明
static PyObject* setCommandDescription(PyObject*, PyObject* args) {
	const char* cmd = "";
	const char* des = "";
	PyObject* callback = nullptr;
	Py_PARSE("ss|O", &cmd, &des, &callback);
	g_commands[cmd] = { des, callback };
	Py_RETURN_NONE;
}
//获取玩家
static PyObject* getPlayerByXuid(PyObject*, PyObject* args) {
	const char* xuid = "";
	Py_PARSE("s", &xuid);
	Player* p = global<Level>->getPlayerByXuid(xuid);
	if (p == nullptr)
		Py_RETURN_ERROR("Failed to find player");
	return ToEntity(p);
}
//获取玩家列表
static PyObject* getPlayerList(PyObject*, PyObject* args) {
	Py_PARSE("");
	PyObject* list = PyList_New(0);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	global<Level>->forEachPlayer(
		[list](Player* p)->bool {
			PyList_Append(list, ToEntity(p));
			return true;
		}
	);
	return list;
}
//修改生物受伤的伤害值
static PyObject* setDamage(PyObject*, PyObject* args) {
	Py_PARSE("i", &g_damage);
	Py_RETURN_NONE;
}
static PyObject* setServerMotd(PyObject*, PyObject* args) {
	const char* name = "";
	Py_PARSE("s", &name);
	if (global<ServerNetworkHandler> == nullptr)
		Py_RETURN_ERROR("Server did not finish loading");
	SymCall<uintptr_t, ServerNetworkHandler*, const string&, bool>("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		global<ServerNetworkHandler>, name, true);
	Py_RETURN_NONE;
}
//根据坐标设置方块
static PyObject* getBlock(PyObject*, PyObject* args) {
	BlockPos bp; int did;
	Py_PARSE("iiii", &bp.x, &bp.y, &bp.z, &did);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = global<Level>->getBlockSource(did);
	if (bs == nullptr)
		Py_RETURN_ERROR("Unknown dimension ID");
	BlockLegacy* bl = bs->getBlock(&bp)->getBlockLegacy();
	return Py_BuildValue("{s:s:s:i}",
		"blockname", bl->getBlockName().c_str(),
		"blockid", bl->getBlockItemID()
	);
}
static PyObject* setBlock(PyObject*, PyObject* args) {
	const char* name = "";
	BlockPos bp; int did;
	Py_PARSE("siiii", &name, &bp.x, &bp.y, &bp.z, &did);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = global<Level>->getBlockSource(did);
	if (bs == nullptr)
		Py_RETURN_ERROR("Unknown dimension ID");
	Block* b = *reinterpret_cast<Block**>(SYM((string("?m") + name + "@VanillaBlocks@@3PEBVBlock@@EB").c_str()));
	if (b == nullptr)
		Py_RETURN_ERROR("Unknown Block");
	bs->setBlock(&bp, b);
	Py_RETURN_NONE;
}
//获取一个结构
static PyObject* getStructure(PyObject*, PyObject* args) {
	BlockPos pos1, pos2; int did;
	Py_PARSE("iiiiiii",
		&pos1.x, &pos1.y, &pos1.z,
		&pos2.x, &pos2.y, &pos2.z, &did
	);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = global<Level>->getBlockSource(did);
	if (bs == nullptr)
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

	return StringToPyUnicode(CompoundTagtoJson(st.save()).dump(4));
}
static PyObject* setStructure(PyObject*, PyObject* args) {
	const char* data = "";
	BlockPos pos; int did;
	Py_PARSE("siiii", &data, &pos.x, &pos.y, &pos.z, &did);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = global<Level>->getBlockSource(did);
	if (bs == nullptr)
		Py_RETURN_ERROR("Unknown dimension ID");
	Json value = StringToJson(data);
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
	st.placeInWorld(bs, global<Level>->getBlockPalette(), &pos, &ss);
	for (int x = 0; x != size.x; ++x) {
		for (int y = 0; y != size.y; ++y) {
			for (int z = 0; z != size.z; ++z) {
				BlockPos bp{ x,y,z };
				bs->neighborChanged(&bp);
			}
		}
	}
	Py_RETURN_NONE;
}
//产生爆炸
static PyObject* explode(PyObject*, PyObject* args) {
	Vec3 pos; int did;
	float power; bool destroy;
	float range; bool fire;
	Py_PARSE("fffifbfb",
		&pos.x, &pos.y, &pos.z, &did, &power, &destroy, &range, &fire
	);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = global<Level>->getBlockSource(did);
	if (!bs)
		Py_RETURN_ERROR("Unknown dimension ID");
	SymCall<bool>("?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
		global<Level>, bs, nullptr, pos, power, fire, destroy, range, true);
	Py_RETURN_NONE;
}
//生成物品
static PyObject* spawnItem(PyObject*, PyObject* args) {
	const char* data = "";
	Vec3 pos; int did;
	Py_PARSE("sfffi", &data, &pos.x, &pos.y, &pos.z, &did);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = global<Level>->getBlockSource(did);
	if (!bs)
		Py_RETURN_ERROR("Unknown dimension ID");
	ItemStack item(StringToJson(data));
	global<Level>->getSpawner()->spawnItem(bs, &item, &pos);
	Py_RETURN_NONE;
}
//是否为史莱姆区块
static PyObject* isSlimeChunk(PyObject*, PyObject* args) {
	unsigned x, z;
	Py_PARSE("II", &x, &z);
	if (IsSlimeChunk(x, z))
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
//设置牌子文字
static PyObject* setSignBlockMessage(PyObject*, PyObject* args) {
	const char* name = "";
	BlockPos bp; int did;
	Py_PARSE("siiii", &name, &bp.x, &bp.y, &bp.z, &did);
	if (global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = global<Level>->getBlockSource(did);
	if (bs == nullptr)
		Py_RETURN_ERROR("Unknown dimension ID");
	BlockActor* sign = bs->getBlockEntity(&bp);
	SymCall<void, BlockActor*, const string&, const string&>("?setMessage@SignBlockActor@@QEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0@Z",
		sign, name, name);
	sign->setChanged();
	Py_RETURN_NONE;
}
//模块方法列表
static PyMethodDef Methods[]{
	{"minVersionRequire", minVersionRequire, METH_VARARGS, nullptr},
	{"getBDSVersion", getBDSVersion, METH_NOARGS, nullptr},
	{"logout", logout, METH_VARARGS, nullptr},
	{"runcmd", runCommand, METH_VARARGS, nullptr},
	{"setListener", setListener, METH_VARARGS, nullptr},
	{"setCommandDescription", setCommandDescription, METH_VARARGS, nullptr},
	{"getPlayerByXuid", getPlayerByXuid, METH_VARARGS, nullptr},
	{"getPlayerList", getPlayerList, METH_NOARGS, nullptr},
	{"setDamage", setDamage, METH_VARARGS, nullptr},
	{"setServerMotd", setServerMotd, METH_VARARGS, nullptr},
	{"getBlock", getBlock, METH_VARARGS, nullptr},
	{"setBlock", setBlock, METH_VARARGS, nullptr},
	{"getStructure", getStructure, METH_VARARGS, nullptr},
	{"setStructure", setStructure, METH_VARARGS, nullptr},
	{"explode", explode, METH_VARARGS, nullptr},
	{"spawnItem", spawnItem, METH_VARARGS, nullptr},
	{"isSlimeChunk", isSlimeChunk, METH_VARARGS, nullptr},
	{"setSignBlockMessage", setSignBlockMessage, METH_VARARGS, nullptr},
	{nullptr}
};
//模块定义
static PyModuleDef Module{
	PyModuleDef_HEAD_INIT,
	"mc",
	"API functions",
	-1,
	Methods,
	nullptr,
	nullptr,
	nullptr,
	nullptr
};
//模块初始化
extern "C" PyObject * mc_init() {
	PyObject* module = PyModule_Create(&Module);
	PyModule_AddObject(module, "Entity", reinterpret_cast<PyObject*>(&PyEntity_Type));
	return module;
}
