?> 此页介绍了mc模块里的内容

---
# 内置函数
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
## getPlayer(name:str) -> mc.Entity
根据名字获取玩家
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
## getStructure(x1:int, y1:int, z1:int, x2:int, y2:int, z2:int, did:int) -> mc.NBT
获取两个坐标之间的结构nbt数据
## setStructure(nbt:mc.NBT, x:int, y:int, z:int, did:int) -> None
在(x,y,z)处设放置一个结构
## explode(x:float, y:float, z:float, did:int, power:float, destroy:bool, range:float, fire:bool) -> None
产生爆炸
## spawnItem(nbt:mc.NBT, x:int, y:int, z:int, did:int) -> None
生成一个掉落物
## isSlimeChunk(x:int, y:int) -> Boolean
检查是否为史莱姆区块
## setSignBlockMessage(msg:str, x:int, y:int, z:int, did:int) -> None
设置牌子文字
---
# Entity
实体类，包括玩家、生物、实体。
## getName() -> str
## setName(name:str) -> None
## getUuid() -> str
## getXuid() -> str
## getPos() -> list
## getDimensionId() -> int
## isStanding() -> bool
## isSneaking() -> bool
## isSitting() -> bool
## getTypeId() -> int
## getTypeName() -> str
## getNBT() -> mc.NBT
## setNBT(nbt:mc.NBT) -> bool
## getHealth() -> int
## getMaxHealth() -> int
## getPermissions() -> int
## setPermissions(perm:int) -> None
## getPlatformOnlineId() -> str
## getPlatform() -> int
## getIP() -> str
## setHand(nbt:mc.NBT) -> None
设置玩家手中物品
## addItem(nbt:mc.NBT) -> None
为玩家新添物品
## removeItem(slot:int, count:int) -> None
移除玩家背包slot格子的count个物品
## teleport(x:float, y:float, z:float, did:int) -> None
传送玩家到指定坐标和维度
## sendCommandPacket(cmd:str) -> None
模拟玩家发送命令包，也就是替玩家执行命令
## sendTextPacket(msg:str, type:int = 0) -> None
模拟玩家发送文本包，type = 0为原始文本，相当于指令tellraw，type = 1为聊天文本，相当于玩家说话
## resendAllChunks() -> None
重新发送玩家客户端的地图区块，可能造成亮度渲染问题
## disconnect(msg:str = '')
断开玩家连接，msg将显示在玩家断开连接后的屏幕上
## transferServer(ip:str, port:int) -> None
传送玩家到指定服务器
## getScore(scoreboard:str) -> int
获取玩家在名为scoreboard的计分板的分数
## setScore(scoreboard:str, count:int) -> None
设置玩家在名为scoreboard的计分板的分数
## addScore(scoreboard:str, count:int) -> None
增加玩家在名为scoreboard的计分板的分数
## reduceScore(scoreboard:str, count:int) -> None
减少玩家在名为scoreboard的计分板的分数
## addLevel(level:int) -> None
增加玩家经验等级
## setBossBar(msg:str, percent:float) -> None
设置玩家BOSS栏，percent是血条百分比(min:0,max:1)
## removeBossBar() -> None
移除玩家BOSS栏
## addTag(name:str) -> None
增加标签
## removeTag(name:str) -> None
移除标签
## getTags() -> list
获取标签
## setSidebar(title:str, content:str) -> None
设置玩家计分板侧边栏  
例:`player.setSidebar('自定义侧边栏', '{"第一行":0, "第二行":2, "我在哪行?":3}')`
## removeSidebar() -> None
移除玩家计分板侧边栏
## sendCustomForm(data:str, callback:function) -> None
向指定的玩家发送一个自定义表单，回调函数原型为callback(Entity:player,selected_data:str) -> None<br>
例:
```py
player.sendCustomForm('{"content":[{"type":"label","text":"这是一个文本标签"},{"placeholder":"水印文本","default":"","type":"input","text":""},{"default":true,"type":"toggle","text":"开关~或许是吧"},{"min":0.0,"max":10.0,"step":2.0,"default":3.0,"type":"slider","text":"游标滑块！？"},{"default":1,"steps":["Step1","Step2","Step 3"],"type":"step_slider","text":"矩阵滑块？!"},{"default":1,"options":["Option 1","Option2","Option3"],"type":"dropdown","text":"如你所见 下拉框"}], "type":"custom_form","title":"这是一个自定义窗体"}'，cb)
```
## sendSimpleForm(title:str, content:str, buttons:list, images:list, callback:function) -> None
向指定的玩家发送一个简单表单，回调函数原型为callback(Entity:player,selected_item:int) -> None<br>
例:
```py
player.sendSimpleForm('标题', '内容', ["生存","死亡","求助"], ['','',''], cb)
```
## sendModalForm(title:str, content:str, button1:str, button2:str, callback:function) -> None
向指定的玩家发送一个模式对话框，回调函数原型为callback(Entity:player,selected_item:bool) -> None<br>
例:
```py
player.sendModalForm('标题', '内容', '按钮1', '按钮2', cb)
```
# NBT
Named Binary Tag
## \_\_init\_\_(type, value)
  1. type: 数据类型  
     value: 数据  
     例: NBT('Int',3) NBT('Compound')
  2. type: SNBT  
     value: SNBT字符串  
     例: NBT('SNBT', snbt)
  3. type: Binary  
     value: bytes  
     例: NBT('Binary', bytes)
## \_\_getitem\_\_(key)
## \_\_setitem\_\_(key, value)
## getType() -> str
## toBinary() -> bytes
## toJson(indentation:int = 4) -> bytes
## toSNBT() -> str
## append(value:mc.NBT) -> None
