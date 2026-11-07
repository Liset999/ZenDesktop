from pathlib import Path
import subprocess, tempfile, argparse
p=argparse.ArgumentParser();p.add_argument('--compiler-dir',required=True,type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[2]
src=(root/'optional/local@windows-animations.wh.cpp').read_text(encoding='utf-8-sig')
start=src.index('    void ShowGhostNoActivate() {');end=src.index('    bool CreateGhostWindow(',start)
method=src[start:end]
code=r'''
#include <windows.h>
#include <cstdio>
static HWND tray;
static auto SetWindowPos_Original = SetWindowPos;
static auto ShowWindow_Original = ShowWindow;
static HWND FindTaskbarForMonitor(HMONITOR) { return tray; }
struct Data { HMONITOR hMon{}; };
struct Animation {
    bool keepGhostBelowTaskbar = true;
    HWND hGhost{};
    Data storage;
    Data* data = &storage;
METHOD
};
static bool Above(HWND upper, HWND lower) {
    for (HWND h = GetTopWindow(nullptr); h; h = GetWindow(h, GW_HWNDNEXT)) {
        if (h == upper) return true;
        if (h == lower) return false;
    }
    return false;
}
int main() {
    // All native windows live on a separate desktop; no user windows/focus touched.
    HDESK desk = CreateDesktopW(L"ZenAnimationZOrderTest", nullptr, nullptr, 0, GENERIC_ALL, nullptr);
    if (!desk || !SetThreadDesktop(desk)) { printf("Desktop setup failed %lu\n", GetLastError()); return 2; }
    int failures = 0;
    for (bool topmost : {false, true}) {
        tray = CreateWindowExW(topmost ? WS_EX_TOPMOST : 0, L"STATIC", L"tray", WS_POPUP, 0,0,100,20,nullptr,nullptr,nullptr,nullptr);
        HWND cover = CreateWindowExW(0, L"STATIC", L"cover", WS_POPUP, 0,0,100,100,nullptr,nullptr,nullptr,nullptr);
        HWND ghost = CreateWindowExW(WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW, L"STATIC", L"ghost", WS_POPUP, 0,0,100,100,nullptr,nullptr,nullptr,nullptr);
        if (!tray || !cover || !ghost) return 3;
        ShowWindow(tray, SW_SHOWNOACTIVATE);ShowWindow(cover, SW_SHOWNOACTIVATE);
        SetWindowPos(cover, HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        Animation animation;animation.hGhost=ghost;animation.ShowGhostNoActivate();
        bool aboveCover=Above(ghost,cover);
        bool belowTray=!topmost || Above(tray,ghost);
        bool staysTopmost=(GetWindowLongPtrW(ghost,GWL_EXSTYLE)&WS_EX_TOPMOST)!=0;
        printf("taskbarTopmost=%d ghostAboveCover=%d belowTopmostTaskbar=%d ghostTopmost=%d\n",topmost,aboveCover,belowTray,staysTopmost);
        if (!aboveCover || !belowTray || !staysTopmost) ++failures;
        DestroyWindow(ghost);DestroyWindow(cover);DestroyWindow(tray);
    }
    return failures ? 1 : 0;
}
'''.replace('METHOD',method).replace('#include <cstdio>','#include <cstdio>\n#include <initializer_list>')
with tempfile.TemporaryDirectory() as d:
    f=Path(d)/'zorder.cpp';out=Path(d)/'zorder.exe';f.write_text(code,encoding='utf-8')
    result=subprocess.run([str(a.compiler_dir/'bin/clang++.exe'),'-std=c++23','-static',str(f),'-luser32','-o',str(out)],capture_output=True)
    if result.returncode: print(result.stderr.decode(errors='replace'));raise SystemExit(result.returncode)
    result=subprocess.run([str(out)],capture_output=True)
    print(result.stdout.decode(errors='replace'));print(result.stderr.decode(errors='replace'))
    raise SystemExit(result.returncode)
