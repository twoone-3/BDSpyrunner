#pragma once
#include <MC/BlockInstance.hpp>

#include "../global.h"

class Block;
struct ContainerClass;
struct BlockEntityClass;
struct NBTClass;
struct BlockClass {
  BlockInstance thiz;

  BlockClass(const BlockInstance& bi);
  BlockClass(BlockInstance&& bi);
  BlockClass(const BlockPos& pos, int dim);

  string getName();
  string getType();
  int getId();
  BlockPos getPos();
  int getDim();
  unsigned short getTileData();

  NBTClass getNbt();
  bool setNbt(const NBTClass&);
  bool hasContainer();
  ContainerClass getContainer();
  bool hasBlockEntity();
  BlockEntityClass getBlockEntity();
  bool removeBlockEntity(const BlockPos& pos);
};
