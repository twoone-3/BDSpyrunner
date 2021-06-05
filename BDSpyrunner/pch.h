#pragma once
//#pragma warning(disable:4996)
#pragma execution_character_set("utf-8")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <filesystem>
#include <map>
#include <unordered_map>
#include "json.h"

using std::string;
using VA = unsigned long long;

#define FETCH(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYM(sym) GetServerSymbol(sym)
#define HOOK(name, ret, sym, ...)		\
struct name {							\
	typedef ret(*fn)(__VA_ARGS__);		\
	static ret _hook(__VA_ARGS__);		\
	static fn original;					\
};										\
name::fn name::original = *reinterpret_cast<name::fn*>(SymHook(sym, name::_hook, &name::original)); \
ret name::_hook(__VA_ARGS__)

extern "C" {
	_declspec(dllimport) int HookFunction(void*, void*, void*);
	_declspec(dllimport) void* GetServerSymbol(const char*);
}
/// <summary>
/// 打印数据
/// </summary>
/// <typeparam name="T">任意类型</typeparam>
/// <param name="data">要打印的数据</param>
template<class T>
static void inline print(const T& data) {
	cout << data << endl;
}
/// <summary>
/// 打印数据
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="...T2"></typeparam>
/// <param name="data">要打印的数据</param>
/// <param name="...other">下次打印的数据</param>
template<class T, class... T2>
static void inline print(const T& data, T2... other) {
	cout << data;
	print(other...);
}
/// <summary>
/// 调用一个函数
/// </summary>
/// <typeparam name="ret">返回类型</typeparam>
/// <typeparam name="...Args">参数类型</typeparam>
/// <param name="sym">符号</param>
/// <param name="...args">参数列表</param>
/// <returns>返回值</returns>
template<typename ret = void, typename... Args>
static ret SymCall(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)
		print("Failed to call ", sym);
	return reinterpret_cast<ret(*)(Args...)>(found)(args...);
}
static void* SymHook(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)
		print("Failed to hook ", sym);
	HookFunction(found, org, hook);
	return org;
}

#pragma region NBT
enum TagType : char {
	End, Byte, Short, Int, Int64, Float, Double,
	ByteArray, String, List, Compound, IntArray
};
struct TagMemoryChunk {
	size_t cap_ = 0;
	size_t size_ = 0;
	unique_ptr<unsigned char[]> data_;

	TagMemoryChunk(size_t size, unsigned char data[]) :cap_(size), size_(size), data_(data) {}
};
//所有Tag的合体
struct Tag {
	void* _vtable;
	char _this[24];

	void put(const string& key, const Tag* value) {
		return SymCall("?put@CompoundTag@@QEAAAEAVTag@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$$QEAV2@@Z",
			this, key, value);
	}
	void putByte(const string& key, unsigned char value) {
		return SymCall("?putByte@CompoundTag@@QEAAAEAEV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@E@Z",
			this, key, value);
	}
	void putShort(const string& key, short value) {
		return SymCall("?putShort@CompoundTag@@QEAAAEAFV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@F@Z",
			this, key, value);
	}
	void putString(const string& key, const string& value) {
		return SymCall("?putString@CompoundTag@@QEAAAEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V23@0@Z",
			this, key, value);
	}
	void putInt(const string& key, int value) {
		return SymCall("?putInt@CompoundTag@@QEAAAEAHV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z",
			this, key, value);
	}
	void putInt64(const string& key, long long value) {
		return SymCall("?putInt64@CompoundTag@@QEAAAEA_JV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_J@Z",
			this, key, value);
	}
	void putFloat(const string& key, float value) {
		return SymCall("?putFloat@CompoundTag@@QEAAAEAMV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@M@Z",
			this, key, value);
	}
	void putByteArray(const string& key, const TagMemoryChunk& value) {
		return SymCall("?putByteArray@CompoundTag@@QEAAAEAUTagMemoryChunk@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U2@@Z",
			this, key, &value);
	}
	void putCompound(const string& key, const Tag* value) {
		return SymCall("?putCompound@CompoundTag@@QEAAPEAV1@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@3@@Z",
			this, key, &value);
	}

	void add(Tag* t) {
		SymCall("?add@ListTag@@QEAAXV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@@Z",
			this, &t);
	}
	void deCompound() {
		SymCall("??1CompoundTag@@UEAA@XZ", this);
	}
	void deList() {
		SymCall("??1ListTag@@UEAA@XZ", this);
	}

	TagType getVariantType() {
		return *((TagType*)this + 40);
	}
	TagType getListType() {
		return *((TagType*)this + 32);
	}
	auto& asByte() { return *(unsigned char*)((VA)this + 8); }
	auto& asShort() { return *(short*)((VA)this + 8); }
	auto& asInt() { return *(int*)((VA)this + 8); }
	auto& asInt64() { return *(long long*)((VA)this + 8); }
	auto& asFloat() { return *(float*)((VA)this + 8); }
	auto& asDouble() { return *(double*)((VA)this + 8); }
	const char* asCString() { return *(const char**)((VA)this + 8); }
	auto& asString() { return *(string*)((VA)this + 8); }
	auto& asByteArray() { return *(TagMemoryChunk*)((VA)this + 8); }
	auto asListTag() { return (Tag*)this; }
	auto& asList() { return *(vector<Tag*>*)((VA)this + 8); }
	auto& asCompound() { return *(map<string, Tag>*)((VA)this + 8); }
};

Tag* newTag(TagType);
Document ListtoJson(Tag*);
Document toJson(Tag*);
Tag* toTag(const Value&);
Tag* ArraytoTag(const Value&);

//新建tag
Tag* newTag(TagType t) {
	Tag* tag;
	SymCall("?newTag@Tag@@SA?AV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@W4Type@1@@Z",
		&tag, t);
	return tag;
}
Document ListtoJson(Tag* t) {
	Document value(kArrayType);
	Document::AllocatorType& allocator = value.GetAllocator();
	for (auto& c : t->asList()) {
		switch (t->getListType()) {
		case End:
			break;
		case Byte:
			value.PushBack(Value(c->asByte()), allocator);
			break;
		case Short:
			value.PushBack(c->asShort(), allocator);
			break;
		case Int:
			value.PushBack(c->asInt(), allocator);
			break;
		case Int64:
			value.PushBack(c->asInt64(), allocator);
			break;
		case Float:
			value.PushBack(c->asFloat(), allocator);
			break;
		case Double:
			value.PushBack(c->asDouble(), allocator);
			break;
		case ByteArray:break;
		case String:
			value.PushBack("", allocator);
			break;
		case List:
			value.PushBack(ListtoJson(c).GetArray(), allocator);
			break;
		case Compound:
			value.PushBack(toJson(c).GetObj(), allocator);
			break;
		}
	}
	return move(value);
}
Document toJson(Tag* t) {
	Document value(kObjectType);
	Document::AllocatorType& allocator = value.GetAllocator();
	for (auto& x : t->asCompound()) {
		TagType type = x.second.getVariantType();
		Value& son = value[x.first + to_string(type)];
		switch (type) {
		case End:
			break;
		case Byte:
			son = x.second.asByte();
			break;
		case Short:
			son = x.second.asShort();
			break;
		case Int:
			son = x.second.asInt();
			break;
		case Int64:
			son = x.second.asInt64();
			break;
		case Float:
			son = x.second.asFloat();
			break;
		case Double:
			son = x.second.asDouble();
			break;
		case ByteArray:
			son.SetArray();
			for (size_t i = 0; i < x.second.asByteArray().size_; ++i)
				son.PushBack(x.second.asByteArray().data_[i], allocator);
			break;
		case String:
			//Value 不能直接用 std::string 赋值
			son.SetString(x.second.asString().c_str(),(SizeType)x.second.asString().length());
			break;
		case List:
			son = ListtoJson(&x.second).GetArray();
			break;
		case Compound:
			son = toJson(&x.second).GetObj();
			break;
		case IntArray:
			print("IntArrayTag");
			break;
		}
	}
	return move(value);
}
Tag* toTag(const Value& value) {
	Tag* c = newTag(Compound);
	for (auto& x : value.GetObj()) {
		string key = x.name.GetString();
		char& e = key.back();
		int type = 0;
		if (*(&e - 1) == '1' && e == '0') {
			type = 10;
			key.resize(key.length() - 2);
		}
		else if (e >= '0' && e <= '9') {
			type = e - '0';
			key.pop_back();
		}
		else continue;
		//cout << key << " - " << type << endl;
		switch (type) {
		case End:break;
		case Byte:
			c->putByte(key, (uint8_t)x.value.GetInt());
			break;
		case Short:
			c->putShort(key, (short)x.value.GetInt());
			break;
		case Int:
			c->putInt(key, x.value.GetInt());
			break;
		case Int64:
			c->putInt64(key, x.value.GetInt64());
			break;
		case Float:
			c->putFloat(key, x.value.GetFloat());
			break;
		case Double:
			c->putFloat(key, (float)x.value.GetDouble());
			break;
		case ByteArray:
		{
			SizeType size = x.value.Size();
			unsigned char* data = new unsigned char[size];
			for (unsigned i = 0; i < size; ++i)
				data[i] = x.value.GetInt();
			TagMemoryChunk tmc(size, data);
			c->putByteArray(key, tmc);
			break;
		}
		case String:
			c->putString(key, x.value.GetString());
			break;
		case List:
		{
			Tag* lt = ArraytoTag(x.value);
			c->put(key, lt);
			lt->deList();
			delete lt;
			break;
		}
		case Compound:
		{
			Tag* t = toTag(x.value);
			c->putCompound(key, t);
			//delete t;
			break;
		}
		}
	}
	return c;
}
Tag* ArraytoTag(const Value& value) {
	Tag* l = newTag(List);
	for (auto& x : value.GetArray()) {
		Type type = x.GetType();
		Tag* t = nullptr;
		switch (type) {
		case rapidjson::kNullType:
			break;
		case rapidjson::kFalseType:
			break;
		case rapidjson::kTrueType:
			break;
		case rapidjson::kObjectType:
			t = toTag(x);
			break;
		case rapidjson::kArrayType:
			t = ArraytoTag(x);
			break;
		case rapidjson::kStringType:
			t = newTag(String);
			*(string*)t->_this = x.GetString();
			break;
		case rapidjson::kNumberType:
			if (x.IsInt() || x.IsUint()) {
				t = newTag(Int);
				*(int*)t->_this = x.GetInt();
			}
			else if (x.IsFloat() || x.IsDouble()) {
				t = newTag(Double);
				*(double*)t->_this = x.GetDouble();
			}

			break;
		default:
			break;
		}
		l->add(t);
	}
	return l;
}
#pragma endregion
#pragma region Math
struct Vec3 {
	float x = 0.0f, y = 0.0f, z = 0.0f;
	string toString() {
		char str[64];
		sprintf_s(str, "(%f,%f,%f)", x, y, z);
		return str;
	}
};
struct Vec2 {
	float x = 0.0f, y = 0.0f;
	string toString() {
		char str[48];
		sprintf_s(str, "(%f,%f)", x, y);
		return str;
	}
};
struct BlockPos {
	int x = 0, y = 0, z = 0;

	string toString() {
		char str[32];
		sprintf_s(str, "(%d,%d,%d)", x, y, z);
		return str;
	}
};
#pragma endregion
#pragma region Block
struct BlockPalette {};
struct BlockLegacy {
	string getBlockName() {
		return FETCH(string, this + 128);
	}
	short getBlockItemID() {//IDA Item::beginCreativeGroup(,Block*,) 18~22
		short v3 = FETCH(short, this + 328);
		if (v3 < 0x100) {
			return v3;
		}
		return (short)(255 - v3);
	}
};
struct Block {
	BlockLegacy* getBlockLegacy() {
		return FETCH(BlockLegacy*, this + 16);
	}
};
struct BlockActor {
	Block* getBlock() {
		return FETCH(Block*, this + 16);
	}
	BlockPos* getPosition() {//IDA BlockActor::BlockActor 18~20
		return (BlockPos*)(this + 44);
	}
};
struct BlockSource {
	Block* getBlock(const BlockPos* bp) {
		return SymCall<Block*>("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z",
			this, bp);
	}
	bool setBlock(const string& name, const BlockPos& bp) {
		return SymCall<bool>("?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HPEBUActorBlockSyncMessage@@@Z",
			this, &bp, *(Block**)GetServerSymbol(("?m" + name + "@VanillaBlocks@@3PEBVBlock@@EB").c_str()), 3, nullptr);
	}
	void neighborChanged(const BlockPos* pos) {
		SymCall("?neighborChanged@BlockSource@@QEAAXAEBVBlockPos@@0@Z",
			this, pos, pos);
	}
	void updateNeighborsAt(const BlockPos* pos) {
		SymCall("?updateNeighborsAt@BlockSource@@QEAAXAEBVBlockPos@@@Z",
			this, pos);
	}
	int getDimensionId() {	//IDA Dimension::onBlockChanged 42
		return FETCH(int, FETCH(VA, this + 32) + 216);
	}
};
#pragma endregion
#pragma region Item
struct Item {};
struct ItemStack {
	VA vtable;
	Item* mItem;
	Tag* mUserData;
	Block* mBlock;
	short mAuxValue;
	char mCount;
	bool mValid;
	VA mPickupTime;
	bool mShowPickUp;
	vector<BlockLegacy*> mCanPlaceOn;
	VA mCanPlaceOnHash;
	vector<BlockLegacy*> mCanDestroy;
	VA mCanDestroyHash;
	VA mBlockingTick;
	ItemStack* mChargedItem;
	VA unk;

	//取物品ID,特殊值,损耗
	short getId() {
		return SymCall<short>("?getId@ItemStackBase@@QEBAFXZ", this);
	}
	short getDamageValue() {
		return SymCall<short>("?getDamageValue@ItemStackBase@@QEBAFXZ", this);
	}
	//取物品名称
	string getName() {
		string str;
		SymCall<string*>("?getRawNameId@ItemStackBase@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this, &str);
		return str;
	}
	//取容器内数量
	int getStackCount() {//IDA ContainerModel::networkUpdateItem
		return FETCH(int, this + 34);
	}
	//判断是否空容器
	bool isNull() {
		return SymCall<bool>("?isNull@ItemStackBase@@QEBA_NXZ", this);
	}
	bool isEmptyStack() {
		return FETCH(char, this + 34) == 0;
	}
	Tag* getNetworkUserData() {
		Tag* ct;
		SymCall("?getNetworkUserData@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			this, &ct);
		return ct;
	}
	Tag* save() {
		Tag* t = 0;
		SymCall("?save@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			this, &t);
		return t;
	}
	ItemStack* fromTag(Tag* t) {
		return SymCall<ItemStack*>("?fromTag@ItemStack@@SA?AV1@AEBVCompoundTag@@@Z",
			this, t);
	}
	bool getFromId(short id, short aux, char count) {
		memcpy(this, SYM("?EMPTY_ITEM@ItemStack@@2V1@B"), 0x90);
		bool ret = SymCall<bool>("?_setItem@ItemStackBase@@IEAA_NH@Z", this, id);
		mCount = count;
		mAuxValue = aux;
		mValid = true;
		return ret;
	}
	Item* getItem() {
		return SymCall<Item*>("?getItem@ItemStackBase@@QEBAPEBVItem@@XZ", this);
	}
	void fromJson(const Value& value) {
		Tag* t = toTag(value);
		fromTag(t);
		t->deCompound();
		delete t;
	}
};
static_assert(sizeof(ItemStack) == 0x90);
struct Container {
	//获取容器内所有物品
	vector<ItemStack*> getSlots() {
		vector<ItemStack*> s;
		SymCall<VA>("?getSlots@Container@@UEBA?BV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ",
			this, &s);
		return move(s);
	}
	void clearItem(int slot, int num) {
		SymCall("?removeItem@Container@@UEAAXHH@Z", this, slot, num);
	}
};
#pragma endregion
#pragma region Actor
struct Actor {
	//获取生物名称信息
	string getNameTag() {
		return SymCall<string&>("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this);
	}
	//获取生物当前所处维度ID
	int getDimensionId() {
		return FETCH(int, this + 236);//IDA Actor::getDimensionId
	}
	//获取生物当前所在坐标
	Vec3* getPos() {
		return (Vec3*)(this + 1220);//IDA Actor::getPos
	}
	//获取生物之前所在坐标
	Vec3* getPosOld() {
		return (Vec3*)(this + 1232);//IDA Actor::getPosOld
	}
	//是否已移除
	bool isRemoved() {
		return FETCH(bool, this + 7688);
	}
	//是否悬空
	bool isStand() {//IDA MovePlayerPacket::MovePlayerPacket 30
		return FETCH(bool, this + 448);
	}
	//取方块源
	BlockSource* getBlockSource() {
		return FETCH(BlockSource*, this + 840);
	}
	ItemStack* getArmor(int slot) {
		return SymCall<ItemStack*>("?getArmor@Actor@@UEBAAEBVItemStack@@W4ArmorSlot@@@Z",
			this, slot);
	}
	//获取实体类型
	unsigned getEntityTypeId() {
		return FETCH(unsigned, this + 968);
	}
	//获取查询用ID
	VA getUniqueID() {
		return SymCall<VA>("?getUniqueID@Actor@@QEBAAEBUActorUniqueID@@XZ", this);
	}
	//获取实体类型
	string getEntityTypeName() {
		string type;
		SymCall<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
			&type, getEntityTypeId());
		return type;
	}
	//更新属性
	VA updateAttrs() {
		return SymCall<VA>("?_sendDirtyActorData@Actor@@QEAAXXZ", this);
	}
	//获取地图信息
	struct Level* getLevel() {//IDA Mob::die 143
		return FETCH(Level*, this + 856);
	}
	VA getAttribute() {
		return FETCH(VA, this + 1144);
	}
	//添加一个状态
	VA addEffect(VA ef) {
		return SymCall<VA>("?addEffect@Actor@@QEAAXAEBVMobEffectInstance@@@Z", this, ef);
	}
	//获取生命值
	int getHealth() {
		return SymCall<int>("?getHealth@Actor@@QEBAHXZ", this);
	}
	int getMaxHealth() {
		return SymCall<int>("?getMaxHealth@Actor@@QEBAHXZ", this);
	}
	void setHealth(int value, int max) {
		VA hattr = ((*(VA(__fastcall**)(Actor*, void*))(*(VA*)this + 1552))(
			this, SYM("?HEALTH@SharedAttributes@@2VAttribute@@B")));
		FETCH(int, hattr + 132) = value;
		FETCH(int, hattr + 128) = max;
		//SymCall("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
	}
	//获取副手
	ItemStack* getOffHand() {
		return SymCall<ItemStack*>("?getOffhandSlot@Actor@@QEBAAEBVItemStack@@XZ", this);
	}
	Tag* save() {
		Tag* t = newTag(Compound);
		SymCall("?save@Actor@@UEAA_NAEAVCompoundTag@@@Z", this, t);
		return t;
	}
	//设置大小
	void setSize(float f1, float f2) {
		SymCall("?setSize@Actor@@UEAAXMM@Z", this, f1, f2);
	}
	//是否潜行
	bool isSneaking() {
		return SymCall<bool>("?isSneaking@Actor@@QEBA_NXZ", this);
	}
};
struct Mob : Actor {
	struct MobEffectInstance { char fill[0x1C]; };
	//获取状态列表
	auto getEffects() {	//IDA Mob::addAdditionalSaveData 84
		return (vector<MobEffectInstance>*)((VA*)this + 190);
	}
};
struct Player : Mob {
	string getUuid() {//IDA ServerNetworkHandler::_createNewPlayer 217
		string p;
		SymCall<string&>("?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this + 2832, &p);
		return p;
	}
	//发送数据包
	void sendPacket(VA pkt) {
		return SymCall<void>("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z",
			this, pkt);
	}
	//根据地图信息获取玩家xuid
	string& getXuid() {
		return SymCall<string&>("?getPlayerXUID@Level@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUUID@mce@@@Z",
			getLevel(), this + 2832);
	}
	//重设服务器玩家名
	void setName(const string& name) {
		SymCall("?setName@Player@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &name);
	}
	//获取网络标识符
	VA getNetId() {
		return (VA)this + 2544;//IDA ServerPlayer::setPermissions 34
	}
	//获取背包
	Container* getContainer() {
		return FETCH(Container*, FETCH(VA, this + 3040) + 176);
	}
	//获取装备容器
	Container* getArmorContainer() {
		return FETCH(Container*, this + 1512);
	}
	VA getContainerManager() {
		return (VA)this + 3024;	//IDA Player::setContainerManager 18
	}
	//获取末影箱
	Container* getEnderChestContainer() {
		return FETCH(Container*, this + 4176);//IDA ReplaceItemCommand::execute 1038
	}
	//设置一个装备
	VA setArmor(int i, ItemStack* item) {
		return SymCall<VA>("?setArmor@ServerPlayer@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z", this, i, item);
	}
	//设置副手
	VA setOffhandSlot(ItemStack* item) {
		return SymCall<VA>("?setOffhandSlot@Player@@UEAAXAEBVItemStack@@@Z", this, item);
	}
	//添加一个物品
	void addItem(ItemStack* item) {
		SymCall<VA>("?addItem@@YAXAEAVPlayer@@AEAVItemStack@@@Z", this, item);
	}
	//增加等级
	void addLevel(const int level) {
		SymCall("?addLevels@Player@@UEAAXH@Z", this, level);
	}
	//获取当前选中的框位置
	int getSelectedItemSlot() {//IDA Player::getSelectedItem 12
		return FETCH(unsigned, FETCH(VA, this + 3040) + 16);
	}
	//获取当前物品
	ItemStack* getSelectedItem() {
		return SymCall<ItemStack*>("?getSelectedItem@Player@@QEBAAEBVItemStack@@XZ", this);
	}
	//获取背包物品
	ItemStack* getInventoryItem(int slot) {
		return getContainer()->getSlots()[slot];
	}
	//获取游戏时命令权限
	char getPermissions() {//IDA ServerPlayer::setPermissions 17
		return FETCH(char, FETCH(char*, this + 2224));
	}
	//设置游戏时命令权限
	void setPermissions(char m) {
		SymCall("?setPermissions@ServerPlayer@@UEAAXW4CommandPermissionLevel@@@Z",
			this, m);
	}
	//获取游戏时游玩权限
	char getPermissionLevel() {//IDA Abilities::setPlayerPermissions ?
		return FETCH(char, FETCH(char*, this + 2224) + 1);
	}
	//设置游戏时游玩权限
	void setPermissionLevel(char m) {
		SymCall("?setPlayerPermissions@Abilities@@QEAAXW4PlayerPermissionLevel@@@Z",
			this + 2224, m);
	}
	//发送背包
	void sendInventroy() {
		SymCall("?sendInventory@ServerPlayer@@UEAAX_N@Z",
			this, true);
	}
	//刷新区块
	void resendAllChunks() {
		SymCall("?resendAllChunks@ServerPlayer@@UEAAXXZ", this);
	}
	//断开连接
	//void disconnect() {
	//	SymCall("?disconnect@ServerPlayer@@QEAAXXZ",this);
	//}
	//传送
	void teleport(Vec3* target, int did) {
		char mem[128];
		SymCall("?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z",
			&mem, this, target, 0, did, 0, 0, 15);
		SymCall("?applyTarget@TeleportCommand@@SAXAEAVActor@@VTeleportTarget@@@Z",
			this, &mem);
	}
};
#pragma endregion
#pragma region ScoreBoard
struct PlayerScore {
	VA getscore() {
		return FETCH(VA, this + 4);
	}
};
struct ScoreInfo {
	//scoreboardcmd list; objective::objective; scoreboard getscores
	//int scores    +12 this[12]
	//string displayname  +96
	//string name +64
	int getCount() {
		return FETCH(int, this + 12);
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
		sid(*s), score(num), fake_name(fake) {
	}

};
struct Objective {
	//获取计分板名称
	auto getScoreName() {
		return FETCH(string, this + 64);
	}
	//获取计分板展示名称
	auto getScoreDisplayName() {
		return FETCH(string, this + 96);
	}
	auto createScoreboardId(Player* player) {
		return SymCall<ScoreboardId*>("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBVPlayer@@@Z", this, player);
	}
	ScoreInfo* getPlayerScore(ScoreboardId* a2) {
		char a1[12];
		return SymCall<ScoreInfo*>("?getPlayerScore@Objective@@QEBA?AUScoreInfo@@AEBUScoreboardId@@@Z",
			this, a1, a2);
	}
};
struct Scoreboard {
	auto getObjective(string str) {
		return SymCall<Objective*>("?getObjective@Scoreboard@@QEBAPEAVObjective@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z", this, &str);
	}
	auto getScoreboardId(const string& str) {
		return SymCall<ScoreboardId*>("?getScoreboardId@Scoreboard@@QEBAAEBUScoreboardId@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &str);
	}
	vector<Objective*> getObjectives() {
		vector<Objective*> s;
		SymCall("?getObjectives@Scoreboard@@QEBA?AV?$vector@PEBVObjective@@V?$allocator@PEBVObjective@@@std@@@std@@XZ",
			this, &s);
		return s;
	}
	auto getDisplayInfoFiltered(string* str) {
		return SymCall<vector<PlayerScore>*>("?getDisplayInfoFiltered@Scoreboard@@QEBA?AV?$vector@UPlayerScore@@V?$allocator@UPlayerScore@@@std@@@std@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@@Z", this, str);
	}
	auto getTrackedIds() {
		return SymCall<vector<ScoreboardId>*>("?getTrackedIds@Scoreboard@@QEBA?AV?$vector@UScoreboardId@@V?$allocator@UScoreboardId@@@std@@@std@@XZ", this);
	}
	auto getScoreboardId(Player* a2) {
		return SymCall<ScoreboardId*>("?getScoreboardId@Scoreboard@@QEBAAEBUScoreboardId@@AEBVActor@@@Z", this, a2);
	}
	//更改玩家分数
	int modifyPlayerScore(ScoreboardId* a3, Objective* a4, int count, int mode) {
		bool a2 = true;
		return SymCall<int>("?modifyPlayerScore@Scoreboard@@QEAAHAEA_NAEBUScoreboardId@@AEAVObjective@@HW4PlayerScoreSetFunction@@@Z",
			this, &a2, a3, a4, count, mode);
	}
	auto createScoreBoardId(const string& s) {
		return SymCall<ScoreboardId*>("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &s);
	}
	auto createScoreBoardId(Player* player) {
		return SymCall<ScoreboardId*>("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBVPlayer@@@Z", this, player);
	}
};
#pragma endregion
#pragma region Level
struct Level {
	//获取方块源 没这个维度返回空指针
	BlockSource* getBlockSource(int did) {
		VA d = SymCall<VA>("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
			this, did);
		if (!d)return 0;
		return FETCH(BlockSource*, d + 96);//IDA Level::tickEntities 120
	}
	Scoreboard* getScoreBoard() {//IDA Level::removeEntityReferences
		return FETCH(Scoreboard*, this + 8464);
	}
	unsigned getSeed() {
		return SymCall<unsigned>("?getSeed@Level@@UEAAIXZ", this);
	}
	string getPlayerNames() {
		string s;
		SymCall<string&>("?getPlayerNames@Level@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this, &s);
		return s;
	}
	Actor* fetchEntity(VA id) {
		return SymCall<Actor*>("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z",
			this, id, false);
	}
	const vector<Player*>& getAllPlayers() {
		return FETCH(vector<Player*>, this + 112);
	}
	BlockPalette* getBlockPalette() {
		return FETCH(BlockPalette*, this + 2072);
	}
};
struct ServerNetworkHandler {
	Player* _getServerPlayer(VA id, VA pkt) {
		return SymCall<Player*>("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
			this, id, FETCH(char, pkt + 16));
	}
};
#pragma endregion
#pragma region Structure
struct string_span {
	size_t len;
	const char* str;
	string_span(const char* s) : len(strlen(s)), str(s) {}
	string_span(const std::string& s) : len(s.length()), str(s.c_str()) {}
};
struct StructureSettings {
	char _this[96];
	StructureSettings(BlockPos* size, bool IgnoreEntities, bool IgnoreBlocks) {
		SymCall("??0StructureSettings@@QEAA@XZ", this);
		FETCH(bool, _this + 32) = IgnoreEntities;
		FETCH(bool, _this + 34) = IgnoreBlocks;
		FETCH(BlockPos, _this + 36) = *size;
		FETCH(BlockPos, _this + 48) = { 0,0,0 };
	}
	~StructureSettings() {
		((string*)this)->~basic_string();
	}
};
#if 0
struct StructureDataLoadHelper {
	char _this[136];

	StructureDataLoadHelper(BlockPos* bp1, BlockPos* bp2, Vec3* v, VA id, char Rotation, char Mirror, Level* level) {
		SymCall("??0StructureDataLoadHelper@@QEAA@AEBVBlockPos@@0AEBVVec3@@UActorUniqueID@@W4Rotation@@W4Mirror@@AEAVLevel@@@Z",
			this, bp1, bp2, v, id, Rotation, Mirror, level);
	}
	~StructureDataLoadHelper() {
		SymCall("??1StructureDataLoadHelper@@UEAA@XZ", this);
	}
};
#endif
struct StructureTemplate {
	char _this[216];
	StructureTemplate(const string_span& s) {
		SymCall("??0StructureTemplate@@QEAA@V?$basic_string_span@$$CBD$0?0@gsl@@@Z",
			this, s);
	}
	~StructureTemplate() {
		SymCall("??1StructureTemplate@@QEAA@XZ", this);
	}
	Tag* save() {
		Tag* t = 0;
		SymCall<Tag*>("?save@StructureTemplateData@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			_this + 32, &t);
		return t;
	}
	void load(Tag* t) {
		SymCall<bool>("?load@StructureTemplateData@@QEAA_NAEBVCompoundTag@@@Z",
			_this + 32, t);
	}
	void fromJson(const Value& value) {
		Tag* t = toTag(value);
		load(t);
		t->deCompound();
		delete t;
	}
	void fillFromWorld(BlockSource* a2, BlockPos* a3, StructureSettings* a4) {
		SymCall("?fillFromWorld@StructureTemplate@@QEAAXAEAVBlockSource@@AEBVBlockPos@@AEBVStructureSettings@@@Z",
			this, a2, a3, a4);
	}
	void placeInWorld(BlockSource* a2, BlockPalette* a3, BlockPos* a4, StructureSettings* a5) {
		SymCall("?placeInWorld@StructureTemplate@@QEBAXAEAVBlockSource@@AEBVBlockPalette@@AEBVBlockPos@@AEBVStructureSettings@@PEAVStructureTelemetryServerData@@_N@Z",
			this, a2, a3, a4, a5);
	}
};
#pragma endregion
