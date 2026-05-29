@echo off
title ZenDesktop One-Key Deploy v3.0.0

:: ============================================================
::  Auto-elevate to Administrator
:: ============================================================
net session >nul 2>&1
if %errorLevel% neq 0 (
    powershell -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
color 0B

echo.
echo  ============================================================
echo    ZenDesktop Premium Theme - One-Key Deploy v3.0.0
echo  ============================================================
echo    4 independent local mods - no conflict with originals
echo  ============================================================
echo.

:: ============================================================
::  Step 1: Detect Windhawk (installed or portable)
:: ============================================================
echo [1/7] Detecting Windhawk installation...
set WINDHAWK_MODS=C:\ProgramData\Windhawk\ModsSource
set WINDHAWK_IS_PORTABLE=0
set WINDHAWK_DIR=

:: Check portable in subfolder (e.g. Windhawk\windhawk.exe)
if exist "%~dp0Windhawk\windhawk.exe" (
    set WINDHAWK_DIR=%~dp0Windhawk\
    set WINDHAWK_MODS=%~dp0Windhawk\AppData\ModsSource
    set WINDHAWK_IS_PORTABLE=1
    echo       [OK] Portable Windhawk detected ^(Windhawk\windhawk.exe^)
    goto :found
)

:: Check portable in same folder
if exist "%~dp0windhawk.exe" (
    set WINDHAWK_DIR=%~dp0
    set WINDHAWK_MODS=%~dp0AppData\ModsSource
    set WINDHAWK_IS_PORTABLE=1
    echo       [OK] Portable Windhawk detected ^(same folder^)
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
    echo [INFO] Windhawk not found. Launching offline installer...
    start /wait "" "%~dp0windhawk_setup_offline.exe"
    if exist "C:\ProgramData\Windhawk\" (
        color 0B
        echo       [OK] Windhawk installed successfully!
        goto :found
    )
)
if exist "%~dp0windhawk_setup.exe" (
    color 0E
    echo [INFO] Windhawk not found. Launching web installer...
    start /wait "" "%~dp0windhawk_setup.exe"
    if exist "C:\ProgramData\Windhawk\" (
        color 0B
        echo       [OK] Windhawk installed successfully!
        goto :found
    )
)

:: Windhawk not found - abort
color 0C
echo [ERROR] Windhawk could not be found or installed!
echo         Please install Windhawk first: https://windhawk.net
echo         Or place deploy.bat next to the Windhawk folder.
pause
exit /b

:found
if not exist "%WINDHAWK_MODS%" mkdir "%WINDHAWK_MODS%" >nul 2>&1
echo.

:: ============================================================
::  Step 2: Stop Windhawk service
:: ============================================================
echo [2/7] Stopping Windhawk service...
sc query Windhawk 2>&1 | findstr /i "RUNNING" >nul 2>&1
if %errorLevel%==0 (
    net stop Windhawk >nul 2>&1
)
taskkill /f /im windhawk.exe >nul 2>&1
echo       [OK] Windhawk stopped
echo.

:: ============================================================
::  Step 3: Disable conflicting original mods from Windhawk store
:: ============================================================
echo [3/7] Disabling conflicting original Windhawk store mods...
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-11-taskbar-styler"    /v "Disabled" /t REG_DWORD /d 1 /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-11-start-menu-styler" /v "Disabled" /t REG_DWORD /d 1 /f >nul 2>&1
echo       [OK] Conflicting mods disabled
echo.

:: ============================================================
::  Step 4: Clean up removed / legacy mods
:: ============================================================
echo [4/7] Cleaning up legacy mod files and registry entries...

:: Remove legacy: taskbar size icons mod (file + registry)
del /f /q "%WINDHAWK_MODS%\local@zen-taskbar-size-icons.wh.cpp"     >nul 2>&1
reg delete "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-size-icons" /f >nul 2>&1

:: Remove legacy: translucent windows mod (file + registry)
del /f /q "%WINDHAWK_MODS%\local@translucent-windows.wh.cpp"        >nul 2>&1
reg delete "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@translucent-windows"    /f >nul 2>&1

echo       [OK] Legacy mods cleaned up
echo.

:: ============================================================
::  Step 5: Copy the 4 core mod sources to Windhawk ModsSource
:: ============================================================
echo [5/7] Deploying mod source files...

copy /y "local@zen-taskbar-acrylic.wh.cpp"          "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 (
    color 0C
    echo [ERROR] Failed to copy taskbar mod!
    echo         Ensure deploy.bat is in the same folder as the .wh.cpp files.
    pause
    exit /b
)

copy /y "local@zen-notificationcenter-acrylic.wh.cpp" "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 (
    color 0C
    echo [ERROR] Failed to copy notification center mod!
    pause
    exit /b
)

copy /y "local@zen-startmenu-acrylic.wh.cpp"         "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 (
    color 0C
    echo [ERROR] Failed to copy start menu mod!
    pause
    exit /b
)

copy /y "local@zen-desktop-toggle-icons.wh.cpp"      "%WINDHAWK_MODS%\" >nul 2>&1
if %errorLevel% neq 0 (
    color 0C
    echo [ERROR] Failed to copy desktop icon toggle mod!
    pause
    exit /b
)

echo       [OK] 4 core mods deployed to: %WINDHAWK_MODS%
echo.

:: ============================================================
::  Step 6: Register mods via registry (installed mode only)
:: ============================================================
echo [6/7] Registering mods in registry...
if %WINDHAWK_IS_PORTABLE%==1 (
    echo       [OK] Portable mode - skipping registry registration
) else (
    rem  ----------------------------------------------------------
    rem   Mod 1: Taskbar Acrylic Styler
    rem  ----------------------------------------------------------
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Disabled"         /t REG_DWORD /d 0           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LoggingEnabled"   /t REG_DWORD /d 0           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Include"          /t REG_SZ    /d "explorer.exe" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Exclude"          /t REG_SZ    /d ""           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Architecture"     /t REG_SZ    /d "x86-64"     /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Version"          /t REG_SZ    /d "2.7.0"      /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LibraryFileName"  /t REG_SZ    /d ""           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic\Settings" /v "theme"   /t REG_SZ    /d "TranslucentTaskbar" /f >nul 2>&1

    rem  ----------------------------------------------------------
    rem   Mod 2: Notification Center Acrylic Styler
    rem  ----------------------------------------------------------
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Disabled"        /t REG_DWORD /d 0                                              /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "LoggingEnabled"  /t REG_DWORD /d 0                                              /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Include"         /t REG_SZ    /d "ShellExperienceHost.exe|ShellHost.exe"        /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Exclude"         /t REG_SZ    /d ""                                             /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Architecture"    /t REG_SZ    /d "x86-64"                                       /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Version"         /t REG_SZ    /d "2.8.0"                                        /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "LibraryFileName" /t REG_SZ    /d ""                                             /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic\Settings" /v "theme"  /t REG_SZ    /d "TranslucentShell"                             /f >nul 2>&1

    rem  ----------------------------------------------------------
    rem   Mod 3: Start Menu Acrylic Styler
    rem  ----------------------------------------------------------
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Disabled"        /t REG_DWORD /d 0                                                                   /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LoggingEnabled"  /t REG_DWORD /d 0                                                                   /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Include"         /t REG_SZ    /d "StartMenuExperienceHost.exe|SearchHost.exe|SearchApp.exe"          /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Exclude"         /t REG_SZ    /d ""                                                                  /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Architecture"    /t REG_SZ    /d "x86-64"                                                            /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Version"         /t REG_SZ    /d "2.7.0"                                                             /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LibraryFileName" /t REG_SZ    /d ""                                                                  /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic\Settings" /v "theme"  /t REG_SZ    /d "TranslucentStartMenu"                                              /f >nul 2>&1

    rem  ----------------------------------------------------------
    rem   Mod 4: Double-Click to Hide Desktop Icons
    rem   NOTE: Enabled by default. If you experience explorer flashing
    rem         on Windows 11 Build 26100 (24H2), disable manually.
    rem  ----------------------------------------------------------
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Disabled"        /t REG_DWORD /d 0           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "LoggingEnabled"  /t REG_DWORD /d 0           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Include"         /t REG_SZ    /d "explorer.exe" /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Exclude"         /t REG_SZ    /d ""           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Architecture"    /t REG_SZ    /d ""           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "LibraryFileName" /t REG_SZ    /d ""           /f >nul 2>&1
    reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Version"         /t REG_SZ    /d "3.0.0"      /f >nul 2>&1

    echo       [OK] Registry entries created for all 4 mods
)
echo.

:: ============================================================
::  Step 7: Start Windhawk service
:: ============================================================
echo [7/7] Starting Windhawk service...
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
echo    Windhawk will compile the 4 mods in the background (~60s).
echo    Open Windhawk to monitor compilation progress.
echo.
echo    Deployed mods:
echo      1. ZenDesktop: Taskbar Acrylic Styler
echo      2. ZenDesktop: Notification Center Acrylic Styler
echo      3. ZenDesktop: Start Menu Acrylic Styler
echo      4. ZenDesktop: Double-Click to Hide Desktop Icons
echo.
echo    Tip: Mod 4 is enabled by default. If you experience
echo         Explorer flashing on Windows 11 24H2, disable it
echo         in the Windhawk UI.
echo.
echo  ============================================================
echo.
pause
