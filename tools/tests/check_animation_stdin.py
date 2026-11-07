from pathlib import Path
import argparse
import re
import subprocess
import tempfile

parser = argparse.ArgumentParser(description='Check animation mod from stdin without a local include search path.')
parser.add_argument('--compiler-dir', required=True, type=Path)
parser.add_argument('--source', type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = args.source or root / 'optional/local@windows-animations.wh.cpp'
code = source.read_text(encoding='utf-8-sig')
header = (root / 'optional/zen-game-guard.h').read_text(encoding='utf-8-sig')
body = re.sub(r'^#pragma once\s*\n', '', header, flags=re.M).rstrip()
embedded = re.search(r'// BEGIN embedded zen-game-guard\.h\n[^\n]*\n(.*?)\n// END embedded zen-game-guard\.h', code, re.S)
if not embedded or embedded[1] != body:
    raise SystemExit('Embedded game guard missing or stale; run tools/sync_animation_guard.py')
command = [str(args.compiler_dir / 'bin/clang++.exe'), '-x', 'c++', '-', '-std=c++23',
           '-target', 'x86_64-w64-mingw32', '-DUNICODE', '-D_UNICODE', '-DWINVER=0x0A00',
           '-D_WIN32_WINNT=0x0A00', '-D_WIN32_IE=0x0A00', '-DNTDDI_VERSION=0x0A000008',
           '-D__USE_MINGW_ANSI_STDIO=0', '-DWH_MOD', '-DWH_MOD_ID=L"local@windows-animations"',
           '-include', 'windhawk_api.h', '-fsyntax-only']
with tempfile.TemporaryDirectory() as directory:
    result = subprocess.run(command, input=source.read_bytes(), cwd=directory, capture_output=True)
print(result.stderr.decode('utf-8', errors='replace'))
print(f'Stdin syntax check exit: {result.returncode}; no DLL built or enabled')
raise SystemExit(result.returncode)
