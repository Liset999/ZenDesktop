@echo off
title ZenDesktop One-Key Deploy Bootstrapper
setlocal EnableDelayedExpansion

cd /d "%~dp0"

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

:: Use "if errorlevel" + goto. Never put "|" inside if "(...)" -- that aborts cmd (flash).
fsutil dirty query %systemdrive% >nul 2>&1
if errorlevel 1 goto NeedElevation
goto RunDeploy

:NeedElevation
color 0B
echo.
echo ============================================================
echo [INFO] Administrator privileges are required.
echo [INFO] A UAC prompt will open next -- click Yes.
echo [INFO] Deploy progress appears in the NEW elevated window.
echo ============================================================
echo.
echo [INFO] Requesting elevation via UAC...
echo.

if not exist "%~dp0elevate-run.vbs" (
    color 0E
    echo [ERROR] elevate-run.vbs missing next to deploy.bat.
    echo [HINT] Right-click deploy.bat and choose "Run as administrator".
    echo.
    pause
    exit /b 1
)

set "ZD_ERR=%TEMP%\ZenDesktop_elevate_deploy.err"
del /q "%ZD_ERR%" >nul 2>&1

:: Primary: VBScript ShellExecute runas (shows UAC reliably from Explorer double-click)
cscript //nologo "%~dp0elevate-run.vbs" "%~f0" "%~dp0" "%*" 2>"%ZD_ERR%"
set "ZD_EL=!errorlevel!"
if "!ZD_EL!"=="0" goto ElevateLaunched

echo [WARN] VBScript elevation failed; trying PowerShell fallback...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process -FilePath 'cmd.exe' -ArgumentList @('/c','cd /d \"%~dp0\" & \"%~f0\"') -WorkingDirectory '%~dp0.' -Verb RunAs"
set "ZD_EL=!errorlevel!"
if not "!ZD_EL!"=="0" goto ElevateFailed

:ElevateLaunched
echo.
echo [OK] UAC prompt should be visible (or elevated window opened).
echo      Click Yes, then watch the elevated window for deploy status.
echo      This starter window stays open -- press a key after reading.
echo.
pause
exit /b 0

:ElevateFailed
color 0E
echo.
echo [ERROR] Could not show UAC / elevation failed.
if exist "%ZD_ERR%" (
    echo ---- details ----
    type "%ZD_ERR%"
    echo ----------------
)
echo.
echo [HINT] Right-click deploy.bat and choose "Run as administrator".
echo.
pause
exit /b 1

:RunDeploy
cd /d "%~dp0"
color 0B
chcp 65001 >nul

echo ============================================================
echo   ZenDesktop One-Key Deploy (PowerShell Bootstrapper)
echo   Incremental: only changed mod sources are overwritten
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0deploy.ps1" %*
set "DEPLOY_EXIT=!errorlevel!"

if not "!DEPLOY_EXIT!"=="0" (
    color 0C
    echo.
    echo [ERROR] Deployment failed! See errors above.
    echo.
    pause
    exit /b !DEPLOY_EXIT!
)

echo.
pause
exit /b 0
