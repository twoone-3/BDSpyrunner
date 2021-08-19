#include <Actor.h>
#include <Tag.h>
#include <ItemStack.h>
#include <global.h>
#include <ScoreBoard.h>

using namespace std;
string Actor::getNameTag() {
	return SymCall<string&>("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this);
}

//设置生物名称信息

void Actor::setNameTag(const std::string&name) {
	VirtualCall(0x1F8, this, &name);
}

//设置生物名称是否可见

void Actor::setNameTagVisible(bool visible) {
	SymCall("?setNameTagVisible@Actor@@UEAAX_N@Z", this, visible);
}

//获取生物当前所处维度ID

int Actor::getDimensionId() {
	int did;
	SymCall("?getDimensionId@Actor@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ",
		this, &did);
	return did;
	//return FETCH(int, this + 236);//IDA Actor::getDimensionId
}

//获取生物当前所在坐标

Vec3* Actor::getPos() {
	return SymCall<Vec3*>("?getPos@Actor@@UEBAAEBVVec3@@XZ", this);
	//return (Vec3*)(this + 1268);//IDA Actor::getPos
}

//获取生物之前所在坐标

Vec3* Actor::getPosOld() {
	return SymCall<Vec3*>("?getPosOld@Actor@@UEBAAEBVVec3@@XZ", this);
	//return (Vec3*)(this + 1280);//IDA Actor::getPosOld
}

//是否已移除

bool Actor::isRemoved() {
	return FETCH(bool, this + 7688);
}

//是否悬空

bool Actor::isStand() {//IDA MovePlayerPacket::MovePlayerPacket 30
	return FETCH(bool, this + 480);
}


//取方块源

BlockSource* Actor::getRegion() {
	return SymCall<BlockSource*>("?getRegion@Actor@@QEBAAEAVBlockSource@@XZ", this);
	//return FETCH(BlockSource*, this + 872);//IDA Actor::getRegion
}

ItemStack* Actor::getArmor(int slot) {
	return SymCall<ItemStack*>("?getArmor@Actor@@UEBAAEBVItemStack@@W4ArmorSlot@@@Z",
		this, slot);
}

//获取实体类型

unsigned Actor::getEntityTypeId() {
	return VirtualCall<unsigned>(0x520, this);
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
	FETCH(int, hattr + 132) = value;
	//SymCall("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
}

void Actor::setMaxHealth(int value) {
	uintptr_t hattr = (*reinterpret_cast<uintptr_t(**)(Actor*, void*)>(*(uintptr_t*)this + 1552))
		(this, SYM("?HEALTH@SharedAttributes@@2VAttribute@@B"));
	FETCH(int, hattr + 128) = value;
	//SymCall("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
}

//获取副手

ItemStack* Actor::getOffHand() {
	return SymCall<ItemStack*>("?getOffhandSlot@Actor@@QEBAAEBVItemStack@@XZ", this);
}

Tag* Actor::save() {
	Tag* t = newTag(TagType::Compound);
	SymCall("?save@Actor@@UEAA_NAEAVCompoundTag@@@Z", this, t);
	return t;
}

//设置大小

void Actor::setSize(float f1, float f2) {
	SymCall("?setSize@Actor@@UEAAXMM@Z", this, f1, f2);
}

//是否潜行

bool Actor::isSneaking() {
	return SymCall<bool>("?isSneaking@Actor@@QEBA_NXZ", this);
}

//获取状态列表

auto Actor::getAllEffects() {
	//return SymCall<vector<MobEffectInstance>*>("?getAllEffects@Actor@@QEBAAEBV?$vector@VMobEffectInstance@@V?$allocator@VMobEffectInstance@@@std@@@std@@XZ", this);
}

//传送

void Actor::teleport(Vec3* target, int did) {
	char mem[128];
	SymCall("?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z",
		&mem, this, target, 0, did, 0, 0, 15);
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

string Player::getUuid() {//IDA ServerNetworkHandler::_createNewPlayer 222
	string p;
	SymCall<string&>("?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this + 3000, &p);
	return p;
}

//发送数据包

//根据地图信息获取玩家xuid

string& Player::getXuid() {
	return SymCall<string&>("?getPlayerXUID@Level@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUUID@mce@@@Z",
		getLevel(), this + 3000);
}

//获取网络标识符

NetworkIdentifier* Player::getClientId() {
	return SymCall<NetworkIdentifier*>("?getClientId@Player@@QEBAAEBVNetworkIdentifier@@XZ",
		this);
	//IDA ServerPlayer::setPermissions 34
}

//获取背包

Container* Player::getInventory() {
	return SymCall<Container*>("?getInventory@Player@@QEAAAEAVContainer@@XZ", this);
	//return FETCH(Container*, FETCH(uintptr_t, this + 3208) + 176);//IDA Player::getInventory
}

//获取装备容器

Container* Player::getArmorContainer() {
	return FETCH(Container*, this + 1648);//IDA Actor::_setArmorContainer 11
}

//获取末影箱

Container* Player::getEnderChestContainer() {
	return FETCH(Container*, this + 4360);//IDA ReplaceItemCommand::execute 1086 
}

//设置一个装备

uintptr_t Player::setArmor(int i, ItemStack* item) {
	return SymCall<uintptr_t>("?setArmor@ServerPlayer@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z", this, i, item);
}

//设置副手

uintptr_t Player::setOffhandSlot(ItemStack* item) {
	return SymCall<uintptr_t>("?setOffhandSlot@Player@@UEAAXAEBVItemStack@@@Z", this, item);
}

//添加一个物品

void Player::addItem(ItemStack* item) {
	SymCall<uintptr_t>("?addItem@@YAXAEAVPlayer@@AEAVItemStack@@@Z", this, item);
}

//增加等级

void Player::addLevel(int level) {
	SymCall("?addLevels@Player@@UEAAXH@Z", this, level);
}

//获取当前选中的框位置

int Player::getSelectedItemSlot() {
	return SymCall<int>("?getSelectedItemSlot@Player@@QEBAHXZ", this);
	//return FETCH(unsigned, FETCH(uintptr_t, this + 3208) + 16);//IDA Player::getSelectedItemSlot
}

//获取当前物品

ItemStack* Player::getSelectedItem() {
	return SymCall<ItemStack*>("?getSelectedItem@Player@@QEBAAEBVItemStack@@XZ", this);
}

//获取背包物品

ItemStack* Player::getInventoryItem(int slot) {
	return getInventory()->getSlots()[slot];
}

//获取游戏时命令权限

char Player::getPermissions() {
	return *FETCH(char*, this + 2376);//IDA ServerPlayer::setPermissions 22
}

//设置游戏时命令权限

void Player::setPermissions(char m) {
	SymCall("?setPermissions@ServerPlayer@@UEAAXW4CommandPermissionLevel@@@Z",
		this, m);
}

//获取游戏时游玩权限

char Player::getPermissionLevel() {//IDA Abilities::setPlayerPermissions ?
	return FETCH(char, FETCH(char*, this + 2376) + 1);
}

//设置游戏时游玩权限

void Player::setPermissionLevel(char m) {
	SymCall("?setPlayerPermissions@Abilities@@QEAAXW4PlayerPermissionLevel@@@Z",
		this + 2376, m);
}

//获取设备id

string Player::getDeviceId() {
	return FETCH(string, this + 8352); //IDA Player::Player  v13 + 8352
}

//获取设备系统类型

int Player::getDeviceOS() {
	return FETCH(int, this + 2368);	//IDA ServerNetworkHandler::createNewPlayer  ConnectionRequest::getDeviceOS
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

void Player::sendPacket(uintptr_t pkt) {
	SymCall("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z",
		this, pkt);
}

//使玩家客户端崩溃
bool Player::crash() {
	uintptr_t pkt = createPacket(58);
	FETCH(int, pkt + 14) = 0;
	FETCH(int, pkt + 15) = 0;
	FETCH(bool, pkt + 48) = 1;
	sendPacket(pkt);
	return true;
}

//杀死实体
bool Actor::kill() {
	SymCall("?kill@Mob@@UEAAXXZ", this);
	return true;
}

unsigned Player::sendModalFormRequestPacket(const string& str) {
	static unsigned id = 0;
	uintptr_t pkt = createPacket(100);
	FETCH(unsigned, pkt + 48) = ++id;
	FETCH(string, pkt + 56) = str;
	sendPacket(pkt);
	return id;
}

void Player::sendTransferPacket(const string& address, unsigned short port) {
	uintptr_t pkt = createPacket(85);
	FETCH(string, pkt + 48) = address;
	FETCH(unsigned short, pkt + 80) = port;
	sendPacket(pkt);
}

void Player::sendDisconnectPacket(const string& msg) {
	uintptr_t pkt = createPacket(5);
	FETCH(string, pkt + 56) = msg;
	sendPacket(pkt);
}

void Player::sendTextPacket(int mode, const string& msg) {
	uintptr_t pkt = createPacket(9);
	FETCH(int, pkt + 48) = mode;
	FETCH(string, pkt + 56) = getNameTag();
	FETCH(string, pkt + 88) = msg;
	sendPacket(pkt);
}

void Player::sendCommandRequestPacket(const string& cmd) {
	uintptr_t pkt = createPacket(77);
	FETCH(string, pkt + 48) = cmd;
	SymCall<uintptr_t>("?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
		g_server_network_handler, getClientId(), pkt);
	//p->sendPacket(pkt);
}

void Player::sendBossEventCodePacket(string name, float per, int eventtype) {
	uintptr_t pkt = createPacket(74);
	FETCH(uintptr_t, pkt + 56) = FETCH(uintptr_t, getUniqueID());
	//FETCH(uintptr_t, pkt + 64) = FETCH(uintptr_t, getUniqueID());
	FETCH(int, pkt + 72) = eventtype;//0显示,1更新,2隐藏,
	FETCH(string, pkt + 80) = name;
	FETCH(float, pkt + 112) = per;
	sendPacket(pkt);
}

void Player::sendsetDisplayObjectivePacket(const string& title, const string& name) {
	uintptr_t pkt = createPacket(107);
	FETCH(string, pkt + 48) = "sidebar";
	FETCH(string, pkt + 80) = name;
	FETCH(string, pkt + 112) = title;
	FETCH(string, pkt + 144) = "dummy";
	FETCH(char, pkt + 176) = 0;
	sendPacket(pkt);
}

void Player::sendSetScorePacket(char type, const vector<ScorePacketInfo>& slot) {
	uintptr_t pkt = createPacket(108);
	FETCH(char, pkt + 48) = type;//{set,remove}
	FETCH(vector<ScorePacketInfo>, pkt + 56) = slot;
	sendPacket(pkt);
}
