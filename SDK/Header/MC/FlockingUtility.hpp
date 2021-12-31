// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

namespace FlockingUtility {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA
    MCAPI bool checkForSteeringCollision(class Actor&, class Vec3 const&, float, class Vec3&);
    MCAPI void chooseRandomDirection(class Actor&, class Vec3&);
    MCAPI void getWaterHeights(class Actor const&, float&, float&);

};