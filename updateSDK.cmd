git submodule deinit --all -f
git rm --cached sdk
rmdir sdk /S /Q
rmdir .git\modules /S /Q
git submodule add --depth=1 https://github.com/LiteLDev/SDK-cpp.git sdk
cd sdk\tools
LibraryBuilder -o ..\lib ..\..\..\BDS
pause