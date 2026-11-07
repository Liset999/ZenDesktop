"""Exercise the source decoder with the getter bytes from local udwm.dll.

The disassembler stub supplies the corresponding instruction text; no DWM hooks.
"""
from pathlib import Path
import subprocess
import tempfile
import os
import shutil

root = Path(__file__).resolve().parents[2]
source = (root / 'optional/local@wobbly-windows.wh.cpp').read_text(encoding='utf-8')
start = source.index('static bool FindVisualProxyAccessPath(')
end = source.index('// Follow only the exact native function', start)
function = source[start:end]
harness = r'''
#include <windows.h>
#include <regex>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdint>
struct WH_DISASM_RESULT { size_t length; char text[128]; };
BYTE* begin; size_t codeSize;
bool IsDwmExecutableAddress(void* p) { return (BYTE*)p >= begin && (BYTE*)p < begin + codeSize; }
bool IsDwmFunctionPointerValid(void* p) { return p == begin; }
bool IsReadableMemory(const void* p, size_t n) { return (const BYTE*)p >= begin && (const BYTE*)p + n <= begin + codeSize; }
bool Wh_Disasm(void* p, WH_DISASM_RESULT* r) {
    BYTE* b=(BYTE*)p; const char* text=nullptr; size_t len=0;
    if (*b==0xc3) { text="ret"; len=1; }
    else if (*b==0xcc) { text="int3"; len=1; }
    else if (IsReadableMemory(b,2) && b[0]==0x75) { text="jnz 0x1000"; len=2; }
    else if (IsReadableMemory(b,2) && b[0]==0x74) { text="jz 0x1000"; len=2; }
    else if (IsReadableMemory(b,3) && !memcmp(b,"\x48\x85\xc0",3)) { text="test rax, rax"; len=3; }
    else if (IsReadableMemory(b,7) && !memcmp(b,"\x48\x8b\x81",3)) { text="mov rax, qword ptr [rcx+0xf8]"; len=7; }
    else if (IsReadableMemory(b,4) && !memcmp(b,"\x48\x8b\x40",3)) { text="mov rax, qword ptr [rax+0x10]"; len=4; }
    else return false;
    r->length=len; strcpy(r->text,text); return true;
}
'''
tests = r'''
int main() {
    BYTE guarded[]={0x48,0x8b,0x81,0xf8,0,0,0,0x48,0x85,0xc0,0x75,2,0xc3,0xcc,0x48,0x8b,0x40,0x10,0xc3};
    BYTE direct[]={0x48,0x8b,0x81,0xf8,0,0,0,0x48,0x8b,0x40,0x10,0xc3};
    // PDB-resolved getter in user-supplied uDWM.dll 10.0.26100.9711, RVA 0x85d30.
    BYTE tailGuarded[]={0x48,0x8b,0x81,0xf8,0,0,0,0x48,0x85,0xc0,0x74,6,0x48,0x8b,0x40,0x10,0xc3,0xcc,0xc3};
    size_t owner,proxy;
    begin=guarded; codeSize=sizeof guarded;
    if (!FindVisualProxyAccessPath(begin,&owner,&proxy) || owner!=0xf8 || proxy!=0x10) {
        puts("FAIL: null-guarded getter rejected"); return 1;
    }
    begin=direct; codeSize=sizeof direct;
    if (!FindVisualProxyAccessPath(begin,&owner,&proxy) || owner!=0xf8 || proxy!=0x10) return 2;
    begin=tailGuarded; codeSize=sizeof tailGuarded;
    if (!FindVisualProxyAccessPath(begin,&owner,&proxy) || owner!=0xf8 || proxy!=0x10) {
        puts("FAIL: user 26100.9711 tail-null-return getter rejected"); return 7;
    }
    tailGuarded[11]=5;
    if (FindVisualProxyAccessPath(begin,&owner,&proxy)) return 8;
    tailGuarded[11]=6; tailGuarded[18]=0xcc;
    if (FindVisualProxyAccessPath(begin,&owner,&proxy)) return 9;
    tailGuarded[18]=0xc3; codeSize=sizeof tailGuarded-1;
    if (FindVisualProxyAccessPath(begin,&owner,&proxy)) return 10;
    codeSize=sizeof tailGuarded; tailGuarded[16]=0xcc;
    if (FindVisualProxyAccessPath(begin,&owner,&proxy)) return 11;
    begin=guarded; codeSize=sizeof guarded;
    guarded[11]=3; // Unknown branch target must not be scanned linearly.
    if (FindVisualProxyAccessPath(begin,&owner,&proxy)) return 3;
    guarded[11]=2; guarded[12]=0xcc; // Null path no longer returns safely.
    if (FindVisualProxyAccessPath(begin,&owner,&proxy)) return 4;
    guarded[12]=0xc3; guarded[18]=0xcc; // Non-null path must return the proxy.
    if (FindVisualProxyAccessPath(begin,&owner,&proxy)) return 5;
    puts("PASS: local/user guarded and direct getters accepted; unproven paths rejected");
}
'''
compiler = os.environ.get('CXX') or shutil.which('clang++') or 'E:/Tools/Windhawk/Compiler/bin/clang++.exe'
with tempfile.TemporaryDirectory(prefix='wobbly-access-') as tmp:
    cpp = Path(tmp) / 'test.cpp'
    binary = Path(tmp) / 'test.exe'
    cpp.write_text(harness + function + tests, encoding='utf-8')
    subprocess.run([compiler, '-std=c++17', '-static', str(cpp), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
