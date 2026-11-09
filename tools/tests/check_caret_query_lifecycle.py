"""Exercise actual caret request/cancellation code with an isolated test DLL."""
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
assert 'JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE' in text, 'Query lifetime not isolated'
assert 'TryCaretFromElementRect' not in text, 'Input rectangle still masquerades as caret'
show = text[text.index('void ShowHint('):text.index('void CompletePendingShow(')]
assert 'QueryCaretScreenRect(' not in show, 'Host still calls MSAA synchronously'
assert 'RequestUiaCaret(focus, current, true)' in text, 'Automatic editor hint removed'
query = text[text.index('    const bool queryDue ='):text.index('    if (IsChineseLayoutForWindow(focus))', text.index('    const bool queryDue ='))]
assert 'g_lastLangQueryTick = GetTickCount64();' in query
assert 'if (lang)' not in query
structs = text[text.index('struct CaretPacket {'):text.index('std::atomic<ULONGLONG> g_logicalFocusTick')]
request = text[text.index('bool RequestUiaCaret('):text.index('bool IsChineseLayoutForWindow(')]
complete = text[text.index('void CompletePendingShow('):text.index('void HandleMouse(')]
poll = text[text.index('void PollOnce()'):text.index('LRESULT CALLBACK OverlayWndProc(')]
retry = poll[poll.index('    const ULONGLONG langRetryMs'):poll.index('    if (IsChineseLayoutForWindow(focus))')]
hooks = text[text.index('LRESULT CALLBACK MouseProc('):text.index('DWORD WINAPI InputThread(')]
assert 'DetectLangHint' not in hooks and 'QueryCaretScreenRect' not in hooks
assert 'g_inputThread = CreateThread' in text
packet = text[text.index('struct CaretPacket {'):text.index('struct PendingShow {')]
helper = r'''
#include <windows.h>
PACKET
extern "C" __declspec(dllexport) void CALLBACK ZenCaretQuery(HWND,HINSTANCE,LPSTR command,int){
 wchar_t name[160];MultiByteToWideChar(CP_ACP,0,command,-1,name,160);
 HANDLE mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,name);
 if(!mapping)return;
 auto* packet=static_cast<CaretPacket*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(CaretPacket)));
 if(packet){
  if(packet->focus==reinterpret_cast<HWND>(1))Sleep(INFINITE);
  packet->editor=packet->focus!=reinterpret_cast<HWND>(3);
  packet->hasRect=packet->focus==reinterpret_cast<HWND>(2);
  packet->rect={10,20,11,40};
  UnmapViewOfFile(packet);
 }
 CloseHandle(mapping);
}
'''.replace('PACKET', packet)
harness = r'''
#include <windows.h>
#include <string>
#include <optional>
#include <atomic>
#include <vector>
#include <cstdio>
static HWND foreground=reinterpret_cast<HWND>(10);
static HWND FakeForeground(){return foreground;}
#define GetForegroundWindow FakeForeground
static HINSTANCE g_hInstance;
static constexpr ULONGLONG kCaretExpiryMs=600;
static bool IsSteamWindow(HWND){return false;}
STRUCTS
REQUEST
static int caretShows=0,pointerShows=0;
static bool FocusStillCurrent(HWND){return true;}
static int GetWindowDpiSafe(HWND){return 96;}
static void ShowBubbleAt(const std::wstring&,RECT,int){++caretShows;}
static void ShowPointerBubble(const std::wstring&){++pointerShows;}
COMPLETE
enum class HintLang {English,Chinese};
static bool g_langQueryFailing=false,g_hasLastLang=false;
static ULONGLONG g_lastLangQueryTick=0;
static constexpr ULONGLONG kLangFailRetryMs=1000,kLangRefreshMs=200;
static std::optional<HintLang> g_staleImeLang;
static HintLang g_lastLang=HintLang::English;
static ULONGLONG fakeTick=2000;
static int languageQueries=0;
static std::optional<HintLang> DetectLangHint(HWND,const wchar_t**){++languageQueries;return {};}
#define GetTickCount64() fakeTick
static void PollLanguage(ULONGLONG now,bool shiftTap=false){
 fakeTick=now;HWND focus=nullptr;
 RETRY
}
#undef GetTickCount64
static int failures=0;
static void Check(bool ok,const char* name){printf("%s %s\n",ok?"PASS":"FAIL",name);failures+=!ok;}
static void Finish(HWND focus){
 for(int i=0;i<50&&g_pendingShow;++i){Sleep(20);CompletePendingShow(focus);}
}
int main(int argc,char** argv){
 for(ULONGLONG now=2000;now<4000;now+=50)PollLanguage(now);
 Check(languageQueries==2,"failed IME query limited to once per second without cache");
 g_hasLastLang=true;
 for(ULONGLONG now=4000;now<6000;now+=50)PollLanguage(now);
 Check(languageQueries==4,"failed IME query limited with cached state");
 PollLanguage(6000,true);
 Check(languageQueries==5,"Shift tap can immediately refresh state");
 g_hInstance=LoadLibraryA(argv[1]);if(!g_hInstance)return 2;
 HWND hung=reinterpret_cast<HWND>(1),editor=reinterpret_cast<HWND>(2);
 auto start=GetTickCount64();
 Check(RequestUiaCaret(hung,L"test"),"start hung provider");
 HANDLE child=nullptr;
 DuplicateHandle(GetCurrentProcess(),g_caretProcess,GetCurrentProcess(),&child,SYNCHRONIZE,FALSE,0);
 Finish(hung);
 Check(GetTickCount64()-start<1500&&!g_pendingShow&&pointerShows==1,"deadline and pointer fallback");
 Check(WaitForSingleObject(child,2000)==WAIT_OBJECT_0,"hung child terminated");CloseHandle(child);
 Check(RequestUiaCaret(editor,L"test",true),"retry after timeout");Finish(editor);
 Check(caretShows==1,"automatic editor reminder retains exact caret");
 HWND noCaret=reinterpret_cast<HWND>(4);
 RequestUiaCaret(noCaret,L"test",true);Finish(noCaret);
 Check(pointerShows==2,"confirmed editor without caret uses pointer");
 HWND button=reinterpret_cast<HWND>(3);
 RequestUiaCaret(button,L"test",true);Finish(button);
 Check(pointerShows==2&&caretShows==1,"non-editor click stays silent");
 RequestUiaCaret(editor,L"test",true);++g_inputGeneration;Finish(editor);
 Check(caretShows==1,"same HWND input change discards old result");
 RequestUiaCaret(editor,L"test",true);++g_focusGeneration;Finish(editor);
 Check(caretShows==1,"logical focus change discards old result");
 RequestUiaCaret(hung,L"test");start=GetTickCount64();CancelCaretQuery();
 Check(GetTickCount64()-start<200&&!g_caretProcess&&!g_caretMapping&&!g_caretJob,"shutdown cancels without provider wait");
 FreeLibrary(g_hInstance);return failures?1:0;
}
'''.replace('STRUCTS', structs).replace('REQUEST', request).replace('COMPLETE', complete).replace('RETRY', retry)
compiler = str(args.compiler_dir / 'bin/clang++.exe')
with tempfile.TemporaryDirectory() as directory:
    directory = Path(directory)
    dll = directory / 'query helper.dll'
    helper_path = directory / 'helper.cpp'
    helper_path.write_text(helper, encoding='utf-8')
    subprocess.run([compiler, '-target', 'x86_64-w64-mingw32', '-std=c++23', '-static', '-shared', str(helper_path), '-o', str(dll)], check=True)
    harness_path = directory / 'lifecycle.cpp'
    harness_path.write_text(harness, encoding='utf-8')
    exe = directory / 'lifecycle.exe'
    subprocess.run([compiler, '-target', 'x86_64-w64-mingw32', '-std=c++23', '-static', str(harness_path), '-o', str(exe)], check=True)
    subprocess.run([str(exe), str(dll)], check=True, timeout=40)
print('Query lifecycle regression checks passed')
