@echo off
cd /d "%~dp0"
net session >nul 2>&1
if errorlevel 1 (
  powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
  exit /b
)
echo Killing explorer so the DLL can unload...
taskkill /F /IM explorer.exe >nul 2>&1
timeout /t 2 /nobreak >nul

echo Unregistering COM...
regsvr32 /u /s "%~dp0ExplorerBlurMica.dll"

echo Removing leftover CLSID keys...
reg delete "HKLM\SOFTWARE\Classes\CLSID\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Classes\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Classes\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /f >nul 2>&1

echo Starting explorer...
start explorer.exe
echo Uninstall done. You can close this window.
timeout /t 4 >nul