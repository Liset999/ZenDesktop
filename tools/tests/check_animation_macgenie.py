from pathlib import Path
import argparse, subprocess, tempfile
p=argparse.ArgumentParser();p.add_argument('--compiler-dir',required=True,type=Path);p.add_argument('--source',type=Path);p.add_argument('--benchmark-only',action='store_true');a=p.parse_args()
root=Path(__file__).resolve().parents[2];src=(a.source or root/'optional/local@windows-animations.wh.cpp').read_text(encoding='utf-8-sig')
def function(marker):
 start=src.index(marker);opening=src.index('{',start);depth=1;end=opening+1
 while depth:
  if src[end]=='{':depth+=1
  elif src[end]=='}':depth-=1
  end+=1
 return src[start:end]
helpers=src[src.index('struct MacGenieGeometry {'):src.index('class AnimationEngine')]
helpers=helpers.split('std::mutex g_d2dFactoryMutex;')[0]
# Only the lamp/math helpers preceding AnimationEngine are required.
helpers=helpers[:helpers.index('class AnimationEngine')] if 'class AnimationEngine' in helpers else helpers
methods='\n'.join(function(m) for m in ['    void MacGenieUvToCanonical(', '    void MacGenieCanonicalToScreen(', '    void ReleaseMacGenie()', '    bool PrecalcMacGenie()', '    void RenderMacGenie('])
code=r'''
#define NOMINMAX
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <vector>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdint>
using std::sinf;using std::cosf;
template<class T>T Clamp(T v,T lo,T hi){return std::max(lo,std::min(v,hi));}
enum class TaskbarEdge{Bottom,Top,Left,Right};
constexpr int kMinRestoreGenie=0;
static bool IsHorizontalTaskbar(TaskbarEdge e){return e==TaskbarEdge::Bottom||e==TaskbarEdge::Top;}
#define Wh_Log(...) ((void)0)
std::atomic<int> g_genieTileCount{35};
ID2D1Factory* factory=nullptr;static ID2D1Factory* AcquireD2DFactory(){return factory;}
HELPERS
struct Data{BOOL isRising=FALSE;HWND hRealWnd=nullptr;};
struct Renderer {
 Data storage;Data*data=&storage;
 int W=1920,H=1080,boundW=1920,boundH=1220,boundLeft=0,boundTop=0,srcStride=1920*4,canvasStride=1920*4,macTiles=35,minRestoreEffect=8;
 BYTE*srcBits=nullptr;BYTE*pBits=nullptr;HDC hCanvasDC=nullptr;
 TaskbarEdge taskbarEdge=TaskbarEdge::Bottom;
 MacGenieGeometry macWindowGeom{0,0,1920,1080},macIconGeom{920,1200,50,1};
 ID2D1Factory*macFactory=nullptr;ID2D1DCRenderTarget*macRenderTarget=nullptr;ID2D1Bitmap*macSnapshot=nullptr;ID2D1BitmapBrush*macBrush=nullptr;ID2D1PathGeometry*macOutline=nullptr;
 float macOutlineLeft=-1e9f,macOutlineRight=-1e9f,macOutlineTop=-1e9f,macOutlineBottom=-1e9f;
 std::vector<D2D1_POINT_2F>macGrid;std::vector<float>axisBounds;
 METHODS
};
int main(){
 if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&factory)))return 2;
 Renderer r;std::vector<BYTE> source(r.W*r.H*4);r.srcBits=source.data();
 for(int y=0;y<r.H;++y)for(int x=0;x<r.W;++x){auto*p=(DWORD*)(r.srcBits+y*r.srcStride+x*4);*p=0xFF000000|((x*255/r.W)<<16)|((y*255/r.H)<<8)|127;}
 r.hCanvasDC=CreateCompatibleDC(nullptr);BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=r.boundW;info.bmiHeader.biHeight=-r.boundH;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
 HBITMAP bitmap=CreateDIBSection(r.hCanvasDC,&info,DIB_RGB_COLORS,(void**)&r.pBits,nullptr,0);auto old=SelectObject(r.hCanvasDC,bitmap);
 if(!bitmap||!r.PrecalcMacGenie())return 3;
 LARGE_INTEGER frequency;QueryPerformanceFrequency(&frequency);std::vector<double>times;int blank=0;
 for(int i=0;i<24;++i){memset(r.pBits,0,r.boundW*r.boundH*4);LARGE_INTEGER start,end;QueryPerformanceCounter(&start);r.RenderMacGenie((i+1)/26.f);QueryPerformanceCounter(&end);times.push_back((end.QuadPart-start.QuadPart)*1000./frequency.QuadPart);int painted=0;for(int p=0;p<r.boundW*r.boundH;++p)if(((DWORD*)r.pBits)[p]>>24)++painted;if(!painted)++blank;}
 std::sort(times.begin(),times.end());printf("1920x1080 render only: median_ms=%.3f p95_ms=%.3f blank_frames=%d\n",times[times.size()/2],times[22],blank);
 int failed=blank;
 if(VERIFY){
  r.W=96;r.H=64;r.srcStride=96*4;
  for(int y=0;y<r.H;++y)for(int x=0;x<r.W;++x)((DWORD*)r.srcBits)[y*r.W+x]=(x<5&&y<5)?0:0xFF000000|((x*255/r.W)<<16)|((y*255/r.H)<<8)|127;
  for(auto edge:{TaskbarEdge::Bottom,TaskbarEdge::Top,TaskbarEdge::Left,TaskbarEdge::Right}){
   r.taskbarEdge=edge;
   if(edge==TaskbarEdge::Bottom){r.macWindowGeom={0,0,96,64};r.macIconGeom={40,110,16,1};}
   if(edge==TaskbarEdge::Top){r.macWindowGeom={0,-64,96,64};r.macIconGeom={40,40,16,1};}
   if(edge==TaskbarEdge::Right){r.macWindowGeom={0,0,64,96};r.macIconGeom={20,140,16,1};}
   if(edge==TaskbarEdge::Left){r.macWindowGeom={0,-96,64,96};r.macIconGeom={20,40,16,1};}
   memset(r.pBits,0,r.boundW*r.boundH*4);r.storage.isRising=FALSE;r.RenderMacGenie(0);
   int mismatch=0;
   for(int y=0;y<r.H;++y)for(int x=0;x<r.W;++x)for(int c=0;c<4;++c){int expected=r.srcBits[y*r.srcStride+x*4+c],actual=r.pBits[y*r.canvasStride+x*4+c];if(abs(actual-expected)>2)++mismatch;}
   memset(r.pBits,0,r.boundW*r.boundH*4);r.RenderMacGenie(0.5f);std::vector<BYTE>middle(r.pBits,r.pBits+r.boundW*r.boundH*4);
   memset(r.pBits,0,r.boundW*r.boundH*4);r.storage.isRising=TRUE;r.RenderMacGenie(0.5f);
   bool reversal=memcmp(r.pBits,middle.data(),middle.size())==0;
   printf("edge=%d identity_pixel_mismatch=%d reverse_matches=%d\n",(int)edge,mismatch,reversal);
   if(mismatch||!reversal)++failed;
  }
 }
 r.ReleaseMacGenie();SelectObject(r.hCanvasDC,old);DeleteObject(bitmap);DeleteDC(r.hCanvasDC);factory->Release();return failed?1:0;
}
'''.replace('HELPERS',helpers).replace('METHODS',methods).replace('VERIFY','false' if a.benchmark_only or 'const float axisSpan = lastY - firstY;' not in methods else 'true')
with tempfile.TemporaryDirectory() as d:
 f=Path(d)/'test.cpp';exe=Path(d)/'test.exe';f.write_text(code,encoding='utf-8')
 result=subprocess.run([str(a.compiler_dir/'bin/clang++.exe'),'-std=c++23','-O2','-static',str(f),'-ld2d1','-lgdi32','-luser32','-o',str(exe)],capture_output=True)
 if result.returncode:print(result.stderr.decode(errors='replace')[:4500]);raise SystemExit(result.returncode)
 result=subprocess.run([str(exe)],capture_output=True);print(result.stdout.decode(errors='replace'));print(result.stderr.decode(errors='replace'));raise SystemExit(result.returncode)
