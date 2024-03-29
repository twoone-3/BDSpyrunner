﻿#pragma once
#include "../global.h"

class BlockActor;
class BlockPos;
struct BlockClass;
struct NBTClass;
struct BlockEntityClass {
  BlockActor* thiz;
  int dim;

  BlockEntityClass(BlockActor* be, int dim);

  BlockPos getPos();
  int getType();

  NBTClass getNbt();
  bool setNbt(const NBTClass& nbt);
  BlockClass getBlock();
};
