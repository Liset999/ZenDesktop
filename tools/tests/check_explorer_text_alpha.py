"""Exercise the Explorer text renderer on an offscreen 32-bit surface."""
import argparse
from pathlib import Path
import subprocess
import tempfile


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler-dir', type=Path, required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = (root / 'local@zen-explorer-styler.wh.cpp').read_text(encoding='utf-8-sig')
helpers = source[source.index('BYTE g_lightTextAlphaLut[256];'):
                 source.index('bool PaintExtTextOutBackground(')]
renderer = source[source.index('BOOL ExtTextOutWithAlpha('):
                  source.index('// ZenDesktop 1.7.2: classic Control Panel (DirectUI)')]
code = r'''
#include <windows.h>
#include <uxtheme.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
auto ExtTextOutW_Original = &ExtTextOutW;
int backgroundCalls;
bool PaintExtTextOutBackground(HDC, const RECT*) {
    ++backgroundCalls;
    return true;
}
HELPERS
RENDERER
int main() {
    BufferedPaintInit();
    HDC dc = CreateCompatibleDC(nullptr);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = 256;
    info.bmiHeader.biHeight = -64;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits;
    HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    HFONT font = CreateFontW(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    InitTextAlphaLuts();
    int failures = 0;
    for (COLORREF color : {RGB(0, 0, 0), RGB(255, 255, 255), RGB(255, 136, 0)}) {
        for (UINT options : {0u, UINT(ETO_CLIPPED), UINT(ETO_GLYPH_INDEX),
                             UINT(ETO_OPAQUE | ETO_CLIPPED)}) {
            ZeroMemory(bits, 256 * 64 * 4);
            SetTextColor(dc, color);
            RECT clip{0, 0, 256, 64};
            WCHAR glyphs[4];
            GetGlyphIndicesW(dc, L"File", 4, reinterpret_cast<WORD*>(glyphs), 0);
            const WCHAR* text = (options & ETO_GLYPH_INDEX) ? glyphs : L"File";
            BOOL result = ExtTextOutWithAlpha(dc, 8, 8, options, &clip, text, 4, nullptr, color);
            GdiFlush();
            int visible = 0;
            bool premultiplied = true;
            auto pixels = static_cast<RGBQUAD*>(bits);
            for (int i = 0; i < 256 * 64; ++i) {
                auto px = pixels[i];
                visible += px.rgbReserved != 0;
                premultiplied &= px.rgbRed <= px.rgbReserved &&
                                 px.rgbGreen <= px.rgbReserved && px.rgbBlue <= px.rgbReserved;
            }
            bool passed = result && visible && premultiplied;
            printf("color=%06lx options=%u visible=%d premultiplied=%d %s\n",
                   static_cast<unsigned long>(color), options, visible, premultiplied,
                   passed ? "PASS" : "FAIL");
            failures += !passed;
        }
    }
    SelectObject(dc, oldFont);
    SelectObject(dc, oldBitmap);
    DeleteObject(font);
    DeleteObject(bitmap);
    DeleteDC(dc);
    BufferedPaintUnInit();
    return failures ? 1 : 0;
}
'''.replace('HELPERS', helpers).replace('RENDERER', renderer)
with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / 'text.cpp'
    exe = Path(directory) / 'text.exe'
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23',
                    '-DUNICODE', '-D_UNICODE', '-static', str(cpp),
                    '-lgdi32', '-luxtheme', '-o', str(exe)], check=True)
    raise SystemExit(subprocess.run([str(exe)]).returncode)

