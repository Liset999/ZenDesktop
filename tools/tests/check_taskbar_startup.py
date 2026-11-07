"""Check taskbar startup recovery without loading a mod into Explorer."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--source', type=Path)
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

harness = r'''
#include <atomic>
#include <cassert>
using HRESULT = long;
std::atomic<bool> g_initialized{true}, g_zenUnloading{false};
bool retryAvailable = true;
int prompts = 0, scheduled = 0;
bool ZenScheduleTapRetry() {
    if (g_zenUnloading || !retryAvailable) return false;
    retryAvailable = false;
    ++scheduled;
    return true;
}
void PromptToRestartExplorer(HRESULT) { ++prompts; }
'''
tests = r'''
int main() {
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(!g_initialized && scheduled == 1 && prompts == 0);
    g_initialized = true;
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(!g_initialized && scheduled == 1 && prompts == 1);
    g_zenUnloading = true;
    ZenHandleSubscriptionFailure(0x8000FFFF);
    assert(scheduled == 1 && prompts == 1);
}
'''
with tempfile.TemporaryDirectory(prefix='taskbar-startup-') as temp:
    cpp = Path(temp) / 'test.cpp'
    binary = Path(temp) / 'test.exe'
    cpp.write_text(harness + failure_handler + tests, encoding='utf-8')
    subprocess.run(['E:/Tools/Windhawk/Compiler/bin/clang++.exe', '-std=c++23',
                    str(cpp), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
print('PASS: startup subscription preservation, worker lifetime, stale callback guard')
print('PASS: first failure retries, repeated failure prompts, unload suppresses recovery')
