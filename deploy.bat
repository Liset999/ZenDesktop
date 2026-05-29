@echo off
title ZenDesktop One-Key Deploy v3.1.0

:: Auto-elevate to Admin
net session >nul 2>&1
if %errorLevel% neq 0 (
    powershell -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
color 0B

echo.
echo  ============================================================
echo    ZenDesktop Premium Theme - One-Key Deploy v3.1.0
echo  ============================================================
echo    3 independent local mods, no conflict with originals
echo  ============================================================
echo.

:: Step 1: Detect Windhawk (installed or portable)
echo [1/7] Detecting Windhawk...
set WINDHAWK_MODS=C:\ProgramData\Windhawk\ModsSource
set WINDHAWK_IS_PORTABLE=0
set WINDHAWK_DIR=

:: Check portable in subfolder (e.g. Windhawk\windhawk.exe)
if exist "%~dp0Windhawk\windhawk.exe" (
    set WINDHAWK_DIR=%~dp0Windhawk\
    set WINDHAWK_MODS=%~dp0Windhawk\AppData\ModsSource
    set WINDHAWK_IS_PORTABLE=1
    echo       [OK] Portable Windhawk detected (Windhawk\windhawk.exe)
    goto :found
)

:: Check portable in same folder
if exist "%~dp0windhawk.exe" (
    set WINDHAWK_DIR=%~dp0
    set WINDHAWK_MODS=%~dp0AppData\ModsSource
    set WINDHAWK_IS_PORTABLE=1
    echo       [OK] Portable Windhawk detected (same folder)
    goto :found
)

:: Check standard installation
if exist "C:\ProgramData\Windhawk\" (
    echo       [OK] Installed Windhawk detected
    goto :found
)

:: Try to install if installer exists
if exist "%~dp0windhawk_setup_offline.exe" (
    color 0E
    echo [INFO] Windhawk is not installed. Found offline setup!
    echo        Starting Windhawk Offline Setup...
    start /wait "" "%~dp0windhawk_setup_offline.exe"
    if exist "C:\ProgramData\Windhawk\" (
        color 0B
        echo       [OK] Windhawk successfully installed!
        goto :found
    )
)
if exist "%~dp0windhawk_setup.exe" (
    color 0E
    echo [INFO] Windhawk is not installed. Found web setup!
    echo        Starting Windhawk Setup...
    start /wait "" "%~dp0windhawk_setup.exe"
    if exist "C:\ProgramData\Windhawk\" (
        color 0B
        echo       [OK] Windhawk successfully installed!
        goto :found
    )
)

:: Not found
color 0C
echo [ERROR] Windhawk not found!
echo         Please install Windhawk first: https://windhawk.net
echo         Or put deploy.bat next to the Windhawk folder.
pause
exit /b

:found
if not exist "%WINDHAWK_MODS%" mkdir "%WINDHAWK_MODS%" >nul 2>&1
echo.

:: Step 2: Stop Windhawk to safely copy files
echo [2/7] Stopping Windhawk service...
net stop Windhawk
taskkill /f /im windhawk.exe >nul 2>&1
echo       [OK] Windhawk stopped
echo.

:: Step 3: Disable original conflicting mods
echo [3/7] Disabling original mods to prevent conflicts...
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-11-taskbar-styler" /v "Disabled" /t REG_DWORD /d 1 /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-11-start-menu-styler" /v "Disabled" /t REG_DWORD /d 1 /f >nul 2>&1
echo       [OK] Original mods disabled
echo.

:: Step 4: Copy mod sources
echo [4/7] Deploying local mod sources...
copy /y "local@zen-taskbar-acrylic.wh.cpp" "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 (
    color 0C
    echo [ERROR] Failed to copy taskbar mod!
    echo         Make sure deploy.bat is in the same folder as the .wh.cpp files.
    pause
    exit /b
)
copy /y "local@zen-startmenu-acrylic.wh.cpp" "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 ( echo [ERROR] Failed to copy start menu mod! & pause & exit /b )
copy /y "local@zen-notificationcenter-acrylic.wh.cpp" "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 ( echo [ERROR] Failed to copy notification center mod! & pause & exit /b )
copy /y "local@translucent-windows.wh.cpp" "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 ( echo [ERROR] Failed to copy translucent windows mod! & pause & exit /b )
copy /y "local@zen-desktop-toggle-icons.wh.cpp" "%WINDHAWK_MODS%\"
if %errorLevel% neq 0 ( echo [ERROR] Failed to copy desktop icons mod! & pause & exit /b )
:: Clean up removed size icons mod from ModsSource
del /f /q "%WINDHAWK_MODS%\local@zen-taskbar-size-icons.wh.cpp" >nul 2>&1
echo       [OK] 5 premium mods deployed to %WINDHAWK_MODS%
echo.

:: Step 5: Register mods (installed version uses registry, portable auto-detects from ModsSource)
echo [5/7] Registering mods...
if %WINDHAWK_IS_PORTABLE%==1 (
    :: Portable: Do not force auto compile
    echo       [OK] Portable mode skipped compilation
) else (
    :: Installed: use registry
    :: Clean up removed size icons mod from registry
    reg delete "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-size-icons" /f >nul 2>&1

    :: 1. Taskbar Acrylic Styler
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Disabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Include" /t REG_SZ /d "explorer.exe" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Exclude" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Architecture" /t REG_SZ /d "x86-64" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Version" /t REG_SZ /d "2.7.0" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LibraryFileName" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic\Settings" /v "theme" /t REG_SZ /d "TranslucentTaskbar" /f >nul 2>&1

    :: 2. Start Menu Acrylic Styler
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Disabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Include" /t REG_SZ /d "StartMenuExperienceHost.exe|SearchHost.exe|SearchApp.exe" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Exclude" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Architecture" /t REG_SZ /d "x86-64" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Version" /t REG_SZ /d "2.7.0" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LibraryFileName" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic\Settings" /v "theme" /t REG_SZ /d "TranslucentStartMenu" /f >nul 2>&1

    :: 3. Notification Center Styler
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Disabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Include" /t REG_SZ /d "ShellExperienceHost.exe|ShellHost.exe" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Exclude" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Architecture" /t REG_SZ /d "x86-64" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Version" /t REG_SZ /d "2.8.0" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "LibraryFileName" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic\Settings" /v "theme" /t REG_SZ /d "TranslucentShell" /f >nul 2>&1

    :: 4. Translucent Windows (Explorer)
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows" /v "Disabled" /t REG_DWORD /d 1 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows" /v "Include" /t REG_SZ /d "*" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows" /v "Exclude" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows" /v "Architecture" /t REG_SZ /d "x86-64" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows" /v "Version" /t REG_SZ /d "1.7.4" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows\Settings" /v "type" /t REG_SZ /d "acrylicblur" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows\Settings" /v "ExtendFrame" /t REG_DWORD /d 1 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows\Settings" /v "FlyoutsEffects" /t REG_DWORD /d 1 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows\Settings" /v "ImmersiveDarkTitle" /t REG_DWORD /d 1 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows\Settings" /v "RenderingMod.ThemeBackground" /t REG_DWORD /d 1 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows\Settings" /v "RenderingMod.SysColors" /t REG_DWORD /d 1 /f >nul 2>&1

    :: 5. Desktop Toggle Icons (Double Click to Hide Icons)
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Disabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Include" /t REG_SZ /d "explorer.exe" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Exclude" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Architecture" /t REG_SZ /d "" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Version" /t REG_SZ /d "3.1.0" /f >nul 2>&1

    echo       [OK] Registry entries created
)
echo.

:: Step 6: Start Windhawk
echo [6/7] Starting Windhawk service...
if %WINDHAWK_IS_PORTABLE%==1 (
    start "" "%WINDHAWK_DIR%windhawk.exe" >nul 2>&1
) else (
    net start Windhawk
)
echo       [OK] Windhawk started
echo.

echo  ============================================================
echo    DEPLOY COMPLETE!
echo  ============================================================
echo.
echo    Windhawk is compiling 5 premium mods in background (~60 sec).
echo    Open Windhawk to check progress.
echo.
echo    Installed mods:
echo      1. ZenDesktop: Taskbar Acrylic Styler
echo      2. ZenDesktop: Start Menu Acrylic Styler
echo      3. ZenDesktop: Notification Center Acrylic Styler
echo      4. ZenDesktop: Translucent Windows (Explorer)
echo      5. ZenDesktop: Double Click to Hide Icons
echo.
echo  ============================================================
echo.
pause
