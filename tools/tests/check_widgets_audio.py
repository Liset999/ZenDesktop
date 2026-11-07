from pathlib import Path
import subprocess,re,argparse,tempfile,os,shutil
parser=argparse.ArgumentParser(description='Compile and run the actual widget DSP/painting code against quiet audio.')
parser.add_argument('--compiler-dir',required=True,type=Path,help='Windhawk Compiler directory')
parser.add_argument('--architecture',choices=['x86','x64','both'],default='both')
options=parser.parse_args()
root=Path(__file__).resolve().parents[2];src=(root/'optional/local@zen-desktop-widgets.wh.cpp').read_text(encoding='utf-8')
src=src.replace('namespace ZenAudio {',r'''
bool testDrag=false;
PCWSTR testHost=L"taskbar";
int TestWhInt(PCWSTR key){
 if(!wcscmp(key,L"audio.appearance.bars"))return 24;
 if(!wcscmp(key,L"audio.advanced.fps"))return 60;
 if(!wcscmp(key,L"audio.advanced.monitor"))return 1;
 if(!wcscmp(key,L"dragMode"))return testDrag;
 if(!wcscmp(key,L"audio.background"))return 1; // Old saved background must not leak through.
 return 0;
}
PCWSTR TestWhString(PCWSTR key){
 if(!wcscmp(key,L"audio.placement"))return testHost;
 if(!wcscmp(key,L"audio.appearance.primaryColor"))return L"#FF1ED760";
 return L"";
}
namespace ZenAudio {
''',1).replace('::Wh_GetIntSetting(', '::TestWhInt(').replace('::Wh_GetStringSetting(', '::TestWhString(')
main=r'''#include <cstdio>
int main(){
 int failures=0;
 // Hidden, temporary windows only; never installed or shown on the desktop.
 testHost=L"panel";
 for(bool enabled:{false,true}){
   testDrag=enabled;
   if(ZenAudio::ZenAudioReadInt(L"interaction.dragEnabled")!=enabled)++failures;
 }
 testHost=L"desktop";
 if(!ZenAudio::ZenAudioReadInt(L"interaction.dragEnabled"))++failures;
 testHost=L"taskbar";
 if(ZenAudio::ZenAudioReadInt(L"interaction.dragEnabled"))++failures;
 testDrag=false;
 HWND window=CreateWindowExW(0,L"STATIC",L"",WS_POPUP,100,100,400,100,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
 if(!window) return 2;
 ZenAudio::g_overlayWnd=window;ZenAudio::g_overlayInPanel=true;
 ZenAudio::g_settings.dragEnabled=false;
 ZenAudio::VizPanelBackdrop();
 LONG_PTR ex=GetWindowLongPtrW(window,GWL_EXSTYLE);
 if(!(ex&WS_EX_TRANSPARENT)||!(ex&WS_EX_NOACTIVATE)||!(ex&WS_EX_LAYERED)||(ex&WS_EX_TOPMOST))++failures;
 if(GetWindowLongPtrW(window,GWL_STYLE)&(WS_CAPTION|WS_THICKFRAME))++failures;
 if(ZenAudio::VizPanelHitTest(window,MAKELPARAM(300,150))!=HTTRANSPARENT)++failures;
 if(ZenAudio::OverlayWndProc(window,WM_SYSCOMMAND,SC_MOVE,0)!=0)++failures;
 ZenAudio::g_settings.dragEnabled=true;
 ZenAudio::VizPanelBackdrop();
 if(GetWindowLongPtrW(window,GWL_EXSTYLE)&WS_EX_TRANSPARENT)++failures;
 if(ZenAudio::VizPanelHitTest(window,MAKELPARAM(300,150))!=HTCAPTION)++failures;
 if(ZenAudio::VizPanelHitTest(window,MAKELPARAM(101,101))!=HTTOPLEFT)++failures;
 ZenAudio::g_settings.dragEnabled=false;
 ZenAudio::VizPanelBackdrop();
 if(!(GetWindowLongPtrW(window,GWL_EXSTYLE)&WS_EX_TRANSPARENT))++failures;
 if(IsWindowVisible(window))++failures;
 DestroyWindow(window);ZenAudio::g_overlayWnd=nullptr;ZenAudio::g_overlayInPanel=false;
 printf("transparent host + shared drag gate + taskbar isolation: failures=%d\n",failures);
 if(ZenAudio::ZenAudioReadInt(L"appearance.barCount")!=24)++failures;
 if(ZenAudio::ZenAudioReadInt(L"performance.targetFps")!=60)++failures;
 if(ZenAudio::ZenAudioReadInt(L"background.enabled")!=0)++failures;
 if(ZenAudio::ZenAudioReadString(L"appearance.gradientColor1")!=L"#FF1ED760")++failures;
 if(ZenAudio::ZenAudioReadString(L"position.host")!=L"taskbar")++failures;
 if(ZenAudio::ZenAudioReadString(L"interaction.contextMenu")!=L"off")++failures;
 if(ZenAudio::ZenAudioReadString(L"analysis.engine")!=L"precision")++failures;
 if(ZenAudio::ZenAudioReadString(L"hardware.renderer")!=L"d3d11")++failures;
 constexpr int n=1024,bin=32;std::vector<float> re(n),im(n,0.f);
 for(int i=0;i<n;i++)re[i]=0.01f*sinf(6.283185307f*bin*i/n);
 ZenAudio::BuildTwiddleFactors(n);ZenAudio::VizFFT(re,im);int peak=1;float mag=0;
 for(int i=1;i<n/2;i++){float m=hypotf(re[i],im[i]);if(m>mag){mag=m;peak=i;}}
 if(peak!=bin||mag<1.f)++failures;
 printf("upstream FFT low-volume sine: bin=%d magnitude=%.4f\n",peak,mag);
 ZenAudio::ttdsp::RealFft precision;
 if(!precision.Init(n))++failures;
 std::vector<float> input(n),pr(n/2+1),pi(n/2+1);
 for(int i=0;i<n;i++)input[i]=0.01f*sinf(6.283185307f*bin*i/n);
 precision.Forward(input.data(),pr.data(),pi.data());int precisionPeak=1;float precisionMag=0;
 for(int i=1;i<n/2;i++){float m=hypotf(pr[i],pi[i]);if(m>precisionMag){precisionMag=m;precisionPeak=i;}}
 if(precisionPeak!=bin||fabsf(precisionMag-5.12f)>0.01f)++failures;
 printf("precision FFT: bin=%d magnitude=%.4f\n",precisionPeak,precisionMag);
 std::fill(re.begin(),re.end(),0.f);std::fill(im.begin(),im.end(),0.f);ZenAudio::VizFFT(re,im);
 for(int i=0;i<n;i++)if(fabsf(re[i])+fabsf(im[i])>1e-6f){++failures;break;}
 printf("settings bridge + hidden defaults + silence: failures=%d\n",failures);
 return failures?1:0;
}
'''
for architecture in (['x86','x64'] if options.architecture=='both' else [options.architecture]):
    args=[str(options.compiler_dir/'bin/clang++.exe'),'-x','c++','-','-std=c++23','-target',('i686-w64-mingw32' if architecture=='x86' else 'x86_64-w64-mingw32'),'-static','-DUNICODE','-D_UNICODE','-DWINVER=0x0A00','-D_WIN32_WINNT=0x0A00','-DWH_MOD','-DWH_EDITING','-DWH_MOD_ID=L"local@zen-desktop-widgets"','-include','windhawk_api.h']
    if architecture=='x86': args.remove('-static')
    libs=re.search(r'// @compilerOptions (.+)',src)[1].split()
    build_dir=tempfile.TemporaryDirectory(prefix='zen-widget-audio-')
    exe=Path(build_dir.name)/'quiet-audio-test.exe'
    r=subprocess.run(args+libs+['-o',str(exe)],input=(src+main).encode(),capture_output=True)
    print(architecture,'build:',r.returncode);print(r.stderr.decode(errors='replace')[:1200])
    if r.returncode: raise SystemExit(r.returncode)
    runtime_env=os.environ.copy()
    runtime_env['PATH']=str(options.compiler_dir/('i686-w64-mingw32' if architecture=='x86' else 'x86_64-w64-mingw32')/'bin')+os.pathsep+runtime_env.get('PATH','')
    if architecture=='x86':
        # Windhawk import libraries use .whl names for the bundled runtimes.
        # Stage exact runtime bytes beside this temporary test executable.
        for library in ['libc++.dll','libunwind.dll','libwinpthread-1.dll']:
            runtime=options.compiler_dir/'i686-w64-mingw32/bin'/library
            if runtime.exists():
                shutil.copy2(runtime,exe.parent/runtime.name)
                shutil.copy2(runtime,exe.parent/(runtime.stem+'.whl'))
    r=subprocess.run([str(exe)],capture_output=True,env=runtime_env);print(r.stdout.decode(errors='replace'));print('regression exit:',r.returncode)
    if r.returncode: raise SystemExit(r.returncode)
