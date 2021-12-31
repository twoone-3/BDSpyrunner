// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Bedrock.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ScriptServerContext {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_SCRIPTSERVERCONTEXT
public:
    class ScriptServerContext& operator=(class ScriptServerContext const&) = delete;
    ScriptServerContext(class ScriptServerContext const&) = delete;
    ScriptServerContext() = delete;
#endif

public:
    MCAPI class gsl::not_null<class Bedrock::NonOwnerPointer<class Minecraft> > getMinecraft();
    MCAPI void reset();
    MCAPI ~ScriptServerContext();

protected:

private:

};