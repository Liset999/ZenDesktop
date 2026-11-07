"""Exercise the mod's vertical placement block with taskbar child rectangles."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = (root / 'local@zen-taskbar-lyrics.wh.cpp').read_text(encoding='utf-8-sig')
start = source.index('        int y = gap;')
end = source.index('        int ax =', start)
placement = source[start:end]
code = r'''
#include <windows.h>
#include <algorithm>
#include <cstdio>
#include <cwchar>
#define FindWindowExW MockFindWindowExW
#define GetWindowRect MockGetWindowRect
static RECT startRect{}, notifyRect{};
static bool launchBand = false;
static HWND FindWindowExW(HWND, HWND, LPCWSTR name, LPCWSTR) {
    if (!wcscmp(name, L"TrayNotifyWnd")) return (HWND)2;
    if (!wcscmp(name, launchBand ? L"LaunchBand" : L"Start")) return (HWND)1;
    return nullptr;
}
static BOOL GetWindowRect(HWND hwnd, RECT* rect) {
    *rect = hwnd == (HWND)1 ? startRect : notifyRect;
    return TRUE;
}
static int Place(bool trayMode, int offset, int dpi, int height, int screenY) {
    HWND tray = (HWND)3;
    int ch = MulDiv(1000, dpi, 96), cw = MulDiv(64, dpi, 96);
    int gap = MulDiv(offset, dpi, 96);
    auto D = [dpi](int value) { return MulDiv(value, dpi, 96); };
    RECT tr{2000, screenY, 2000 + cw, screenY + ch};
    POINT origin{tr.left, tr.top};
    struct { int h, cx, cy; } L{height, 0, 0};
''' + placement + r'''
    return L.cy;
}
int main() {
    int checks = 0, failures = 0;
    auto check = [&](bool ok, const char* label) {
        ++checks;
        if (!ok) { ++failures; std::printf("FAIL %s\n", label); }
    };
    for (int dpi : {96, 144, 192}) for (int screenY : {-1200, 0, 300})
    for (bool useLaunchBand : {false, true}) {
        launchBand = useLaunchBand;
        auto D = [dpi](int value) { return MulDiv(value, dpi, 96); };
        int height = D(64), ch = D(1000);
        // A sidebar host can span the entire bar instead of being a Start button.
        startRect = {2000, screenY, 2000 + D(64), screenY + ch};
        for (int offset : {12, 100, 240})
            check(Place(false, offset, dpi, height, screenY) == D(offset), "full-height host preserves offset");
        // Real compact Start button still reserves its height.
        startRect.bottom = screenY + D(48);
        for (int offset : {-20, 12, 100})
            check(Place(false, offset, dpi, height, screenY) == D(48) + D(offset), "compact Start avoidance");
        check(Place(false, -500, dpi, height, screenY) == 0, "negative offset clamps at top");
        check(Place(false, 2000, dpi, height, screenY) == ch - height, "large offset clamps at bottom");
        notifyRect = {2000, screenY + D(900), 2000 + D(64), screenY + ch};
        for (int offset : {-20, 12, 100})
            check(Place(true, offset, dpi, height, screenY) == D(900) - height - D(offset), "tray offset");
        notifyRect = {};
        check(Place(true, 100, dpi, height, screenY) == ch - height - D(100) - D(80), "missing tray fallback");
        notifyRect = {2000, screenY, 2000 + D(64), screenY + ch};
        check(Place(true, 100, dpi, height, screenY) == ch - height - D(100) - D(80), "full-height tray fallback");
    }
    std::printf("%d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
'''
with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / 'vertical-offset.cpp'
    exe = cpp.with_suffix('.exe')
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23',
                    '-static', str(cpp), '-luser32', '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
