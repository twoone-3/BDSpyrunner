## getBDSVersion() -> str
获取BDS的版本号
## runCommand(cmd:str) -> bool
执行一条控制台命令
## runCommandEx(cmd:str) -> (bool,str)
执行一条控制台命令，返回执行结果和消息
## setListener(key:str, callback:function) -> None
设置监听器，事件触发时callback将会被调用  
可用的key可在**Event**界面查询
## registerCommand(cmd:str, callback:function，description:str) -> None
注册指令，玩家输入以cmd开头的指令都会触发回调  
回调函数原型：callback(player:mc.Entity, cmd:str) -> None
## getPlayerByXuid(xuid:str) -> mc.Entity
根据Xuid获取玩家
## getPlayerList() -> list
获取玩家列表
## getEntityList() -> list
获取实体列表
## setServerMotd(motd:str)
设置服务器外显名字
## broadcastText(text:str, type:str)
广播文本
## broadcastTitle(text:str, type:str, fade_in_duration:int, remain_duration:int, fade_out_duration:int)
广播文本
## getBlock(x:int, y:int, z:int, did:int) -> mc.BlockInstance
获取指定位置方块信息
## setBlock(x:int, y:int, z:int, did:int, name:str, tile_data:int) -> none
设置指定位置的方块，tile_data为特殊值
## getStructure(x1:int,y1:int,z1:int,x2:int,y2:int,z2:int,did:int) -> str
获取两个坐标之间的结构nbt数据
## setStructure(data:str,x:int,y:int,z:int,did:int) -> None
在(x,y,z)处设放置一个结构，data为结构JSON字符串
## explode(x:float,y:float,z:float,did:int,power:float,destroy:bool,range:float,fire:bool) -> None
产生爆炸
## spawnItem(data:str,x:int,y:int,z:int,did:int) -> None
生成一个掉落物，data为物品JSON字符串
## isSlimeChunk(x:int,y:int) -> Boolean
检查是否为史莱姆区块
## setSignBlockMessage(msg:str,x:int,y:int,z:int,did:int) -> None
设置牌子文字
