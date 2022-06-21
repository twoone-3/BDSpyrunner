#pragma once
#include <Global.hpp>
#include <DynamicCommandAPI.h>

py::object convertResult(const DynamicCommand::Result& result);

struct CommandClass {
	std::unique_ptr<DynamicCommandInstance> thiz;

	CommandClass(const string& name, const string& desc, CommandPermissionLevel perm);
	string getName();
	bool setAlias(const string& alias);
	string setEnum(const string& name, const vector<string>& arr);
	// name, type, description, identifier, option
	int64_t mandatory(const string& name, DynamicCommand::ParameterType type,
		string description = "", string identifier = "", CommandParameterOption option = None);
	// name, type, description, option
	int64_t mandatory(const string& name, DynamicCommand::ParameterType type,
		string description = "", CommandParameterOption option = None);
	// name, type, description, identifier, option
	int64_t optional(const string& name, DynamicCommand::ParameterType type,
		string description = "", string identifier = "", CommandParameterOption option = None);
	// name, type, description, option
	int64_t optional(const string& name, DynamicCommand::ParameterType type,
		string description = "", CommandParameterOption option = None);
	bool addOverload();
	// vector<index>
	bool addOverload(const vector<size_t>& args);
	// vector<identifier>
	bool addOverload(vector<string>&& args);
	// function (command, origin, output, results){}
	bool setCallback(const py::function& cb);
	// setup(Function<Command, Origin, Output, Map<String, Any>>)
	bool setup();
	string toString();
	string setSoftEnum(const string& name, const vector<string>& values);
	bool addSoftEnumValues(const string& name, const vector<string>& values);
	bool removeSoftEnumValues(const string& name, const vector<string>& values);
	vector<string> getSoftEnumValues(const string& name);
	vector<string> getSoftEnumNames();
};

