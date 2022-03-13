#include "Common.h"

using namespace std;
#pragma region Mess
//转Player
Player* P(Actor* a) {
	if (!IsPlayer(a))
		throw std::runtime_error("The ptr is not Player*");
	return static_cast<Player*>(a);
}
//struct PyNBT;
//struct PyContainer;
//struct PyItem;
//struct PyEntity;
//struct PyBlock;
//struct PyLogger;
#pragma endregion
#pragma region NBT
struct PyNBT {
	unique_ptr<Tag> thiz;

	PyNBT(unique_ptr<CompoundTag>&& other) { thiz = move(other); }
	PyNBT(unique_ptr<Tag>&& other) { thiz = move(other); }
	PyNBT(const PyNBT& other) { thiz = other.thiz->copy(); }
	PyNBT(PyNBT&& other) { thiz = move(other.thiz); }
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

	PyNBT __getitem__(int key) { return thiz->asListTag()->operator[](key)->copy(); }
	PyNBT __getitem__(const string& key) { return thiz->asCompoundTag()->operator[](key)->copy(); }
	void __setitem__(const string& key, const PyNBT& val) { thiz->asCompoundTag()->put(key, val.thiz->copy()); }
	auto asByte() { return thiz->asByteTag()->value(); }
	auto asShort() { return thiz->asShortTag()->value(); }
	auto asInt() { return thiz->asIntTag()->value(); }
	auto asInt64() { return thiz->asInt64Tag()->value(); }
	auto asFloat() { return thiz->asFloatTag()->value(); }
	auto asDouble() { return thiz->asDoubleTag()->value(); }
	string_view getType() { return magic_enum::enum_name(thiz->getTagType()); }
	py::bytes toBinary() { return thiz->asCompoundTag()->toBinaryNBT(); }
	string toJson(int indentatiton = 4) { return thiz->toJson(indentatiton); }
	string toSNBT() { return thiz->asCompoundTag()->toSNBT(); }
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
	ItemStack* thiz;

	PyItem(ItemStack* other) :thiz(other) {}
	//PyItem(const ItemStack& other) :thiz(new ItemStack(other)) {}
	PyItem(const PyNBT& nbt) :thiz(new ItemStack(ItemStack::fromTag(*nbt.thiz->asCompoundTag()))) {}
	string getName() { return thiz->getName(); }
	PyNBT getNBT() { return PyNBT(thiz->getNbt()); }
};
#pragma endregion
#pragma region Container
struct PyContainer {
	Container* thiz;

	PyContainer(const string& type_str, const py::object& value) {}

	PyItem __getitem__(int slot) { return thiz->getSlot(slot); }
	void __setitem__(int slot, const PyItem& val) { *thiz->getSlot(slot) = *val.thiz; }
	vector<PyItem> getAllSlots() {
		vector<PyItem> slots;
		for (auto& i : thiz->getAllSlots()) {
			slots.push_back(const_cast<ItemStack*>(i));
		}
		return slots;
	}
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
	BlockPos getPos() { return thiz->getPos(); }
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
	PyNBT getNBT() { return CompoundTag::fromActor(thiz); }
	//设置nbt数据
	bool setNBT(PyNBT& nbt) { return thiz->setNbt(nbt.thiz->asCompoundTag()); }
	//获取生命值
	int getHealth() { return thiz->getHealth(); }
	//获取最大生命值
	int getMaxHealth() { return thiz->getMaxHealth(); }
	//获取游戏模式
	int getGameMode() { return P(thiz)->getPlayerGameType(); }
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
	//获取背包
	PyContainer getInventory() { P(thiz)->getInventory(); }
	//设置玩家手上物品
	void setHand(const PyItem& item) {
		const_cast<ItemStack&>(P(thiz)->getSelectedItem()) = *item.thiz;
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
			[this, cb] (string arg) {cb(new PyEntity(thiz), arg); });
	}
	void sendSimpleForm(const string& title, const string& content,
		const vector<string>& buttons, const vector<string>& images, const py::function& cb) {
		if (buttons.size() != images.size())
			throw py::value_error("The number of buttons is not equal to the number of images");
		P(thiz)->sendSimpleFormPacket(title, content, buttons, images,
			[this, cb] (int arg) {cb(new PyEntity(thiz), arg); });
	}
	void sendModalForm(const string& title, const string& content,
		const string& button1, const string& button2, const py::function& cb) {
		P(thiz)->sendModalFormPacket(title, content, button1, button2,
			[this, cb] (bool arg) {cb(new PyEntity(thiz), arg);	});
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
};
#pragma endregion
#pragma region Block
struct PyBlock {
	BlockInstance thiz;

	PyBlock(const BlockInstance& bi) :thiz(bi) {}
	PyBlock(const BlockPos& pos, int dim) :thiz(Level::getBlockInstance(pos, dim)) {}
	string getName() { return thiz.getBlock()->getName().getString(); }
	PyNBT getNBT() { return CompoundTag::fromBlock(thiz.getBlock()); }
	BlockPos getPos() { return thiz.getPosition(); }
	int getDimensionId() { return thiz.getDimensionId(); }
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
#pragma region Functions
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
	g_commands[cmd] = make_pair(des, py::function(cb));
	py::print(cb);
	py::print(g_commands[cmd].second);
}
//获取玩家
PyEntity getPlayer(const string& name) {
	Player* p = Level::getPlayer(name);
	if (p == nullptr)
		throw runtime_error("Failed to find player");
	return p;
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
void setServerMotd(const string& motd) {
	if (Global<ServerNetworkHandler> == nullptr)
		throw runtime_error("Server dim not finish loading");
	LL::setServerMotd(motd);
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
	Global<Level>->getSpawner().spawnItem(pos, dim, item.thiz);
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
//是否为史莱姆区块
constexpr int IsSlimeChunk(unsigned x, unsigned z) {
	unsigned mt0 = (x * 0x1F1F1F1F) ^ z;
	unsigned mt1 = (1812433253u * (mt0 ^ (mt0 >> 30u)) + 1);
	unsigned mt2 = mt1;
	for (unsigned i = 2; i < 398; ++i) {
		mt2 = (1812433253u * (mt2 ^ (mt2 >> 30u)) + i);
	}
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
#pragma endregion
PYBIND11_EMBEDDED_MODULE(mc, m) {
	using py::literals::operator""_a;
#pragma region NBT
	py::class_<PyNBT>(m, "NBT")
		.def(py::init<string, py::object>(), "type"_a, "value"_a = nullptr)
		.def("__getitem__", py::overload_cast<int>(&PyNBT::__getitem__))
		.def("__getitem__", py::overload_cast<const string&>(&PyNBT::__getitem__))
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
#pragma endregion
#pragma region Item
	py::class_<PyItem>(m, "Item")
		.def(py::init<PyNBT>(), "nbt"_a)
		.def("getName", &PyItem::getName)
		.def("getNBT", &PyItem::getNBT)
		;
#pragma endregion
#pragma region Container
	py::class_<PyContainer>(m, "Container")
		.def("__getitem__", &PyContainer::__getitem__)
		.def("__setitem__", &PyContainer::__setitem__)
		.def("getAllSlots", &PyContainer::getAllSlots)
		;
#pragma endregion
#pragma region Entity
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
		.def("getGameMode", &PyEntity::getGameMode)
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
		;
#pragma endregion
#pragma region Block
	py::class_<PyBlock>(m, "Block")
		.def(py::init<BlockPos, int>())
		.def("getName", &PyBlock::getName)
		.def("getDimensionId", &PyBlock::getDimensionId)
		.def("getNBT", &PyBlock::getNBT)
		.def("getPos", &PyBlock::getPos)
		;
#pragma endregion
#pragma region Logger
	py::class_<PyLogger>(m, "Logger")
		.def(py::init<string>())
		.def("info", &PyLogger::info)
		.def("debug", &PyLogger::debug)
		.def("fatal", &PyLogger::fatal)
		.def("error", &PyLogger::error)
		;
#pragma endregion
#pragma region Functions
	m
		.def("getBDSVersion", &LL::getBdsVersion)
		.def("runCommand", &Level::runcmd)
		.def("runCommandEx", &Level::runcmdEx)
		.def("setListener", &setListener)
		.def("registerCommand", &registerCommand)
		.def("getPlayer", &getPlayer)
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
		.def("setSignBlockMessage", &setSignBlockMessage)
		.def("isSlimeChunk", &IsSlimeChunk)
		;
#pragma endregion
#pragma region Vec3
	py::class_<Vec3>(m, "Vec3")
		.def(py::init<float, float, float>())
		.def_property("x", [] (const Vec3& pos) {return pos.x; }, [] (Vec3& pos, float val) {pos.x = val; })
		.def_property("y", [] (const Vec3& pos) {return pos.y; }, [] (Vec3& pos, float val) {pos.y = val; })
		.def_property("z", [] (const Vec3& pos) {return pos.z; }, [] (Vec3& pos, float val) {pos.z = val; })
		.def("toString", &Vec3::toString)
		;
#pragma endregion
#pragma region BockPos
	py::class_<BlockPos>(m, "BlockPos")
		.def(py::init<int, int, int>())
		.def_property("x", [] (const BlockPos& pos) {return pos.x; }, [] (BlockPos& pos, int val) {pos.x = val; })
		.def_property("y", [] (const BlockPos& pos) {return pos.y; }, [] (BlockPos& pos, int val) {pos.y = val; })
		.def_property("z", [] (const BlockPos& pos) {return pos.z; }, [] (BlockPos& pos, int val) {pos.z = val; })
		.def("toString", &BlockPos::toString)
		;
#pragma endregion
#pragma region SimpleForm
	py::class_<Form::SimpleForm>(m, "SimpleForm")
		.def(py::init<string, string>())
		.def_property("x", [] (const BlockPos& pos) {return pos.x; }, [] (BlockPos& pos, int val) {pos.x = val; })
		.def_property("y", [] (const BlockPos& pos) {return pos.y; }, [] (BlockPos& pos, int val) {pos.y = val; })
		.def_property("z", [] (const BlockPos& pos) {return pos.z; }, [] (BlockPos& pos, int val) {pos.z = val; })
		.def("sendTo", &Form::SimpleForm::sendTo)
		;
#pragma endregion
}
//事件回调，初始化对象将申请GIL
class Callbacker {
public:
	Callbacker(EventCode t) :type_(t), arg_() {}
	~Callbacker() {
		logger.debug("{}", arg_.ref_count());
	}
	//事件回调
	bool callback() {
		bool pass = true;
		arg_.inc_ref();
		for (auto cb : g_cb_functions[type_]) {
			//TODO: 为什么需要增加引用计数？
			pass = cb(arg_) != py::bool_(false);
		}
		return pass;
	}
	template <typename T>
	Callbacker& insert(const char* key, const T& item) {
		arg_[key] = item;
		return *this;
	}

private:
	EventCode type_;
	py::dict arg_;
	py::gil_scoped_acquire gil_;
};

#define EVENT_BEGIN(evt) evt::subscribe([code](evt e){Callbacker h(code); h.insert("Event", py::cast(magic_enum::enum_name(code))); PY_TRY
#define EVENT_INSERT(key) h.insert(#key, e.m##key)
#define EVENT_INSERT_EX(key, value) h.insert(#key, (value))
#define EVENT_END PY_CATCH return h.callback();})

void EnableEventListener(EventCode code) {
	using namespace Event;
	switch (code) {
	case EventCode::onPreJoin:
		EVENT_BEGIN(PlayerPreJoinEvent);
		EVENT_INSERT(IP);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(XUID);
		EVENT_END;
		break;
	case EventCode::onJoin:
		EVENT_BEGIN(PlayerJoinEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onLeft:
		EVENT_BEGIN(PlayerLeftEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(XUID);
		EVENT_END;
		break;
	case EventCode::onPlayerCmd:
		EVENT_BEGIN(PlayerCmdEvent);
		EVENT_INSERT(Command);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT_EX(isSuccess, e.mResult->isSuccess());
		EVENT_END;
		break;
	case EventCode::onChat:
		EVENT_BEGIN(PlayerChatEvent);
		EVENT_INSERT(Message);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onPlayerDie:
		EVENT_BEGIN(PlayerDieEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT_EX(Cause, magic_enum::enum_name(e.mDamageSource->getCause()));
		EVENT_INSERT_EX(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onRespawn:
		EVENT_BEGIN(PlayerRespawnEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onChangeDim:
		EVENT_BEGIN(PlayerChangeDimEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(ToDimensionId);
		EVENT_END;
		break;
	case EventCode::onJump:
		EVENT_BEGIN(PlayerJumpEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onSneak:
		EVENT_BEGIN(PlayerSneakEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(IsSneaking);
		EVENT_END;
		break;
	case EventCode::onAttack:
		EVENT_BEGIN(PlayerAttackEvent);
		EVENT_INSERT(AttackDamage);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT_EX(Target, PyEntity(e.mTarget));
		EVENT_END;
		break;
	case EventCode::onEat:
		EVENT_BEGIN(PlayerEatEvent);
		EVENT_INSERT_EX(FoodItem, PyItem(e.mFoodItem));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onMove:
		EVENT_BEGIN(PlayerMoveEvent);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(Pos);
		EVENT_END;
		break;
	case EventCode::onChangeSprinting:
		EVENT_BEGIN(PlayerSprintEvent);
		EVENT_INSERT(IsSprinting);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onSpawnProjectile:
		EVENT_BEGIN(ProjectileSpawnEvent);
		//EVENT_INSERT(Identifier);
		EVENT_INSERT_EX(Shooter, PyEntity(e.mShooter));
		EVENT_INSERT(Type);
		EVENT_END;
		break;
	case EventCode::onFireworkShootWithCrossbow:
		break;
	case EventCode::onSetArmor:
		EVENT_BEGIN(PlayerSetArmorEvent);
		EVENT_INSERT_EX(ArmorItem, PyItem(e.mArmorItem));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(Slot);
		EVENT_END;
		break;
	case EventCode::onRide:
		EVENT_BEGIN(EntityRideEvent);
		EVENT_INSERT_EX(Rider, PyEntity(e.mRider));
		EVENT_INSERT_EX(Vehicle, PyEntity(e.mVehicle));
		EVENT_END;
		break;
	case EventCode::onStepOnPressurePlate:
		break;
	case EventCode::onUseItem:
		EVENT_BEGIN(PlayerUseItemEvent);
		EVENT_INSERT_EX(ItemStack, PyItem(e.mItemStack));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onPickupItem:
		EVENT_BEGIN(PlayerPickupItemEvent);
		EVENT_INSERT_EX(ItemEntity, PyEntity(e.mItemEntity));
		EVENT_INSERT_EX(ItemStack, PyItem(e.mItemStack));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onDropItem:
		EVENT_BEGIN(PlayerDropItemEvent);
		EVENT_INSERT_EX(ItemStack, PyItem(e.mItemStack));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onUseItemOn:
		EVENT_BEGIN(PlayerUseItemOnEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT_EX(ItemStack, PyItem(e.mItemStack));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onInventoryChange:
		EVENT_BEGIN(PlayerInventoryChangeEvent);
		EVENT_INSERT(NewItemStack);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(PreviousItemStack);
		EVENT_INSERT(Slot);
		EVENT_END;
		break;
	case EventCode::onChangeArmorStand:
		EVENT_BEGIN(ArmorStandChangeEvent);
		//EVENT_INSERT(ArmorStand); todo
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(Slot);
		EVENT_END;
		break;
	case EventCode::onStartDestroyBlock:
		EVENT_BEGIN(PlayerStartDestroyBlockEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onDestroyBlock:
		EVENT_BEGIN(PlayerDestroyBlockEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onWitherBossDestroy:
		EVENT_BEGIN(WitherBossDestroyEvent);
		//TODO: AABB and WitherBoss
		EVENT_END;
		break;
	case EventCode::onPlaceBlock:
		EVENT_BEGIN(PlayerPlaceBlockEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onLiquidFlow:
		break;
	case EventCode::onOpenContainer:
		EVENT_BEGIN(PlayerOpenContainerEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		//TODO: EVENT_INSERT(Container);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onCloseContainer:
		EVENT_BEGIN(PlayerCloseContainerEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		//TODO: EVENT_INSERT(Container);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onContainerChange:
		EVENT_BEGIN(ContainerChangeEvent);
		EVENT_INSERT(Actor);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		//TODO: EVENT_INSERT(Container);
		EVENT_INSERT(NewItemStack);
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(PreviousItemStack);
		EVENT_INSERT(Slot);
		EVENT_END;
		break;
	case EventCode::onOpenContainerScreen:
		break;
	case EventCode::onExplode:
		break;
	case EventCode::onBlockExploded:
		break;
	case EventCode::onBedExplode:
		break;
	case EventCode::onRespawnAnchorExplode:
		break;
	case EventCode::onEntityExplode:
		break;
	case EventCode::onBlockExplode:
		EVENT_BEGIN(BlockExplodeEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT(Breaking);
		EVENT_INSERT(Fire);
		EVENT_INSERT(MaxResistance);
		EVENT_INSERT(Radius);
		EVENT_END;
		break;
	case EventCode::onMobDie:
		EVENT_BEGIN(MobDieEvent);
		EVENT_INSERT(Mob);
		EVENT_INSERT_EX(Cause, magic_enum::enum_name(e.mDamageSource->getCause()));
		EVENT_INSERT_EX(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onMobHurt:
		EVENT_BEGIN(MobHurtEvent);
		EVENT_INSERT(Mob);
		EVENT_INSERT_EX(Cause, magic_enum::enum_name(e.mDamageSource->getCause()));
		EVENT_INSERT_EX(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onCmdBlockExecute:
		EVENT_BEGIN(CmdBlockExecuteEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT(Command);
		EVENT_INSERT(IsMinecart);
		EVENT_INSERT(Minecart);
		EVENT_END;
		break;
	case EventCode::onRedStoneUpdate:
		EVENT_BEGIN(RedStoneUpdateEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT(IsActivated);
		EVENT_INSERT(RedStonePower);
		EVENT_END;
		break;
	case EventCode::onProjectileHitEntity:
		EVENT_BEGIN(ProjectileHitEntityEvent);
		EVENT_INSERT(Source);
		EVENT_INSERT(Target);
		EVENT_END;
		break;
	case EventCode::onProjectileHitBlock:
		EVENT_BEGIN(ProjectileHitBlockEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT(Source);
		EVENT_END;
		break;
	case EventCode::onBlockInteracted:
		EVENT_BEGIN(BlockInteractedEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onUseRespawnAnchor:
		EVENT_BEGIN(PlayerUseRespawnAnchorEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onFarmLandDecay:
		EVENT_BEGIN(FarmLandDecayEvent);
		EVENT_INSERT(Actor);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_END;
		break;
	case EventCode::onUseFrameBlock:
		EVENT_BEGIN(PlayerUseFrameBlockEvent);
		EVENT_INSERT_EX(BlockInstance, PyBlock(e.mBlockInstance));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT_EX(Type, magic_enum::enum_name(e.mType));
		EVENT_END;
		break;
	case EventCode::onPistonPush:
		EVENT_BEGIN(PistonPushEvent);
		EVENT_INSERT(PistonBlockInstance);
		EVENT_INSERT(TargetBlockInstance);
		EVENT_END;
		break;
	case EventCode::onHopperSearchItem:
		EVENT_BEGIN(HopperSearchItemEvent);
		EVENT_INSERT(DimensionId);
		EVENT_INSERT(HopperBlock);
		EVENT_INSERT(MinecartPos);
		EVENT_END;
		break;
	case EventCode::onHopperPushOut:
		EVENT_BEGIN(HopperPushOutEvent);
		EVENT_INSERT(DimensionId);
		EVENT_INSERT(Pos);
		EVENT_END;
		break;
	case EventCode::onFireSpread:
		EVENT_BEGIN(FireSpreadEvent);
		EVENT_INSERT(DimensionId);
		EVENT_INSERT(Target);
		EVENT_END;
		break;
	case EventCode::onBlockChanged:
		EVENT_BEGIN(BlockChangedEvent);
		EVENT_INSERT_EX(NewBlockInstance, PyBlock(e.mNewBlockInstance));
		EVENT_INSERT_EX(PreviousBlockInstance, PyBlock(e.mPreviousBlockInstance));
		EVENT_END;
		break;
	case EventCode::onNpcCmd:
		EVENT_BEGIN(NpcCmdEvent);
		EVENT_INSERT(Command);
		EVENT_INSERT_EX(Npc, PyEntity(e.mNpc));
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_END;
		break;
	case EventCode::onScoreChanged:
		EVENT_BEGIN(PlayerScoreChangedEvent);
		EVENT_INSERT_EX(ObjectiveName, e.mObjective->getName());
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT(Score);
		//TODO: more info about Objective
		EVENT_END;
		break;
	case EventCode::onServerStarted:
		EVENT_BEGIN(ServerStartedEvent);
		EVENT_END;
		break;
	case EventCode::onConsoleCmd:
		EVENT_BEGIN(ConsoleCmdEvent);
		EVENT_INSERT(Command);
		EVENT_END;
		break;
	case EventCode::onConsoleOutput:
		EVENT_BEGIN(ConsoleOutputEvent);
		EVENT_INSERT(Output);
		EVENT_END;
		break;
	case EventCode::onConsumeTotem:
		break;
	case EventCode::onEffectChanged:
		EVENT_BEGIN(PlayerEffectChangedEvent);
		//TODO: more info about Effect
		EVENT_INSERT_EX(Effect, e.mEffect->getDisplayName());
		EVENT_INSERT_EX(Player, PyEntity(e.mPlayer));
		EVENT_INSERT_EX(Type, magic_enum::enum_name(e.mEventType));
		EVENT_END;
		break;
	default:
		break;
	}
}
void PrintError(const std::exception& e) {
	logger.error("\n{}", e.what());
}

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
		logger.error("\nParsing JSON failed! {}", e.what());
		return nullptr;
	}
}
//将Python解释器初始化插入bds主函数
THook(int, "main", int argc, char* argv[], char* envp[]) {
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
	PY_TRY;
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
	PY_CATCH;
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
		PY_TRY;
		py::gil_scoped_acquire _gil;
		for (auto& [cmd, data] : g_commands) {
			if (e.mCommand._Starts_with(cmd)) {
				//TODO: 为什么崩溃
				data.second(PyEntity(e.mPlayer), e.mCommand);
				return false;
			}
		}
		PY_CATCH;
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
