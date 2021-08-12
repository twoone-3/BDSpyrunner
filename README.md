# BDSpyrunner
![](https://img.shields.io/github/license/twoone-3/BDSpyrunner)
![](https://img.shields.io/github/downloads/twoone-3/BDSpyrunner/total)
![](https://img.shields.io/github/v/release/twoone-3/BDSpyrunner)
![](https://img.shields.io/github/commit-activity/m/twoone-3/BDSpyrunner)
![](https://img.shields.io/github/issues/twoone-3/BDSpyrunner)
# 简介
**BDSpyrunner**(以下简称pyr)是一个运行在BDS(Bedrock Dedicated Server)的的插件加载平台<br>
pyr通过使用CPython与Detours给Python提供多种监听器和API函数,有效简化了插件的开发成本
# 安装
1. 下载 **Release** 中的 **BDSpyrunner.dll** 与 **BDSpyrunner.pdb** (可选)
2. 前往 https://www.python.org/ftp/python/3.7.9/python-3.7.9-amd64.exe 下载安装 **Python3.7.9** ,记得勾选添加到**环境变量**
2. 将dll放入 **./plugins** 文件夹,运行BDS,会自动生成py文件夹,如果看到**[BDSpyrunner] x.x.x loaded.**,则pyr加载成功了
7. 将插件放入 **./plugins/py** ,再次运行BDS,就可以正常使用了
# 插件开发
参考
* [API接口](https://github.com/twoone-3/BDSpyrunner/wiki/API)
* [监听器](https://github.com/twoone-3/BDSpyrunner/wiki/Listener)
# 联系我们
QQ group : 1167270197
