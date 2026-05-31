@echo off
title ZenDesktop One-Key Deploy Bootstrapper
setlocal EnableDelayedExpansion

:: privilege detection and elevation
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    echo [INFO] Requesting Administrator Privileges...
    powershell -Command "Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
color 0B

echo ============================================================
echo   ZenDesktop One-Key Deploy (PowerShell Bootstrapper)
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0deploy.ps1"

if %errorLevel% neq 0 (
    color 0C
    echo.
    echo [ERROR] Deployment failed! See errors above.
    echo.
    pause
    exit /b %errorLevel%
)

exit /b 0
