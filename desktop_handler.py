# -*- coding: utf-8 -*-
"""
DesktopTaskbarHelper - Desktop Handler
This module implements the global mouse hooking using pynput and win32gui.
It intercepts left-clicks, checks the system double-click speed interval,
and verifies if the clicked point belongs to empty space on the desktop.
If it is empty space, it triggers the native icon visibility toggle.
"""
import logging

import time
import ctypes
import threading
import win32api
import win32gui
import win32con
import win32process
from pynput import mouse

log = logging.getLogger(__name__)

# Struct definitions for cross-process hit testing on SysListView32
class POINT(ctypes.Structure):
    _fields_ = [("x", ctypes.c_long), ("y", ctypes.c_long)]

class LVHITTESTINFO(ctypes.Structure):
    """Must match the full Win32 LVHITTESTINFO struct (including iGroup added in Vista+).
    A missing field causes struct size mismatch in cross-process memory operations,
    leading to corrupted iItem read-back and false empty-space detection."""
    _fields_ = [
        ("pt", POINT),
        ("flags", ctypes.c_uint),
        ("iItem", ctypes.c_int),
        ("iSubItem", ctypes.c_int),
        ("iGroup", ctypes.c_int),  # Critical: required since Windows Vista
    ]

# Win32 Constants for memory operations and ListView messages
PROCESS_VM_OPERATION = 0x0008
PROCESS_VM_READ = 0x0010
PROCESS_VM_WRITE = 0x0020
MEM_COMMIT = 0x1000
MEM_RELEASE = 0x8000
PAGE_READWRITE = 0x04
LVM_HITTEST = 0x1012

class DesktopHandler:
    """
    Manages global mouse hooks to toggle desktop icon visibility on double-clicking
    empty areas of the desktop.
    """
    def __init__(self, on_toggle_callback=None):
        self.enabled = True
        self.listener = None
        self.last_click_time = 0
        self.last_click_pos = (0, 0)
        self.on_toggle_callback = on_toggle_callback
        
    def find_shelldll_defview(self):
        """
        Finds the handle to the active SHELLDLL_DefView window, which is responsible
        for managing desktop display and accepting WM_COMMAND toggle messages.
        """
        # Attempt 1: Search under the standard Progman (Program Manager)
        progman = win32gui.FindWindow("Progman", "Program Manager")
        if progman:
            shelldll = win32gui.FindWindowEx(progman, 0, "SHELLDLL_DefView", None)
            if shelldll:
                return shelldll
                
        # Attempt 2: Search within WorkerW windows (active when wallpaper engines/Windows active backgrounds are running)
        results = []
        def enum_cb(hwnd, results):
            try:
                class_name = win32gui.GetClassName(hwnd)
                if class_name == "WorkerW":
                    child = win32gui.FindWindowEx(hwnd, 0, "SHELLDLL_DefView", None)
                    if child:
                        results.append(child)
            except Exception:
                pass
            return True
            
        try:
            win32gui.EnumWindows(enum_cb, results)
        except Exception:
            pass
            
        if results:
            return results[0]
        return None

    def find_desktop_syslistview(self):
        """
        Finds the handle to the SysListView32 window that holds the desktop icon grid.
        """
        # Attempt 1: Search under Progman
        progman = win32gui.FindWindow("Progman", "Program Manager")
        if progman:
            shelldll = win32gui.FindWindowEx(progman, 0, "SHELLDLL_DefView", None)
            if shelldll:
                syslistview = win32gui.FindWindowEx(shelldll, 0, "SysListView32", None)
                if syslistview:
                    return syslistview
                    
        # Attempt 2: Search within WorkerW
        results = []
        def enum_cb(hwnd, results):
            try:
                class_name = win32gui.GetClassName(hwnd)
                if class_name == "WorkerW":
                    shelldll = win32gui.FindWindowEx(hwnd, 0, "SHELLDLL_DefView", None)
                    if shelldll:
                        syslistview = win32gui.FindWindowEx(shelldll, 0, "SysListView32", None)
                        if syslistview:
                            results.append(syslistview)
            except Exception:
                pass
            return True
            
        try:
            win32gui.EnumWindows(enum_cb, results)
        except Exception:
            pass
            
        if results:
            return results[0]
        return None

    def is_empty_space(self, hwnd, x, y):
        """
        Checks whether the given screen coordinate (x, y) falls on empty space
        or an icon in SysListView32, utilizing cross-process virtual memory and hit testing.
        """
        try:
            # Get the process ID of the explorer thread owning SysListView32
            _, pid = win32process.GetWindowThreadProcessId(hwnd)
            if not pid:
                log.debug("is_empty_space: failed to get PID, defaulting to False (safe)")
                return False
                
            # Open explorer.exe process memory with appropriate rights
            h_process = ctypes.windll.kernel32.OpenProcess(
                PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
                False,
                pid
            )
            if not h_process:
                # If we lack permission or cannot open, default to False (do NOT toggle)
                log.debug("is_empty_space: OpenProcess failed, defaulting to False (safe)")
                return False
                
            # Allocate space for LVHITTESTINFO inside explorer.exe's memory space
            remote_mem = ctypes.windll.kernel32.VirtualAllocEx(
                h_process,
                None,
                ctypes.sizeof(LVHITTESTINFO),
                MEM_COMMIT,
                PAGE_READWRITE
            )
            if not remote_mem:
                ctypes.windll.kernel32.CloseHandle(h_process)
                log.debug("is_empty_space: VirtualAllocEx failed, defaulting to False (safe)")
                return False
                
            # Convert screen coordinates to client coordinates relative to SysListView32
            try:
                client_x, client_y = win32gui.ScreenToClient(hwnd, (x, y))
            except Exception:
                client_x, client_y = x, y
                
            # Setup the local hit-test structure
            local_info = LVHITTESTINFO()
            local_info.pt.x = client_x
            local_info.pt.y = client_y
            local_info.flags = 0
            local_info.iItem = -1
            local_info.iSubItem = -1
            local_info.iGroup = -1
            
            # Write structure to remote process
            written = ctypes.c_size_t(0)
            ctypes.windll.kernel32.WriteProcessMemory(
                h_process,
                remote_mem,
                ctypes.byref(local_info),
                ctypes.sizeof(local_info),
                ctypes.byref(written)
            )
            
            # Send the LVM_HITTEST message
            win32gui.SendMessage(hwnd, LVM_HITTEST, 0, remote_mem)
            
            # Read back the updated structure containing results
            read_bytes = ctypes.c_size_t(0)
            ctypes.windll.kernel32.ReadProcessMemory(
                h_process,
                remote_mem,
                ctypes.byref(local_info),
                ctypes.sizeof(local_info),
                ctypes.byref(read_bytes)
            )
            
            # Free remote memory and close process handle
            ctypes.windll.kernel32.VirtualFreeEx(h_process, remote_mem, 0, MEM_RELEASE)
            ctypes.windll.kernel32.CloseHandle(h_process)
            
            # iItem == -1 means no icon/item was hit (empty space)
            is_empty = local_info.iItem == -1
            log.debug(f"is_empty_space: client=({client_x},{client_y}) iItem={local_info.iItem} flags={local_info.flags} -> empty={is_empty}")
            return is_empty
            
        except Exception as e:
            # Fallback to False under any exception — never accidentally toggle!
            log.debug(f"is_empty_space: exception {e}, defaulting to False (safe)")
            return False

    def on_click(self, x, y, button, pressed):
        """
        Callback triggered by the global pynput mouse listener.
        """
        if not self.enabled:
            return
            
        if button == mouse.Button.left and pressed:
            current_time = time.time()
            try:
                # Retrieve the OS configured double-click time in milliseconds
                double_click_time = win32gui.GetDoubleClickTime() / 1000.0
            except Exception:
                double_click_time = 0.5  # Standard default of 500ms
            
            # Get system double-click distance thresholds (in pixels)
            try:
                cx_dblclk = win32api.GetSystemMetrics(win32con.SM_CXDOUBLECLK)
                cy_dblclk = win32api.GetSystemMetrics(win32con.SM_CYDOUBLECLK)
            except Exception:
                cx_dblclk, cy_dblclk = 4, 4  # Default fallback
            
            last_x, last_y = self.last_click_pos
            time_ok = (current_time - self.last_click_time) <= double_click_time
            dist_ok = abs(x - last_x) <= cx_dblclk and abs(y - last_y) <= cy_dblclk
                
            # If left-clicks are within BOTH time AND distance thresholds
            if time_ok and dist_ok:
                try:
                    # Retrieve the window handle under the current mouse position
                    hwnd = win32gui.WindowFromPoint((x, y))
                    if hwnd:
                        class_name = win32gui.GetClassName(hwnd)
                        log.debug(f"Double-click detected at ({x},{y}) on window class: {class_name}")
                        
                        # Validate that the clicked window belongs to the desktop
                        # We MUST include "SHELLDLL_DefView" because when icons are hidden,
                        # WindowFromPoint often directly returns the shell view window itself.
                        if class_name in ("SysListView32", "WorkerW", "Progman", "SHELLDLL_DefView"):
                            syslistview = self.find_desktop_syslistview()
                            
                            # If we hit SysListView32 (icons are visible), verify empty grid space.
                            # If we hit WorkerW, Progman or SHELLDLL_DefView directly, icons are either already hidden
                            # or we clicked on raw wallpaper root, so bypass empty hit-test to guarantee restoring works!
                            if class_name == "SysListView32" and syslistview:
                                if not self.is_empty_space(syslistview, x, y):
                                    log.debug("Hit-test: clicked on an ICON, NOT toggling.")
                                    # Reset timer so this doesn't chain into the next click
                                    self.last_click_time = 0
                                    return
                                    
                            # Send toggle command to SHELLDLL_DefView
                            shelldll = self.find_shelldll_defview()
                            if shelldll:
                                log.debug("Toggling desktop icons!")
                                win32gui.SendMessage(shelldll, win32con.WM_COMMAND, 0x7402, 0)
                                if self.on_toggle_callback:
                                    self.on_toggle_callback()
                except Exception as e:
                    log.debug(f"on_click exception: {e}")
                # Reset timer after a completed double-click to prevent triple-click chaining
                self.last_click_time = 0
                self.last_click_pos = (0, 0)
            else:
                self.last_click_time = current_time
                self.last_click_pos = (x, y)

    def start(self):
        """
        Starts the mouse listener in a background thread.
        """
        if self.listener is None:
            self.listener = mouse.Listener(on_click=self.on_click)
            self.listener.start()

    def stop(self):
        """
        Stops the mouse listener thread.
        """
        if self.listener is not None:
            self.listener.stop()
            self.listener = None
