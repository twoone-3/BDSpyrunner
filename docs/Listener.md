# Listener
使用setListener来设置监听器<br>
setListener会将第一个参数转换成事件代码<br>
再将函数放入一个无序表中<br>
当事件触发时，BDSpyrunner将会捕获数据并调用函数<br>
当只回传多个数据时，参数为传递字典<br>
当只回传一个数据时，参数为这个数据<br>
* 完整示例
```py
import mc
def fn(e):
	print(e)
	return False
mc.setListener('onUseItem',fn)
```
## onConsoleInput
拦截可否：是<br>
回传数据：
* cmd – 指令数据
## onConsoleOutput
拦截可否：是<br>
回传数据：
* output – 输出信息
## onPlayerJoin
拦截可否：否<br>
回传数据：
* player - 玩家指针
## onPlayerLeft
拦截可否：否<br>
回传数据：
* player - 玩家指针
## onPlayerAttack
拦截可否：是<br>
回传数据：
* actor – 被攻击实体指针
* player - 玩家指针
## onSelectForm
拦截可否：是<br>
回传数据：
* formid – 表单ID
* selected – 表单回传的选择项信息
* player - 玩家指针
## onUseItem
`注：Win10客户端使用物品事件会在单次点击内触发多次`<br>
拦截可否：是<br>
回传数据：
* position – 操作方块所在位置
* itemid – 物品ID
* itemaux – 物品特殊值
* itemname – 物品名称
* player - 玩家指针<br>
## onPlaceBlock
拦截可否：是<br>
回传数据：
* position – 操作方块所在位置
* blockid – 方块ID
* blockname – 方块名称
* player - 玩家指针
## onDestroyBlock
拦截可否：是<br>
回传数据：
* position – 操作方块所在位置
* blockid – 方块ID
* blockname – 方块名称
* player - 玩家指针
## onOpenChest
拦截可否：是<br>
回传数据：
* position – 操作方块所在位置
* player - 玩家指针
## onCloseChest
拦截可否：否<br>
回传数据：
* position – 操作方块所在位置
* player - 玩家指针
## onOpenBarrel
拦截可否：否<br>
回传数据：
* position – 操作方块所在位置
* player - 玩家指针
## onCloseBarrel
拦截可否：否<br>
回传数据：
* position – 操作方块所在位置
* player - 玩家指针
## onContainerChange
拦截可否：否<br>
回传数据：
* itemid – 物品ID
* itemcount – 物品数量
* itemname – 物品名字
* itemaux – 物品特殊值
* position – 操作方块所在位置
* blockid – 方块ID
* blockname – 方块名称
* slot – 操作格子位置
* player - 玩家指针
## onChangeDimension
拦截可否：是<br>
回传数据：
* player - 玩家指针
## onMobDie
拦截可否：否<br>
回传数据：
* dmcase – 伤害具体原因ID
* actor1 - 死亡实体指针
* actor2 - 伤害源实体指针
## onMobHurt
`注：此监听存在相关组件setDamage用于设置伤害值`<br>
拦截可否：是<br>
回传数据：
* dmcase – 伤害具体原因ID
* actor1 - 死亡实体指针
* actor2 - 伤害源实体指针
* damage - 理论伤害值
## onRespawn
拦截可否：否<br>
回传数据： 
* player - 玩家指针
## onChat
拦截可否：否<br>
回传数据：
* sender – 发送者名字
* target – 接收者名字
* msg – 接收到的信息
* style – 聊天类型
## onInputText
拦截可否：是<br>
回传数据：
* msg – 输入的文本
* player - 玩家指针
## onCommandBlockUpdate
拦截可否：是<br>
回传数据：
* player - 玩家指针
* cmd – 将被更新的新指令
* mode – 命令方块模式
* condition – 是否有条件
* redstone – 是否要红石
* output – 上一次输出
* rawname – 命令方块名字
* delay – 延迟
* position – 命令方块所在位置
## onInputCommand
拦截可否：是<br>
回传数据：
* cmd – 玩家输入的指令
* player - 玩家指针
## onCommandBlockPerform
拦截可否：是<br>
回传数据：
* cmd – 将被执行的指令
* rawname – 命令方块名称
* position – 执行者所在位置
* mode – 命令方块模式
* condition – 是否有条件
## onLevelExplode
拦截可否：是<br>
回传数据：
* actor – 爆炸者实体指针（床爆炸时该指针无效）
* position – 爆炸点所在位置
* dimensionid – 爆炸者所处维度ID
* power – 爆炸强度
## onSetArmor
拦截可否：否<br>
回传数据：
* player - 玩家指针
* itemid – 物品ID
* itemcount – 物品数量
* itemname – 物品名字
* itemaux – 物品特殊值
* slot – 操作格子位置
## onFallBlockTransform
拦截可否：是<br>
回传数据：
* player - 玩家指针
* position – 方块所在位置
* dimensionid – 所处维度ID
## onUseRespawnAnchorBlock
拦截可否：是<br>
回传数据：
* player - 玩家指针
* position – 方块所在位置
* dimensionid – 所处维度ID
## onScoreChanged
拦截可否：否<br>
回传数据：
* scoreboardid - 计分板ID
* playersnum – 玩家分数
* objectivename – 对象实际名称
* objectivedisname - 对象显示名称
## onMove
拦截可否：否<br>
回传数据：
* player - 玩家指针
## onPistonPush
拦截可否：否<br>
回传数据：
* blockname - 方块名字
* blockid - 方块ID
* blockpos - 方块坐标
* pistonpos - 活塞坐标
* dimensionid - 维度ID
## onEndermanRandomTeleport
拦截可否：是<br>
回传数据：
* actor - 实体指针
