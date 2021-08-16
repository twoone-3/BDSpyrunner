#pragma once
#include <string>

struct Vec3 {
	float x = 0.0f, y = 0.0f, z = 0.0f;
	std::string toString() {
		char str[64];
		sprintf_s(str, "(%f,%f,%f)", x, y, z);
		return str;
	}
};
struct Vec2 {
	float x = 0.0f, y = 0.0f;
	std::string toString() {
		char str[48];
		sprintf_s(str, "(%f,%f)", x, y);
		return str;
	}
};
struct BlockPos {
	int x = 0, y = 0, z = 0;

	std::string toString() {
		char str[32];
		sprintf_s(str, "(%d,%d,%d)", x, y, z);
		return str;
	}
};
