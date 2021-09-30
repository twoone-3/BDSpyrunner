# BDSpyrunner
![Liscense](https://img.shields.io/github/license/twoone-3/BDSpyrunner)
![Downloads](https://img.shields.io/github/downloads/twoone-3/BDSpyrunner/total)
![Release](https://img.shields.io/github/v/release/twoone-3/BDSpyrunner)
![BDS](https://img.shields.io/badge/support--BDS--version-1.17.31.01-blue)

简体中文 | [English](README.md)
# 简介
`BDSpyrunner`是一个用于`BDS`(Bedrock Dedicated Server)的插件加载器。
它使用了 CPython 和 Detours，来连接Python与BDS，使得Python可以操作Minecraft中的一些事物。
极大的降低了插件开发门槛，有效简化了插件的开发成本。
# 用法
1. 下载`Release`中的`BDSpyrunner.dll`与`BDSpyrunner.pdb`(可选)
2. 安装pyr，需要使用[LiteLoader](https://github.com/LiteLDev/LiteLoaderBDS)，请确保您已经安装了LiteLoader
3. 前往 https://www.python.org/ftp/python/3.7.9/python-3.7.9-amd64.exe 下载安装 `Python3.7.9`，记得勾选添加到`环境变量`
4. 将dll放入`./plugins`文件夹，运行BDS，会自动生成py文件夹，如果看到`[BDSpyrunner] x.x.x loaded.`，则pyr加载成功了
5. 将插件放入`./plugins/py`，再次运行BDS，就可以正常使用了
# 其他功能
* 自动更新：下载release中的update_pyr.bat并放入plugins文件夹即可开启自动更新
# 文档
* [Wiki](https://github.com/twoone-3/BDSpyrunner/wiki/)
# 联系我们
* QQ 群 : 1167270197
* Telegram : ![Telegram](https://img.shields.io/badge/telegram-BDSpyrunner-blue?&logo=telegram&link=https://t.me/bdspyrunner)