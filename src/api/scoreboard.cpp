#include "scoreboard.h"

#include <MC/Objective.hpp>
#include <MC/Player.hpp>
#include <MC/Scoreboard.hpp>
#include <optional>

#include "player.h"
#include "utils.h"

ObjectiveClass::ObjectiveClass(Objective* obj) : thiz(obj) {}

string ObjectiveClass::getName() {
  if (!thiz) return {};
  return thiz->getName();
}

string ObjectiveClass::getDisplayName() {
  if (!thiz) return {};
  return thiz->getDisplayName();
}

bool ObjectiveClass::setDisplay(const string& slot, int sort) {
  if (!thiz) return {};
  return thiz->setDisplay(slot, (ObjectiveSortOrder)sort);
}

std::optional<int> ObjectiveClass::setScore(const string& id, int score) {
  if (!thiz) return {};
  return Scoreboard::setScore(getName(), id, score);
}

std::optional<int> ObjectiveClass::addScore(const string& id, int score) {
  if (!thiz) return {};
  return Scoreboard::addScore(getName(), id, score);
}

std::optional<int> ObjectiveClass::reduceScore(const string& id, int score) {
  if (!thiz) return {};
  return Scoreboard::reduceScore(getName(), id, score);
}

bool ObjectiveClass::deleteScore(const string& id) {
  if (!thiz) return {};
  return Scoreboard::removeFromObjective(getName(), id);
}

int ObjectiveClass::getScore(const string& id) {
  if (!thiz) return {};
  return Scoreboard::getScore(getName(), id);
}
