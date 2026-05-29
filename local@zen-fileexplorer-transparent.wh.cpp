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
{
  "transparencyMode": {
    "type": "combo",
    "default": "mica",
    "options": {
      "mica": "Mica - 壁纸派生背景 (推荐)",
      "micaAlt": "Mica Alt - 更亮的壁纸背景 (22H2+)",
      "clear": "Clear - 无背景效果",
      "blur": "Blur - 丙烯酸模糊效果"
    },
    "description": "文件夹背景透明模式"
  },
  "textColorMode": {
    "type": "combo",
    "default": "default",
    "options": {
      "default": "Default - 系统默认",
      "white": "White - 强制白色文字",
      "dark": "Dark - 强制深灰文字",
      "system": "System - 跟随系统主题"
    },
    "description": "文件列表文字颜色覆盖"
  },
  "applyToNavPane": {
    "type": "bool",
    "default": false,
    "description": "透明效果应用到导航树面板"
  },
  "applyToCommandBar": {
    "type": "bool",
    "default": false,
    "description": "透明效果应用到命令工具栏"
  }
}
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

#include <unordered_set>

// ==================== 窗口消息常量 ====================

#define WM_DEFERRED_INIT (WM_APP + 0x100)

// ==================== Windows 版本检测 ====================

enum class Win11Build {
    kUnknown,
    k21H2,    // 22000
    k22H2,    // 22621
    k23H2,    // 22631
    k24H2,    // 26100
};

Win11Build g_win11Build = Win11Build::kUnknown;
bool g_canUseDwmBackdrop = false;  // DWMWA_SYSTEMBACKDROP_TYPE 可用 (22H2+)

// ntdll 版本号结构体（避免依赖 winternl.h）
struct RTL_OSVERSIONINFOW_TP {
    ULONG dwOSVersionInfoSize;
    ULONG dwMajorVersion;
    ULONG dwMinorVersion;
    ULONG dwBuildNumber;
    ULONG dwPlatformId;
    WCHAR szCSDVersion[128];
};

void DetectWindowsBuild()
{
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return;

    // RtlGetVersion 是 ntdll 中获取真实版本号的可靠方式
    typedef LONG(WINAPI* RtlGetVersion_t)(PVOID);
    auto pRtlGetVersion = (RtlGetVersion_t)GetProcAddress(
        hNtdll, "RtlGetVersion");
    if (!pRtlGetVersion) return;

    RTL_OSVERSIONINFOW_TP osvi = { sizeof(osvi) };
    if (pRtlGetVersion(&osvi) == 0)
    {
        DWORD build = osvi.dwBuildNumber;
        if (build == 22000)
            g_win11Build = Win11Build::k21H2;
        else if (build == 22621)
            g_win11Build = Win11Build::k22H2;
        else if (build == 22631)
            g_win11Build = Win11Build::k23H2;
        else if (build >= 26100)
            g_win11Build = Win11Build::k24H2;
        else if (build >= 22000)
            g_win11Build = Win11Build::k22H2;

        g_canUseDwmBackdrop = (build >= 22621);
    }
}

// ==================== 设置管理 ====================

enum class TransparencyMode {
    kMica,
    kMicaAlt,
    kClear,
    kBlur,
};

enum class TextColorMode {
    kDefault,
    kWhite,
    kDark,
    kSystem,
};

struct {
    TransparencyMode transparencyMode;
    TextColorMode textColorMode;
    bool applyToNavPane;
    bool applyToCommandBar;
} g_settings;

void LoadSettings()
{
    PCWSTR mode = Wh_GetStringSetting(L"transparencyMode");
    g_settings.transparencyMode = TransparencyMode::kMica;
    if (wcscmp(mode, L"micaAlt") == 0)
        g_settings.transparencyMode = TransparencyMode::kMicaAlt;
    else if (wcscmp(mode, L"clear") == 0)
        g_settings.transparencyMode = TransparencyMode::kClear;
    else if (wcscmp(mode, L"blur") == 0)
        g_settings.transparencyMode = TransparencyMode::kBlur;
    Wh_FreeStringSetting(mode);

    PCWSTR textMode = Wh_GetStringSetting(L"textColorMode");
    g_settings.textColorMode = TextColorMode::kDefault;
    if (wcscmp(textMode, L"white") == 0)
        g_settings.textColorMode = TextColorMode::kWhite;
    else if (wcscmp(textMode, L"dark") == 0)
        g_settings.textColorMode = TextColorMode::kDark;
    else if (wcscmp(textMode, L"system") == 0)
        g_settings.textColorMode = TextColorMode::kSystem;
    Wh_FreeStringSetting(textMode);

    g_settings.applyToNavPane = Wh_GetIntSetting(L"applyToNavPane") != 0;
    g_settings.applyToCommandBar = Wh_GetIntSetting(L"applyToCommandBar") != 0;

    Wh_Log(L"[ExplorerTP] Settings: mode=%d textColor=%d nav=%d cmd=%d",
           (int)g_settings.transparencyMode, (int)g_settings.textColorMode,
           g_settings.applyToNavPane, g_settings.applyToCommandBar);
}

// ==================== DWM/ACCENT API 动态加载 ====================

typedef HRESULT(WINAPI* DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
DwmSetWindowAttribute_t pDwmSetWindowAttribute = nullptr;

typedef BOOL(WINAPI* SetWindowCompositionAttribute_t)(HWND, PVOID);
SetWindowCompositionAttribute_t pSetWindowCompositionAttribute = nullptr;

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

// ACCENT_STATE 常量
constexpr DWORD ACCENT_DISABLED = 0;
constexpr DWORD ACCENT_ENABLE_GRADIENT = 1;
constexpr DWORD ACCENT_ENABLE_TRANSPARENTBLURBEHIND = 2;
constexpr DWORD ACCENT_ENABLE_BLURBEHIND = 3;
constexpr DWORD ACCENT_ENABLE_ACRYLICBLURBEHIND = 4;
constexpr DWORD ACCENT_ENABLE_HOSTBACKDROP = 5;

// WCA 常量
constexpr DWORD WCA_ACCENT_POLICY = 19;

// DWMWA 常量 (Win11 22H2+)
constexpr DWORD DWMWA_SYSTEMBACKDROP_TYPE = 102;
constexpr DWORD DWMSBT_DISABLE = 1;
constexpr DWORD DWMSBT_MAINWINDOW = 2;
constexpr DWORD DWMSBT_TABBEDWINDOW = 3;

void LoadDwmApis()
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

void ApplyBackdropEffect(HWND hWnd, TransparencyMode mode)
{
    if (mode == TransparencyMode::kMica && g_canUseDwmBackdrop && pDwmSetWindowAttribute)
    {
        DWORD backdropType = DWMSBT_MAINWINDOW;
        pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                               &backdropType, sizeof(backdropType));
    }
    else if (mode == TransparencyMode::kMicaAlt && g_canUseDwmBackdrop && pDwmSetWindowAttribute)
    {
        DWORD backdropType = DWMSBT_TABBEDWINDOW;
        pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                               &backdropType, sizeof(backdropType));
    }
    else if (mode == TransparencyMode::kClear)
    {
        if (g_canUseDwmBackdrop && pDwmSetWindowAttribute)
        {
            DWORD backdropType = DWMSBT_DISABLE;
            pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));
        }
    }
    else if (mode == TransparencyMode::kBlur)
    {
        if (pSetWindowCompositionAttribute)
        {
            ACCENT_POLICY accent = {
                ACCENT_ENABLE_ACRYLICBLURBEHIND,
                0,
                0x10FFFFFF,
                0
            };
            WINCOMPATTRDATA wcd = { WCA_ACCENT_POLICY, &accent, sizeof(accent) };
            pSetWindowCompositionAttribute(hWnd, &wcd);
        }
        else if (g_canUseDwmBackdrop && pDwmSetWindowAttribute)
        {
            DWORD backdropType = DWMSBT_TABBEDWINDOW;
            pDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));
        }
    }
}

// ==================== 文字颜色工具 ====================

COLORREF GetTextColorForMode()
{
    switch (g_settings.textColorMode)
    {
    case TextColorMode::kWhite:
        return RGB(0xFF, 0xFF, 0xFF);
    case TextColorMode::kDark:
        return RGB(0x33, 0x33, 0x33);
    case TextColorMode::kSystem:
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

bool IsTextColorOverridden()
{
    return g_settings.textColorMode != TextColorMode::kDefault;
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
CreateWindowExW_t Real_CreateWindowExW;

std::unordered_set<HWND> g_processedCabinets;

// ==================== 核心透明化逻辑 ====================

void ApplyTransparencyToCabinet(HWND hCabinet)
{
    if (g_processedCabinets.count(hCabinet))
        return;
    g_processedCabinets.insert(hCabinet);

    Wh_Log(L"[ExplorerTP] Apply transparency to %08X mode=%d",
           (DWORD)(ULONG_PTR)hCabinet, (int)g_settings.transparencyMode);

    ApplyBackdropEffect(hCabinet, g_settings.transparencyMode);
    EnumChildWindows(hCabinet, EnumCabinetChildProc, 0);
    InvalidateRect(hCabinet, NULL, TRUE);
}

BOOL CALLBACK EnumCabinetChildProc(HWND hChild, LPARAM lParam)
{
    (void)lParam;

    WCHAR szClass[128];
    if (!GetClassNameW(hChild, szClass, ARRAYSIZE(szClass)))
        return TRUE;

    if (wcscmp(szClass, L"SHELLDLL_DefView") == 0)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hChild, ShellDefViewSubclassProc, 0);

        // 查找 SysListView32 (22H2/23H2 传统列表视图)
        HWND hListView = FindWindowExW(hChild, NULL, L"SysListView32", NULL);
        if (hListView)
        {
            ListView_SetBkColor(hListView, CLR_NONE);
            ListView_SetTextBkColor(hListView, CLR_NONE);

            if (IsTextColorOverridden())
            {
                ListView_SetTextColor(hListView, GetTextColorForMode());
            }

            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hListView, ListViewSubclassProc, 0);
            InvalidateRect(hListView, NULL, TRUE);
        }

        // 24H2+ 可能用 DirectUIHWND 渲染文件列表
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
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hChild, NavPaneSubclassProc, 0);
        }
    }
    else if (wcscmp(szClass, L"ReBarWindow32") == 0 ||
             wcscmp(szClass, L"ToolbarWindow32") == 0)
    {
        if (g_settings.applyToCommandBar)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hChild, TransparentChildSubclassProc, 0);
        }
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
    case WM_CHANGE:
        if (IsTextColorOverridden())
            ListView_SetTextColor(hWnd, GetTextColorForMode());
        break;
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

void ApplyToExistingExplorerWindows()
{
    EnumWindows(EnumExistingExplorerProc, 0);
}

// ==================== 重新应用设置 ====================

void ReapplyToAllCabinets()
{
    auto cabinets = g_processedCabinets;
    g_processedCabinets.clear();

    for (HWND hCabinet : cabinets)
    {
        ApplyBackdropEffect(hCabinet, g_settings.transparencyMode);
        InvalidateRect(hCabinet, NULL, TRUE);
    }

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

void UnsubclassAllWindows()
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
    Wh_Log(L"[ExplorerTP] Win11 build=%d, canUseDwmBackdrop=%d",
           (int)g_win11Build, g_canUseDwmBackdrop);

    LoadDwmApis();
    LoadSettings();

    if (!Wh_SetFunctionHook(
            (void*)CreateWindowExW,
            (void*)CreateWindowExW_Hook,
            (void**)&Real_CreateWindowExW))
    {
        Wh_Log(L"[ExplorerTP] Failed to hook CreateWindowExW");
        return FALSE;
    }

    ApplyToExistingExplorerWindows();
    Wh_Log(L"[ExplorerTP] Initialized successfully");
    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L"[ExplorerTP] AfterInit - reapplying...");
    ApplyToExistingExplorerWindows();
}

void Wh_ModUninit()
{
    Wh_Log(L"[ExplorerTP] Uninitializing...");
    UnsubclassAllWindows();
    Wh_Log(L"[ExplorerTP] Uninitialized");
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"[ExplorerTP] Settings changed, reloading...");
    LoadSettings();
    ReapplyToAllCabinets();
    Wh_Log(L"[ExplorerTP] Settings applied");
}
