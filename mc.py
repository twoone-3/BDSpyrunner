import mco
from typing import Optional, Callable
import api.getPlayerList as api

# Listener
def setListener(event: str, function: Callable[[object], Optional[bool]]) -> None:
    notContainer = True
    if event == "onConsoleCmd":
        event = "onConsoleInput"
    if event == "onJoin": #listener name (from py plugin)
        event = "onPlayerJoin" #pyr provided listener name
    if event == "onLeft":
        event = "onPlayerLeft"
    if event == "onFormSelected":
        event = "onSelectForm"
    if event == "onOpenContainer":
        notContainer = False
        mco.setListener("onOpenChest", function)
        mco.setListener("onOpenBarrel", function)
    if event == "onCloseContainer":
        notContainer = False
        mco.setListener("onCloseChest", function)
        mco.setListener("onCloseBarrel", function)
    if event == "onChangeDim":
        event = "onChangeDimension"
    if event == "onPlayerCmd":
        event = "onInputCommand"
    if event == "onCmdBlockExecute":
        event = "onCommandBlockPerform"
    if event == "onFarmLandDecay":
        event = "onFallBlockTransform"
    if event == "onUseRespawnAnchor":
        event = "onUseRespawnAnchorBlock"
    if notContainer:
        mco.setListener(event, function)


# API
def getrBDSVerion() -> str:
    return mco.getBDSVersion()

def logout(message: str) -> None:
    return mco.logout(message)

def runcmd(command: str) -> None:
    return mco.runcmd(command)

def setCommandDescription(cmd:str, description:str, function: Callable[[object], Optional[bool]] = None) -> None:
    return mco.setCommandDescription(cmd, description, function)

def getPlayerByXuid(xuid: str) -> mco.Entity:
    return mco.getPlayerByXuid(xuid)

def getPlayerList() -> list:
    return mco.getPlayerList()
    return api.getPlayerList()

def setDamage(dmg:int) -> None:
    return mco.setDamage(dmg)

def setServerMotd(motd:str):
    return mco.setServerMotd(motd)

def getBlock(x:int, y:int, z:int, did:int) -> dict:
    return mco.getBlock(x, y, z, did)

def setBlock(name:str, x:int, y:int, z:int, did:int) -> None:
    return mco.setBlock(name, x, y, z, did)

def getStructure(x1:int, y1:int, z1:int, x2:int, y2:int, z2:int, did:int) -> str:
    return mco.getStructure(x1, y1, z1, x2, y2, z2, did)

def setStructure(data:str, x:int, y:int, z:int, did:int) -> None:
    return mco.setStructure(data, x, y, z, did)

def explode(x:float, y:float, z:float, did:int, power:float, destroy:bool, range:float, fire:bool) -> None:
    return explode(x, y, z, did, power, destroy, range, fire)

def spawnItem(data:str, x:int, y:int, z:int, did:int) -> None:
    return spawnItem(data, x, y, z, did)

def isSlimeChunk(x:int, y:int) -> bool:
    return isSlimeChunk(x, y)

def setSignBlockMessage(msg:str, x:int, y:int, z:int, did:int) -> None:
    return setSignBlockMessage(msg, x, y, z, did)

api.init()