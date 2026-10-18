@echo off
echo ===================================================
echo   ZenDesktop: Instantly Restart Explorer.exe
echo ===================================================
echo Killing explorer.exe...
taskkill /f /im explorer.exe
taskkill /f /im StartMenuExperienceHost.exe
taskkill /f /im SearchHost.exe
echo Starting explorer.exe...
start explorer.exe
echo Done! All Shell host processes restarted successfully.
exit
