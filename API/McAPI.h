#pragma once
#include <Global.hpp>
struct PlayerClass;
struct EntityClass;
struct NbtClass;
struct BlockClass;
struct ItemClass;
struct ObjectiveClass;

namespace mc {
void setListener(const string& event_name, const py::function& cb);
//注册命令
void registerCommand(const string& name, const string& desc, const py::function& cb, CommandPermissionLevel perm = GameMasters);
//获取玩家
PlayerClass getPlayer(const string& name);
//获取玩家列表
py::list getOnlinePlayers();
//获取实体列表
py::list getAllEntities();
//获取方块
BlockClass getBlock(const BlockPos& pos, int dim);
//设置方块
bool setBlock(const BlockPos& pos, int dim, const string& name, int tile_data);
//设置方块
bool setBlock(const BlockPos& pos, int dim, const BlockClass& b);
//从指定地点获取NBT结构数据
NbtClass getStructure(const BlockPos& pos1, const BlockPos& pos2, int dim,
	bool ignore_entities = true, bool ignore_blocks = false);
//从NBT结构数据导出结构到指定地点
bool setStructure(const NbtClass& nbt, const BlockPos& pos, int dim,
	Mirror mir, Rotation rot);
//产生爆炸
void explode(const Vec3& pos, int dim, float power, bool destroy, float range, bool fire);
//生成物品
void spawnItem(ItemClass& item, Vec3 pos, int dim);
//设置牌子文字
void setSignBlockMessage(BlockPos pos, int dim,const string& text );
//是否为史莱姆区块
int IsSlimeChunk(unsigned x, unsigned z);

EntityClass cloneMob(const Vec3& pos, int dim, const EntityClass& ac);

PlayerClass spawnSimulatedPlayer(const string& name, BlockPos& pos, int dim);

bool spawnParticle(const string& type, const Vec3& pos, int dim);

bool sendCmdOutput(const string& output);

bool setMaxNumPlayers(int num);

ObjectiveClass getDisplayObjective(const string& slot);
ObjectiveClass clearDisplayObjective(const string& slot);
ObjectiveClass getScoreObjective(const string& name);
ObjectiveClass newScoreObjective(const string& name, const string& display);
bool removeScoreObjective(const string& name);
vector<ObjectiveClass> getAllScoreObjectives();
}
