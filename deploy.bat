@echo off
title ZenDesktop One-Key Deploy v4.0.0
setlocal EnableDelayedExpansion

:: ============================================================
::  Pre-elevation: Capture paths from user environment
:: ============================================================
set "USER_WINDHAWK_PATH="
where windhawk.exe >nul 2>&1
if %errorLevel%==0 (
    for /f "delims=" %%i in ('where windhawk.exe') do (
        set "USER_WINDHAWK_PATH=%%~dpi"
    )
)
if "%USER_WINDHAWK_PATH%"=="" (
    for /f "tokens=2*" %%a in ('reg query "HKCU\Software\Windhawk" /v "InstallLocation" 2^>nul') do (
        set "USER_WINDHAWK_PATH=%%b"
    )
)
if not "%USER_WINDHAWK_PATH%"=="" (
    (echo !USER_WINDHAWK_PATH!) > "%TEMP%\zen_windhawk_path.txt"
) else (
    if exist "%TEMP%\zen_windhawk_path.txt" del "%TEMP%\zen_windhawk_path.txt" >nul 2>&1
)

:: ============================================================
::  Privilege Detection and Elevation
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
echo    ZenDesktop One-Key Deploy
echo  ============================================================
echo.

:: ============================================================
::  Step 1: Resolve Windhawk Installation Paths
:: ============================================================
echo [1/4] Detecting Windhawk installation path...
set "WINDHAWK_DIR="
set "WINDHAWK_MODS="

:: Restore path captured from user session
if exist "%TEMP%\zen_windhawk_path.txt" (
    set /p WINDHAWK_DIR=<"%TEMP%\zen_windhawk_path.txt"
    del "%TEMP%\zen_windhawk_path.txt" >nul 2>&1
    for /f "tokens=* delims= " %%a in ("!WINDHAWK_DIR!") do set "WINDHAWK_DIR=%%a"
)

:: Try running process (most reliable)
if "!WINDHAWK_DIR!"=="" (
    for /f "delims=" %%a in ('powershell -NoProfile -Command "(Get-Process windhawk -ErrorAction SilentlyContinue | Select-Object -First 1).Path" 2^>nul') do (
        if not "%%a"=="" set "WINDHAWK_DIR=%%~dpa"
    )
)

:: Try where in admin context
if "!WINDHAWK_DIR!"=="" (
    for /f "delims=" %%a in ('where windhawk.exe 2^>nul') do (
        set "WINDHAWK_DIR=%%~dpa"
        if not "!WINDHAWK_DIR!"=="" goto :found_exe
    )
)

:: Registry checks
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

:: Portable fallback
if "!WINDHAWK_DIR!"=="" (
    if exist "%~dp0Windhawk\windhawk.exe" (
        set "WINDHAWK_DIR=%~dp0Windhawk"
    ) else if exist "%~dp0windhawk.exe" (
        set "WINDHAWK_DIR=%~dp0"
    )
)

:: Final fallback
if "!WINDHAWK_DIR!"=="" (
    if exist "C:\Program Files\Windhawk\windhawk.exe" (
        set "WINDHAWK_DIR=C:\Program Files\Windhawk"
    )
)

:found_exe
if not "!WINDHAWK_DIR!"=="" (
    if "!WINDHAWK_DIR:~-1!"=="\" set "WINDHAWK_DIR=!WINDHAWK_DIR:~0,-1!"
)

:: Determine ModsSource folder
if exist "!WINDHAWK_DIR!\AppData\ModsSource" (
    set "WINDHAWK_MODS=!WINDHAWK_DIR!\AppData\ModsSource"
) else if exist "C:\ProgramData\Windhawk\ModsSource" (
    set "WINDHAWK_MODS=C:\ProgramData\Windhawk\ModsSource"
) else (
    set "WINDHAWK_MODS=!WINDHAWK_DIR!\AppData\ModsSource"
)

if not exist "!WINDHAWK_DIR!\windhawk.exe" (
    color 0C
    echo [ERROR] Windhawk executable could not be resolved!
    echo         Please install Windhawk or place deploy.bat next to Windhawk.exe.
    pause
    exit /b
)

if not exist "!WINDHAWK_MODS!" mkdir "!WINDHAWK_MODS!" >nul 2>&1

echo       [OK] Windhawk Home: !WINDHAWK_DIR!
echo       [OK] Mods Directory: !WINDHAWK_MODS!
echo.

:: ============================================================
::  Step 2: Stop Windhawk
:: ============================================================
echo [2/4] Stopping Windhawk...
taskkill /f /im windhawk.exe >nul 2>&1
timeout /t 1 /nobreak >nul
echo       [OK] Windhawk stopped.
echo.

:: ============================================================
::  Step 3: Copy mod files & clear compiler cache
:: ============================================================
echo [3/4] Copying mod files and clearing compiler cache...

set "FILES_COPIED=0"
for %%f in (
    "local@zen-taskbar-acrylic.wh.cpp"
    "local@zen-notificationcenter-acrylic.wh.cpp"
    "local@zen-startmenu-acrylic.wh.cpp"
    "local@zen-desktop-toggle-icons.wh.cpp"
) do (
    if exist "%~dp0%%~f" (
        copy /y "%~dp0%%~f" "!WINDHAWK_MODS!\" >nul 2>&1
        if !errorlevel! equ 0 (
            echo       [OK] %%~f
            set /a FILES_COPIED+=1
            :: Extract Mod ID from filename (remove .wh.cpp)
            set "MOD_NAME=%%~nf"
            set "MOD_ID=!MOD_NAME:.wh=!"
            :: Clear compiled DLL path to force Windhawk to recompile on startup
            reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\!MOD_ID!" /v "LibraryFileName" /t REG_SZ /d "" /f >nul 2>&1
        ) else (
            echo       [FAIL] %%~f
        )
    ) else (
        echo       [SKIP] %%~f - not found
    )
)

if !FILES_COPIED! equ 0 (
    color 0C
    echo [ERROR] No mod files were copied. Check that .wh.cpp files exist in the script folder.
    pause
    exit /b
)

echo.

:: ============================================================
::  Step 4: Restart Windhawk
:: ============================================================
echo [4/4] Restarting Windhawk...
start "" "!WINDHAWK_DIR!\windhawk.exe" >nul 2>&1
if %errorlevel% neq 0 (
    sc start Windhawk >nul 2>&1
    if !errorlevel! neq 0 start windhawk.exe >nul 2>&1
)

echo       [OK] Windhawk started.
echo.
color 0A
echo  ============================================================
echo    [SUCCESS] ZenDesktop deployed successfully!
echo  ============================================================
echo.
echo    Open Windhawk UI and click "Compile" to apply the mods.
echo  ============================================================
echo.
pause
exit /b
