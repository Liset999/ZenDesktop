@echo off
title ZenDesktop 一键部署
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if not exist "%~dp0deploy.ps1" (
    color 0C
    echo.
    echo ============================================================
    echo [错误] 请不要在压缩包内直接双击运行！
    echo.
    echo 请先将压缩包【完整解压】到一个文件夹中，
    echo 然后在该文件夹内运行 deploy.bat。
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
echo [提示] 需要管理员权限才能部署。
echo [提示] 接下来会弹出 UAC，请点击「是」。
echo [提示] 部署进度会显示在【新的管理员窗口】里。
echo ============================================================
echo.
echo [提示] 正在申请管理员权限...
echo.

set "ZD_BAT=%~f0"
set "ZD_DIR=%~dp0"
set "ZD_ARGS=%*"

:: Primary: PowerShell Start-Process -Verb RunAs. No double quotes inside the
:: -Command string (cmd would split on them and run "&" itself); paths come in via env vars.
powershell -NoProfile -ExecutionPolicy Bypass -Command "try { $q=[char]34; $a='/c '+$q+$q+$env:ZD_BAT+$q+' '+$env:ZD_ARGS+$q; Start-Process -FilePath $env:ComSpec -ArgumentList $a -WorkingDirectory $env:ZD_DIR -Verb RunAs -ErrorAction Stop; exit 0 } catch { Write-Host $_.Exception.Message; exit 1 }"
if not errorlevel 1 goto ElevateLaunched

:: Fallback: VBScript (VBScript may be disabled on newer Windows 11 builds)
if not exist "%~dp0elevate-run.vbs" goto ElevateFailed
cscript //nologo "%~dp0elevate-run.vbs" "%~f0" "%~dp0" "%*"
if errorlevel 1 goto ElevateFailed

:ElevateLaunched
echo.
echo [完成] 应已弹出 UAC（或已打开管理员窗口）。
echo        请点击「是」，然后在管理员窗口查看部署结果。
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
echo [提示] 请右键 deploy.bat，选择【以管理员身份运行】。
echo.
pause
exit /b 1

:RunDeploy
cd /d "%~dp0"
color 0B
:: UTF-8 console so PowerShell Chinese status lines render correctly on zh-CN Windows
chcp 65001 >nul

echo ============================================================
echo   ZenDesktop 一键部署（PowerShell）
echo   增量部署：内容未变的 Mod 源码不会被覆盖
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0deploy.ps1" %*
set "DEPLOY_EXIT=!errorlevel!"

if not "!DEPLOY_EXIT!"=="0" (
    color 0C
    echo.
    echo [错误] 部署失败！请查看上方报错信息。
    echo.
    pause
    exit /b !DEPLOY_EXIT!
)

echo.
pause
exit /b 0
