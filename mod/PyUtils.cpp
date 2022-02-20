#include "PyUtils.h"
#include "Common.h"
#include "Module.h"

using namespace std;
#pragma region Mess
//转Player
Player* P(Actor* a) {
	if (!IsPlayer(a))
		throw std::runtime_error("The ptr is not Player*");
	return static_cast<Player*>(a);
}
//Vec3转list
py::list ToList(const Vec3& pos) {
	py::list l;
	l.append(pos.x);
	l.append(pos.y);
	l.append(pos.z);
	return l;
}
//方块坐标转list
py::list ToList(const BlockPos& pos) {
	py::list l;
	l.append(pos.x);
	l.append(pos.y);
	l.append(pos.z);
	return l;
}
#pragma endregion
#pragma region NBT
struct PyNBT {
	unique_ptr<Tag> thiz;

	PyNBT(unique_ptr<CompoundTag>&& other) {
		thiz = move(other);
	}
	PyNBT(unique_ptr<Tag>&& other) {
		thiz = move(other);
	}
	PyNBT(PyNBT&& other) {
		thiz = move(other.thiz);
	}
	//有三种模式
	// 1. a1数据类型 a2数据
	//    NBT('Int',3) NBT('Compound')
	// 2. SNBT模式 a2填SNBT字符串
	//    NBT('SNBT', snbt)
	// 3. 二进制模式 a2填bytes
	//    NBT('Binary', bytes)
	PyNBT(const string& type_str, const py::object& value) {
		if (type_str == "SNBT"sv) {
			thiz = CompoundTag::fromSNBT(py::str(value));
			return;
		}
		else if (type_str == "Binary"sv) {
			thiz = CompoundTag::fromBinaryNBT(py::bytes(value));
			return;
		}
		auto type = magic_enum::enum_cast<Tag::Type>(type_str);
		if (!type)
			throw py::type_error("Invalied NBT type " + type_str);
		switch (type.value()) {
		case Tag::End:
			thiz = EndTag::create();
			break;
		case Tag::Byte:
			thiz = ByteTag::create();
			thiz->asByteTag()->value() = py::int_(value);
			break;
		case Tag::Short:
			thiz = ShortTag::create();
			thiz->asShortTag()->value() = py::int_(value);
			break;
		case Tag::Int:
			thiz = IntTag::create();
			thiz->asIntTag()->value() = py::int_(value);
			break;
		case Tag::Int64:
			thiz = Int64Tag::create();
			thiz->asInt64Tag()->value() = py::int_(value);
			break;
		case Tag::Float:
			thiz = FloatTag::create();
			thiz->asFloatTag()->value() = py::float_(value);
			break;
		case Tag::Double:
			thiz = DoubleTag::create();
			thiz->asDoubleTag()->value() = py::float_(value);
			break;
		case Tag::ByteArray:
			thiz = ByteArrayTag::create();
			//TODO: enable to construct ByteArray
			break;
		case Tag::String:
			thiz = StringTag::create();
			thiz->asStringTag()->value() = py::str(value);
			break;
		case Tag::List:
			thiz = ListTag::create();
			break;
		case Tag::Compound:
			thiz = CompoundTag::create();
			break;
		case Tag::IntArray:
			thiz = IntArrayTag::create();
			break;
		}
	}

	PyNBT __getitem__(const string& key) {
		return thiz->asCompoundTag()->operator[](key)->copy();
	}
	void __setitem__(const string& key, const PyNBT& val) {
		thiz->asCompoundTag()->put(key, val.thiz->copy());
	}
	auto asByte() { return thiz->asByteTag()->value(); }
	auto asShort() { return thiz->asShortTag()->value(); }
	auto asInt() { return thiz->asIntTag()->value(); }
	auto asInt64() { return thiz->asInt64Tag()->value(); }
	auto asFloat() { return thiz->asFloatTag()->value(); }
	auto asDouble() { return thiz->asDoubleTag()->value(); }
	string_view getType() {
		return magic_enum::enum_name(thiz->getTagType());
	}
	py::bytes toBinary() {
		return thiz->asCompoundTag()->toBinaryNBT();
	}
	string toJson(int indentatiton = 4) {
		return thiz->toJson(indentatiton);
	}
	string toSNBT() {
		return thiz->asCompoundTag()->toSNBT();
	}
	void append(const PyNBT& value) {
		if (thiz->getTagType() != Tag::List)
			throw py::type_error("Type of tag must be list");
		//TODO: find out why it don't work
		thiz->asListTag()->add(value.thiz->copy());
	}
};
#pragma endregion
#pragma region Item
struct PyItem {
	ItemStack thiz;

	PyItem(const PyNBT& nbt) :thiz(ItemStack::fromTag(*nbt.thiz->asCompoundTag())) {}
	string getName() { return thiz.getName(); }
	PyNBT getNBT() { return PyNBT(thiz.getNbt()); }
};
#pragma endregion
#pragma region Entity
struct PyEntity {
	Actor* thiz;

	PyEntity(Actor* a) :thiz(a) {}
	//获取名字
	string getName() { return thiz->getNameTag(); }
	//设置名字
	void setName(const string& n) { thiz->setNameTag(n); }
	//设置名字可见性
	void setNameVisible(bool visible) { thiz->setNameTagVisible(visible); }
	//获取UUID
	string getUuid() { return P(thiz)->getUuid(); }
	//获取XUID
	string getXuid() { return P(thiz)->getXuid(); }
	//获取坐标
	py::list getPos() { return ToList(thiz->getPos()); }
	//获取维度ID
	int getDimensionId() { return thiz->getDimensionId(); }
	//是否着地
	bool isStanding() { return thiz->isStanding(); }
	//是否潜行
	bool isSneaking() { return thiz->isSneaking(); }
	//是否坐下
	bool isSitting() { return thiz->isSitting(); }
	//获取类型
	int getTypeId() { return thiz->getEntityTypeId(); }
	//获取类型字符串
	string getTypeName() { return thiz->getTypeName(); }
	//获取nbt数据
	PyNBT getNBT() { return PyNBT(CompoundTag::fromActor(thiz)); }
	//设置nbt数据
	bool setNBT(PyNBT& nbt) { return thiz->setNbt(nbt.thiz->asCompoundTag()); }
	//获取生命值
	int getHealth() { return thiz->getHealth(); }
	//获取最大生命值
	int getMaxHealth() { return thiz->getMaxHealth(); }
	//获取权限
	int getPermissions() { return static_cast<int>(P(thiz)->getPlayerPermissionLevel()); }
	//设置权限
	void setPermissions(int perm) { P(thiz)->setPermissions(static_cast<CommandPermissionLevel>(perm)); }
	//获取设备ID
	string getPlatformOnlineId() { return P(thiz)->getPlatformOnlineId(); }
	//获取设备类型
	int getPlatform() { return P(thiz)->getPlatform(); }
	//获取IP
	string getIP() { return Global<RakNet::RakPeer>->getAdr(*P(thiz)->getNetworkIdentifier()).ToString(false, ':'); }
	//设置玩家手上物品
	void setHand(const PyItem& item) {
		const_cast<ItemStack&>(P(thiz)->getSelectedItem()) = item.thiz;
		P(thiz)->sendInventory(true);
	}
	//增加玩家背包物品
	void addItem(const PyNBT& item_nbt) {
		auto item = ItemStack::fromTag(*item_nbt.thiz->asCompoundTag());
		P(thiz)->giveItem(&item);
		P(thiz)->sendInventory(true);
	}
	//移除玩家背包物品
	void removeItem(int slot, int num) {
		P(thiz)->getInventory().removeItem(slot, num);
		P(thiz)->sendInventory(true);
	}
	//传送
	void teleport(const Vec3& pos, int dim) {
		thiz->teleport(pos, dim);
	}
	//发送文本
	void sendText(const string& text, const string& type = "RAW") {
		auto t = magic_enum::enum_cast<TextType>(type);
		if (!t)
			throw py::value_error("Invalid text type " + type);
		P(thiz)->sendTextPacket(text, t.value());
	}
	//模拟玩家执行命令
	void runCommand(const string& cmd) { P(thiz)->sendCommandRequestPacket(cmd); }
	//重新发送所有区块
	void resendAllChunks() { P(thiz)->resendAllChunks(); }
	//断开连接
	void disconnect(const string& msg = "") { P(thiz)->kick(msg); }
	//获取玩家分数
	int getScore(const string& objname) { return P(thiz)->getScore(objname); }
	//设置玩家分数
	void setScore(const string& objname, int count) { P(thiz)->setScore(objname, count); }
	//增加玩家分数
	void addScore(const string& objname, int count) { P(thiz)->addScore(objname, count); }
	//减少玩家分数
	void reduceScore(const string& objname, int count) { P(thiz)->reduceScore(objname, count); }
	//增加等级
	void addLevel(int level) { P(thiz)->addLevels(level); }
	//跨服传送
	void transferServer(const string& address, unsigned short port) {
		P(thiz)->sendTransferPacket(address, port);
	}
	//发送表单
	void sendCustomForm(const string& str, const py::function& cb) {
		P(thiz)->sendCustomFormPacket(str,
			[this, cb] (string arg) {
			cb(new PyEntity(thiz), arg);
		}
		);
	}
	void sendSimpleForm(const string& title, const string& content,
		const vector<string>& buttons, const vector<string>& images, const py::function& cb) {
		if (buttons.size() != images.size())
			throw py::value_error("The number of buttons is not equal to the number of images");
		P(thiz)->sendSimpleFormPacket(title, content, buttons, images,
			[this, cb] (int arg) {
			cb(new PyEntity(thiz), arg);
		}
		);
	}
	void sendModalForm(const string& title, const string& content,
		const string& button1, const string& button2, const py::function& cb) {
		P(thiz)->sendModalFormPacket(title, content, button1, button2,
			[this, cb] (bool arg) {
			cb(new PyEntity(thiz), arg);
		}
		);
	}
	//设置侧边栏
	void setSidebar(const string& title, const string& side_data, const string& order_str) {
		auto order = magic_enum::enum_cast<ObjectiveSortOrder>(order_str);
		if (!order)
			throw py::value_error("Invalid order " + order_str);
		ObjectiveSortOrder::Ascending;
		ObjectiveSortOrder::Descending;
		vector<pair<string, int>> data;
		fifo_json value = StrToJson(side_data);
		for (auto& [key, val] : value.items()) {
			data.push_back({key, val});
		}
		P(thiz)->setSidebar(title, data, order.value());
	}
	void removeSidebar() { P(thiz)->removeSidebar(); }
	//Boss栏
	void setBossbar(const string& name, float per) { P(thiz)->sendBossEventPacket(BossEvent::Show, name, per, BossEventColour::Red); }
	void removeBossbar(const string& name) { P(thiz)->sendBossEventPacket(BossEvent::Hide, name, 0, BossEventColour::Red); }
	//标签
	void addTag(const string& tag) { thiz->addTag(tag); }
	void removeTag(const string& tag) { thiz->removeTag(tag); }
	py::list getTags() {
		auto tags = thiz->getTags();
		py::list l;
		for (size_t i = 0; i < tags.size(); i++) {
			l.append(tags[i]);
		}
		return l;
	}
	//杀死实体
	void kill() { thiz->kill(); }
	//获取游戏模式
	int getGameMode() { return P(thiz)->getPlayerGameType(); }
};
#pragma endregion
#pragma region Block
struct PyBlock {
	BlockInstance thiz;

	PyBlock(const BlockInstance& bi) :thiz(bi) {}
	string getName() {
		return thiz.getBlock()->getName().getString();
	}
	PyNBT getNBT() {
		return PyNBT(CompoundTag::fromBlock(thiz.getBlock()));
	}
	py::list getPos() {
		return ToList(thiz.getPosition());
	}
	int getDimensionId() {
		return thiz.getDimensionId();
	}
};
#pragma endregion
#pragma region Logger
struct PyLogger {
	Logger thiz;

	PyLogger(const string& title) :thiz(title) {}
	void info(const string& msg) { thiz.info(msg); }
	void debug(const string& msg) { thiz.debug(msg); }
	void fatal(const string& msg) { thiz.fatal(msg); }
	void error(const string& msg) { thiz.error(msg); }
};
#pragma endregion
#pragma region functions
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
//设置监听
void setListener(const string& event_name, const py::function& cb) {
	auto event_code = magic_enum::enum_cast<EventCode>(event_name);
	if (!event_code)
		throw py::value_error("Invalid event name " + event_name);
	//如果监听器未启用，则启用
	if (g_cb_functions.find(event_code.value()) == g_cb_functions.end())
		EnableEventListener(event_code.value());
	//添加回调函数
	g_cb_functions[event_code.value()].push_back(cb);
}
//注册命令
void registerCommand(const string& cmd, const py::function& cb, const string& des) {
	g_commands[cmd] = {des, cb};
}
//获取玩家
PyEntity* getPlayerByXuid(const string& xuid) {
	Player* p = Level::getPlayer(xuid);
	if (p == nullptr)
		throw runtime_error("Failed to find player");
	return new PyEntity(p);
}
//获取玩家列表
py::list getPlayerList() {
	py::list l;
	for (auto p : Level::getAllPlayers()) {
		l.append(new PyEntity(p));
	}
	return l;
}
//获取实体列表
py::list getEntityList() {
	py::list l;
	for (auto p : Level::getAllEntities()) {
		l.append(new PyEntity(p));
	}
	return l;
}
//设置服务器motd
void setServerMotd(const string& name) {
	if (Global<ServerNetworkHandler> == nullptr)
		throw runtime_error("Server dim not finish loading");
	SymCall("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		uintptr_t, ServerNetworkHandler*, const string&, bool)(Global<ServerNetworkHandler>, name, true);
}
//广播文本
void broadcastText(const string& text, const string& type = "RAW") {
	auto t = magic_enum::enum_cast<TextType>(type);
	if (!t)
		throw py::value_error("Invalid text type " + type);
	Level::broadcastText(text, t.value());
}
//广播标题
void broadcastTitle(const string& text, const string& type,
	int fade_in_duration, int remain_duration, int fade_out_duration) {
	auto t = magic_enum::enum_cast<TitleType>(type);
	if (!t)
		throw py::value_error("Invalid title type " + type);
	Level::broadcastTitle(text, t.value(), fade_in_duration, remain_duration, fade_out_duration);
}
//获取方块
PyBlock getBlock(const BlockPos& pos, int dim) {
	BlockSource* bs = Level::getBlockSource(dim);
	if (bs == nullptr)
		throw py::value_error("Unknown dimension ID");
	return Level::getBlockInstance(pos, bs);
}
//设置方块
void setBlock(const BlockPos& pos, int dim, const string& name, int tile_data) {
	Level::setBlock(pos, dim, name, tile_data);
}
//设置方块
void setBlock(const BlockPos& pos, int dim, PyBlock& b) {
	Level::setBlock(pos, dim, b.thiz.getBlock());
}
//从指定地点获取NBT结构数据
PyNBT getStructure(const BlockPos& pos1, const BlockPos& pos2, int dim,
	bool ignore_entities = true, bool ignore_blocks = false) {
	auto st = StructureTemplate::fromWorld("name", dim, pos1, pos2, ignore_entities, ignore_blocks);
	return st.save();
}
//从NBT结构数据导出结构到指定地点
void setStructure(const PyNBT& nbt, const BlockPos& pos, int dim,
	const string& mirror_str, const string& rotation_str) {
	//enum Mirror : unsigned char {
	//	None_15 = 0,
	//	X,
	//	Z,
	//	XZ,
	//};
	//enum Rotation : unsigned char {
	//	None_14 = 0,
	//	Rotate90,
	//	Rotate180,
	//	Rotate270,
	//	Total,
	//};
	auto mir = magic_enum::enum_cast<Mirror>(mirror_str);
	if (!mir)
		throw py::value_error("Invalid mirror type " + mirror_str);
	auto rot = magic_enum::enum_cast<Rotation>(rotation_str);
	if (!rot)
		throw py::value_error("Invalid rotation type " + rotation_str);
	StructureTemplate::fromTag("name", *nbt.thiz->asCompoundTag())
		.toWorld(dim, pos, mir.value(), rot.value());
	/*for (int x = 0; x != size.x; ++x) {
		for (int y = 0; y != size.y; ++y) {
			for (int z = 0; z != size.z; ++z) {
				BlockPos pos{ x, y, z };
				bs->neighborChanged(pos, pos);
			}
		}
	}*/
}
//产生爆炸
void explode(const Vec3& pos, int dim, float power, bool destroy, float range, bool fire) {
	BlockSource* bs = Level::getBlockSource(dim);
	if (!bs)
		throw py::value_error("Unknown dimension ID");
	Global<Level>->explode(*bs, nullptr, pos, power, fire, destroy, range, true);
}
//生成物品
void spawnItem(PyItem& item, Vec3& pos, int dim) {
	Global<Level>->getSpawner().spawnItem(pos, dim, &item.thiz);
}
//设置牌子文字
void setSignBlockMessage(const string& name, BlockPos bp, int dim) {
	BlockSource* bs = Level::getBlockSource(dim);
	if (bs == nullptr)
		throw py::value_error("Unknown dimension ID");
	auto sign = static_cast<SignBlockActor*>(Global<Level>->getBlockEntity(bp, dim));
	sign->setMessage(name, name);
	sign->setChanged();
}

#pragma endregion

PYBIND11_EMBEDDED_MODULE(mc, m) {
	using py::literals::operator""_a;
	py::class_<PyNBT>(m, "NBT")
		.def(py::init<string, py::object>(), "type"_a, "value"_a = nullptr)
		.def("__getitem__", &PyNBT::__getitem__)
		.def("__setitem__", &PyNBT::__setitem__)
		.def("asByte", &PyNBT::asByte)
		.def("asShort", &PyNBT::asShort)
		.def("asInt", &PyNBT::asInt)
		.def("asInt64", &PyNBT::asInt64)
		.def("asFloat", &PyNBT::asFloat)
		.def("asDouble", &PyNBT::asDouble)
		.def("getType", &PyNBT::getType)
		.def("toBinary", &PyNBT::toBinary)
		.def("toJson", &PyNBT::toJson)
		.def("toSNBT", &PyNBT::toSNBT)
		.def("append", &PyNBT::append)
		;
	py::class_<PyEntity>(m, "Entity")
		.def("getName", &PyEntity::getName)
		.def("setName", &PyEntity::setName)
		.def("getUuid", &PyEntity::getUuid)
		.def("getXuid", &PyEntity::getXuid)
		.def("getPos", &PyEntity::getPos)
		.def("getDimensionId", &PyEntity::getDimensionId)
		.def("isStanding", &PyEntity::isStanding)
		.def("isSneaking", &PyEntity::isSneaking)
		.def("isSitting", &PyEntity::isSitting)
		.def("getTypeId", &PyEntity::getTypeId)
		.def("getTypeName", &PyEntity::getTypeName)
		.def("getNBT", &PyEntity::getNBT)
		.def("setNBT", &PyEntity::setNBT)
		.def("getHealth", &PyEntity::getHealth)
		.def("getMaxHealth", &PyEntity::getMaxHealth)
		.def("getPermissions", &PyEntity::getPermissions)
		.def("setPermissions", &PyEntity::setPermissions)
		.def("getPlatformOnlineId", &PyEntity::getPlatformOnlineId)
		.def("getPlatform", &PyEntity::getPlatform)
		.def("getIP", &PyEntity::getIP)
		.def("setHand", &PyEntity::setHand)
		.def("addItem", &PyEntity::addItem)
		.def("removeItem", &PyEntity::removeItem)
		.def("teleport", &PyEntity::teleport)
		.def("sendText", &PyEntity::sendText)
		.def("runCommand", &PyEntity::runCommand)
		.def("resendAllChunks", &PyEntity::resendAllChunks)
		.def("disconnect", &PyEntity::disconnect)
		.def("getScore", &PyEntity::getScore)
		.def("setScore", &PyEntity::setScore)
		.def("addScore", &PyEntity::addScore)
		.def("reduceScore", &PyEntity::reduceScore)
		.def("addLevel", &PyEntity::addLevel)
		.def("transferServer", &PyEntity::transferServer)
		.def("sendCustomForm", &PyEntity::sendCustomForm)
		.def("sendSimpleForm", &PyEntity::sendSimpleForm)
		.def("sendModalForm", &PyEntity::sendModalForm)
		.def("setSidebar", &PyEntity::setSidebar)
		.def("removeSidebar", &PyEntity::removeSidebar)
		.def("setBossbar", &PyEntity::setBossbar)
		.def("removeBossbar", &PyEntity::removeBossbar)
		.def("addTag", &PyEntity::addTag)
		.def("removeTag", &PyEntity::removeTag)
		.def("getTags", &PyEntity::getTags)
		.def("kill", &PyEntity::kill)
		.def("getGameMode", &PyEntity::getGameMode)
		;
	py::class_<PyLogger>(m, "Logger")
		.def(py::init<string>())
		.def("info", &PyLogger::info)
		.def("debug", &PyLogger::debug)
		.def("fatal", &PyLogger::fatal)
		.def("error", &PyLogger::error)
		;
	py::class_<Vec3>(m, "Vec3")
		.def(py::init<float, float, float>())
		.def_property("x", [] (const Vec3& pos) {return pos.x; }, [] (Vec3& pos, float val) {pos.x = val; })
		.def_property("y", [] (const Vec3& pos) {return pos.y; }, [] (Vec3& pos, float val) {pos.y = val; })
		.def_property("z", [] (const Vec3& pos) {return pos.z; }, [] (Vec3& pos, float val) {pos.z = val; })
		;
	py::class_<BlockPos>(m, "BlockPos")
		.def(py::init<int, int, int>())
		.def_property("x", [] (const BlockPos& pos) {return pos.x; }, [] (BlockPos& pos, int val) {pos.x = val; })
		.def_property("y", [] (const BlockPos& pos) {return pos.y; }, [] (BlockPos& pos, int val) {pos.y = val; })
		.def_property("z", [] (const BlockPos& pos) {return pos.z; }, [] (BlockPos& pos, int val) {pos.z = val; })
		;
	m
		.def("getBDSVersion", &Common::getServerVersionString)
		.def("runCommand", &Level::runcmd)
		.def("runCommandEx", &Level::runcmdEx)
		.def("setListener", &setListener)
		.def("registerCommand", &registerCommand)
		.def("getPlayerByXuid", &getPlayerByXuid)
		.def("getPlayerList", &getPlayerList)
		.def("getEntityList", &getEntityList)
		.def("setServerMotd", &setServerMotd)
		.def("broadcastText", &broadcastText)
		.def("broadcastTitle", &broadcastTitle)
		.def("getBlock", &getBlock)
		.def("setBlock", py::overload_cast<const BlockPos&, int, const string&, int>(&setBlock))
		.def("setBlock", py::overload_cast<const BlockPos&, int, PyBlock&>(&setBlock))
		.def("getStructure", &getStructure)
		.def("setStructure", &setStructure)
		.def("explode", &explode)
		.def("spawnItem", &spawnItem)
		.def("isSlimeChunk", &IsSlimeChunk)
		.def("setSignBlockMessage", &setSignBlockMessage)
		;
}
