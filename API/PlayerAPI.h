#pragma once
#include <Global.hpp>
#include <MC/ScriptModuleMinecraft.hpp>

class SimulatedPlayer;
class Player;
struct ContainerClass;
struct ItemClass;
struct BlockClass;
struct EntityClass;
struct NBTClass;
enum CommandPermissionLevel : char;

struct PlayerClass {
	Player* thiz;

	PlayerClass(Player* p);
	PlayerClass(const PlayerClass& other);

	string getName();
	Vec3 getPos();
	BlockPos getBlockPos();
	int getDim();
	string getRealName();
	string getXuid();
	string getUuid();
	CommandPermissionLevel getPermLevel();
	GameType getGameMode();
	int getMaxHealth();
	int getHealth();
	bool getInAir();
	bool getInWater();
	bool getSneaking();
	float getSpeed();
	Vec2 getDirection();
	string getUniqueID();
	string getLangCode();

	bool isOP();
	bool setPermLevel(CommandPermissionLevel);
	bool setGameMode(GameType);

	bool runcmd(const string&);
	bool teleport(const Vec3& pos, int dim);
	bool kill();
	bool kick(const string& msg = "Disconnecting from the server");
	bool sendText(const string& msg, TextType type = TextType::RAW);
	bool talkAs(const string& msg);
	bool talkTo(const string& msg, Player* target);
	bool rename(const string& name);
	bool transServer(const string& ip, short port = 19132);
	bool crash();
	bool hurt(int damage);
	bool setOnFire(int time);
	bool refreshChunks();
	bool giveItem(const ItemClass& item);
	bool clearItem(const string& type);
	bool isSprinting();
	bool setSprinting(bool b);

	BlockClass getBlockStandingOn();
	ItemClass getHand();
	ItemClass getOffHand();
	ContainerClass getInventory();
	ContainerClass getArmor();
	ContainerClass getEnderChest();
	std::pair<BlockPos, int> getRespawnPosition();
	bool setRespawnPosition(BlockPos pos, int dim);
	bool refreshItems();

	int getScore(const string& key);
	bool setScore(const string& key, int value);
	bool addScore(const string& key, int value);
	bool reduceScore(const string& key, int value);
	bool deleteScore(const string& key);
	bool setSidebar(const string& title, std::vector<std::pair<std::string, int>> data, ObjectiveSortOrder order);
	bool removeSidebar();
	bool setBossBar(int64_t uid, const string& name, float percent, BossEventColour colour = BossEventColour::Red);
	bool removeBossBar();
	bool removeBossBar(int64_t uid);
	bool addLevel(int value);
	int getLevel();
	void resetLevel();
	int getXpNeededForNextLevel();
	bool addExperience(int value);

	bool sendCustomForm(const string& str, const py::function& cb);
	bool sendSimpleForm(const string& title, const string& content, const vector<string>& buttons, const vector<string>& images, const py::function& cb);
	bool sendModalForm(const string& title, const string& content, const string& button1, const string& button2, const py::function& cb);

	bool setExtraData(string key, const py::object& value);
	py::object getExtraData(string key);
	bool delExtraData(string key);

	NBTClass getNbt();
	bool setNbt(const NBTClass& nbt);
	bool addTag(const string& tag);
	bool hasTag(const string& tag);
	bool removeTag(const string& tag);
	vector<string> getAllTags();
	EntityClass getEntityFromViewVector(float maxDistance = 5.25f);
	BlockClass getBlockFromViewVector(
	    bool includeLiquid = false,
	    bool solidOnly = false,
	    float maxDistance = 5.25f,
	    bool ignoreBorderBlocks = true,
	    bool fullOnly = false);

	bool isSimulatedPlayer();

	SimulatedPlayer* asSimulatedPlayer();
	bool simulateSneak();
	bool simulateAttack();
	bool simulateAttack(const EntityClass& target);
	bool simulateDestory();
	bool simulateDestory(const BlockPos& pos, ScriptModuleMinecraft::ScriptFacing face);
	bool simulateDisconnect();
	bool simulateInteract();
	bool simulateInteract(const EntityClass& target);
	bool simulateInteract(const BlockPos& pos, ScriptModuleMinecraft::ScriptFacing face);
	bool simulateJump();
	bool simulateLocalMove(const Vec3& target, float speed = 1.0f);
	bool simulateWorldMove(const Vec3& target, float speed = 1.0f);
	bool simulateMoveTo(const Vec3& target, float speed = 1.0f);
	bool simulateLookAt(const EntityClass& target);
	bool simulateLookAt(const BlockPos& pos);
	bool simulateLookAt(const Vec3& pos);
	bool simulateSetBodyRotation(float rotation);
	py::dict simulateNavigateTo(const EntityClass& target, float speed = 1.f);
	py::dict simulateNavigateTo(const Vec3& pos, float speed = 1.f);
	bool simulateNavigateTo(std::vector<Vec3> path, float speed = 1.f);
	bool simulateUseItem();
	bool simulateUseItem(int slot);
	bool simulateUseItem(const ItemClass& item);
	bool simulateUseItem(const ItemClass& item, const BlockPos& pos, ScriptModuleMinecraft::ScriptFacing face = ScriptModuleMinecraft::ScriptFacing::Down, const Vec3& relativePos = {0.5, 0.5, 0.5});
	bool simulateUseItem(int slot, const BlockPos& pos, ScriptModuleMinecraft::ScriptFacing face = ScriptModuleMinecraft::ScriptFacing::Down, const Vec3& relativePos = {0.5, 0.5, 0.5});
	bool simulateStopDestroyingBlock();
	bool simulateStopInteracting();
	bool simulateStopMoving();
	bool simulateStopUsingItem();
	bool simulateStopSneaking();

	string getIP();
	int getAvgPing();
	int getAvgPacketLoss();
	string getOs();
	string getClientId();

	bool removeItem(int slot, int count);
	bool setHeadShow(const string& name);
};