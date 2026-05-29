// ==WindhawkMod==
// @id              zen-fileexplorer-transparent
// @name            ZenDesktop: File Explorer Transparent
// @description     文件管理器透明底座，支持下拉菜单配置视觉材质与全局文字颜色覆盖。
// @version         1.0.0
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZenDesktop: File Explorer Transparent

将文件管理器内部区域变为完全透明，让底层系统级材质（如 Mica 或 Acrylic）完美透出。
结合我们标准化的设置系统，您可以强制改变整个文件列表区的文件名文字颜色！

### 核心特性
* **下拉配置**: 提供统一直观的下拉菜单，无需手动输入。
* **原生材质**: 直接调用底层 `DWM` 系统级渲染，性能零开销，不挂后台。
* **强制文字改色**: 针对浅色背景材质，可以强行把文字全部染黑、染白，甚至是彩色。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
- themePreset: "acrylic"
  $name: "🎭 视觉风格 (Visual Style)"
  $description: "选择底层的系统级视觉材质。推荐使用亚克力以获得最佳玻璃质感。"
  $options:
    - none: "无材质 (None)"
    - mica: "毛玻璃 (Mica)"
    - micaAlt: "毛玻璃增强 (Mica Alt)"
    - acrylic: "亚克力 (Acrylic)"
- textColorMode: "default"
  $name: "🔤 文字颜色 (Text Color)"
  $description: "自定义文字颜色，强行覆盖文件管理器原生颜色。浅色材质下文字不清晰时推荐切换为深黑。"
  $options:
    - default: "主题默认 (Theme Default)"
    - white: "强制白色 (Force White)"
    - dark: "强制深黑 (Force Dark)"
    - red: "🔴 红色 (Red)"
    - green: "🟢 绿色 (Green)"
    - blue: "🔵 蓝色 (Blue)"
    - yellow: "🟡 黄色 (Yellow)"
    - orange: "🟠 橙色 (Orange)"
    - purple: "🟣 紫色 (Purple)"
    - pink: "🌸 粉色 (Pink)"
    - cyan: "💠 青色 (Cyan)"
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <windhawk_utils.h>
#include <windhawk_api.h>
#include <unordered_set>

// ==================== 消息常量 ====================
#define WM_DEFERRED_INIT (WM_APP + 0x100)

// ==================== 全局状态与设置 ====================
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t Real_CreateWindowExW;

std::unordered_set<HWND> g_processedCabinets;

DWORD g_backdropType = 3; // 默认 Acrylic
COLORREF g_customTextColor = CLR_INVALID;

void LoadSettings() {
    PCWSTR themePreset = Wh_GetStringSetting(L"themePreset");
    if (themePreset) {
        if (wcscmp(themePreset, L"none") == 0) g_backdropType = 1;
        else if (wcscmp(themePreset, L"acrylic") == 0) g_backdropType = 3;
        else if (wcscmp(themePreset, L"micaAlt") == 0) g_backdropType = 4;
        else if (wcscmp(themePreset, L"mica") == 0) g_backdropType = 2;
        Wh_FreeStringSetting(themePreset);
    }

    PCWSTR textColorMode = Wh_GetStringSetting(L"textColorMode");
    if (textColorMode) {
        if (wcscmp(textColorMode, L"white") == 0) g_customTextColor = RGB(255, 255, 255);
        else if (wcscmp(textColorMode, L"dark") == 0) g_customTextColor = RGB(0, 0, 0);
        else if (wcscmp(textColorMode, L"red") == 0) g_customTextColor = RGB(255, 51, 51);
        else if (wcscmp(textColorMode, L"green") == 0) g_customTextColor = RGB(51, 255, 51);
        else if (wcscmp(textColorMode, L"blue") == 0) g_customTextColor = RGB(51, 153, 255);
        else if (wcscmp(textColorMode, L"yellow") == 0) g_customTextColor = RGB(255, 204, 0);
        else if (wcscmp(textColorMode, L"orange") == 0) g_customTextColor = RGB(255, 136, 0);
        else if (wcscmp(textColorMode, L"purple") == 0) g_customTextColor = RGB(204, 102, 255);
        else if (wcscmp(textColorMode, L"pink") == 0) g_customTextColor = RGB(255, 102, 178);
        else if (wcscmp(textColorMode, L"cyan") == 0) g_customTextColor = RGB(51, 255, 255);
        else g_customTextColor = CLR_INVALID;
        Wh_FreeStringSetting(textColorMode);
    }
}

// ==================== 子类化过程前向声明 ====================
LRESULT CALLBACK CabinetSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);
LRESULT CALLBACK ShellDefViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);
LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);
LRESULT CALLBACK TransparentChildSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);
BOOL CALLBACK EnumCabinetChildProc(HWND hChild, LPARAM lParam);
BOOL CALLBACK EnumWindowForUnsubclassProc(HWND hChild, LPARAM lParam);
BOOL CALLBACK EnumExistingExplorerProc(HWND hWnd, LPARAM lParam);

// ==================== DWM 工具函数 ====================
void ApplyMicaToWindow(HWND hWnd)
{
    HMODULE hDwmApi = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hDwmApi) return;

    using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
    auto pDwmSetWindowAttribute = (DwmSetWindowAttribute_t)GetProcAddress(hDwmApi, "DwmSetWindowAttribute");

    if (pDwmSetWindowAttribute)
    {
        // DWMWA_SYSTEMBACKDROP_TYPE = 102 (Win11 22H2+)
        pDwmSetWindowAttribute(hWnd, 102, &g_backdropType, sizeof(g_backdropType));
    }
    FreeLibrary(hDwmApi);
}

// ==================== 透明化应用 ====================
void ApplyTransparencyToCabinet(HWND hCabinet)
{
    if (g_processedCabinets.count(hCabinet)) return;
    g_processedCabinets.insert(hCabinet);

    ApplyMicaToWindow(hCabinet);
    EnumChildWindows(hCabinet, EnumCabinetChildProc, 0);
    InvalidateRect(hCabinet, NULL, TRUE);
}

BOOL CALLBACK EnumCabinetChildProc(HWND hChild, LPARAM lParam)
{
    (void)lParam;
    WCHAR szClass[128];
    if (!GetClassNameW(hChild, szClass, ARRAYSIZE(szClass))) return TRUE;

    if (wcscmp(szClass, L"SHELLDLL_DefView") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hChild, ShellDefViewSubclassProc, 0);

        HWND hListView = FindWindowExW(hChild, NULL, L"SysListView32", NULL);
        if (hListView)
        {
            ListView_SetBkColor(hListView, CLR_NONE);
            ListView_SetTextBkColor(hListView, CLR_NONE);
            if (g_customTextColor != CLR_INVALID) {
                ListView_SetTextColor(hListView, g_customTextColor);
            }
            WindhawkUtils::SetWindowSubclassFromAnyThread(hListView, ListViewSubclassProc, 0);
            InvalidateRect(hListView, NULL, TRUE);
        }
    }
    else if (wcscmp(szClass, L"DirectUIHWND") == 0 ||
             wcscmp(szClass, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
             wcscmp(szClass, L"WorkerW") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hChild, TransparentChildSubclassProc, 0);
    }
    return TRUE;
}

// ==================== 子类化处理过程 ====================
LRESULT CALLBACK CabinetSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_DEFERRED_INIT:
        ApplyTransparencyToCabinet(hWnd);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_NCDESTROY:
        g_processedCabinets.erase(hWnd);
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ShellDefViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        SetBkMode((HDC)wParam, TRANSPARENT);
        if (g_customTextColor != CLR_INVALID) {
            SetTextColor((HDC)wParam, g_customTextColor);
        }
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 1;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TransparentChildSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 1;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ==================== CreateWindowExW Hook ====================
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = Real_CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName))
    {
        if (wcscmp(lpClassName, L"CabinetWClass") == 0)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, CabinetSubclassProc, 0);
            PostMessageW(hWnd, WM_DEFERRED_INIT, 0, 0);
        }
    }
    return hWnd;
}

// ==================== 处理已存在的窗口 ====================
BOOL CALLBACK EnumExistingExplorerProc(HWND hWnd, LPARAM lParam)
{
    (void)lParam;
    DWORD dwProcessId;
    GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (dwProcessId != GetCurrentProcessId()) return TRUE;

    WCHAR szClass[64];
    if (GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass)) && wcscmp(szClass, L"CabinetWClass") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, CabinetSubclassProc, 0);
        ApplyTransparencyToCabinet(hWnd);
    }
    return TRUE;
}

void ApplyToExistingExplorerWindows()
{
    EnumWindows(EnumExistingExplorerProc, 0);
}

void ReapplyAllSettings()
{
    LoadSettings();
    for (HWND hCabinet : g_processedCabinets) {
        ApplyMicaToWindow(hCabinet); // 重新应用材质
        // 重新遍历子窗口以应用新的文字颜色
        EnumChildWindows(hCabinet, EnumCabinetChildProc, 0);
        InvalidateRect(hCabinet, NULL, TRUE);
    }
}

// ==================== 清理函数 ====================
BOOL CALLBACK EnumWindowForUnsubclassProc(HWND hChild, LPARAM lParam)
{
    (void)lParam;
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hChild, ShellDefViewSubclassProc);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hChild, ListViewSubclassProc);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hChild, TransparentChildSubclassProc);
    return TRUE;
}

void UnsubclassAllWindows()
{
    for (HWND hCabinet : g_processedCabinets)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hCabinet, CabinetSubclassProc);
        EnumChildWindows(hCabinet, EnumWindowForUnsubclassProc, 0);
        InvalidateRect(hCabinet, NULL, TRUE);
    }
    g_processedCabinets.clear();
}

// ==================== Windhawk 入口点 ====================
BOOL Wh_ModInit()
{
    LoadSettings();
    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&Real_CreateWindowExW))
        return FALSE;
    ApplyToExistingExplorerWindows();
    return TRUE;
}

void Wh_ModSettingsChanged()
{
    ReapplyAllSettings();
}

void Wh_ModAfterInit()
{
    ApplyToExistingExplorerWindows();
}

void Wh_ModUninit()
{
    UnsubclassAllWindows();
}
