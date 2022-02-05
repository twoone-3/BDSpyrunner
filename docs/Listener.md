事件描述格式：
```
监听器关键字
介绍
拦截可否
回传数据
```
# Listener
事件监听器
* 使用setListener来设置监听器
* 回调函数有且只有一个参数，类型为字典

以下为所有事件：
## onConsoleInput
控制台输入监听  
拦截可否：是  
回传数据：
* cmd – 指令数据
## onConsoleOutput
控制台输出监听  
拦截可否：是  
回传数据：
* output – 输出信息
## onPlayerJoin
玩家加入服务器监听  
拦截可否：否  
回传数据：
* player - 玩家
## onPlayerLeft
玩家离开服务器监听  
拦截可否：否  
回传数据：
* player - 玩家
## onPlayerAttack
玩家攻击监听  
拦截可否：是  
回传数据：
* actor – 被攻击实体
* player - 玩家
## onSelectForm
玩家选择GUI表单监听  
拦截可否：是  
回传数据：
* formid – 表单ID
* selected – 表单回传的选择项信息
* player - 玩家
## onUseItem
使用物品监听  
`注：Win10客户端使用物品事件会在单次点击内触发多次`  
拦截可否：是  
回传数据：
* position – 操作方块所在位置
* itemid – 物品ID
* itemaux – 物品特殊值
* itemname – 物品名称
* player - 玩家
## onPlaceBlock
放置方块监听  
拦截可否：是  
回传数据：
* position – 操作方块所在位置
* blockid – 方块ID
* blockname – 方块名称
* player - 玩家
## onDestroyBlock
破坏方块监听  
拦截可否：是  
回传数据：
* position – 操作方块所在位置
* blockid – 方块ID
* blockname – 方块名称
* player - 玩家
## onOpenChest
开箱监听  
拦截可否：是  
回传数据：
* position – 操作方块所在位置
* player - 玩家
## onCloseChest
关箱监听  
拦截可否：否  
回传数据：
* position – 操作方块所在位置
* player - 玩家
## onOpenBarrel
开桶监听  
拦截可否：否  
回传数据：
* position – 操作方块所在位置
* player - 玩家
## onCloseBarrel
关桶监听  
拦截可否：否  
回传数据：
* position – 操作方块所在位置
* player - 玩家
## onContainerChange
放入取出物品监听  
拦截可否：否  
回传数据：
* itemid – 物品ID
* itemcount – 物品数量
* itemname – 物品名字
* itemaux – 物品特殊值
* position – 操作方块所在位置
* blockid – 方块ID
* blockname – 方块名称
* slot – 操作格子位置
* player - 玩家
## onChangeDimension
切换维度监听  
拦截可否：是  
回传数据：
* player - 玩家
## onMobDie
生物死亡监听  
拦截可否：否  
回传数据：
* dmcase – 伤害具体原因ID
* actor1 - 死亡实体
* actor2 - 伤害源实体
## onMobHurt
生物受伤监听  
在此监听下可以使用setDamage来设置伤害值  
拦截可否：是  
回传数据：
* dmcase – 伤害具体原因ID
* actor1 - 死亡实体
* actor2 - 伤害源实体
* damage - 理论伤害值
## onRespawn
玩家重生监听  
拦截可否：否  
回传数据： 
* player - 玩家
## onChat
聊天监听  
拦截可否：否  
回传数据：
* sender – 发送者名字
* target – 接收者名字
* msg – 接收到的信息
* style – 聊天类型
## onInputText
玩家输入文本监听  
拦截可否：是  
回传数据：
* msg – 输入的文本
* player - 玩家
## onCommandBlockUpdate
玩家更新命令方块监听  
拦截可否：是  
回传数据：
* player - 玩家
* cmd – 将被更新的新指令
* mode – 命令方块模式
* condition – 是否有条件
* redstone – 是否要红石
* output – 上一次输出
* rawname – 命令方块名字
* delay – 延迟
* position – 命令方块所在位置
## onInputCommand
玩家输入指令监听  
拦截可否：是  
回传数据：
* cmd – 玩家输入的指令
* player - 玩家
## onCommandBlockPerform
命令方块执行指令监听  
拦截可否：是  
回传数据：
* cmd – 将被执行的指令
* rawname – 命令方块名称
* position – 执行者所在位置
* mode – 命令方块模式
* condition – 是否有条件
## onLevelExplode
爆炸监听  
拦截可否：是  
回传数据：
* actor – 爆炸者实体（床爆炸时该无效）
* position – 爆炸点所在位置
* dimensionid – 爆炸者所处维度ID
* power – 爆炸强度
## onSetArmor
玩家穿戴监听  
拦截可否：否  
回传数据：
* player - 玩家
* itemid – 物品ID
* itemcount – 物品数量
* itemname – 物品名字
* itemaux – 物品特殊值
* slot – 操作格子位置
## onFallBlockTransform
耕地破坏监听  
拦截可否：是  
回传数据：
* player - 玩家
* position – 方块所在位置
* dimensionid – 所处维度ID
## onUseRespawnAnchorBlock
使用重生锚监听  
拦截可否：是  
回传数据：
* player - 玩家
* position – 方块所在位置
* dimensionid – 所处维度ID
## onScoreChanged
计分板改变监听  
拦截可否：否  
回传数据：
* scoreboardid - 计分板ID
* playersnum – 玩家分数
* objectivename – 对象实际名称
* objectivedisname - 对象显示名称
## onMove
玩家移动监听  
拦截可否：否  
回传数据：
* player - 玩家
## onPistonPush
活塞推动监听  
拦截可否：否  
回传数据：
* blockname - 方块名字
* blockid - 方块ID
* blockpos - 方块坐标
* pistonpos - 活塞坐标
* dimensionid - 维度ID
## onEndermanRandomTeleport
末影人随机传送监听  
拦截可否：是  
回传数据：
* actor - 实体
## onServerStarted
服务器开服完成监听  
拦截可否：否  
回传数据：
* none - 无
## onDropItem
玩家丢出物品监听  
拦截可否：是  
回传数据：
* player - 玩家
* itemid - 物品ID
* itemcount - 物品数量
* itemname - 物品名
* itemaux - 物品特殊值
## onTakeIte
玩家捡起物品监听  
拦截可否：是  
回传数据：
* player - 玩家
* actor - 被捡起物品的
## onRide
生物骑乘监听  
拦截可否：是  
回传数据：
* actor1 - 骑乘者
* actor2 - 被骑乘者
## onUseFrameBlock
操作展示框监听  
拦截可否：是  
回传数据：
* player - 玩家
* blockpos - 方块坐标
* dimensionid - 方块维度
## onJump
玩家跳跃监听  
拦截可否：否  
回传数据：
* player - 玩家
## onSneak
玩家潜行监听  
拦截可否：否  
回传数据：
*player - 玩家
## onBlockInteracted
方块接受玩家互动监听  
拦截可否：是  
回传数据：
* player - 玩家
* blockpos - 方块坐标
* blockname - 方块名
* blockid - 方块ID
* dimensionid - 方块维度
## onBlockExploded
方块被爆炸破坏监听  
拦截可否：否  
回传数据：
* actor - 爆炸实体
* blockpos - 方块坐标
* blockname - 方块名
* blockid - 方块ID
* dimensionid - 方块维度
## onUseSignBlock
告示牌使用监听  
拦截可否：是  
回传数据：
* player - 玩家
* text - 文本内容
* position - 坐标
