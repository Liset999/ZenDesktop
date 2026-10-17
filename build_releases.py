import zipfile
import os

common_files = [
    'local@zen-taskbar-acrylic.wh.cpp',
    'local@zen-notificationcenter-acrylic.wh.cpp',
    'local@zen-startmenu-acrylic.wh.cpp',
    'local@zen-desktop-toggle-icons.wh.cpp',
    'local@zen-explorer-styler.wh.cpp',
    'local@windows-11-settings-styler-fork.wh.cpp',
    'local@macos-minimize-animation.wh.cpp',
    'deploy.bat',
    'deploy.ps1',
    'Readme.txt',
    'README.md',
    'restart_explorer.bat',
    'tools/ZenDesktop-StartupCheck.ps1',  # optional, installed only with -StartupCheck
    # v4.2: optional mods (installed only on opt-in by deploy.ps1)
    'optional/local@wobbly-windows.wh.cpp',
    'optional/local@windows-animations.wh.cpp',
    'optional/THIRD_PARTY_NOTICES.md',
]

releases = {
    'ZenDesktop_OneKeyDeploy_v4.2.0.zip': common_files + ['windhawk_setup.exe'],
    'ZenDesktop_OneKeyDeploy_v4.2.0_Offline.zip': common_files + ['windhawk_setup_offline.exe'],
    'ZenDesktop_OneKeyDeploy_v4.2.0_Online.zip': common_files
}

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
                print(f"  [WARNING] File not found: {f}")
    if os.path.exists(zip_name):
        print(f"Successfully created {zip_name}. Size: {os.path.getsize(zip_name)} bytes\n")
