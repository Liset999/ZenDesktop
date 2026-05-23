# -*- coding: utf-8 -*-
"""
DesktopTaskbarHelper - Taskbar Handler (TranslucentTB Orchestrator)

Due to Windows 11 XAML Island restrictions blocking native SetWindowCompositionAttribute,
this module now acts as a lightweight orchestrator for the open-source TranslucentTB portable app.
It launches TranslucentTB on startup and cleans it up on exit.
"""

import os
import subprocess
import threading
import urllib.request
import zipfile
import win32gui
import win32con

class TaskbarHandler:
    def __init__(self):
        self.running = False
        self.process = None
        import sys
        if getattr(sys, 'frozen', False):
            base_dir = os.path.dirname(os.path.abspath(sys.executable))
        else:
            base_dir = os.path.dirname(os.path.abspath(__file__))
        self.ext_dir = os.path.join(base_dir, "ext")
        self.ttb_dir = os.path.join(self.ext_dir, "TranslucentTB")
        self.ttb_exe = os.path.join(self.ttb_dir, "TranslucentTB.exe")

    def _download_and_extract(self):
        """Downloads TranslucentTB portable if not present."""
        if os.path.exists(self.ttb_exe):
            return True
            
        try:
            print("[TaskbarHandler] Downloading TranslucentTB Portable...")
            os.makedirs(self.ext_dir, exist_ok=True)
            url = "https://github.com/TranslucentTB/TranslucentTB/releases/download/2024.1/TranslucentTB-portable-x64.zip"
            zip_path = os.path.join(self.ext_dir, "ttb.zip")
            
            req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
            with urllib.request.urlopen(req) as response, open(zip_path, 'wb') as out_file:
                out_file.write(response.read())
                
            print("[TaskbarHandler] Extracting...")
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(self.ttb_dir)
            os.remove(zip_path)
            return True
        except Exception as e:
            print(f"[TaskbarHandler] Failed to setup TranslucentTB: {e}")
            return False

    def _is_running(self):
        """Checks if TranslucentTB is already running to avoid duplicate injection."""
        try:
            output = subprocess.check_output('tasklist /FI "IMAGENAME eq TranslucentTB.exe"', shell=True, stderr=subprocess.DEVNULL).decode(errors='ignore')
            return "TranslucentTB.exe" in output
        except Exception:
            return False

    def start(self):
        """Starts TranslucentTB in the background if not already running."""
        if self._is_running():
            print("[TaskbarHandler] TranslucentTB is already running, attaching seamlessly.")
            self.running = True
            return

        if not self._download_and_extract():
            print("[TaskbarHandler] TranslucentTB is missing and could not be downloaded.")
            return
            
        try:
            startupinfo = subprocess.STARTUPINFO()
            startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            self.process = subprocess.Popen(
                [self.ttb_exe], 
                startupinfo=startupinfo,
                creationflags=subprocess.CREATE_NO_WINDOW,
                cwd=self.ttb_dir
            )
            self.running = True
            print("[TaskbarHandler] TranslucentTB Orchestrator started.")
        except Exception as e:
            print(f"[TaskbarHandler] Error launching TranslucentTB: {e}")

    def stop(self):
        """
        Detaches from TranslucentTB without forcefully killing it.
        Force killing causes DLL injection mismatches in explorer.exe.
        """
        self.running = False
        self.process = None
        print("[TaskbarHandler] TranslucentTB Orchestrator detached.")

    def set_style(self, style):
        """
        Legacy method. 
        Style management is now delegated entirely to TranslucentTB's native system tray menu.
        """
        pass
