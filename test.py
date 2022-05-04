import mc
import time

# some tool functions
def listToStr(list: list):
	result = ", ".join([str(element) for element in list])
	return f"({result})"

def pVT(variable): # printVariableTypes
	return type(variable)[:1] 

def printLog(content):
	print(f"[{time.strftime('%H:%M:%S')}]{content}")
	#mc.logout(time.strftime('%H:%M:%S') + content + "\n")

# entity test results
'''
variable prefix definition:
	s_: string
	i_: int
	l_: list
	b_: bool
	f_: float
	e_: entity

Entity的属性
name:str - 实体名字                          # passing
uuid:str - 玩家UUID                          # passing
xuid:str - 玩家XUID                          # passing  
pos:list - 实体坐标                          # passing
did:int - 实体维度ID                         # passing
is_standing:bool - 实体是否站立在方块上       # passing
is_sneaking:bool - 实体是否潜行              # passing
health:int - 实体当前生命                    # output unknown
maxhealth:int - 实体最大生命                 # passing
perm:int - 玩家权限值                        # passing
platform_online_id:str - 玩家设备ID          # empty string
platform:int - 玩家设备操作系统               # passing
IP:str - 玩家IP                              # passing
'''



# test functions
def testonConsoleCmd(cmd): #testonConsoleInput
	printLog("[TEST] Listener onConsoleInput s_cmd = " + cmd)
#mc.setListener('onConsoleCmd', testonConsoleCmd) # onConsoleInput -> onConsoleCmd
# status: passing

def testonConsoleOutput(output):
	printLog("[TEST] Listener onConsoleOutput s_output = " + output)
#mc.setListener('onConsoleOutput', testonConsoleOutput)
# status: passing

def testonJoin(player): #testonPlayerJoin
	printLog("[TEST] Listener onJoin s_player.name = " + str(player.name) + "s_player.xuid = " + str(player.xuid))
#mc.setListener('onJoin', testonJoin) #onPlayerJoin -> onJoin
# status: passing

def testonLeft(player): #testonPlayerLeft
	printLog(f"[TEST] Listener onLeft s_player.uuid = " + player.uuid + "s_player.IP = " + player.IP)
#mc.setListener('onLeft', testonLeft) #onPlayerLeft -> onLeft
# status: passing

def testonPlayerAttack(e):
	printLog(f"[TEST] Listener onPlayerAttack l_e['player'].pos = {listToStr(e['player'].pos)}, e['actor'].health = {e['actor'].typename}")
	playerList = mc.getPlayerList()
	printLog("[TEST] mcAPI l_getPlayerList = %s" % (playerList))
#mc.setListener('onPlayerAttack', testonPlayerAttack)
# status: passing

def testonFormSelected(e): #testonSelectForm
	printLog(f"[TEST] Listener onFormSelected i_e['formid'] = {e['formid']}, i_e['selected'] = {type(e['selected'])}, s_e['player'].name = {type(e['player'].name)}")
	e['player'].teleport(0,100,0,0)
#mc.setListener("onFormSelected", testonFormSelected)
# status: passing
# import mc
# mc.getPlayerList()[0].sendCustomForm('{"content":[{"type":"label","text":"114514"}], "type":"custom_form","title":"test"}')

def testonUseItem(e):
	printLog(f"[TEST] Listener onUseItem i_e['itemid'] = {e['itemid']}, i_e['player'].did = {e['player'].did}")
#mc.setListener("onUseItem", testonUseItem)
# status: passing

def testonPlaceBlock(e):
	printLog(f"[TEST] Listener onPlaceBlock i_e['blockid'] = {e['blockid']}, b_e['player'].is_standing = {e['player'].is_standing}") # Entity.isstand -> Entity.is_standing
#mc.setListener("onPlaceBlock", testonPlaceBlock)
# status: passing

def testonDestroyBlock(e):
	printLog(f"[TEST] Listener onDestroyBlock l_e['position'] = {listToStr(e['position'])}, {e['position'][1]}, {e['position'][2]}）, b_e['player'].is_sneaking = {e['player'].is_sneaking}") # issneak -> is_sneaking
#mc.setListener("onDestroyBlock", testonDestroyBlock)
# status: passing

def testonOpenContainer(e):
	printLog(f"[TEST] Listener onOpenContainer l_e['position'] = {listToStr(e['position'])}, i_e['player'].typeid = {e['player'].typeid}")
#mc.setListener("onOpenContainer", testonOpenContainer)
# status: passing

def testonCloseContainer(e):
	printLog(f"[TEST] Listener onCloseContainer l_e['position'] = {listToStr(e['position'])}, s_e['player'].typename = e['player'].typename)")
#mc.setListener("onCloseContainer", testonCloseContainer)
# status: no output

def testonContainerChange(e):
	printLog('[TEST] Listener onContainerChange l_e["position"] = （%f,%f,%f）, i_itemid = %d, i_itemcount = %d, s_IP = %s' %
												(e['position'][0], e['position'][1], e['position'][2], e['itemid'], e['itemcount'], e['player'].IP))
#mc.setListener("onContainerChange", testonContainerChange)
# status: passing

def testonChangeDim(player): #testonChangeDimension
	printLog('[TEST] Listener onChangeDim s_playerDim = %s' % (player.dim))
#mc.setListener("onChangeDim", testonChangeDim)
# status: passing

def testonMobDie(e):
	printLog('[TEST] Listener onMobDie i_e["dmcase"] = %s, e_e["actor1"] = %s, i_e["actor2"].maxhealth = %s' % (e["dmcase"], e["actor1"], e["actor2"].maxhealth))
#mc.setListener("onMobDie", testonMobDie)
#actor1 always return nullptr
#return nullptr when use /kill command to kill mob
# status: passing

def testonMobHurt(e):
	printLog('[TEST] Listener onMobHurt i_e["dmcase"] = %s, i_e["actor1"].health = %s, s_e["actor2"].name = %s, i_e["damage"] = %s' % (e["dmcase"], e["actor1"].health, e["actor2"].name, e["damage"]))
#mc.setListener("onMobHurt", testonMobHurt)
# status: passing

def testonRespawn(player):
	printLog(f'[TEST] Listener onRespawn s_player.xuid = {player.xuid}')
#mc.setListener("onRespawn", testonRespawn)
# status: passing

def testonChat(e):
	printLog(f'[TEST] Listener onChat s_e["sender"] = {e["sender"]}, s_e["target"] = {e["target"]}, s_e["msg"] = {e["msg"]}, s_e["style"] = {e["style"]}')
#mc.setListener("onChat", testonChat)
# status: passing

def testonInputText(e):
	printLog(f'[TEST] Listener onInputText s_e["msg"] = {e["msg"]}, s_e["player"].IP = {e["player"].IP}')
#mc.setListener("onInputText", testonInputText)
# status: passing

def testonCommandBlockUpdate(e):
	printLog(f'[TEST] Listener onCommandBlockUpdate i_e["player"].did = {e["player"].did}, s_e["cmd"] = {e["cmd"]}, s_e["mode"] = {e["mode"]}, s_e["condition"] = {e["condition"]}, s_e["redstone"] = {e["redstone"]}, s_e["output"] = {e["output"]}, s_e["rawname"] = {e["rawname"]}, s_e["delay"] = {e["delay"]}, l_e["position] = {e["position"]} ')
#mc.setListener("onCommandBlockUpdate", testonCommandBlockUpdate)
# status: passing

def testonPlayerCmd(e):
	printLog(f'[TEST] Listener onPlayerCmd s_e["cmd"] = {e["cmd"]}, s_e["player"] = {e["player"]} ')
#mc.setListener("onPlayerCmd", testonPlayerCmd)
# status: passing

def testonCmdBlockExecute(e):
	printLog(f'[TEST] Listener onCmdBlockExecute s_e["cmd"] = {e["cmd"]}, s_e["rawname"] = {e["rawname"]}, l_e["position] = {e["position"]}, s_e["mode"] = {e["mode"]}, s_e["condition"] = {e["condition"]}')
#mc.setListener("onCmdBlockExecute", testonCmdBlockExecute)
# status: passing

def testonLevelExplode(e):
	printLog(f'[TEST] Listener onLevelExplode l_e["actor"].pos = {e["actor"].pos}, l_e["position"] = {e["position"]}, i_dimensionid = {e["dimensionid"]}, s_e["power"] = {e["power"]}')
#mc.setListener("onLevelExplode", testonLevelExplode)
# status: passing

def testonSetArmor(e):
	printLog(f'[TEST] Listener onSetArmor s_e["player"].name = {e["player"].name}, i_e["itemid"] = {e["itemid"]}, i_e["itemcount"] = {e["itemcount"]}, s_e["itemname"] = {e["itemname"]}, s_e["itemaux"] = {e["itemaux"]}, s_e["slot"] = {e["slot"]}')
#mc.setListener("onSetArmor", testonSetArmor)
# status: passing

def testonFarmLandDecay(e): 
	printLog(f'[TEST] Listener onFarmLandDecay i_e["player"].platform_online_id = {e["player"].platform_online_id}, l_e["position"] = {e["position"]}, l_e["dimensionid"] = {e["dimensionid"]}')
#mc.setListener("onFarmLandDecay", testonFarmLandDecay)
# status: passing

def testonUseRespawnAnchor(e): #testonUseRespawnAnchorBlock
	printLog(f'[TEST] Listener onUseRespawnAnchor i_e["player"].perm = {e["player"].perm}, l_e["position"] = {e["position"]}, i_e["dimensionid"] = {e["dimensionid"]}')
#mc.setListener("onUseRespawnAnchor", testonUseRespawnAnchor)
# status: passing

def testonScoreChanged(e):
	printLog(f'[TEST] Listener onScoreChanged i_e["scoreboardid"] = {e["scoreboardid"]}, i_e["playersnum"] = {e["playersnum"]}, s_e["objectivename"] = {e["objectivename"]}, s_e["objectivedisname"] = {e["objectivedisname"]}')
#mc.setListener("onScoreChanged", testonScoreChanged)
# status: no output, recommend that you retest, scripts:
#/scoreboard objectives add test dummy test
#/scoreboard players add @a test 114514

def testonMove(e):
	printLog(f'[TEST] Listener onMove l_e.pos = {listToStr(e.pos)}')
#mc.setListener("onMove", testonMove)
# status: passing

def testonPistonPush(e):
	printLog(f'[TEST] Listener onPistonPush s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}, l_e["blockpos"] = {e["blockpos"]}, l_e["pistonpos"] = e["pistonpos"], i_e["dimensionid"] = {e["dimensionid"]}')
#mc.setListener("onPistonPush", testonPistonPush)
# status: passing with problem
#key pistonpos will cause crash and it has been removed from dll plugin, please fix

def testonEndermanRandomTeleport(e):
	printLog(f'[TEST] Listener onEndermanRandomTeleport l_e.pos = {listToStr(e.pos)}')
#mc.setListener("onEndermanRandomTeleport", testonEndermanRandomTeleport)
# status: passing

def testonServerStarted(e):
	printLog(f'[TEST] Listener onServerStarted')
#mc.setListener("onServerStarted", testonServerStarted)
# status: passing

def testonDropItem(e):
	printLog(f'[TEST] Listener onDropItem s_e["player"].name = {e["player"].name}, i_e["itemid"] = {e["itemid"]}, i_e["itemcount"] = {e["itemcount"]}, s_e["itemname"] = {e["itemname"]}, i_e["itemcount"] = {e["itemaux"]}')
mc.setListener("onDropItem", testonDropItem)
# status: passing

def testonTakeItem(e):
	printLog(f'[TEST] Listener onTakeItem b_e["player"].is_sneaking = {e["player"].is_sneaking}, l_e["actor"].pos = {listToStr(e["actor"].pos)}')
#mc.setListener("onTakeItem", testonTakeItem)
# status: passing

def testonRide(e):
	printLog(f'[TEST] Listener onRide s_e["actor1"].name = {e["actor1"].name}, s_e["actor2"].name = {e["actor2"].name}')
#mc.setListener("onRide", testonRide)
# status: passing

def testonUseFrameBlock(e):
	printLog(f'[TEST] Listener onUseFrameBlock s_e["player"].name = {e["player"].name}, l_e["blockpos"] = {listToStr(e["blockpos"])}, i_e["dimensionid"] = {e["dimensionid"]}')
#mc.setListener("onUseFrameBlock", testonUseFrameBlock)
# status: passing

def testonJump(e):
	printLog(f'[TEST] Listener onJump l_e.pos = {e.pos}')
mc.setListener("onJump", testonJump)
# status: passing

def testonSneak(e):
	printLog(f'[TEST] Listener onSneak s_e.platform_online_id = {e.platform_online_id}')
#mc.setListener("onSneak", testonSneak)
# status: passing

def testonBlockInteracted(e):
	printLog(f'[TEST] Listener onBlockInteracted s_e["player"].name = {e["player"].name}, l_e["blockpos"] = {listToStr(e["blockpos"])}, s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}, i_e["dimensionid"] = {e["dimensionid"]}')
#mc.setListener("onBlockInteracted", testonBlockInteracted)
# status: passing

def testonFireSpread(e):
	printLog(f'[TEST] Listener onFireSpread l_e["blockpos"] = {listToStr(e["blockpos"])}, s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}')
#mc.setListener("onFireSpread", testonFireSpread)
# status: no output

def testonBlockExploded(e):
	printLog(f'[TEST] Listener onBlockExploded s_e["actor"].name = {e["actor"].name}, l_e["blockpos"] = {listToStr(e["blockpos"])}, s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}, i_e["dimensionid"] = {e["dimensionid"]}')
#mc.setListener("onBlockExploded", testonBlockExploded)
# status: passing

def testonUseSignBlock(e):
	printLog(f'[TEST] Listener onUseSignBlock i_e["player"].platform = {e["player"].platform}, s_e["text"] = {e["text"]}, l_e["position"] = {e["position"]}')
#mc.setListener("onUseSignBlock", testonUseSignBlock)
# status: no output

#temple
def test(e):
	printLog(f'[TEST] Listener ')

#mc.setListener('onUseItem', testonUseItem)
#mc.setListener('onPlaceBlock', testonPlaceBlock)
#mc.setListener('onDestroyBlock', testonDestroyBlock)
#mc.setListener('onOpenContainer', testonOpenContainer) # onOpenChest, onOpenBarrel -> onOpenContainer
#mc.setListener('onCloseContainer', testonCloseContainer) # onCloseChest, onCloseBarrel -> onCloseContainer
#no output
#mc.setListener('onContainerChange', testonContainerChange)
#no full output
#mc.setListener('onChangeDim', testonChangeDim) # onChangeDimension -> onChangeDim
#mc.setListener('onMobDie', testonMobDie)
#mc.setListener("onMobHurt", testonMobHurt)
#mc.setListener("onRespawn", testonRespawn)
#mc.setListener("onChat", testonChat)
#mc.setListener("onInputText", testonInputText)
#mc.setListener("onCommandBlockUpdate", testonCommandBlockUpdate)
#mc.setListener("onPlayerCmd", testonPlayerCmd) # onInputCommand -> onPlayerCmd
#mc.setListener("onCmdBlockExecute", testonCmdBlockExecute) # onCommandBlockPerform -> onCmdBlockExecute
#mc.setListener("onLevelExplode", testonLevelExplode)
#mc.setListener("onSetArmor", testonSetArmor)
#mc.setListener("onFarmLandDecay", testonFarmLandDecay) # onFallBlockTransform -> onFarmLandDecay
#mc.setListener("onUseRespawnAnchor", testonUseRespawnAnchor) # onUseRespawnAnchorBlock -> onUseRespawnAnchor
#mc.setListener("onScoreChanged", testonScoreChanged)
#no output
#-mc.setListener("onMove", testonMove) # frist test this listener and then disable it to test other listener
#mc.setListener("onPistonPush", testonPistonPush)
#-mc.setListener("onEndermanRandomTeleport", testonEndermanRandomTeleport)
#-mc.setListener("onServerStarted", testonServerStarted)
#-mc.setListener("onDropItem", testonDropItem)
#-mc.setListener("onTakeItem", testonTakeItem)
#mc.setListener("onRide", testonRide)
#mc.setListener("onUseFrameBlock", testonUseFrameBlock)
#-mc.setListener("onJump", testonJump)
#-mc.setListener("onSneak", testonSneak)
#mc.setListener("onBlockInteracted", testonBlockInteracted)
#mc.setListener("onFireSpread", testonFireSpread)
#mc.setListener("onBlockExploded", testonBlockExploded)
#mc.setListener("onUseSignBlock", testonUseSignBlock)
#no ouput
# temple
#mc.setListener("", test)


'''
def testonOpenChest(e):
	printLog('[TEST] Listener onOpenChest l_e['position'] = （%f,%f,%f）, e['player'].typeid = %d' % (e['position'][0], e['position'][1], e['position'][2], e['player'].typeid))

def testonCloseChest(e):
	printLog('[TEST] Listener onCloseChest l_e['position'] = %f, e['player'].typename = %s' % (e['position'][0], e['position'][1], e['position'][2], e['player'].typename))

def testonOpenBarrel(e):
	printLog('[TEST] Listener onOpenBarrel l_e['position'] = （%f,%f,%f）, e['player'].platform_online_id = %s' % (e['position'][0], e['position'][1], e['position'][2], e['player'].platform_online_id))

def testonCloseBarrel(e):
	printLog('[TEST] Listener onCloseBarrel l_e['position'] = （%f,%f,%f）, e['player'].platform = %d' % (e['position'][0], e['position'][1], e['position'][2], e['player'].platform))
'''
##mc.setListener('onJoin', testonPlayerJoin)
##mc.setListener('onLeft', testonPlayerLeft)
##mc.setListener('onConsoleInput', testonConsoleInput)
##mc.setListener('onOpenBarrel', testonOpenBarrel)
##mc.setListener('onOpenChest', testonOpenChest)
##mc.setListener('onCloseChest', testonCloseChest)
##mc.setListener('onCloseBarrel', testonCloseBarrel)
##mc.setListener('onChangeDimension', testonChangeDimension)

