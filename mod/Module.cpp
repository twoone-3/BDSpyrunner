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
	int v1, v2, v3;
	if (PyArg_ParseTuple(args, "iii:" __FUNCTION__, &v1, &v2, &v3)) {
		if (v1 > PYR_MAJOR_VERSION)
			Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
		if (v2 > PYR_MINOR_VERSION)
			Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
		if (v3 > PYR_MICRO_VERSION)
			Py_RETURN_ERROR("The plugin version does not meet the minimum requirements");
	}
	Py_RETURN_NONE;
}
//获取BDS版本
static PyObject* getBDSVersion(PyObject*, PyObject*) {
	return StringToPyUnicode(GetBDSVersion());
}
//指令输出
static PyObject* logout(PyObject*, PyObject* args) {
	const char* msg = "";
	if (PyArg_ParseTuple(args, "s:" __FUNCTION__, &msg)) {
		SymCall<ostream&>("??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
			&cout, msg, strlen(msg));
	}
	Py_RETURN_NONE;
}
//执行指令
static PyObject* runCommand(PyObject*, PyObject* args) {
	const char* cmd = "";
	if (PyArg_ParseTuple(args, "s:" __FUNCTION__, &cmd)) {
		if (!Global<SPSCQueue>::data)
			Py_RETURN_ERROR("Command queue is not initialized");
		SymCall<bool, SPSCQueue*, const string&>("??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			Global<SPSCQueue>::data, cmd);
	}
	Py_RETURN_NONE;
}
//设置监听
static PyObject* setListener(PyObject*, PyObject* args) {
	const char* name = ""; PyObject* func;
	if (PyArg_ParseTuple(args, "sO:" __FUNCTION__, &name, &func)) {
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
static PyObject* setCommandDescription(PyObject*, PyObject* args) {
	const char* cmd = "";
	const char* des = "";
	PyObject* callback = nullptr;
	if (PyArg_ParseTuple(args, "ss|O:" __FUNCTION__, &cmd, &des, &callback)) {
		g_commands[cmd] = { des, callback };
	}
	Py_RETURN_NONE;
}
//获取玩家
static PyObject* getPlayerByXuid(PyObject*, PyObject* args) {
	const char* xuid = "";
	if (PyArg_ParseTuple(args, "s:" __FUNCTION__, &xuid)) {
		Player* p = Global<Level>::data->getPlayerByXuid(xuid);
		if (!p)
			Py_RETURN_ERROR("Failed to find player");
		return ToEntity(p);
	}
	Py_RETURN_NONE;
}
static PyObject* getPlayerList(PyObject*, PyObject*) {
	PyObject* list = PyList_New(0);
	if (!Global<Level>::data)
		Py_RETURN_ERROR("Level is not set");
	Global<Level>::data->forEachPlayer(
		[list](Player* p)->bool {
			PyList_Append(list, ToEntity(p));
			return true;
		}
	);
	return list;
}
//修改生物受伤的伤害值
static PyObject* setDamage(PyObject*, PyObject* args) {
	PyArg_ParseTuple(args, "i:" __FUNCTION__, &g_damage);
	Py_RETURN_NONE;
}
static PyObject* setServerMotd(PyObject*, PyObject* args) {
	const char* name = "";
	if (PyArg_ParseTuple(args, "s:" __FUNCTION__, &name)) {
		if (!Global<ServerNetworkHandler>::data)
			Py_RETURN_ERROR("Server did not finish loading");
		SymCall<uintptr_t, ServerNetworkHandler*, const string&, bool>("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
			Global<ServerNetworkHandler>::data, name, true);
	}
	Py_RETURN_NONE;
}
//根据坐标设置方块
static PyObject* getBlock(PyObject*, PyObject* args) {
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "iiii:" __FUNCTION__, &bp.x, &bp.y, &bp.z, &did)) {
		if (!Global<Level>::data)
			Py_RETURN_ERROR("Level is not set");
		BlockSource* bs = Global<Level>::data->getBlockSource(did);
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
static PyObject* setBlock(PyObject*, PyObject* args) {
	const char* name = "";
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "siiii:" __FUNCTION__, &name, &bp.x, &bp.y, &bp.z, &did)) {
		if (!Global<Level>::data)
			Py_RETURN_ERROR("Level is not set");
		BlockSource* bs = Global<Level>::data->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		Block* b = *reinterpret_cast<Block**>(SYM((string("?m") + name + "@VanillaBlocks@@3PEBVBlock@@EB").c_str()));
		if (!b)
			Py_RETURN_ERROR("Unknown Block");
		bs->setBlock(&bp, b);
	}
	Py_RETURN_NONE;
}
//获取一个结构
static PyObject* getStructure(PyObject*, PyObject* args) {
	BlockPos pos1, pos2; int did;
	if (PyArg_ParseTuple(args, "iiiiiii:" __FUNCTION__,
		&pos1.x, &pos1.y, &pos1.z,
		&pos2.x, &pos2.y, &pos2.z, &did)) {
		if (!Global<Level>::data)
			Py_RETURN_ERROR("Level is not set");
		BlockSource* bs = Global<Level>::data->getBlockSource(did);
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

		return StringToPyUnicode(CompoundTagtoJson(st.save()).dump(4));
	}
	Py_RETURN_NONE;
}
static PyObject* setStructure(PyObject*, PyObject* args) {
	const char* data = "";
	BlockPos pos; int did;
	if (PyArg_ParseTuple(args, "siiii:" __FUNCTION__,
		&data, &pos.x, &pos.y, &pos.z, &did)) {
		if (!Global<Level>::data)
			Py_RETURN_ERROR("Level is not set");
		BlockSource* bs = Global<Level>::data->getBlockSource(did);
		if (!bs)
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
		st.placeInWorld(bs, Global<Level>::data->getBlockPalette(), &pos, &ss);
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
static PyObject* explode(PyObject*, PyObject* args) {
	Vec3 pos; int did;
	float power; bool destroy;
	float range; bool fire;
	if (PyArg_ParseTuple(args, "fffifbfb:" __FUNCTION__,
		&pos.x, &pos.y, &pos.z, &did, &power, &destroy, &range, &fire)) {
		if (!Global<Level>::data)
			Py_RETURN_ERROR("Level is not set");
		BlockSource* bs = Global<Level>::data->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		SymCall<bool>("?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
			Global<Level>::data, bs, nullptr, pos, power, fire, destroy, range, true);
	}
	Py_RETURN_NONE;
}
//生成物品
static PyObject* spawnItem(PyObject*, PyObject* args) {
	const char* data = "";
	Vec3 pos; int did;
	if (PyArg_ParseTuple(args, "sfffi:" __FUNCTION__, &data, &pos.x, &pos.y, &pos.z, &did)) {
		if (!Global<Level>::data)
			Py_RETURN_ERROR("Level is not set");
		BlockSource* bs = Global<Level>::data->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		ItemStack item(StringToJson(data));
		Global<Level>::data->getSpawner()->spawnItem(bs, &item, &pos);
		cout << pos.toString() << endl;
	}
	Py_RETURN_NONE;
}
//是否为史莱姆区块
static PyObject* isSlimeChunk(PyObject*, PyObject* args) {
	unsigned x, z;
	if (PyArg_ParseTuple(args, "II:" __FUNCTION__, &x, &z)) {
		if (IsSlimeChunk(x, z))
			Py_RETURN_TRUE;
		else
			Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}
//设置牌子文字
static PyObject* setSignBlockMessage(PyObject*, PyObject* args) {
	const char* name = "";
	BlockPos bp; int did;
	if (PyArg_ParseTuple(args, "siiii:" __FUNCTION__, &name, &bp.x, &bp.y, &bp.z, &did)) {
		if (!Global<Level>::data)
			Py_RETURN_ERROR("Level is not set");
		BlockSource* bs = Global<Level>::data->getBlockSource(did);
		if (!bs)
			Py_RETURN_ERROR("Unknown dimension ID");
		BlockActor* sign = bs->getBlockEntity(&bp);
		SymCall<void, BlockActor*, const string&, const string&>("?setMessage@SignBlockActor@@QEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0@Z",
			sign, name, name);
		sign->setChanged();
	}
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
PyObject* mc_init() {
	PyObject* module = PyModule_Create(&Module);
	PyModule_AddObject(module, "Entity", reinterpret_cast<PyObject*>(&PyEntity_Type));
	return module;
}
