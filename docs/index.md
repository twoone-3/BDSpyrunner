* 本文档规则:
> description of the function
> function(args...)
> a1:description
> a2:description
# mc
mc模块的函数
## getVersion
参数:none<br>
返回值:int<br>
介绍:获取pyr的版本号<br>
## logout
参数:str<br>
返回值:none<br>
介绍:向控制台发送输出消息(可拦截)<br>
## runcmd
参数:str<br>
返回值:none<br>
介绍:执行一条控制台命令<br>
## setListener
参数:str,function<br>
返回值:none<br>
介绍:设置function作为str事件监听器，function必须是只有一个参数的函数，事件触发时function将被调用，这个参数是附带的数据<br>
## setCommandDescription
参数:str,str<br>
返回值:none<br>
介绍:设置指令说明，a1为指令字符串，a2为描述，方便玩家补全<br>
## setServerMotd
参数:str<br>
返回值:none<br>
介绍:设置服务器外显名字<br>
## getBlock
参数:int,int,int,int<br>
返回值:dict<br>
介绍:获取指定位置方块信息<br>
例:`getBlock(x,y,z,dimensionid)`
## setBlock
参数:str,int,int,int,int<br>
返回值:none<br>
介绍:设置指定位置的方块，a1是方块名称，大小写必须正确，否则会失败<br>
例:`setBlock('Stone',x,y,z,dimensionid)`
## getStructure 
参数:int,int,int,int,int,int,int<br>
返回值:str<br>
介绍:获取两个坐标之间的结构，did是维度ID，返回值是结构JSON字符串<br>
例:`struct = mc.getStructure(x1,y1,z1,x2,y2,z2,did)`
## setStructure
参数:str,int,int,int,int<br>
返回值:none<br>
介绍:在xyz设放置一个结构，did是维度ID，struct是结构JSON字符串<br>
例:`mc.setStructure(struct,x,y,z,did)`
## getPlayerByXuid
参数:str<br>
返回值:Entity<br>
介绍:根据Xuid获取玩家<br>
## getPlayerList
参数:none<br>
返回值:list<br>
介绍:获取玩家列表<br>
## setDamage
参数:int<br>
返回值:none<br>
介绍:设置生物受伤的伤害值<br>
注:此函数仅在onMobHurt事件下调用才能生效<br>
## explode
参数:float,float,float,int,float,bool,float,bool<br>
返回值:none<br>
介绍:产生一个爆炸(a1~a3 xyz,a4 did,a5 power,a6 destroy,a7 range,a8 fire)<br>
---
# Entity
所有玩家和实体指针都存放在这个类中
## 属性列表
* name - str - 实体名字（可修改）
* uuid - str - 玩家UUID
* xuid - str - 玩家XUID
* pos - list - 实体坐标
* did - int - 实体维度ID
* isstand - bool - 实体是否站立在方块上
* issneak - bool - 实体是否潜行
* health - int - 实体当前生命
* maxhealth - int - 实体最大生命
* perm - int - 玩家权限值（可修改，有0,1,2,3,4）
* deviceid - int - 玩家设备ID
* deviceos - int - 玩家设备操作系统（1代表Android，7代表Win10）
* ip - str - 玩家IP
## getAllItem
参数:none<br>
返回值:str<br>
介绍:获取玩家所有物品，返回的是JSON字符串<br>
共有五种容器:Hand,OffHand,Inventory,Armor,EndChest<br>
## setAllItem
参数:str<br>
返回值:none<br>
介绍:设置玩家所有物品<br>
共有四种容器:OffHand,Inventory,Armor,EndChest<br>
## setHand
参数:str<br>
返回值:none<br>
介绍:设置玩家手中物品<br>
## addItem
参数:str<br>
返回值:none<br>
介绍:新增物品<br>
## removeItem
参数:int,int<br>
返回值:none<br>
介绍:移除玩家背包a1格子的a2个物品<br>
## teleport
参数:float,float,float,int<br>
返回值:none<br>
介绍:传送玩家到指定坐标和维度<br>
## sendCommandPacket
参数:str<br>
返回值:none<br>
介绍:模拟玩家执行命令<br>
## sendTextPacket
参数:str,int = 0<br>
返回值:none<br>
介绍:向玩家发送一个文本包（0为原始文本，1为聊天文本）<br>
## resendAllChunks
参数:none<br>
返回值:none<br>
介绍:刷新玩家区块<br>
## disconnect
参数:str<br>
返回值:none<br>
介绍:断开玩家连接<br>
## transferServer
参数:str,int<br>
返回值:none<br>
介绍:传送玩家到指定服务器<br>
## getScore
参数:str<br>
返回值:int<br>
介绍:获取玩家计分板分数<br>
## modifyScore
参数:str,int,int<br>
返回值:none<br>
介绍:修改玩家分数，第三个参数是修改模式{0:set,1:add,2:remove}<br>
## addLevel
参数:int<br>
返回值:none<br>
介绍:增加玩家经验等级<br>
## setBossBar
参数:str,float<br>
返回值:none<br>
介绍:设置玩家BOSS栏，a2是血条百分比(min:0,max:1)<br>
## removeBossBar
参数:none<br>
返回值:none<br>
介绍:移除玩家BOSS栏<br>
## addTag
参数:str<br>
返回值:none<br>
介绍:增加标签<br>
## removeTag
参数:str<br>
返回值:none<br>
介绍:移除标签<br>
## getTags
参数:none<br>
返回值:str<br>
介绍:获取标签<br>
## setSidebar
参数:str,str<br>
返回值:none<br>
介绍:设置玩家侧边栏，原理是发包<br>
例:`player.setSidebar('自定义侧边栏', '{"第一行":0, "第二行":2, "我在哪行?":3}')`<br>
## removeSidebar
参数:none<br>
返回值:none<br>
介绍:移除玩家自定义侧边栏<br>
## sendSimpleForm
参数:str,str,str<br>
返回值:int<br>
介绍:向指定的玩家发送一个简单表单<br>
例:player.sendSimpleForm('致命选项', '请选择:', '[{"text":"生存"},{"text":"死亡"},{"text":"求助"}]')<br>
## sendModalForm
参数:str,str,str,str<br>
返回值:int<br>
介绍:向指定的玩家发送一个模式对话框<br>
例:player.sendModalForm('没有第三个选项', '请选择:', '生存', '死亡')<br>
## sendCustomForm
参数:str<br>
返回值:int<br>
介绍:向指定的玩家发送一个自定义表单<br>
例:player.sendCustomForm('{"content":[{"type":"label","text":"这是一个文本标签"},{"placeholder":"水印文本","default":"","type":"input","text":""},{"default":true,"type":"toggle","text":"开关~或许是吧"},{"min":0.0,"max":10.0,"step":2.0,"default":3.0,"type":"slider","text":"游标滑块！？"},{"default":1,"steps":["Step1","Step2","Step 3"],"type":"step_slider","text":"矩阵滑块？!"},{"default":1,"options":["Option 1","Option2","Option3"],"type":"dropdown","text":"如你所见 下拉框"}], "type":"custom_form","title":"这是一个自定义窗体"}')<br>
## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/twoone-3/BDSpyrunner/edit/master/docs/index.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/twoone-3/BDSpyrunner/settings/pages). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://support.github.com/contact) and we’ll help you sort it out.
