import mco
from typing import Optional, Callable
import time
import os
import json

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
        return mco.setListener(event, function)


def removeListener(event: str, function: Callable[[object], Optional[bool]]) -> None:
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
        return mco.removeListener(event, function)


# API
def getBDSVerion() -> str:
    return mco.getBDSVersion()

def logout(message: str) -> None:
    return mco.logout(message)

def runcmd(command: str) -> None:
    return mco.runcmd(command)

def setCommandDescription(cmd:str, description:str, function: Callable[[object], Optional[bool]] = None) -> None:
    if function:
        return mco.setCommandDescription(cmd, description, function)
    else:
        return mco.setCommandDescription(cmd, description)

def getPlayerByXuid(xuid: str) -> mco.Entity:
    return mco.getPlayerByXuid(xuid)

def getPlayerList() -> list:
    return mco.getPlayerList()

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


######### PRIVATE API ##########
'''
log_out_function_inner = __import__("mc").logout
def log_out_function_replace(content: str):
    log_out_function_inner(content + "\n")
logger = log_out_function_replace
'''
logger = print

def log(*content, name: str = "Plugin", level: str = "INFO", info: str = ""):
    date = time.strftime('%H:%M:%S')
    strs = ""
    for string in content:
        strs += str(string)
    if strs[:2] == "('":
        content = strs[2:-3]
    else:
        content = strs
    if __name__ != '__main__':
        if name != "plugin" and content != "Test Message" and level != "INFO" and info != "":
            logger(f"{date} {level} [{name}][{info}] {content}")
        elif name != "plugin" and level != "INFO":
            logger(f"{date} {level} [{name}] {content}")
        elif name != "plugin" and info != "":
            logger(f"{date} INFO [{name}][{info}] {content}")
        elif info != "":
            logger(f"{date} INFO [{name}][{info}] {content}")
        else:
            logger(f"{date} INFO [{name}] {content}")


def read_conf(folder:str, filename:str, encoding="utf-8"):
    if os.path.exists(f"plugins/py/{folder}/{filename}"):
        try:
            with open(f"plugins/py/{folder}/{filename}", "r", encoding=encoding) as file:
                config = json.load(file)
        except:
            with open(f"plugins/py/{folder}/{filename}", "r", encoding="gbk") as file:
                config = json.load(file)
        return config
    else:
        return None


def save_conf(folder:str, filename:str, config={}, encoding="utf-8"):
    with open(f"plugins/py/{folder}/{filename}", 'w+', encoding=encoding) as file:
        json.dump(config, file, indent='\t', ensure_ascii=False)


def make_conf(folder:str, filename:str, config={}, encoding="utf-8"):
    if not os.path.exists(f"plugins/py/{folder}/{filename}"):
        os.makedirs(f"plugins/py/{folder}")
        save_conf(folder, filename, config, encoding)
        return True
    else:
        return False