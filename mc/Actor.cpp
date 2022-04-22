#include "Actor.h"
#include "tool.h"
#include "Tag.h"
#include "ItemStack.h"
#include "ScoreBoard.h"

using namespace std;
//创建包
inline uintptr_t createPacket(int type) {
	uintptr_t pkt[2];
	SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		pkt, type);
	return *pkt;
}

string Actor::getNameTag() {
	return SymCall<string&>("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this);
}

//设置生物名称信息

void Actor::setNameTag(const string& name) {
	VirtualCall(0x1F8, this, &name);
}

//设置生物名称是否可见

void Actor::setNameTagVisible(bool visible) {
	SymCall("?setNameTagVisible@Actor@@UEAAX_N@Z", this, visible);
}

int Actor::getDimensionId() {
	int did;
	SymCall("?getDimensionId@Actor@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ",
		this, &did);
	return did;
}

//获取生物当前所在坐标
Vec3* Actor::getPos() {
	return SymCall<Vec3*>("?getPosition@Actor@@UEBAAEBVVec3@@XZ", this);
}

//获取生物之前所在坐标

Vec3* Actor::getPosPrev() {
	return SymCall<Vec3*>("?getPosPrev@Actor@@UEBAAEBVVec3@@XZ", this);
}

//是否悬空

bool Actor::isStanding() {//IDA MovePlayerPacket::MovePlayerPacket
	return Dereference<bool>(this, 472);
}

//是否潜行

bool Actor::isSneaking() {
	return SymCall<bool>("?isSneaking@Actor@@QEBA_NXZ", this);
}

//取方块源

BlockSource* Actor::getRegion() {
	return SymCall<BlockSource*>("?getRegion@Actor@@QEBAAEAVBlockSource@@XZ", this);
	//return Dereference<BlockSource*, this + 872);//IDA Actor::getRegion
}

ItemStack* Actor::getArmor(int slot) {
	return SymCall<ItemStack*>("?getArmor@Actor@@UEBAAEBVItemStack@@W4ArmorSlot@@@Z",
		this, slot);
}

//获取实体类型

unsigned Actor::getEntityTypeId() {
	return VirtualCall<unsigned>(0x550, this);
	//return SymCall<unsigned>("?getEntityTypeId@Actor@@UEBA?AW4ActorType@@XZ", this);
}

//获取查询用ID

uintptr_t Actor::getUniqueID() {
	return SymCall<uintptr_t>("?getUniqueID@Actor@@QEBAAEBUActorUniqueID@@XZ", this);
}

//获取实体类型

string Actor::getEntityTypeName() {
	string type;
	SymCall<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
		&type, getEntityTypeId());
	return type;
}

//更新属性

uintptr_t Actor::updateAttrs() {
	return SymCall<uintptr_t>("?_sendDirtyActorData@Actor@@QEAAXXZ", this);
}

//获取地图信息

Level* Actor::getLevel() {
	return SymCall<Level*>("?getLevel@Actor@@QEBAAEBVLevel@@XZ", this);
}

//添加一个状态

uintptr_t Actor::addEffect(uintptr_t ef) {
	return SymCall<uintptr_t>("?addEffect@Actor@@QEAAXAEBVMobEffectInstance@@@Z", this, ef);
}

//获取生命值

int Actor::getHealth() {
	return SymCall<int>("?getHealth@Actor@@QEBAHXZ", this);
}

int Actor::getMaxHealth() {
	return SymCall<int>("?getMaxHealth@Actor@@QEBAHXZ", this);
}

void Actor::setHealth(int value) {
	uintptr_t hattr = (*reinterpret_cast<uintptr_t(**)(Actor*, void*)>(*(uintptr_t*)this + 1552))
		(this, SYM("?HEALTH@SharedAttributes@@2VAttribute@@B"));
	Dereference<int>(hattr, 132) = value;
	//SymCall("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
}

void Actor::setMaxHealth(int value) {
	uintptr_t hattr = (*reinterpret_cast<uintptr_t(**)(Actor*, void*)>(*(uintptr_t*)this + 1552))
		(this, SYM("?HEALTH@SharedAttributes@@2VAttribute@@B"));
	Dereference<int>(hattr, 128) = value;
	//SymCall("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
}

//获取副手

ItemStack* Actor::getOffhandSlot() {
	return SymCall<ItemStack*>("?getOffhandSlot@Actor@@QEBAAEBVItemStack@@XZ", this);
}

Tag* Actor::save() {
	Tag* t = newTag(TagType::Compound);
	VirtualCall(0x530, this, t);
	//SymCall("?save@Actor@@UEAA_NAEAVCompoundTag@@@Z", this, t);
	return t;
}

//设置大小

void Actor::setSize(float f1, float f2) {
	SymCall("?setSize@Actor@@UEAAXMM@Z", this, f1, f2);
}

//获取状态列表

auto Actor::getAllEffects() {
	//return SymCall<vector<MobEffectInstance>*>("?getAllEffects@Actor@@QEBAAEBV?$vector@VMobEffectInstance@@V?$allocator@VMobEffectInstance@@@std@@@std@@XZ", this);
}
class TeleportRotationData {
public:
	char filler[32];
};
//传送
void Actor::teleport(Vec3* target, int did) {
	char mem[128];
	TeleportRotationData data;
	SymCall("?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$AutomaticID@VDimension@@H@@AEBV?$optional@VTeleportRotationData@@@std@@H@Z",
		&mem, this, target, 0, did, &data, 15);
	SymCall("?applyTarget@TeleportCommand@@SAXAEAVActor@@VTeleportTarget@@@Z",
		this, &mem);
}

//新增标签

bool Actor::addTag(const string& str) {
	return SymCall<bool>("?addTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, &str);
}

//移除标签

bool Actor::removeTag(const string& str) {
	return SymCall<bool>("?removeTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, &str);
}

//获取标签

span<string> Actor::getTags() {
	span<string> tags;
	SymCall<span<string>&>("?getTags@Actor@@QEBA?BV?$span@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0?0@gsl@@XZ",
		this, &tags);
	return tags;
}

void Actor::kill() {
	SymCall("?kill@Mob@@UEAAXXZ", this);
}

UserEntityIdentifierComponent* Player::getUserEntityIdentifierComponent() {
	return SymCall<UserEntityIdentifierComponent*>("??$tryGetComponent@VUserEntityIdentifierComponent@@@Actor@@QEAAPEAVUserEntityIdentifierComponent@@XZ",
		this);
}

string Player::getUuid() {//IDA ServerNetworkHandler::_createNewPlayer 222
	//uintptr_t userid = SymCall<uintptr_t>("??$tryGetComponent@VUserEntityIdentifierComponent@@@Actor@@QEAAPEAVUserEntityIdentifierComponent@@XZ",
	//	this);
	string p;
	void* v33 = **(void***)(this + 8);
	int v107 = *(int*)(this + 16);
	void* v34 = SymCall<void*>("??$try_get@VUserEntityIdentifierComponent@@@?$basic_registry@VEntityId@@@entt@@QEBA?A_PVEntityId@@@Z", v33, &v107);
	SymCall<string&>("?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		uintptr_t(v34) + 168, &p);
	return p;
}

//根据地图信息获取玩家xuid

string Player::getXuid() {
	string xuid;
	SymCall<string&>("?getXuid@Player@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this,&xuid);
	return xuid;
}

//获取网络标识符

NetworkIdentifier* Player::getClientId() {
	return &getUserEntityIdentifierComponent()->nid;
	//void* v6 = **(void***)(this + 8);
	//int v58 = *(int*)(this + 16);
	//char* v7 = SymCall<char*>("??$try_get@VUserEntityIdentifierComponent@@@?$basic_registry@VEntityId@@@entt@@QEBA?A_PVEntityId@@@Z", v6, &v58);
	//return (NetworkIdentifier*)(uintptr_t(v7) + 160);
	//IDA ServerPlayer::setPermissions 34
}

//获取背包

Container* Player::getInventory() {
	return SymCall<Container*>("?getInventory@Player@@QEAAAEAVContainer@@XZ",
		this);
	//return Dereference<Container*, Dereference<uintptr_t, this + 3208) + 176);//IDA Player::getInventory
}

//获取装备容器

Container* Player::getArmorContainer() {
	return SymCall<Container*>("?getArmorContainer@Actor@@QEBAAEBVSimpleContainer@@XZ",
		this);
}

//获取末影箱

Container* Player::getEnderChestContainer() {
	//IDA Player::addAdditionalSaveData EnderChestInventory
	return Dereference<Container*>(this, 4184);
}

//设置一个装备

uintptr_t Player::setArmor(int i, ItemStack* item) {
	return SymCall<uintptr_t>("?setArmor@ServerPlayer@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z",
		this, i, item);
}

//设置副手

uintptr_t Player::setOffhandSlot(ItemStack* item) {
	return SymCall<uintptr_t>("?setOffhandSlot@Player@@UEAAXAEBVItemStack@@@Z",
		this, item);
}

//添加一个物品

void Player::addItem(ItemStack* item) {
	SymCall<uintptr_t>("?addItem@@YAXAEAVPlayer@@AEAVItemStack@@@Z",
		this, item);
}

//增加等级

void Player::addLevel(int level) {
	SymCall("?addLevels@Player@@UEAAXH@Z",
		this, level);
}

//获取当前选中的框位置

int Player::getSelectedItemSlot() {
	return SymCall<int>("?getSelectedItemSlot@Player@@QEBAHXZ",
		this);
}

//获取当前物品

ItemStack* Player::getSelectedItem() {
	return SymCall<ItemStack*>("?getSelectedItem@Player@@QEBAAEBVItemStack@@XZ",
		this);
}

//获取背包物品

ItemStack* Player::getInventoryItem(int slot) {
	return getInventory()->getSlots()[slot];
}

//获取游戏时游玩权限
PlayerPermissionLevel Player::getPlayerPermissionLevel() {
	return SymCall<PlayerPermissionLevel>("?getPlayerPermissionLevel@Player@@QEBA?AW4PlayerPermissionLevel@@XZ",
		this);
}

//设置游戏时命令权限

void Player::setPermissions(PlayerPermissionLevel m) {
	SymCall("?setPermissions@ServerPlayer@@UEAAXW4CommandPermissionLevel@@@Z",
		this, m);
}

//获取设备id
string Player::getPlatformOnlineId() {
	string id;
	SymCall<string&>("?getPlatformOnlineId@Player@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &id);
	return id;
	//return Dereference<string, this + 8352);
	//IDA Player::Player  v13 + 8352
}

//获取设备系统类型

unsigned Player::getPlatform() {
	return SymCall<unsigned>("?getPlatform@Player@@QEBA?AW4BuildPlatform@@XZ",
		this);
	//return Dereference<int, this + 2336);
	//IDA ServerNetworkHandler::createNewPlayer  ConnectionRequest::getDeviceOS
}

//发送背包

void Player::sendInventroy() {
	SymCall("?sendInventory@ServerPlayer@@UEAAX_N@Z",
		this, true);
}

//刷新区块

void Player::resendAllChunks() {
	SymCall("?resendAllChunks@ServerPlayer@@UEAAXXZ", this);
}

//发送数据包

void Player::sendNetworkPacket(uintptr_t pkt) {
	SymCall("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z",
		this, pkt);
}

unsigned Player::sendModalFormRequestPacket(const string& str) {
	static unsigned id = 0;
	uintptr_t pkt = createPacket(100);
	Dereference<unsigned>(pkt, 48) = ++id;
	Dereference<string>(pkt, 56) = str;
	sendNetworkPacket(pkt);
	return id;
}

void Player::sendTransferPacket(const string& address, unsigned short port) {
	uintptr_t pkt = createPacket(85);
	Dereference<string>(pkt, 48) = address;
	Dereference<unsigned short>(pkt, 80) = port;
	sendNetworkPacket(pkt);
}

void Player::sendDisconnectPacket(const string& msg) {
	uintptr_t pkt = createPacket(5);
	Dereference<string>(pkt, 56) = msg;
	sendNetworkPacket(pkt);
}

void Player::sendTextPacket(int mode, const string& msg) {
	uintptr_t pkt = createPacket(9);
	Dereference<int>(pkt, 48) = mode;
	Dereference<string>(pkt, 56) = getNameTag();
	Dereference<string>(pkt, 88) = msg;
	sendNetworkPacket(pkt);
}

void Player::sendCommandRequestPacket(const string& cmd) {
	uintptr_t pkt = createPacket(77);
	Dereference<string>(pkt, 48) = cmd;
	//SymCall<uintptr_t>("?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	//	global<ServerNetworkHandler>, getClientId(), pkt);
	sendNetworkPacket(pkt);
}

void Player::sendBossEventCodePacket(string name, float per, int eventtype) {
	uintptr_t pkt = createPacket(74);
	Dereference<uintptr_t>(pkt, 56) = Dereference<uintptr_t>(getUniqueID());
	//Dereference<uintptr_t, pkt + 64) = Dereference<uintptr_t, getUniqueID());
	Dereference<int>(pkt, 72) = eventtype;//0显示,1更新,2隐藏,
	Dereference<string>(pkt, 80) = name;
	Dereference<float>(pkt, 112) = per;
	sendNetworkPacket(pkt);
}

void Player::sendsetDisplayObjectivePacket(const string& title, const string& name) {
	uintptr_t pkt = createPacket(107);
	Dereference<string>(pkt, 48) = "sidebar";
	Dereference<string>(pkt, 80) = name;
	Dereference<string>(pkt, 112) = title;
	Dereference<string>(pkt, 144) = "dummy";
	Dereference<char>(pkt, 176) = 0;
	sendNetworkPacket(pkt);
}

void Player::sendSetScorePacket(char type, const vector<ScorePacketInfo>& slot) {
	uintptr_t pkt = createPacket(108);
	Dereference<char>(pkt, 48) = type;//{set,remove}
	Dereference<vector<ScorePacketInfo>>(pkt, 56) = slot;
	sendNetworkPacket(pkt);
}

bool IsPlayer(Actor* ptr) {
	if (ptr == nullptr)
		return false;
	if (ptr->getEntityTypeId() != 319)
		return false;
	return true;
}
