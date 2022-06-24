#pragma once
#include <Global.hpp>
#include <DynamicCommandAPI.h>

py::object convertResult(const DynamicCommand::Result& result);

struct CommandClass {
	std::unique_ptr<DynamicCommandInstance> thiz;

	CommandClass(const string& name, const string& desc, CommandPermissionLevel perm);
	string getName();
	bool setAlias(const string& alias);
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

	// SoftEnum 是带补全的字符串
	// 补全项可以实时更改
	// 不同选项不能接不同参数
	// Enum 则可以做到
	// cmd <add|remove> name
	// cmd <list>
	// cmd <rename> name name2

	string setEnum(const string& name, const vector<string>& arr);
	string setSoftEnum(const string& name, const vector<string>& values);
	bool addSoftEnumValues(const string& name, const vector<string>& values);
	bool removeSoftEnumValues(const string& name, const vector<string>& values);
	vector<string> getSoftEnumValues(const string& name);
	vector<string> getSoftEnumNames();
};

