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
    #
    # NOTE: We intentionally do NOT bundle TranslucentTB inside the EXE (no --add-data).
    # TranslucentTB injects ExplorerHooks.dll into explorer.exe, and the DLL's disk path
    # gets locked in Explorer's memory. If the path changes between runs (which happens
    # with PyInstaller's temp _MEIPASS directories), TranslucentTB throws a version
    # mismatch error. Keeping ext/ as a sibling folder ensures the path is always stable.
    pyinstaller_args = [
        app_path,
        '--onefile',
        '--noconsole',
        '--clean',
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
            
            # Post-build: copy ext/ folder next to the EXE for distribution
            import shutil
            dist_ext = os.path.join(current_dir, "dist", "ext")
            src_ext = os.path.join(current_dir, "ext")
            if os.path.exists(src_ext):
                if os.path.exists(dist_ext):
                    shutil.rmtree(dist_ext)
                shutil.copytree(src_ext, dist_ext)
                print(f" -> Copied ext/ folder to dist/ext/")
                
                # Create a ready-to-distribute zip
                import zipfile
                zip_path = os.path.join(current_dir, "dist", "ZenDesktop_Portable.zip")
                with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zf:
                    zf.write(exe_path, "ZenDesktop/DesktopTaskbarHelper.exe")
                    for root, dirs, files in os.walk(dist_ext):
                        for f in files:
                            full = os.path.join(root, f)
                            arcname = os.path.join("ZenDesktop", "ext", os.path.relpath(full, dist_ext))
                            zf.write(full, arcname)
                zip_mb = os.path.getsize(zip_path) / (1024 * 1024)
                print(f" -> Distribution zip: {zip_path}")
                print(f" -> Zip Size: {zip_mb:.2f} MB")
            
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
