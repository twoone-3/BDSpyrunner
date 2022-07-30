#include "McAPI.h"
#include <DynamicCommandAPI.h>

#include <API/EntityAPI.h>
#include <API/PlayerAPI.h>
#include <API/BlockAPI.h>
#include <API/NBTAPI.h>
#include <API/ItemAPI.h>
#include <API/EventAPI.h>
#include <API/ScoreboardAPI.h>

#include <MC/Level.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/SimulatedPlayer.hpp>
#include <MC/Spawner.hpp>
#include <MC/SignBlockActor.hpp>
#include <LoggerAPI.h>

namespace mc {
void setListener(const string& event_name, const py::function& cb) {
	auto event_code = magic_enum::enum_cast<EventCode>(event_name);
	if (!event_code)
		throw py::value_error("Invalid event name " + event_name);
	//如果监听器未启用，则启用
	if (listeners.find(event_code.value()) == listeners.end())
		EnableEventListener(event_code.value());
	//添加回调函数
	listeners[event_code.value()].push_back(cb);
}

void registerCommand(const string& name, const string& desc, const py::function& cb, CommandPermissionLevel perm) {
	using ParamType = DynamicCommand::ParameterType;
	auto command = DynamicCommand::createCommand(name, desc, perm);
	command->addOverload();
	command->setCallback(
	    [cb](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
		    PY_TRY;
		    py::gil_scoped_acquire gil_;
		    cb(PlayerClass((Player*)origin.getPlayer()), results);
		    PY_CATCH;
	    });
	DynamicCommand::setup(std::move(command));
}

PlayerClass getPlayer(const string& target /*name or xuid*/) {
	Player* p = Global<Level>->getPlayer(target);
	if (p == nullptr)
		throw std::runtime_error("Failed to find player");
	return p;
}

py::list getOnlinePlayers() {
	py::list l;
	for (auto p : Level::getAllPlayers()) {
		l.append(EntityClass(p));
	}
	return l;
}

py::list getAllEntities() {
	py::list l;
	for (auto p : Level::getAllEntities()) {
		l.append(EntityClass(p));
	}
	return l;
}

BlockClass getBlock(const BlockPos& pos, int dim) {
	if (Global<Level> == nullptr)
		throw std::runtime_error("Level is not loaded");
	Block* b = Level::getBlockEx(pos, dim);
	if (b == nullptr)
		throw py::value_error("Failed to find block");
	return BlockInstance::createBlockInstance(b, pos, dim);
}

bool setBlock(const BlockPos& pos, int dim, const string& name, int tile_data) {
	return Level::setBlock(pos, dim, name, tile_data);
}

bool setBlock(const BlockPos& pos, int dim, const BlockClass& b) {
	return Level::setBlock(pos, dim, const_cast<BlockClass&>(b).thiz.getBlock());
}

NBTClass getStructure(const BlockPos& pos1, const BlockPos& pos2, int dim, bool ignore_entities, bool ignore_blocks) {
	auto st = StructureTemplate::fromWorld("name", dim, pos1, pos2, ignore_entities, ignore_blocks);
	return st.save();
}

bool setStructure(const NBTClass& nbt, const BlockPos& pos, int dim, Mirror mir, Rotation rot) {
	auto st = StructureTemplate::fromTag("name", *nbt.thiz->asCompoundTag());
	/*for (int x = 0; x != size.x; ++x) {
	    for (int y = 0; y != size.y; ++y) {
	        for (int z = 0; z != size.z; ++z) {
	            BlockPos pos{ x, y, z };
	            bs->neighborChanged(pos, pos);
	        }
	    }
	}*/
	return st.toWorld(dim, pos, mir, rot);
}

void explode(const Vec3& pos, int dim, float power, bool destroy, float range, bool fire) {
	BlockSource* bs = Level::getBlockSource(dim);
	if (!bs)
		throw py::value_error("Unknown dimension ID");
	Global<Level>->explode(*bs, nullptr, pos, power, fire, destroy, range, true);
}

void spawnItem(ItemClass& item, Vec3 pos, int dim) {
	Global<Level>->getSpawner().spawnItem(pos, dim, item.thiz);
}

void setSignBlockMessage(BlockPos pos, int dim, const string& text) {
	BlockSource* bs = Level::getBlockSource(dim);
	if (bs == nullptr)
		throw py::value_error("Unknown dimension ID");
	auto sign = static_cast<SignBlockActor*>(Global<Level>->getBlockEntity(pos, dim));
	sign->setMessage(text, text);
	sign->setChanged();
}

int IsSlimeChunk(unsigned x, unsigned z) {
	unsigned mt0 = (x * 0x1F1F1F1F) ^ z;
	unsigned mt1 = (1812433253u * (mt0 ^ (mt0 >> 30u)) + 1);
	unsigned mt2 = mt1;
	for (unsigned i = 2; i < 398; ++i) {
		mt2 = (1812433253u * (mt2 ^ (mt2 >> 30u)) + i);
	}
	unsigned k = (mt0 & 0x80000000u) + (mt1 & 0x7FFFFFFFu);
	mt0 = mt2 ^ (k >> 1u);
	if (k & 1)
		mt0 ^= 2567483615u;
	mt0 ^= (mt0 >> 11u);
	mt0 ^= (mt0 << 7u) & 0x9D2C5680u;
	mt0 ^= (mt0 << 15u) & 0xEFC60000u;
	mt0 ^= (mt0 >> 18u);
	return !(mt0 % 10);
}

EntityClass spawnMob(const Vec3& pos, int dim, const string& type) {
	return Level::spawnMob(pos, dim, type);
}

EntityClass cloneMob(const Vec3& pos, int dim, const EntityClass& ac) {
	return Level::cloneMob(pos, dim, ac.thiz);
}

PlayerClass spawnSimulatedPlayer(const string& name, BlockPos& pos, int dim) {
	return SimulatedPlayer::create(name, pos, dim);
}

bool spawnParticle(const string& type, const Vec3& pos, int dim) {
	Global<Level>->spawnParticleEffect(type, pos, Global<Level>->getDimension(dim));
	return true;
}

bool sendCmdOutput(const string& output) {
	using namespace std;
	string finalOutput(output);
	finalOutput += "\r\n";

	SymCall("??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
	    ostream&,
	    ostream&,
	    const char*,
	    size_t)(cout, finalOutput.c_str(), finalOutput.size());
	return true;
}

bool setMaxNumPlayers(int num) {
	int back = Global<ServerNetworkHandler>->setMaxNumPlayers(num);
	Global<ServerNetworkHandler>->updateServerAnnouncement();
	return back == 0;
}

ObjectiveClass getDisplayObjective(const string& slot) {
	return (Objective*)Global<Scoreboard>->getDisplayObjective(slot);
}

ObjectiveClass clearDisplayObjective(const string& slot) {
	return Global<Scoreboard>->clearDisplayObjective(slot);
}

ObjectiveClass getScoreObjective(const string& name) {
	return Global<Scoreboard>->getObjective(name);
}

ObjectiveClass newScoreObjective(const string& name, const string& display) {
	return Scoreboard::newObjective(name, display);
}

bool removeScoreObjective(const string& name) {
	auto obj = Global<Scoreboard>->getObjective(name);
	if (!obj)
		return false;
	Global<Scoreboard>->removeObjective(obj);
	return true;
}

vector<ObjectiveClass> getAllScoreObjectives() {
	vector<ObjectiveClass> res;
	for (auto x : Global<Scoreboard>->getObjectives()) {
		res.push_back(ObjectiveClass(const_cast<Objective*>(x)));
	}
	return res;
}

}// namespace mc
