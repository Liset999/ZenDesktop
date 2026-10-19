@echo off
title ZenDesktop One-Key Deploy Bootstrapper
setlocal EnableDelayedExpansion

cd /d "%~dp0"

:: 检查是否已解压（防止在压缩包内直接双击导致找不到文件闪退）
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

:: privilege detection and elevation
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    echo [INFO] 正在申请管理员权限，请在弹出的 UAC 窗口中点击「是」...
    if "%~1"=="" (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs } catch { exit 1 }"
    ) else (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -ArgumentList '%*' -WorkingDirectory '%~dp0' -Verb RunAs } catch { exit 1 }"
    )
    if !errorLevel! neq 0 (
        color 0E
        echo.
        echo [提示] 自动获取管理员权限未成功（可能被取消或安全策略限制）。
        echo 请直接右键 deploy.bat，选择【以管理员身份运行】。
        echo.
        pause
    )
    exit /b
)

cd /d "%~dp0"
color 0B

echo ============================================================
echo   ZenDesktop One-Key Deploy (PowerShell Bootstrapper)
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
