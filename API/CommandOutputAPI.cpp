#include "CommandOutputAPI.h"

#include <MC/CommandOutput.hpp>

CommandOutputClass::CommandOutputClass(CommandOutput* p) : thiz(p) {}

bool CommandOutputClass::empty() { return thiz->empty(); }

int CommandOutputClass::getSuccessCount() { return thiz->getSuccessCount(); }

bool CommandOutputClass::success() {
  thiz->success();
  return true;
}

bool CommandOutputClass::success(const string& msg) {
  thiz->success(msg);
  return true;
}

bool CommandOutputClass::addMessage(const string& msg) {
  thiz->addMessage(msg);
  return true;
}

bool CommandOutputClass::error(const string& msg) {
  thiz->error(msg);
  return true;
}

const char* CommandOutputClass::toString() { return "<CommandOutput>"; }
