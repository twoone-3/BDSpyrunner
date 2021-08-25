@echo off
if exist ..\BDS\bdxcore_mod (
	copy .\x64\Release\BDSpyrunner.dll ..\BDS\bdxcore_mod\BDSpyrunner.dll
	copy .\x64\Release\BDSpyrunner.pdb ..\BDS\bdxcore_mod\BDSpyrunner.pdb
)