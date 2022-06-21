#include <Global.hpp>
#include "GuiAPI.h"
#include "PlayerAPI.h"
#include "McAPI.h"
#include <SendPacketAPI.h>
#include <MC/Player.hpp>

using namespace std;
using namespace Form;

SimpleFormClass::SimpleFormClass() : thiz("", "") {}

bool SimpleFormClass::sendForm(Player* player, const py::function& callback) {
	return thiz.sendTo(player,
		[callback](Player* pl, int chosen) {
			if (LL::isServerStopping())
				return;
			call(callback, PlayerClass(pl), chosen);
		});
}

//成员函数

SimpleFormClass& SimpleFormClass::setTitle(const string& title) {
	thiz.setTitle(title);
	return *this;
}

SimpleFormClass& SimpleFormClass::setContent(const string& content) {
	thiz.setContent(content);
	return *this;
}

SimpleFormClass& SimpleFormClass::addButton(const string& text, const string& image) {
	thiz.addButton(text, image);
	return *this;
}

//////////////////// Custom Form ////////////////////

CustomFormClass::CustomFormClass() : thiz("") {}

//成员函数
bool CustomFormClass::sendForm(Player* player, const py::function& callback) {
	return thiz.sendToForRawJson(player,
		[callback](Player* pl, string data) {
			if (LL::isServerStopping())
				return;
			call(callback, PlayerClass(pl), py::eval(data));
		});
}

CustomFormClass& CustomFormClass::setTitle(const string& title) {
	thiz.setTitle(title);
	return *this;
}

CustomFormClass& CustomFormClass::addLabel(const string& text) {
	thiz.addLabel(text, text);
	return *this;
}

CustomFormClass& CustomFormClass::addInput(const string& name, const string& title, const string& placeholder, const string& def) {
	thiz.addInput(name, title, placeholder, def);
	return *this;
}

CustomFormClass& CustomFormClass::addSwitch(const string& name, const string& title, bool def) {
	thiz.addToggle(name, title, def);
	return *this;
}

CustomFormClass& CustomFormClass::addDropdown(const string& name, const string& title, const vector<string>& options, int defId) {
	thiz.addDropdown(name, title, options, defId);
	return *this;
}

CustomFormClass& CustomFormClass::addSlider(const string& name, const string& title, int min, int max, int step, int def) {
	thiz.addSlider(name, title, min, max, step, def);
	return *this;
}

CustomFormClass& CustomFormClass::addStepSlider(const string& name, const string& title, const vector<string>& options, int defId) {
	thiz.addStepSlider(name, title, options, defId);
	return *this;
}

//////////////////// APIs ////////////////////

// auto McClass::newSimpleForm() {
//	return SimpleFormClass::newForm();
// }
//
// auto McClass::newCustomForm() {
//	return CustomFormClass::newForm();
// }