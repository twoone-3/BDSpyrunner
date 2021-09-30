# BDSpyrunner
![Liscense](https://img.shields.io/github/license/twoone-3/BDSpyrunner)
![Downloads](https://img.shields.io/github/downloads/twoone-3/BDSpyrunner/total)
![Release](https://img.shields.io/github/v/release/twoone-3/BDSpyrunner)
![BDS](https://img.shields.io/badge/support--BDS--version-1.17.31.01-blue)

[简体中文](README_ZH.md) | English
# Introduction
`BDSpyrunner` is a plug-in loader for `BDS` (Bedrock Dedicated Server).
It uses CPython and Detours to connect Python and BDS so that Python can operate some things in Minecraft.
It greatly reduces the threshold for plug-in development and effectively simplifies the cost of plug-in development.
# Usage
1. Download `BDSpyrunner.dll` and `BDSpyrunner.pdb` in `Release` (optional).
2. To install pyr, you need to use [LiteLoader](https://github.com/LiteLDev/LiteLoaderBDS), please make sure you have installed LiteLoader
3. Go to https://www.python.org/ftp/python/3.7.9/python-3.7.9-amd64.exe to download and install `Python3.7.9`, remember to check add to `Environment Variables`.
4. Put the dll into the `./plugins` folder, run BDS, the py folder will be automatically generated, if you see `[BDSpyrunner] x.x.x loaded.`, the pyr has been loaded successfully.
5. Put the plug-in into `./plugins/py`, run BDS again, you can use it normally.
# Other functions
* Automatic update: Download the update_pyr.bat in the release and put it in the plugins folder to start the automatic update
# Documents
* [Wiki](https://github.com/twoone-3/BDSpyrunner/wiki/)
# Contact us
* QQ group : 1167270197
* Telegram : ![Telegram](https://img.shields.io/badge/telegram-BDSpyrunner-blue?&logo=telegram&link=https://t.me/bdspyrunner)