@echo off
echo ===================================================
echo   ZenDesktop: Instantly Restart Explorer.exe
echo ===================================================
echo Killing explorer.exe...
taskkill /f /im explorer.exe
echo Starting explorer.exe...
start explorer.exe
echo Done! Explorer restarted successfully in 1 second.
exit
