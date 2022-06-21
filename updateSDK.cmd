git submodule deinit --all -f
git rm --cached SDK
rmdir /S /Q SDK
git submodule add -f https://github.com/LiteLDev/LiteLoaderSDK SDK
cd SDK\Tools
LibraryBuilder.exe -o ..\Lib "C:\Users\31976\Desktop\BDS"
pause