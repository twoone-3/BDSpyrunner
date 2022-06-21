#include <Global.hpp>
#include "BlockAPI.h"
#include "EntityAPI.h"
#include "PlayerAPI.h"
#include "McAPI.h"
#include "ContainerAPI.h"
#include "ItemAPI.h"
#include "GuiAPI.h"
#include "NbtAPI.h"
#include <MC/Player.hpp>
#include <MC/NetworkIdentifier.hpp>
#include <MC/Actor.hpp>
#include <MC/Container.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/Scoreboard.hpp>
#include <MC/Objective.hpp>
#include <MC/ScoreboardId.hpp>
#include <MC/ListTag.hpp>
#include <MC/CompoundTag.hpp>
#include <MC/SimulatedPlayer.hpp>
#include <MC/BlockSource.hpp>
#include <PlayerInfoAPI.h>
#include <unordered_map>
#include <algorithm>
using namespace std;

py::dict NavigateResultToObject(const ScriptNavigationResult& res) {
	py::dict obj;
	obj["isFullPath"] = res.mIsFullPath;
	obj["path"] = res.mPath;
	return obj;
}

PlayerClass::PlayerClass(Player* p) : thiz(p) {}

string PlayerClass::getName() {
	if (!thiz)
		return {};
	return thiz->getName();
}

Vec3 PlayerClass::getPos() {
	if (!thiz)
		return {};
	return thiz->getPosition();
}

BlockPos PlayerClass::getBlockPos() {
	if (!thiz)
		return {};
	return thiz->getBlockPos();
}

int PlayerClass::getDim() {
	if (!thiz)
		return {};
	return thiz->getDimensionId();
}

string PlayerClass::getRealName() {
	if (!thiz)
		return {};
	return thiz->getRealName();
}

string PlayerClass::getXuid() {
	if (!thiz)
		return {};
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
	if (!thiz)
		return {};
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
	if (!thiz)
		return {};
	return thiz->getCommandPermissionLevel();
}

GameType PlayerClass::getGameMode() {
	if (!thiz)
		return {};
	return thiz->getPlayerGameType();
}

float PlayerClass::getSpeed() {
	if (!thiz)
		return {};
	return thiz->getSpeed();
}

Vec2 PlayerClass::getDirection() {
	if (!thiz)
		return Vec2(0, 0);
	return thiz->getRotation();
}

int PlayerClass::getMaxHealth() {
	if (!thiz)
		return {};
	return thiz->getMaxHealth();
}

int PlayerClass::getHealth() {
	if (!thiz)
		return {};
	return thiz->getHealth();
}

bool PlayerClass::getInAir() {
	if (!thiz)
		return {};
	return !thiz->isOnGround() && !thiz->isInWater();
}

bool PlayerClass::getInWater() {
	if (!thiz)
		return {};
	return thiz->isInWater();
}

bool PlayerClass::getSneaking() {
	if (!thiz)
		return {};
	return thiz->isSneaking();
}

string PlayerClass::getUniqueID() {
	if (!thiz)
		return {};
	else
		return std::to_string(thiz->getUniqueID().id);
}

string PlayerClass::getLangCode() {
	if (!thiz)
		return {};
	return thiz->getLanguageCode();
}

bool PlayerClass::teleport(const Vec3& pos, int dim) {
	return thiz->teleport(pos, dim);
}

bool PlayerClass::kill() {
	if (!thiz)
		return {};
	thiz->kill();
	return true;
}

bool PlayerClass::kick(const string& msg) {
	if (!thiz)
		return {};
	return thiz->kick(msg);
}

bool PlayerClass::tell(const string& msg, TextType type) {
	if (!thiz)
		return {};
	return thiz->sendTextPacket(msg, type);
}

bool PlayerClass::talkAs(const string& msg) {
	if (!thiz)
		return {};
	return thiz->sendTextTalkPacket(msg, nullptr);
}

bool PlayerClass::talkTo(const string& msg, Player* target) {
	if (!target)
		return false;
	if (!thiz)
		return {};
	return thiz->sendTextTalkPacket(msg, target);
}

bool PlayerClass::rename(const string& name) {
	if (!thiz)
		return {};
	return thiz->rename(name);
}

bool PlayerClass::transServer(const string& address, short port) {
	if (!thiz)
		return {};
	return thiz->transferServer(address, port);
}

bool PlayerClass::crash() {
	if (!thiz)
		return {};
	return thiz->crashClient();
}

bool PlayerClass::hurt(int damage) {
	if (!thiz)
		return {};
	return thiz->hurtEntity(damage);
}

bool PlayerClass::isOP() {
	if (!thiz)
		return {};
	return thiz->isOperator();
}

bool PlayerClass::setPermLevel(CommandPermissionLevel perm) {
	if (!thiz)
		return {};
	thiz->setPermissions(perm);
	return true;
}

bool PlayerClass::setGameMode(GameType mode) {
	if (!thiz)
		return {};
	thiz->setPlayerGameType(mode);
	return true;
}

bool PlayerClass::runcmd(const string& cmd) {
	if (!thiz)
		return {};
	return thiz->runcmd(cmd);
}

BlockClass PlayerClass::getBlockStandingOn() {
	if (!thiz)
		return {{}, 0};
	return {thiz->getBlockPosCurrentlyStandingOn(nullptr), thiz->getDimensionId()};
}

ItemClass PlayerClass::getHand() {
	if (!thiz)
		return nullptr;
	return thiz->getHandSlot();
}

ItemClass PlayerClass::getOffHand() {
	if (!thiz)
		return nullptr;
	return (ItemStack*)&thiz->getOffhandSlot();
}

ContainerClass PlayerClass::getInventory() {
	if (!thiz)
		return nullptr;
	return &thiz->getInventory();
}

ContainerClass PlayerClass::getArmor() {
	if (!thiz)
		return nullptr;
	return &thiz->getArmorContainer();
}

ContainerClass PlayerClass::getEnderChest() {
	if (!thiz)
		return nullptr;
	return thiz->getEnderChestContainer();
}

std::pair<BlockPos, int> PlayerClass::getRespawnPosition() {
	if (!thiz)
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
	if (!thiz)
		return {};
	if (dim < 0)
		return false;
	thiz->setRespawnPosition(pos, dim);
	return true;
}

bool PlayerClass::refreshItems() {
	if (!thiz)
		return {};
	return thiz->refreshInventory();
}

int PlayerClass::getScore(const string& key) {
	if (!thiz)
		return {};
	return Global<Scoreboard>->getScore(thiz, key);
}

bool PlayerClass::setScore(const string& key, int value) {
	if (!thiz)
		return {};
	return Global<Scoreboard>->setScore(thiz, key, value);
}

bool PlayerClass::addScore(const string& key, int value) {
	if (!thiz)
		return {};
	return Global<Scoreboard>->addScore(thiz, key, value);
}

bool PlayerClass::reduceScore(const string& key, int value) {
	if (!thiz)
		return {};
	return Global<Scoreboard>->reduceScore(thiz, key, value);
}

bool PlayerClass::deleteScore(const string& key) {
	if (!thiz)
		return {};
	return ::Global<Scoreboard>->deleteScore(thiz, key);
}

bool PlayerClass::setSidebar(const string& title, std::vector<std::pair<std::string, int>> data, ObjectiveSortOrder order) {
	if (!thiz)
		return {};
	return thiz->setSidebar(title, data, order);
}

bool PlayerClass::removeSidebar() {
	if (!thiz)
		return {};
	return thiz->removeSidebar();
}

bool PlayerClass::setBossBar(int64_t uid, const string& name, float percent, BossEventColour colour) {
	if (!thiz)
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
	if (!thiz)
		return {};
	return thiz->sendBossEventPacket(BossEvent::Hide, "", 0, BossEventColour::Red); // Remove
}

bool PlayerClass::removeBossBar(int64_t uid) {
	if (!thiz)
		return {};
	thiz->removeBossEvent(uid);
	return true;
}

bool PlayerClass::addLevel(int value) {
	if (!thiz)
		return {};
	thiz->addLevels(value);
	return true;
}

int PlayerClass::getLevel() {
	if (!thiz)
		return {};
	return thiz->getPlayerLevel();
}

void PlayerClass::resetLevel() {
	if (!thiz)
		return;
	thiz->resetPlayerLevel();
}

int PlayerClass::getXpNeededForNextLevel() {
	if (!thiz)
		return {};
	return thiz->getXpNeededForNextLevel();
}

bool PlayerClass::addExperience(int value) {
	if (!thiz)
		return {};
	thiz->addExperience(value);
	return true;
}

bool PlayerClass::sendCustomForm(const string& str, const py::function& cb) {
	if (!thiz)
		return {};
	return thiz->sendCustomFormPacket(str,
		[this, cb](const string& arg) {
			if (LL::isServerStopping())
				return;
			call(cb, PlayerClass(thiz), py::eval(arg));
		});
}

bool PlayerClass::sendSimpleForm(const string& title, const string& content, const vector<string>& buttons, const vector<string>& images, const py::function& cb) {
	if (!thiz)
		return {};
	// if (buttons.empty() || images.empty())
	//	throw py::value_error("buttons and images shuold not be empty");
	if (buttons.size() != images.size())
		throw py::value_error("The number of buttons is not equal to the number of images");
	return thiz->sendSimpleFormPacket(title, content, buttons, images,
		[this, cb](int arg) {
			if (LL::isServerStopping())
				return;
			call(cb, PlayerClass(thiz), arg);
		});
}

bool PlayerClass::sendModalForm(const string& title, const string& content, const string& button1, const string& button2, const py::function& cb) {
	if (!thiz)
		return {};
	return thiz->sendModalFormPacket(title, content, button1, button2,
		[this, cb](bool arg) {
			if (LL::isServerStopping())
				return;
			call(cb, PlayerClass(thiz), arg);
		});
}

// catch (const fifo_json::exception& e) {
//	logger.error("Fail to parse Json string in sendCustomForm!");
//	logger.error(TextEncoding::toUTF8(e.what()));
//	PrintScriptStackTrace();
//	return {};
// }

/*
bool PlayerClass::sendPacket()
{

	try
	{
	if (!thiz)
		return {};
		auto id = PacketClass::_getPacketid(args[0]);
		auto wb = PacketClass::extractPacket(args[0]);
		auto pkt = MyPkt("", (MinecraftPacketIds)id);
		pkt.write(wb);
		Raw_SendPacket(thiz, &pkt);
	}
	return {};
}*/

bool PlayerClass::refreshChunks() {
	if (!thiz)
		return {};
	thiz->resendAllChunks();
	return true;
}

bool PlayerClass::giveItem(const ItemClass& item) {
	if (!thiz)
		return false;
	if (!item.thiz)
		return false;
	return thiz->giveItem(item.thiz);
}

bool PlayerClass::clearItem(const string& type) {
	if (!thiz)
		return {};
	return thiz->clearItem(type);
}

bool PlayerClass::isSprinting() {
	if (!thiz)
		return {};
	return thiz->isSprinting();
}

bool PlayerClass::setSprinting(bool b) {
	if (!thiz)
		return {};
	thiz->setSprinting(b);
	return true;
}

bool PlayerClass::setExtraData(string key, const py::object& value) {
	if (!thiz)
		return {};
	if (key.empty())
		return false;
	player_data[thiz->getRealName() + "-" + key] = value;
	return true;
}

py::object PlayerClass::getExtraData(string key) {
	if (!thiz)
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
	if (!thiz)
		return {};
	if (key.empty())
		return false;
	player_data.erase(thiz->getRealName() + "-" + key);
	return true;
}

NbtClass PlayerClass::getNbt() {
	if (!thiz)
		return {};
	return thiz->getNbt();
}

bool PlayerClass::setNbt(const NbtClass& nbt) {
	if (!thiz)
		return {};
	if (!nbt.thiz)
		return {};
	return thiz->setNbt(nbt.thiz->asCompoundTag());
}

bool PlayerClass::addTag(const string& tag) {
	if (!thiz)
		return {};
	return thiz->addTag(tag);
}

bool PlayerClass::hasTag(const string& tag) {
	if (!thiz)
		return {};
	return thiz->hasTag(tag);
}

bool PlayerClass::removeTag(const string& tag) {
	if (!thiz)
		return {};
	return thiz->removeTag(tag);
}

vector<string> PlayerClass::getAllTags() {
	if (!thiz)
		return {};
	return thiz->getAllTags();
}

EntityClass PlayerClass::getEntityFromViewVector(float maxDistance) {
	if (!thiz)
		return nullptr;
	auto entity = thiz->getActorFromViewVector(maxDistance);
	if (entity)
		return EntityClass(entity);
	return nullptr;
}

BlockClass PlayerClass::getBlockFromViewVector(bool includeLiquid, bool solidOnly, float maxDistance, bool ignoreBorderBlocks, bool fullOnly) {
	if (!thiz)
		return BlockInstance::Null;
	return thiz->getBlockFromViewVector(includeLiquid, solidOnly, maxDistance, ignoreBorderBlocks, fullOnly);
}

bool PlayerClass::isSimulatedPlayer() {
	return thiz->isSimulatedPlayer();
}

SimulatedPlayer* PlayerClass::asSimulatedPlayer() {
	if (thiz->isSimulatedPlayer()) {
		return static_cast<SimulatedPlayer*>(thiz);
	}
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
	// TODO: need dimension???
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateInteract(pos, face);
}

bool PlayerClass::simulateDestory(const BlockPos& pos, ScriptFacing face) {
	// TODO: need dimension???
	SimulatedPlayer* sp = asSimulatedPlayer();
	if (!sp)
		return {};
	return sp->simulateDestroyBlock(pos, face);
}

string PlayerClass::getIP() {
	if (!thiz)
		return {};
	return thiz->getNetworkIdentifier()->getIP();
}

int PlayerClass::getAvgPing() {
	if (!thiz)
		return {};
	return thiz->getAvgPing();
}

int PlayerClass::getAvgPacketLoss() {
	if (!thiz)
		return {};
	return 0;
}

string PlayerClass::getOs() {
	if (!thiz)
		return {};
	return thiz->getDeviceName();
}

string PlayerClass::getClientId() {
	if (!thiz)
		return {};
	return thiz->getClientId();
}

py::object PlayerClass::getAllItems() {
	if (!thiz)
		return {};
	ItemStack* hand = thiz->getHandSlot();
	ItemStack* offHand = (ItemStack*)&thiz->getOffhandSlot();
	vector<const ItemStack*> inventory = thiz->getInventory().getAllSlots();
	vector<const ItemStack*> armor = thiz->getArmorContainer().getAllSlots();
	vector<const ItemStack*> endChest = thiz->getEnderChestContainer()->getAllSlots();

	py::dict result;

	// hand
	result["hand"] = ItemClass(hand);

	// offHand
	result["offHand"] = ItemClass(offHand);

	// inventory
	py::list inventoryArr;
	for (const ItemStack* item : inventory) {
		inventoryArr.append(ItemClass((ItemStack*)item));
	}
	result["inventory"] = inventoryArr;

	// armor
	py::list armorArr;
	for (const ItemStack* item : armor) {
		armorArr.append(ItemClass((ItemStack*)item));
	}
	result["armor"] = armorArr;

	// endChest
	py::list endChestArr;
	for (const ItemStack* item : endChest) {
		endChestArr.append(ItemClass((ItemStack*)item));
	}
	result["endChest"] = endChestArr;

	return result;
}

bool PlayerClass::removeItem(int inventory_id, int count) {
	if (!thiz)
		return {};
	Container& container = thiz->getInventory();
	if (inventory_id > container.getSize())
		return false;
	container.removeItem(inventory_id, count);
	return true;
}
