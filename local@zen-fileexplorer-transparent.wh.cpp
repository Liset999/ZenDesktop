// ==WindhawkMod==
// @id              zen-fileexplorer-transparent
// @name            TPDesktop: File Explorer Transparent Background
// @description     Makes File Explorer folder background transparent - shows desktop wallpaper through the file list area. Configurable modes. Zero overhead, no background processes.
// @version         2.0.0
// @author          TPGoFighting
// @github          https://github.com/TPGoFighting
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# TPDesktop: File Explorer Transparent Background

Makes the File Explorer folder background transparent so your desktop
wallpaper shows through the file list area. File icons and text remain
fully visible and functional.

### Transparency Modes:
* **Mica** (default): Uses Windows 11 Mica backdrop - wallpaper-derived
  background with subtle styling. Best compatibility across all Win11 builds.
* **Mica Alt**: Uses Mica Alt (lighter variant) for a brighter backdrop.
  Requires Win11 22H2+.
* **Clear**: No backdrop tint - clean background with no DWM styling.
  Child windows become transparent, letting the native window color show.
* **Blur**: Acrylic blur effect behind the file list area. Requires
  Win11 22H2+ for best results.

### Key Features:
* **All Windows 11 Versions**: Compatible with 21H2 through 24H2+.
  Automatically adapts to the current Win11 build.
* **Zero Overhead**: Embedded natively inside explorer.exe. No background
  EXE, no taskbar icons, 0% CPU, virtually 0 MB extra RAM.
* **Settings UI**: Full Windhawk settings page - customize transparency
  mode, text color, and which areas to apply the effect to.
* **Text Color Mode**: Override text color for readability on transparent
  backgrounds. Choose white, dark gray, or system-aware auto-switching.
* **Process-Native Hooking**: Automatically subclasses File Explorer windows
  as they are created. Handles new windows, tabs, and Explorer restarts.
* **Safe**: Uses native Win32 subclassing and DWM composition. No XAML
  Diagnostics dependency.

### Notes
* Navigation pane and command bar retain default backgrounds by default.
  Enable transparency for them in settings if desired.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- transparencyMode: "blur"
  $name: 文件夹背景透明模式
  $options:
    - "mica": "Mica - 壁纸衍生背景"
    - "micaAlt": "Mica Alt - 变体壁纸背景 (22H2+)"
    - "clear": "Clear - 无背景效果"
    - "blur": "Blur - 丙烯酸模糊效果"
- textColorMode: "default"
  $name: 文件列表文字颜色覆盖
  $options:
    - "default": "Default - 系统默认"
    - "white": "White - 强制白色文字"
    - "dark": "Dark - 强制深灰文字"
    - "system": "System - 跟随系统主题"
- applyToNavPane: false
  $name: 透明效果应用到导航面板
- applyToCommandBar: false
  $name: 透明效果应用到命令栏工具栏
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

#include <unordered_set>

// ==================== 消息常量 ====================

#define WM_DEFERRED_INIT (WM_APP + 0x100)

// 修正：mingw-w64 dwmapi.h 的 DWMWA_SYSTEMBACKDROP_TYPE = 38（枚举位置）
// 正确 Windows SDK 值应为 102，头文件枚举值不可用，自定义常量代替
#define DWMWA_SYSTEMBACKDROP_TYPE_102 102

// ==================== Windows 版本检测 ====================

// 使用 ntdll!RtlGetNtVersionNumbers 检测构建版本号
// 不需要任何结构体定义，兼容所有 MinGW 版本
bool g_canUseDwmBackdrop = false;  // DWMWA_SYSTEMBACKDROP_TYPE_102 可用 (22H2+)

static void DetectWindowsBuild()
{
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return;

    typedef void(WINAPI* RtlGetNtVersionNumbers_t)(DWORD*, DWORD*, DWORD*);
    auto pRtlGetNtVersionNumbers = (RtlGetNtVersionNumbers_t)
        GetProcAddress(hNtdll, "RtlGetNtVersionNumbers");
    if (!pRtlGetNtVersionNumbers) return;

    DWORD major = 0, minor = 0, build = 0;
    pRtlGetNtVersionNumbers(&major, &minor, &build);
    build &= 0x0FFFFFFF;  // 高半字节是修订号，掩码去除

    // DWMWA_SYSTEMBACKDROP_TYPE_102 从 Win11 22H2 (build 22621) 开始可用
    if (build >= 22621)
        g_canUseDwmBackdrop = true;
}

// ==================== 设置管理 ====================

enum TransparencyMode {
    kModeMica,
    kModeMicaAlt,
    kModeClear,
    kModeBlur,
};

enum TextColorMode {
    kColorDefault,
    kColorWhite,
    kColorDark,
    kColorSystem,
};

struct {
    TransparencyMode transparencyMode;
    TextColorMode textColorMode;
    bool applyToNavPane;
    bool applyToCommandBar;
} g_settings;

static void LoadSettings()
{
    PCWSTR mode = Wh_GetStringSetting(L"transparencyMode");
    g_settings.transparencyMode = kModeMica;
    if (wcscmp(mode, L"micaAlt") == 0)
        g_settings.transparencyMode = kModeMicaAlt;
    else if (wcscmp(mode, L"clear") == 0)
        g_settings.transparencyMode = kModeClear;
    else if (wcscmp(mode, L"blur") == 0)
        g_settings.transparencyMode = kModeBlur;
    Wh_FreeStringSetting(mode);

    PCWSTR textMode = Wh_GetStringSetting(L"textColorMode");
    g_settings.textColorMode = kColorDefault;
    if (wcscmp(textMode, L"white") == 0)
        g_settings.textColorMode = kColorWhite;
    else if (wcscmp(textMode, L"dark") == 0)
        g_settings.textColorMode = kColorDark;
    else if (wcscmp(textMode, L"system") == 0)
        g_settings.textColorMode = kColorSystem;
    Wh_FreeStringSetting(textMode);

    g_settings.applyToNavPane = Wh_GetIntSetting(L"applyToNavPane") != 0;
    g_settings.applyToCommandBar = Wh_GetIntSetting(L"applyToCommandBar") != 0;
}

// ==================== DWM/ACCENT API 动态加载 ====================

typedef HRESULT(WINAPI* DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
static DwmSetWindowAttribute_t pDwmSetWindowAttribute = nullptr;

typedef BOOL(WINAPI* SetWindowCompositionAttribute_t)(HWND, PVOID);
static SetWindowCompositionAttribute_t pSetWindowCompositionAttribute = nullptr;

// ACCENT_POLICY 结构体 (uxtheme 未公开)
struct ACCENT_POLICY {
    DWORD AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

struct WINCOMPATTRDATA {
    DWORD Attribute;
    ACCENT_POLICY* Data;
    SIZE_T DataSize;
};

// ACCENT_STATE 常量 (未公开)
#define ACCENT_DISABLED 0
#define ACCENT_ENABLE_GRADIENT 1
#define ACCENT_ENABLE_TRANSPARENTBLURBEHIND 2
#define ACCENT_ENABLE_BLURBEHIND 3
#define ACCENT_ENABLE_ACRYLICBLURBEHIND 4

// WCA 常量 (未公开)
#define WCA_ACCENT_POLICY 19

static void LoadDwmApis()
{
    HMODULE hDwmApi = GetModuleHandleW(L"dwmapi.dll");
    if (hDwmApi)
    {
        pDwmSetWindowAttribute = (DwmSetWindowAttribute_t)
            GetProcAddress(hDwmApi, "DwmSetWindowAttribute");
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32)
    {
        pSetWindowCompositionAttribute = (SetWindowCompositionAttribute_t)
            GetProcAddress(hUser32, "SetWindowCompositionAttribute");
    }
}

// ==================== 背景效果应用 ====================

// 清除之前设置的 SWCA 效果
static void ClearAccent(HWND hWnd)
{
    if (pSetWindowCompositionAttribute)
    {
        ACCENT_POLICY accent = { ACCENT_DISABLED, 0, 0, 0 };
        WINCOMPATTRDATA wcd = { WCA_ACCENT_POLICY, &accent, sizeof(accent) };
        pSetWindowCompositionAttribute(hWnd, &wcd);
    }
}

// 将 DWM 背景恢复为系统默认 (AUTO)
static void ResetDwmBackdrop(HWND hWnd)
{
    if (g_canUseDwmBackdrop && pDwmSetWindowAttribute)
    {
        DWORD auto_ = DWMSBT_AUTO;
        pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE_102,
                               &auto_, sizeof(auto_));
    }
}

static void ApplyBackdropEffect(HWND hWnd, TransparencyMode mode)
{
    // 先清除所有已有效果，避免 SWCA 与 DWM 冲突
    ClearAccent(hWnd);
    ResetDwmBackdrop(hWnd);

    if (mode == kModeBlur)
    {
        // Blur: 真实可用的丙烯酸模糊效果
        // 保留 Mica 作为基础层，再叠 SWCA 丙烯酸模糊 + 白色着色
        if (pSetWindowCompositionAttribute)
        {
            ACCENT_POLICY accent = {
                ACCENT_ENABLE_ACRYLICBLURBEHIND,
                0,
                0x30FFFFFF,  // AA=0x30 (19%白色着色) —— 肉眼可见的磨砂玻璃效果
                0
            };
            WINCOMPATTRDATA wcd = { WCA_ACCENT_POLICY, &accent, sizeof(accent) };
            pSetWindowCompositionAttribute(hWnd, &wcd);
        }
        // Fallback: MicaAlt 提供变体背景
        else if (g_canUseDwmBackdrop && pDwmSetWindowAttribute)
        {
            DWORD backdropType = DWMSBT_TABBEDWINDOW;
            pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE_102,
                                   &backdropType, sizeof(backdropType));
        }
    }
    else if (mode == kModeClear)
    {
        // Clear: 完全去除背景，无 DWM 无 SWCA
        if (g_canUseDwmBackdrop && pDwmSetWindowAttribute)
        {
            DWORD none = DWMSBT_NONE;
            pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE_102,
                                   &none, sizeof(none));
        }
    }
    else if (mode == kModeMica && g_canUseDwmBackdrop && pDwmSetWindowAttribute)
    {
        DWORD backdropType = DWMSBT_MAINWINDOW;
        pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE_102,
                               &backdropType, sizeof(backdropType));
    }
    else if (mode == kModeMicaAlt && g_canUseDwmBackdrop && pDwmSetWindowAttribute)
    {
        DWORD backdropType = DWMSBT_TABBEDWINDOW;
        pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE_102,
                               &backdropType, sizeof(backdropType));
    }

    // 强制刷新窗口
    InvalidateRect(hWnd, NULL, TRUE);
}

// ==================== 文字颜色工具 ====================

static COLORREF GetTextColorForMode()
{
    switch (g_settings.textColorMode)
    {
    case kColorWhite:
        return RGB(0xFF, 0xFF, 0xFF);
    case kColorDark:
        return RGB(0x33, 0x33, 0x33);
    case kColorSystem:
    {
        HKEY hKey;
        DWORD value = 1;
        DWORD size = sizeof(value);
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr,
                             (LPBYTE)&value, &size);
            RegCloseKey(hKey);
        }
        return (value == 0) ? RGB(0xFF, 0xFF, 0xFF) : RGB(0x33, 0x33, 0x33);
    }
    default:
        return CLR_DEFAULT;
    }
}

static bool IsTextColorOverridden()
{
    return g_settings.textColorMode != kColorDefault;
}

// ==================== 子类化过程声明 ====================

LRESULT CALLBACK CabinetSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK ShellDefViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK ListViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK TransparentChildSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK NavPaneSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

BOOL CALLBACK EnumCabinetChildProc(HWND, LPARAM);
BOOL CALLBACK EnumExistingExplorerProc(HWND, LPARAM);

// ==================== 全局状态 ====================

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t Real_CreateWindowExW;

static std::unordered_set<HWND> g_processedCabinets;

// ==================== 核心透明化逻辑 ====================

static void ApplyTransparencyToCabinet(HWND hCabinet)
{
    if (g_processedCabinets.count(hCabinet))
        return;
    g_processedCabinets.insert(hCabinet);

    ApplyBackdropEffect(hCabinet, g_settings.transparencyMode);
    EnumChildWindows(hCabinet, EnumCabinetChildProc, 0);
    InvalidateRect(hCabinet, NULL, TRUE);
}

BOOL CALLBACK EnumCabinetChildProc(HWND hChild, LPARAM lParam)
{
    (void)lParam;

    WCHAR szClass[128];
    if (!GetClassNameW(hChild, szClass, ARRAYSIZE(szClass)) || !hChild)
        return TRUE;

    if (wcscmp(szClass, L"SHELLDLL_DefView") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hChild, ShellDefViewSubclassProc, 0);

        HWND hListView = FindWindowExW(hChild, NULL, L"SysListView32", NULL);
        if (hListView)
        {
            ListView_SetBkColor(hListView, CLR_NONE);
            ListView_SetTextBkColor(hListView, CLR_NONE);

            if (IsTextColorOverridden())
                ListView_SetTextColor(hListView, GetTextColorForMode());

            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hListView, ListViewSubclassProc, 0);
            InvalidateRect(hListView, NULL, TRUE);
        }

        HWND hDui = FindWindowExW(hChild, NULL, L"DirectUIHWND", NULL);
        if (hDui)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hDui, TransparentChildSubclassProc, 0);
        }
    }
    else if (wcscmp(szClass, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hChild, TransparentChildSubclassProc, 0);
    }
    else if (wcscmp(szClass, L"DirectUIHWND") == 0 ||
             wcscmp(szClass, L"UIItemsView") == 0 ||
             wcscmp(szClass, L"ItemsView") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hChild, TransparentChildSubclassProc, 0);
    }
    else if (wcscmp(szClass, L"TreeView") == 0 ||
             wcscmp(szClass, L"NamespaceTreeControl") == 0)
    {
        if (g_settings.applyToNavPane)
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hChild, NavPaneSubclassProc, 0);
    }
    else if (wcscmp(szClass, L"ReBarWindow32") == 0 ||
             wcscmp(szClass, L"ToolbarWindow32") == 0)
    {
        if (g_settings.applyToCommandBar)
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hChild, TransparentChildSubclassProc, 0);
    }
    else if (wcscmp(szClass, L"UserSettingsBox") == 0 ||
             wcscmp(szClass, L"WorkerW") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hChild, TransparentChildSubclassProc, 0);
    }

    return TRUE;
}

// ==================== 子类化窗口过程 ====================

LRESULT CALLBACK CabinetSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData)
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

LRESULT CALLBACK ShellDefViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam, DWORD_PTR dwRefData)
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
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 1;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TransparentChildSubclassProc(HWND hWnd, UINT uMsg,
                                               WPARAM wParam, LPARAM lParam,
                                               DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT:
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NavPaneSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ==================== CreateWindowExW Hook ====================

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = Real_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName))
    {
        if (wcscmp(lpClassName, L"CabinetWClass") == 0)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWnd, CabinetSubclassProc, 0);
            PostMessageW(hWnd, WM_DEFERRED_INIT, 0, 0);
        }
    }

    return hWnd;
}

// ==================== 处理已存在窗口 ====================

BOOL CALLBACK EnumExistingExplorerProc(HWND hWnd, LPARAM lParam)
{
    (void)lParam;

    DWORD dwProcessId;
    GetWindowThreadProcessId(hWnd, &dwProcessId);

    if (dwProcessId != GetCurrentProcessId())
        return TRUE;

    WCHAR szClass[64];
    if (GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass)) &&
        wcscmp(szClass, L"CabinetWClass") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, CabinetSubclassProc, 0);
        ApplyTransparencyToCabinet(hWnd);
    }

    return TRUE;
}

static void ApplyToExistingExplorerWindows()
{
    EnumWindows(EnumExistingExplorerProc, 0);
}

// ==================== 重新应用设置 ====================

static void ReapplyToAllCabinets()
{
    // 清空记录，ApplyToExistingExplorerWindows 会重新枚举并应用
    g_processedCabinets.clear();
    ApplyToExistingExplorerWindows();
}

// ==================== 清理函数 ====================

BOOL CALLBACK EnumWindowForUnsubclassProc(HWND hChild, LPARAM lParam)
{
    (void)lParam;

    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        hChild, ShellDefViewSubclassProc);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        hChild, ListViewSubclassProc);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        hChild, TransparentChildSubclassProc);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        hChild, NavPaneSubclassProc);

    return TRUE;
}

static void UnsubclassAllWindows()
{
    for (HWND hCabinet : g_processedCabinets)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hCabinet, CabinetSubclassProc);
        EnumChildWindows(hCabinet, EnumWindowForUnsubclassProc, 0);
        InvalidateRect(hCabinet, NULL, TRUE);
    }
    g_processedCabinets.clear();
}

// ==================== Windhawk 入口点 ====================

BOOL Wh_ModInit()
{
    Wh_Log(L"[ExplorerTP] Initializing v2.0.0...");

    DetectWindowsBuild();
    Wh_Log(L"[ExplorerTP] canUseDwmBackdrop=%d", g_canUseDwmBackdrop);

    LoadDwmApis();
    LoadSettings();

    // Hook CreateWindowExW 以捕获新打开的 Explorer 窗口
    // 即使 Hook 失败（避免其它模组占用），已有窗口仍会被处理
    bool hookSucceeded = Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CreateWindowExW_Hook,
        (void**)&Real_CreateWindowExW);

    if (!hookSucceeded)
    {
        Wh_Log(L"[ExplorerTP] Warning: CreateWindowExW hook unavailable");
        Wh_Log(L"[ExplorerTP] New windows won't auto-detect, existing windows will still work");
    }

    ApplyToExistingExplorerWindows();
    return TRUE;
}

void Wh_ModAfterInit()
{
    ApplyToExistingExplorerWindows();
}

void Wh_ModUninit()
{
    // 恢复所有窗口到系统默认背景
    for (HWND hCabinet : g_processedCabinets)
    {
        ClearAccent(hCabinet);
        ResetDwmBackdrop(hCabinet);
        InvalidateRect(hCabinet, NULL, TRUE);
    }

    UnsubclassAllWindows();
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
    ReapplyToAllCabinets();
}
