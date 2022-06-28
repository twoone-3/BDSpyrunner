import mc
import json


class CustomForm:

	def __init__(self, title: str):
		self.title = title
		self.content = []

	def addLabel(self, text):
		'''添加文本标签'''
		j = {}
		j["type"] = "label"
		j["text"] = text
		self.content.append(j)

	def addInput(self, text, placeholder=''):
		'''添加输入框'''
		j = {}
		j["type"] = "input"
		j["text"] = text
		j["placeholder"] = placeholder
		self.content.append(j)

	def addToggle(self, text, default):
		'''添加开关'''
		j = {}
		j["type"] = "toggle"
		j["text"] = text
		j["default"] = default
		self.content.append(j)

	def addDropdown(self, text, options):
		'''添加下拉框'''
		j = {}
		j["type"] = "dropdown"
		j["text"] = text
		if options == []:
			options = ['null']
		j["options"] = options
		self.content.append(j)

	def addSlider(self, text, pmin, pmax, step, default):
		'''添加滑块'''
		j = {}
		j["type"] = "slider"
		j["text"] = text
		j["min"] = pmin
		j["max"] = pmax
		j["step"] = step
		j["default"] = default
		self.content.append(j)

	def addStepSlider(self, text, steps, default):
		'''添加矩阵滑块'''
		j = {}
		j["type"] = "step_slider"
		j["text"] = text
		j["steps"] = steps
		j["default"] = default
		self.content.append(j)

	def send(self, player, cb):
		fj = {}
		fj["type"] = "custom_form"
		fj["title"] = self.title
		fj["content"] = self.content
		return player.sendCustomForm(json.dumps(fj, ensure_ascii=False), cb)


class SimpleForm:

	def __init__(self, title: str, content: str = ''):
		self.title = title
		self.content = content
		self.buttons = []
		self.images = []

	def addButton(self, button: str, image: str = ''):
		self.buttons.append(button)
		self.images.append(image)

	def send(self, player, cb):
		return player.sendSimpleForm(
		    self.title, self.content, self.buttons, self.images, cb
		)