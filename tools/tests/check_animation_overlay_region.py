"""Exercise the widget's region and animation occlusion check on a private desktop."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', type=Path, required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
animation = (root / 'optional/local@windows-animations.wh.cpp').read_text(encoding='utf-8-sig')
widgets = (root / 'optional/local@zen-desktop-widgets.wh.cpp').read_text(encoding='utf-8-sig')
checker = animation[animation.index('static bool IsTransparentXamlExplorerHostIsland('):animation.index('static PSECURITY_DESCRIPTOR BuildSharedStateSd()')]
publisher_start = widgets.index('static RECT g_taskbarOverlayRegion{};')
publisher = widgets[publisher_start:widgets.index('void RenderVisualizer() {', publisher_start)]
code = r'''
#define NOMINMAX
#include <windows.h>
#include <dwmapi.h>
#include <cmath>
#include <cstdio>

static constexpr PCWSTR kPropSwitchAnimationGhost = L"test.ghost";
// Reproduce point conversion failing for region corners outside the target.
static bool PhysicalRectToAnimationSpace(HWND, RECT*) { return false; }
static HWND GetAnimationCoordinateWindow(HWND target) { return target; }
TRANSFORM
static HWND g_overlayWnd;
static bool g_overlayInTaskbar = true;
static UINT g_swapChainWidth = 200, g_swapChainHeight = 30;
static float g_visualOffsetX = 40, g_visualOffsetY = 720;
static struct { bool on = false; float dx = 0, dy = 0; } g_split;
namespace ttgfx {
static struct {
    bool visualAttached = true, sc = true;
    float dcOffX = 30, dcOffY = 700;
    UINT w = 220, h = 56;
} g;
}
PUBLISHER
CHECKER

int main() {
    HDESK desktop = CreateDesktopW(L"ZenAnimationOverlayTest", nullptr, nullptr, 0, GENERIC_ALL, nullptr);
    if (!desktop || !SetThreadDesktop(desktop)) return 2;
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const PCWSTR overlayClass = L"DesktopAudioVisOverlay_local@zen-desktop-widgets";
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = DefWindowProcW;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.lpszClassName = L"Shell_TrayWnd";
    if (!RegisterClassW(&windowClass)) return 3;
    windowClass.lpszClassName = overlayClass;
    if (!RegisterClassW(&windowClass)) return 3;

    HWND target = CreateWindowExW(0, L"STATIC", nullptr, WS_POPUP | WS_VISIBLE,
                                 200, 200, 400, 400, nullptr, nullptr, nullptr, nullptr);
    HWND tray = CreateWindowExW(WS_EX_TOPMOST, L"Shell_TrayWnd", nullptr,
                               WS_POPUP | WS_VISIBLE, 0, 700, 900, 64,
                               nullptr, nullptr, windowClass.hInstance, nullptr);
    g_overlayWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                                   overlayClass, nullptr, WS_POPUP | WS_VISIBLE,
                                   0, 0, 900, 900, nullptr, nullptr, windowClass.hInstance, nullptr);
    if (!target || !tray || !g_overlayWnd) return 4;
    const RECT capture{200, 200, 600, 600};
    ScreenCaptureEligibility state;
    bool passed = !CanCaptureWindowFromScreen(target, capture, tray, &state) &&
                  state.blockingWindow == g_overlayWnd;
    printf("unbounded overlay blocks=%d\n", passed);
    SyncTaskbarOverlayRegion();
    RECT region{};
    const RECT expected{28, 698, 252, 758};
    const bool regionCorrect = GetWindowRgnBox(g_overlayWnd, &region) == SIMPLEREGION &&
                               EqualRect(&region, &expected);
    const bool clear = CanCaptureWindowFromScreen(target, capture, tray, &state);
    printf("text and graphics region=%d, transparent outer area allows capture=%d\n", regionCorrect, clear);
    passed = passed && regionCorrect && clear;
    g_split.on = true;
    g_split.dx = 240;
    g_split.dy = -260;
    SyncTaskbarOverlayRegion();
    bool splitBlocks = !CanCaptureWindowFromScreen(target, capture, tray, &state) &&
                       state.blockingWindow == g_overlayWnd;
    printf("shifted split still blocks=%d\n", splitBlocks);
    passed = passed && splitBlocks;
    g_split.on = false;
    SyncTaskbarOverlayRegion();
    HWND foreground = CreateWindowExW(WS_EX_TOPMOST, L"STATIC", nullptr,
                                      WS_POPUP | WS_VISIBLE, 300, 300, 80, 80,
                                      nullptr, nullptr, nullptr, nullptr);
    bool realWindowBlocks = foreground && !CanCaptureWindowFromScreen(target, capture, tray, &state) &&
                            state.blockingWindow == foreground;
    printf("ordinary foreground window still blocks=%d\n", realWindowBlocks);
    return passed && realWindowBlocks ? 0 : 1;
}
'''.replace('TRANSFORM', animation[animation.index('struct TaskbarCoordinateTransform {'):animation.index('static std::wstring GetClassNameStr(')]).replace('PUBLISHER', publisher).replace('CHECKER', checker)
with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / 'test.cpp'
    exe = Path(directory) / 'test.exe'
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23', '-static',
                    '-DWINVER=0x0A00', '-D_WIN32_WINNT=0x0A00', str(cpp),
                    '-ldwmapi', '-lgdi32', '-luser32', '-o', str(exe)], check=True)
    raise SystemExit(subprocess.run([str(exe)]).returncode)
