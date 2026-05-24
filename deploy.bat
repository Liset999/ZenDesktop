@echo off
title ZenDesktop One-Key Deploy v2.5.1

:: Request Admin
net session >nul 2>&1
if %errorLevel% neq 0 (
    powershell -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
color 0B

echo.
echo  ============================================================
echo    ZenDesktop Premium Theme - One-Key Deploy v2.5.1
echo  ============================================================
echo    3 independent local mods, no conflict with originals
echo  ============================================================
echo.

:: Step 1: Check Windhawk
echo [1/7] Checking Windhawk...
if not exist "C:\ProgramData\Windhawk\ModsSource" (
    color 0C
    echo [ERROR] Windhawk not found! Install from: https://windhawk.net
    pause
    exit /b
)
echo       [OK] Windhawk found
echo.

:: Step 2: Stop Windhawk
echo [2/7] Stopping Windhawk service...
net stop Windhawk >nul 2>&1
timeout /t 3 /nobreak >nul
echo       [OK] Service stopped
echo.

:: Step 3: Disable original conflicting mods (if they exist)
echo [3/7] Disabling original mods to prevent conflicts...
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-11-taskbar-styler" /v "Disabled" /t REG_DWORD /d 1 /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-11-start-menu-styler" /v "Disabled" /t REG_DWORD /d 1 /f >nul 2>&1
echo       [OK] Original mods disabled
echo.

:: Step 4: Copy 3 local@ source files
echo [4/7] Deploying local mod source files...
copy /y "local@zen-taskbar-acrylic.wh.cpp" "C:\ProgramData\Windhawk\ModsSource\" >nul
if %errorLevel% neq 0 ( echo [ERROR] Taskbar source copy failed! & pause & exit /b )
copy /y "local@zen-startmenu-acrylic.wh.cpp" "C:\ProgramData\Windhawk\ModsSource\" >nul
if %errorLevel% neq 0 ( echo [ERROR] Start menu source copy failed! & pause & exit /b )
copy /y "local@zen-desktop-toggle-icons.wh.cpp" "C:\ProgramData\Windhawk\ModsSource\" >nul
if %errorLevel% neq 0 ( echo [ERROR] Desktop icons source copy failed! & pause & exit /b )
echo       [OK] 3 local mod sources deployed
echo.

:: Step 5: Register local@zen-taskbar-acrylic in registry
echo [5/7] Registering mods in Windhawk Engine...

:: --- Taskbar Acrylic ---
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Disabled" /t REG_DWORD /d 0 /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Include" /t REG_SZ /d "explorer.exe" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Exclude" /t REG_SZ /d "" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Architecture" /t REG_SZ /d "x86-64" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Version" /t REG_SZ /d "2.5.1" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LibraryFileName" /t REG_SZ /d "" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic\Settings" /v "theme" /t REG_SZ /d "TranslucentTaskbar" /f >nul

:: --- Start Menu Acrylic ---
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Disabled" /t REG_DWORD /d 0 /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Include" /t REG_SZ /d "StartMenuExperienceHost.exe|SearchHost.exe|SearchApp.exe" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Exclude" /t REG_SZ /d "" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Architecture" /t REG_SZ /d "x86-64" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Version" /t REG_SZ /d "2.5.1" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LibraryFileName" /t REG_SZ /d "" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic\Settings" /v "theme" /t REG_SZ /d "TranslucentStartMenu" /f >nul

:: --- Desktop Toggle Icons (if not already registered) ---
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Disabled" /t REG_DWORD /d 0 /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "LoggingEnabled" /t REG_DWORD /d 0 /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Include" /t REG_SZ /d "explorer.exe" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Exclude" /t REG_SZ /d "" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Architecture" /t REG_SZ /d "" /f >nul
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Version" /t REG_SZ /d "2.5.1" /f >nul

echo       [OK] All 3 mods registered
echo.

:: Step 6: Enable AlwaysCompileModsLocally (required for local@ mods)
echo [6/7] Enabling local compilation...
reg add "HKLM\SOFTWARE\Windhawk\Settings" /v "AlwaysCompileModsLocally" /t REG_DWORD /d 1 /f >nul
echo       [OK] Local compilation enabled
echo.

:: Step 7: Start Windhawk
echo [7/7] Starting Windhawk (will compile 3 mods in background)...
net start Windhawk >nul 2>&1
echo       [OK] Windhawk started
echo.

echo  ============================================================
echo    DEPLOY COMPLETE!
echo  ============================================================
echo.
echo    Windhawk is compiling 3 local mods in background (~60 sec).
echo    Open Windhawk to check compilation progress.
echo.
echo    Installed mods:
echo      1. ZenDesktop: Taskbar Acrylic Styler
echo      2. ZenDesktop: Start Menu Acrylic Styler
echo      3. ZenDesktop: Double Click to Hide Icons
echo.
echo    Original mods have been DISABLED (not removed).
echo    You can re-enable them later if needed.
echo.
echo  ============================================================
echo.
pause
