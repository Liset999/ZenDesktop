"""Check Steam query exclusion using the mod's actual process matcher."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', type=Path, required=True)
parser.add_argument('--source', type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = args.source or root / 'local@zen-ime-caret-hint.wh.cpp'
text = source.read_text(encoding='utf-8-sig')
assert 'bool IsSteamWindow(HWND hwnd)' in text, 'Steam process guard missing'
matcher = text[text.index('bool IsSteamWindow('):text.index('bool QueryAccessibleCaret(')]
poll = text[text.index('void PollOnce()'):text.index('LRESULT CALLBACK OverlayWndProc(')]
assert poll.index('IsSteamWindow(foreground)') < poll.index('DetectLangHint(')
assert poll.index('IsSteamWindow(foreground)') < poll.index('RequestUiaCaret(')
assert 'CancelCaretQuery();' in poll
msaa = text[text.index('bool QueryAccessibleCaret('):text.index('BOOL CALLBACK FindChromiumCaret(')]
assert msaa.index('IsSteamWindow(hwnd)') < msaa.index('AccessibleObjectFromWindow(')
uia = text[text.index('std::optional<RECT> QueryUiaCaret('):text.index('bool RequestUiaCaret(')]
assert uia.index('IsSteamWindow(GetForegroundWindow())') < uia.index('GetFocusedElement(')
code = r'''
#include <windows.h>
#include <cwchar>
#include <cstdio>
static const wchar_t* image;
static bool canOpen=true, canQuery=true;
static int closed=0;
DWORD FakeThread(HWND hwnd,DWORD* id){*id=hwnd?42:0;return hwnd?1:0;}
HANDLE FakeOpen(DWORD,BOOL,DWORD){return canOpen?reinterpret_cast<HANDLE>(1):nullptr;}
BOOL FakeQuery(HANDLE,DWORD,wchar_t* buffer,DWORD* length){
 if(!canQuery)return FALSE;
 wcscpy(buffer,image);*length=wcslen(image);return TRUE;
}
BOOL FakeClose(HANDLE){++closed;return TRUE;}
#define GetWindowThreadProcessId FakeThread
#define OpenProcess FakeOpen
#define QueryFullProcessImageNameW FakeQuery
#define CloseHandle FakeClose
MATCHER
int main(){
 HWND hwnd=reinterpret_cast<HWND>(1);
 const wchar_t* names[]={L"E:\\Game\\steam.exe",L"D:\\Steam\\STEAMWEBHELPER.EXE",
 L"C:\\Apps\\chrome.exe",L"C:\\Apps\\mysteam.exe",L"C:\\steam.exe\\other.exe"};
 int failures=0;
 for(int i=0;i<5;++i){image=names[i];failures+=IsSteamWindow(hwnd)!=(i<2);}
 failures+=closed!=5;
 failures+=IsSteamWindow(nullptr);
 canOpen=false;failures+=IsSteamWindow(hwnd);canOpen=true;
 canQuery=false;failures+=IsSteamWindow(hwnd);failures+=closed!=6;
 printf("Steam matcher and handle cleanup failures=%d\n",failures);
 return failures?1:0;
}
'''.replace('MATCHER', matcher)
compiler = str(args.compiler_dir / 'bin/clang++.exe')
with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / 'guard.cpp'
    exe = Path(directory) / 'guard.exe'
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([compiler, '-std=c++23', '-static', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
    command = [compiler, '-x', 'c++', '-', '-std=c++23', '-target', 'x86_64-w64-mingw32',
               '-DUNICODE', '-D_UNICODE', '-DWINVER=0x0A00', '-D_WIN32_WINNT=0x0A00',
               '-DWH_MOD', '-DWH_MOD_ID=L"local@zen-ime-caret-hint"',
               '-include', 'windhawk_api.h', '-fsyntax-only']
    subprocess.run(command, input=source.read_bytes(), cwd=directory, check=True)
print('Steam exclusion ordering and full stdin syntax passed')
