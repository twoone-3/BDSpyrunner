#include "Module.h"
#include "magic_enum.hpp"

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
//获取BDS版本
Py_METHOD_DEFINE(getBDSVersion) {
	return ToPyObject(Common::getGameVersionString());
}
//执行指令
Py_METHOD_DEFINE(runCommand) {
	const char* cmd = "";
	Py_PARSE("s", &cmd);
	auto result = Level::runcmd(cmd);
	return ToPyObject(result);
}
//执行指令
Py_METHOD_DEFINE(runCommandEx) {
	const char* cmd = "";
	Py_PARSE("s", &cmd);
	auto info = Level::runcmdEx(cmd);
	//TODO: check whether it has memory leak
	return PyTuple_Pack(2, ToPyObject(info.first), ToPyObject(info.second));
}
//设置监听
Py_METHOD_DEFINE(setListener) {
	const char* event_name = "";
	PyObject* func = nullptr;
	Py_PARSE("sO", &event_name, &func);
	if (!PyFunction_Check(func))
		Py_RETURN_ERROR("a2 is not callable");
	auto event_code = magic_enum::enum_cast<EventCode>(event_name);
	if (!event_code)
		Py_RETURN_ERROR_FORMAT("Invalid event name %s", event_name);
	//如果监听器未启用，则启用
	if (g_callback_functions.find(event_code.value()) == g_callback_functions.end())
		EnableEventListener(event_code.value());
	//添加回调函数
	g_callback_functions[event_code.value()].push_back(func);
	Py_RETURN_NONE;
}
//设置指令说明
Py_METHOD_DEFINE(registerCommand) {
	const char* cmd = "";
	PyObject* callback = nullptr;
	const char* des = "";
	Py_PARSE("sOs", &cmd, &callback, &des);
	g_commands[cmd] = {des, callback};
	Py_RETURN_NONE;
}
//获取玩家
Py_METHOD_DEFINE(getPlayerByXuid) {
	const char* xuid = "";
	Py_PARSE("s", &xuid);
	Player* p = Level::getPlayer(xuid);
	if (p == nullptr)
		Py_RETURN_ERROR("Failed to find player");
	return ToPyObject(p);
}
//获取玩家列表
Py_METHOD_DEFINE(getPlayerList) {
	PyObject* list = PyList_New(0);
	for (auto p : Level::getAllPlayers()) {
		PyObject* player = ToPyObject(p);
		PyList_Append(list, player);
		Py_DECREF(player);
	}
	return list;
}
//获取实体列表
Py_METHOD_DEFINE(getEntityList) {
	PyObject* list = PyList_New(0);
	for (auto a : Level::getAllEntities()) {
		PyObject* entity = ToPyObject(a);
		PyList_Append(list, entity);
		Py_DECREF(entity);
	}
	return list;
}
//设置服务器motd
Py_METHOD_DEFINE(setServerMotd) {
	const char* name = "";
	Py_PARSE("s", &name);
	if (Global<ServerNetworkHandler> == nullptr)
		Py_RETURN_ERROR("Server dim not finish loading");
	SymCall("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		uintptr_t, ServerNetworkHandler*, const string&, bool)(Global<ServerNetworkHandler>, name, true);
	Py_RETURN_NONE;
}
//广播文本
Py_METHOD_DEFINE(broadcastText) {
	const char* text = "";
	const char* type = "RAW";
	Py_PARSE("s|i", &text, &type);
	auto t = magic_enum::enum_cast<TextType>(type);
	if (!t)
		Py_RETURN_ERROR_FORMAT("Invalid text type %s", type);
	Level::broadcastText(text, t.value());
	Py_RETURN_NONE;
}
//广播标题
Py_METHOD_DEFINE(broadcastTitle) {
	const char* text = "";
	const char* type = "";
	int fade_in_duration;
	int remain_duration;
	int fade_out_duration;
	Py_PARSE("ssiii", &text, &type, &fade_in_duration, &remain_duration, &fade_out_duration);
	auto t = magic_enum::enum_cast<TitleType>(type);
	if (!t)
		Py_RETURN_ERROR_FORMAT("Invalid title type %s", type);
	Level::broadcastTitle(text, t.value(), fade_in_duration, remain_duration, fade_out_duration);
	Py_RETURN_NONE;
}
//获取方块
Py_METHOD_DEFINE(getBlock) {
	BlockPos bp;
	int dim;
	Py_PARSE("iiii", &bp.x, &bp.y, &bp.z, &dim);
	BlockSource* bs = Level::getBlockSource(dim);
	if (bs == nullptr)
		Py_RETURN_ERROR("Unknown dimension ID");
	auto bi = Level::getBlockInstance(bp, bs);
	return ToPyObject(bi);
}
//设置方块
Py_METHOD_DEFINE(setBlock) {
	BlockPos pos;
	int dim;
	const char* name = "";
	//方块附加值
	int tile_data;
	Py_PARSE("iiiisi", &pos.x, &pos.y, &pos.z, &dim, &name, &tile_data);
	Level::setBlock(pos, dim, name, tile_data);
	Py_RETURN_NONE;
}
//从指定地点获取NBT结构数据
Py_METHOD_DEFINE(getStructure) {
	BlockPos pos1, pos2;
	int dim;
	bool ignore_entities = true;
	bool ignore_blocks = false;
	Py_PARSE("iiiiiii|bb",
		&pos1.x, &pos1.y, &pos1.z, &pos2.x, &pos2.y, &pos2.z,
		&dim, &ignore_entities, &ignore_blocks);
	auto st = StructureTemplate::fromWorld("name", dim, pos1, pos2, ignore_entities, ignore_blocks);
	return ToPyObject(st.save());
}
//从NBT结构数据导出结构到指定地点
Py_METHOD_DEFINE(setStructure) {
	PyObject* nbt = nullptr;
	BlockPos pos;
	int dim;
	//enum Mirror : unsigned char {
	//	None_15 = 0,
	//	X,
	//	Z,
	//	XZ,
	//};
	const char* mirror_str = "";
	//enum Rotation : unsigned char {
	//	None_14 = 0,
	//	Rotate90,
	//	Rotate180,
	//	Rotate270,
	//	Total,
	//};
	const char* rotation_str = "";
	Py_PARSE("Oiiiiss",
		&nbt, &pos.x, &pos.y, &pos.z, &dim, &mirror_str, &rotation_str);
	auto mir = magic_enum::enum_cast<Mirror>(mirror_str);
	if (!mir)
		Py_RETURN_ERROR_FORMAT("Invalid mirror type %s", mirror_str);
	auto rot = magic_enum::enum_cast<Rotation>(rotation_str);
	if (!rot)
		Py_RETURN_ERROR_FORMAT("Invalid rotation type %s", rotation_str);
	StructureTemplate::fromTag("name", *PyNBT_TO_COMPOUNDTAG(nbt))
		.toWorld(dim, pos, mir.value(), rot.value());
	/*for (int x = 0; x != size.x; ++x) {
		for (int y = 0; y != size.y; ++y) {
			for (int z = 0; z != size.z; ++z) {
				BlockPos pos{ x, y, z };
				bs->neighborChanged(pos, pos);
			}
		}
	}*/
	Py_RETURN_NONE;
}
//产生爆炸
Py_METHOD_DEFINE(explode) {
	Vec3 pos;
	int dim;
	float power;
	bool destroy;
	float range;
	bool fire;
	Py_PARSE("fffifbfb",
		&pos.x, &pos.y, &pos.z,
		&dim, &power, &destroy, &range, &fire);
	BlockSource* bs = Level::getBlockSource(dim);
	if (!bs)
		Py_RETURN_ERROR("Unknown dimension ID");
	Global<Level>->explode(*bs, nullptr, pos, power, fire, destroy, range, true);
	Py_RETURN_NONE;
}
//生成物品
Py_METHOD_DEFINE(spawnItem) {
	PyObject* item_nbt = nullptr;
	Vec3 pos;
	int dim;
	Py_PARSE("Offfi", &item_nbt, &pos.x, &pos.y, &pos.z, &dim);
	ItemStack item = ItemStack::fromTag(*PyNBT_TO_COMPOUNDTAG(item_nbt));
	Global<Level>->getSpawner().spawnItem(pos, dim, &item);
	Py_RETURN_NONE;
}
//是否为史莱姆区块
Py_METHOD_DEFINE(isSlimeChunk) {
	unsigned x, z;
	Py_PARSE("II", &x, &z);
	return ToPyObject(IsSlimeChunk(x, z));
}
//设置牌子文字
Py_METHOD_DEFINE(setSignBlockMessage) {
	const char* name = "";
	BlockPos bp;
	int dim;
	Py_PARSE("siiii", &name, &bp.x, &bp.y, &bp.z, &dim);
	if (Global<Level> == nullptr)
		Py_RETURN_ERROR("Level is not set");
	BlockSource* bs = Level::getBlockSource(dim);
	if (bs == nullptr)
		Py_RETURN_ERROR("Unknown dimension ID");
	auto sign = static_cast<SignBlockActor*>(Global<Level>->getBlockEntity(bp, dim));
	sign->setMessage(name, name);
	sign->setChanged();
	Py_RETURN_NONE;
}
//模块方法列表
static PyMethodDef methods[] {
	Py_METHOD_NOARGS(getBDSVersion),
	Py_METHOD_VARARGS(runCommand),
	Py_METHOD_VARARGS(runCommandEx),
	Py_METHOD_VARARGS(setListener),
	Py_METHOD_VARARGS(registerCommand),
	Py_METHOD_VARARGS(getPlayerByXuid),
	Py_METHOD_NOARGS(getPlayerList),
	Py_METHOD_NOARGS(getEntityList),
	Py_METHOD_VARARGS(setServerMotd),
	Py_METHOD_VARARGS(getBlock),
	Py_METHOD_VARARGS(setBlock),
	Py_METHOD_VARARGS(getStructure),
	Py_METHOD_VARARGS(setStructure),
	Py_METHOD_VARARGS(explode),
	Py_METHOD_VARARGS(spawnItem),
	Py_METHOD_VARARGS(isSlimeChunk),
	Py_METHOD_VARARGS(setSignBlockMessage),
	Py_METHOD_END};
//模块定义
static PyModuleDef Module {
	PyModuleDef_HEAD_INIT,
	"mc",
	"API functions",
	-1,
	methods,
	nullptr,
	nullptr,
	nullptr,
	nullptr};
//模块初始化
extern "C" PyObject * McInit() {
	PyObject* m = PyModule_Create(&Module);
	PyModule_AddObject(m, "Entity", reinterpret_cast<PyObject*>(&PyEntity_Type));
	PyModule_AddObject(m, "BlockInstance", reinterpret_cast<PyObject*>(&PyBlockInstance_Type));
	PyModule_AddObject(m, "ItemStack", reinterpret_cast<PyObject*>(&PyItemStack_Type));
	PyModule_AddObject(m, "NBT", reinterpret_cast<PyObject*>(&PyNBT_Type));
	return m;
}
