#include <Global.hpp>
#include "BlockAPI.h"
#include "EntityAPI.h"
#include "PlayerAPI.h"
#include "ContainerAPI.h"
#include "ItemAPI.h"
#include "NBTAPI.h"
#include <PlayerInfoAPI.h>
#include <LLAPI.h>
#include <MC/BlockSource.hpp>
#include <MC/Command.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/SimulatedPlayer.hpp>
#include <MC/Level.hpp>
#include <MC/Packet.hpp>

py::dict NavigateResultToObject(const ScriptNavigationResult& res) {
	py::dict obj;
	obj["isFullPath"] = res.mIsFullPath;
	obj["path"] = res.mPath;
	return obj;
}

PlayerClass::PlayerClass(Player* p) : thiz(p) {}

PlayerClass::PlayerClass(const PlayerClass& other) : thiz(other.thiz) {}

string PlayerClass::getName() {
	if (thiz == nullptr)
		return "";
	return thiz->getName();
}

Vec3 PlayerClass::getPos() {
	if (thiz == nullptr)
		return Vec3::ZERO;
	return thiz->getPosition();
}

BlockPos PlayerClass::getBlockPos() {
	if (thiz == nullptr)
		return BlockPos::ZERO;
	return thiz->getBlockPos();
}

int PlayerClass::getDim() {
	if (thiz == nullptr)
		return 0;
	return thiz->getDimensionId();
}

string PlayerClass::getRealName() {
	if (thiz == nullptr)
		return "";
	return thiz->getRealName();
}

string PlayerClass::getXuid() {
	if (thiz == nullptr)
		return "";
	string xuid;
	try {
		xuid = thiz->getXuid();
	}
	catch (...) {
		logger.debug("Fail in getXuid!");
		xuid = PlayerInfo::getXuid(thiz->getRealName());
	}
	return xuid;
}

string PlayerClass::getUuid() {
	if (thiz == nullptr)
		return "";
	string uuid;
	try {
		uuid = thiz->getUuid();
	}
	catch (...) {
		logger.debug("Fail in getUuid!");
		uuid = PlayerInfo::getUUID(thiz->getRealName());
	}
	return uuid;
}

CommandPermissionLevel PlayerClass::getPermLevel() {
	if (thiz == nullptr)
		return {};
	return thiz->getCommandPermissionLevel();
}

GameType PlayerClass::getGameMode() {
	if (thiz == nullptr)
		return {};
	return thiz->getPlayerGameType();
}

float PlayerClass::getSpeed() {
	if (thiz == nullptr)
		return 0.f;
	return thiz->getSpeed();
}

Vec2 PlayerClass::getDirection() {
	if (thiz == nullptr)
		return Vec2::ZERO;
	return thiz->getRotation();
}

int PlayerClass::getMaxHealth() {
	if (thiz == nullptr)
		return 0;
	return thiz->getMaxHealth();
}

int PlayerClass::getHealth() {
	if (thiz == nullptr)
		return 0;
	return thiz->getHealth();
}

bool PlayerClass::getInAir() {
	if (thiz == nullptr)
		return false;
	return !thiz->isOnGround() && !thiz->isInWater();
}

bool PlayerClass::getInWater() {
	if (thiz == nullptr)
		return false;
	return thiz->isInWater();
}

bool PlayerClass::getSneaking() {
	if (thiz == nullptr)
		return false;
	return thiz->isSneaking();
}

string PlayerClass::getUniqueID() {
	if (thiz == nullptr)
		return "";
	return std::to_string(thiz->getUniqueID().id);
}

string PlayerClass::getLangCode() {
	if (thiz == nullptr)
		return "";
	return thiz->getLanguageCode();
}

bool PlayerClass::teleport(const Vec3& pos, int dim) {
	return thiz->teleport(pos, dim);
}

bool PlayerClass::kill() {
	if (thiz == nullptr)
		return false;
	thiz->kill();
	return true;
}

bool PlayerClass::kick(const string& msg) {
	if (thiz == nullptr)
		return false;
	return thiz->kick(msg);
}

bool PlayerClass::sendText(const string& msg, TextType type) {
	if (thiz == nullptr)
		return false;
	return thiz->sendTextPacket(msg, type);
}

bool PlayerClass::talkAs(const string& msg) {
	if (thiz == nullptr)
		return false;
	return thiz->sendTextTalkPacket(msg, nullptr);
}

bool PlayerClass::talkTo(const string& msg, Player* target) {
	if (!target)
		return false;
	if (thiz == nullptr)
		return false;
	return thiz->sendTextTalkPacket(msg, target);
}

bool PlayerClass::rename(const string& name) {
	if (thiz == nullptr)
		return false;
	return thiz->rename(name);
}

bool PlayerClass::transServer(const string& address, short port) {
	if (thiz == nullptr)
		return false;
	return thiz->transferServer(address, port);
}

bool PlayerClass::crash() {
	if (thiz == nullptr)
		return false;
	return thiz->crashClient();
}

bool PlayerClass::hurt(int damage) {
	if (thiz == nullptr)
		return false;
	return thiz->hurtEntity(damage);
}

bool PlayerClass::setOnFire(int time) {
	if (!thiz)
		return false;
	return thiz->setOnFire(time, true);
}

bool PlayerClass::isOP() {
	if (thiz == nullptr)
		return false;
	return thiz->isOperator();
}

bool PlayerClass::setPermLevel(CommandPermissionLevel perm) {
	if (thiz == nullptr)
		return false;
	thiz->setPermissions(perm);
	return true;
}

bool PlayerClass::setGameMode(GameType mode) {
	if (thiz == nullptr)
		return false;
	thiz->setPlayerGameType(mode);
	return true;
}

bool PlayerClass::runcmd(const string& cmd) {
	if (thiz == nullptr)
		return false;
	return thiz->runcmd(cmd);
}

BlockClass PlayerClass::getBlockStandingOn() {
	if (thiz == nullptr)
		return {{}, 0};
	return {thiz->getBlockPosCurrentlyStandingOn(nullptr), thiz->getDimensionId()};
}

ItemClass PlayerClass::getHand() {
	if (thiz == nullptr)
		return nullptr;
	return thiz->getHandSlot();
}

ItemClass PlayerClass::getOffHand() {
	if (thiz == nullptr)
		return nullptr;
	return (ItemStack*)&thiz->getOffhandSlot();
}

ContainerClass PlayerClass::getInventory() {
	if (thiz == nullptr)
		return nullptr;
	return &thiz->getInventory();
}

ContainerClass PlayerClass::getArmor() {
	if (thiz == nullptr)
		return nullptr;
	return &thiz->getArmorContainer();
}

ContainerClass PlayerClass::getEnderChest() {
	if (thiz == nullptr)
		return nullptr;
	return thiz->getEnderChestContainer();
}

std::pair<BlockPos, int> PlayerClass::getRespawnPosition() {
	if (thiz == nullptr)
		return {};
	auto position = thiz->getRespawnPosition();
	if (position.first.y == 32767) {
		auto region = Level::getBlockSource(position.second);
		if (region)
			position.first = region->getHeightmapPos(position.first);
	}
	return position;
}

bool PlayerClass::setRespawnPosition(BlockPos pos, int dim) {
	if (thiz == nullptr)
		return {};
	if (dim < 0)
		return false;
	thiz->setRespawnPosition(pos, dim);
	return true;
}

bool PlayerClass::refreshItems() {
	if (thiz == nullptr)
		return {};
	return thiz->refreshInventory();
}

int PlayerClass::getScore(const string& key) {
	if (thiz == nullptr)
		return {};
	return Global<Scoreboard>->getScore(thiz, key);
}

bool PlayerClass::setScore(const string& key, int value) {
	if (thiz == nullptr)
		return {};
	return Global<Scoreboard>->setScore(thiz, key, value);
}

bool PlayerClass::addScore(const string& key, int value) {
	if (thiz == nullptr)
		return {};
	return Global<Scoreboard>->addScore(thiz, key, value);
}

bool PlayerClass::reduceScore(const string& key, int value) {
	if (thiz == nullptr)
		return {};
	return Global<Scoreboard>->reduceScore(thiz, key, value);
}

bool PlayerClass::deleteScore(const string& key) {
	if (thiz == nullptr)
		return {};
	return ::Global<Scoreboard>->deleteScore(thiz, key);
}

bool PlayerClass::setSidebar(const string& title, std::vector<std::pair<std::string, int>> data, ObjectiveSortOrder order) {
	if (thiz == nullptr)
		return {};
	return thiz->setSidebar(title, data, order);
}

bool PlayerClass::removeSidebar() {
	if (thiz == nullptr)
		return {};
	return thiz->removeSidebar();
}

bool PlayerClass::setBossBar(int64_t uid, const string& name, float percent, BossEventColour colour) {
	if (thiz == nullptr)
		return {};
	if (percent < 0)
		percent = 0;
	else if (percent > 100)
		percent = 100;
	float value = percent / 100;
	thiz->updateBossEvent(uid, name, value, colour); // TODO: test whether it work
	return thiz->sendBossEventPacket(BossEvent::Show, name, value, colour);
}

bool PlayerClass::removeBossBar() {
	if (thiz == nullptr)
		return {};
	return thiz->sendBossEventPacket(BossEvent::Hide, "", 0, BossEventColour::Red); // Remove
}

bool PlayerClass::removeBossBar(int64_t uid) {
	if (thiz == nullptr)
		return {};
	thiz->removeBossEvent(uid);
	return true;
}

bool PlayerClass::addLevel(int value) {
	if (thiz == nullptr)
		return {};
	thiz->addLevels(value);
	return true;
}

int PlayerClass::getLevel() {
	if (thiz == nullptr)
		return {};
	return thiz->getPlayerLevel();
}

void PlayerClass::resetLevel() {
	if (thiz == nullptr)
		return;
	thiz->resetPlayerLevel();
}

int PlayerClass::getXpNeededForNextLevel() {
	if (thiz == nullptr)
		return {};
	return thiz->getXpNeededForNextLevel();
}

bool PlayerClass::addExperience(int value) {
	if (thiz == nullptr)
		return {};
	thiz->addExperience(value);
	return true;
}

bool PlayerClass::sendCustomForm(const string& str, const py::function& cb) {
	if (thiz == nullptr)
		return {};
	Player* p = thiz;
	return thiz->sendCustomFormPacket(str,
		[p, cb](const string& arg) {
			if (LL::isServerStopping())
				return;
			if (arg == "null")
				return;
			PY_TRY;
			py::gil_scoped_acquire gil_;
			auto json = py::module_::import("json");
			cb(PlayerClass(p), json.attr("loads")(arg));
			PY_CATCH;
		});
}

bool PlayerClass::sendSimpleForm(const string& title, const string& content, const vector<string>& buttons, const vector<string>& images, const py::function& cb) {
	if (thiz == nullptr)
		return {};
	if (buttons.size() != images.size())
		throw py::value_error("The number of buttons is not equal to the number of images");
	Player* p = thiz;
	return thiz->sendSimpleFormPacket(title, content, buttons, images,
		[p, cb](int arg) {
			if (LL::isServerStopping())
				return;
			if (arg == -1)
				return;
			PY_TRY;
			py::gil_scoped_acquire gil_;
			cb(PlayerClass(p), arg);
			PY_CATCH;
		});
}

bool PlayerClass::sendModalForm(const string& title, const string& content, const string& button1, const string& button2, const py::function& cb) {
	if (thiz == nullptr)
		return {};
	Player* p = thiz;
	return thiz->sendModalFormPacket(title, content, button1, button2,
		[p, cb](bool arg) {
			if (LL::isServerStopping())
				return;
			PY_TRY;
			py::gil_scoped_acquire gil_;
			cb(PlayerClass(p), arg);
			PY_CATCH;
		});
}

bool PlayerClass::refreshChunks() {
	if (thiz == nullptr)
		return {};
	thiz->resendAllChunks();
	return true;
}

bool PlayerClass::giveItem(const ItemClass& item) {
	if (thiz == nullptr)
		return false;
	if (!item.thiz)
		return false;
	return thiz->giveItem(item.thiz);
}

bool PlayerClass::clearItem(const string& type) {
	if (thiz == nullptr)
		return {};
	return thiz->clearItem(type);
}

bool PlayerClass::isSprinting() {
	if (thiz == nullptr)
		return {};
	return thiz->isSprinting();
}

bool PlayerClass::setSprinting(bool b) {
	if (thiz == nullptr)
		return {};
	thiz->setSprinting(b);
	return true;
}

bool PlayerClass::setExtraData(string key, const py::object& value) {
	if (thiz == nullptr)
		return {};
	if (key.empty())
		return false;
	player_data[thiz->getRealName() + "-" + key] = value;
	return true;
}

py::object PlayerClass::getExtraData(string key) {
	if (thiz == nullptr)
		return {};
	if (key.empty())
		return {};
	auto res = player_data.find(thiz->getRealName() + "-" + key);
	if (res == player_data.end())
		return {};
	else
		return res->second;
}

bool PlayerClass::delExtraData(string key) {
	if (thiz == nullptr)
		return {};
	if (key.empty())
		return false;
	return player_data.erase(thiz->getRealName() + "-" + key);
}

NBTClass PlayerClass::getNbt() {
	if (thiz == nullptr)
		return {};
	return thiz->getNbt();
}

bool PlayerClass::setNbt(const NBTClass& nbt) {
	if (thiz == nullptr)
		return {};
	if (!nbt.thiz)
		return {};
	return thiz->setNbt(nbt.thiz->asCompoundTag());
}

bool PlayerClass::addTag(const string& tag) {
	if (thiz == nullptr)
		return {};
	return thiz->addTag(tag);
}

bool PlayerClass::hasTag(const string& tag) {
	if (thiz == nullptr)
		return {};
	return thiz->hasTag(tag);
}

bool PlayerClass::removeTag(const string& tag) {
	if (thiz == nullptr)
		return {};
	return thiz->removeTag(tag);
}

vector<string> PlayerClass::getAllTags() {
	if (thiz == nullptr)
		return {};
	return thiz->getAllTags();
}

EntityClass PlayerClass::getEntityFromViewVector(float maxDistance) {
	if (thiz == nullptr)
		return nullptr;
	return EntityClass(thiz->getActorFromViewVector(maxDistance));
}

BlockClass PlayerClass::getBlockFromViewVector(bool includeLiquid, bool solidOnly, float maxDistance, bool ignoreBorderBlocks, bool fullOnly) {
	if (thiz == nullptr)
		return BlockInstance::Null;
	return thiz->getBlockFromViewVector(includeLiquid, solidOnly, maxDistance, ignoreBorderBlocks, fullOnly);
}

bool PlayerClass::isSimulatedPlayer() {
	if (thiz == nullptr)
		return false;
	return thiz->isSimulatedPlayer();
}

SimulatedPlayer* PlayerClass::asSimulatedPlayer() {
	if (thiz->isSimulatedPlayer())
		return static_cast<SimulatedPlayer*>(thiz);
	return nullptr;
}

bool PlayerClass::simulateSneak() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateSneak();
}

bool PlayerClass::simulateAttack() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateAttack();
}

bool PlayerClass::simulateAttack(const EntityClass& target) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	Actor* actor = target.thiz;
	if (!actor)
		return {};
	return sp->simulateAttack(actor);
}

bool PlayerClass::simulateDestory() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateDestory();
}

bool PlayerClass::simulateDisconnect() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateDisconnect();
	return true;
}

bool PlayerClass::simulateInteract() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateInteract();
}

bool PlayerClass::simulateInteract(const EntityClass& target) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	Actor* actor = target.thiz;
	if (!actor)
		return {};
	return sp->simulateInteract(*actor);
}

bool PlayerClass::simulateJump() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateJump();
}

bool PlayerClass::simulateLocalMove(const Vec3& target, float speed) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateLocalMove(target, speed);
	return true;
}

bool PlayerClass::simulateWorldMove(const Vec3& target, float speed) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateWorldMove(target, speed);
	return true;
}

bool PlayerClass::simulateMoveTo(const Vec3& target, float speed) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateMoveToLocation(target, speed);
	return true;
}

bool PlayerClass::simulateLookAt(const EntityClass& target) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateLookAt(*target.thiz);
	return true;
}

bool PlayerClass::simulateLookAt(const BlockPos& pos) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateLookAt(pos);
	return true;
}

bool PlayerClass::simulateLookAt(const Vec3& pos) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateLookAt(pos);
	return true;
}

bool PlayerClass::simulateSetBodyRotation(float rotation) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateSetBodyRotation(rotation);
	return true;
}

py::dict PlayerClass::simulateNavigateTo(const EntityClass& target, float speed) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	Actor* actor = target.thiz;
	if (!actor)
		return {};
	auto res = sp->simulateNavigateToEntity(*actor, speed);
	return NavigateResultToObject(res);
}

py::dict PlayerClass::simulateNavigateTo(const Vec3& pos, float speed) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	auto res = sp->simulateNavigateToLocation(pos, speed);
	return NavigateResultToObject(res);
}

bool PlayerClass::simulateNavigateTo(vector<Vec3> path, float speed) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateNavigateToLocations(std::move(path), speed);
	return true;
}

bool PlayerClass::simulateUseItem() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateUseItem();
}

bool PlayerClass::simulateUseItem(int slot) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateUseItemInSlot(slot);
}

bool PlayerClass::simulateUseItem(const ItemClass& item) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateUseItem(*item.thiz);
}

bool PlayerClass::simulateUseItem(const ItemClass& item, const BlockPos& pos, ScriptFacing face, const Vec3& relativePos) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateUseItemOnBlock(*item.thiz, pos, face, relativePos);
}

bool PlayerClass::simulateUseItem(int slot, const BlockPos& pos, ScriptFacing face, const Vec3& relativePos) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateUseItemInSlotOnBlock(slot, pos, face, relativePos);
}

bool PlayerClass::simulateStopDestroyingBlock() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateStopDestroyingBlock();
	return true;
}

bool PlayerClass::simulateStopInteracting() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateStopInteracting();
	return true;
}

bool PlayerClass::simulateStopMoving() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateStopMoving();
	return true;
}

bool PlayerClass::simulateStopUsingItem() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	sp->simulateStopUsingItem();
	return true;
}

bool PlayerClass::simulateStopSneaking() {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateStopSneaking();
}

bool PlayerClass::simulateInteract(const BlockPos& pos, ScriptFacing face) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateInteract(pos, face);
}

bool PlayerClass::simulateDestory(const BlockPos& pos, ScriptFacing face) {
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateDestroyBlock(pos, face);
}

string PlayerClass::getIP() {
	if (thiz == nullptr)
		return {};
	return thiz->getNetworkIdentifier()->getIP();
}

int PlayerClass::getAvgPing() {
	if (thiz == nullptr)
		return {};
	return thiz->getAvgPing();
}

int PlayerClass::getAvgPacketLoss() {
	if (thiz == nullptr)
		return {};
	return 0;
}

string PlayerClass::getOs() {
	if (thiz == nullptr)
		return {};
	return thiz->getDeviceName();
}

string PlayerClass::getClientId() {
	if (thiz == nullptr)
		return {};
	return thiz->getClientId();
}

bool PlayerClass::removeItem(int inventory_id, int count) {
	if (thiz == nullptr)
		return {};
	Container& container = thiz->getInventory();
	if (inventory_id > container.getSize())
		return false;
	container.removeItem(inventory_id, count);
	thiz->sendInventory(true);
	return true;
}

class SetActorDataPacket : public Packet {
public:
	ActorRuntimeID rid;
	std::vector<std::unique_ptr<DataItem>> items;

	MCAPI virtual MinecraftPacketIds getId() const;
	MCAPI virtual std::string getName() const;
	MCAPI virtual void write(BinaryStream&) const;

private:
	MCAPI virtual StreamReadResult _read(ReadOnlyBinaryStream&);
};

bool PlayerClass::setHeadShow(const string& name) {
	if (thiz == nullptr)
		return {};
	SetActorDataPacket packet;
	packet.rid = thiz->getRuntimeID();
	if (packet.rid.id == 0)
		return false;
	packet.items.emplace_back(DataItem::create(ActorDataIDs::NAMETAG, name));
	thiz->sendNetworkPacket(packet);
	return true;
}
