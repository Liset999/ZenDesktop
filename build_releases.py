import zipfile
import os

common_files = [
    'local@zen-taskbar-acrylic.wh.cpp',
    'local@zen-notificationcenter-acrylic.wh.cpp',
    'local@zen-startmenu-acrylic.wh.cpp',
    'local@zen-desktop-toggle-icons.wh.cpp',
    'local@zen-explorer-styler.wh.cpp',
    'local@zen-taskbar-lyrics.wh.cpp',
    'local@zen-ime-caret-hint.wh.cpp',
    'local@windows-11-settings-styler-fork.wh.cpp',
    'deploy.bat',
    'deploy.ps1',
    'uninstall.bat',
    'uninstall.ps1',
    'LICENSE',
    'Readme.txt',
    'AAA使用前必看.txt',
    'CHANGELOG_v5.0.0.md',
    'CHANGELOG_v5.2.0.md',
    'CHANGELOG_v5.2.5.md',
    'README.md',
    'docs/donate-wechat.png',
    'docs/screenshots',  # README.md preview images
    'restart_explorer.bat',
    'THIRD_PARTY_NOTICES-taskbar-lyrics.md',
    'tools/uninstall-ExplorerBlurMica.bat',  # for v4.1 upgraders: removes the old EBM registration
    'tools/ZenDesktop-StartupCheck.ps1',  # optional, installed only with -StartupCheck
    'tools/Initialize-DesktopWidgetSettings.ps1',
    'tools/Get-WobblyDiagnostics.ps1',
    'docs/wobbly-diagnostics.md',
    'tools/update-desktop-widgets.ps1',
    'tools/desktop-widget-defaults.json',
    'docs/desktop-widgets-3.0.md',
    'dev/widgets/vendor/LICENSE-TourneTable.txt',
    'tools/ai-status',
    # v4.2: optional mods (installed only on opt-in by deploy.ps1)
    'optional/local@wobbly-windows.wh.cpp',
    'optional/local@windows-animations.wh.cpp',
    'optional/local@zen-desktop-widgets.wh.cpp',
    'optional/zen-desktop-host.h',
    'optional/zen-game-guard.h',
    'optional/THIRD_PARTY_NOTICES.md',
]

releases = {
    'ZenDesktop_OneKeyDeploy_v5.2.5.zip': common_files + ['windhawk_setup.exe'],
    'ZenDesktop_OneKeyDeploy_v5.2.5_Offline.zip': common_files + ['windhawk_setup_offline.exe'],
    'ZenDesktop_OneKeyDeploy_v5.2.5_Online.zip': common_files
}

# Windhawk expects the metadata marker at the beginning of the source file.
for source in (f for f in common_files if f.endswith('.wh.cpp')):
    with open(source, 'rb') as mod_file:
        if not mod_file.read(18).startswith(b'// ==WindhawkMod=='):
            raise ValueError(f'Invalid Windhawk metadata header: {source}')

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
                raise FileNotFoundError(f'Required release file not found: {f}')
    if os.path.exists(zip_name):
        print(f"Successfully created {zip_name}. Size: {os.path.getsize(zip_name)} bytes\n")
