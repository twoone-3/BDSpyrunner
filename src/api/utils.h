#pragma once
#include <MC/Command.hpp>
#include <MC/StructureTemplate.hpp>

#include "../py_utils.h"

struct PlayerClass;
struct EntityClass;
struct NBTClass;
struct BlockClass;
struct ItemClass;
struct ObjectiveClass;

namespace mc {
void setListener(const string& event_name, const py::function& cb);

void registerCommand(const string& name, const string& desc,
                     const py::function& cb,
                     CommandPermissionLevel perm = GameMasters);

PlayerClass getPlayer(const string& name);

py::list getOnlinePlayers();

py::list getAllEntities();

BlockClass getBlock(const BlockPos& pos, int dim);

bool setBlock(const BlockPos& pos, int dim, const string& name, int tile_data);

bool setBlock(const BlockPos& pos, int dim, const BlockClass& b);
//从指定地点获取NBT结构数据
NBTClass getStructure(const BlockPos& pos1, const BlockPos& pos2, int dim,
                      bool ignore_entities = true, bool ignore_blocks = false);
//从NBT结构数据导出结构到指定地点
bool setStructure(const NBTClass& nbt, const BlockPos& pos, int dim,
                  Mirror mir = None_15, Rotation rot = None_14);
//产生爆炸
void explode(const Vec3& pos, int dim, float power, bool destroy, float range,
             bool fire);
//生成物品
void spawnItem(ItemClass& item, Vec3 pos, int dim);
//设置牌子文字
void setSignBlockMessage(BlockPos pos, int dim, const string& text);
//是否为史莱姆区块
int IsSlimeChunk(unsigned x, unsigned z);

EntityClass spawnMob(const Vec3& pos, int dim, const string& type);

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

}  // namespace mc
