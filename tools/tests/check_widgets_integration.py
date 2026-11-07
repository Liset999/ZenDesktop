"""Validate the unified mod and its actual role-dispatch lifecycle.

No installation, registry changes, process launching or desktop interaction.
"""
from pathlib import Path
import argparse
import hashlib
import importlib.util
import json
import re
import subprocess
import tempfile
import yaml

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', required=True, type=Path)
options = parser.parse_args()
source_path = ROOT / 'optional/local@zen-desktop-widgets.wh.cpp'
source = source_path.read_text(encoding='utf-8')
assert '// @version         3.0.5' in source
assert set(re.findall(r'^// @architecture\s+([^\r\n]+)',source,re.M)) == {'x86','x86-64'}
assert '// @include         explorer.exe' in source and '// @include         windhawk.exe' in source
assert 'ZabCapture' not in source and '#include "' not in source
settings = yaml.safe_load(source.split('// ==WindhawkModSettings==')[1].split('/*')[1].split('*/')[0])
assert next(iter(settings[-1])) == 'clockAdvanced'
audio = next(item['audio'] for item in settings if 'audio' in item)
assert audio[0]['enabled'] is False
assert [next(iter(item)) for item in audio] == ['enabled','placement','taskbarOffset','shape','barHeight','appearance','advanced']
# Taskbar left/right offset reaches the pinned engine through the settings bridge.
assert '{L"position.taskbarOffsetX", L"0", L"audio.taskbarOffset"}' in source
assert 'g_settings.taskbarOffsetX = ReadSizeSetting(L"position.taskbarOffsetX", -4000.f, 4000.f, 0.f);' in source
assert 'blockX = std::clamp(blockX + g_settings.taskbarOffsetX, waLeft,' in source
assert 'if (g_settings.hostTaskbar && !g_overlayInPanel && g_settings.taskbarOffsetX != 0.0f)' in source
def check_localized(fields):
    for field in fields:
        assert '$name:zh-CN' in field
        for option in field.get('$options', []):
            assert any('\u4e00' <= ch <= '\u9fff' for ch in str(next(iter(option.values()))))
        for key,value in field.items():
            if not key.startswith('$') and isinstance(value,list): check_localized(value)
check_localized(audio)
appearance = next(field['appearance'] for field in audio if 'appearance' in field)
for name in ['primaryColor','secondaryColor']:
    field = next(field for field in appearance if name in field)
    palette = {key:label for option in field['$options'] for key,label in option.items()}
    assert field[name] in palette and len(palette) == 11
    assert all(re.fullmatch(r'#[0-9A-F]{8}', key) for key in palette)
    assert all('#' not in label for label in palette.values())
spec = importlib.util.spec_from_file_location('widget_builder', ROOT / 'tools/build-desktop-widgets.py')
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)
assert builder.flatten(settings) == json.loads((ROOT / 'tools/desktop-widget-defaults.json').read_text(encoding='utf-8'))
before = hashlib.sha256(source_path.read_bytes()).hexdigest()
builder.generate()
assert hashlib.sha256(source_path.read_bytes()).hexdigest() == before
print('PASS: one standalone mod, upstream pin, Chinese settings, defaults, reproducible generation')

test = r'''
#include <windows.h>
#include <cwchar>
#include <cstdio>
#include <atomic>
#define WH_MOD_ID L"zen-desktop-widgets-unit-test"
void Wh_Log(PCWSTR,...) {}
std::atomic<bool> testEnabled{false}; bool testLauncher=true, testUpstreamInit=true;
PCWSTR testPath=L"C:\\Windows\\explorer.exe";
int shellInit=0,shellAfter=0,shellChange=0,shellStop=0,audioInit=0,audioChange=0,audioStop=0;
std::atomic<int> audioStart{0};
DWORD WINAPI TestModule(HMODULE,LPWSTR out,DWORD size){wcscpy_s(out,size,testPath);return (DWORD)wcslen(out);}
#define GetModuleFileNameW TestModule
BOOL ZcModInit(){++shellInit;return TRUE;}
void ZcModAfterInit(){++shellAfter;}
void ZcModSettingsChanged(){++shellChange;}
void ZcModUninit(){++shellStop;}
bool ZwAudioWanted(){return testEnabled;}
namespace ZenAudio {
 bool g_isToolModProcessLauncher=false;
 BOOL Wh_ModInit(){++audioInit;g_isToolModProcessLauncher=testLauncher;return testUpstreamInit;}
 void Wh_ModAfterInit(){++audioStart;}
 void Wh_ModSettingsChanged(){++audioChange;}
 void Wh_ModUninit(){if(!g_isToolModProcessLauncher)++audioStop;}
}
'''
test += (ROOT / 'dev/widgets/lifecycle.cpp').read_text(encoding='utf-8')
test += r'''
int main(){
 int failures=0;
#ifdef _WIN64
 const int expectedShell=1;
 testEnabled=true;Wh_ModInit();Wh_ModAfterInit();Wh_ModSettingsChanged();Wh_ModUninit();
 if(shellInit!=1||shellAfter!=1||shellChange!=1||shellStop!=1||audioInit!=0||audioStart!=0)++failures;
#else
 const int expectedShell=0;
 if(Wh_ModInit()!=FALSE||shellInit!=0)++failures;
#endif
 testPath=L"E:\\Tools\\Windhawk\\windhawk.exe";testEnabled=false;testLauncher=true;
 Wh_ModInit();Wh_ModAfterInit();if(audioStart!=0)++failures;
 testEnabled=true;Wh_ModSettingsChanged();for(int retry=0;retry<100&&audioStart<1;retry++)Sleep(10);if(audioStart!=1)++failures;
 testEnabled=false;Wh_ModSettingsChanged();if(audioStart!=1)++failures;
 printf("launcher diagnostics starts=%d thread=%d lastError=%lu\n",(int)audioStart,g_zwAudioLauncherThread!=nullptr,GetLastError());
 Wh_ModUninit();if(g_zwAudioLauncherThread)++failures;
 testLauncher=false;testEnabled=true;Wh_ModInit();Wh_ModAfterInit();Wh_ModSettingsChanged();
 if(audioChange!=1||shellInit!=expectedShell)++failures;
 testEnabled=false;Wh_ModSettingsChanged();if(audioStop!=1)++failures;
 testUpstreamInit=false;if(Wh_ModInit()!=FALSE)++failures;
 printf("shell/tool routing, disabled startup, enable/disable, rejected host: failures=%d\n",failures);
 return failures?1:0;
}
'''
for architecture in ['i686-w64-mingw32','x86_64-w64-mingw32']:
    with tempfile.TemporaryDirectory(prefix='zen-widget-roles-') as directory:
        exe = Path(directory) / 'roles.exe'
        command = [str(options.compiler_dir / 'bin/clang++.exe'), '-x', 'c++', '-', '-std=c++23',
                   '-target', architecture, '-static', '-DUNICODE', '-D_UNICODE', '-o', str(exe)]
        result = subprocess.run(command, input=test.encode(), capture_output=True)
        if result.returncode:
            raise SystemExit(result.stderr.decode(errors='replace'))
        result = subprocess.run([str(exe)], capture_output=True, timeout=10)
        print(architecture, result.stdout.decode(errors='replace'))
        if result.returncode: raise SystemExit(result.returncode)
