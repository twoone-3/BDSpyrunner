# -*- coding: utf-8 -*-
# 插件作者：05007，QQ：654921949
# 参与维护：twoone3
# 代码开源可供参考
# 转载发布必须经过作者授权！
import json
import os
from mc import *
from _form import *

VERSION = "2.0.0"
home = "plugins\\pshop\\"
logger = Logger("pshop")
pshop = {}


def str2json(data):
	jsondata = json.loads(data)
	return jsondata


def json2str(data):
	data2 = json.dumps(data, ensure_ascii=False)
	return data2


def json2fmtstr(data):
	data2 = json.dumps(
	    data, indent=4, separators=(", ", ": "), ensure_ascii=False
	)
	return data2


def MakeDirs(path):
	"""创建文件夹"""
	return os.makedirs(os.getcwd() + os.sep + path, exist_ok=True)


def ReadConfig(path):
	"""读取配置"""
	with open(home + path) as json_file:
		config = json.load(json_file)
	return config


def UpdateConfig(path, config):
	"""更新配置"""
	with open(home + path, "w") as json_file:
		json.dump(
		    config,
		    json_file,
		    indent=4,
		    separators=(", ", ": "),
		    ensure_ascii=False
		)
	return None


def CreateSellShop(goods_name, typename, scoreboard, price, nbt, bool_needtag):
	global pshop
	pshop["selldata"][goods_name] = {}
	pshop["selldata"][goods_name]["scoreboard"] = scoreboard
	pshop["selldata"][goods_name]["price"] = price
	pshop["selldata"][goods_name]["typename"] = typename
	pshop["selldata"][goods_name]["nbt"] = nbt
	pshop["selldata"][goods_name]["needtag"] = bool_needtag
	if goods_name not in pshop["listsell"]:
		pshop["listsell"].append(goods_name)
	UpdateConfig("pshop.json", pshop)


def GetSellShop(goods_name):
	global pshop
	try:
		return pshop["selldata"][goods_name]
	except:
		return {}


def RemoveSellShop(goods_name):
	global pshop
	if GetSellShop(goods_name) == {}:
		return False
	pshop["listsell"].remove(goods_name)
	del pshop["selldata"][goods_name]
	UpdateConfig("pshop.json", pshop)
	return True


def CreateBuyShop(goods_name, typename, scoreboard, price, nbt):
	global pshop
	try:
		nbt["Count1"] = 1
	except:
		pass
	pshop["buydata"][goods_name] = {}
	pshop["buydata"][goods_name]["scoreboard"] = scoreboard
	pshop["buydata"][goods_name]["price"] = price
	pshop["buydata"][goods_name]["typename"] = typename
	pshop["buydata"][goods_name]["nbt"] = nbt
	if goods_name not in pshop["listbuy"]:
		pshop["listbuy"].append(goods_name)
	UpdateConfig("pshop.json", pshop)


def GetBuyShop(goods_name):
	global pshop
	if goods_name in pshop["buydata"]:
		return pshop["buydata"][goods_name]
	else:
		return {}


def RemoveBuyShop(goods_name):
	global pshop
	if GetBuyShop(goods_name) == {}:
		return False
	pshop["listbuy"].remove(goods_name)
	del pshop["buydata"][goods_name]
	UpdateConfig("pshop.json", pshop)
	return True


# ====================经济部分(暂时用计分板)==========================================
def GetMoney(player, scoreboard):
	return player.getScore(scoreboard)


def AddMoney(player, num, scoreboard):
	player.addScore(scoreboard, num)


def ReduceMoney(player, num, scoreboard):
	if GetMoney(player, scoreboard) < num:
		return False
	player.reduceScore(scoreboard, num)
	return True


# ===================================================================================
def GetPlayerInventoryItems(player, itemname, needtag, ench):
	playeritems = player.getInventory()
	itemnum = 0
	for pitem in playeritems:
		if pitem["Name8"] == itemname:
			if not needtag:
				itemnum += pitem["Count1"]
			else:
				try:
					ench["Count1"] = pitem["Count1"]
					if pitem == ench:
						itemnum += pitem["Count1"]
				except:
					pass

	return itemnum


def RemovePlayerInventoryItems(player: Player, itemname, num, needtag, ench):
	if GetPlayerInventoryItems(player, itemname, needtag, ench) < num:
		return False
	playeritems = player.getInventory()
	slot = 0
	itemneedreomve = num
	for pitem in playeritems:
		if pitem["Name8"] == itemname:
			if not needtag:
				itemCount = pitem["Count1"]
				if itemCount >= itemneedreomve:
					player.removeItem(int(slot), int(itemneedreomve))
					return True
				else:
					player.removeItem(int(slot), int(itemCount))
					itemneedreomve -= itemCount
			else:
				try:
					ench["Count1"] = pitem["Count1"]
					if pitem == ench:
						itemCount = pitem["Count1"]
						if itemCount >= itemneedreomve:
							player.removeItem(int(slot), int(itemneedreomve))
							return True
						else:
							player.removeItem(int(slot), int(itemCount))
							itemneedreomve -= itemCount
				except:
					pass
		slot += 1
	return True


# ====================================================================================


def PshopBuy2(player, selected):
	goods_name = pshop["listbuy"][selected]
	shop = GetBuyShop(goods_name)
	string_nbt = NBT.fromSNBT(shop["nbt"]).toJson(4)
	cf = CustomForm("商品信息")
	cf.addLabel(goods_name)
	cf.addLabel("§e单价：" + str(shop["price"]) + "/个")
	cf.addSlider("count", "购买数量：", 0, 64)
	cf.addLabel("物品NBT信息：\n" + string_nbt)

	def PshopBuy3(player, selected):
		num = selected["count"]
		if num < 0:
			player.tell("§e[shop]§c数量必须为正数")
			return
		shop = GetBuyShop(goods_name)
		price = shop["price"]
		scoreboard = shop["scoreboard"]
		money = int(price * num)
		nbt = NBT.fromSNBT(shop["nbt"])
		buynum = nbt["Count"].asByte()
		if num * buynum > 1500 or num > 30:
			player.tell("§e[shop]§c购买失败, 物品过多！")
			return False
		if ReduceMoney(player, money, scoreboard):
			# shop['nbt']['Count1']=int(num*buynum)
			for i in range(num):
				item = Item()
				item.setNbt(nbt)
				player.giveItem(item)
			# shop['nbt']['Count1']=int(buynum)
			player.tell("§e[shop]§a成功购买, 花费金币: " + str(money))
		else:
			player.tell("§e[shop]§c余额不足")

	cf.sendForm(player, PshopBuy3)


def PshopBuy(player, selected):
	typename = pshop["typename"][selected]
	sf = SimpleForm("回收商店", "请选择商品")
	if not pshop["listbuy"]:
		player.tell("§e[shop]§c商店暂时没有货物")
		return
	for goods_name in pshop["listbuy"]:
		if GetBuyShop(goods_name)["typename"] == typename:
			sf.addButton(goods_name)
	sf.sendForm(player, PshopBuy2)


def PshopSell(player, selected):
	typename = pshop["typename"][selected]
	sf = SimpleForm("出售商店", "请选择商品")
	if not pshop["listsell"]:
		player.tell("§e[shop]§c商店暂时没有货物")
		return
	for goods_name in pshop["listsell"]:
		if GetSellShop(goods_name)["typename"] == typename:
			sf.addButton(goods_name)

	def PshopSell2(player, selected):
		goods_name = pshop["listsell"][selected]
		shop = GetSellShop(goods_name)
		nbt = NBT.fromSNBT(shop["nbt"])
		string_nbt = json2fmtstr(nbt)
		cf = CustomForm("商品信息")
		cf.addLabel(goods_name)
		cf.addLabel("§e单价：" + str(shop["price"]) + "/个")
		cf.addLabel(
		    "§9我拥有的数量：" + str(
		        GetPlayerInventoryItems(
		            player, nbt["Name"], shop["needtag"], nbt
		        )
		    )
		)
		cf.addSlider("count", "回收数量", 0, 64)
		cf.addLabel("物品NBT信息：\n" + string_nbt)

		def PshopSell3(player, selected):
			num = selected["count"]
			if num < 0:
				player.tell("§e[shop]§c数量必须为正数")
				return
			shop = GetSellShop(goods_name)
			needtag = shop["needtag"]
			nbt = shop["nbt"]
			itemname = nbt["Name"].asString()
			if RemovePlayerInventoryItems(player, itemname, num, needtag, nbt):
				price = shop["price"]
				money = int(price * num)
				scoreboard = shop["scoreboard"]
				AddMoney(player, money, scoreboard)
				player.tell("§e[shop]§a回收成功, 获得金币: " + str(money))
			else:
				player.tell("§e[shop]§c物品不足")

		cf.sendForm(player, PshopSell3)

	sf.sendForm(player, PshopSell2)


def PshopManage(player, selected):
	if selected == 0:
		cf = CustomForm("创建商品分类")
		cf.addInput("typename", "请输入类型名称")

		def CreateShopType(player, selected):
			if not selected:
				return
			typename = selected["typename"]
			if typename == "":
				player.tell("§e[shop]§c请输入类型名称")
				return
			if typename not in pshop["typename"]:
				pshop["typename"].append(typename)
				player.tell("§e[shop]§a创建成功")
			UpdateConfig("pshop.json", pshop)

		cf.sendForm(player, CreateShopType)
	elif selected == 1:
		cf = CustomForm("删除商品分类")
		cf.addDropdown("typename", "选择类型", pshop["typename"])

		def DeleteShopType(player, selected):
			if not selected:
				return
			typename = selected["typename"]
			if typename == "无":
				player.tell("§e[shop]§c请先创建商品类型")
				return
			a = []
			for goods_name in pshop["listsell"]:
				if pshop["selldata"][goods_name]["typename"] == typename:
					del pshop["selldata"][goods_name]
					a.append(goods_name)
			for goods_name in a:
				pshop["listsell"].remove(goods_name)
			a = []
			for goods_name in pshop["listbuy"]:
				if pshop["buydata"][goods_name]["typename"] == typename:
					del pshop["buydata"][goods_name]
					a.append(goods_name)
			for goods_name in a:
				pshop["listbuy"].remove(goods_name)
			a = []
			pshop["typename"].remove(typename)
			UpdateConfig("pshop.json", pshop)
			player.tell("§e[shop]§a删除成功")

		cf.sendForm(player, DeleteShopType)
	# buy
	elif selected == 2:
		cf = CustomForm("添加手持物品")
		cf.addInput("name", "输入商品名称")
		cf.addInput("scoreboardname", "使用的计分板名称(默认money)")
		cf.addInput("price", "输入单价")
		cf.addDropdown("typename", "选择商品类型", pshop["typename"])

		def AddBuy(player, selected):
			if not selected:
				return
			goods_name = selected["name"]
			if goods_name == "":
				player.tell("§e[shop]§c请输入商品名称")
				return
			scoreboard = selected["scoreboardname"]
			if scoreboard == "":
				scoreboard = "money"
			typename = selected["typename"]
			if typename == "无":
				player.tell("§e[shop]§c请先创建商品类型")
				return
			if selected["price"].isdigit():
				price = int(selected["price"])
				if price < 0:
					player.tell("§e[shop]§c价格不能为负数")
					return
				nbt = player.getHand().getNbt().toSNBT(0, SnbtFormat.Minimize)
				CreateBuyShop(goods_name, typename, scoreboard, price, nbt)
				player.tell("§e[shop]§a添加成功")
			else:
				player.tell("§e[shop]§c请输入正确的价格")
				return

		cf.sendForm(player, AddBuy)
	elif selected == 3:
		cf = CustomForm("删除购买商品")
		cf.addDropdown("goods", "请选择商品", pshop["listbuy"])

		def RemoveBuy(player, selected):
			if not selected:
				return
			goods_name = selected["goods"]
			if RemoveBuyShop(goods_name):
				player.tell("§e[shop]§a移除成功")
			else:
				player.tell("§e[shop]§c移除失败")

		cf.sendForm(player, RemoveBuy)
	# sell
	elif selected == 4:
		cf = CustomForm("添加手持物品")
		cf.addInput("name", "输入商品名称")
		cf.addInput("scoreboardname", "使用的计分板名称(默认money)")
		cf.addInput("price", "输入单价")
		cf.addToggle("need_same_nbt", "是否需要nbt一致", False)
		cf.addDropdown("typename", "选择商品类型", pshop["typename"])

		def AddSell(player, selected):
			if not selected:
				return
			goods_name = selected["name"]
			if goods_name == "":
				player.tell("§e[shop]§c请输入商品名称")
				return
			scoreboard = selected["scoreboardname"]
			if scoreboard == "":
				scoreboard = "money"
			typename = selected["typename"]
			if typename == "无":
				player.tell("§e[shop]§c请先创建商品类型")
				return
			if selected["price"].isdigit():
				price = int(selected["price"])
				if price < 0:
					player.tell("§e[shop]§c价格不能为负数")
					return
				nbt = player.getHand().getNbt().toSNBT(0, SnbtFormat.Minimize)
				CreateSellShop(
				    goods_name,
				    typename,
				    scoreboard,
				    price,
				    nbt,
				    selected["need_same_nbt"],
				)
				player.tell("§e[shop]§a添加成功")
			else:
				player.tell("§e[shop]§c请输入正确的价格")
				return

		cf.sendForm(player, AddSell)
	elif selected == 5:
		cf = CustomForm("删除出售商品")
		cf.addDropdown("goods", "请选择商品", pshop["listsell"])

		def RemoveSell(player, selected):
			goods_name = selected["goods"]
			if RemoveSellShop(goods_name):
				player.tell("§e[shop]§a移除成功")
			else:
				player.tell("§e[shop]§c移除失败")

		cf.sendForm(player, RemoveSell)


# ====================================================================================
def PshopInit():
	global pshop
	MakeDirs(home)

	if not os.path.exists(home + "pshop.json"):
		UpdateConfig("pshop.json", pshop)
		logger.info("创建配置文件：pshop.json")
	pshop["selldata"] = {}
	pshop["buydata"] = {}
	pshop["listsell"] = []
	pshop["listbuy"] = []
	pshop["typename"] = []
	pshop = ReadConfig("pshop.json")

	def PshopCallback(origin, results):
		player = origin.player
		op = str(results["operator"])
		if op == "buy":
			if not pshop["typename"]:
				player.tell("§e[shop]§c商店还没有设置商品分类")
				return
			sf = SimpleForm("购买商店", "选择商品类型")
			for shoptype in pshop["typename"]:
				sf.addButton(shoptype)
			sf.sendForm(player, PshopBuy)
		elif op == "sell":
			if not pshop["typename"]:
				player.tell("§e[shop]§c商店还没有设置商品分类")
				return
			sf = SimpleForm("出售商店", "选择商品类型")
			for shoptype in pshop["typename"]:
				sf.addButton(shoptype)
			sf.sendForm(player, PshopSell)
		elif op == "manage":
			if not player.isOP():
				player.tell("§e[shop]§c权限不足")
				return False
			sf = SimpleForm("商店管理员", "请选择")
			sf.addButton("创建商品分类")
			sf.addButton("删除商品分类")
			sf.addButton("添加出售商店")
			sf.addButton("移除出售商店")
			sf.addButton("添加回收商店")
			sf.addButton("移除回收商店")
			sf.sendForm(player, PshopManage)

	c = Command("pshop", "商店命令", CommandPermissionLevel.Any)
	c.mandatory(
	    "operator",
	    ParameterType.SoftEnum,
	    c.setSoftEnum("operator", ["buy", "sell", "manage"]),
	)
	c.overload(["operator"])
	c.setCallback(PshopCallback)
	c.setup()

	def onJoin(e):
		e["Player"].addScore("money", 0)  # 添加计分板

	setListener("onJoin", onJoin)


PshopInit()
