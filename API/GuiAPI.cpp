#include <Global.hpp>
#include "GuiAPI.h"
#include "PlayerAPI.h"
#include "McAPI.h"
#include <SendPacketAPI.h>
#include <MC/Player.hpp>

using namespace std;
using namespace Form;

SimpleFormClass::SimpleFormClass(const string& title, const string& content) : thiz(title, content) {}

bool SimpleFormClass::sendForm(const PlayerClass& target, const py::function& callback) {
	return thiz.sendTo(target.thiz,
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

CustomFormClass::CustomFormClass(const string& title) : thiz(title) {}

//成员函数
bool CustomFormClass::sendForm(const PlayerClass& target, const py::function& callback) {
	return thiz.sendTo(target.thiz,
		[callback](Player* pl, map<string, std::shared_ptr<CustomFormElement>> data) {
			if (LL::isServerStopping())
				return;
			py::dict res;
			for (auto& [name, element] : data) {
				switch (element->getType()) {
				case Form::CustomFormElement::Type::Label:
					break;
				case Form::CustomFormElement::Type::Input:
					res[name.c_str()] = element->getString();
					break;
				case Form::CustomFormElement::Type::Toggle:
					res[name.c_str()] = element->getBool();
					break;
				case Form::CustomFormElement::Type::Dropdown:
					res[name.c_str()] = element->getNumber();
					break;
				case Form::CustomFormElement::Type::Slider:
					res[name.c_str()] = element->getNumber();
					break;
				case Form::CustomFormElement::Type::StepSlider:
					res[name.c_str()] = element->getNumber();
					break;
				default:
					break;
				}
			}
			call(callback, PlayerClass(pl), res);
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

CustomFormClass& CustomFormClass::addInput(const string& name, const string& title, const string& placeholder, const string& default_) {
	thiz.addInput(name, title, placeholder, default_);
	return *this;
}

CustomFormClass& CustomFormClass::addToggle(const string& name, const string& title, bool default_) {
	thiz.addToggle(name, title, default_);
	return *this;
}

CustomFormClass& CustomFormClass::addDropdown(const string& name, const string& title, const vector<string>& options, int default_id) {
	thiz.addDropdown(name, title, options, default_id);
	return *this;
}

CustomFormClass& CustomFormClass::addSlider(const string& name, const string& title, int min, int max, int step, int default_) {
	thiz.addSlider(name, title, min, max, step, default_);
	return *this;
}

CustomFormClass& CustomFormClass::addStepSlider(const string& name, const string& title, const vector<string>& options, int default_id) {
	thiz.addStepSlider(name, title, options, default_id);
	return *this;
}
