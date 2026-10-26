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
    color 0B
    echo.
    echo ============================================================
    echo [INFO] Administrator privileges are required.
    echo [INFO] A UAC prompt will open next -- click Yes.
    echo [INFO] Deploy progress appears in the NEW elevated window.
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
        echo Right-click deploy.bat and choose "Run as administrator".
        echo.
        pause
        exit /b 1
    )
    echo.
    echo [OK] Elevated window launched (or UAC accepted).
    echo      Watch that window for deploy status.
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
echo   ZenDesktop One-Key Deploy (PowerShell Bootstrapper)
echo   Incremental: only changed mod sources are overwritten
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0deploy.ps1" %*
set DEPLOY_EXIT=!errorLevel!

if !DEPLOY_EXIT! neq 0 (
    color 0C
    echo.
    echo [ERROR] Deployment failed! See errors above.
    echo.
    pause
    exit /b !DEPLOY_EXIT!
)

:: Elevated / interactive runs: keep the window open so the summary can be read.
echo.
pause
exit /b 0
