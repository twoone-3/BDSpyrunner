#pragma once
//#pragma warning(disable:4996)
#pragma execution_character_set("utf-8")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include "json.hpp"

#pragma region using
using std::string;
using std::wstring;
using std::string_view;
using std::vector;
using std::pair;
using std::map;
using std::unordered_map;
using std::function;
using std::unique_ptr;
using std::exception;
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using nlohmann::json;
using JsonType = json::value_t;
using VA = unsigned long long;
#pragma endregion
#pragma region utils
#define INFO(str) cout << "[BDSpyrunner] " << str << endl
#define ERR(str) cerr << "[BDSpyrunner] " << str << endl
#define FETCH(type, ptr) (*reinterpret_cast<type*>(ptr))
#define SYM GetServerSymbol
//制作一个钩子
#define HOOK(name, ret, sym, ...)		\
struct name {							\
	using func = ret(__VA_ARGS__);		\
	static func _hook;					\
	static func* original;				\
};										\
name::func* name::original = *reinterpret_cast<name::func**>(SymHook(sym, name::_hook, &name::original)); \
ret name::_hook(__VA_ARGS__)
//提供Detours
extern "C" _declspec(dllimport)
int HookFunction(void*, void*, void*);
//获取符号
extern "C" _declspec(dllimport)
void* GetServerSymbol(const char*);
//调用一个函数
template<typename ret = void, typename... Args>
static ret SymCall(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)
		ERR("Failed to call " << sym);
	return reinterpret_cast<ret(*)(Args...)>(found)(args...);
}
static void* SymHook(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)
		ERR("Failed to hook " << sym);
	HookFunction(found, org, hook);
	return org;
}
//哈希
constexpr size_t Hash(const char* s) {
	unsigned h = 0;
	for (; *s; ++s)
		h = 5 * h + *s;
	return size_t(h);
}
//转换字符串为json
static json StringtoJson(string_view str) {
	try {
		return json::parse(str);
	}
	catch (const exception& e) {
		ERR(e.what());
		return nullptr;
	}
}
#pragma endregion
#pragma region span
//数组观察者
template <typename T>
struct span {
	size_t size;
	T* data;
};
//string_span
template <>
struct span<char> {
	size_t len;
	const char* str;
	span(const char* s) : len(strlen(s)), str(s) {}
	span(const string& s) : len(s.length()), str(s.c_str()) {}
};
#pragma endregion
#pragma region NetWork
struct NetworkIdentifier {
	//空白
	string getAddress() {
		string str;
		SymCall<string&>("?getAddress@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this, &str);
		return str;
	}
	string toString() {
		string str;
		SymCall<string&>("?toString@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this, &str);
		return str;
	}
};
struct SystemAddress {
	char _this[132];
	SystemAddress() {
		SymCall("??0SystemAddress@RakNet@@QEAA@XZ", this);
	}
	string toString() {
		return SymCall<const char*>("?ToString@SystemAddress@RakNet@@QEBAPEBD_ND@Z",
			this, true, ':');
	}
};
struct RakPeer {
	SystemAddress getSystemAddress(NetworkIdentifier* ni) {
		SystemAddress sa;
		SymCall<SystemAddress&>("?GetSystemAddressFromGuid@RakPeer@RakNet@@UEBA?AUSystemAddress@2@URakNetGUID@2@@Z",
			this, &sa, ni);
		return sa;
	}
};
struct ServerNetworkHandler {
	struct Player* _getServerPlayer(VA id, VA pkt) {
		return SymCall<Player*>("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
			this, id, FETCH(char, pkt + 16));
	}
};
#pragma endregion
#pragma region NBT
enum class TagType : uint8_t {
	End, Byte, Short, Int, Int64, Float, Double,
	ByteArray, String, List, Compound, IntArray
};
struct TagMemoryChunk {
	size_t capacity = 0;
	size_t size = 0;
	unique_ptr<uint8_t[]> data;

	TagMemoryChunk(size_t size, uint8_t data[]) :capacity(size), size(size), data(data) {}
};
//所有Tag的合体
struct Tag {
	void* vtable;
	char data[32];

	void put(const string& key, Tag* value) {
		return SymCall("?put@CompoundTag@@QEAAAEAVTag@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$$QEAV2@@Z",
			this, key, value);
	}
	void putByte(const string& key, uint8_t value) {
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
	auto& asByte() { return *reinterpret_cast<uint8_t*>(data); }
	auto& asShort() { return *reinterpret_cast<short*>(data); }
	auto& asInt() { return *reinterpret_cast<int*>(data); }
	auto& asInt64() { return *reinterpret_cast<long long*>(data); }
	auto& asFloat() { return *reinterpret_cast<float*>(data); }
	auto& asDouble() { return *reinterpret_cast<double*>(data); }
	auto& asString() { return *reinterpret_cast<string*>(data); }
	auto& asByteArray() { return *reinterpret_cast<TagMemoryChunk*>(data); }
	auto& asList() { return *reinterpret_cast<vector<Tag*>*>(data); }
	auto& asCompound() { return *reinterpret_cast<map<string, Tag>*>(data); }
};

static Tag* newTag(TagType);
static json ListtoJson(Tag*);
static json CompoundTagtoJson(Tag*);
static Tag* ObjecttoTag(const json&);
static Tag* ArraytoTag(const json&);

//新建tag
Tag* newTag(TagType t) {
	Tag* tag;
	SymCall("?newTag@Tag@@SA?AV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@W4Type@1@@Z",
		&tag, t);
	return tag;
}
json ListtoJson(Tag* t) {
	json value(JsonType::array);
	for (auto& c : t->asList()) {
		switch (t->getListType()) {
		case TagType::End:
			break;
		case TagType::Byte:
			value.push_back(c->asByte());
			break;
		case TagType::Short:
			value.push_back(c->asShort());
			break;
		case TagType::Int:
			value.push_back(c->asInt());
			break;
		case TagType::Int64:
			value.push_back(c->asInt64());
			break;
		case TagType::Float:
			value.push_back(c->asFloat());
			break;
		case TagType::Double:
			value.push_back(c->asDouble());
			break;
		case TagType::ByteArray:

			break;
		case TagType::String:
			value.push_back(c->asString());
			break;
		case TagType::List:
			value.push_back(ListtoJson(c));
			break;
		case TagType::Compound:
			value.push_back(CompoundTagtoJson(c));
			break;
		}
	}
	return value;
}
json CompoundTagtoJson(Tag* t) {
	json value;
	for (auto& x : t->asCompound()) {
		TagType type = x.second.getVariantType();
		json& son = value[x.first + std::to_string(static_cast<uint32_t>(type))];
		switch (type) {
		case TagType::End:
			break;
		case TagType::Byte:
			son = x.second.asByte();
			break;
		case TagType::Short:
			son = x.second.asShort();
			break;
		case TagType::Int:
			son = x.second.asInt();
			break;
		case TagType::Int64:
			son = x.second.asInt64();
			break;
		case TagType::Float:
			son = x.second.asFloat();
			break;
		case TagType::Double:
			son = x.second.asDouble();
			break;
		case TagType::ByteArray:
			for (size_t i = 0; i < x.second.asByteArray().size; ++i)
				son.push_back(x.second.asByteArray().data[i]);
			break;
		case TagType::String:
			son = x.second.asString();
			break;
		case TagType::List:
			son = ListtoJson(&x.second);
			break;
		case TagType::Compound:
			son = CompoundTagtoJson(&x.second);
			break;
		case TagType::IntArray:
			break;
		default:
			break;
		}
	}
	return value;
}
Tag* ObjecttoTag(const json& value) {
	Tag* c = newTag(TagType::Compound);
	for (auto& [key, val] : value.items()) {
		string new_key = key;
		char& e = new_key.back();
		TagType type;
		if (*(&e - 1) == '1' && e == '0') {
			type = TagType::Compound;
			new_key.pop_back();
			new_key.pop_back();
		}
		else if (e >= '0' && e <= '9') {
			type = TagType(e - '0');
			new_key.pop_back();
		}
		else continue;
		//cout << key << " - " << type << endl;
		switch (type) {
		case TagType::End:
			break;
		case TagType::Byte:
			c->putByte(new_key, val.get<uint8_t>());
			break;
		case TagType::Short:
			c->putShort(new_key, val.get<short>());
			break;
		case TagType::Int:
			c->putInt(new_key, val.get<int>());
			break;
		case TagType::Int64:
			c->putInt64(new_key, val.get<long long>());
			break;
		case TagType::Float:
			c->putFloat(new_key, val.get<float>());
			break;
		case TagType::Double:
			c->putFloat(new_key, (val.get<float>()));
			break;
		case TagType::ByteArray: {
			size_t size = val.size();
			uint8_t* data = new uint8_t[size];
			for (unsigned i = 0; i < size; ++i)
				data[i] = uint8_t(val[i].get<int>());
			TagMemoryChunk tmc(size, data);
			c->putByteArray(new_key, tmc);
			break;
		}
		case TagType::String:
			c->putString(new_key, val.get<string>());
			break;
		case TagType::List: {
			Tag* list = ArraytoTag(val);
			c->put(new_key, list);
			list->deList();
			delete list;
			break;
		}
		case TagType::Compound: {
			Tag* t = ObjecttoTag(val);
			c->putCompound(new_key, t);
			//delete t;
			break;
		}
		case TagType::IntArray:
			break;
		default:
			break;
		}
	}
	return c;
}
Tag* ArraytoTag(const json& value) {
	Tag* list = newTag(TagType::List);
	Tag* tag = nullptr;
	for (auto& x : value) {
		switch (x) {
		case JsonType::null:
			break;
		case JsonType::object:
			tag = ObjecttoTag(x);
			break;
		case JsonType::array:
			tag = ArraytoTag(x);
			break;
		case JsonType::string:
			tag = newTag(TagType::String);
			FETCH(string, tag->data) = x.get<string>();
			break;
		case JsonType::boolean:
			break;
		case JsonType::number_integer:
		case JsonType::number_unsigned:
			tag = newTag(TagType::Int);
			FETCH(int, tag->data) = x.get<int>();
			break;
		case JsonType::number_float:
			tag = newTag(TagType::Double);
			FETCH(double, tag->data) = x.get<double>();
			break;
		case JsonType::binary:
			break;
		case JsonType::discarded:
			break;
		default:
			break;
		}
		list->add(tag);
	}
	return list;
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
	short getBlockItemID() {
		return SymCall<short>("?getBlockItemId@BlockLegacy@@QEBAFXZ", this);
		//IDA Item::beginCreativeGroup(,Block*,) 18~22
		//short v3 = FETCH(short, this + 328);
		//if (v3 < 0x100) {
		//	return v3;
		//}
		//return short(255 - v3);
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
		return reinterpret_cast<BlockPos*>(this + 44);
	}
};
struct BlockSource {
	Block* getBlock(const BlockPos* bp) {
		return SymCall<Block*>("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z",
			this, bp);
	}
	bool setBlock(Block* b, BlockPos* bp) {
		return SymCall<bool>("?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HPEBUActorBlockSyncMessage@@@Z",
			this, bp, b, 3, nullptr);
	}
	void neighborChanged(const BlockPos* pos) {
		SymCall("?neighborChanged@BlockSource@@QEAAXAEBVBlockPos@@0@Z",
			this, pos, pos);
	}
	void updateNeighborsAt(const BlockPos* pos) {
		SymCall("?updateNeighborsAt@BlockSource@@QEAAXAEBVBlockPos@@@Z",
			this, pos);
	}
	int getDimensionId() {
		int did;
		SymCall<int>("?getDimensionId@BlockSource@@QEBA?AV?$AutomaticID@VDimension@@H@@XZ",
			this, &did);
		return did;
		//IDA Dimension::onBlockChanged 42
		//return FETCH(int, FETCH(VA, this + 32) + 216);
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
	short mAuxjson;
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
	short getDamagejson() {
		return SymCall<short>("?getDamagejson@ItemStackBase@@QEBAFXZ", this);
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
		Tag* t = nullptr;
		SymCall("?getNetworkUserData@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			this, &t);
		return t;
	}
	Tag* save() {
		Tag* t = nullptr;
		SymCall("?save@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			this, &t);
		return t;
	}
	ItemStack* fromTag(Tag* t) {
		return SymCall<ItemStack*>("?fromTag@ItemStack@@SA?AV1@AEBVCompoundTag@@@Z",
			this, t);
	}
	bool getFromId(short id, short aux, char count) {
		memcpy(this, SYM("?EMPTY_ITEM@ItemStack@@2V1@B"), sizeof(ItemStack));
		bool ret = SymCall<bool>("?_setItem@ItemStackBase@@IEAA_NH@Z", this, id);
		mCount = count;
		mAuxjson = aux;
		mValid = true;
		return ret;
	}
	Item* getItem() {
		return SymCall<Item*>("?getItem@ItemStackBase@@QEBAPEBVItem@@XZ", this);
	}
	void fromJson(const json& value) {
		Tag* t = ObjecttoTag(value);
		fromTag(t);
		t->deCompound();
		delete t;
	}
};
struct Container {
	//获取容器内所有物品
	vector<ItemStack*> getSlots() {
		vector<ItemStack*> s;
		SymCall<VA>("?getSlots@Container@@UEBA?BV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ",
			this, &s);
		return s;
	}
	void clearItem(int slot, int num) {
		SymCall("?removeItem@Container@@UEAAXHH@Z", this, slot, num);
	}
};
#pragma endregion
#pragma region Actor
struct MobEffectInstance {
	char fill[28];

	Tag* save() {
		Tag* t;
		SymCall("?save@MobEffectInstance@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
			this, &t);
		return t;
	}
	void load(Tag* t) {
		SymCall("?load@MobEffectInstance@@SA?AV1@AEBVCompoundTag@@@Z",
			this, t);
	}
};
struct Actor {
	//获取生物名称信息
	string getNameTag() {
		return SymCall<string&>("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this);
	}
	//获取生物当前所处维度ID
	int getDimensionId() {
		int did;
		SymCall("?getDimensionId@Actor@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ",
			this, &did);
		return did;
		//return FETCH(int, this + 236);//IDA Actor::getDimensionId
	}
	//获取生物当前所在坐标
	Vec3* getPos() {
		return SymCall<Vec3*>("?getPos@Actor@@UEBAAEBVVec3@@XZ", this);
		//return (Vec3*)(this + 1268);//IDA Actor::getPos
	}
	//获取生物之前所在坐标
	Vec3* getPosOld() {
		return SymCall<Vec3*>("?getPosOld@Actor@@UEBAAEBVVec3@@XZ", this);
		//return (Vec3*)(this + 1280);//IDA Actor::getPosOld
	}
	//是否已移除
	bool isRemoved() {
		return FETCH(bool, this + 7688);
	}
	//是否悬空
	bool isStand() {//IDA MovePlayerPacket::MovePlayerPacket 30
		return FETCH(bool, this + 480);
	}
	//取方块源
	BlockSource* getRegion() {
		return SymCall<BlockSource*>("?getRegion@Actor@@QEBAAEAVBlockSource@@XZ", this);
		//return FETCH(BlockSource*, this + 872);//IDA Actor::getRegion
	}
	ItemStack* getArmor(int slot) {
		return SymCall<ItemStack*>("?getArmor@Actor@@UEBAAEBVItemStack@@W4ArmorSlot@@@Z",
			this, slot);
	}
	//获取实体类型
	unsigned getEntityTypeId() {
		return SymCall<unsigned>("?getEntityTypeId@Actor@@UEBA?AW4ActorType@@XZ", this);
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
	struct Level* getLevel() {
		return SymCall<Level*>("?getLevel@Actor@@QEBAAEBVLevel@@XZ", this);
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
		Tag* t = newTag(TagType::Compound);
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
	//获取状态列表
	auto getAllEffects() {
		return SymCall<vector<MobEffectInstance>*>("?getAllEffects@Actor@@QEBAAEBV?$vector@VMobEffectInstance@@V?$allocator@VMobEffectInstance@@@std@@@std@@XZ", this);
	}
	//传送
	void teleport(Vec3* target, int did) {
		char mem[128];
		SymCall("?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z",
			&mem, this, target, 0, did, 0, 0, 15);
		SymCall("?applyTarget@TeleportCommand@@SAXAEAVActor@@VTeleportTarget@@@Z",
			this, &mem);
	}
	//新增标签
	bool addTag(const string& str) {
		return SymCall<bool>("?addTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &str);
	}
	//移除标签
	bool removeTag(const string& str) {
		return SymCall<bool>("?removeTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &str);
	}
	//获取标签
	span<string> getTags() {
		span<string> tags;
		SymCall<span<string>&>("?getTags@Actor@@QEBA?BV?$span@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0?0@gsl@@XZ",
			this, &tags);
		return tags;
	}
};
struct Mob : Actor {};
struct Player : Mob {
	string getUuid() {//IDA ServerNetworkHandler::_createNewPlayer 222
		string p;
		SymCall<string&>("?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this + 3000, &p);
		return p;
	}
	//发送数据包
	void sendPacket(VA pkt) {
		SymCall("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z",
			this, pkt);
	}
	//根据地图信息获取玩家xuid
	string& getXuid() {
		return SymCall<string&>("?getPlayerXUID@Level@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUUID@mce@@@Z",
			getLevel(), this + 3000);
	}
	//重设服务器玩家名
	void setName(const string& name) {
		SymCall("?setName@Player@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &name);
	}
	//获取网络标识符
	NetworkIdentifier* getClientId() {
		return SymCall<NetworkIdentifier*>("?getClientId@Player@@QEBAAEBVNetworkIdentifier@@XZ",
			this);
		//IDA ServerPlayer::setPermissions 34
	}
	//获取背包
	Container* getInventory() {
		return SymCall<Container*>("?getInventory@Player@@QEAAAEAVContainer@@XZ", this);
		//return FETCH(Container*, FETCH(VA, this + 3208) + 176);//IDA Player::getInventory
	}
	//获取装备容器
	Container* getArmorContainer() {
		return FETCH(Container*, this + 1648);//IDA Actor::_setArmorContainer 11
	}
	//获取末影箱
	Container* getEnderChestContainer() {
		return FETCH(Container*, this + 4360);//IDA ReplaceItemCommand::execute 1086 
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
	void addLevel(int level) {
		SymCall("?addLevels@Player@@UEAAXH@Z", this, level);
	}
	//获取当前选中的框位置
	int getSelectedItemSlot() {
		return SymCall<int>("?getSelectedItemSlot@Player@@QEBAHXZ", this);
		//return FETCH(unsigned, FETCH(VA, this + 3208) + 16);//IDA Player::getSelectedItemSlot
	}
	//获取当前物品
	ItemStack* getSelectedItem() {
		return SymCall<ItemStack*>("?getSelectedItem@Player@@QEBAAEBVItemStack@@XZ", this);
	}
	//获取背包物品
	ItemStack* getInventoryItem(int slot) {
		return getInventory()->getSlots()[slot];
	}
	//获取游戏时命令权限
	char getPermissions() {
		return *FETCH(char*, this + 2376);//IDA ServerPlayer::setPermissions 22
	}
	//设置游戏时命令权限
	void setPermissions(char m) {
		SymCall("?setPermissions@ServerPlayer@@UEAAXW4CommandPermissionLevel@@@Z",
			this, m);
	}
	//获取游戏时游玩权限
	char getPermissionLevel() {//IDA Abilities::setPlayerPermissions ?
		return FETCH(char, FETCH(char*, this + 2376) + 1);
	}
	//设置游戏时游玩权限
	void setPermissionLevel(char m) {
		SymCall("?setPlayerPermissions@Abilities@@QEAAXW4PlayerPermissionLevel@@@Z",
			this + 2376, m);
	}
	//获取设备id
	string getDeviceId() {
		return FETCH(string, this + 8352); //IDA Player::Player  v13 + 8352
	}
	//获取设备系统类型
	int getDeviceOS() {
		return FETCH(int, this + 2368);	//IDA ServerNetworkHandler::createNewPlayer  ConnectionRequest::getDeviceOS
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
	string getScoreName() {
		return FETCH(string, this + 64);
	}
	//获取计分板展示名称
	string getScoreDisplayName() {
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
	Objective* getObjective(string str) {
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
struct SPSCQueue;
struct Level {
	//获取方块源 没这个维度返回空指针
	BlockSource* getBlockSource(int did) {
		VA d = SymCall<VA>("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
			this, did);
		if (!d)
			return nullptr;
		return FETCH(BlockSource*, d + 96);//IDA Level::tickEntities 120
	}
	Scoreboard* getScoreBoard() {
		return FETCH(Scoreboard*, this + 8600);//IDA Level::getScoreboard
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
	Player* getPlayerByXuid(const string& xuid) {
		return SymCall<Player*>("?getPlayerByXuid@Level@@UEBAPEAVPlayer@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &xuid);
	}
	vector<Player*> getAllPlayers() {
		return FETCH(vector<Player*>, this + 112);//IDA Level::forEachPlayer
	}
	BlockPalette* getBlockPalette() {
		return FETCH(BlockPalette*, this + 2120);
	}
};
#pragma endregion
#pragma region Structure
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
		reinterpret_cast<string*>(this)->~basic_string();
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
	StructureTemplate(const span<char>& s) {
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
	void fromJson(const json& value) {
		Tag* t = ObjecttoTag(value);
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
