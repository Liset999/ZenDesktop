"""Link the generated mod against the real Windhawk API and inspect exports.

Creates only a temporary diagnostic DLL; never loads or installs it.
"""
from pathlib import Path
import argparse
import re
import struct
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', required=True, type=Path)
parser.add_argument('--engine-dir', type=Path)
parser.add_argument('--architecture',choices=['x86','x64','both'],default='both')
args = parser.parse_args()
if args.engine_dir:
    engine = args.engine_dir
else:
    engines = [p for p in (args.compiler_dir.parent / 'Engine').iterdir() if (p / '64/windhawk.lib').is_file()]
    engine = sorted(engines, key=lambda p: tuple(int(n) for n in p.name.split('.')))[-1]
source = (root / 'optional/local@zen-desktop-widgets.wh.cpp').read_text(encoding='utf-8')
libraries = re.search(r'// @compilerOptions\s+([^\r\n]+)', source)[1].split()
for architecture in (['x86','x64'] if args.architecture == 'both' else [args.architecture]):
    command = [str(args.compiler_dir / 'bin/clang++.exe'), '-x', 'c++', '-', '-std=c++23',
               '-target', ('i686-w64-mingw32' if architecture == 'x86' else 'x86_64-w64-mingw32'), '-DUNICODE', '-D_UNICODE', '-DWH_MOD',
               '-DWINVER=0x0A00', '-D_WIN32_WINNT=0x0A00', '-D_WIN32_IE=0x0A00',
               '-DNTDDI_VERSION=0x0A000008', '-D__USE_MINGW_ANSI_STDIO=0',
               '-DWH_MOD_ID=L"local@zen-desktop-widgets"', '-include', 'windhawk_api.h',
               '-shared', '-O2', '-x', 'none', str(engine / ('32' if architecture == 'x86' else '64') / 'windhawk.lib'), *libraries]
    with tempfile.TemporaryDirectory(prefix='zen-widgets-build-') as directory:
        dll = Path(directory) / 'widgets.dll'
        result = subprocess.run(command + ['-o', str(dll)], input=source.encode(), capture_output=True)
        print(result.stderr.decode(errors='replace'))
        if result.returncode:
            raise SystemExit(result.returncode)
        data = dll.read_bytes()
        pe = struct.unpack_from('<I', data, 0x3c)[0]
        sections_count = struct.unpack_from('<H', data, pe + 6)[0]
        opt_size = struct.unpack_from('<H', data, pe + 20)[0]
        opt = pe + 24
        assert struct.unpack_from('<H', data, opt)[0] == (0x10b if architecture == 'x86' else 0x20b)
        sections = opt + opt_size

        def offset(rva):
            for index in range(sections_count):
                section = sections + index * 40
                virtual_size, address, raw_size, raw = struct.unpack_from('<IIII', data, section + 8)
                if address <= rva < address + max(virtual_size, raw_size):
                    return raw + rva - address
            raise ValueError(f'Unmapped RVA {rva:x}')

        export = offset(struct.unpack_from('<I', data, opt + (96 if architecture == 'x86' else 112))[0])
        names_count = struct.unpack_from('<I', data, export + 24)[0]
        names = offset(struct.unpack_from('<I', data, export + 32)[0])
        exports = []
        for index in range(names_count):
            start = offset(struct.unpack_from('<I', data, names + 4 * index)[0])
            exports.append(data[start:data.index(b'\0', start)].decode('ascii'))
        # Windhawk resolves the C++ mangled names (engine/mod.cpp), not C names.
        callbacks = {name for name in exports if re.fullmatch(r'_Z[0-9]+Wh_Mod[A-Za-z]+v', name)}
        expected = {'_Z10Wh_ModInitv', '_Z15Wh_ModAfterInitv', '_Z12Wh_ModUninitv', '_Z21Wh_ModSettingsChangedv'}
        assert callbacks == expected, callbacks
        assert '_ZN8ZenAudio10Wh_ModInitEv' in exports
        print(f'PASS: real Windhawk {architecture} DLL linked ({len(data)} bytes), four unified callbacks and isolated upstream callbacks')
