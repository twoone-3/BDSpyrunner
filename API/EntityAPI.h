#pragma once
#include <Global.hpp>

class Actor;
struct ContainerClass;
struct PlayerClass;
struct BlockClass;
struct NbtClass;
struct EntityClass {
	Actor* thiz;

	EntityClass(Actor* a);

	string getName();
	string getType();
	int getId();
	Vec3 getPos();
	BlockPos getBlockPos();
	int getDim();
	int getMaxHealth();
	int getHealth();
	bool getInAir();
	bool getInWater();
	float getSpeed();
	Vec2 getDirection();
	string getUniqueID();

	bool teleport(const Vec3& pos, int dim);
	bool kill();
	bool hurt(float damage);
	bool setOnFire(int time);

	bool isPlayer();
	PlayerClass toPlayer();
	bool isItemEntity();
	EntityClass toItem();
	BlockClass getBlockStandingOn();
	ContainerClass getArmor();
	bool hasContainer();
	ContainerClass getContainer();
	bool refreshItems();

	NbtClass getNbt();
	bool setNbt(const NbtClass& nbt);
	bool addTag(const string& tag);
	bool removeTag(const string& tag);
	bool hasTag(const string& tag);
	vector<string> getAllTags();
	EntityClass getEntityFromViewVector(float maxDistance = 5.25f);
	BlockClass getBlockFromViewVector(
		bool includeLiquid = false,
		bool solidOnly = false,
		float maxDistance = 5.25f,
		bool ignoreBorderBlocks = true,
		bool fullOnly = false);
};