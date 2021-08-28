@echo off
timeout /t 5 /nobreak
if exist plugins\\py\\cache (
	copy plugins\\py\\cache\\BDSpyrunner.dll plugins\\BDSpyrunner.dll
	copy plugins\\py\\cache\\BDSpyrunner.pdb plugins\\BDSpyrunner.pdb
	rd /s /q plugins\\py\\cache
)
exit