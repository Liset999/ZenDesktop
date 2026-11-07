"""Regression for sticky GSMTC session selection (extracts PickStickyMediaApp)."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = (root / 'local@zen-taskbar-lyrics.wh.cpp').read_text(encoding='utf-8-sig')
start = source.index('struct MediaSessionCandidate {')
end = source.index('static GlobalSystemMediaTransportControlsSession FindSessionByApp(', start)
block = source[start:end]
assert 'PickStickyMediaApp' in block
assert 'MediaAppPriority' in block
assert 'kSessionStickHoldMs = 1500' in block
code = r'''
#include <cstdio>
#include <string>
#include <vector>
using ULONGLONG = unsigned long long;
''' + block + r'''
int main() {
    int failures = 0, checks = 0;
    auto check = [&](bool ok) { ++checks; if (!ok) ++failures; };
    StickySessionState st;
    std::vector<MediaSessionCandidate> both = {
        {L"com.bilibili.bilibiliPC", true, true},
        {L"Spotify.exe", true, true},
    };
    // Music outranks video even when Bilibili is system current.
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 1000) == L"Spotify.exe");
    check(st.stickyApp == L"Spotify.exe");
    // Rapid current flips must not displace a Playing music sticky.
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 1100) == L"Spotify.exe");
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 3000) == L"Spotify.exe");
    check(PickStickyMediaApp(both, L"Spotify.exe", st, 3100) == L"Spotify.exe");
    // Sticky paused: challenger Playing needs holdMs before switch.
    both[1].playing = false;
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 4000) == L"Spotify.exe");
    check(st.challengerApp == L"com.bilibili.bilibiliPC");
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 5499) == L"Spotify.exe");
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 5500) == L"com.bilibili.bilibiliPC");
    check(st.stickyApp == L"com.bilibili.bilibiliPC");
    // Higher-priority music can upgrade off a Playing low-priority sticky after hold.
    st = {};
    st.stickyApp = L"com.bilibili.bilibiliPC";
    both[0].playing = true;
    both[1].playing = true;
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 100) == L"com.bilibili.bilibiliPC");
    check(st.challengerApp == L"Spotify.exe");
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 1599) == L"com.bilibili.bilibiliPC");
    check(PickStickyMediaApp(both, L"com.bilibili.bilibiliPC", st, 1600) == L"Spotify.exe");
    // Sticky disappears -> adopt best immediately.
    st = {};
    st.stickyApp = L"gone.exe";
    std::vector<MediaSessionCandidate> one = {{L"Chrome", true, true}};
    check(PickStickyMediaApp(one, L"Chrome", st, 100) == L"Chrome");
    // Empty titles ignored.
    st = {};
    std::vector<MediaSessionCandidate> emptyTitle = {{L"x", true, false}};
    check(PickStickyMediaApp(emptyTitle, L"x", st, 1).empty());
    std::printf("Session sticky: %d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
'''
with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / 'session_sticky.cpp'
    exe = Path(directory) / 'session_sticky.exe'
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23',
                    '-static', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
