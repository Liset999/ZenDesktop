# -*- coding: utf-8 -*-
"""
DesktopTaskbarHelper - Main Application Entry (Headless Daemon Edition)
This module acts as an invisible background service that:
1. Enables/disables desktop icons visibility on double-click.
2. Silently orchestrates TranslucentTB portable tool for premium taskbar effects.
3. Automatically sets HKCU Startup Run key to enable run-on-boot.
4. Harmonizes process lifecycle: exits cleanly when TranslucentTB is closed by the user.
"""

import os
import sys
import time
import winreg
import win32gui
import win32con

# Force import resolution from base directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from desktop_handler import DesktopHandler
from taskbar_handler import TaskbarHandler

# Paths and Keys
REG_RUN_KEY = r"Software\Microsoft\Windows\CurrentVersion\Run"
REG_APP_NAME = "DesktopTaskbarHelper"

# Global handler references
desktop_handler = None
taskbar_handler = None

def set_start_on_boot(enabled):
    """
    Sets or deletes HKCU startup registry key for the helper executable.
    """
    try:
        key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, REG_RUN_KEY, 0, winreg.KEY_SET_VALUE)
        if enabled:
            if getattr(sys, 'frozen', False):
                cmd = f'"{sys.executable}"'
            else:
                script_path = os.path.abspath(sys.argv[0])
                cmd = f'"{sys.executable}" "{script_path}"'
            winreg.SetValueEx(key, REG_APP_NAME, 0, winreg.REG_SZ, cmd)
        else:
            try:
                winreg.DeleteValue(key, REG_APP_NAME)
            except FileNotFoundError:
                pass
        winreg.CloseKey(key)
        return True
    except Exception as e:
        print(f"[Registry] Unable to modify startup registry key: {e}")
        return False

def main():
    global desktop_handler, taskbar_handler
    
    # 1. Automatically register program to boot startup registry key
    set_start_on_boot(True)
    
    # 2. Instantiate and configure handlers
    desktop_handler = DesktopHandler()
    desktop_handler.enabled = True
    
    taskbar_handler = TaskbarHandler()
    
    # 3. Spin up daemon helper listeners/threads
    desktop_handler.start()
    taskbar_handler.start()
    
    # Check if taskbar handler successfully started or attached to TranslucentTB
    if not taskbar_handler.running:
        import ctypes
        ctypes.windll.user32.MessageBoxW(
            0,
            "无法启动或下载 TranslucentTB 任务栏透明引擎。\n\n"
            "请确保：\n"
            "1. 您的电脑已联网（本程序会自动从 GitHub 下载依赖），或者\n"
            "2. 您已经手动运行了 TranslucentTB，或者\n"
            "3. 将包含 TranslucentTB 的 'ext' 文件夹复制到了本程序同级目录下。\n\n"
            "本程序需要与 TranslucentTB 协同运行以提供完整的任务栏特效，程序即将退出。",
            "ZenDesktop - 启动失败",
            0x10 | 0x0  # MB_ICONERROR | MB_OK
        )
        sys.exit(1)
    
    print("[Engine] DesktopTaskbarHelper background engine successfully started.")
    print("[Engine] TranslucentTB (TTB) orchestrated. Monitoring life cycle...")
    
    # 4. Life cycle harmonization loop
    # We poll every 2 seconds to check if TranslucentTB is still running.
    # If the user exits TranslucentTB via its official system tray icon,
    # we detect it and perform a graceful clean shutdown immediately.
    try:
        # Give TranslucentTB some initial time to spawn/inject
        time.sleep(3)
        while True:
            if not taskbar_handler._is_running():
                print("[Engine] TranslucentTB is closed by the user. Exiting daemon helper...")
                break
            time.sleep(2)
    except (KeyboardInterrupt, SystemExit):
        print("[Engine] Interrupted. Shutting down...")
    finally:
        # Clean shutdown and hook detaching
        if desktop_handler:
            desktop_handler.stop()
        if taskbar_handler:
            taskbar_handler.stop()
        print("[Engine] Graceful cleanup finished. Goodbye.")

if __name__ == "__main__":
    main()
