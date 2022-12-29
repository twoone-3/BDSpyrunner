# BDSpyrunner

![Liscense](https://img.shields.io/github/license/twoone-3/BDSpyrunner)
![Downloads](https://img.shields.io/github/downloads/twoone-3/BDSpyrunner/total)
![BDS](https://img.shields.io/badge/support--BDS--version-1.19.51.01-blue)

[简体中文](README.md) | English

# Warning! This branch is developed based on the main branch Release1.8.7, API and Listener are not as rich as the main branch, please pay attention to the listener and API used by your plugin when using

# Introduction

`BDSpyrunner` is a plug-in loader for `BDS` (Bedrock Dedicated Server).
It uses CPython and Detours to connect Python and BDS so that Python can operate some things in Minecraft.
It greatly reduces the threshold for plug-in development and effectively simplifies the cost of plug-in development.

# Usage

1. Download `BDSpyrunner.dll`, `mc.py` and `BDSpyrunner.pdb` from `Release`. The pdb file is not required, but it will come in handy for feedback in case of crashes
2. Make sure you have installed [LiteLoader](https://github.com/LiteLDev/LiteLoaderBDS) or [BDXCore](https://github.com/jfishing/BDXCore)
3. Download and install [Python3.7.9](https://www.python.org/ftp/python/3.7.9/python-3.7.9-amd64.exe), check `Add Python to Path` when installing
4. Put `BDSpyrunner.dll` and `BDSpyrunner.pdb` into the folders you use to store plugins, such as `plugins`, `bdxcore_mod`
5. Put `mc.py` into the `. /plugins/py` folder
6. Find the Python installation directory, copy the Dlls and Lib folders to the `./plugins/py` folder

# Documents

* [Wiki](https://docs.jfishing.love/docs/en/README.md)

# Contact us

* Please submit an issue if you encounter a problem
