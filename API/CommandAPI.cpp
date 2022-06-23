#include "CommandAPI.h"
#include "McAPI.h"
#include "ItemAPI.h"
#include "EntityAPI.h"
#include "PlayerAPI.h"
#include "BlockAPI.h"
#include "CommandOriginAPI.h"
#include "CommandOutputAPI.h"
#include <RegCommandAPI.h>

py::object convertResult(const DynamicCommand::Result& result) {
	if (!result.isSet)
		return py::object(); // null
	switch (result.type) {
	case DynamicCommand::ParameterType::Bool:
		return py::bool_(result.getRaw<bool>());
	case DynamicCommand::ParameterType::Int:
		return py::int_(result.getRaw<int>());
	case DynamicCommand::ParameterType::Float:
		return py::float_(result.getRaw<float>());
	case DynamicCommand::ParameterType::String:
		return py::str(result.getRaw<std::string>());
	case DynamicCommand::ParameterType::Actor: {
		py::list arr;
		for (auto i : result.get<std::vector<Actor*>>()) {
			arr.append(EntityClass(i));
		}
		return arr;
	}
	case DynamicCommand::ParameterType::Player: {
		py::list arr;
		for (auto i : result.get<std::vector<Player*>>()) {
			arr.append(PlayerClass(i));
		}
		return arr;
	}
	case DynamicCommand::ParameterType::BlockPos: {
		py::list arr;
		arr.append(result.get<BlockPos>());
		auto dim = result.origin->getDimension();
		arr.append(dim ? int(dim->getDimensionId()) : -1);
		return arr;
	}
	case DynamicCommand::ParameterType::Vec3: {
		py::list arr;
		arr.append(result.get<Vec3>());
		auto dim = result.origin->getDimension();
		arr.append(dim ? int(dim->getDimensionId()) : -1);
		return arr;
	}
	case DynamicCommand::ParameterType::Message:
		return py::str(result.getRaw<CommandMessage>().getMessage(*result.origin));
	case DynamicCommand::ParameterType::RawText:
		return py::str(result.getRaw<std::string>());
	case DynamicCommand::ParameterType::JsonValue:
		return py::str(JsonHelpers::serialize(result.getRaw<Json::Value>()));
	case DynamicCommand::ParameterType::Item:
		return py::cast(ItemClass(new ItemStack(result.getRaw<CommandItem>().createInstance(1, 1, nullptr, true).value_or(ItemInstance::EMPTY_ITEM))));
	case DynamicCommand::ParameterType::Block:
		return py::cast(BlockClass(BlockInstance::createBlockInstance(
			const_cast<Block*>(result.getRaw<Block const*>()), BlockPos::MIN, -1)));
	case DynamicCommand::ParameterType::Effect:
		return py::str(result.getRaw<MobEffect const*>()->getResourceName());
	case DynamicCommand::ParameterType::Enum:
		return py::str(result.getRaw<std::string>());
	case DynamicCommand::ParameterType::SoftEnum:
		return py::str(result.getRaw<std::string>());
	case DynamicCommand::ParameterType::Command:
		return py::str(result.getRaw<std::unique_ptr<Command>>()->getCommandName());
	case DynamicCommand::ParameterType::ActorType:
		return py::str(result.getRaw<ActorDefinitionIdentifier const*>()->getCanonicalName());
	default:
		return py::object(); // null
		break;
	}
}

//////////////////// Command APIs ////////////////////

CommandClass::CommandClass(const string& name, const string& desc, CommandPermissionLevel perm) : thiz(DynamicCommand::createCommand(name, desc, perm)) {}

string CommandClass::getName() {
	return thiz->getCommandName();
}

bool CommandClass::setAlias(const string& alias) {
	return thiz->setAlias(alias);
}

string CommandClass::setEnum(const string& name, const vector<string>& arr) {
	return thiz->setEnum(name, arr);
}

// name, type, description, identifier, option

int64_t CommandClass::mandatory(const string& name, DynamicCommand::ParameterType type, string description, string identifier, CommandParameterOption option) {
	return thiz->mandatory(name, type, description, identifier, option);
}

// name, type, description, option

int64_t CommandClass::mandatory(const string& name, DynamicCommand::ParameterType type, string description, CommandParameterOption option) {
	return thiz->mandatory(name, type, description, option);
}

// name, type, description, identifier, option

int64_t CommandClass::optional(const string& name, DynamicCommand::ParameterType type, string description, string identifier, CommandParameterOption option) {
	return thiz->optional(name, type, description, identifier, option);
}

// name, type, description, option

int64_t CommandClass::optional(const string& name, DynamicCommand::ParameterType type, string description, CommandParameterOption option) {
	return thiz->optional(name, type, description, option);
}

bool CommandClass::addOverload() {
	return thiz->addOverload();
}

// vector<index>

bool CommandClass::addOverload(const vector<size_t>& args) {
	vector<DynamicCommandInstance::ParameterIndex> params;
	for (int i = 0; i < args.size(); ++i) {
		params.emplace_back(thiz.get(), args[i]);
	}
	return thiz->addOverload(std::move(params));
}

// vector<identifier>

bool CommandClass::addOverload(vector<string>&& args) {
	return thiz->addOverload(std::move(args));
}

// function (command, origin, output, results){}

bool CommandClass::setCallback(const py::function& cb) {
	thiz->setCallback([cb](const DynamicCommand& command, const CommandOrigin& origin,
						  CommandOutput& output, std::unordered_map<string, DynamicCommand::Result>& results) {
		call(cb, CommandOriginClass(const_cast<CommandOrigin*>( & origin)), results);
	});
	return true;
}

// setup(Function<Command, Origin, Output, Map<String, Any>>)

bool CommandClass::setup() {
	return DynamicCommand::setup(std::move(thiz));
}

string CommandClass::toString() {
	return fmt::format("<Command({})>", thiz->getCommandName());
}

string CommandClass::setSoftEnum(const string& name, const vector<string>& values) {
	return thiz->setSoftEnum(name, values);
}

bool CommandClass::addSoftEnumValues(const string& name, const vector<string>& values) {
	return thiz->addSoftEnumValues(name, values);
}

bool CommandClass::removeSoftEnumValues(const string& name, const vector<string>& values) {
	return thiz->removeSoftEnumValues(name, values);
}

vector<string> CommandClass::getSoftEnumValues(const string& name) {
	return thiz->getSoftEnumValues(name);
}

vector<string> CommandClass::getSoftEnumNames() {
	return thiz->getSoftEnumNames();
}

//void onExecute(DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output,
//	std::unordered_map<std::string, DynamicCommand::Result>& results) {
//	auto instance = command.getInstance();
//	auto& commandName = instance->getCommandName();
//	if (localShareData->commandCallbacks.find(commandName) == localShareData->commandCallbacks.end()) {
//		logger.warn("Command {} failed to execute, is the plugin unloaded?", commandName);
//		return;
//	}
//	EngineScope enter(localShareData->commandCallbacks[commandName].fromEngine);
//	Local<Object> args = Object::newObject();
//	auto cmd = CommandClass::newCommand(const_cast<DynamicCommandInstance*>(instance));
//	auto ori = CommandOriginClass::newCommandOrigin(&origin);
//	auto outp = CommandOutputClass::newCommandOutput(&output);
//	for (auto& [name, param] : results)
//		args.set(name, convertResult(param));
//	localShareData->commandCallbacks[commandName].func.thiz.call({}, cmd, ori, outp, args);
//}

// not complete
// void onExecute2(DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output,
//	std::unordered_map<std::string, DynamicCommand::Result>& results) {
//	auto instance = command.getInstance();
//	auto& commandName = instance->getCommandName();
//	if (localShareData->commandCallbacks.find(commandName) == localShareData->commandCallbacks.end()) {
//		logger.warn("Command {} failed to execute, is the plugin unloaded?", commandName);
//		return;
//	}
//	EngineScope enter(localShareData->commandCallbacks[commandName].fromEngine);
//	try {
//		// auto ctx = CommandContextClass::newCommandContext(&command, &origin, &output, &results);
//		// Local<Object> args = Object::newObject();
//		// for (auto& [name, param] : results)
//		//     args.set(name, convertResult(param));
//		// localShareData->commandCallbacks[commandName].func.thiz.call({}, ctx, args);
//	}
//	CATCH_WITHOUT_RETURN("Fail in executing command \"" + commandName + "\"!")
//}

