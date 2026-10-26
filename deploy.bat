@echo off
title ZenDesktop One-Key Deploy Bootstrapper
setlocal EnableDelayedExpansion

cd /d "%~dp0"

:: Fail fast if the archive was not extracted (running from inside a zip)
if not exist "%~dp0deploy.ps1" (
    color 0C
    echo.
    echo ============================================================
    echo [ERROR] Do not run deploy.bat from inside the zip archive.
    echo.
    echo Extract the full archive to a folder first,
    echo then run deploy.bat from that folder.
    echo ============================================================
    echo.
    pause
    exit /b 1
)

:: privilege detection and elevation
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    echo [INFO] Requesting Administrator privileges via UAC...
    if "%~1"=="" (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs } catch { exit 1 }"
    ) else (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -ArgumentList '%*' -WorkingDirectory '%~dp0' -Verb RunAs } catch { exit 1 }"
    )
    if !errorLevel! neq 0 (
        color 0E
        echo.
        echo [HINT] Could not elevate automatically (UAC cancelled or blocked).
        echo Right-click deploy.bat and choose "Run as administrator".
        echo.
        pause
    )
    exit /b
)

cd /d "%~dp0"
color 0B

:: UTF-8 console so PowerShell Chinese status lines render correctly on zh-CN Windows
chcp 65001 >nul

echo ============================================================
echo   ZenDesktop One-Key Deploy (PowerShell Bootstrapper)
echo   Incremental: only changed mod sources are overwritten
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0deploy.ps1" %*

if %errorLevel% neq 0 (
    color 0C
    echo.
    echo [ERROR] Deployment failed! See errors above.
    echo.
    pause
    exit /b %errorLevel%
)

:: Elevated runs open a new window: keep it open so the summary can be read (interactive runs only).
if "%~1"=="" pause
exit /b 0
