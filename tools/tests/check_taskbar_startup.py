"""Check taskbar startup recovery without loading a mod into Explorer."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--source', type=Path)
parser.add_argument('--compiler-dir', type=Path, default=Path('D:/Tools/Windhawk/Compiler'))
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = args.source or root / 'local@zen-taskbar-acrylic.wh.cpp'
code = source.read_text(encoding='utf-8-sig')

def function_text(signature):
    start = code.index(signature)
    opening = code.index('{', start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (code[end] == '{') - (code[end] == '}')
        end += 1
    return code[start:end]

failure_handler = function_text('void ZenHandleSubscriptionFailure(HRESULT statusCode) {')
window_handler = function_text('static LRESULT CALLBACK TaskbarCreatedWndProc(')
assert 'if (!g_initialized && ZenReapplyAllowed())' in window_handler
constructor = function_text('VisualTreeWatcher::VisualTreeWatcher(')
assert constructor.index('AddRef();') < constructor.index('CreateThread(')
assert constructor.index('watcher->m_stopping.load()') < constructor.index('ZenHandleSubscriptionFailure(hr)')
assert constructor.index('ZenHandleSubscriptionFailure(hr)') < constructor.index('watcher->Release();')
unadvise = function_text('void VisualTreeWatcher::UnadviseVisualTreeChange()')
assert unadvise.index('m_stopping = true') < unadvise.index('->UnadviseVisualTreeChange(this)')
callback = function_text('HRESULT VisualTreeWatcher::OnVisualTreeChange(')
assert callback.index('m_stopping.load()') < callback.index('FromHandle(')
retry = code[code.index('g_zenTapRetryTimer = CreateThreadpoolTimer('):]
assert 'ReapplyTaskbarStyles(true);' in retry.split('return TRUE;', 1)[0]

recovery = code[code.index('std::mutex g_zenTapRetryMutex;'):
                code.index('void ZenStopTapRetry()')]
harness = r'''
#include <atomic>
#include <cassert>
#include <mutex>
using HRESULT = long;
using ULONGLONG = unsigned long long;
using PTP_TIMER = void*;
struct FILETIME { unsigned long dwLowDateTime, dwHighDateTime; };
union ULARGE_INTEGER {
    struct { unsigned long LowPart, HighPart; };
    unsigned long long QuadPart;
};
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#define Wh_Log(...) ((void)0)
std::atomic<bool> g_initialized{true}, g_zenUnloading{false};
int scheduled = 0;
void SetThreadpoolTimer(PTP_TIMER, FILETIME*, int, int) { ++scheduled; }
'''
tests = r'''
int main() {
    g_zenTapRetryTimer = reinterpret_cast<void*>(1);
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(!g_initialized && scheduled == 1);
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(scheduled == 1); // queued != exhausted
    for (int expected = 2; expected <= 3; ++expected) {
        g_zenTapRetryPending = false; // consume the timer
        ZenHandleSubscriptionFailure(0x8000FFFF);
        assert(scheduled == expected);
    }
    g_zenTapRetryPending = false;
    ZenResetTapRetryBudget();
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(scheduled == 4); // cooldown recovery continues after bounded retries
    g_zenTapRetryPending = false;
    ZenResetTapRetryBudget();
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(scheduled == 5);
    g_zenTapRetryPending = false;
    g_zenUnloading = true;
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(scheduled == 5);
}
'''
with tempfile.TemporaryDirectory(prefix='taskbar-startup-') as temp:
    cpp = Path(temp) / 'test.cpp'
    binary = Path(temp) / 'test.exe'
    cpp.write_text(harness + recovery + tests, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23', '-static',
                    str(cpp), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
print('PASS: startup subscription preservation, worker lifetime, stale callback guard')
print('PASS: queued failures coalesce, bounded retries continue in cooldown, unload suppresses recovery')
