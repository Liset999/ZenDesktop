@echo off
title ZenDesktop Uninstaller
setlocal

:: privilege detection and elevation (same as deploy.bat)
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    echo [INFO] Requesting Administrator Privileges...
    if "%~1"=="" (
        powershell -Command "Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs"
    ) else (
        powershell -Command "Start-Process -FilePath '%~f0' -ArgumentList '%*' -WorkingDirectory '%~dp0' -Verb RunAs"
    )
    exit /b
)

cd /d "%~dp0"
color 0B

echo ============================================================
echo   ZenDesktop Uninstall (PowerShell Bootstrapper)
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0uninstall.ps1" %*

if %errorLevel% neq 0 (
    color 0C
    echo.
    echo [ERROR] Uninstall failed! See errors above.
    echo.
    pause
    exit /b %errorLevel%
)

echo.
pause
exit /b 0
