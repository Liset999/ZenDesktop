import zipfile
import os

common_files = [
    'local@zen-taskbar-acrylic.wh.cpp',
    'local@zen-notificationcenter-acrylic.wh.cpp',
    'local@zen-startmenu-acrylic.wh.cpp',
    'local@zen-desktop-toggle-icons.wh.cpp',
    'local@zen-stage-manager.wh.cpp',
    'ExplorerBlurMica',
    'ZenDesktopCustomizer.py',
    'run_customizer.bat',
    'deploy.bat',
    'deploy.ps1',
    'Readme.txt',
    'README.md',
    'restart_explorer.bat'
]

releases = {
    'ZenDesktop_OneKeyDeploy_v4.0.0.zip': common_files + ['windhawk_setup.exe'],
    'ZenDesktop_OneKeyDeploy_v4.0.0_Offline.zip': common_files + ['windhawk_setup_offline.exe'],
    'ZenDesktop_OneKeyDeploy_v4.0.0_Online.zip': common_files
}

# Ensure we extract the offline installer first
if not os.path.exists('windhawk_setup_offline.exe'):
    print("Extracting offline installer from previous archive...")
    with zipfile.ZipFile('ZenDesktop_OneKeyDeploy_v2.7.0_Offline.zip') as old_zip:
        old_zip.extract('windhawk_setup_offline.exe')

for zip_name, file_list in releases.items():
    print(f"Creating {zip_name}...")
    with zipfile.ZipFile(zip_name, 'w', zipfile.ZIP_DEFLATED) as z:
        for f in file_list:
            if os.path.exists(f):
                if os.path.isdir(f):
                    for root, dirs, files in os.walk(f):
                        for file in files:
                            filepath = os.path.join(root, file)
                            z.write(filepath)
                            print(f"  Added {filepath}")
                else:
                    z.write(f)
                    print(f"  Added {f}")
            else:
                print(f"  [ERROR] File not found: {f}")
    print(f"Successfully created {zip_name}. Size: {os.path.getsize(zip_name)} bytes\n")
