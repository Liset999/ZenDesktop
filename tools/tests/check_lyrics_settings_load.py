"""Regression: compact layout must load settings even when positionMode is empty."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = (root / 'local@zen-taskbar-lyrics.wh.cpp').read_text(encoding='utf-8-sig')
start = source.index('static bool SettingsBodyWritten(')
end = source.index('static void LoadSettings()', start)
block = source[start:end]
assert 'layout > 0 || !positionMode.empty()' in block
# clickAction must live inside the settings-written branch, not only behind mode.empty().
load = source[source.index('static void LoadSettings()'):source.index('static bool WantsCapturedBackdrop')]
assert 'SettingsBodyWritten(g_settingsLayout, mode)' in load
assert 'GetStr(L"clickAction")' in load
code = r'''
#include <cstdio>
#include <string>
''' + block + r'''
int main() {
    int failures = 0, checks = 0;
    auto check = [&](bool ok) { ++checks; if (!ok) ++failures; };
    check(!SettingsBodyWritten(0, L""));           // never saved flat
    check(SettingsBodyWritten(0, L"widgets"));     // flat with positionMode
    check(SettingsBodyWritten(1, L""));            // grouped layout
    check(SettingsBodyWritten(2, L""));            // compact: common.* only
    check(SettingsBodyWritten(2, L"widgets"));
    std::printf("Settings load gate: %d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
'''
with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / 'settings_load.cpp'
    exe = Path(directory) / 'settings_load.exe'
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23',
                    '-static', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
