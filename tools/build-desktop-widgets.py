"""Generate a standalone Windhawk mod from shell widgets + pinned Tourne'Table.

Requires PyYAML for development only. The generated .wh.cpp has no private
includes or runtime Python dependency. Never installs, compiles or enables it.
"""
from pathlib import Path
import hashlib
import json
import re
import yaml

ROOT = Path(__file__).resolve().parents[1]
DEV = ROOT / 'dev/widgets'


def settings(source):
    return yaml.safe_load(source.split('// ==WindhawkModSettings==', 1)[1].split('/*', 1)[1].split('*/', 1)[0])


def flatten(items, prefix=''):
    result = {}
    for item in items:
        for key, value in item.items():
            if key.startswith('$'):
                continue
            name = prefix + key
            if isinstance(value, list):
                result.update(flatten(value, name + '.'))
            else:
                result[name] = int(value) if isinstance(value, bool) else value
    return result


def wstring(value):
    return 'L' + json.dumps(str(value), ensure_ascii=False)


def patch_once(source, before, after):
    if source.count(before) != 1:
        raise ValueError('Upstream adaptation anchor changed: ' + before[:90])
    return source.replace(before, after, 1)


def generate():
    core = (DEV / 'desktop-core.wh.cpp').read_text(encoding='utf-8')
    raw = (DEV / 'vendor/tourne-table-2.1.1.wh.cpp').read_bytes()
    provenance = json.loads((DEV / 'vendor/tourne-table.json').read_text())
    assert hashlib.sha256(raw).hexdigest() == provenance['sha256']
    upstream = raw.decode('utf-8-sig')
    defaults = flatten(settings(upstream))
    # Tested taskbar preset; unexposed features retain upstream defaults.
    defaults.update({
        'appearance.shape': 'stereo', 'appearance.barCount': 24,
        'appearance.barWidth': '3', 'appearance.barGap': '3',
        'appearance.barMaxSize': '22', 'appearance.barIdleSize': '1',
        'appearance.barCornerRadius': '2', 'appearance.colorMode': 'gradient',
        'position.host': 'taskbar', 'position.horizontalPosition': '25',
        'position.verticalPosition': '50', 'position.taskbarOffsetX': '0',
        'background.enabled': 0,
        # The unified Chinese settings page owns interaction/configuration.
        'interaction.contextMenu': 'off', 'validation.showErrors': 0,
        'position.panelBackdrop': 'none',
        'interaction.dragModifier': 'none', 'interaction.dragButton': 'left',
    })
    aliases = {
        'appearance.shape': 'shape', 'appearance.barCount': 'bars',
        'appearance.barWidth': 'barWidth', 'appearance.barGap': 'barGap',
        'appearance.barMaxSize': 'barHeight',
        'appearance.gradientColor1': 'primaryColor',
        'appearance.gradientColor2': 'secondaryColor',
        'appearance.sensitivity': 'inputSensitivity',
        'appearance.autoGain': 'autoGain', 'appearance.smoothing': 'smoothing',
        'position.host': 'placement', 'position.monitor': 'monitor',
        'position.horizontalPosition': 'horizontal',
        'position.taskbarOffsetX': 'taskbarOffset',
        'position.verticalPosition': 'vertical',
        'performance.targetFps': 'fps', 'background.enabled': 'background',
        'background.color': 'backgroundColor',
    }
    rows = []
    appearance_keys = {'bars', 'barWidth', 'barGap', 'primaryColor', 'secondaryColor'}
    advanced_keys = {'monitor', 'horizontal', 'vertical', 'inputSensitivity', 'autoGain', 'smoothing', 'fps'}
    for key, value in defaults.items():
        name = aliases.get(key)
        group = 'appearance.' if name in appearance_keys else 'advanced.' if name in advanced_keys else ''
        alias = wstring('audio.' + group + name) if name else 'nullptr'
        rows.append('    {' + ', '.join((wstring(key), wstring(value), alias)) + '},')
    adapter = '''
struct ZenAudioDefault { PCWSTR name; PCWSTR fallback; PCWSTR setting; };
const ZenAudioDefault kZenAudioDefaults[] = {
''' + '\n'.join(rows) + '''
};
const ZenAudioDefault* ZenAudioDefaultFor(PCWSTR name) {
    for (const auto& item : kZenAudioDefaults)
        if (!wcscmp(item.name, name)) return &item;
    return nullptr;
}
int ZenAudioReadInt(PCWSTR name) {
    if (!wcscmp(name, L"interaction.dragEnabled")) {
        PCWSTR host = ::Wh_GetStringSetting(L"audio.placement");
        bool taskbar = host && !wcscmp(host, L"taskbar");
        if (host) ::Wh_FreeStringSetting(host);
        return !taskbar && ::Wh_GetIntSetting(L"dragMode") != 0;
    }
    // The desktop widget has no card, including when upgrading old settings.
    if (!wcscmp(name, L"background.enabled")) return 0;
    const auto* item = ZenAudioDefaultFor(name);
    if (!item) return 0;
    return item->setting ? ::Wh_GetIntSetting(item->setting) : _wtoi(item->fallback);
}
std::wstring ZenAudioReadString(PCWSTR name) {
    const auto* item = ZenAudioDefaultFor(name);
    if (!item) return L"";
    if (!item->setting) return item->fallback;
    PCWSTR value = ::Wh_GetStringSetting(item->setting);
    std::wstring result = value && *value ? value : item->fallback;
    if (value) ::Wh_FreeStringSetting(value);
    if (!wcscmp(name, L"position.host") && result == L"both") result = L"taskbar";
    return result;
}
size_t ZenAudioGetValue(PCWSTR key, PWSTR buffer, size_t count) {
    return ::Wh_GetStringValue((std::wstring(L"audio.")+key).c_str(), buffer, count);
}
BOOL ZenAudioSetValue(PCWSTR key, PCWSTR value) {
    return ::Wh_SetStringValue((std::wstring(L"audio.")+key).c_str(), value);
}
'''
    body = upstream[upstream.index('#include <windhawk_utils.h>'):]
    includes = re.findall(r'^#include <[^>]+>', body, re.M)
    body = re.sub(r'^#include <[^>]+>\s*$', '', body, flags=re.M)
    body = patch_once(body, 'return Wh_GetIntSetting(name);', 'return ZenAudioReadInt(name);')
    body = patch_once(body, '''        PCWSTR s = Wh_GetStringSetting(name);
        v = s;
        Wh_FreeStringSetting(s);''', '        v = ZenAudioReadString(name);')
    body = body.replace('Wh_GetStringValue(', 'ZenAudioGetValue(').replace('Wh_SetStringValue(', 'ZenAudioSetValue(')
    panel_host = (DEV / 'panel-host.cpp').read_text(encoding='utf-8')
    start = body.index('void VizPanelBackdrop() {')
    end = body.index('// "Ctrl+Alt+V"', start)
    body = body[:start] + panel_host + '\n' + body[end:]
    body = patch_once(body, '    VizPanelLoad();\n    RECT r = g_panelRect;', '''    g_panelRect = {};
    g_panelZoom = 1.f;
    if (!::Wh_GetIntSetting(L"resetPosition")) VizPanelLoad();
    g_panelPinned = g_panelHidden = false;
    RECT r = g_panelRect;''')
    body = patch_once(body, '''        r.left = (mi.rcWork.left + mi.rcWork.right - w) / 2;
        r.top = (mi.rcWork.top + mi.rcWork.bottom - h) / 2;''', '''        r.left = mi.rcWork.left + (int)((mi.rcWork.right - mi.rcWork.left - w) * g_settings.horizontalPosition / 100.f);
        r.top = mi.rcWork.top + (int)((mi.rcWork.bottom - mi.rcWork.top - h) * g_settings.verticalPosition / 100.f);''')
    body = patch_once(body, 'WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW | (g_panelPinned ? WS_EX_TOPMOST : 0)',
                      'WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | (g_settings.dragEnabled ? 0 : WS_EX_TRANSPARENT)')
    body = patch_once(body, '''OVERLAY_WINDOW_CLASS, L"Tourne'Table", WS_POPUP | WS_THICKFRAME, r.left, r.top,
                                  r.right - r.left, r.bottom - r.top, nullptr, nullptr, hInstance, nullptr);''', '''OVERLAY_WINDOW_CLASS, nullptr, WS_POPUP, r.left, r.top,
                                  r.right - r.left, r.bottom - r.top, GetShellWindow(), nullptr, hInstance, nullptr);''')
    body = patch_once(body, 'LRESULT VizPanelHitTest(HWND hWnd, LPARAM lParam) {',
                      'LRESULT VizPanelHitTest(HWND hWnd, LPARAM lParam) {\n    if (!g_settings.dragEnabled) return HTTRANSPARENT;')
    body = patch_once(body, '''            case WM_SYSCOMMAND:
                if ((wParam & 0xFFF0) == SC_MAXIMIZE''', '''            case WM_MOUSEACTIVATE:
                return MA_NOACTIVATE;
            case WM_SYSCOMMAND:
                if (!g_settings.dragEnabled && ((wParam & 0xFFF0) == SC_MOVE || (wParam & 0xFFF0) == SC_SIZE)) return 0;
                if ((wParam & 0xFFF0) == SC_MAXIMIZE''')
    body = patch_once(body, '''    VizPanelBackdrop();
    VizPanelHotkey();

    // Creation at startup''', '''    if (::Wh_GetIntSetting(L"resetPosition")) {
        ClearPositionOverride();
        ZenAudioSetValue(L"panel", L"");
        g_panelRect = {};
        g_panelZoom = 1.f;
        if (g_overlayInPanel && g_overlayWnd) PostMessage(g_overlayWnd, WM_APP_CLEANUP, 0, 0);
    }
    VizPanelBackdrop();
    VizPanelHotkey();

    // Creation at startup''')
    body = patch_once(body, 'BOOL WhTool_ModInit() {', 'BOOL WhTool_ModInit() {\n    if (!ZwAudioWanted()) return FALSE;')
    # Win11 24H2: Wallpaper Engine paints children of Progman's WorkerW.
    # Put the visualization beside that WorkerW, below the icons, not under it.
    body = patch_once(body, '    HWND hWorkerW = nullptr;\n    EnumWindows(', '''    if ((GetWindowLongPtr(hProgman, GWL_EXSTYLE) & WS_EX_NOREDIRECTIONBITMAP) &&
        FindWindowEx(hProgman, nullptr, L"WorkerW", nullptr)) return hProgman;
    HWND hWorkerW = nullptr;
    EnumWindows(''')
    body = patch_once(body, '        g_overlayInTaskbar = false;\n        g_overlayInPanel = false;', '''        HWND icons = FindWindowEx(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (icons) SetWindowPos(g_overlayWnd, icons, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
        g_overlayInTaskbar = false;
        g_overlayInPanel = false;''')
    # Taskbar overlay window region. In taskbar mode the tool process owns a
    # full virtual-screen child window while only painting the bars, and the
    # window animation mod's capture check counted that empty window as an
    # occluder. Publish just the painted surfaces as the window region.
    body = patch_once(body, 'void RenderVisualizer() {', '''static RECT g_taskbarOverlayRegion{};
static bool g_taskbarOverlayRegionValid = false;
static bool g_settingTaskbarOverlayRegion = false;

static void SyncTaskbarOverlayRegion() {
    if (!g_overlayInTaskbar || !g_overlayWnd || g_settingTaskbarOverlayRegion) return;

    // Include the whole extent of each attached surface. The host still uses
    // virtual desktop coordinates, but its unused area needn't occlude captures.
    RECT bounds{};
    auto includeSurface = [&](float x, float y, UINT width, UINT height) {
        if (!width || !height) return;
        RECT surface{static_cast<LONG>(floorf(x)) - 2,
                     static_cast<LONG>(floorf(y)) - 2,
                     static_cast<LONG>(ceilf(x + width)) + 2,
                     static_cast<LONG>(ceilf(y + height)) + 2};
        UnionRect(&bounds, &bounds, &surface);
    };
    const auto& graphics = ttgfx::g;
    auto includeSurfaces = [&](float dx, float dy) {
        includeSurface(g_visualOffsetX + dx, g_visualOffsetY + dy,
                       g_swapChainWidth, g_swapChainHeight);
        if (graphics.visualAttached && graphics.sc) {
            includeSurface(graphics.dcOffX + dx, graphics.dcOffY + dy,
                           graphics.w, graphics.h);
        }
    };
    includeSurfaces(0.f, 0.f);
    if (g_split.on) includeSurfaces(g_split.dx, g_split.dy);

    if (IsRectEmpty(&bounds) || (g_taskbarOverlayRegionValid &&
                                 EqualRect(&bounds, &g_taskbarOverlayRegion))) return;
    HRGN region = CreateRectRgnIndirect(&bounds);
    g_settingTaskbarOverlayRegion = true;
    if (region && SetWindowRgn(g_overlayWnd, region, FALSE)) {
        g_taskbarOverlayRegion = bounds;
        g_taskbarOverlayRegionValid = true;
    } else {
        if (region) DeleteObject(region);
        // An old region could clip a surface that has moved or grown.
        SetWindowRgn(g_overlayWnd, nullptr, FALSE);
        g_taskbarOverlayRegionValid = false;
    }
    g_settingTaskbarOverlayRegion = false;
}

''' + 'void RenderVisualizer() {')
    body = patch_once(body, '''void RenderVisualizer() {
    if (g_unloading || !g_dc || !g_swapChain) return;''', '''void RenderVisualizer() {
    if (g_unloading || !g_dc || !g_swapChain) return;
    struct SyncRegionOnExit {
        ~SyncRegionOnExit() { SyncTaskbarOverlayRegion(); }
    } syncRegion;''')
    body = patch_once(body, '''        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;''', '''        case WM_WINDOWPOSCHANGED: {
            if (g_settingTaskbarOverlayRegion) break;
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;''')
    body = patch_once(body, '''        case WM_DESTROY:
            ReleaseSwapChainResources();''', '''        case WM_DESTROY:
            g_taskbarOverlayRegionValid = false;
            ReleaseSwapChainResources();''')
    # Taskbar left/right offset, in pixels, on top of the Position percentage.
    body = patch_once(body, '''    float horizontalPosition = 50.0f;
    float verticalPosition = 88.0f;''', '''    float horizontalPosition = 50.0f;
    float taskbarOffsetX = 0.0f;     // taskbar mode only, physical px nudge
    float verticalPosition = 88.0f;''')
    body = patch_once(body, '    g_settings.monitor = std::max(1, VizSettingInt(L"position.monitor"));',
                      '    g_settings.monitor = std::max(1, VizSettingInt(L"position.monitor"));\n'
                      '    g_settings.taskbarOffsetX = ReadSizeSetting(L"position.taskbarOffsetX", -4000.f, 4000.f, 0.f);')
    body = patch_once(body, '    if (!g_overlayInPanel && !g_settings.dockApp.empty() && g_dockActive.load(std::memory_order_relaxed)) {',
                      '''    // Taskbar mode only: slide the bars clear of the taskbar's own furniture.
    // A percentage of the taskbar's width is a clumsy way of saying "40 px
    // further right", so this offset is pixels and adds to that percentage.
    if (g_settings.hostTaskbar && !g_overlayInPanel && g_settings.taskbarOffsetX != 0.0f)
        blockX = std::clamp(blockX + g_settings.taskbarOffsetX, waLeft,
                            std::max(waLeft, waLeft + workWidth - totalWidth));
    if (!g_overlayInPanel && !g_settings.dockApp.empty() && g_dockActive.load(std::memory_order_relaxed)) {''')
    shell_settings = [x for x in settings(core) if 'audio' not in x]
    audio_settings = yaml.safe_load((DEV / 'audio-settings.yaml').read_text(encoding='utf-8'))
    # Keep the clock advanced section last, as requested by the user.
    index = next(i for i, item in enumerate(shell_settings) if 'clockAdvanced' in item)
    merged_settings = shell_settings[:index] + audio_settings + shell_settings[index:]
    metadata_yaml = yaml.safe_dump(merged_settings, allow_unicode=True, sort_keys=False, width=150)
    core = re.sub(r'(// ==WindhawkModSettings==\s*/\*).*?(\*/\s*// ==/WindhawkModSettings==)',
                  lambda m: m[1]+'\n'+metadata_yaml+m[2], core, count=1, flags=re.S)
    core = re.sub(r'// @version\s+[^\r\n]+', '// @version         3.0.5', core, count=1)
    core = core.replace('// @include         explorer.exe', '// @include         explorer.exe\n// @include         windhawk.exe')
    core = re.sub(r'^// @architecture[^\r\n]+', '// @architecture    x86\n// @architecture    x86-64', core, count=1, flags=re.M)
    core = core.replace('// @author          Lanbo, m417z', '// @author          Lanbo, m417z, USER-TOURNE, Salyts, GR0UD')
    if '// @license ' not in core:
        core = core.replace('// @version         3.0.5', '// @version         3.0.5\n// @license         GPL-3.0')
    options = []
    for source in (core, upstream):
        for option in re.search(r'// @compilerOptions\s+([^\r\n]+)', source)[1].split():
            if option not in options: options.append(option)
    core = re.sub(r'// @compilerOptions\s+[^\r\n]+', '// @compilerOptions ' + ' '.join(options), core, count=1)
    core = core.replace('- **2.0.0**：', '- **3.0.5**：任务栏音频可调左右位置（像素），不再和任务栏中间挤在一起的图标、文字重叠。\n- **3.0.4**：任务栏音频窗口只占音柱实际绘制区域，不再作为整屏遮挡干扰窗口动画的截图判断。\n- **3.0.3**：音频起始、结束颜色改为中文预设下拉选择，无需填写色值。\n- **3.0.2**：桌面音频无底色和窗口边框，与时钟、日历共用拖动摆放和重置位置；音频设置按常用、外观、高级排列，保留旧设置。\n- **3.0.1**：音频改用 Tourne’Table 2.1.1 社区引擎，同一 Mod 设置页与独立音频进程，支持双架构，无需另外安装；首次默认关闭。\n- **2.0.0**：')
    license_text = (DEV / 'vendor/LICENSE-TourneTable.txt').read_text(encoding='utf-8')
    notice = '// BEGIN PINNED TOURNE TABLE\n/*\nTourne\'Table 2.1.1 by USER-TOURNE.\n' + provenance['source'] + '\nSHA256 '+provenance['sha256']+'\n\n'+license_text+'\n*/\n'
    shell_start = core.index('#pragma clang diagnostic push')
    core = core[:shell_start] + '#ifdef _WIN64 // Explorer-only shell widgets\n' + core[shell_start:] + '\n#endif // _WIN64 shell widgets\n'
    output = core + '\n' + '\n'.join(dict.fromkeys(includes)) + '\n'
    output += '\nbool ZwAudioWanted() { return Wh_GetIntSetting(L"audio.enabled") != 0; }\n'
    output += notice + 'namespace ZenAudio {\n' + adapter + '\n' + body + '\n} // namespace ZenAudio\n// END PINNED TOURNE TABLE\n'
    output += (DEV / 'lifecycle.cpp').read_text(encoding='utf-8')
    output = '\n'.join(line.rstrip() for line in output.splitlines()) + '\n'
    (ROOT / 'optional/local@zen-desktop-widgets.wh.cpp').write_text(output, encoding='utf-8')
    (ROOT / 'tools/desktop-widget-defaults.json').write_text(json.dumps(flatten(merged_settings),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('Generated standalone desktop widgets 3.0.5; upstream audio source SHA256 verified.')


if __name__ == '__main__':
    generate()
