// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "ScriptApi.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ScriptServerBlockMovedByPistonEvent {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_SCRIPTSERVERBLOCKMOVEDBYPISTONEVENT
public:
    class ScriptServerBlockMovedByPistonEvent& operator=(class ScriptServerBlockMovedByPistonEvent const&) = delete;
    ScriptServerBlockMovedByPistonEvent(class ScriptServerBlockMovedByPistonEvent const&) = delete;
#endif

public:
    /*0*/ virtual ~ScriptServerBlockMovedByPistonEvent();
    /*1*/ virtual bool _serialize(class ScriptEngine&, class ScriptApi::ScriptObjectHandle&) const;
    /*
    inline  ~ScriptServerBlockMovedByPistonEvent(){
         (ScriptServerBlockMovedByPistonEvent::*rv)();
        *((void**)&rv) = dlsym("??1ScriptServerBlockMovedByPistonEvent@@UEAA@XZ");
        return (this->*rv)();
    }
    */
    MCAPI ScriptServerBlockMovedByPistonEvent();
    MCAPI void setBlockPos(class BlockPos const&);
    MCAPI void setPistonAction(std::string);
    MCAPI void setPistonPos(class BlockPos const&);

protected:

private:
    MCAPI static class HashedString const mHash;

};