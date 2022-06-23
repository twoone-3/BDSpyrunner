#pragma once
#include <Global.hpp>

class CommandOutput;
struct CommandOutputClass {
	CommandOutput* thiz;

	CommandOutputClass(CommandOutput* p);

	// MCAPI bool empty() const;
	bool empty();

	// MCAPI int getSuccessCount() const;
	int getSuccessCount();

	// MCAPI void success(std::string const&, std::vector<class CommandOutputParameter> const&);
	// MCAPI void success();
	bool success();
	bool success(const string& msg);

	bool addMessage(const string& msg);

	// MCAPI void error(std::string const&, std::vector<class CommandOutputParameter> const&);
	bool error(const string& msg);

	// MCAPI void setHasPlayerText();
	// auto setHasPlayerText()
	//{
	//     try
	//     {
	//         thiz->setHasPlayerText();
	//         return true;
	//     }
	//     CATCH("Fail in getBlockPosition!");
	// };
	// MCAPI bool wantsData() const;
	// auto wantsData()
	//{
	//     try
	//     {
	//         return thiz->wantsData();
	//     }
	//     CATCH("Fail in getBlockPosition!");
	// };
	// MCAPI void addToResultList(std::string const&, std::string const&);
	// MCAPI void addToResultList(std::string const&, class Actor const&);
	// auto addToResultList();
	// MCAPI void forceOutput(std::string const&, std::vector<class CommandOutputParameter> const&);
	// auto forceOutput();
	// MCAPI class CommandPropertyBag const& getData() const;
	// auto getData() const;
	// MCAPI std::vector<class CommandOutputMessage> const& getMessages() const;
	// auto getMessages() const;
	// MCAPI void load(enum CommandOutputType, int, std::vector<class CommandOutputMessage>&&, std::unique_ptr<class CommandPropertyBag>&&);
	// auto load();

	const char* toString();
};
