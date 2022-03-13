@echo off
git submodule deinit SDK
git rm --cached SDK
rmdir /S /Q SDK
git submodule add https://github.com/LiteLDev/LiteLoaderSDK SDK
pause