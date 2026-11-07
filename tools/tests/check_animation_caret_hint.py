import argparse, subprocess, tempfile
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--compiler-dir',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[2]
s=(root/'zen-ime-caret-hint-0.2.4.wh.cpp').read_text(encoding='utf-8-sig')
handler=s[s.index('LRESULT CALLBACK OverlayWndProc('):s.index('LRESULT CALLBACK HostWndProc(')]
code=r'''
#include <windows.h>
#include <optional>
#include <cstdio>
static HWND overlay;
static bool g_focusHintPending=true;
static std::optional<int> g_pendingShow=1;
static void HideBubble(){ShowWindow(overlay,SW_HIDE);}
HANDLER
int main(){
 auto desktop=CreateDesktopW(L"ZenCaretCaptureTest",nullptr,nullptr,0,GENERIC_ALL,nullptr);
 if(!desktop||!SetThreadDesktop(desktop))return 2;
 WNDCLASSW cls{};cls.lpfnWndProc=OverlayWndProc;cls.lpszClassName=L"ZenImeCaretHintOverlay";cls.hInstance=GetModuleHandleW(nullptr);
 if(!RegisterClassW(&cls))return 3;
 overlay=CreateWindowW(cls.lpszClassName,L"",WS_POPUP|WS_VISIBLE,10,10,80,30,nullptr,nullptr,cls.hInstance,nullptr);
 if(!overlay||!IsWindowVisible(overlay))return 4;
 DWORD_PTR reply=0;
 bool sent=SendMessageTimeoutW(overlay,RegisterWindowMessageW(L"ZenDesktop.DismissCaretHintForCapture.v1"),0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,50,&reply)!=0;
 bool passed=sent&&reply&&!IsWindowVisible(overlay)&&!g_pendingShow&&!g_focusHintPending;
 printf("dismiss acknowledged=%d hidden=%d pending cleared=%d\n",sent&&reply,!IsWindowVisible(overlay),!g_pendingShow&&!g_focusHintPending);
 DestroyWindow(overlay);return passed?0:1;
}
'''.replace('HANDLER',handler)
with tempfile.TemporaryDirectory() as d:
 f=Path(d)/'test.cpp';exe=Path(d)/'test.exe';f.write_text(code,encoding='utf-8')
 subprocess.run([str(a.compiler_dir/'bin/clang++.exe'),'-std=c++23','-static',str(f),'-o',str(exe)],check=True)
 raise SystemExit(subprocess.run([str(exe)]).returncode)
