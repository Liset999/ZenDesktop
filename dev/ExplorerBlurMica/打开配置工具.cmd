@echo off
cd /d "%~dp0"
powershell.exe -NoLogo -NoProfile -STA -ExecutionPolicy Bypass -File "%~dp0config-editor.ps1"
if errorlevel 1 (
  echo Failed to start config-editor.ps1
  pause
)