#pragma once
#include <Global.hpp>

#include <FormUI.h>

struct SimpleFormClass {
	Form::SimpleForm thiz;

	SimpleFormClass();

	bool sendForm(Player* player, const py::function& callback);

	//³ÉÔ±º¯Êý
	SimpleFormClass& setTitle(const string& title);
	SimpleFormClass& setContent(const string& content);
	SimpleFormClass& addButton(const string& text, const string& image);
};


struct CustomFormClass {
	Form::CustomForm thiz;

	CustomFormClass();

	bool sendForm(Player* player, const py::function& callback);

	CustomFormClass& setTitle(const string& title);
	CustomFormClass& addLabel(const string& text);
	CustomFormClass& addInput(const string& name, const string& title, const string& placeholder = "", const string& def = "");
	CustomFormClass& addSwitch(const string& name, const string& title, bool def = false);
	CustomFormClass& addDropdown(const string& name, const string& title, const vector<string>& options, int defId = 0);
	CustomFormClass& addSlider(const string& name, const string& title, int min, int max, int step = 1, int def = 0);
	CustomFormClass& addStepSlider(const string& name, const string& title, const vector<string>& options, int defId = 0);
};
