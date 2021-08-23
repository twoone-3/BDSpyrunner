#pragma once
#include <vector>
#include "tool.h"
#include "Position.h"
#include "span.h"

struct BlockSource;
struct ItemStack;
struct Level;
struct Tag;
struct NetworkIdentifier;
struct Container;
struct ScorePacketInfo;
struct Actor {
	//获取生物名称信息
	std::string getNameTag();
	//设置生物名称信息
	void setNameTag(const std::string&);
	//设置生物名称是否可见
	void setNameTagVisible(bool visible);
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
	uintptr_t getUniqueID();
	//获取实体类型
	std::string getEntityTypeName();
	//更新属性
	uintptr_t updateAttrs();
	//获取地图信息
	Level* getLevel();
	//添加一个状态
	uintptr_t addEffect(uintptr_t ef);
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
	bool addTag(const std::string& str);
	//移除标签
	bool removeTag(const std::string& str);
	//获取标签
	span<std::string> getTags();
	//杀死实体
	void kill();
};
struct Mob : Actor {};
struct Player : Mob {
	std::string getUuid();
	//根据地图信息获取玩家xuid
	std::string& getXuid();
	//获取网络标识符
	NetworkIdentifier* getClientId();
	//获取背包
	Container* getInventory();
	//获取装备容器
	Container* getArmorContainer();
	//获取末影箱
	Container* getEnderChestContainer();
	//设置一个装备
	uintptr_t setArmor(int i, ItemStack* item);
	//设置副手
	uintptr_t setOffhandSlot(ItemStack* item);
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
	std::string getDeviceId();
	//获取设备系统类型
	int getDeviceOS();
	//发送背包
	void sendInventroy();
	//刷新区块
	void resendAllChunks();
	//崩溃客户端
	void crash();
	//发送数据包
	void sendPacket(uintptr_t pkt);
	unsigned sendModalFormRequestPacket(const std::string& str);
	void sendTransferPacket(const std::string& address, unsigned short port);
	void sendDisconnectPacket(const std::string& msg);
	void sendTextPacket(int mode, const std::string& msg);
	void sendCommandRequestPacket(const std::string& cmd);
	void sendBossEventCodePacket(std::string name, float per, int eventtype);
	void sendsetDisplayObjectivePacket(const std::string& title, const std::string& name = "name");
	void sendSetScorePacket(char type, const std::vector<ScorePacketInfo>& slot);
};
