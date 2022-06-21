#include "CommandOutputAPI.h"

//////////////////// APIs ////////////////////

CommandOutputClass::CommandOutputClass(CommandOutput* p) : thiz(p) {}

// MCAPI bool empty() const;

bool CommandOutputClass::empty() {
	return thiz->empty();
}

// MCAPI int getSuccessCount() const;

int CommandOutputClass::getSuccessCount() {
	return thiz->getSuccessCount();
}

// MCAPI void success(std::string const&, std::vector<class CommandOutputParameter> const&);
// MCAPI void success();

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

// MCAPI void error(std::string const&, std::vector<class CommandOutputParameter> const&);

bool CommandOutputClass::error(const string& msg) {
	thiz->error(msg);
	return true;
}

const char* CommandOutputClass::toString() {
	return "<CommandOutput>";
}


// MCAPI enum CommandOutputType getType() const;
// auto CommandOutputClass::getType()
//{
//     try
//     {
//         return magic_enum::enum_name(get()->getType());
//     }
//     CATCH("Fail in getType!");
// };
