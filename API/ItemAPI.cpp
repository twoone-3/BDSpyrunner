#include <Global.hpp>
#include "ItemAPI.h"
#include "McAPI.h"
#include "EntityAPI.h"
#include "NbtAPI.h"
#include <MC/CompoundTag.hpp>
#include <MC/ItemStack.hpp>
#include <vector>
#include <string>


ItemClass::ItemClass(ItemStack* p) : thiz(p) {}

string ItemClass::getName() {
	return thiz->getName();
}

string ItemClass::getType() {
	return thiz->getTypeName();
}

short ItemClass::getId() {
	return thiz->getId();
}

int ItemClass::getCount() {
	return thiz->getCount();
}

int ItemClass::getAux() {
	return thiz->getAux();
}

bool ItemClass::set(const ItemClass& i) {
	ItemStack* item = i.thiz;
	if (!item)
		throw std::invalid_argument("invalid item");
	return thiz->setItem(item);
}

ItemClass ItemClass::clone() {
	if (!thiz)
		throw std::invalid_argument("invalid item");
	return ItemClass(thiz->clone_s());
}

bool ItemClass::isNull() {
	return thiz->isNull();
}

bool ItemClass::setNull() {
	thiz->setNull();
	return true;
}

bool ItemClass::setAux(short aux) {
	thiz->setAuxValue(aux);
	return true;
}

bool ItemClass::setLore(const vector<string>& lores) {
	return thiz->setLore(lores);
}

NbtClass ItemClass::getNbt() {
	return thiz->getNbt();
}

bool ItemClass::setNbt(const NbtClass& nbt) {
	return thiz->setNbt(nbt.thiz->asCompoundTag());
}

// auto McClass::newItem() {
//	CHECK_ARGS_COUNT(args, 1);
//
//	try {
//		if (args[0].isString()) {
//			// name & count
//			if (args.size() >= 2 && args[1].isNumber()) {
//				string type = args[0].toStr();
//				int cnt = args[1].toInt();
//
//				ItemStack* item = ItemStack::create(type, cnt);
//				if (!item)
//					return {}; // Null
//				else
//					return ItemClass::newItem(item);
//			}
//			else {
//				LOG_TOO_FEW_ARGS();
//				return {};
//			}
//		}
//		else {
//			CompoundTag* nbt = (CompoundTag*)NbtClass::extract(args[0]);
//			if (nbt) {
//				ItemStack* item = ItemStack::create(nbt->clone());
//				if (!item)
//					return {}; // Null
//				else
//					return ItemClass::newItem(item);
//			}
//			else {
//				LOG_WRONG_ARG_TYPE();
//				return {};
//			}
//		}
//	}
//	CATCH("Fail in NewItem!");
// }

// auto McClass::spawnItem() {
//	CHECK_ARGS_COUNT(args, 2);
//
//	try {
//		FloatVec4 pos;
//		if (args.size() == 2) {
//			if (IsInstanceOf<IntPos>(args[1])) {
//				// IntPos
//				IntPos* posObj = IntPos::extractPos(args[1]);
//				if (posObj->dim < 0)
//					return false;
//				else {
//					pos.x = posObj->x;
//					pos.y = posObj->y;
//					pos.z = posObj->z;
//					pos.dim = posObj->dim;
//				}
//			}
//			else if (IsInstanceOf<FloatPos>(args[1])) {
//				// FloatPos
//				FloatPos* posObj = FloatPos::extractPos(args[1]);
//				if (posObj->dim < 0)
//					return false;
//				else {
//					pos = *posObj;
//				}
//			}
//			else {
//				LOG_WRONG_ARG_TYPE();
//				return {};
//			}
//		}
//		else if (args.size() == 5) {
//			// Number Pos
//			CHECK_ARG_TYPE(args[1], ValueKind::kNumber);
//			CHECK_ARG_TYPE(args[2], ValueKind::kNumber);
//			CHECK_ARG_TYPE(args[3], ValueKind::kNumber);
//			CHECK_ARG_TYPE(args[4], ValueKind::kNumber);
//			pos = {args[1].asNumber().toFloat(), args[2].asNumber().toFloat(), args[3].asNumber().toFloat(), args[4].toInt()};
//		}
//		else {
//			LOG_WRONG_ARGS_COUNT();
//			return {};
//		}
//
//
//		ItemStack* it = ItemClass::extract(args[0]);
//		if (it) {
//			// By Item
//			Actor* entity = Level::spawnItem(pos.getVec3(), pos.dim, it);
//			if (!entity)
//				return {}; // Null
//			else
//				return EntityClass::newEntity(entity);
//		}
//		else {
//			LOG_WRONG_ARG_TYPE();
//			return {};
//		}
//	}
//	CATCH("Fail in SpawnItem!");
// }