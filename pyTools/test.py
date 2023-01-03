import mc
import time

# some tool functions
def listToStr(list: list):
	result = ", ".join([str(element) for element in list])
	return f"({result})"

def pVT(variable): # printVariableTypes
	return type(variable)[:1] 

def printLog(content):
	mc.removeListener
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
name:str - 实体名字                          # test
uuid:str - 玩家UUID                          # test
xuid:str - 玩家XUID                          # test  
pos:list - 实体坐标                          # test
did:int - 实体维度ID                         # test
is_standing:bool - 实体是否站立在方块上       # test
is_sneaking:bool - 实体是否潜行              # test
health:int - 实体当前生命                    # test
maxhealth:int - 实体最大生命                 # test
perm:int - 玩家权限值                        # test
platform_online_id:str - 玩家设备ID          # empty string
platform:int - 玩家设备操作系统               # test
IP:str - 玩家IP                              # test

Problematic but available listeners:
onPistonPush
'''



# test functions
def testonConsoleCmd(cmd): #testonConsoleInput
	printLog("[TEST] Listener onConsoleInput s_cmd = " + cmd)
	mc.removeListener('onConsoleCmd', testonConsoleCmd)
mc.setListener('onConsoleCmd', testonConsoleCmd) # onConsoleInput -> onConsoleCmd
# status: test

def testonConsoleOutput(output):
	printLog("[TEST] Listener onConsoleOutput s_output = " + output)
	mc.removeListener('onConsoleOutput', testonConsoleOutput)
mc.setListener('onConsoleOutput', testonConsoleOutput)
# status: test

def testonJoin(player): #testonPlayerJoin
	mc.getPlayerList()[0].sendCustomForm('{"content":[{"type":"label","text":"114514"}], "type":"custom_form","title":"test"}')
	printLog(f"[TEST] Listener onJoin s_player.uuid = " + player.uuid + " s_player.IP = " + player.IP)
	mc.removeListener('onJoin', testonJoin)
mc.setListener('onJoin', testonJoin) #onPlayerJoin -> onJoin
# status: pass

def testonLeft(player): #testonPlayerLeft
	printLog("[TEST] Listener onLeft s_player.name = " + str(player.name) + " s_player.xuid = " + str(player.xuid))
	mc.removeListener('onLeft', testonLeft)
mc.setListener('onLeft', testonLeft) #onPlayerLeft -> onLeft
# status: pass

def testonPlayerAttack(e):
	printLog(f"[TEST] Listener onPlayerAttack l_e['player'].pos = {listToStr(e['player'].pos)}, e['actor'].health = {e['actor'].health}")
	playerList = mc.getPlayerList()
	printLog("[TEST] mcAPI l_getPlayerList = %s" % (playerList))
	mc.removeListener('onPlayerAttack', testonPlayerAttack)
mc.setListener('onPlayerAttack', testonPlayerAttack)
# status: test+

def testonFormSelected(e): #testonSelectForm
	printLog(f"[TEST] Listener onFormSelected i_e['formid'] = {e['formid']}, l_e['selected'] = {e['selected']}, s_e['player'].name = {e['player'].name}")
	e['player'].teleport(0,100,0,0)
	mc.removeListener('onFormSelected', testonFormSelected)
mc.setListener("onFormSelected", testonFormSelected)
# status: test
# import mc
# mc.getPlayerList()[0].sendCustomForm('{"content":[{"type":"label","text":"114514"}], "type":"custom_form","title":"test"}')

def testonUseItem(e):
	printLog(f"[TEST] Listener onUseItem i_e['itemid'] = {e['itemid']}, i_e['player'].did = {e['player'].did}")
	mc.removeListener("onUseItem", testonUseItem)
mc.setListener("onUseItem", testonUseItem)
# status: test

def testonPlaceBlock(e):
	printLog(f"[TEST] Listener onPlaceBlock i_e['blockid'] = {e['blockid']}, b_e['player'].is_standing = {e['player'].is_standing}") # Entity.isstand -> Entity.is_standing
	mc.removeListener("onPlaceBlock", testonPlaceBlock)
mc.setListener("onPlaceBlock", testonPlaceBlock)
# status: test

def testonDestroyBlock(e):
	printLog(f"[TEST] Listener onDestroyBlock l_e['position'] = {listToStr(e['position'])}, {e['position'][1]}, {e['position'][2]}）, b_e['player'].is_sneaking = {e['player'].is_sneaking}") # issneak -> is_sneaking
	mc.removeListener("onDestroyBlock", testonDestroyBlock)
mc.setListener("onDestroyBlock", testonDestroyBlock)
# status: test

def testonOpenContainer(e):
	printLog(f"[TEST] Listener onOpenContainer l_e['position'] = {listToStr(e['position'])}, i_e['player'].typeid = {e['player'].typeid}")
	mc.removeListener("onOpenContainer", testonOpenContainer)
mc.setListener("onOpenContainer", testonOpenContainer)
# status: test

def testonCloseContainer(e):
	printLog(f"[TEST] Listener onCloseContainer l_e['position'] = {listToStr(e['position'])}, s_e['player'].typename = e['player'].typename)")
	mc.removeListener("onCloseContainer", testonCloseContainer)
mc.setListener("onCloseContainer", testonCloseContainer)
# status: test

def testonContainerChange(e):
	printLog('[TEST] Listener onContainerChange l_e["position"] = （%f,%f,%f）, i_itemid = %d, i_itemcount = %d, s_IP = %s' %
												(e['position'][0], e['position'][1], e['position'][2], e['itemid'], e['itemcount'], e['player'].IP))
	mc.removeListener("onContainerChange", testonContainerChange)
mc.setListener("onContainerChange", testonContainerChange)
# status: test

def testonChangeDim(player): #testonChangeDimension
	printLog('[TEST] Listener onChangeDim s_playerDim = %s' % (player.did))
	mc.removeListener("onChangeDim", testonChangeDim)
mc.setListener("onChangeDim", testonChangeDim)
# status: test

def testonMobDie(e):
	if e["actor2"]:
		printLog('[TEST] Listener onMobDie i_e["dmcase"] = %s, e_e["actor1"] = %s, i_e["actor2"].maxhealth = %s' % (e["dmcase"], e["actor1"], e["actor2"].maxhealth))
	else:
		printLog('[TEST] Listener onMobDie i_e["dmcase"] = %s, e_e["actor1"] = %s, i_e["actor2"] = %s' % (e["dmcase"], e["actor1"], e["actor2"]))
	mc.removeListener("onMobDie", testonMobDie)
mc.setListener("onMobDie", testonMobDie)
#actor1 always return nullptr
#return nullptr when use /kill command to kill mob
# status: test

def testonMobHurt(e):
	if e["actor2"]:
		printLog('[TEST] Listener onMobHurt i_e["dmcase"] = %s, i_e["actor1"].health = %s, s_e["actor2"].name = %s, i_e["damage"] = %s' % (e["dmcase"], e["actor1"].health, e["actor2"].name, e["damage"]))
	else:
		printLog('[TEST] Listener onMobHurt i_e["dmcase"] = %s, i_e["actor1"].health = %s, s_e["actor2"] = %s, i_e["damage"] = %s' % (e["dmcase"], e["actor1"].health, e["actor2"], e["damage"]))
	#mc.removeListener("onMobHurt", testonMobHurt)
mc.setListener("onMobHurt", testonMobHurt)
# status: pass

def testonRespawn(player):
	printLog(f'[TEST] Listener onRespawn s_player.xuid = {player.xuid}')
	mc.removeListener("onRespawn", testonRespawn)
mc.setListener("onRespawn", testonRespawn)
# status: test

def testonChat(e):
	printLog(f'[TEST] Listener onChat s_e["sender"] = {e["sender"]}, s_e["target"] = {e["target"]}, s_e["msg"] = {e["msg"]}, s_e["style"] = {e["style"]}')
	mc.removeListener("onChat", testonChat)
mc.setListener("onChat", testonChat)
# status: test

def testonInputText(e):
	printLog(f'[TEST] Listener onInputText s_e["msg"] = {e["msg"]}, s_e["player"].IP = {e["player"].IP}')
	mc.removeListener("onInputText", testonInputText)
mc.setListener("onInputText", testonInputText)
# status: test

def testonCommandBlockUpdate(e):
	printLog(f'[TEST] Listener onCommandBlockUpdate i_e["player"].did = {e["player"].did}, s_e["cmd"] = {e["cmd"]}, s_e["mode"] = {e["mode"]}, s_e["condition"] = {e["condition"]}, s_e["redstone"] = {e["redstone"]}, s_e["output"] = {e["output"]}, s_e["rawname"] = {e["rawname"]}, s_e["delay"] = {e["delay"]}, l_e["position] = {e["position"]} ')
	mc.removeListener("onCommandBlockUpdate", testonCommandBlockUpdate)
mc.setListener("onCommandBlockUpdate", testonCommandBlockUpdate)
# status: test

def testonPlayerCmd(e):
	printLog(f'[TEST] Listener onPlayerCmd s_e["cmd"] = {e["cmd"]}, s_e["player"] = {e["player"]} ')
	mc.removeListener("onPlayerCmd", testonPlayerCmd)
mc.setListener("onPlayerCmd", testonPlayerCmd)
# status: test

def testonCmdBlockExecute(e):
	printLog(f'[TEST] Listener onCmdBlockExecute s_e["cmd"] = {e["cmd"]}, s_e["rawname"] = {e["rawname"]}, l_e["position] = {e["position"]}, s_e["mode"] = {e["mode"]}, s_e["condition"] = {e["condition"]}')
	mc.removeListener("onCmdBlockExecute", testonCmdBlockExecute)
mc.setListener("onCmdBlockExecute", testonCmdBlockExecute)
# status: test

def testonLevelExplode(e):
	printLog(f'[TEST] Listener onLevelExplode l_e["actor"].pos = {e["actor"].pos}, l_e["position"] = {e["position"]}, i_dimensionid = {e["dimensionid"]}, s_e["power"] = {e["power"]}')
	mc.removeListener("onLevelExplode", testonLevelExplode)
mc.setListener("onLevelExplode", testonLevelExplode)
# status: test

def testonSetArmor(e):
	printLog(f'[TEST] Listener onSetArmor s_e["player"].name = {e["player"].name}, i_e["itemid"] = {e["itemid"]}, i_e["itemcount"] = {e["itemcount"]}, s_e["itemname"] = {e["itemname"]}, s_e["itemaux"] = {e["itemaux"]}, s_e["slot"] = {e["slot"]}')
	mc.removeListener("onSetArmor", testonSetArmor)
mc.setListener("onSetArmor", testonSetArmor)
# status: test

def testonFarmLandDecay(e): 
	printLog(f'[TEST] Listener onFarmLandDecay i_e["player"].platform_online_id = {e["player"].platform_online_id}, l_e["position"] = {e["position"]}, l_e["dimensionid"] = {e["dimensionid"]}')
	mc.removeListener("onFarmLandDecay", testonFarmLandDecay)
mc.setListener("onFarmLandDecay", testonFarmLandDecay)
# status: test

def testonUseRespawnAnchor(e): #testonUseRespawnAnchorBlock
	printLog(f'[TEST] Listener onUseRespawnAnchor i_e["player"].perm = {e["player"].perm}, l_e["position"] = {e["position"]}, i_e["dimensionid"] = {e["dimensionid"]}')
	mc.removeListener("onUseRespawnAnchor", testonUseRespawnAnchor)
mc.setListener("onUseRespawnAnchor", testonUseRespawnAnchor)
# status: test

def testonScoreChanged(e):
	printLog(f'[TEST] Listener onScoreChanged i_e["scoreboardid"] = {e["scoreboardid"]}, i_e["playersnum"] = {e["playersnum"]}, s_e["objectivename"] = {e["objectivename"]}, s_e["objectivedisname"] = {e["objectivedisname"]}')
	mc.removeListener("onScoreChanged", testonScoreChanged)
mc.setListener("onScoreChanged", testonScoreChanged)
# status: test
# scripts:
#/scoreboard objectives add test dummy test
#/scoreboard players add @a test 114514

def testonMove(e):
	printLog(f'[TEST] Listener onMove l_e.pos = {listToStr(e.pos)}')
	mc.removeListener("onMove", testonMove)
mc.setListener("onMove", testonMove)
# status: test

def testonPistonPush(e):
	printLog(f'[TEST] Listener onPistonPush s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}, l_e["blockpos"] = {e["blockpos"]}, l_e["pistonpos"] = e["pistonpos"], i_e["dimensionid"] = {e["dimensionid"]}')
	mc.removeListener("onPistonPush", testonPistonPush)
mc.setListener("onPistonPush", testonPistonPush)
# status: test with problem
#key pistonpos will cause crash and it has been removed from dll plugin, please fix

def testonEndermanRandomTeleport(e):
	printLog(f'[TEST] Listener onEndermanRandomTeleport l_e.pos = {listToStr(e.pos)}')
	mc.removeListener("onEndermanRandomTeleport", testonEndermanRandomTeleport)
mc.setListener("onEndermanRandomTeleport", testonEndermanRandomTeleport)
# status: test

def testonServerStarted(e):
	printLog(f'[TEST] Listener onServerStarted')
	mc.removeListener("onServerStarted", testonServerStarted)
mc.setListener("onServerStarted", testonServerStarted)
# status: test

def testonDropItem(e):
	printLog(f'[TEST] Listener onDropItem s_e["player"].name = {e["player"].name}, i_e["itemid"] = {e["itemid"]}, i_e["itemcount"] = {e["itemcount"]}, s_e["itemname"] = {e["itemname"]}, i_e["itemcount"] = {e["itemaux"]}')
	mc.removeListener("onDropItem", testonDropItem)
mc.setListener("onDropItem", testonDropItem)
# status: test

def testonTakeItem(e):
	printLog(f'[TEST] Listener onTakeItem b_e["player"].is_sneaking = {e["player"].is_sneaking}, l_e["actor"].pos = {listToStr(e["actor"].pos)}')
	mc.removeListener("onTakeItem", testonTakeItem)
mc.setListener("onTakeItem", testonTakeItem)
# status: psss

def testonRide(e):
	printLog(f'[TEST] Listener onRide s_e["actor1"].name = {e["actor1"].name}, s_e["actor2"].name = {e["actor2"].name}')
	mc.removeListener("onRide", testonRide)
mc.setListener("onRide", testonRide)
# status: test

def testonUseFrameBlock(e):
	printLog(f'[TEST] Listener onUseFrameBlock s_e["player"].name = {e["player"].name}, l_e["blockpos"] = {listToStr(e["blockpos"])}, i_e["dimensionid"] = {e["dimensionid"]}')
	mc.removeListener("onUseFrameBlock", testonUseFrameBlock)
mc.setListener("onUseFrameBlock", testonUseFrameBlock)
# status: test

def testonJump(e):
	printLog(f'[TEST] Listener onJump l_e.pos = {e.pos}')
	mc.removeListener("onJump", testonJump)
mc.setListener("onJump", testonJump)
# status: test

def testonSneak(e):
	printLog(f'[TEST] Listener onSneak s_e.platform_online_id = {e.platform_online_id}')
	mc.removeListener("onSneak", testonSneak)
mc.setListener("onSneak", testonSneak)
# status: test

def testonBlockInteracted(e):
	printLog(f'[TEST] Listener onBlockInteracted s_e["player"].name = {e["player"].name}, l_e["blockpos"] = {listToStr(e["blockpos"])}, s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}, i_e["dimensionid"] = {e["dimensionid"]}')
	mc.removeListener("onBlockInteracted", testonBlockInteracted)
mc.setListener("onBlockInteracted", testonBlockInteracted)
# status: test

def testonFireSpread(e):
	printLog(f'[TEST] Listener onFireSpread l_e["blockpos"] = {listToStr(e["blockpos"])}, s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}')
	mc.removeListener("onFireSpread", testonFireSpread)
mc.setListener("onFireSpread", testonFireSpread)
# status: test

def testonBlockExploded(e):
	printLog(f'[TEST] Listener onBlockExploded s_e["actor"].name = {e["actor"].name}, l_e["blockpos"] = {listToStr(e["blockpos"])}, s_e["blockname"] = {e["blockname"]}, i_e["blockid"] = {e["blockid"]}, i_e["dimensionid"] = {e["dimensionid"]}')
	mc.removeListener("onBlockExploded", testonBlockExploded)
mc.setListener("onBlockExploded", testonBlockExploded)
# status: test

def testonUseSignBlock(e):
	printLog(f'[TEST] Listener onUseSignBlock i_e["player"].platform = {e["player"].platform}, s_e["text"] = {e["text"]}, l_e["position"] = {e["position"]}')
	mc.removeListener("onUseSignBlock", testonUseSignBlock)
mc.setListener("onUseSignBlock", testonUseSignBlock)


def testonPreJoin(e):
    printLog(f'[TEST] Listener onPreJoin e.ip = {e.ip}, e.name = {e.name}')
# status: test

#temple
def test(e):
	printLog(f'[TEST] Listener ')
	
# mc.setListener('onUseItem', testonUseItem)
# mc.setListener('onPlaceBlock', testonPlaceBlock)
# mc.setListener('onDestroyBlock', testonDestroyBlock)
# mc.setListener('onOpenContainer', testonOpenContainer) # onOpenChest, onOpenBarrel -> onOpenContainer
# mc.setListener('onCloseContainer', testonCloseContainer) # onCloseChest, onCloseBarrel -> onCloseContainer
# #no output
# mc.setListener('onContainerChange', testonContainerChange)
# #no full output
# mc.setListener('onChangeDim', testonChangeDim) # onChangeDimension -> onChangeDim
# mc.setListener('onMobDie', testonMobDie)
# mc.setListener("onMobHurt", testonMobHurt)
# mc.setListener("onRespawn", testonRespawn)
# mc.setListener("onChat", testonChat)
# mc.setListener("onInputText", testonInputText)
# mc.setListener("onCommandBlockUpdate", testonCommandBlockUpdate)
# mc.setListener("onPlayerCmd", testonPlayerCmd) # onInputCommand -> onPlayerCmd
# mc.setListener("onCmdBlockExecute", testonCmdBlockExecute) # onCommandBlockPerform -> onCmdBlockExecute
# mc.setListener("onLevelExplode", testonLevelExplode)
# mc.setListener("onSetArmor", testonSetArmor)
# mc.setListener("onFarmLandDecay", testonFarmLandDecay) # onFallBlockTransform -> onFarmLandDecay
# mc.setListener("onUseRespawnAnchor", testonUseRespawnAnchor) # onUseRespawnAnchorBlock -> onUseRespawnAnchor
# mc.setListener("onScoreChanged", testonScoreChanged)
# #no output
# #-mc.setListener("onMove", testonMove) # frist test this listener and then disable it to test other listener
# mc.setListener("onPistonPush", testonPistonPush)
# #-mc.setListener("onEndermanRandomTeleport", testonEndermanRandomTeleport)
# #-mc.setListener("onServerStarted", testonServerStarted)
# #-mc.setListener("onDropItem", testonDropItem)
# #-mc.setListener("onTakeItem", testonTakeItem)
# mc.setListener("onRide", testonRide)
# mc.setListener("onUseFrameBlock", testonUseFrameBlock)
# #-mc.setListener("onJump", testonJump)
# #-mc.setListener("onSneak", testonSneak)
# mc.setListener("onBlockInteracted", testonBlockInteracted)
# mc.setListener("onFireSpread", testonFireSpread)
# mc.setListener("onBlockExploded", testonBlockExploded)
# mc.setListener("onUseSignBlock", testonUseSignBlock)
# #no ouput
# # temple
# mc.setListener("", test)


'''
def testonOpenChest(e):
	printLog('[TEST] Listener onOpenChest l_e['position'] = （%f,%f,%f）, e['player'].typeid = %d' % (e['position'][0], e['position'][1], e['position'][2], e['player'].typeid))
	mc.removeListener

def testonCloseChest(e):
	printLog('[TEST] Listener onCloseChest l_e['position'] = %f, e['player'].typename = %s' % (e['position'][0], e['position'][1], e['position'][2], e['player'].typename))
	mc.removeListener

def testonOpenBarrel(e):
	printLog('[TEST] Listener onOpenBarrel l_e['position'] = （%f,%f,%f）, e['player'].platform_online_id = %s' % (e['position'][0], e['position'][1], e['position'][2], e['player'].platform_online_id))
	mc.removeListener

def testonCloseBarrel(e):
	printLog('[TEST] Listener onCloseBarrel l_e['position'] = （%f,%f,%f）, e['player'].platform = %d' % (e['position'][0], e['position'][1], e['position'][2], e['player'].platform))
	mc.removeListener
'''
#mc.setListener('onJoin', testonPlayerJoin)
#mc.setListener('onLeft', testonPlayerLeft)
#mc.setListener('onConsoleInput', testonConsoleInput)
#mc.setListener('onOpenBarrel', testonOpenBarrel)
#mc.setListener('onOpenChest', testonOpenChest)
#mc.setListener('onCloseChest', testonCloseChest)
#mc.setListener('onCloseBarrel', testonCloseBarrel)
#mc.setListener('onChangeDimension', testonChangeDimension)
mc.setListener("onPreJoin", testonPreJoin)
