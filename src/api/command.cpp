#include "command.h"

#include <llapi/LoggerAPI.h>

#include <llapi/mc/CommandBlockName.hpp>
#include <llapi/mc/CommandBlockNameResult.hpp>
#include <llapi/mc/Dimension.hpp>
#include <llapi/mc/ItemInstance.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/JsonHelpers.hpp>
#include <llapi/mc/MobEffect.hpp>

#include "block.h"
#include "command_origin.h"
#include "command_output.h"
#include "entity.h"
#include "item.h"
#include "player.h"
#include "utils.h"

py::object CastResult(const DynamicCommand::Result& result) {
  if (!result.isSet) return py::object();  // null
  switch (result.type) {
    case DynamicCommand::ParameterType::Bool:
      return py::bool_(result.getRaw<bool>());
    case DynamicCommand::ParameterType::Int:
      return py::int_(result.getRaw<int>());
    case DynamicCommand::ParameterType::Float:
      return py::float_(result.getRaw<float>());
    case DynamicCommand::ParameterType::String:
      return py::str(result.getRaw<string>());
    case DynamicCommand::ParameterType::Actor: {
      py::list arr;
      for (auto i : result.get<vector<Actor*>>()) {
        arr.append(EntityClass(i));
      }
      return arr;
    }
    case DynamicCommand::ParameterType::Player: {
      py::list arr;
      for (auto i : result.get<vector<Player*>>()) {
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
      return py::str(
          result.getRaw<CommandMessage>().getMessage(*result.origin));
    case DynamicCommand::ParameterType::RawText:
      return py::str(result.getRaw<string>());
    case DynamicCommand::ParameterType::JsonValue:
      return py::str(JsonHelpers::serialize(result.getRaw<Json::Value>()));
    case DynamicCommand::ParameterType::Item:
      return py::cast(
          ItemClass(new ItemStack(result.getRaw<CommandItem>()
                                      .createInstance(1, 1, nullptr, true)
                                      .value_or(ItemInstance::EMPTY_ITEM))));
    case DynamicCommand::ParameterType::Block:
      return py::cast(BlockClass(BlockInstance::createBlockInstance(
          const_cast<Block*>(
              result.getRaw<CommandBlockName>().resolveBlock(0).getBlock()),
          BlockPos::MIN, -1)));
    case DynamicCommand::ParameterType::Effect:
      return py::str(result.getRaw<MobEffect const*>()->getResourceName());
    case DynamicCommand::ParameterType::Enum:
      return py::str(result.getRaw<string>());
    case DynamicCommand::ParameterType::SoftEnum:
      return py::str(result.getRaw<string>());
    case DynamicCommand::ParameterType::Command:
      return py::str(result.getRaw<unique_ptr<Command>>()->getCommandName());
    case DynamicCommand::ParameterType::ActorType:
      return py::str(result.getRaw<ActorDefinitionIdentifier const*>()
                         ->getCanonicalName());
    default:
      return py::object();  // null
      break;
  }
}

CommandClass::CommandClass(const string& name, const string& desc,
                           CommandPermissionLevel perm)
    : thiz(DynamicCommand::createCommand(name, desc, perm)) {}

string CommandClass::getName() {
  if (thiz == nullptr) return "";
  return thiz->getCommandName();
}

bool CommandClass::setAlias(const string& alias) {
  if (thiz == nullptr) return false;
  return thiz->setAlias(alias);
}

// name, type, description, identifier, option

int64_t CommandClass::mandatory(const string& name,
                                DynamicCommand::ParameterType type,
                                string description, string identifier,
                                CommandParameterOption option) {
  if (thiz == nullptr) return -1;
  return thiz->mandatory(name, type, description, identifier, option);
}

// name, type, description, option

int64_t CommandClass::mandatory(const string& name,
                                DynamicCommand::ParameterType type,
                                string description,
                                CommandParameterOption option) {
  if (thiz == nullptr) return -1;
  return thiz->mandatory(name, type, description, option);
}

// name, type, description, identifier, option

int64_t CommandClass::optional(const string& name,
                               DynamicCommand::ParameterType type,
                               string description, string identifier,
                               CommandParameterOption option) {
  if (thiz == nullptr) return -1;
  return thiz->optional(name, type, description, identifier, option);
}

// name, type, description, option

int64_t CommandClass::optional(const string& name,
                               DynamicCommand::ParameterType type,
                               string description,
                               CommandParameterOption option) {
  if (thiz == nullptr) return -1;
  return thiz->optional(name, type, description, option);
}

bool CommandClass::addOverload() {
  if (thiz == nullptr) return false;
  return thiz->addOverload();
}

// vector<index>

bool CommandClass::addOverload(const vector<size_t>& args) {
  if (thiz == nullptr) return false;
  vector<DynamicCommandInstance::ParameterIndex> params;
  for (int i = 0; i < args.size(); ++i) {
    params.emplace_back(thiz.get(), args[i]);
  }
  return thiz->addOverload(move(params));
}

// vector<identifier>

bool CommandClass::addOverload(vector<string>&& args) {
  if (thiz == nullptr) return false;
  return thiz->addOverload(move(args));
}

// function (command, origin, output, results){}

bool CommandClass::setCallback(const py::function& cb) {
  if (thiz == nullptr) return false;
  thiz->setCallback(
      [cb](const DynamicCommand& command, const CommandOrigin& origin,
           CommandOutput& output,
           unordered_map<string, DynamicCommand::Result>& results) {
        PY_TRY;
        py::gil_scoped_acquire gil_;
        cb(CommandOriginClass(const_cast<CommandOrigin*>(&origin)), results);
        PY_CATCH;
      });
  return true;
}

// setup(Function<Command, Origin, Output, Map<String, Any>>)

bool CommandClass::setup() {
  if (thiz == nullptr) return false;
  return DynamicCommand::setup(move(thiz));
}

string CommandClass::toString() {
  if (thiz == nullptr) return "";
  return fmt::format("<Command({})>", thiz->getCommandName());
}

string CommandClass::setEnum(const string& name, const vector<string>& arr) {
  if (thiz == nullptr) return "";
  return thiz->setEnum(name, arr);
}

string CommandClass::setSoftEnum(const string& name,
                                 const vector<string>& values) {
  if (thiz == nullptr) return "";
  return thiz->setSoftEnum(name, values);
}

bool CommandClass::addSoftEnumValues(const string& name,
                                     const vector<string>& values) {
  if (thiz == nullptr) return false;
  return thiz->addSoftEnumValues(name, values);
}

bool CommandClass::removeSoftEnumValues(const string& name,
                                        const vector<string>& values) {
  if (thiz == nullptr) return false;
  return thiz->removeSoftEnumValues(name, values);
}
