#pragma once
#include <Global.hpp>

struct EntityClass;
struct PlayerClass;
struct NBTClass;
class CommandOrigin;
class BlockPos;
class Vec3;
struct CommandOriginClass {
	CommandOrigin* thiz;

	CommandOriginClass(CommandOrigin* p);
	int getOriginType();
	string_view getOriginTypeName();
	string getOriginName();
	BlockPos getBlockPosition();
	Vec3 getPosition();
	int getDim();
	EntityClass getEntity();
	PlayerClass getPlayer();
	NBTClass getNbt();
	const char* toString();
};
