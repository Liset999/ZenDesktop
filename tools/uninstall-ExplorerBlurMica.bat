@echo off
chcp 936 >nul
title ExplorerBlurMica 卸载工具
fsutil dirty query %systemdrive% >nul 2>&1
if %errorlevel% neq 0 (
    powershell -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)
echo ====================================================
echo     ExplorerBlurMica 全局彻底卸载清理工具
echo ====================================================
echo.
echo [*] 正在查询已注册的 DLL 路径...
for /f "tokens=2,*" %%A in ('reg query "HKCR\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}\InProcServer32" /ve 2^>nul ^| findstr /i "REG_SZ"') do (
    if exist "%%B" (
        echo [发现组件] %%B
        regsvr32 /u /s "%%B"
    )
)
for /f "tokens=2,*" %%A in ('reg query "HKCR\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}\InProcServer32" /ve 2^>nul ^| findstr /i "REG_SZ"') do (
    if exist "%%B" (
        echo [发现组件] %%B
        regsvr32 /u /s "%%B"
    )
)
for /f "tokens=2,*" %%A in ('reg query "HKCR\CLSID\{B44BD3C8-E597-4E08-AE43-246CE24698E7}\InProcServer32" /ve 2^>nul ^| findstr /i "REG_SZ"') do (
    if exist "%%B" (
        echo [发现组件] %%B
        regsvr32 /u /s "%%B"
    )
)
if exist "%~dp0ExplorerBlurMica.dll" (
    regsvr32 /u /s "%~dp0ExplorerBlurMica.dll"
)
echo [*] 正在清理注册表项...
reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Classes\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Classes\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\Classes\CLSID\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\WOW6432Node\Classes\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\WOW6432Node\Classes\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /f >nul 2>&1
reg delete "HKLM\SOFTWARE\WOW6432Node\Classes\CLSID\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
reg delete "HKCU\SOFTWARE\Classes\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKCU\SOFTWARE\Classes\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /f >nul 2>&1
reg delete "HKCU\SOFTWARE\Classes\CLSID\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
reg delete "HKCR\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /f >nul 2>&1
reg delete "HKCR\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /f >nul 2>&1
reg delete "HKCR\CLSID\{B44BD3C8-E597-4E08-AE43-246CE24698E7}" /f >nul 2>&1
echo [*] 正在重启资源管理器...
taskkill /F /IM explorer.exe >nul 2>&1
timeout /t 1 /nobreak >nul
start explorer.exe
echo.
echo ====================================================
echo [成功] ExplorerBlurMica 已彻底从系统卸载并清理！
echo ====================================================
echo.
pause