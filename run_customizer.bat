@echo off
title ZenDesktop Customizer Launcher
cd /d "%~dp0"
echo [INFO] Checking Python environment...
python --version >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] Python is not installed or not in PATH!
    echo Please install Python 3.x and check "Add Python to PATH" during setup.
    pause
    exit /b 1
)

python -c "import customtkinter" >nul 2>&1
if %errorLevel% neq 0 (
    echo [INFO] CustomTkinter dependency not found. Installing via pip...
    pip install customtkinter
)

echo [INFO] Launching ZenDesktop Customizer...
start pythonw ZenDesktopCustomizer.py
exit
