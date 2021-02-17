#pragma once
#include "pch.h"
#include "NBT.hpp"
using namespace std;
struct BlockLegacy {
	string getBlockName() {
		return f(string, this + 128);
	}
	short getBlockItemID() {// IDA Item::beginCreativeGroup(,Block*,) 18~22
		short v3 = f(short, this + 328);
		if (v3 < 0x100) {
			return v3;
		}
		return (short)(255 - v3);
	}
};
struct Block {
	BlockLegacy* getBlockLegacy() {
		return SYMCALL<BlockLegacy*>(MSSYM_B1QE14getLegacyBlockB1AA5BlockB2AAE19QEBAAEBVBlockLegacyB2AAA2XZ, this);
	}
};
struct BlockPos { int x = 0, y = 0, z = 0; };
struct BlockActor {
	Block* getBlock() {
		return f(Block*, this + 16);
	}
	// 取方块位置
	BlockPos* getPosition() {// IDA BlockActor::BlockActor 18~20
		return f(BlockPos*, this + 44);
	}
};
struct BlockSource {
	Block* getBlock(BlockPos* bp) {
		return SYMCALL<Block*>(MSSYM_B1QA8getBlockB1AE11BlockSourceB2AAE13QEBAAEBVBlockB2AAE12AEBVBlockPosB3AAAA1Z,
			this, bp);
	}
	// 获取方块所处维度
	int getDimensionId() {	// IDA Dimension::onBlockChanged 42
		return f(int, (f(VA, this + 32) + 208));
	}
};
struct string_span {
	size_t len;
	const char* str;
	string_span(const char* s) : len(strlen(s)), str(s) {}
	string_span(const string& s) : len(s.length()), str(s.c_str()) {}
};
struct StructureManager {

};
struct StructureSettings {
	char fill[96];
	StructureSettings() {
		SYMCALL(MSSYM_B2QQE180StructureSettingsB2AAA4QEAAB1AA2XZ, this);
	}
};
struct StructureTemplate {
	char fill[216];
	StructureTemplate(const string_span& s) {
		SYMCALL(MSSYM_B2QQE180StructureTemplateB2AAA4QEAAB1AA1VB2QDA5basicB1UA6stringB1UA4spanB3ADDA3CBDB1DA10B1QA10B1AA3gslB3AAAA1Z,
			this, s);
	}
	Tag* save() {
		Tag* t = 0;
		SYMCALL<Tag*>(MSSYM_MD5_9f92b0974a4b58b3702084a5ac57f1e6,
			fill + 32, &t);
		return t;
	}
	void fillFromWorld(BlockSource* a2, BlockPos* a3, StructureSettings* a4) {
		SYMCALL(MSSYM_B1QE13fillFromWorldB1AE17StructureTemplateB2AAE20QEAAXAEAVBlockSourceB2AAE12AEBVBlockPosB2AAE21AEBVStructureSettingsB3AAAA1Z,
			this, a2, a3, a4);
	}
};
struct Level {
	StructureManager* getStructureManager() {
		return f(StructureManager*, this + 8160);
	}
	VA getScoreBoard() {// IDA Level::removeEntityReferences
		return f(VA, this + 8376);
	}
	struct Actor* fetchEntity(VA id) {
		return SYMCALL<struct Actor*>(MSSYM_B1QE11fetchEntityB1AA5LevelB2AAE13QEBAPEAVActorB2AAE14UActorUniqueIDB3AAUA1NB1AA1Z,
			this, id, false);
	}
	auto getPlayers() {
		return f(vector<struct Player*>, this + 104);
	}
	void forEachPlayer(const function<bool(struct Player*)>& todo) {
		SYMCALL(MSSYM_B1QE13forEachPlayerB1AA5LevelB2AAA6QEBAXVB2QDA8functionB3ADDA3A6AB1UE11NAEBVPlayerB3AAAA1ZB1AA3stdB3AAAA1Z,
			this, &todo);
	}
	void forEachPlayer(const function<void(Actor*)>& todo) {
		auto begin = (uint64_t*)*((uint64_t*)this + 13);
		auto end = (uint64_t*)*((uint64_t*)this + 14);
		while (begin != end) {
			auto* player = (Actor*)(*begin);
			if (player)todo(player);
			++begin;
		}
	}
};
struct Vec3 { float x = 0.0f, y = 0.0f, z = 0.0f; };
struct Vec2 { float x = 0.0f, y = 0.0f; };
struct MobEffectInstance { char fill[0x1C]; };
struct Item;
struct ItemStackBase {
	VA vtable;
	VA mItem;
	VA mUserData;
	VA mBlock;
	short mAuxValue;
	char mCount;
	char mValid;
	char unk[4];
	VA mPickupTime;
	char mShowPickUp;
	char unk2[7];
	vector<VA*> mCanPlaceOn;
	VA mCanPlaceOnHash;
	vector<VA*> mCanDestroy;
	VA mCanDestroyHash;
	VA mBlockingTick;
	ItemStackBase* mChargedItem;
	VA uk;

	// 取物品ID,特殊值,损耗
	short getId() {
		return SYMCALL<short>(MSSYM_B1QA5getIdB1AE13ItemStackBaseB2AAA7QEBAFXZ, this);
	}
	short getDamageValue() {
		return SYMCALL<short>(MSSYM_B1QE14getDamageValueB1AE13ItemStackBaseB2AAA7QEBAFXZ, this);
	}
	// 取物品名称
	string getName() {
		string str;
		SYMCALL<string*>(MSSYM_MD5_2f9d68ca736b0da0c26f063f568898bc,
			this, &str);
		return str;
	}
	// 取容器内数量
	int getStackCount() {// IDA ContainerModel::networkUpdateItem
		return f(int, this + 34);
	}
	// 判断是否空容器
	bool isNull() {
		return SYMCALL<bool>(MSSYM_B1QA6isNullB1AE13ItemStackBaseB2AAA4QEBAB1UA3NXZ, this);
	}
	Tag* getNetworkUserData() {
		Tag* ct;
		SYMCALL(MSSYM_MD5_f5043f88ed613abbdd6903d706ad5ee3,
			this, &ct);
		return ct;
	}
	Tag* save() {
		Tag* t = 0;
		SYMCALL(MSSYM_MD5_e02d5851c93a43bfe24a4396ecb87cde,
			this, &t);
		return t;
	}
	bool isEmptyStack() {
		return f(char, this + 34) == 0;
	}
	ItemStackBase* fromTag(Tag* t) {
		return SYMCALL<ItemStackBase*>(MSSYM_B1QA7fromTagB1AA9ItemStackB2AAA2SAB1QA3AV1B1AE15AEBVCompoundTagB3AAAA1Z,
			this, t);
	}
	bool getFromId(short id, short aux, char count) {
		memcpy(this, SYM(MSSYM_B1QA5EMPTYB1UA4ITEMB1AA9ItemStackB2AAA32V1B1AA1B), 0x90);
		bool ret = SYMCALL<bool>(MSSYM_B2QUA7setItemB1AE13ItemStackBaseB2AAA4IEAAB1UA2NHB1UA1NB1AA1Z, this, id);
		mCount = count;
		mAuxValue = aux;
		mValid = true;
		return ret;
	}
	Item* getItem() {
		return SYMCALL<Item*>(MSSYM_B1QA7getItemB1AE13ItemStackBaseB2AAE12QEBAPEBVItemB2AAA2XZ, this);
	}
};
struct ItemStack : ItemStackBase {};
struct Container {
	// 获取容器内所有物品
	auto getSlots() {
		vector<ItemStack*> s;
		SYMCALL<VA>(MSSYM_B1QA8getSlotsB1AA9ContainerB2AAA4UEBAB1QA2BVB2QDA6vectorB1AE13PEBVItemStackB2AAA1VB2QDA9allocatorB1AE13PEBVItemStackB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, &s);
		return s;
	}
	void clearItem(int slot, int num) {
		SYMCALL(MSSYM_B1QE10removeItemB1AA9ContainerB2AAA7UEAAXHHB1AA1Z, this, slot, num);
	}
};
struct Actor {
	// 获取生物名称信息
	string getNameTag() {
		return SYMCALL<string&>(MSSYM_MD5_7044ab83168b0fd345329e6566fd47fd, this);
	}
	// 获取生物当前所处维度ID
	int getDimensionId() {
		int did;
		SYMCALL<int&>(MSSYM_B1QE14getDimensionIdB1AA5ActorB2AAA4UEBAB1QA2AVB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB2AAA2XZ,
			this, &did);
		return did;
	}
	// 获取生物当前所在坐标
	Vec3* getPos() {
		return SYMCALL<Vec3*>(MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
	}
	// 是否悬空
	bool isStand() {// IDA MovePlayerPacket::MovePlayerPacket 30
		return f(bool, this + 448);
	}
	// 取方块源
	BlockSource* getRegion() {
		return f(BlockSource*, this + 3312);
	}
	// 获取实体类型
	unsigned getEntityTypeId() {
		return f(unsigned, this + 964);
	}
	// 获取查询用ID
	VA getUniqueID() {
		return SYMCALL<VA>(MSSYM_B1QE11getUniqueIDB1AA5ActorB2AAE21QEBAAEBUActorUniqueIDB2AAA2XZ, this);
	}
	// 获取实体类型
	string getEntityTypeName() {
		string en_name;
		SYMCALL<string&>(MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68,
			&en_name, getEntityTypeId());
		return en_name;
	}
	// 更新属性
	VA updateAttrs() {
		return SYMCALL<VA>(MSSYM_B2QUE18sendDirtyActorDataB1AA5ActorB2AAA7QEAAXXZ, this);
	}
	VA getAttribute() {
		return f(VA, this + 1144);
	}
	ItemStack* getArmor(unsigned slot) {
		return SYMCALL<ItemStack*>(MSSYM_B1QA8getArmorB1AA5ActorB2AAE17UEBAAEBVItemStackB2AAE11W4ArmorSlotB3AAAA1Z,
			this, slot);
	}
	// 添加一个状态
	VA addEffect(VA ef) {
		return SYMCALL<VA>(MSSYM_B1QA9addEffectB1AA5ActorB2AAE26QEAAXAEBVMobEffectInstanceB3AAAA1Z, this, ef);
	}
	// 获取生命值
	int getHealth() {
		return SYMCALL<int>(MSSYM_B1QA9getHealthB1AA5ActorB2AAA7QEBAHXZ, this);
	}
	int getMaxHealth() {
		return SYMCALL<int>(MSSYM_B1QE12getMaxHealthB1AA5ActorB2AAA7QEBAHXZ, this);
	}
	void setHealth(int value, int max) {
		VA hattr = ((*(VA(__fastcall**)(Actor*, void*))(*(VA*)this + 1552))(
			this, SYM(MSSYM_B1QA6HEALTHB1AE16SharedAttributesB2AAE112VAttributeB2AAA1B)));
		f(int, hattr + 132) = value;
		f(int, hattr + 128) = max;
		//SYMCALL("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
	}
	Tag* save() {
		Tag* t = newTag(Compound);
		SYMCALL(MSSYM_B1QA4saveB1AA5ActorB2AAA4UEAAB1UE16NAEAVCompoundTagB3AAAA1Z, this, t);
		return t;
	}
};
struct Mob : Actor {
	// 获取状态列表
	auto getEffects() {	// IDA Mob::addAdditionalSaveData 84
		return (vector<MobEffectInstance>*)((VA*)this + 190);
	}
	VA getLevel() {// IDA Mob::die 143
		return f(VA, this + 856);
	}
};
struct Player : Mob {
	string getUuid() {// IDA ServerNetworkHandler::_createNewPlayer 217
		string p;
		SYMCALL<string&>(MSSYM_MD5_40e8abf6eb08f7ee446159cdd0a7f283,
			this + 2824, &p);
		return p;
	}
	// 发送数据包
	void sendPacket(VA pkt) {
		return SYMCALL<void>(MSSYM_B1QE17sendNetworkPacketB1AE12ServerPlayerB2AAE15UEBAXAEAVPacketB3AAAA1Z,
			this, pkt);
	}
	// 根据地图信息获取玩家xuid
	string& getXuid() {
		return SYMCALL<string&>(MSSYM_MD5_337bfad553c289ba4656ac43dcb60748,
			getLevel(), this + 2824);
	}
	// 重设服务器玩家名
	void setName(string name) {
		SYMCALL(MSSYM_B1QA7setNameB1AA6PlayerB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
			this, name);
	}
	// 获取网络标识符
	VA getNetId() {
		return (VA)this + 2536;// IDA ServerPlayer::setPermissions 34
	}
	// 获取背包
	Container* getContainer() {
		return (Container*)f(VA, f(VA, this + 3048) + 176);
	}
	VA getContainerManager() {
		return (VA)this + 3040;		// IDA Player::setContainerManager 18
	}
	// 获取末影箱
	Container* getEnderChestContainer() {
		return SYMCALL<Container*>(MSSYM_B1QE22getEnderChestContainerB1AA6PlayerB2AAE27QEAAPEAVEnderChestContainerB2AAA2XZ, this);
	}
	// 设置一个装备
	void setArmor(unsigned i, ItemStack* item) {
		SYMCALL(MSSYM_B1QA8setArmorB1AE12ServerPlayerB2AAE16UEAAXW4ArmorSlotB2AAE13AEBVItemStackB3AAAA1Z,
			this, i, item);
	}
	// 设置副手
	void setOffhandSlot(ItemStack* item) {
		return SYMCALL(MSSYM_B1QE14setOffhandSlotB1AA6PlayerB2AAE18UEAAXAEBVItemStackB3AAAA1Z,
			this, item);
	}
	// 添加一个物品
	void addItem(ItemStackBase* item) {
		SYMCALL<VA>(MSSYM_B1QA7addItemB2AAE13YAXAEAVPlayerB2AAE13AEAVItemStackB3AAAA1Z, this, item);
	}
	// 获取当前选中的框位置
	int getSelectdItemSlot() {// IDA Player::getSelectedItem 12
		return f(unsigned, f(VA, this + 3048) + 16);
	}
	// 获取当前物品
	ItemStack* getSelectedItem() {
		return SYMCALL<ItemStack*>(MSSYM_B1QE15getSelectedItemB1AA6PlayerB2AAE17QEBAAEBVItemStackB2AAA2XZ, this);
	}
	// 获取背包物品
	ItemStack* getInventoryItem(int slot) {
		return SYMCALL<ItemStack*>(MSSYM_B1QA7getItemB1AE16FillingContainerB2AAE17UEBAAEBVItemStackB2AAA1HB1AA1Z, *(__int64**)(*((__int64*)this + 0x17D) + 0xB0), slot);
	}
	// 获取游戏时命令权限
	char getPermission() {// IDA ServerPlayer::setPermissions 17
		return *f(char*, this + 2216);
	}
	// 设置游戏时命令权限
	void setPermission(char m) {
		SYMCALL(MSSYM_B1QE14setPermissionsB1AE12ServerPlayerB2AAE29UEAAXW4CommandPermissionLevelB3AAAA1Z,
			this, m);
	}
	// 获取游戏时游玩权限
	char getPermissionLevel() {// IDA Abilities::setPlayerPermissions ?
		return f(char, f(char*, this + 2192) + 1);
	}
	// 设置游戏时游玩权限
	void setPermissionLevel(char m) {
		SYMCALL(MSSYM_B1QE20setPlayerPermissionsB1AA9AbilitiesB2AAE28QEAAXW4PlayerPermissionLevelB3AAAA1Z,
			this + 2192, m);
	}
	void sendInventroy() {
		SYMCALL(MSSYM_B1QE13sendInventoryB1AE12ServerPlayerB2AAA5UEAAXB1UA1NB1AA1Z,
			this, true);
	}
	void teleport(Vec3 target, int dim) {
		SYMCALL(MSSYM_MD5_6a6046fa3188aa75fca8643d7e51ceae,
			this, target, 0, dim, 0, 0, 0, SYM(MSSYM_B1QA7INVALIDB1UA2IDB1AE13ActorUniqueIDB2AAA32U1B1AA1B));
	}
};
struct PlayerScore {
	VA getscore() {
		return f(VA, this + 4);
	}
};
struct ScoreInfo {
	//scoreboardcmd list; objective::objective; scoreboard getscores
	//int scores    +12 this[12]
	// string displayname  +96
	//string name +64
	int getCount() {
		return f(int, this + 12);
	}
};
struct ScoreboardId {
	int id;
	VA null;
};
struct ScorePacketInfo {
	ScoreboardId sid;
	string obj_name = "name";
	unsigned score;
	enum Type : char { Invalid = 0, Player = 1, Actor = 2, Fake = 3 };
	Type type = Fake;
	VA pid;
	VA aid;
	string fake_name;

	ScorePacketInfo(ScoreboardId* s, unsigned num, const string& fake) :
		sid(*s), score(num), fake_name(fake) {}

};
struct Objective {
	//获取计分板名称
	auto getScoreName() {
		return f(string, this + 64);
	}
	//获取计分板展示名称
	auto getScoreDisplayName() {
		return f(string, this + 96);
	}
	auto createScoreboardId(Player* player) {
		return SYMCALL<ScoreboardId*>(MSSYM_B1QE18createScoreboardIdB1AE16ServerScoreboardB2AAE20UEAAAEBUScoreboardIdB2AAE10AEBVPlayerB3AAAA1Z, this, player);
	}
	ScoreInfo* getPlayerScore(ScoreboardId* a2) {
		char a1[12];
		return SYMCALL<ScoreInfo*>(MSSYM_B1QE14getPlayerScoreB1AA9ObjectiveB2AAA4QEBAB1QE11AUScoreInfoB2AAE16AEBUScoreboardIdB3AAAA1Z,
			this, a1, a2);
	}
};
struct Scoreboard {
	auto getObjective(string str) {
		return SYMCALL<Objective*>(MSSYM_MD5_844f126769868c7d0ef42725c3859954, this, &str);
	}
	auto getScoreboardId(const string& str) {
		return SYMCALL<ScoreboardId*>(MSSYM_MD5_ecded9d31b4a1c24ba985b0a377bef64,
			this, &str);
	}
	vector<Objective*> getObjectives() {
		vector<Objective*> s;
		SYMCALL(MSSYM_B1QE13getObjectivesB1AE10ScoreboardB2AAA4QEBAB1QA2AVB2QDA6vectorB1AE13PEBVObjectiveB2AAA1VB2QDA9allocatorB1AE13PEBVObjectiveB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, &s);
		return s;
	}
	auto getDisplayInfoFiltered(string* str) {
		return SYMCALL<vector<PlayerScore>*>(MSSYM_MD5_3b3c17fbee13a54836ae12d93bb0dbae, this, str);
	}
	auto getTrackedIds() {
		return SYMCALL<vector<ScoreboardId>*>(MSSYM_B1QE13getTrackedIdsB1AE10ScoreboardB2AAA4QEBAB1QA2AVB2QDA6vectorB1AE13UScoreboardIdB2AAA1VB2QDA9allocatorB1AE13UScoreboardIdB3AAAA3stdB3AAAA3stdB2AAA2XZ, this);
	}
	auto getScoreboardId(Player* a2) {
		return SYMCALL<ScoreboardId*>(MSSYM_B1QE15getScoreboardIdB1AE10ScoreboardB2AAE20QEBAAEBUScoreboardIdB2AAA9AEBVActorB3AAAA1Z, this, a2);
	}
	//更改玩家分数
	int modifyPlayerScore(ScoreboardId* a3, Objective* a4, int count, int mode) {
		bool a2 = true;
		return SYMCALL<int>(MSSYM_B1QE17modifyPlayerScoreB1AE10ScoreboardB2AAA8QEAAHAEAB1UE17NAEBUScoreboardIdB2AAE13AEAVObjectiveB2AAE25HW4PlayerScoreSetFunctionB3AAAA1Z,
			this, &a2, a3, a4, count, mode);
	}
	auto createScoreBoardId(const string& s) {
		return SYMCALL<ScoreboardId*>(MSSYM_MD5_853292b07f4fef37d30365afb122e197,
			this, &s);
	}
	auto createScoreBoardId(Player* player) {
		return SYMCALL<ScoreboardId*>(MSSYM_B1QE18createScoreboardIdB1AE16ServerScoreboardB2AAE20UEAAAEBUScoreboardIdB2AAE10AEBVPlayerB3AAAA1Z, this, player);
	}
};
struct MinecraftCommands {
	void* myVTBL;
	void* UUID[2];
	Level* lvl;
	string Name;
	char Perm;
	static void* fake_vtbl[26];
	static void dummy() {
	};
	MinecraftCommands(Level* l) {
		if (fake_vtbl[1] == nullptr) {
			memcpy(fake_vtbl, (void**)(SYM(MSSYM_B3QQUE207ServerCommandOriginB2AAA26BB1A)) - 1, sizeof(fake_vtbl));
			fake_vtbl[1] = (void*)dummy;
		}
		myVTBL = fake_vtbl + 1;
		Name = "Server";
		Perm = 5;
		lvl = l;
	}
};
void* MinecraftCommands::fake_vtbl[26];
