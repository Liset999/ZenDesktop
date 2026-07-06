
import re

content = open('notification_center_full.wh.cpp', 'r', encoding='utf-8').read()

# 1. Modify header
content = content.replace(
    '// @id              windows-11-notification-center-styler',
    '// @id              local@zen-notificationcenter-acrylic'
).replace(
    '// @name            Windows 11 Notification Center Styler',
    '// @name            ZenDesktop: Notification Center Acrylic Styler'
).replace(
    '// @description     Customize the Notification Center and Action Center with themes contributed by others or create your own',
    '// @description     Premium acrylic/frosted glass notification center and quick settings. Based on m417z Notification Center Styler.'
).replace(
    '// @version         1.5',
    '// @version         2.8.0'
)

# 2. Update the settings YAML - change default theme
content = content.replace(
    '- theme: ""\n  $name: Theme',
    '- theme: "ZenDesktopGlass"\n  $name: Theme',
    1
)

# 3. Add ZenDesktopGlass to options list
old_options_fragment = '  $options:\n  - "": None\n  - TranslucentShell: TranslucentShell'
new_options_fragment = '  $options:\n  - ZenDesktopGlass: ZenDesktop Glass (Recommended - Theme Aware)\n  - "": None\n  - TranslucentShell: TranslucentShell'
content = content.replace(old_options_fragment, new_options_fragment, 1)

# 4. Build the ZenDesktopGlass theme code (raw string to avoid escape issues)
zen_theme = (
    '\n'
    'const Theme g_themeZenDesktopGlass = {{\n'
    '    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12"}},\n'
    '    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12"}},\n'
    '    ThemeTargetStyles{L"Grid#ControlCenterRegion", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12"}},\n'
    '    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ControlCenterRegion", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12"}},\n'
    '    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12"}},\n'
    '    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12"}},\n'
    '    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"ActionCenter.FocusSessionControl#FocusSessionControl > Grid#FocusGrid", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12",\n'
    '        L"Padding=2,4,2,4"}},\n'
    '    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {\n'
    '        L"Background:={ThemeResource ZenBg}",\n'
    '        L"BorderThickness=0",\n'
    '        L"CornerRadius=12",\n'
    '        L"Margin=-2,-2,-2,-2"}},\n'
    '    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Border", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"ContentPresenter#PageContent", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"ScrollViewer#ListContent", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"ActionCenter.FlexibleToastView#FlexibleNormalToastView", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {\n'
    '        L"CornerRadius=10"}},\n'
    '    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot > ContentPresenter#PageHeader", {\n'
    '        L"Background=Transparent"}},\n'
    '    ThemeTargetStyles{L"ControlCenter.MediaTransportControls#MediaTransportControls > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {\n'
    '        L"Height=Auto"}},\n'
    '}, {\n'
    '    L"CommonBgBrush={ThemeResource ZenBg}",\n'
    '}, {\n'
    '    L"ZenBg@Light:=<WindhawkBlur BlurAmount=\\"25\\" TintColor=\\"#30000000\\"/>",\n'
    '    L"ZenBg@Dark:=<WindhawkBlur BlurAmount=\\"25\\" TintColor=\\"#60000000\\"/>",\n'
    '}};\n'
    '\n'
)

# 5. Insert ZenDesktopGlass theme before the existing themes
marker = '// clang-format off\n'
pos = content.find(marker)
if pos != -1:
    insert_pos = pos + len(marker)
    content = content[:insert_pos] + zen_theme + content[insert_pos:]
    print('Inserted ZenDesktopGlass theme at clang-format off marker')
else:
    # Try alternate approach
    pos2 = content.find('const Theme g_themeTranslucentShell')
    if pos2 != -1:
        content = content[:pos2] + zen_theme + content[pos2:]
        print('Inserted ZenDesktopGlass at TranslucentShell (fallback)')
    else:
        print('ERROR: Could not find insertion point')

# 6. Register theme in GetTheme function
# Find the theme lookup pattern - the notification center uses wcscmp checks
old_block = 'if (wcscmp(themeName, L"TranslucentShell") == 0) {\n        return &g_themeTranslucentShell;\n    }'
new_block = ('if (wcscmp(themeName, L"ZenDesktopGlass") == 0) {\n        return &g_themeZenDesktopGlass;\n    }\n\n'
             '    if (wcscmp(themeName, L"TranslucentShell") == 0) {\n        return &g_themeTranslucentShell;\n    }')
if old_block in content:
    content = content.replace(old_block, new_block, 1)
    print('Registered ZenDesktopGlass in GetTheme via wcscmp')
else:
    # Try finding the pattern more flexibly
    m = re.search(r'if \(wcscmp\(themeName, L"TranslucentShell"\)', content)
    if m:
        insert_at = m.start()
        insert_code = 'if (wcscmp(themeName, L"ZenDesktopGlass") == 0) {\n        return &g_themeZenDesktopGlass;\n    }\n\n    '
        content = content[:insert_at] + insert_code + content[insert_at:]
        print('Registered ZenDesktopGlass in GetTheme (regex fallback)')
    else:
        # Try the RETURN_IF_THEME macro pattern
        old_return = 'RETURN_IF_THEME(TranslucentShell)'
        new_return = 'RETURN_IF_THEME(ZenDesktopGlass)\n    RETURN_IF_THEME(TranslucentShell)'
        if old_return in content:
            content = content.replace(old_return, new_return, 1)
            print('Registered ZenDesktopGlass via RETURN_IF_THEME macro')
        else:
            print('WARNING: Could not find theme registration point')

# Write the result
with open('local@zen-notificationcenter-acrylic.wh.cpp', 'w', encoding='utf-8') as out:
    out.write(content)

print(f'Written notification center mod: {len(content)} chars, {len(content.splitlines())} lines')

# Verify key content
check = content
print('ZenDesktopGlass in file:', 'ZenDesktopGlass' in check)
print('ZenBg@Dark in file:', 'ZenBg@Dark' in check)
print('ControlCenterRegion in file:', 'Grid#ControlCenterRegion' in check)
print('ZenBg ThemeResource registered:', 'g_themeZenDesktopGlass' in check)
