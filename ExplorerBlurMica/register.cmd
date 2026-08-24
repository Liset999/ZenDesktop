@echo off

title Asking for administrator access
mode CON COLS=37 LINES=3
color F0
echo :::::::::::::::::::::::::::::::::::::
echo :: Requesting administrator access ::
echo :::::::::::::::::::::::::::::::::::::
cd /d "%~dp0" && ( if exist "%temp%\getadmin.vbs" del "%temp%\getadmin.vbs" ) && fsutil dirty query %systemdrive% 1>nul 2>nul || (  cmd /u /c echo Set UAC = CreateObject^("Shell.Application"^) : UAC.ShellExecute "cmd.exe", "/k cd ""%~sdp0"" && ""%~s0"" %Apply%", "", "runas", 1 >> "%temp%\getadmin.vbs" && "%temp%\getadmin.vbs" && exit /B )
color
cls

title Applying
call :isAdmin
if %errorlevel% == 0 (
    echo Applying effects
	regsvr32 "%~dp0ExplorerBlurMica.dll"
	REM config-editor-wpf.exe looks for these two CLSIDs, not Maplespe's BHO id.
	reg add "HKLM\SOFTWARE\Classes\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /ve /t REG_SZ /d "ExplorerBlurMica BHO" /f >nul
	reg add "HKLM\SOFTWARE\Classes\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}\InProcServer32" /ve /t REG_SZ /d "%~dp0ExplorerBlurMica.dll" /f >nul
	reg add "HKLM\SOFTWARE\Classes\CLSID\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}\InProcServer32" /v ThreadingModel /t REG_SZ /d Apartment /f >nul
	reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}" /v NoInternetExplorer /t REG_DWORD /d 1 /f >nul
	reg add "HKLM\SOFTWARE\Classes\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}" /ve /t REG_SZ /d "ExplorerBlurMica BHO" /f >nul
	reg add "HKLM\SOFTWARE\Classes\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}\InProcServer32" /ve /t REG_SZ /d "%~dp0ExplorerBlurMica.dll" /f >nul
	reg add "HKLM\SOFTWARE\Classes\CLSID\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}\InProcServer32" /v ThreadingModel /t REG_SZ /d Apartment /f >nul
    echo.
) else (
	echo Please run as Administrator
)

echo Restarting explorer
taskkill /F /IM explorer.exe >nul
start explorer.exe
echo .
echo title Success
echo Changes applied successfully
echo You can now close this window, it will close automatically in 5 seconds
timeout /t 5 >nul
exit /b 0

:isAdmin
fsutil dirty query %systemdrive% >nul
exit /b %errorlevel%