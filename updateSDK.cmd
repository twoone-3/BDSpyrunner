git submodule deinit --all -f
git rm --cached SDK
rmdir SDK /S /Q
rmdir ".git/modules" /S /Q
git submodule add -b develop https://github.com/LiteLDev/LiteLoaderSDK SDK
cd SDK\Tools
LibraryBuilder.exe -o ..\Lib "..\..\..\BDS"
pause