## 🎈 实体相关事件

#### `"onMobDie"` - 生物死亡

- 监听函数原型
  `function(mob,source,cause)`
- 参数：
  - mob : `Entity`  
    死亡的实体对象
  - source : `Entity`  
    伤害来源的实体对象（可能为`Null`）
  - cause : `Integer`  
    死亡原因

- 拦截事件：不可以拦截

注意，当玩家死亡时，除了触发`onPlayerDie`事件，这个事件同样也会被触发一次

<br>

#### `"onMobHurt"` - 生物受伤（包括玩家）

- 监听函数原型
  `function(mob,source,damage,cause)`
- 参数：
  - mob : `Entity`  
    受伤的实体对象
  - source : `Entity`  
    伤害来源的实体对象（可能为`Null`）
  - damage : `Float`  
    受到的伤害数值
  - cause : `DamageCause`   
    受伤原因
- 拦截事件：函数返回`false`

DamageCause 为伤害原因枚举，枚举值如下，有问号的待验证，？？？待补充
|伤害原因|枚举值|解释|
|--|--|--|
| None | -0x01 | 其他 |
| Override | 0x00 | 非正常方式（如脚本直接设置血量为0），这种方式的伤害不会被盔甲与buff吸收 |
| Contact | 0x01 | 接触伤害（如仙人掌） |
| EntityAttack | 0x02 | 实体攻击 |
| Projectile | 0x03 | 抛射物攻击 |
| Suffocation | 0x04 | 窒息（密封空间） |
| Fall | 0x05 | 掉落 |
| Fire | 0x06 | 着火 |
| FireTick | 0x07 | 着火 |
| Lava | 0x08 | 熔岩 |
| Drowning | 0x09 | 溺水 |
| BlockExplosion | 0x0A | 方块爆炸 |
| EntityExplosion | 0x0B | 实体爆炸 |
| Void | 0x0C | 虚空 |
| Suicide | 0x0D | 自杀 |
| Magic | 0x0E | 尖牙对生物造成的伤害、守卫者对生物造成的魔法伤害和药水伤害等 |
| Wither | 0x0F | 凋零效果 |
| Starve | 0x10 | 饥饿 |
| Anvil | 0x11 | 下落的铁砧 |
| Thorns | 0x12 | 荆棘 |
| FallingBlock | 0x13 | 下落的方块 |
| Piston | 0x14 | 活塞 |
| FlyIntoWall | 0x15 | 动态能量（滑翔撞墙） |
| Magma | 0x16 | 岩浆块 |
| Fireworks | 0x17 | 烟花 |
| Lightning | 0x18 | 闪电 |
| Charging | 0x19 | ？？？ |
| Temperature | 0x1A | 温度 （雪人？） |
| Freezing | 0x1B | 冰冻 |
| Stalactite | 0x1C | 被钟乳石砸到 |
| Stalagmite | 0x1D | 掉落到石笋上 |
| All | 0x1F | 所有 |


<br>

#### `"onEntityExplode"` - 发生由实体引起的爆炸

- 监听函数原型
  `function(source,pos,radius,maxResistance,isDestroy,isFire)`
- 参数：
  - source : `Entity`  
    爆炸来源的实体对象
  - pos : `FloatPos`  
    爆炸发生的坐标
  - radius : `Float`    
  爆炸波及的半径
  - maxResistance : `Float`  
    爆炸可破坏的方块爆炸抗性上限
  - isDestroy : `Boolean`  
    爆炸是否破坏方块
  - isFire : `Boolean`  
    爆炸是否产生火焰
  
- 拦截事件：函数返回`false`

<br>

#### `"onMobSpawn"` - 发生于实体生成

- 监听函数原型
  `function(typeName,pos)`
- 参数：
  - typeName : `string`  
    生成实体名称
  - pos : `FloatPos`  
    生成的坐标
  
- 拦截事件：函数返回`false`

<br>

#### `"onProjectileHitEntity"` - 实体被弹射物击中

- 监听函数原型
  `function(entity,source)`
- 参数：
  - entity: `Entity`  
    被击中的实体对象
  - source : `Entity`  
    发射的弹射物实体（如箭）
- 拦截事件：不可以拦截

<br>

#### `"onWitherBossDestroy"` - 凋零破坏方块

- 监听函数原型
  `function(witherBoss,AAbb,aaBB)`
- 参数：
  - witherBoss: `Entity`  
    凋零的实体对象

  - AAbb: `IntPos`
  
    凋零将破坏的区域（长方体），对角点A坐标
  
  - aaBB: `IntPos`
  
    凋零将破坏的区域（长方体），对角点B坐标
  
- 拦截事件：函数返回`false`

注意，此事件不包括凋零爆炸的破坏。

<br>

#### `"onRide"` - 生物骑乘

- 监听函数原型
  `function(entity1,entity2)`
- 参数：
  - entity1 : `Entity`  
    尝试骑乘的实体对象
  - entity2 : `Entity`  
    被骑乘的实体对象
- 拦截事件：函数返回`false`

注：骑乘包括坐矿车、坐船、骑马、骑猪等。

<br>

#### `"onStepOnPressurePlate"` - 生物踩压力板

- 监听函数原型
  `function(entity,pressurePlate)`
- 参数：
  - entity : `Entity`  
    踩压力板的实体对象
  - pressurePlate : `Block`  
    被踩的压力板方块对象
- 拦截事件：函数返回`false`

注：生物踩压力板时，将会反复多次触发此事件。

<br>

#### `"onSpawnProjectile"` - 弹射物创建

- 监听函数原型
  `function(shooter,type)`
- 参数：

  - shooter : `Entity`  
    发射弹射物的的实体对象

  - type : `String`  
    弹射物标准类型名

- 拦截事件：函数返回 `false`

注：已知可拦截的弹射物有鸡蛋、末影珍珠、雪球、三叉戟、箭、钓竿（鱼钩）。

<br>

#### `"onProjectileCreated"` - 弹射物创建完毕

- 监听函数原型
  `function(shooter,entity)`
- 参数：

  - shooter : `Entity`  
    创建此弹射物的的实体对象

  - entity : `Entity`  
    被创建的弹射物实体对象

- 拦截事件：不可以拦截

<br>

#### `"onNpcCmd"` - NPC执行命令

- 监听函数原型
  `function(npc,pl,cmd)`
- 参数：
  - npc : `Entity`  
    执行命令的NPC实体对象
  - pl : `Player`  
    触发NPC命令执行的玩家对象
  - cmd : `String`  
    NPC执行的命令
- 拦截事件：函数返回`false`

<br>

#### `"onChangeArmorStand"` - 操作盔甲架

- 监听函数原型
  `function(as,pl,slot)`

- 参数：

  - as: `Entity`  
    被操作的盔甲架实体对象

  - pl : `Player`  
    操作盔甲架的玩家对象

  - slot : `Number`

    装备栏编号

- 拦截事件：函数返回`false`

<br>

#### `"onEntityTransformation"` - 实体转变

- 监听函数原型
  `function(uniqueId,entity)`
- 参数：

  - uniqueId: `String` 
    转变前的实体的唯一标识符
  - entity : `Entity` 
    转换完成的实体
- 拦截事件：不可以拦截

注：此事件为 `Addons` 中实体的 `TransformationComponent` 激活时触发，多用于引擎与Addon交互。由于转变前的实体指针很快被销毁，因此只提供`UniqueId`。
