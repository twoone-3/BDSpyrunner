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
//ע������
void registerCommand(const string& name, const string& desc, const py::function& cb, CommandPermissionLevel perm = GameMasters);
//��ȡ���
PlayerClass getPlayer(const string& name);
//��ȡ����б�
py::list getOnlinePlayers();
//��ȡʵ���б�
py::list getAllEntities();
//��ȡ����
BlockClass getBlock(const BlockPos& pos, int dim);
//���÷���
bool setBlock(const BlockPos& pos, int dim, const string& name, int tile_data);
//���÷���
bool setBlock(const BlockPos& pos, int dim, const BlockClass& b);
//��ָ���ص��ȡNBT�ṹ����
NbtClass getStructure(const BlockPos& pos1, const BlockPos& pos2, int dim,
	bool ignore_entities = true, bool ignore_blocks = false);
//��NBT�ṹ���ݵ����ṹ��ָ���ص�
bool setStructure(const NbtClass& nbt, const BlockPos& pos, int dim,
	Mirror mir, Rotation rot);
//������ը
void explode(const Vec3& pos, int dim, float power, bool destroy, float range, bool fire);
//������Ʒ
void spawnItem(ItemClass& item, Vec3 pos, int dim);
//������������
void setSignBlockMessage(BlockPos pos, int dim,const string& text );
//�Ƿ�Ϊʷ��ķ����
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
