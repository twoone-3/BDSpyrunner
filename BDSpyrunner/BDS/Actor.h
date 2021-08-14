#pragma once
#include "../tool.h"
#include "Math.h"
#include "span.h"

struct BlockSource;
struct ItemStack;
struct Level;
struct Tag;
struct NetworkIdentifier;
struct Container;
struct Actor {
	//获取生物名称信息
	string getNameTag();
	//获取生物当前所处维度ID
	int getDimensionId();
	//获取生物当前所在坐标
	Vec3* getPos();
	//获取生物之前所在坐标
	Vec3* getPosOld();
	//是否已移除
	bool isRemoved();
	//是否悬空
	bool isStand();
	//取方块源
	BlockSource* getRegion();
	ItemStack* getArmor(int slot);
	//获取实体类型
	unsigned getEntityTypeId();
	//获取查询用ID
	VA getUniqueID();
	//获取实体类型
	string getEntityTypeName();
	//更新属性
	VA updateAttrs();
	//获取地图信息
	Level* getLevel();
	//添加一个状态
	VA addEffect(VA ef);
	//获取生命值
	int getHealth();
	int getMaxHealth();
	void setHealth(int value);
	void setMaxHealth(int value);
	//获取副手
	ItemStack* getOffHand();
	Tag* save();
	//设置大小
	void setSize(float f1, float f2);
	//是否潜行
	bool isSneaking();
	//获取状态列表
	auto getAllEffects();
	//传送
	void teleport(Vec3* target, int did);
	//新增标签
	bool addTag(const string& str);
	//移除标签
	bool removeTag(const string& str);
	//获取标签
	span<string> getTags();
};
struct Mob : Actor {};
struct Player : Mob {
	string getUuid();
	//发送数据包
	void sendPacket(VA pkt);
	//根据地图信息获取玩家xuid
	string& getXuid();
	//重设服务器玩家名
	void setName(const string& name);
	//获取网络标识符
	NetworkIdentifier* getClientId();
	//获取背包
	Container* getInventory();
	//获取装备容器
	Container* getArmorContainer();
	//获取末影箱
	Container* getEnderChestContainer();
	//设置一个装备
	VA setArmor(int i, ItemStack* item);
	//设置副手
	VA setOffhandSlot(ItemStack* item);
	//添加一个物品
	void addItem(ItemStack* item);
	//增加等级
	void addLevel(int level);
	//获取当前选中的框位置
	int getSelectedItemSlot();
	//获取当前物品
	ItemStack* getSelectedItem();
	//获取背包物品
	ItemStack* getInventoryItem(int slot);
	//获取游戏时命令权限
	char getPermissions();
	//设置游戏时命令权限
	void setPermissions(char m);
	//获取游戏时游玩权限
	char getPermissionLevel();
	//设置游戏时游玩权限
	void setPermissionLevel(char m);
	//获取设备id
	string getDeviceId();
	//获取设备系统类型
	int getDeviceOS();
	//发送背包
	void sendInventroy();
	//刷新区块
	void resendAllChunks();
	//断开连接
	//void disconnect() {
	//	SymCall("?disconnect@ServerPlayer@@QEAAXXZ",this);
	//}
};
