git submodule deinit --all -f
git rm --cached SDK
rmdir SDK /S /Q
rmdir ".git/modules" /S /Q
git submodule add https://github.com/LiteLDev/LiteLoaderSDK SDK
cd SDK\Tools
LibraryBuilder.exe -o ..\Lib "C:\Users\31976\Desktop\BDS"
pause