from pathlib import Path
import argparse, subprocess, tempfile
p=argparse.ArgumentParser();p.add_argument('--compiler-dir',required=True,type=Path);p.add_argument('--source',type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[2]
src=(a.source or root/'optional/local@windows-animations.wh.cpp').read_text(encoding='utf-8-sig')
function=src[src.index('POINT GetTaskbarButtonPositionAsync('):src.index('\nenum class SwsSelectionIntent')]
function=function.split('static void PrefetchTaskbarDockPosition(')[0]
types=src[src.index('struct TaskbarDockWindowIdentity {'):src.index('struct ProcessNameCacheValue {')]
pending=src[src.index('struct UiaPending {'):src.index('static std::wstring MakeProcessDockKey')]
code=r'''
#include <windows.h>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <cstdint>
#include <cstdio>
TYPES
PENDING
namespace AnimConstants { constexpr int UiaLookupWaitMs=400,UiaLookupInFlightMs=3000,UiaNegativeCacheMs=10000,UiaColdNegativeCacheMs=500; }
std::mutex g_StateMutex;
std::unordered_map<HWND,TaskbarDockCacheValue> g_TaskbarDockPositions,g_TaskbarDockFallbackPositions;
std::unordered_map<HWND,TaskbarDockWindowIdentity> g_TaskbarDockLookupIdentities;
std::unordered_map<HWND,DWORD> g_TaskbarDockLookupStartedTicks,g_TaskbarDockNegativeUntilTicks,g_TaskbarDockPositiveUntilTicks;
std::unordered_map<HWND,uint64_t> g_TaskbarDockLookupGenerations;
std::unordered_map<std::wstring,POINT> g_ProcessDockPositions;
uint64_t g_NextTaskbarDockLookupGeneration=0;LONG g_seenTaskbarLayoutEpoch=1;
std::vector<std::thread> workers;int jobs=0;bool failStart=false;
bool SyncTaskbarLayoutEpoch(){return true;}void RefreshTaskbarWindowSetSignature(){}
LONG GetSharedTaskbarLayoutEpoch(){return 1;}
TaskbarDockWindowIdentity CaptureTaskbarDockWindowIdentity(HWND){return {1,1,(HMONITOR)1,1};}
bool TaskbarDockIdentityMatches(const TaskbarDockWindowIdentity&a,const TaskbarDockWindowIdentity&b){return a.windowToken==b.windowToken;}
bool IsTaskbarDockCacheValueUsable(const TaskbarDockCacheValue&v,const TaskbarDockWindowIdentity&i,HMONITOR m,TaskbarEdge e,DWORD){return v.identity.windowToken==i.windowToken&&v.identity.monitor==m&&v.edge==e;}
std::wstring GetProcessNameCached(HWND){return L"test";}
std::wstring MakeProcessDockKey(const std::wstring&n,HMONITOR,TaskbarEdge){return n;}
std::wstring ProcHintForUia(const std::wstring&n){return n;}
DWORD WaitForHandleWithSentMessagePump(HANDLE h,DWORD ms){return WaitForSingleObject(h,ms);}
DWORD WINAPI UiaWorkerThread(LPVOID arg){
 auto*t=(UiaTask*)arg;Sleep(450);
 {std::lock_guard<std::mutex> lock(g_StateMutex);
  g_TaskbarDockPositions[t->hWndApp]={{123,456},t->taskbarEdge,t->identity,GetTickCount()};
  g_TaskbarDockLookupStartedTicks.erase(t->hWndApp);
 }
 t->pending->targetPosition={123,456};t->pending->found=true;SetEvent(t->pending->done);t->pending->Release();delete t;return 0;
}
bool StartWorkerThread(LPTHREAD_START_ROUTINE fn,LPVOID data){if(failStart)return false;++jobs;workers.emplace_back([=]{fn(data);});return true;}
FUNCTION
int main(){
 HWND hwnd=(HWND)1;HMONITOR mon=(HMONITOR)1;POINT fallback={7,8};int failures=0;
 DWORD start=GetTickCount();auto first=GetTaskbarButtonPositionAsync(hwnd,L"test",fallback,mon,TaskbarEdge::Bottom);DWORD elapsed=GetTickCount()-start;
 printf("cold slow provider: caller_ms=%lu jobs=%d returned_fallback=%d\n",elapsed,jobs,first.x==7);
 if(elapsed>=80||first.x!=7||jobs!=1)++failures;
 auto inFlight=GetTaskbarButtonPositionAsync(hwnd,L"test",fallback,mon,TaskbarEdge::Bottom);
 if(jobs!=1||inFlight.x!=7)++failures;
 for(auto&t:workers)t.join();
 auto cached=GetTaskbarButtonPositionAsync(hwnd,L"test",fallback,mon,TaskbarEdge::Bottom);
 printf("completed lookup: cached=(%ld,%ld) jobs=%d\n",cached.x,cached.y,jobs);
 if(cached.x!=123||cached.y!=456||jobs!=1)++failures;
 failStart=true;auto failed=GetTaskbarButtonPositionAsync((HWND)2,L"test",fallback,mon,TaskbarEdge::Bottom);
 if(failed.x!=7||jobs!=1)++failures;
 printf("failures=%d\n",failures);return failures?1:0;
}
'''.replace('TYPES',types).replace('PENDING',pending).replace('FUNCTION',function)
with tempfile.TemporaryDirectory() as d:
 f=Path(d)/'test.cpp';exe=Path(d)/'test.exe';f.write_text(code,encoding='utf-8')
 result=subprocess.run([str(a.compiler_dir/'bin/clang++.exe'),'-std=c++23','-static',str(f),'-o',str(exe)],capture_output=True)
 if result.returncode:print(result.stderr.decode(errors='replace'));raise SystemExit(result.returncode)
 result=subprocess.run([str(exe)],capture_output=True);print(result.stdout.decode(errors='replace'));raise SystemExit(result.returncode)
