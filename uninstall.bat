@echo off
title ZenDesktop Uninstaller
setlocal EnableDelayedExpansion

cd /d "%~dp0"

:: privilege detection and elevation (same as deploy.bat)
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    color 0B
    echo.
    echo ============================================================
    echo [INFO] Administrator privileges are required.
    echo [INFO] A UAC prompt will open next -- click Yes.
    echo [INFO] Uninstall progress appears in the NEW elevated window.
    echo ============================================================
    echo.
    if "%~1"=="" (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs | Out-Null; exit 0 } catch { exit 1 }"
    ) else (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -ArgumentList '%*' -WorkingDirectory '%~dp0' -Verb RunAs | Out-Null; exit 0 } catch { exit 1 }"
    )
    if !errorLevel! neq 0 (
        color 0E
        echo.
        echo [HINT] Could not elevate (UAC cancelled or blocked).
        echo Right-click uninstall.bat and choose "Run as administrator".
        echo.
        pause
        exit /b 1
    )
    echo.
    echo [OK] Elevated window launched (or UAC accepted).
    echo      Watch that window for uninstall status.
    echo      This starter window can close after you press a key.
    echo.
    pause
    exit /b 0
)

cd /d "%~dp0"
color 0B

:: UTF-8 console so PowerShell Chinese status lines render correctly on zh-CN Windows
chcp 65001 >nul

echo ============================================================
echo   ZenDesktop Uninstall (PowerShell Bootstrapper)
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0uninstall.ps1" %*
set UN_EXIT=!errorLevel!

if !UN_EXIT! neq 0 (
    color 0C
    echo.
    echo [ERROR] Uninstall failed! See errors above.
    echo.
    pause
    exit /b !UN_EXIT!
)

echo.
pause
exit /b 0
