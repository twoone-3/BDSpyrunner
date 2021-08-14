#include "Actor.h"
#include "Tag.h"
#include "ItemStack.h"
//获取生物名称信息

inline string Actor::getNameTag() {
	return SymCall<string&>("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this);
}

//获取生物当前所处维度ID

inline int Actor::getDimensionId() {
	int did;
	SymCall("?getDimensionId@Actor@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ",
		this, &did);
	return did;
	//return FETCH(int, this + 236);//IDA Actor::getDimensionId
}

//获取生物当前所在坐标

inline Vec3* Actor::getPos() {
	return SymCall<Vec3*>("?getPos@Actor@@UEBAAEBVVec3@@XZ", this);
	//return (Vec3*)(this + 1268);//IDA Actor::getPos
}

//获取生物之前所在坐标

inline Vec3* Actor::getPosOld() {
	return SymCall<Vec3*>("?getPosOld@Actor@@UEBAAEBVVec3@@XZ", this);
	//return (Vec3*)(this + 1280);//IDA Actor::getPosOld
}

//是否已移除

inline bool Actor::isRemoved() {
	return FETCH(bool, this + 7688);
}

//是否悬空

inline bool Actor::isStand() {//IDA MovePlayerPacket::MovePlayerPacket 30
	return FETCH(bool, this + 480);
}


//取方块源

inline BlockSource* Actor::getRegion() {
	return SymCall<BlockSource*>("?getRegion@Actor@@QEBAAEAVBlockSource@@XZ", this);
	//return FETCH(BlockSource*, this + 872);//IDA Actor::getRegion
}

inline ItemStack* Actor::getArmor(int slot) {
	return SymCall<ItemStack*>("?getArmor@Actor@@UEBAAEBVItemStack@@W4ArmorSlot@@@Z",
		this, slot);
}

//获取实体类型

inline unsigned Actor::getEntityTypeId() {
	return VirtualCall<unsigned>(0x520, this);
	//return SymCall<unsigned>("?getEntityTypeId@Actor@@UEBA?AW4ActorType@@XZ", this);
}

//获取查询用ID

inline VA Actor::getUniqueID() {
	return SymCall<VA>("?getUniqueID@Actor@@QEBAAEBUActorUniqueID@@XZ", this);
}

//获取实体类型

inline string Actor::getEntityTypeName() {
	string type;
	SymCall<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
		&type, getEntityTypeId());
	return type;
}

//更新属性

inline VA Actor::updateAttrs() {
	return SymCall<VA>("?_sendDirtyActorData@Actor@@QEAAXXZ", this);
}

//获取地图信息

inline Level* Actor::getLevel() {
	return SymCall<Level*>("?getLevel@Actor@@QEBAAEBVLevel@@XZ", this);
}

//添加一个状态

inline VA Actor::addEffect(VA ef) {
	return SymCall<VA>("?addEffect@Actor@@QEAAXAEBVMobEffectInstance@@@Z", this, ef);
}

//获取生命值

inline int Actor::getHealth() {
	return SymCall<int>("?getHealth@Actor@@QEBAHXZ", this);
}

inline int Actor::getMaxHealth() {
	return SymCall<int>("?getMaxHealth@Actor@@QEBAHXZ", this);
}

inline void Actor::setHealth(int value) {
	VA hattr = (*reinterpret_cast<VA(**)(Actor*, void*)>(*(VA*)this + 1552))
		(this, SYM("?HEALTH@SharedAttributes@@2VAttribute@@B"));
	FETCH(int, hattr + 132) = value;
	//SymCall("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
}

inline void Actor::setMaxHealth(int value) {
	VA hattr = (*reinterpret_cast<VA(**)(Actor*, void*)>(*(VA*)this + 1552))
		(this, SYM("?HEALTH@SharedAttributes@@2VAttribute@@B"));
	FETCH(int, hattr + 128) = value;
	//SymCall("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
}

//获取副手

inline ItemStack* Actor::getOffHand() {
	return SymCall<ItemStack*>("?getOffhandSlot@Actor@@QEBAAEBVItemStack@@XZ", this);
}

inline Tag* Actor::save() {
	Tag* t = newTag(TagType::Compound);
	SymCall("?save@Actor@@UEAA_NAEAVCompoundTag@@@Z", this, t);
	return t;
}

//设置大小

inline void Actor::setSize(float f1, float f2) {
	SymCall("?setSize@Actor@@UEAAXMM@Z", this, f1, f2);
}

//是否潜行

inline bool Actor::isSneaking() {
	return SymCall<bool>("?isSneaking@Actor@@QEBA_NXZ", this);
}

//获取状态列表

inline auto Actor::getAllEffects() {
	//return SymCall<vector<MobEffectInstance>*>("?getAllEffects@Actor@@QEBAAEBV?$vector@VMobEffectInstance@@V?$allocator@VMobEffectInstance@@@std@@@std@@XZ", this);
}

//传送

inline void Actor::teleport(Vec3* target, int did) {
	char mem[128];
	SymCall("?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z",
		&mem, this, target, 0, did, 0, 0, 15);
	SymCall("?applyTarget@TeleportCommand@@SAXAEAVActor@@VTeleportTarget@@@Z",
		this, &mem);
}

//新增标签

inline bool Actor::addTag(const string& str) {
	return SymCall<bool>("?addTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, &str);
}

//移除标签

inline bool Actor::removeTag(const string& str) {
	return SymCall<bool>("?removeTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, &str);
}

//获取标签

inline span<string> Actor::getTags() {
	span<string> tags;
	SymCall<span<string>&>("?getTags@Actor@@QEBA?BV?$span@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0?0@gsl@@XZ",
		this, &tags);
	return tags;
}

inline string Player::getUuid() {//IDA ServerNetworkHandler::_createNewPlayer 222
	string p;
	SymCall<string&>("?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this + 3000, &p);
	return p;
}

//发送数据包

inline void Player::sendPacket(VA pkt) {
	SymCall("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z",
		this, pkt);
}

//根据地图信息获取玩家xuid

inline string& Player::getXuid() {
	return SymCall<string&>("?getPlayerXUID@Level@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUUID@mce@@@Z",
		getLevel(), this + 3000);
}

//重设服务器玩家名

inline void Player::setName(const string& name) {
	SymCall("?setName@Player@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
		this, &name);
}

//获取网络标识符

inline NetworkIdentifier* Player::getClientId() {
	return SymCall<NetworkIdentifier*>("?getClientId@Player@@QEBAAEBVNetworkIdentifier@@XZ",
		this);
	//IDA ServerPlayer::setPermissions 34
}

//获取背包

inline Container* Player::getInventory() {
	return SymCall<Container*>("?getInventory@Player@@QEAAAEAVContainer@@XZ", this);
	//return FETCH(Container*, FETCH(VA, this + 3208) + 176);//IDA Player::getInventory
}

//获取装备容器

inline Container* Player::getArmorContainer() {
	return FETCH(Container*, this + 1648);//IDA Actor::_setArmorContainer 11
}

//获取末影箱

inline Container* Player::getEnderChestContainer() {
	return FETCH(Container*, this + 4360);//IDA ReplaceItemCommand::execute 1086 
}

//设置一个装备

inline VA Player::setArmor(int i, ItemStack* item) {
	return SymCall<VA>("?setArmor@ServerPlayer@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z", this, i, item);
}

//设置副手

inline VA Player::setOffhandSlot(ItemStack* item) {
	return SymCall<VA>("?setOffhandSlot@Player@@UEAAXAEBVItemStack@@@Z", this, item);
}

//添加一个物品

inline void Player::addItem(ItemStack* item) {
	SymCall<VA>("?addItem@@YAXAEAVPlayer@@AEAVItemStack@@@Z", this, item);
}

//增加等级

inline void Player::addLevel(int level) {
	SymCall("?addLevels@Player@@UEAAXH@Z", this, level);
}

//获取当前选中的框位置

inline int Player::getSelectedItemSlot() {
	return SymCall<int>("?getSelectedItemSlot@Player@@QEBAHXZ", this);
	//return FETCH(unsigned, FETCH(VA, this + 3208) + 16);//IDA Player::getSelectedItemSlot
}

//获取当前物品

inline ItemStack* Player::getSelectedItem() {
	return SymCall<ItemStack*>("?getSelectedItem@Player@@QEBAAEBVItemStack@@XZ", this);
}

//获取背包物品

inline ItemStack* Player::getInventoryItem(int slot) {
	return getInventory()->getSlots()[slot];
}

//获取游戏时命令权限

inline char Player::getPermissions() {
	return *FETCH(char*, this + 2376);//IDA ServerPlayer::setPermissions 22
}

//设置游戏时命令权限

inline void Player::setPermissions(char m) {
	SymCall("?setPermissions@ServerPlayer@@UEAAXW4CommandPermissionLevel@@@Z",
		this, m);
}

//获取游戏时游玩权限

inline char Player::getPermissionLevel() {//IDA Abilities::setPlayerPermissions ?
	return FETCH(char, FETCH(char*, this + 2376) + 1);
}

//设置游戏时游玩权限

inline void Player::setPermissionLevel(char m) {
	SymCall("?setPlayerPermissions@Abilities@@QEAAXW4PlayerPermissionLevel@@@Z",
		this + 2376, m);
}

//获取设备id

inline string Player::getDeviceId() {
	return FETCH(string, this + 8352); //IDA Player::Player  v13 + 8352
}

//获取设备系统类型

inline int Player::getDeviceOS() {
	return FETCH(int, this + 2368);	//IDA ServerNetworkHandler::createNewPlayer  ConnectionRequest::getDeviceOS
}

//发送背包

inline void Player::sendInventroy() {
	SymCall("?sendInventory@ServerPlayer@@UEAAX_N@Z",
		this, true);
}

//刷新区块

inline void Player::resendAllChunks() {
	SymCall("?resendAllChunks@ServerPlayer@@UEAAXXZ", this);
}
