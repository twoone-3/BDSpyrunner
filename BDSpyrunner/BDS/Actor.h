#pragma once
#include "../pch.h"
#include "Level.h"
#include "Math.h"
#include "Block.h"
#include "Item.h"
#include "NBT.h"
struct Actor {
	//获取生物名称信息
	string getNameTag() {
		return SYMCALL<string&>("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this);
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
		return SYMCALL<ItemStack*>("?getArmor@Actor@@UEBAAEBVItemStack@@W4ArmorSlot@@@Z",
			this, slot);
	}
	//获取实体类型
	unsigned getEntityTypeId() {
		return FETCH(unsigned, this + 968);
	}
	//获取查询用ID
	VA getUniqueID() {
		return SYMCALL<VA>("?getUniqueID@Actor@@QEBAAEBUActorUniqueID@@XZ", this);
	}
	//获取实体类型
	string getEntityTypeName() {
		string type;
		SYMCALL<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
			&type, getEntityTypeId());
		return type;
	}
	//更新属性
	VA updateAttrs() {
		return SYMCALL<VA>("?_sendDirtyActorData@Actor@@QEAAXXZ", this);
	}
	//获取地图信息
	Level* getLevel() {//IDA Mob::die 143
		return FETCH(Level*, this + 856);
	}
	VA getAttribute() {
		return FETCH(VA, this + 1144);
	}
	//添加一个状态
	VA addEffect(VA ef) {
		return SYMCALL<VA>("?addEffect@Actor@@QEAAXAEBVMobEffectInstance@@@Z", this, ef);
	}
	//获取生命值
	int getHealth() {
		return SYMCALL<int>("?getHealth@Actor@@QEBAHXZ", this);
	}
	int getMaxHealth() {
		return SYMCALL<int>("?getMaxHealth@Actor@@QEBAHXZ", this);
	}
	void setHealth(int value, int max) {
		VA hattr = ((*(VA(__fastcall**)(Actor*, void*))(*(VA*)this + 1552))(
			this, SYM("?HEALTH@SharedAttributes@@2VAttribute@@B")));
		FETCH(int, hattr + 132) = value;
		FETCH(int, hattr + 128) = max;
		//SYMCALL("?_setDirty@AttributeInstance@@AEAAXXZ", hattr);
	}
	//获取副手
	ItemStack* getOffHand() {
		return SYMCALL<ItemStack*>("?getOffhandSlot@Actor@@QEBAAEBVItemStack@@XZ", this);
	}
	Tag* save() {
		Tag* t = newTag(Compound);
		SYMCALL("?save@Actor@@UEAA_NAEAVCompoundTag@@@Z", this, t);
		return t;
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
		SYMCALL<string&>("?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			this + 2832, &p);
		return p;
	}
	//发送数据包
	void sendPacket(VA pkt) {
		return SYMCALL<void>("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z",
			this, pkt);
	}
	//根据地图信息获取玩家xuid
	string& getXuid() {
		return SYMCALL<string&>("?getPlayerXUID@Level@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUUID@mce@@@Z",
			getLevel(), this + 2832);
	}
	//重设服务器玩家名
	void setName(const string& name) {
		SYMCALL("?setName@Player@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
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
		return SYMCALL<VA>("?setArmor@ServerPlayer@@UEAAXW4ArmorSlot@@AEBVItemStack@@@Z", this, i, item);
	}
	//设置副手
	VA setOffhandSlot(ItemStack* item) {
		return SYMCALL<VA>("?setOffhandSlot@Player@@UEAAXAEBVItemStack@@@Z", this, item);
	}
	//添加一个物品
	void addItem(ItemStack* item) {
		SYMCALL<VA>("?addItem@@YAXAEAVPlayer@@AEAVItemStack@@@Z", this, item);
	}
	//增加等级
	void addLevel(const int level) {
		SYMCALL("?addLevels@Player@@UEAAXH@Z", this, level);
	}
	//获取当前选中的框位置
	int getSelectedItemSlot() {//IDA Player::getSelectedItem 12
		return FETCH(unsigned, FETCH(VA, this + 3040) + 16);
	}
	//获取当前物品
	ItemStack* getSelectedItem() {
		return SYMCALL<ItemStack*>("?getSelectedItem@Player@@QEBAAEBVItemStack@@XZ", this);
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
		SYMCALL("?setPermissions@ServerPlayer@@UEAAXW4CommandPermissionLevel@@@Z",
			this, m);
	}
	//获取游戏时游玩权限
	char getPermissionLevel() {//IDA Abilities::setPlayerPermissions ?
		return FETCH(char, FETCH(char*, this + 2224) + 1);
	}
	//设置游戏时游玩权限
	void setPermissionLevel(char m) {
		SYMCALL("?setPlayerPermissions@Abilities@@QEAAXW4PlayerPermissionLevel@@@Z",
			this + 2224, m);
	}
	//发送背包
	void sendInventroy() {
		SYMCALL("?sendInventory@ServerPlayer@@UEAAX_N@Z",
			this, true);
	}
	//刷新区块
	void resendAllChunks() {
		SYMCALL("?resendAllChunks@ServerPlayer@@UEAAXXZ",this);
	}
	//断开连接
	//void disconnect() {
	//	SYMCALL("?disconnect@ServerPlayer@@QEAAXXZ",this);
	//}
	//传送
	void teleport(Vec3* target, int did) {
		char mem[128];
		SYMCALL("?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z",
			&mem, this, target, 0, did, 0, 0, 15);
		SYMCALL("?applyTarget@TeleportCommand@@SAXAEAVActor@@VTeleportTarget@@@Z",
			this, &mem);
	}
};
