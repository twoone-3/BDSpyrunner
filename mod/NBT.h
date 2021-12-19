#pragma once
#include "JsonTool.h"
#include <MC/Tag.hpp>
#include <MC/ByteTag.hpp>
#include <MC/ShortTag.hpp>
#include <MC/IntTag.hpp>
#include <MC/Int64Tag.hpp>
#include <MC/FloatTag.hpp>
#include <MC/DoubleTag.hpp>
#include <MC/ByteArrayTag.hpp>
#include <MC/StringTag.hpp>
#include <MC/ListTag.hpp>
#include <MC/CompoundTag.hpp>
#include <MC/IntArrayTag.hpp>

json ToJson(ListTag& t);
json ToJson(CompoundTag& t);
ListTag* ToListTag(const json& value);
CompoundTag* ToCompoundTag(const json& value);
