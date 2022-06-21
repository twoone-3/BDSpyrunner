#pragma once
#include <Global.hpp>
#include <MC/CommandOrigin.hpp>

struct EntityClass;
struct PlayerClass;
struct NbtClass;
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
	NbtClass getNbt();
	const char* toString();
};
