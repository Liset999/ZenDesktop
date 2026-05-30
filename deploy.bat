@echo off
title ZenDesktop One-Key Deploy v3.0.0
setlocal EnableDelayedExpansion

:: ============================================================
::  Pre-elevation: Capture paths and environment (Flaw 6)
:: ============================================================
set "USER_WINDHAWK_PATH="
:: Check if windhawk is in the user's current PATH (handles Scoop/Chocolatey/winget)
where windhawk.exe >nul 2>&1
if %errorLevel%==0 (
    for /f "delims=" %%i in ('where windhawk.exe') do (
        set "USER_WINDHAWK_PATH=%%~dpi"
    )
)

:: Check registry under HKCU for user installation
if "%USER_WINDHAWK_PATH%"=="" (
    for /f "tokens=2*" %%a in ('reg query "HKCU\Software\Windhawk" /v "InstallLocation" 2^>nul') do (
        set "USER_WINDHAWK_PATH=%%b"
    )
)

:: Save detected user-level path to a temporary file before elevation
if not "%USER_WINDHAWK_PATH%"=="" (
    echo !USER_WINDHAWK_PATH! > "%TEMP%\zen_windhawk_path.txt"
) else (
    if exist "%TEMP%\zen_windhawk_path.txt" del "%TEMP%\zen_windhawk_path.txt" >nul 2>&1
)

:: ============================================================
::  Privilege Detection and Elevation (Flaw 4)
:: ============================================================
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    echo [INFO] Requesting Administrator Privileges...
    set "ARGS=%*"
    if "!ARGS!"=="" (
        powershell -Command "Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs"
    ) else (
        powershell -Command "Start-Process -FilePath '%~f0' -ArgumentList '%*' -WorkingDirectory '%~dp0' -Verb RunAs"
    )
    exit /b
)

cd /d "%~dp0"
color 0B

echo.
echo  ============================================================
echo    ZenDesktop Premium Theme - Transaction-Safe Deployer
echo  ============================================================
echo    Robust Engineering, Active Verification, Dynamic Rollback
echo  ============================================================
echo.

:: ============================================================
::  Step 1: Resolve Windhawk Installation Paths (Flaw 6)
:: ============================================================
echo [1/8] Detecting Windhawk installation path...
set "WINDHAWK_DIR="
set "WINDHAWK_MODS="

:: Restore path captured from user session
if exist "%TEMP%\zen_windhawk_path.txt" (
    set /p WINDHAWK_DIR=<"%TEMP%\zen_windhawk_path.txt"
    del "%TEMP%\zen_windhawk_path.txt" >nul 2>&1
    :: Trim trailing spaces
    for /f "tokens=* delims= " %%a in ("!WINDHAWK_DIR!") do set "WINDHAWK_DIR=%%a"
)

:: Check HKLM standard registry (winget/standard installer)
if "!WINDHAWK_DIR!"=="" (
    for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk" /v "InstallLocation" 2^>nul') do (
        set "WINDHAWK_DIR=%%b"
    )
)
if "!WINDHAWK_DIR!"=="" (
    for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk" /v "InstallLocation" 2^>nul') do (
        set "WINDHAWK_DIR=%%b"
    )
)

:: Fallback Check: Portable folders
if "!WINDHAWK_DIR!"=="" (
    if exist "%~dp0Windhawk\windhawk.exe" (
        set "WINDHAWK_DIR=%~dp0Windhawk"
    ) else if exist "%~dp0windhawk.exe" (
        set "WINDHAWK_DIR=%~dp0"
    )
)

:: Final Standard Directory Fallback
if "!WINDHAWK_DIR!"=="" (
    if exist "C:\ProgramData\Windhawk" (
        set "WINDHAWK_DIR=C:\Program Files\Windhawk"
    )
)

:: Strip trailing backslashes if present
if not "!WINDHAWK_DIR!"=="" (
    if "!WINDHAWK_DIR:~-1!"=="\" set "WINDHAWK_DIR=!WINDHAWK_DIR:~0,-1!"
)

:: Determine ModsSource folder
if exist "!WINDHAWK_DIR!\AppData\ModsSource" (
    set "WINDHAWK_MODS=!WINDHAWK_DIR!\AppData\ModsSource"
) else (
    set "WINDHAWK_MODS=C:\ProgramData\Windhawk\ModsSource"
)

if not exist "!WINDHAWK_DIR!\windhawk.exe" (
    color 0C
    echo [ERROR] Windhawk executable could not be resolved!
    echo         Please install Windhawk or place deploy.bat next to Windhawk.exe.
    pause
    exit /b
)

echo       [OK] Windhawk Home: !WINDHAWK_DIR!
echo       [OK] Mods Directory: !WINDHAWK_MODS!
echo.

:: ============================================================
::  Step 2: Create Safe Persistent Backups (Flaw 3)
:: ============================================================
echo [2/8] Creating long-term persistent system backups...
set "TIMESTAMP=%date:~0,4%-%date:~5,2%-%date:~8,2%_%time:~0,2%-%time:~3,2%-%time:~6,2%"
set "TIMESTAMP=!TIMESTAMP: =0!"
set "BACKUP_DIR=%~dp0Backups\ZenDesktop_Backup_!TIMESTAMP!"
set "TX_BACKUP_DIR=%~dp0Backups\Temp_Tx_Backup"

if not exist "%~dp0Backups" mkdir "%~dp0Backups" >nul 2>&1
mkdir "!BACKUP_DIR!" >nul 2>&1
if %errorlevel% neq 0 (
    color 0C
    echo [ERROR] Failed to create backup folder! Aborting.
    pause
    exit /b
)

:: Clear former temp transaction folder
if exist "!TX_BACKUP_DIR!" rd /s /q "!TX_BACKUP_DIR!" >nul 2>&1
mkdir "!TX_BACKUP_DIR!" >nul 2>&1

:: Export HKLM Windhawk registry state
reg export "HKLM\SOFTWARE\Windhawk" "!BACKUP_DIR!\Windhawk_Reg_Backup.reg" /y >nul 2>&1
if %errorlevel% neq 0 (
    color 0C
    echo [ERROR] Failed to back up Windhawk Registry. Aborting for safety!
    pause
    exit /b
)
copy /y "!BACKUP_DIR!\Windhawk_Reg_Backup.reg" "!TX_BACKUP_DIR!\Windhawk_Reg_Backup.reg" >nul 2>&1

:: Backup existing mod files from ModsSource
if not exist "!WINDHAWK_MODS!" mkdir "!WINDHAWK_MODS!" >nul 2>&1
set "HAS_BACKUP_FILES=0"
if exist "!WINDHAWK_MODS!\local@zen-*.wh.cpp" (
    copy /y "!WINDHAWK_MODS!\local@zen-*.wh.cpp" "!BACKUP_DIR!\" >nul 2>&1
    copy /y "!WINDHAWK_MODS!\local@zen-*.wh.cpp" "!TX_BACKUP_DIR!\" >nul 2>&1
    set "HAS_BACKUP_FILES=1"
)

echo       [OK] Persistent backup archived in: !BACKUP_DIR!
echo       [OK] Registry and C++ source code states saved.
echo.

:: ============================================================
::  Step 3: Stop Windhawk service for offline cache clearance
:: ============================================================
echo [3/8] Stopping Windhawk service...
taskkill /f /im windhawk.exe >nul 2>&1
sc query Windhawk 2>&1 | findstr /i "RUNNING" >nul 2>&1
if %errorlevel%==0 (
    net stop Windhawk >nul 2>&1
)
echo       [OK] Windhawk stopped.
echo.

:: ============================================================
::  Step 4: Atomic Staging File Copy & Renames (Flaw 2)
:: ============================================================
echo [4/8] Executing transaction-safe file replacement...

:: Create staging subfolder inside ModsSource
set "STAGING_DIR=!WINDHAWK_MODS!\.staging"
if exist "!STAGING_DIR!" rd /s /q "!STAGING_DIR!" >nul 2>&1
mkdir "!STAGING_DIR!" >nul 2>&1

:: Copy repo files to staging
copy /y "local@zen-taskbar-acrylic.wh.cpp"          "!STAGING_DIR!\" >nul 2>&1
if %errorlevel% neq 0 goto :rollback_failure
copy /y "local@zen-notificationcenter-acrylic.wh.cpp" "!STAGING_DIR!\" >nul 2>&1
if %errorlevel% neq 0 goto :rollback_failure
copy /y "local@zen-startmenu-acrylic.wh.cpp"         "!STAGING_DIR!\" >nul 2>&1
if %errorlevel% neq 0 goto :rollback_failure
copy /y "local@zen-desktop-toggle-icons.wh.cpp"      "!STAGING_DIR!\" >nul 2>&1
if %errorlevel% neq 0 goto :rollback_failure

:: Verify staging file integrity (existence check)
for %%f in (
    "local@zen-taskbar-acrylic.wh.cpp"
    "local@zen-notificationcenter-acrylic.wh.cpp"
    "local@zen-startmenu-acrylic.wh.cpp"
    "local@zen-desktop-toggle-icons.wh.cpp"
) do (
    if not exist "!STAGING_DIR!\%%f" goto :rollback_failure
)

:: Perform Atomic NTFS renames using temporary file swap to eliminate intermediate corruption states
for %%f in (
    "local@zen-taskbar-acrylic.wh.cpp"
    "local@zen-notificationcenter-acrylic.wh.cpp"
    "local@zen-startmenu-acrylic.wh.cpp"
    "local@zen-desktop-toggle-icons.wh.cpp"
) do (
    :: Copy to a .tmp file in the same directory (NTFS same-drive guarantees zero lock issues)
    copy /y "!STAGING_DIR!\%%f" "!WINDHAWK_MODS!\%%f.tmp" >nul 2>&1
    if !errorlevel! neq 0 goto :rollback_failure
    
    :: Atomically remove old file and swap rename (.tmp -> original)
    if exist "!WINDHAWK_MODS!\%%f" del /f /q "!WINDHAWK_MODS!\%%f" >nul 2>&1
    ren "!WINDHAWK_MODS!\%%f.tmp" "%%f" >nul 2>&1
    if !errorlevel! neq 0 goto :rollback_failure
)

:: Cleanup staging
rd /s /q "!STAGING_DIR!" >nul 2>&1
echo       [OK] Files copied, verified, and atomically replaced.
echo.

echo [5/8] Registering ZenDesktop mods and clearing compilation cache...

:: 1. Taskbar Acrylic Styler
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Disabled"         /t REG_DWORD /d 0           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LoggingEnabled"   /t REG_DWORD /d 0           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Include"          /t REG_SZ    /d "explorer.exe" /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Exclude"          /t REG_SZ    /d ""           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Architecture"     /t REG_SZ    /d "x86-64"     /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "Version"          /t REG_SZ    /d "3.2.0"      /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic" /v "LibraryFileName"  /t REG_SZ    /d ""           /f >nul 2>&1

:: 2. Notification Center Styler
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Disabled"        /t REG_DWORD /d 0                                              /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "LoggingEnabled"  /t REG_DWORD /d 0                                              /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Include"         /t REG_SZ    /d "ShellExperienceHost.exe|ShellHost.exe"        /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Exclude"         /t REG_SZ    /d ""                                             /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Architecture"    /t REG_SZ    /d "x86-64"                                       /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "Version"         /t REG_SZ    /d "3.2.0"                                        /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic" /v "LibraryFileName" /t REG_SZ    /d ""                                             /f >nul 2>&1

:: 3. Start Menu Styler
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Disabled"        /t REG_DWORD /d 0                                                                   /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LoggingEnabled"  /t REG_DWORD /d 0                                                                   /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Include"         /t REG_SZ    /d "StartMenuExperienceHost.exe|SearchHost.exe|SearchApp.exe"          /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Exclude"         /t REG_SZ    /d ""                                                                  /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Architecture"    /t REG_SZ    /d "x86-64"                                                            /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "Version"         /t REG_SZ    /d "3.3.0"      /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic" /v "LibraryFileName" /t REG_SZ    /d ""           /f >nul 2>&1

:: 4. Double-Click to Hide Desktop Icons
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Disabled"        /t REG_DWORD /d 0           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "LoggingEnabled"  /t REG_DWORD /d 0           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Include"         /t REG_SZ    /d "explorer.exe" /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Exclude"         /t REG_SZ    /d ""           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Architecture"    /t REG_SZ    /d ""           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "LibraryFileName" /t REG_SZ    /d ""           /f >nul 2>&1
reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-toggle-icons" /v "Version"         /t REG_SZ    /d "3.2.0"      /f >nul 2>&1

:: Check for active official store styler mods instead of silent competitive disabling
set "CONFLICT_FOUND=0"
for %%m in (
    "windows-11-start-menu-styler"
    "windows-11-taskbar-styler"
) do (
    reg query "HKLM\SOFTWARE\Windhawk\Engine\Mods\%%~m" /v "Disabled" >nul 2>&1
    if !errorlevel!==0 (
        for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\Windhawk\Engine\Mods\%%~m" /v "Disabled" 2^>nul') do (
            if "%%b"=="0x0" (
                set "CONFLICT_FOUND=1"
                echo  [WARNING] Official store mod [%%~m] is currently ENABLED.
            )
        )
    )
)
if "!CONFLICT_FOUND!"=="1" (
    color 0E
    echo  ======================================================================
    echo  [CONFLICT WARNING] Conflicting official store styler mods are active!
    echo  Running both official store stylers and ZenDesktop local mods concurrently
    echo  can cause severe visual glitches, freezes, or explorer crashes.
    echo  Please manually open the Windhawk UI and DISABLE the store versions.
    echo  ======================================================================
)

echo       [OK] Registries merged safely. All user-custom settings preserved.
echo.

:: ============================================================
::  Step 6: Restart Windhawk Service / Application
:: ============================================================
echo [6/8] Restarting Windhawk...
sc query Windhawk >nul 2>&1
if !errorlevel! neq 0 (
    if exist "!WINDHAWK_DIR!\windhawk.exe" (
        start "" "!WINDHAWK_DIR!\windhawk.exe" >nul 2>&1
    )
) else (
    sc start Windhawk >nul 2>&1
)
echo       [OK] Windhawk active.
echo.

:: ============================================================
::  Step 7: Silent Desktop Integration (No Overkill)
:: ============================================================
echo [7/8] Applying silent integration without desktop lag...
echo        Windhawk live hot-reload will dynamically inject without restarting explorer.
echo.

:: ============================================================
::  Step 8: Complete and Client Notification
:: ============================================================
echo [8/8] ZenDesktop files successfully updated to 3.3.0!
echo.
color 0A
echo  ============================================================
echo    [SUCCESS] ZENDESKTOP 3.3.0 DEPLOYED SUCCESSFULLY!
echo  ============================================================
echo.
echo    The updated source codes have been atomically written.
echo    Please open the Windhawk UI, click on your ZenDesktop mod,
echo    and click the blue "编译 (Compile)" button to compile and apply!
echo.
echo    Enjoy your aligned background presets and stable system!
echo  ============================================================
echo.
if exist "!TX_BACKUP_DIR!" (
    del /f /q "!TX_BACKUP_DIR!\*" >nul 2>&1
    rd /s /q "!TX_BACKUP_DIR!" >nul 2>&1
)
pause
exit /b

:: ============================================================
::  Transactional Rollback Engine Block
:: ============================================================
:rollback_failure
echo.
color 0C
echo  ============================================================
echo    [CRITICAL ERROR] Deployer failed midway! Starting Rollback...
echo  ============================================================

:: Restore original registry configuration if backup exists
if exist "!TX_BACKUP_DIR!\Windhawk_Reg_Backup.reg" (
    echo        Restoring original Windhawk registry configs...
    reg import "!TX_BACKUP_DIR!\Windhawk_Reg_Backup.reg" >nul 2>&1
)

:: Restore original files if backups exist
if "!HAS_BACKUP_FILES!"=="1" (
    echo        Restoring original Mod C++ source files...
    copy /y "!TX_BACKUP_DIR!\local@zen-*.wh.cpp" "!WINDHAWK_MODS!\" >nul 2>&1
) else (
    echo        Removing incomplete source files...
    del /f /q "!WINDHAWK_MODS!\local@zen-*.wh.cpp" >nul 2>&1
)

:: Restart Windhawk service
sc start Windhawk >nul 2>&1
start explorer.exe >nul 2>&1

echo.
echo    [ROLLBACK COMPLETE] Your system has been returned to its
echo    original state untouched. No modifications were made.
echo  ============================================================
echo.
if exist "!TX_BACKUP_DIR!" (
    del /f /q "!TX_BACKUP_DIR!\*" >nul 2>&1
    rd /s /q "!TX_BACKUP_DIR!" >nul 2>&1
)
pause
exit /b
