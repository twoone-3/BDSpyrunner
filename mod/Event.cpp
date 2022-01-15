#include "Event.h"
#include "Tool.h"
#include "Module.h"

using namespace std;
//事件回调助手，创建会申请GIL
class Callbacker {
public:
	Callbacker(EventCode t) :
		type_(t), arg_(nullptr), gil_() {
	}
	~Callbacker() {
		Py_XDECREF(arg_);
	}
	//事件回调
	bool call() {
		bool intercept = true;
		//如果没有则跳过
		auto& cbs = g_callback_functions[type_];
		for (auto cb : cbs) {
			PyObject* result = PyObject_CallFunction(cb, "O", arg_);
			PrintPythonError();
			if (result == Py_False)
				intercept = false;
			Py_XDECREF(result);
		}
		return intercept;
	}
	Callbacker& setArg(PyObject* arg) {
		arg_ = arg;
		return *this;
	}
	Callbacker& insert(string_view key, PyObject* item) {
		if (arg_ == nullptr)
			arg_ = PyDict_New();
		PyDict_SetItemString(arg_, key.data(), item);
		Py_DECREF(item);
		return *this;
	}
	Callbacker& insert(string_view key, string_view item) {
		return insert(key, ToPyStr(item));
	}
	Callbacker& insert(string_view key, Actor* item) {
		return insert(key, ToEntity(item));
	}
	Callbacker& insert(string_view key, ItemStack* item) {
		return insert(key, ToItem(item));
	}
	Callbacker& insert(string_view key, BlockPos* item) {
		return insert(key, ToList(item));
	}
	Callbacker& insert(string_view key, BlockPos& item) {
		return insert(key, ToList(&item));
	}
	Callbacker& insert(string_view key, const Vec3& item) {
		return insert(key, ToList(item));
	}
	Callbacker& insert(string_view key, short item) {
		return insert(key, PyLong_FromLong(item));
	}
	Callbacker& insert(string_view key, int item) {
		return insert(key, PyLong_FromLong(item));
	}
	Callbacker& insert(string_view key, unsigned item) {
		return insert(key, PyLong_FromUnsignedLong(item));
	}
	Callbacker& insert(string_view key, long long item) {
		return insert(key, PyLong_FromLongLong(item));
	}
	Callbacker& insert(string_view key, unsigned long long item) {
		return insert(key, PyLong_FromUnsignedLongLong(item));
	}
	Callbacker& insert(string_view key, float item) {
		return insert(key, PyLong_FromDouble(item));
	}
private:
	EventCode type_;
	PyObject* arg_;
	PyGILGuard gil_;
};

static const std::unordered_map<string, EventCode> g_events{
	{ "onPreJoin", EventCode::onPreJoin },
	{ "onJoin", EventCode::onJoin },
	{ "onLeft", EventCode::onLeft },
	{ "onPlayerCmd", EventCode::onPlayerCmd },
	{ "onChat", EventCode::onChat },
	{ "onPlayerDie", EventCode::onPlayerDie },
	{ "onRespawn", EventCode::onRespawn },
	{ "onChangeDim", EventCode::onChangeDim },
	{ "onJump", EventCode::onJump },
	{ "onSneak", EventCode::onSneak },
	{ "onAttack", EventCode::onAttack },
	{ "onEat", EventCode::onEat },
	{ "onMove", EventCode::onMove },
	{ "onChangeSprinting", EventCode::onChangeSprinting },
	{ "onSpawnProjectile", EventCode::onSpawnProjectile },
	{ "onFireworkShootWithCrossbow", EventCode::onFireworkShootWithCrossbow },
	{ "onSetArmor", EventCode::onSetArmor },
	{ "onRide", EventCode::onRide },
	{ "onStepOnPressurePlate", EventCode::onStepOnPressurePlate },
	{ "onMobDie", EventCode::onMobDie },
	{ "onMobHurt", EventCode::onMobHurt },
	{ "onUseItem", EventCode::onUseItem },
	{ "onTakeItem", EventCode::onTakeItem },
	{ "onDropItem", EventCode::onDropItem },
	{ "onUseItemOn", EventCode::onUseItemOn },
	{ "onInventoryChange", EventCode::onInventoryChange },
	{ "onChangeArmorStand", EventCode::onChangeArmorStand },
	{ "onStartDestroyBlock", EventCode::onStartDestroyBlock },
	{ "onDestroyBlock", EventCode::onDestroyBlock },
	{ "onWitherBossDestroy", EventCode::onWitherBossDestroy },
	{ "onPlaceBlock", EventCode::onPlaceBlock },
	{ "onExplode", EventCode::onExplode },
	{ "onBedExplode", EventCode::onBedExplode },
	{ "onRespawnAnchorExplode", EventCode::onRespawnAnchorExplode },
	{ "onBlockExploded", EventCode::onBlockExploded },
	{ "onEntityExplode", EventCode::onEntityExplode },
	{ "onBlockExplode", EventCode::onBlockExplode },
	{ "onLiquidFlow", EventCode::onLiquidFlow },
	{ "onOpenContainer", EventCode::onOpenContainer },
	{ "onCloseContainer", EventCode::onCloseContainer },
	{ "onContainerChangeSlot", EventCode::onContainerChange },
	{ "onContainerChange", EventCode::onContainerChange },
	{ "onOpenContainerScreen", EventCode::onOpenContainerScreen },
	{ "onCmdBlockExecute", EventCode::onCmdBlockExecute },
	{ "onRedStoneUpdate", EventCode::onRedStoneUpdate },
	{ "onProjectileHitBlock", EventCode::onProjectileHitBlock },
	{ "onProjectileHitEntity", EventCode::onProjectileHitEntity },
	{ "onBlockInteracted", EventCode::onBlockInteracted },
	{ "onUseRespawnAnchor", EventCode::onUseRespawnAnchor },
	{ "onFarmLandDecay", EventCode::onFarmLandDecay },
	{ "onUseFrameBlock", EventCode::onUseFrameBlock },
	{ "onPistonPush", EventCode::onPistonPush },
	{ "onHopperSearchItem", EventCode::onHopperSearchItem },
	{ "onHopperPushOut", EventCode::onHopperPushOut },
	{ "onFireSpread", EventCode::onFireSpread },
	{ "onBlockChanged", EventCode::onBlockChanged },
	{ "onNpcCmd", EventCode::onNpcCmd },
	{ "onScoreChanged", EventCode::onScoreChanged },
	{ "onServerStarted", EventCode::onServerStarted },
	{ "onConsoleCmd", EventCode::onConsoleCmd },
	{ "onConsoleOutput", EventCode::onConsoleOutput },
	{ "onTick", EventCode::onTick },
	{ "onMoneyAdd", EventCode::onMoneyAdd },
	{ "onMoneyReduce", EventCode::onMoneyReduce },
	{ "onMoneyTrans", EventCode::onMoneyTrans },
	{ "onMoneySet", EventCode::onMoneySet },
	{ "onFormSelected", EventCode::onFormSelected },
	{ "onConsumeTotem", EventCode::onConsumeTotem },
	{ "onEffectAdded", EventCode::onEffectAdded },
	{ "onEffectRemoved", EventCode::onEffectRemoved },
	{ "onEffectUpdated", EventCode::onEffectUpdated }
};
std::optional<EventCode> StringToEventCode(const std::string& s) {
	auto x = g_events.find(s);
	if (x == g_events.end())
		return nullopt;
	else
		return x->second;
}

#define EVENT_BEGIN(evt) if(!evt::hasListener())evt::subscribe([code](evt e){Callbacker h(code)
#define EVENT_INSERT(key) h.insert(#key, e.m##key)
#define EVENT_INSERT2(key,value) h.insert(#key, value)
#define EVENT_END return h.call();})

void EnableEventListener(EventCode code) {
	switch (code) {
	case EventCode::onPreJoin:
		EVENT_BEGIN(Event::PlayerPreJoinEvent);
		EVENT_INSERT(IP);
		EVENT_INSERT(Player);
		EVENT_INSERT(XUID);
		EVENT_END;
		break;
	case EventCode::onJoin:
		EVENT_BEGIN(Event::PlayerJoinEvent);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onLeft:
		EVENT_BEGIN(Event::PlayerLeftEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(XUID);
		EVENT_END;
		break;
	case EventCode::onPlayerCmd:
		EVENT_BEGIN(Event::PlayerCmdEvent);
		EVENT_INSERT(Command);
		EVENT_INSERT(Player);
		EVENT_INSERT2(isSuccess, e.mResult->isSuccess());
		EVENT_END;
		break;
	case EventCode::onChat:
		EVENT_BEGIN(Event::PlayerChatEvent);
		EVENT_INSERT(Message);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onPlayerDie:
		EVENT_BEGIN(Event::PlayerDieEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT2(Cause, int(e.mDamageSource->getCause()));
		EVENT_INSERT2(Entity, e.mDamageSource->getEntity());
		EVENT_END;
		break;
	case EventCode::onRespawn:
		EVENT_BEGIN(Event::PlayerRespawnEvent);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onChangeDim:
		EVENT_BEGIN(Event::PlayerChangeDimEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(ToDimensionId);
		EVENT_END;
		break;
	case EventCode::onJump:
		EVENT_BEGIN(Event::PlayerJumpEvent);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onSneak:
		EVENT_BEGIN(Event::PlayerSneakEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(IsSneaking);
		EVENT_END;
		break;
	case EventCode::onAttack:
		EVENT_BEGIN(Event::PlayerAttackEvent);
		EVENT_INSERT(AttackDamage);
		EVENT_INSERT(Player);
		EVENT_INSERT(Target);
		EVENT_END;
		break;
	case EventCode::onEat:
		EVENT_BEGIN(Event::PlayerEatEvent);
		EVENT_INSERT(FoodItem);
		EVENT_INSERT(Player);
		EVENT_END;
		break;
	case EventCode::onMove:
		EVENT_BEGIN(Event::PlayerMoveEvent);
		EVENT_INSERT(Player);
		EVENT_INSERT(Pos);
		EVENT_END;
		break;
	case EventCode::onChangeSprinting:
		break;
	case EventCode::onSpawnProjectile:
		break;
	case EventCode::onFireworkShootWithCrossbow:
		break;
	case EventCode::onSetArmor:
		break;
	case EventCode::onRide:
		break;
	case EventCode::onStepOnPressurePlate:
		break;
	case EventCode::onUseItem:
		break;
	case EventCode::onTakeItem:
		break;
	case EventCode::onDropItem:
		break;
	case EventCode::onUseItemOn:
		break;
	case EventCode::onInventoryChange:
		break;
	case EventCode::onChangeArmorStand:
		break;
	case EventCode::onStartDestroyBlock:
		break;
	case EventCode::onDestroyBlock:
		break;
	case EventCode::onWitherBossDestroy:
		break;
	case EventCode::onPlaceBlock:
		break;
	case EventCode::onLiquidFlow:
		break;
	case EventCode::onOpenContainer:
		break;
	case EventCode::onCloseContainer:
		break;
	case EventCode::onContainerChange:
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
		break;
	case EventCode::onMobDie:
		break;
	case EventCode::onMobHurt:
		break;
	case EventCode::onCmdBlockExecute:
		break;
	case EventCode::onRedStoneUpdate:
		break;
	case EventCode::onProjectileHitEntity:
		break;
	case EventCode::onProjectileHitBlock:
		break;
	case EventCode::onBlockInteracted:
		break;
	case EventCode::onUseRespawnAnchor:
		break;
	case EventCode::onFarmLandDecay:
		break;
	case EventCode::onUseFrameBlock:
		break;
	case EventCode::onPistonPush:
		break;
	case EventCode::onHopperSearchItem:
		break;
	case EventCode::onHopperPushOut:
		break;
	case EventCode::onFireSpread:
		break;
	case EventCode::onBlockChanged:
		break;
	case EventCode::onNpcCmd:
		break;
	case EventCode::onScoreChanged:
		break;
	case EventCode::onServerStarted:
		break;
	case EventCode::onConsoleCmd:
		break;
	case EventCode::onFormSelected:
		break;
	case EventCode::onConsoleOutput:
		break;
	case EventCode::onTick:
		break;
	case EventCode::onMoneyAdd:
		break;
	case EventCode::onMoneyReduce:
		break;
	case EventCode::onMoneyTrans:
		break;
	case EventCode::onMoneySet:
		break;
	case EventCode::onConsumeTotem:
		break;
	case EventCode::onEffectAdded:
		break;
	case EventCode::onEffectUpdated:
		break;
	case EventCode::onEffectRemoved:
		break;
	case EventCode::EVENT_COUNT:
		break;
	default:
		break;
	}
}
#if 0
//开服完成
THook(void, "?startServerThread@ServerInstance@@QEAAXXZ",
	uintptr_t _this) {
	Callbacker h(EventCode::onServerStarted);
	h.setArg(Py_None).call();
	original(_this);
}
//控制台输出，实际上是ostrram::operator<<的底层调用
THook(ostream&, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
	ostream& _this, const char* str, uintptr_t size) {
	Callbacker h(EventCode::onConsoleOutput);
	if (&_this == &cout) {
		h.setArg(ToPyStr(str));
		if (!h.call())
			return _this;
	}
	return original(_this, str, size);
}
//控制台输入，实际上是命令队列的底层
THook(bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	uintptr_t _this, string* cmd) {
	Callbacker h(EventCode::ConsoleCmdEvent);
	static bool debug = false;
	if (*cmd == "pydebug") {
		if (debug)
			debug = false;
		else
			debug = true;
		return false;
	}
	if (debug) {
		PyRun_SimpleString(cmd->c_str());
		return false;
	}
	h.setArg(ToPyStr(*cmd));
	if (h.call())
		return original(_this, cmd);
	return false;
}
//玩家加入发包
THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
	ServerNetworkHandler* _this, NetworkIdentifier& id,/*SetLocalPlayerAsInitializedPacket*/ uintptr_t pkt) {
	Callbacker h(EventCode::onPlayerJoin);
	Player* p = reinterpret_cast<Player*>(_this->getServerPlayer(id));
	if (p) {
		h.setArg(ToEntity(p)).call();
	}
	original(_this, id, pkt);
}
//玩家退出
THook(void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	uintptr_t _this, Player* p, char v3) {
	Callbacker h(EventCode::onPlayerLeft);
	h.setArg(ToEntity(p));
	h.call();
	return original(_this, p, v3);
}
//使用物品
THook(bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	uintptr_t _this, ItemStack* item, BlockPos* bp, char a4, uintptr_t a5, Block* b) {
	Callbacker h(EventCode::onUseItem);
	Player* p = Fetch<Player*>(_this, 8);
	const BlockLegacy& bl = b->getLegacyBlock();
	h
		.insert("player", p)
		.insert("itemid", item->getId())
		.insert("itemaux", item->getAuxValue())
		.insert("itemcount", item->getCount())
		.insert("itemname", item->getName())
		.insert("blockname", bl.getBlockEntityType())
		.insert("blockid", bl.getBlockItemId())
		.insert("position", bp);
	if (h.call())
		return original(_this, item, bp, a4, a5, b);
	return false;
}
//放置方块
THook(bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* _this, Block* b, BlockPos* bp, unsigned char a4, Actor* p, bool _bool) {
	Callbacker h(EventCode::onPlaceBlock);
	if (IsPlayer(p)) {
		auto& bl = b->getLegacyBlock();
		h
			.insert("player", p)
			.insert("blockname", bl.getBlockEntityType())
			.insert("blockid", bl.getBlockItemId())
			.insert("position", bp);
		if (!h.call())
			return false;
	}
	return original(_this, b, bp, a4, p, _bool);
}
//破坏方块
THook(bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource& _this, Actor* a1, BlockPos& a2, ItemStack* a3, bool a4) {
	Callbacker h(EventCode::onDestroyBlock);
	if (IsPlayer(a1)) {
		auto& bl = _this.getBlock(a2).getLegacyBlock();
		h
			.insert("player", a1)
			.insert("blockname", bl.getBlockEntityType())
			.insert("blockid", bl.getBlockItemId())
			.insert("position", a2);
		if (!h.call())
			return false;
	}
	return original(_this, a1, a2, a3, a4);
}
//开箱
THook(bool, "?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	Callbacker h(EventCode::onOpenChest);
	h
		.insert("player", p)
		.insert("position", bp);
	if (h.call())
		return original(_this, p, bp);
	return false;
}
//开桶，不能拦截，拦截请看方块交互
THook(bool, "?use@BarrelBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* p, BlockPos* bp) {
	Callbacker h(EventCode::onOpenBarrel);
	h
		.insert("player", p)
		.insert("position", bp);
	if (h.call())
		return original(_this, p, bp);
	return false;
}
//关箱
THook(void, "?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	Callbacker h(EventCode::onCloseChest);
	auto bp = (BlockPos*)(_this - 196);
	h
		.insert("player", p)
		.insert("position", bp);
	original(_this, p);
}
//关桶
THook(void, "?stopOpen@BarrelBlockActor@@UEAAXAEAVPlayer@@@Z",
	uintptr_t _this, Player* p) {
	Callbacker h(EventCode::onCloseBarrel);
	auto bp = (BlockPos*)(_this - 196);
	h
		.insert("player", p)
		.insert("position", bp);
	original(_this, p);
}
//放入取出物品，实际上是容器改变
THook(void, "?containerContentChanged@LevelContainerModel@@UEAAXH@Z",
	uintptr_t _this, unsigned slot) {
	Callbacker h(EventCode::onContainerChange);
	Player* p = Fetch<Player*>(_this, 208);//IDA LevelContainerModel::_getContainer line 15 25 v3
	BlockSource& bs = p->getRegion();
	BlockPos& bp = Fetch<BlockPos>(_this, 216);
	auto& bl = bs.getBlock(bp).getLegacyBlock();
	short bid = bl.getBlockItemId();
	if (bid == 54 || bid == 130 || bid == 146 || bid == -203 || bid == 205 || bid == 218) {	//非箱子、桶、潜影盒的情况不作处理
		uintptr_t v5 = (*reinterpret_cast<uintptr_t(**)(uintptr_t)>(Fetch<uintptr_t>(_this) + 160))(_this);
		if (v5) {
			ItemStack* item = (*reinterpret_cast<ItemStack * (**)(uintptr_t, uintptr_t)>(Fetch<uintptr_t>(v5) + 40))(v5, slot);
			h
				.insert("player", p)
				.insert("itemid", item->getId())
				.insert("itemaux", item->getAuxValue())
				.insert("itemcount", item->getCount())
				.insert("itemname", item->getName())
				.insert("blockname", bl.getBlockEntityType())
				.insert("blockid", bid)
				.insert("position", bp)
				.insert("slot", slot);
		}
	}
	original(_this, slot);
}
//玩家攻击
THook(bool, "?attack@Player@@UEAA_NAEAVActor@@AEBW4ActorDamageCause@@@Z",
	Player* p, Actor* a, ActorDamageCause* c) {
	Callbacker h(EventCode::onPlayerAttack);
	h
		.insert("player", p)
		.insert("actor", a);
	if (h.call())
		return original(p, a, c);
	return false;
}
//切换纬度
THook(bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z",
	uintptr_t _this, Player* p, uintptr_t req) {
	Callbacker h(EventCode::onChangeDimension);
	bool result = original(_this, p, req);
	if (result) {
		h.setArg(ToEntity(p)).call();
	}
	return result;
}
//生物死亡
THook(void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z",
	Mob* _this, uintptr_t dmsg) {
	Callbacker h(EventCode::onMobDie);
	char v72;
	Actor* sa = _this->getLevel().getEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 64))(dmsg, &v72)));
	h
		.insert("actor1", _this)
		.insert("actor2", sa)
		.insert("dmcase", Fetch<unsigned>(dmsg, 8))
		;
	if (h.call())
		original(_this, dmsg);
}
//生物受伤
THook(bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@H_N1@Z",
	Mob* _this, uintptr_t dmsg, int a3, bool a4, bool a5) {
	Callbacker h(EventCode::onMobHurt);
	g_damage = a3;//将生物受伤的值设置为可调整
	char v72;
	Actor* sa = _this->getLevel().getEntity(*(uintptr_t*)((*(uintptr_t(__fastcall**)(uintptr_t, char*))(*(uintptr_t*)dmsg + 64))(dmsg, &v72)));
	h
		.insert("actor1", _this)
		.insert("actor2", sa)
		.insert("dmcase", Fetch<unsigned>(dmsg, 8))
		.insert("damage", a3)
		;
	if (h.call())
		return original(_this, dmsg, g_damage, a4, a5);
	return false;
}
//玩家重生
THook(void, "?respawn@Player@@UEAAXXZ",
	Player* p) {
	Callbacker h(EventCode::onRespawn);
	h.setArg(ToEntity(p)).call();
	original(p);
}
//聊天，消息title msg w等...
THook(void, "?fireEventPlayerMessage@MinecraftEventing@@AEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@000@Z",
	uintptr_t _this, string& sender, string& target, string& msg, string& style) {
	Callbacker h(EventCode::onChat);
	h.insert("sender", sender)
		.insert("target", target)
		.insert("msg", msg)
		.insert("style", style);
	h.call();
	original(_this, sender, target, msg, style);
}
//玩家输入文本
THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	ServerNetworkHandler* _this, NetworkIdentifier& id, /*TextPacket*/uintptr_t pkt) {
	Callbacker h(EventCode::onInputText);
	Player* p = reinterpret_cast<Player*>(_this->getServerPlayer(id));
	if (p) {
		const string& msg = Fetch<string>(pkt, 88);
		h.insert("player", p)
			.insert("msg", msg);
		if (!h.call())
			return;
	}
	original(_this, id, pkt);
}
//玩家输入命令
THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	ServerNetworkHandler* _this, NetworkIdentifier& id, /*CommandRequestPacket*/uintptr_t pkt) {
	Callbacker h(EventCode::onInputCommand);
	Player* p = reinterpret_cast<Player*>(_this->getServerPlayer(id));
	if (p) {
		const string& cmd = Fetch<string>(pkt, 48);
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
//命令方块更新
THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
	ServerNetworkHandler* _this, NetworkIdentifier& id, /*CommandBlockUpdatePacket*/uintptr_t pkt) {
	Callbacker h(EventCode::onCommandBlockUpdate);
	Player* p = reinterpret_cast<Player*>(_this->getServerPlayer(id));
	if (p) {
		BlockPos& bp = Fetch<BlockPos>(pkt, 48);
		unsigned short mode = Fetch<unsigned short>(pkt, 60);
		bool condition = Fetch<bool>(pkt, 62);
		bool redstone = Fetch<bool>(pkt, 63);
		string cmd = Fetch<string>(pkt, 72);
		string output = Fetch<string>(pkt, 104);
		string rawname = Fetch<string>(pkt, 136);
		int delay = Fetch<int>(pkt, 168);
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
THook(bool, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* bs, Actor* a3, Vec3 pos, float a5, bool a6, bool a7, float a8, bool a9) {
	Callbacker h(EventCode::onLevelExplode);
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
THook(bool, "?_execute@CommandBlock@@AEBAXAEAVBlockSource@@AEAVCommandBlockActor@@AEBVBlockPos@@_N@Z",
	uintptr_t _this, BlockSource* a2, uintptr_t a3, BlockPos* bp, bool a5) {
	Callbacker h(EventCode::onCommandBlockPerform);
	//脉冲:0,重复:1,链:2
	int mode = SymCall<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z", a3, a2);
	//无条件:0,有条件:1
	bool condition = SymCall<bool>("?getConditionalMode@CommandBlockActor@@QEBA_NAEAVBlockSource@@@Z", a3, a2);
	//SymCall<string&>("?getName@BaseCommandBlock@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
	//	a3 + 200);
	//a3 + 200 BaseCommandBlock
	string cmd = Fetch<string>(a3, 256);
	string rawname = Fetch<string>(a3, 288);
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
THook(void, "??0MovePlayerPacket@@QEAA@AEAVPlayer@@W4PositionMode@1@HH@Z",
	uintptr_t _this, Player* p, char a3, int a4, int a5) {
	Callbacker h(EventCode::onMove);
	h.setArg(ToEntity(p)).call();
	return original(_this, p, a3, a4, a5);
}
//玩家穿戴
THook(void, "?setArmor@Player@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z",
	Player* p, unsigned slot, ItemStack* i) {
	Callbacker h(EventCode::onSetArmor);
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
THook(void, "?onScoreChanged@ServerScoreboard@@UEAAXAEBUScoreboardId@@AEBVObjective@@@Z",
	Scoreboard* _this, ScoreboardId& a1, Objective& a2) {
	//创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	//修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	Callbacker h(EventCode::onScoreChanged);
	h.insert("scoreboardid", a1.id)
		.insert("playersnum", a2.getPlayerScore(a1).getCount())
		.insert("objectivename", a2.getName())
		.insert("objectivedisname", a2.getDisplayName());
	original(_this, a1, a2);
}
//耕地破坏
THook(void, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z",
	uintptr_t _this, BlockSource* a1, BlockPos* a2, Actor* p, uintptr_t a4) {
	Callbacker h(EventCode::onFallBlockTransform);
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
THook(bool, "?trySetSpawn@RespawnAnchorBlock@@CA_NAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* p, BlockPos* a2, BlockSource* a3, Level* a4) {
	Callbacker h(EventCode::onUseRespawnAnchorBlock);
	h.insert("player", ToEntity(p))
		.insert("position", ToList(a2))
		.insert("dimensionid", a3->getDimensionId());
	if (h.call())
		return original(p, a2, a3, a4);
	return false;
}
//活塞推
THook(bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z",
	BlockActor* _this, BlockSource& bs, BlockPos& bp, unsigned a3, unsigned a4) {
	Callbacker h(EventCode::onPistonPush);
	auto& bl = bs.getBlock(bp).getLegacyBlock();
	string bn = bl.getRawNameId();
	short bid = bl.getBlockItemId();
	BlockPos bp2 = _this->getPosition();
	h.insert("blockname", bn)
		.insert("blockid", bid)
		.insert("blockpos", bp)
		.insert("pistonpos", bp2)
		.insert("dimensionid", bs.getDimensionId());
	if (h.call())
		return original(_this, bs, bp, a3, a4);
	return false;
}
//末影人随机传送（没人会用吧？）
THook(bool, "?randomTeleport@TeleportComponent@@QEAA_NAEAVActor@@@Z",
	uintptr_t _this, Actor* a1) {
	Callbacker h(EventCode::onEndermanRandomTeleport);
	h.setArg(ToEntity(a1)).call();
	if (h.call())
		return original(_this, a1);
	return false;
}
//丢物品
THook(bool, "?drop@Player@@UEAA_NAEBVItemStack@@_N@Z",
	Player* _this, ItemStack* a2, bool a3) {
	Callbacker h(EventCode::onDropItem);
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
THook(bool, "?take@Player@@QEAA_NAEAVActor@@HH@Z",
	Player* _this, Actor* actor, int a2, int a3) {
	Callbacker h(EventCode::onTakeItem);
	h.insert("player", _this)
		.insert("actor", actor);
	if (h.call())
		return original(_this, actor, a2, a3);
	return false;
}
//骑
THook(bool, "?canAddPassenger@Actor@@UEBA_NAEAV1@@Z",
	Actor* a1, Actor* a2) {
	Callbacker h(EventCode::onRide);
	h.insert("actor1", a1)
		.insert("actor2", a2);
	if (h.call())
		return original(a1, a2);
	return false;
}
//放入取出物品展示框的物品
THook(bool, "?use@ItemFrameBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	Callbacker h(EventCode::onUseFrameBlock);
	h.insert("player", a2)
		.insert("blockpos", a3)
		.insert("dimensionid", a2->getDimensionId());
	if (h.call())
		return original(_this, a2, a3);
	return false;
}
//点击物品展示框
THook(bool, "?attack@ItemFrameBlock@@UEBA_NPEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* a2, BlockPos* a3) {
	Callbacker h(EventCode::onUseFrameBlock);
	h.insert("player", a2)
		.insert("blockpos", a3)
		.insert("dimensionid", a2->getDimensionId());
	if (h.call())
		return original(_this, a2, a3);
	return false;
}
//玩家跳跃
THook(void, "?jumpFromGround@Player@@UEAAXXZ",
	Player* _this) {
	Callbacker h(EventCode::onJump);
	h.setArg(ToEntity(_this));
	if (h.call())
		return original(_this);
}
//玩家潜行
THook(void, "?sendActorSneakChanged@ActorEventCoordinator@@QEAAXAEAVActor@@_N@Z",
	uintptr_t _this, Actor* a1, bool a2) {
	Callbacker h(EventCode::onSneak);
	h.setArg(ToEntity(a1));
	if (h.call())
		return original(_this, a1, a2);
}
//火势蔓延
THook(bool, "?_trySpawnBlueFire@FireBlock@@AEBA_NAEAVBlockSource@@AEBVBlockPos@@@Z",
	uintptr_t _this, BlockSource& bs, BlockPos& bp) {
	Callbacker h(EventCode::onFireSpread);
	auto& bl = bs.getBlock(bp).getLegacyBlock();
	h
		.insert("blockpos", bp)
		.insert("blockname", bl.getRawNameId())
		.insert("blockid", bl.getBlockItemId())
		.insert("dimensionid", bs.getDimensionId());
	if (h.call())
		return original(_this, bs, bp);
	return false;
}
//方块交互（除箱子，工作台）
THook(void, "?onBlockInteractedWith@VanillaServerGameplayEventListener@@UEAA?AW4EventResult@@AEAVPlayer@@AEBVBlockPos@@@Z",
	uintptr_t _this, Player* pl, BlockPos& bp) {
	Callbacker h(EventCode::onBlockInteracted);
	BlockSource* bs = Level::getBlockSource(pl->getDimensionId());
	auto& bl = bs->getBlock(bp).getLegacyBlock();
	h.insert("player", pl)
		.insert("blockpos", bp)
		.insert("blockname", bl.getRawNameId())
		.insert("blockid", bl.getBlockItemId())
		.insert("dimensionid", bs->getDimensionId());
	if (h.call())
		return original(_this, pl, bp);
}
//方块被爆炸破坏
THook(void, "?onExploded@Block@@QEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@@Z",
	Block* _this, BlockSource& bs, BlockPos& bp, Actor* actor) {
	Callbacker h(EventCode::onBlockExploded);
	auto& bl = bs.getBlock(bp).getLegacyBlock();
	h.insert("actor", actor)
		.insert("blockpos", bp)
		.insert("blockname", bl.getRawNameId())
		.insert("blockid", bl.getBlockItemId())
		.insert("dimensionid", bs.getDimensionId());
	if (h.call())
		return original(_this, bs, bp, actor);
}
//使用牌子
THook(uintptr_t, "?use@SignBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	uintptr_t _this, Player* a1, BlockPos& a2) {
	Callbacker h(EventCode::onUseSignBlock);
	BlockSource& bs = a1->getRegion();
	BlockActor* ba = bs.getBlockEntity(a2);
	string text;
	//获取沉浸式文本内容
	SymCall<string&>("?getImmersiveReaderText@SignBlockActor@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEAVBlockSource@@@Z",
		ba, &text, &bs);
	h.insert("player", a1)
		.insert("text", text)
		.insert("position", a2);
	if (h.call())
		return original(_this, a1, a2);
	return 0;
}
#endif