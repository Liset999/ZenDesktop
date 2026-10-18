@echo off
cd /d "%~dp0"
net session >nul 2>&1
if errorlevel 1 (
  echo Need Administrator
  powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
  exit /b
)
taskkill /F /IM explorer.exe >nul 2>&1
timeout /t 1 /nobreak >nul
copy /Y "%~dp0ExplorerBlurMica.dll.winui427k" "%~dp0ExplorerBlurMica.dll"
regsvr32 /s "%~dp0ExplorerBlurMica.dll"
start explorer.exe
echo Restored 427KB WinUI DLL. Close this window.
timeout /t 4 >nul