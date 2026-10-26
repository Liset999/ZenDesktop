@echo off
title ZenDesktop Uninstaller
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if not exist "%~dp0uninstall.ps1" (
    color 0C
    echo.
    echo [ERROR] uninstall.ps1 not found next to uninstall.bat.
    echo.
    pause
    exit /b 1
)

fsutil dirty query %systemdrive% >nul 2>&1
if errorlevel 1 goto NeedElevation
goto RunUninstall

:NeedElevation
color 0B
echo.
echo ============================================================
echo [INFO] Administrator privileges are required.
echo [INFO] A UAC prompt will open next -- click Yes.
echo [INFO] Uninstall progress appears in the NEW elevated window.
echo ============================================================
echo.
echo [INFO] Requesting elevation via UAC...
echo.

if not exist "%~dp0elevate-run.vbs" (
    color 0E
    echo [ERROR] elevate-run.vbs missing next to uninstall.bat.
    echo [HINT] Right-click uninstall.bat and choose "Run as administrator".
    echo.
    pause
    exit /b 1
)

set "ZD_ERR=%TEMP%\ZenDesktop_elevate_uninstall.err"
del /q "%ZD_ERR%" >nul 2>&1

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
echo      Click Yes, then watch the elevated window for uninstall status.
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
echo [HINT] Right-click uninstall.bat and choose "Run as administrator".
echo.
pause
exit /b 1

:RunUninstall
cd /d "%~dp0"
color 0B
chcp 65001 >nul

echo ============================================================
echo   ZenDesktop Uninstall (PowerShell Bootstrapper)
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0uninstall.ps1" %*
set "UN_EXIT=!errorlevel!"

if not "!UN_EXIT!"=="0" (
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
