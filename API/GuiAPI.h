#pragma once
#include <Global.hpp>
#include <FormUI.h>

struct PlayerClass;
struct SimpleFormClass {
	Form::SimpleForm thiz;

	SimpleFormClass(const string& title, const string& content = "");

	bool sendForm(const PlayerClass& target, const py::function& callback);

	//³ÉÔ±º¯Êý
	SimpleFormClass& setTitle(const string& title);
	SimpleFormClass& setContent(const string& content);
	SimpleFormClass& addButton(const string& text, const string& image = "");
};


struct CustomFormClass {
	Form::CustomForm thiz;

	CustomFormClass(const string& title);

	bool sendForm(const PlayerClass& target, const py::function& callback);

	CustomFormClass& setTitle(const string& title);
	CustomFormClass& addLabel(const string& text);
	CustomFormClass& addInput(const string& name, const string& title, const string& placeholder = "", const string& default_ = "");
	CustomFormClass& addToggle(const string& name, const string& title, bool default_ = false);
	CustomFormClass& addDropdown(const string& name, const string& title, const vector<string>& options, int default_id = 0);
	CustomFormClass& addSlider(const string& name, const string& title, int min, int max, int step = 1, int default_ = 0);
	CustomFormClass& addStepSlider(const string& name, const string& title, const vector<string>& options, int default_id = 0);
};
