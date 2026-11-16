"""Exercise the real shell32 header bitmap and the mod's three copy hooks."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler-dir', type=Path, required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = (root / 'local@zen-explorer-styler.wh.cpp').read_text(encoding='utf-8-sig')
hooks = source[source.index('using BitBlt_t = decltype(&BitBlt);'):
               source.index('// The navigation pane divider is a horizontal line')]
code = r'''
#define NOMINMAX
#include <windows.h>
#include <vector>
#include <cassert>
#include <cstdio>
#define WH_MOD_ID L"local@zen-explorer-styler"
HWND g_entireWindowEffectWndForThread;
struct DCWnd { HWND hWnd; bool fileExplorer; };
const DCWnd* GetMemoryDCWnd(HDC) { return nullptr; }
auto CreateCompatibleDC_Original = &CreateCompatibleDC;
auto DeleteDC_Original = &DeleteDC;
auto FillRect_Original = &FillRect;
HOOKS
int main() {
    auto shell = LoadLibraryW(L"shell32.dll");
    assert(shell);
    auto root = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 1, 1,
                                HWND_MESSAGE, nullptr, nullptr, nullptr);
    assert(root);
    g_entireWindowEffectWndForThread = root;
    SetPropW(root, L"windhawk_background_effect-" WH_MOD_ID, (HANDLE)1);
    BitBlt_Original = &BitBlt;
    StretchBlt_Original = &StretchBlt;
    AlphaBlend_Original = &AlphaBlend;
    auto bitmap = (HBITMAP)LoadImageW(shell, MAKEINTRESOURCEW(635), IMAGE_BITMAP,
                                     0, 0, LR_CREATEDIBSECTION);
    assert(bitmap);
    BITMAP size{};
    GetObject(bitmap, sizeof(size), &size);
    auto source = CreateCompatibleDC(nullptr);
    auto oldSource = SelectObject(source, bitmap);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = 256;
    info.bmiHeader.biHeight = -180;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    DWORD* pixels = nullptr;
    auto targetBitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS,
                                        (void**)&pixels, nullptr, 0);
    auto target = CreateCompatibleDC(nullptr);
    auto oldTarget = SelectObject(target, targetBitmap);
    auto clear = [&] { GdiFlush(); std::fill_n(pixels, 256 * 180, 0x7FABCDEF); };
    auto verify = [&](const char* name) {
        GdiFlush();
        for (int y = 10; y < 100; ++y)
            for (int x = 10; x < 202; ++x) assert(pixels[y * 256 + x] == 0);
        assert(pixels[0] == 0x7FABCDEF);
        printf("PASS: %s clears header to transparent black, preserves outside\n", name);
    };
    clear();
    StretchBlt(target, 10, 10, 192, 90, source, 0, 0,
                size.bmWidth, size.bmHeight, SRCCOPY);
    GdiFlush();
    int invalid = 0;
    for (int y = 10; y < 100; ++y)
        for (int x = 10; x < 202; ++x) {
            DWORD pixel = pixels[y * 256 + x];
            invalid += (pixel & 0x00FFFFFF) && !(pixel >> 24);
        }
    printf("Native header copy leaves %d pixels with RGB and zero alpha\n", invalid);
    assert(invalid > 0);
    clear();
    assert(StretchBlt_Hook(target, 10, 10, 192, 90, source, 0, 0,
                           size.bmWidth, size.bmHeight, SRCCOPY));
    verify("StretchBlt");
    clear();
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, 0};
    assert(AlphaBlend_Hook(target, 10, 10, 192, 90, source, 0, 0,
                           size.bmWidth, size.bmHeight, blend));
    verify("AlphaBlend");
    clear();
    assert(BitBlt_Hook(target, 10, 10, size.bmWidth, size.bmHeight,
                       source, 0, 0, SRCCOPY));
    GdiFlush();
    assert(pixels[10 * 256 + 10] == 0);
    puts("PASS: BitBlt clears the matching resource");
    RemovePropW(root, L"windhawk_background_effect-" WH_MOD_ID);
    assert(!ZenPaintControlPanelHeader(target, 0, 0, 192, 90, source));
    SetPropW(root, L"windhawk_background_effect-" WH_MOD_ID, (HANDLE)1);
    // The same dimensions and almost identical pixels must not classify a
    // picture/icon as the header. Use a 32-bit copy to retain an exact change.
    BITMAPINFO otherInfo = info;
    otherInfo.bmiHeader.biWidth = size.bmWidth;
    otherInfo.bmiHeader.biHeight = -size.bmHeight;
    void* otherBits;
    auto other = CreateDIBSection(nullptr, &otherInfo, DIB_RGB_COLORS,
                                  &otherBits, nullptr, 0);
    auto otherDC = CreateCompatibleDC(nullptr);
    auto oldOther = SelectObject(otherDC, other);
    BitBlt(otherDC, 0, 0, size.bmWidth, size.bmHeight, source, 0, 0, SRCCOPY);
    SetPixel(otherDC, 1, 1, RGB(255, 0, 255));
    assert(!ZenPaintControlPanelHeader(target, 0, 0, 192, 90, otherDC));
    puts("PASS: other images and windows without glass keep native rendering");
    SelectObject(otherDC, oldOther); DeleteDC(otherDC); DeleteObject(other);
    SelectObject(target, oldTarget); DeleteDC(target); DeleteObject(targetBitmap);
    SelectObject(source, oldSource); DeleteDC(source); DeleteObject(bitmap);
    DestroyWindow(root);
}
'''.replace('HOOKS', hooks)
with tempfile.TemporaryDirectory(prefix='cpl-header-') as temp:
    cpp = Path(temp) / 'test.cpp'
    binary = Path(temp) / 'test.exe'
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23', '-static',
                    '-target', 'x86_64-w64-mingw32', '-DUNICODE', '-D_UNICODE', '-DWINVER=0x0A00',
                    '-D_WIN32_WINNT=0x0A00', str(cpp), '-lgdi32', '-lmsimg32',
                    '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
