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


def str2json(data: str):
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


def CreateSellShop(goods_name, typename, price, nbt, bool_needtag):
	global pshop
	pshop["selldata"][goods_name] = {}
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


def CreateBuyShop(goods_name, typename, price, nbt):
	global pshop
	try:
		nbt["Count"] = 1
	except:
		pass
	pshop["buydata"][goods_name] = {}
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


# 经济部分
def GetMoney(player):
	return getMoney(player.xuid)


def AddMoney(player, num):
	return addMoney(player.xuid, num)


def ReduceMoney(player, num):
	return reduceMoney(player.xuid, num)


def GetPlayerInventoryItems(player, itemname, needtag, ench):
	playeritems = player.getInventory().getAllItems()
	itemnum = 0
	for pitem in playeritems:
		nbt = pitem.getNbt()
		if nbt["Name"].asString() == itemname:
			if not needtag:
				itemnum += nbt["Count"].asByte()
			else:
				try:
					ench["Count"] = nbt["Count"]
					if nbt == ench:
						itemnum += nbt["Count"].asByte()
				except:
					pass

	return itemnum


def RemovePlayerInventoryItems(player: Player, itemname, num, needtag, ench):
	if GetPlayerInventoryItems(player, itemname, needtag, ench) < num:
		return False
	playeritems = player.getInventory().getAllItems()
	slot = 0
	itemneedreomve = num
	for pitem in playeritems:
		nbt = pitem.getNbt()
		if nbt["Name"].asString() == itemname:
			if not needtag:
				itemCount = nbt["Count"].asByte()
				if itemCount >= itemneedreomve:
					player.removeItem(int(slot), int(itemneedreomve))
					return True
				else:
					player.removeItem(int(slot), int(itemCount))
					itemneedreomve -= itemCount
			else:
				try:
					ench["Count"] = nbt["Count"]
					if nbt == ench:
						itemCount = nbt["Count"].asByte()
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


def PshopBuy3(player, selected):
	num = int(selected[2])
	if num < 0:
		player.sendText("§e[shop]§c数量必须为正数")
		return
	shop = GetBuyShop(player.getExtraData("gn"))
	price = shop["price"]
	money = int(price * num)
	nbt = NBT.fromSNBT(shop["nbt"])
	buynum = nbt["Count"].asByte()
	if num * buynum > 1500 or num > 30:
		player.sendText("§e[shop]§c购买失败, 物品过多！")
		return
	if ReduceMoney(player, money):
		# shop['nbt']['Count1']=int(num*buynum)
		for i in range(num):
			item = Item()
			item.setNbt(nbt)
			player.giveItem(item)
		# shop['nbt']['Count1']=int(buynum)
		player.sendText("§e[shop]§a成功购买, 花费金币: " + str(money))
	else:
		player.sendText("§e[shop]§c余额不足")


def PshopBuy2(player, selected):
	goods_name = pshop["listbuy"][selected]
	shop = GetBuyShop(goods_name)
	string_nbt = NBT.fromSNBT(shop["nbt"]).toJson(4)
	cf = CustomForm("商品信息")
	cf.addLabel(goods_name)
	cf.addLabel("§e单价：" + str(shop["price"]) + "/个")
	cf.addSlider("购买数量：", 0, 64)  # 2
	cf.addLabel("物品NBT信息：\n" + string_nbt)
	player.setExtraData("gn", goods_name)
	cf.send(player, PshopBuy3)


def PshopBuy(player, selected):
	typename = pshop["typename"][selected]
	sf = SimpleForm("回收商店", "请选择商品")
	if not pshop["listbuy"]:
		player.sendText("§e[shop]§c商店暂时没有货物")
		return
	for goods_name in pshop["listbuy"]:
		if GetBuyShop(goods_name)["typename"] == typename:
			sf.addButton(goods_name)
	sf.send(player, PshopBuy2)


def PshopSell3(player, selected):
	num = selected[3]
	if num < 0:
		player.sendText("§e[shop]§c数量必须为正数")
		return
	shop = GetSellShop(player.getExtraData("gn"))
	needtag = shop["needtag"]
	nbt = NBT.fromSNBT(shop["nbt"])
	itemname = nbt["Name"].asString()
	if RemovePlayerInventoryItems(player, itemname, num, needtag, nbt):
		price = shop["price"]
		money = int(price * num)
		AddMoney(player, money)
		player.sendText("§e[shop]§a回收成功, 获得金币: " + str(money))
	else:
		player.sendText("§e[shop]§c物品不足")


def PshopSell2(player, selected):
	goods_name = pshop["listsell"][selected]
	shop = GetSellShop(goods_name)
	nbt = NBT.fromSNBT(shop["nbt"])
	string_nbt = NBT.fromSNBT(shop["nbt"]).toJson(4)
	cf = CustomForm("商品信息")
	cf.addLabel(goods_name)
	cf.addLabel("§e单价：" + str(shop["price"]) + "/个")
	cf.addLabel(
	    "§9我拥有的数量：" + str(
	        GetPlayerInventoryItems(player, nbt["Name"], shop["needtag"], nbt)
	    )
	)
	cf.addSlider("回收数量", 0, 64)
	cf.addLabel("物品NBT信息：\n" + string_nbt)
	player.setExtraData("gn", goods_name)
	cf.send(player, PshopSell3)


def PshopSell(player, selected):
	typename = pshop["typename"][selected]
	sf = SimpleForm("出售商店", "请选择商品")
	if not pshop["listsell"]:
		player.sendText("§e[shop]§c商店暂时没有货物")
		return
	for goods_name in pshop["listsell"]:
		if GetSellShop(goods_name)["typename"] == typename:
			sf.addButton(goods_name)
	sf.send(player, PshopSell2)


def PshopManage(player, selected):
	# create type
	if selected == 0:
		cf = CustomForm("创建商品分类")
		cf.addInput("请输入类型名称")

		def CreateShopType(player, selected):
			typename = selected[0]
			if typename == "":
				player.sendText("§e[shop]§c名称不能为空")
				return
			if typename in pshop["typename"]:
				player.sendText("§e[shop]§c该分类已存在")
				return
			pshop["typename"].append(typename)
			player.sendText("§e[shop]§a创建成功")
			UpdateConfig("pshop.json", pshop)

		cf.send(player, CreateShopType)
	# delete type
	elif selected == 1:
		cf = CustomForm("删除商品分类")
		cf.addDropdown("选择类型", pshop["typename"])

		def DeleteShopType(player, selected):
			typename = pshop["typename"][selected[0]]
			if typename == "":
				player.sendText("§e[shop]§c请先创建商品类型")
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
			player.sendText("§e[shop]§a删除成功")

		cf.send(player, DeleteShopType)
	# buy
	elif selected == 2:
		cf = CustomForm("添加手持物品")
		cf.addInput("输入商品名称")
		cf.addInput("输入单价")
		cf.addDropdown("选择商品类型", pshop["typename"])

		def AddBuy(player, selected):
			if not selected:
				return
			goods_name = selected[0]
			if goods_name == "":
				player.sendText("§e[shop]§c请输入商品名称")
				return
			typename = pshop["typename"][selected[2]]
			if typename == "无":
				player.sendText("§e[shop]§c请先创建商品类型")
				return
			if selected[1].isdigit():
				price = int(selected[1])
				if price < 0:
					player.sendText("§e[shop]§c价格不能为负数")
					return
				nbt = player.getHand().getNbt().toSNBT(0, SnbtFormat.Minimize)
				CreateBuyShop(goods_name, typename, price, nbt)
				player.sendText("§e[shop]§a添加成功")
			else:
				player.sendText("§e[shop]§c请输入正确的价格")
				return

		cf.send(player, AddBuy)
	elif selected == 3:
		cf = CustomForm("删除购买商品")
		cf.addDropdown("请选择商品", pshop["listbuy"])

		def RemoveBuy(player, selected):
			if not selected:
				return
			goods_name = pshop["listbuy"][selected[0]]
			if RemoveBuyShop(goods_name):
				player.sendText("§e[shop]§a移除成功")
			else:
				player.sendText("§e[shop]§c移除失败")

		cf.send(player, RemoveBuy)
	# sell
	elif selected == 4:
		cf = CustomForm("添加手持物品")
		cf.addInput("输入商品名称")
		cf.addInput("输入单价")
		cf.addToggle("是否需要nbt一致", False)
		cf.addDropdown("选择商品类型", pshop["typename"])

		def AddSell(player, selected):
			if not selected:
				return
			goods_name = selected[0]
			if goods_name == "":
				player.sendText("§e[shop]§c请输入商品名称")
				return
			typename = pshop["typename"][selected[3]]
			if typename == "无":
				player.sendText("§e[shop]§c请先创建商品类型")
				return
			if selected[1].isdigit():
				price = int(selected[1])
				if price < 0:
					player.sendText("§e[shop]§c价格不能为负数")
					return
				nbt = player.getHand().getNbt().toSNBT(0, SnbtFormat.Minimize)
				CreateSellShop(
				    goods_name,
				    typename,
				    price,
				    nbt,
				    selected[2],
				)
				player.sendText("§e[shop]§a添加成功")
			else:
				player.sendText("§e[shop]§c请输入正确的价格")
				return

		cf.send(player, AddSell)
	elif selected == 5:
		cf = CustomForm("删除出售商品")
		cf.addDropdown("请选择商品", pshop["listsell"])

		def RemoveSell(player, selected):
			goods_name = pshop["listsell"][selected[0]]
			if RemoveSellShop(goods_name):
				player.sendText("§e[shop]§a移除成功")
			else:
				player.sendText("§e[shop]§c移除失败")

		cf.send(player, RemoveSell)


def PshopInit():
	global pshop
	MakeDirs(home)

	pshop["selldata"] = {}
	pshop["buydata"] = {}
	pshop["listsell"] = []
	pshop["listbuy"] = []
	pshop["typename"] = []

	if not os.path.exists(home + "pshop.json"):
		UpdateConfig("pshop.json", pshop)
		logger.info("创建配置文件：pshop.json")
	pshop = ReadConfig("pshop.json")

	def PshopCallback(origin, results):
		player = origin.player
		op = str(results["operator"])
		if op == "buy":
			if not pshop["typename"]:
				player.sendText("§e[shop]§c商店还没有设置商品分类")
				return
			sf = SimpleForm("购买商店", "选择商品类型")
			for shoptype in pshop["typename"]:
				sf.addButton(shoptype)
			sf.send(player, PshopBuy)
		elif op == "sell":
			if not pshop["typename"]:
				player.sendText("§e[shop]§c商店还没有设置商品分类")
				return
			sf = SimpleForm("出售商店", "选择商品类型")
			for shoptype in pshop["typename"]:
				sf.addButton(shoptype)
			sf.send(player, PshopSell)
		elif op == "manage":
			if not player.isOP():
				player.sendText("§e[shop]§c权限不足")
				return False
			sf = SimpleForm("商店管理员", "请选择")
			sf.addButton("创建商品分类")
			sf.addButton("删除商品分类")
			sf.addButton("添加出售商店")
			sf.addButton("移除出售商店")
			sf.addButton("添加回收商店")
			sf.addButton("移除回收商店")
			sf.send(player, PshopManage)

	c = Command("pshop", "商店命令", CommandPermissionLevel.Any)
	c.mandatory(
	    "operator", ParameterType.SoftEnum,
	    c.setSoftEnum("operator", ["buy", "sell", "manage"])
	)
	c.overload(["operator"])
	c.setCallback(PshopCallback)
	c.setup()


PshopInit()
