#pragma once
#include <Global.hpp>

class CommandOutput;
struct CommandOutputClass {
  CommandOutput* thiz;

  CommandOutputClass(CommandOutput* p);

  bool empty();

  int getSuccessCount();

  bool success();
  bool success(const string& msg);

  bool addMessage(const string& msg);

  bool error(const string& msg);

  const char* toString();
};
