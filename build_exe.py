# -*- coding: utf-8 -*-
"""
DesktopTaskbarHelper - Build Executor
Uses PyInstaller directly to package app.py into a single, windowless (noconsole) executable.
"""

import os
import sys
import subprocess

def run_build():
    print("=" * 60)
    print("DesktopTaskbarHelper Executable Packager")
    print("=" * 60)
    
    current_dir = os.path.dirname(os.path.abspath(__file__))
    app_path = os.path.join(current_dir, "app.py")
    
    # Check if pyinstaller is installed
    try:
        import PyInstaller
        print(f" -> PyInstaller detected (version: {PyInstaller.__version__})")
    except ImportError:
        print(" -> PyInstaller not found. Installing now...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "pyinstaller"])
        import PyInstaller
        print(f" -> PyInstaller successfully installed (version: {PyInstaller.__version__})")
        
    print("[1/2] Compiling sources and bundling into single EXE...")
    
    # We will trigger the main entrypoint of PyInstaller programmatically
    import PyInstaller.__main__
    
    # Build options:
    # --onefile: Single self-contained EXE
    # --noconsole: Hide CMD window
    # --clean: Clear PyInstaller cache
    # --add-data: Bundles the entire ext/ folder containing TranslucentTB directly inside the single EXE
    ext_src = os.path.join(current_dir, "ext")
    pyinstaller_args = [
        app_path,
        '--onefile',
        '--noconsole',
        '--clean',
        f'--add-data={ext_src}{os.pathsep}ext',
        '--name=DesktopTaskbarHelper',
        f'--workpath={os.path.join(current_dir, "build")}',
        f'--distpath={os.path.join(current_dir, "dist")}',
    ]
    
    try:
        PyInstaller.__main__.run(pyinstaller_args)
        print("-" * 60)
        print("[2/2] Package compilation completed successfully!")
        
        exe_path = os.path.join(current_dir, "dist", "DesktopTaskbarHelper.exe")
        if os.path.exists(exe_path):
            size_mb = os.path.getsize(exe_path) / (1024 * 1024)
            print(f" -> Output EXE: {exe_path}")
            print(f" -> File Size: {size_mb:.2f} MB")
            print("=" * 60)
            return True
        else:
            print(" -> Error: Output EXE not found in dist/ directory.")
            return False
            
    except Exception as e:
        print(f" -> Build compilation failed: {e}")
        print("=" * 60)
        return False

if __name__ == "__main__":
    run_build()
