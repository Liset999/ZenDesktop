@echo off
title ZenDesktop 卸载
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if not exist "%~dp0uninstall.ps1" (
    color 0C
    echo.
    echo [错误] 未找到 uninstall.ps1（应与 uninstall.bat 同目录）。
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
echo [提示] 需要管理员权限才能卸载。
echo [提示] 接下来会弹出 UAC，请点击「是」。
echo [提示] 卸载进度会显示在【新的管理员窗口】里。
echo ============================================================
echo.
echo [提示] 正在申请管理员权限...
echo.

if not exist "%~dp0elevate-run.vbs" (
    color 0E
    echo [错误] 缺少 elevate-run.vbs（应与 uninstall.bat 同目录）。
    echo [提示] 请右键 uninstall.bat，选择【以管理员身份运行】。
    echo.
    pause
    exit /b 1
)

set "ZD_ERR=%TEMP%\ZenDesktop_elevate_uninstall.err"
del /q "%ZD_ERR%" >nul 2>&1

cscript //nologo "%~dp0elevate-run.vbs" "%~f0" "%~dp0" "%*" 2>"%ZD_ERR%"
set "ZD_EL=!errorlevel!"
if "!ZD_EL!"=="0" goto ElevateLaunched

echo [警告] VBScript 提权失败，尝试 PowerShell 备用方式...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process -FilePath 'cmd.exe' -ArgumentList @('/c','cd /d \"%~dp0\" & \"%~f0\"') -WorkingDirectory '%~dp0.' -Verb RunAs"
set "ZD_EL=!errorlevel!"
if not "!ZD_EL!"=="0" goto ElevateFailed

:ElevateLaunched
echo.
echo [完成] 应已弹出 UAC（或已打开管理员窗口）。
echo        请点击「是」，然后在管理员窗口查看卸载结果。
echo        本启动窗口会保留，看完提示后按任意键关闭。
echo.
pause
exit /b 0

:ElevateFailed
color 0E
echo.
echo [错误] 无法弹出 UAC / 提权失败。
if exist "%ZD_ERR%" (
    echo ---- 详细信息 ----
    type "%ZD_ERR%"
    echo ----------------
)
echo.
echo [提示] 请右键 uninstall.bat，选择【以管理员身份运行】。
echo.
pause
exit /b 1

:RunUninstall
cd /d "%~dp0"
color 0B
:: UTF-8 console so PowerShell Chinese status lines render correctly
chcp 65001 >nul

echo ============================================================
echo   ZenDesktop 卸载（PowerShell）
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0uninstall.ps1" %*
set "UN_EXIT=!errorlevel!"

if not "!UN_EXIT!"=="0" (
    color 0C
    echo.
    echo [错误] 卸载失败！请查看上方报错信息。
    echo.
    pause
    exit /b !UN_EXIT!
)

echo.
pause
exit /b 0
