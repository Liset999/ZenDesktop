from pathlib import Path
import argparse, subprocess, tempfile
p=argparse.ArgumentParser();p.add_argument('--compiler-dir',required=True,type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[2];src=(root/'optional/local@windows-animations.wh.cpp').read_text(encoding='utf-8-sig')
policy=src[src.index('    const bool excelCloseScreenOnlyCapture ='):src.index('    const bool allowShowDesktopSwitchGhostWait =')]
first=src.index('        bool capturedFromScreen = false;',src.index('    auto CaptureNow ='))
selection=src[first:src.index('        if (captured && capturedFromScreen) {',first)]
bitblt=src[src.index('static bool ShouldUseBitBlt('):src.index('static bool IsTransparentXamlExplorerHostIsland(')]
helper=''
if 'static bool RequiresComposedWindowSnapshot(' in src:
 start=src.index('static bool RequiresShellComposedWindowSnapshot(') if 'static bool RequiresShellComposedWindowSnapshot(' in src else src.index('static bool RequiresComposedWindowSnapshot(');end=src.index('static bool RequiresComposedWindowSnapshot(');helper=src[start:src.index('\n}',end)+2]
code=r'''
#include <windows.h>
#include <string>
#include <cstring>
#include <cstdio>
#define PW_RENDERFULLCONTENT 2
static bool explorer=false,backdrop=false,accent=false,settings=false,hostedSettings=false;
static bool screenAvailable=true;static int screenCalls=0,printCalls=0;
static HWND current=(HWND)1;
#define GetForegroundWindow FakeForeground
static HWND FakeForeground(){return current;}
static bool RequiresScreenCapture(HWND){return false;}
static bool IsExplorerFrameWindow(HWND){return explorer;}
static bool HasExplicitSystemBackdrop(HWND){return backdrop;}
static bool IsTranslucentWindowsAccentBlurActive(HWND){return accent;}
static std::wstring GetProcessNameCached(HWND h){return settings||(hostedSettings&&h==(HWND)2)?L"systemsettings":L"ordinary";}
static std::wstring GetClassNameStr(HWND){return hostedSettings?L"ApplicationFrameWindow":L"ordinary";}
#define FindWindowExW FakeFindWindowExW
static HWND FakeFindWindowExW(HWND,HWND,PCWSTR,PCWSTR){return hostedSettings?(HWND)2:nullptr;}
static bool RequiresExcelScreenOnlyCloseCapture(HWND,bool){return false;}
#define GdiFlush FakeGdiFlush
static void FakeGdiFlush(){}
#define PrintWindow FakePrintWindow
static BOOL FakePrintWindow(HWND,HDC,UINT){++printCalls;return TRUE;}
#define IsWindowVisible FakeIsWindowVisible
static BOOL FakeIsWindowVisible(HWND){return TRUE;}
#define IsIconic FakeIsIconic
static BOOL FakeIsIconic(HWND){return FALSE;}
BITBLT
HELPER
struct Result{bool captured,fromScreen;int screens,prints;};
static Result Capture(bool rising,bool isClosing=false){
 HWND hWnd=current;HDC tempDC=nullptr;DWORD pixel=0;void*tempBits=&pixel;int rawW=1,rawH=1;
 bool onTargetThread=true,allowScreenFallback=!rising;
 screenCalls=printCalls=0;
 auto CaptureScreen=[&]()->BOOL{++screenCalls;return screenAvailable;};
 POLICY
 SELECTION
 return {captured!=FALSE,capturedFromScreen,screenCalls,printCalls};
}
int main(){int failed=0;
 for(int app=0;app<5;++app){
  explorer=app==0;settings=app==1;backdrop=app==2;accent=app==3;hostedSettings=app==4;
  screenAvailable=true;auto min=Capture(false);
  printf("material=%d minimize_screen=%d prints=%d\n",app,min.fromScreen,min.prints);
  if(!min.captured||!min.fromScreen||min.prints)++failed;
  screenAvailable=false;auto blocked=Capture(false);
  const bool shell=explorer||settings||hostedSettings;
  if(shell ? (blocked.captured||blocked.prints) : (!blocked.captured||blocked.prints!=1))++failed;
  auto uncachedRestore=Capture(true);
  if(shell ? (uncachedRestore.captured||uncachedRestore.prints) : (!uncachedRestore.captured||uncachedRestore.prints!=1))++failed;
  if(uncachedRestore.screens)++failed;
  auto close=Capture(false,true);
  if(shell ? (close.captured||close.prints) : (!close.captured||close.prints!=1))++failed;
 }
 explorer=settings=backdrop=accent=hostedSettings=false;screenAvailable=true;
 auto ordinary=Capture(false);if(!ordinary.captured||ordinary.prints!=1)++failed;
 printf("failures=%d\n",failed);return failed?1:0;}
'''.replace('BITBLT',bitblt).replace('HELPER',helper).replace('POLICY',policy).replace('SELECTION',selection)
with tempfile.TemporaryDirectory() as d:
 f=Path(d)/'test.cpp';exe=Path(d)/'test.exe';f.write_text(code,encoding='utf-8')
 result=subprocess.run([str(a.compiler_dir/'bin/clang++.exe'),'-std=c++23','-static',str(f),'-o',str(exe)],capture_output=True)
 if result.returncode:print(result.stderr.decode(errors='replace'));raise SystemExit(result.returncode)
 result=subprocess.run([str(exe)],capture_output=True);print(result.stdout.decode(errors='replace'));raise SystemExit(result.returncode)
