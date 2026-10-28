// ==WindhawkMod==
// @id              zen-desktop-clock
// @name            ZenDesktop: 桌面时钟
// @name:zh-CN      ZenDesktop: 桌面时钟
// @description     A stylish clock and date drawn on the desktop behind the icons: tilt, soft drop shadow, glow, outline and flip-card presets.
// @description:zh-CN 在桌面图标后方显示花样时钟与日期：倾斜、柔和投影、霓虹光晕、描边、翻页卡片等预设。
// @version         1.2.0
// @author          Lanbo, m417z
// @github          https://github.com/Liset999
// @donateUrl       https://ifdian.net/a/Lanboo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ld2d1 -ldwrite -ld3d11 -ldxgi -ldcomp -lshcore -luuid -lole32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The desktop host lookup (WorkerW behind the icons), the DirectComposition
// swap chain set-up and the window life cycle are adapted from m417z's
// "Desktop Live Overlay" (GPL-3.0):
// https://github.com/m417z/my-windhawk-mods · https://windhawk.net/mods/desktop-live-overlay
// Clock layout, presets and effects (rotation, shadow, glow, outline, flip
// cards) are written for ZenDesktop by Lanbo.

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

# ZenDesktop: 桌面时钟

在桌面图标**后方**绘制大号时间与日期，不挡图标、不抢焦点、不吃鼠标。

- **17 种样式预设**：倾斜阴影、极简、霓虹、翻页、大字封面、竖排、描边空心、玻璃卡片、复古数码管、彩虹渐变、日落渐变、中文大写、文字时钟、圆形表盘、进度环、左侧细线、双色错位；另有「自定义」。
- **设置很短**：样式、位置（九宫格 + 微调偏移）、大小、颜色/强调色、24 小时、显示秒、日期格式、资源占用、日历（+农历）、显示器。细调（字体、旋转、投影、光晕、描边、渐变、版式…）都收在「高级」里，只对「自定义」样式生效。
- **资源占用**：可选显示 CPU / 内存占用，每 2 秒刷新（只在开启时采样）；进度环、圆形表盘样式下显示为两个小圆环，其他样式为一行小字。
- **月历**：可放在时钟右侧或下方，今天用强调色高亮，可显示农历（内置 1999–2099 农历表，不联网）。
- 只在分钟（或开了秒时每秒；开了资源占用时每 2 秒）变化时重绘；资源管理器重启后自动恢复。

基于 m417z 的 Desktop Live Overlay（GPL-3.0）的桌面宿主与渲染框架。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: tilted
  $name: "Style"
  $name:zh-CN: "样式"
  $options:
  - tilted: "Tilted shadow"
  - minimal: "Minimal"
  - neon: "Neon glow"
  - flip: "Flip cards"
  - cover: "Magazine cover"
  - stacked: "Stacked"
  - hollow: "Hollow outline"
  - glass: "Glass card"
  - segment: "Retro 7-segment"
  - rainbow: "Rainbow gradient"
  - sunset: "Sunset gradient"
  - chinese: "Chinese numerals"
  - sentence: "Word clock"
  - analog: "Analog dial"
  - ring: "Progress ring"
  - accent: "Accent bar"
  - glitch: "Two-tone glitch"
  - custom: "Custom (Advanced)"
  $options:zh-CN:
  - tilted: "倾斜阴影"
  - minimal: "极简"
  - neon: "霓虹"
  - flip: "翻页"
  - cover: "大字封面"
  - stacked: "竖排"
  - hollow: "描边空心"
  - glass: "玻璃卡片"
  - segment: "复古数码管"
  - rainbow: "彩虹渐变"
  - sunset: "日落渐变"
  - chinese: "中文大写"
  - sentence: "文字时钟"
  - analog: "圆形表盘"
  - ring: "进度环"
  - accent: "左侧细线"
  - glitch: "双色错位"
  - custom: "自定义（用「高级」设置）"
- anchor: center
  $name: "Position"
  $name:zh-CN: "位置"
  $options:
  - topLeft: "Top left"
  - top: "Top"
  - topRight: "Top right"
  - left: "Left"
  - center: "Center"
  - right: "Right"
  - bottomLeft: "Bottom left"
  - bottom: "Bottom"
  - bottomRight: "Bottom right"
  $options:zh-CN:
  - topLeft: "左上"
  - top: "上"
  - topRight: "右上"
  - left: "左"
  - center: "正中"
  - right: "右"
  - bottomLeft: "左下"
  - bottom: "下"
  - bottomRight: "右下"
- offset:
  - x: 0
    $name: "Horizontal"
    $name:zh-CN: "水平（正数向右）"
  - y: -80
    $name: "Vertical"
    $name:zh-CN: "垂直（正数向下）"
  $name: "Fine offset (px at 100%)"
  $name:zh-CN: "微调偏移（像素）"
- size: 100
  $name: "Size (%)"
  $name:zh-CN: "大小（%）"
  $description: "50–300. Scales everything, on top of display scaling."
  $description:zh-CN: "50–300，整体缩放（已自动适配显示器缩放）。"
- color: ""
  $name: "Text color"
  $name:zh-CN: "颜色"
  $description: "Empty = preset color. #RRGGBB or #AARRGGBB."
  $description:zh-CN: "留空用预设颜色；可填 #RRGGBB 或 #AARRGGBB。"
- accentColor: ""
  $name: "Accent color"
  $name:zh-CN: "强调色"
  $description: "Empty = preset. Used by rings, accent bar, second hand, calendar today."
  $description:zh-CN: "留空用预设；用于进度环、竖线、秒针、日历今天。"
- use24Hour: true
  $name: "24-hour time"
  $name:zh-CN: "24 小时制"
- showSeconds: false
  $name: "Show seconds"
  $name:zh-CN: "显示秒"
- dateFormat: cn
  $name: "Date"
  $name:zh-CN: "日期格式"
  $options:
  - cn: "10月28日 周三"
  - cnFull: "2026年10月28日 星期三"
  - iso: "2026-10-28"
  - mdy: "10/28/2026"
  - dmy: "28/10/2026"
  - none: "Hidden"
  $options:zh-CN:
  - cn: "10月28日 周三"
  - cnFull: "2026年10月28日 星期三"
  - iso: "2026-10-28"
  - mdy: "10/28/2026"
  - dmy: "28/10/2026"
  - none: "不显示"
- showResources: false
  $name: "Show CPU / memory usage"
  $name:zh-CN: "显示资源占用（CPU / 内存）"
  $description: "Refreshes every 2 s while on."
  $description:zh-CN: "开启后每 2 秒刷新一次；关闭时不采样。"
- calendar: none
  $name: "Month calendar"
  $name:zh-CN: "显示日历"
  $options:
  - none: "Off"
  - right: "Right of clock"
  - below: "Below clock"
  $options:zh-CN:
  - none: "不显示"
  - right: "时钟右侧"
  - below: "时钟下方"
- lunar: true
  $name: "Lunar dates in calendar"
  $name:zh-CN: "日历显示农历"
- monitor: 1
  $name: "Monitor"
  $name:zh-CN: "显示器"
  $description: "1 = first, 2 = second… 0 = all."
  $description:zh-CN: "1 = 第一个，2 = 第二个……0 = 全部。"
- advanced:
  - layout: normal
    $name: "Layout"
    $name:zh-CN: "版式"
    $options:
    - normal: "Time + date"
    - stacked: "Hours over minutes"
    - cover: "Date above"
    - sentence: "Sentence"
    - chinese: "Chinese numerals"
    - analog: "Analog dial"
    - ring: "Progress ring"
    $options:zh-CN:
    - normal: "时间 + 日期"
    - stacked: "竖排"
    - cover: "日期在上"
    - sentence: "文字句子"
    - chinese: "中文大写"
    - analog: "圆形表盘"
    - ring: "进度环"
  - timeFont: "Segoe UI Variable Display"
    $name:zh-CN: "时间字体"
  - timeFontSize: 140
    $name:zh-CN: "时间字号"
  - timeFontWeight: 600
    $name:zh-CN: "时间字重（100–900）"
  - dateFont: "Segoe UI Variable Display"
    $name:zh-CN: "日期字体"
  - dateFontSize: 40
    $name:zh-CN: "日期字号"
  - dateFontWeight: 500
    $name:zh-CN: "日期字重"
  - textAlign: left
    $name:zh-CN: "行对齐"
    $options:
    - left: "Left"
    - center: "Center"
    - right: "Right"
    $options:zh-CN:
    - left: "左"
    - center: "中"
    - right: "右"
  - lineGap: 4
    $name:zh-CN: "行间距"
  - rotation: -8
    $name:zh-CN: "旋转角度（-45~45）"
  - shadow: true
    $name:zh-CN: "投影"
  - shadowColor: "#B3000000"
    $name:zh-CN: "投影颜色（#AARRGGBB，AA 为不透明度）"
  - shadowBlur: 8
    $name:zh-CN: "投影模糊"
  - shadowOffsetX: 6
    $name:zh-CN: "投影水平偏移"
  - shadowOffsetY: 8
    $name:zh-CN: "投影垂直偏移"
  - glow: false
    $name:zh-CN: "光晕"
  - glowColor: "#00E5FF"
    $name:zh-CN: "光晕颜色"
  - glowRadius: 12
    $name:zh-CN: "光晕半径"
  - outline: false
    $name:zh-CN: "描边"
  - outlineColor: "#000000"
    $name:zh-CN: "描边颜色"
  - outlineWidth: 2
    $name:zh-CN: "描边宽度"
  - hollow: false
    $name:zh-CN: "空心字（只留描边）"
  - gradientStart: ""
    $name:zh-CN: "渐变起始色（两个都填才启用）"
  - gradientEnd: ""
    $name:zh-CN: "渐变结束色"
  - flipCards: false
    $name:zh-CN: "翻页卡片底"
  - cardColor: "#E0202024"
    $name:zh-CN: "翻页卡片颜色"
  - glassCard: false
    $name:zh-CN: "玻璃卡片底"
  - accentBar: false
    $name:zh-CN: "左侧竖线"
  - showWeekday: false
    $name:zh-CN: "日期加星期"
  $name: "Advanced (Custom style only)"
  $name:zh-CN: "高级（仅「自定义」样式生效）"
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwrite.h>
#include <dxgi1_3.h>
#include <shellscalingapi.h>
#include <wrl/client.h>
#include <dwrite_1.h>

#include <atomic>
#include <cmath>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

#define ZC_TIMER_REFRESH 1
#define ZC_TIMER_MSG_DISPLAY 1
#define ZC_TIMER_MSG_RECREATE 2
#define ZC_WM_CLEANUP (WM_APP + 1)
#define ZC_WM_SETTINGS (WM_APP + 2)
#define ZC_OVERLAY_CLASS (L"ZenDesktopClock_" WH_MOD_ID)
#define ZC_MESSAGE_CLASS (L"ZenDesktopClockMsg_" WH_MOD_ID)

// D2D built-in effect CLSIDs (defined locally so no dxguid import is needed).
// CLSID_D2D1Shadow {C67EA361-1863-4E69-89DB-695D3E9A5B6B}
static const CLSID kZcClsidShadow = {
    0xc67ea361, 0x1863, 0x4e69, {0x89, 0xdb, 0x69, 0x5d, 0x3e, 0x9a, 0x5b, 0x6b}};
// CLSID_D2D1Morphology {EAE6C40D-626A-4C2D-BFCB-391001ABE202}
static const CLSID kZcClsidMorphology = {
    0xeae6c40d, 0x626a, 0x4c2d, {0xbf, 0xcb, 0x39, 0x10, 0x01, 0xab, 0xe2, 0x02}};
// Property indices (D2D1_SHADOW_PROP / D2D1_MORPHOLOGY_PROP).
enum : UINT32 {
    kZcShadowPropBlur = 0,
    kZcShadowPropColor = 1,
    kZcMorphPropMode = 0,
    kZcMorphPropWidth = 1,
    kZcMorphPropHeight = 2,
    kZcMorphModeDilate = 1,
};

enum class ZcDate { mdy, dmy, iso, cn, cnFull, none };
enum class ZcAlign { left, center, right };

enum class ZcLayoutKind { normal, stacked, cover, sentence, chinese, analog, ring };

struct ZcStyle {
    std::wstring timeFont, dateFont;
    int timeSize = 140, timeWeight = 600, dateSize = 40, dateWeight = 500;
    D2D1_COLOR_F text{1, 1, 1, 1};
    float rotation = 0;
    bool shadow = false;
    D2D1_COLOR_F shadowColor{0, 0, 0, 1};
    float shadowBlur = 8, shadowDx = 6, shadowDy = 8;
    bool glow = false;
    D2D1_COLOR_F glowColor{0, 0.9f, 1, 1};
    float glowRadius = 12;
    bool outline = false;
    D2D1_COLOR_F outlineColor{0, 0, 0, 1};
    float outlineWidth = 2;
    bool flip = false;
    D2D1_COLOR_F cardColor{0.125f, 0.125f, 0.14f, 0.88f};
    // 1.1.0 additions.
    ZcLayoutKind layout = ZcLayoutKind::normal;
    bool hollow = false;              // outline only, transparent fill
    std::vector<D2D1_COLOR_F> grad;   // >= 2 stops: gradient text fill
    bool glass = false;               // translucent rounded card behind block
    D2D1_COLOR_F glassColor{1, 1, 1, 0.16f};
    D2D1_COLOR_F glassBorder{1, 1, 1, 0.38f};
    bool ghost = false;               // dim "88:88" behind the digits
    D2D1_COLOR_F ghostColor{1, 1, 1, 0.08f};
    bool accentBar = false;           // vertical bar on the left
    D2D1_COLOR_F accent{1, 0.42f, 0.2f, 1};
    bool weekday = false;             // force weekday into the date line
    bool dup = false;                 // two-tone offset duplicates
    D2D1_COLOR_F dupA{0, 0.9f, 1, 0.85f}, dupB{1, 0.1f, 0.55f, 0.85f};
    float dupOffset = 5;
    bool dateAbove = false;
    float dateTracking = 0;           // extra letter spacing for the date
    bool upper = false;               // uppercase date text
    D2D1_COLOR_F track{1, 1, 1, 0.18f};  // ring track / dial face line
};

struct ZcSettings {
    bool use24 = true, seconds = false;
    ZcDate date = ZcDate::mdy;
    ZcAlign align = ZcAlign::left;
    int anchor = 4;  // 0..8, row-major 3x3
    int offsetX = 0, offsetY = -80, monitor = 1, lineGap = 4;
    int size = 100;      // user scale, %
    bool resources = false;
    int calendar = 0;    // 0 off, 1 right, 2 below
    bool lunar = true;
    ZcStyle style;
} g_zc;

// Latest CPU / memory load in percent (sampled only when shown).
float g_zcCpu = 0, g_zcMem = 0;
ULONGLONG g_zcPrevIdle = 0, g_zcPrevTotal = 0;

ULONGLONG ZcFt(const FILETIME& f) {
    return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime;
}

void ZcSampleResources() {
    FILETIME idle, kernel, user;
    if (GetSystemTimes(&idle, &kernel, &user)) {
        ULONGLONG i = ZcFt(idle), total = ZcFt(kernel) + ZcFt(user);  // kernel includes idle
        if (g_zcPrevTotal && total > g_zcPrevTotal) {
            double busy = 1.0 - (double)(i - g_zcPrevIdle) / (double)(total - g_zcPrevTotal);
            g_zcCpu = (float)(busy < 0 ? 0 : busy > 1 ? 100 : busy * 100);
        }
        g_zcPrevIdle = i;
        g_zcPrevTotal = total;
    }
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms)) g_zcMem = (float)ms.dwMemoryLoad;
}

std::atomic<bool> g_zcUnloading{false};
std::atomic<bool> g_zcLazyInit{false};
std::atomic<bool> g_zcInitOk{false};
HWND g_zcOverlayWnd = nullptr;
HWND g_zcMessageWnd = nullptr;
UINT_PTR g_zcCreateTimer = 0;
UINT g_zcTaskbarCreatedMsg = 0;

[[clang::no_destroy]] ComPtr<ID3D11Device> g_zcD3d;
[[clang::no_destroy]] ComPtr<IDXGIDevice> g_zcDxgiDevice;
[[clang::no_destroy]] ComPtr<IDXGIFactory2> g_zcDxgiFactory;
[[clang::no_destroy]] ComPtr<ID2D1Factory1> g_zcD2dFactory;
[[clang::no_destroy]] ComPtr<ID2D1Device> g_zcD2dDevice;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_zcDwrite;
[[clang::no_destroy]] ComPtr<IDXGISwapChain1> g_zcSwapChain;
[[clang::no_destroy]] ComPtr<ID2D1DeviceContext> g_zcDc;
[[clang::no_destroy]] ComPtr<ID2D1Bitmap1> g_zcTarget;
[[clang::no_destroy]] ComPtr<IDCompositionDevice> g_zcCompDevice;
[[clang::no_destroy]] ComPtr<IDCompositionTarget> g_zcCompTarget;
[[clang::no_destroy]] ComPtr<IDCompositionVisual> g_zcCompVisual;

////////////////////////////////////////////////////////////////////////////////
// Utilities (RunFromWindowThread / GetCurrentModuleHandle / FolderView and
// WorkerW lookup adapted from m417z's Desktop Live Overlay, GPL-3.0)

using ZcThreadProc_t = void(WINAPI*)(void*);

bool ZcRunFromWindowThread(HWND hWnd, ZcThreadProc_t proc, void* procParam) {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param {
        ZcThreadProc_t proc;
        void* procParam;
    };
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == msg) {
                    Param* p = (Param*)cwp->lParam;
                    p->proc(p->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }
    Param p{proc, procParam};
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

HMODULE ZcModule() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&ZcModule, &module);
    return module;
}

bool ZcIsFolderView(HWND hWnd) {
    WCHAR buf[64];
    if (!GetClassName(hWnd, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"SysListView32")) {
        return false;
    }
    if (!GetWindowText(hWnd, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"FolderView")) {
        return false;
    }
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    if (!parent || !GetClassName(parent, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"SHELLDLL_DefView") || GetWindowTextLength(parent) > 0) {
        return false;
    }
    HWND parent2 = GetAncestor(parent, GA_PARENT);
    if (!parent2) {
        return false;
    }
    if ((!GetClassName(parent2, buf, ARRAYSIZE(buf)) ||
         _wcsicmp(buf, L"Progman")) &&
        parent2 != GetShellWindow()) {
        return false;
    }
    return true;
}

HWND ZcGetProgman() {
    HWND progman = FindWindow(L"Progman", nullptr);
    if (!progman) {
        return nullptr;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(progman, &pid);
    return pid == GetCurrentProcessId() ? progman : nullptr;
}

BOOL CALLBACK ZcFindWorkerProc(HWND hWnd, LPARAM lParam) {
    if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
        return TRUE;
    }
    HWND worker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
    if (worker) {
        *(HWND*)lParam = worker;
        return FALSE;
    }
    return TRUE;
}

// The WorkerW window that sits behind the desktop icons.
HWND ZcGetWorkerW() {
    HWND progman = ZcGetProgman();
    if (!progman) {
        return nullptr;
    }
    SendMessage(progman, 0x052C, 0xD, 0);
    SendMessage(progman, 0x052C, 0xD, 1);
    HWND worker = nullptr;
    EnumWindows(ZcFindWorkerProc, (LPARAM)&worker);
    if (!worker) {
        SendMessage(progman, 0x052C, 0, 0);
        EnumWindows(ZcFindWorkerProc, (LPARAM)&worker);
    }
    if (!worker) {
        // 24H2+: WorkerW is a child of Progman.
        worker = FindWindowEx(progman, nullptr, L"WorkerW", nullptr);
    }
    return worker ? worker : progman;
}

float ZcMonitorScale(HMONITOR monitor) {
    UINT dx = 96, dy = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dx, &dy))) {
        return dx / 96.0f;
    }
    return 1.0f;
}

BOOL CALLBACK ZcEnumMonitorProc(HMONITOR m, HDC, LPRECT, LPARAM lParam) {
    ((std::vector<HMONITOR>*)lParam)->push_back(m);
    return TRUE;
}

std::vector<HMONITOR> ZcTargetMonitors() {
    std::vector<HMONITOR> all;
    EnumDisplayMonitors(nullptr, nullptr, ZcEnumMonitorProc, (LPARAM)&all);
    if (g_zc.monitor <= 0) {
        return all;
    }
    if (g_zc.monitor <= (int)all.size()) {
        return {all[g_zc.monitor - 1]};
    }
    return {MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)};
}

bool ZcParseColor(PCWSTR s, D2D1_COLOR_F* out) {
    if (!s) {
        return false;
    }
    if (*s == L'#') {
        s++;
    }
    size_t len = wcslen(s);
    if (len != 6 && len != 8) {
        return false;
    }
    WCHAR* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (!end || *end) {
        return false;
    }
    unsigned a = len == 8 ? (v >> 24) & 0xFF : 0xFF;
    out->r = ((v >> 16) & 0xFF) / 255.0f;
    out->g = ((v >> 8) & 0xFF) / 255.0f;
    out->b = (v & 0xFF) / 255.0f;
    out->a = a / 255.0f;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

std::wstring ZcStr(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

D2D1_COLOR_F ZcColor(PCWSTR name, D2D1_COLOR_F def) {
    std::wstring s = ZcStr(name);
    D2D1_COLOR_F c = def;
    if (!ZcParseColor(s.c_str(), &c)) {
        c = def;
    }
    return c;
}

int ZcClamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Chinese font with fallbacks (first installed family wins).
std::wstring ZcPickFont(std::initializer_list<const wchar_t*> names) {
    ComPtr<IDWriteFontCollection> fc;
    if (g_zcDwrite && SUCCEEDED(g_zcDwrite->GetSystemFontCollection(&fc))) {
        for (auto n : names) {
            UINT32 idx;
            BOOL exists = FALSE;
            if (SUCCEEDED(fc->FindFamilyName(n, &idx, &exists)) && exists) {
                return n;
            }
        }
    }
    return *names.begin();
}

void ZcApplyPreset(const std::wstring& name, ZcStyle& s) {
    const wchar_t* kFont = L"Segoe UI Variable Display";
    s = ZcStyle{};
    s.timeFont = kFont;
    s.dateFont = kFont;
    auto softShadow = [&](float a, float blur, float dx, float dy) {
        s.shadow = true;
        s.shadowColor = {0, 0, 0, a};
        s.shadowBlur = blur;
        s.shadowDx = dx;
        s.shadowDy = dy;
    };
    if (name == L"tilted") {
        // White large text, tilted ~-8 degrees, dark soft shadow.
        s.timeSize = 150;
        s.timeWeight = 700;
        s.dateSize = 44;
        s.dateWeight = 600;
        s.rotation = -8;
        softShadow(0.75f, 10, 6, 10);
    } else if (name == L"minimal") {
        s.timeFont = L"Segoe UI Variable Display Light";
        s.timeSize = 120;
        s.timeWeight = 200;
        s.dateSize = 30;
        s.dateWeight = 400;
        s.text = {1, 1, 1, 0.92f};
        softShadow(0.35f, 4, 0, 2);
    } else if (name == L"neon") {
        s.timeFont = s.dateFont = L"Bahnschrift";
        s.timeSize = 130;
        s.timeWeight = 600;
        s.dateSize = 38;
        s.dateWeight = 500;
        s.text = {0.88f, 1, 1, 1};
        s.glow = true;
        s.glowColor = {0, 0.9f, 1, 1};
        s.glowRadius = 14;
    } else if (name == L"flip") {
        s.timeFont = L"Bahnschrift";
        s.timeSize = 120;
        s.timeWeight = 700;
        s.dateSize = 34;
        s.dateWeight = 600;
        s.flip = true;
        s.cardColor = {0.11f, 0.11f, 0.125f, 0.92f};
        softShadow(0.6f, 8, 0, 6);
    } else if (name == L"cover") {
        // Magazine cover: huge hairline time, tiny tracked date above.
        s.timeFont = L"Segoe UI Variable Display Light";
        s.timeSize = 220;
        s.timeWeight = 100;
        s.dateSize = 22;
        s.dateWeight = 600;
        s.dateAbove = true;
        s.dateTracking = 6;
        s.upper = true;
        s.weekday = true;
        softShadow(0.3f, 6, 0, 3);
    } else if (name == L"stacked") {
        s.timeFont = L"Bahnschrift";
        s.timeSize = 150;
        s.timeWeight = 700;
        s.dateSize = 30;
        s.dateWeight = 500;
        s.layout = ZcLayoutKind::stacked;
        softShadow(0.5f, 8, 0, 5);
    } else if (name == L"hollow") {
        s.timeFont = L"Segoe UI Variable Display";
        s.timeSize = 160;
        s.timeWeight = 800;
        s.dateSize = 40;
        s.dateWeight = 700;
        s.hollow = true;
        s.outline = true;
        s.outlineColor = {1, 1, 1, 1};
        s.outlineWidth = 2;
    } else if (name == L"glass") {
        s.timeSize = 120;
        s.timeWeight = 600;
        s.dateSize = 32;
        s.dateWeight = 500;
        s.glass = true;
        s.weekday = true;
        softShadow(0.25f, 6, 0, 3);
    } else if (name == L"segment") {
        // Retro 7-segment: condensed tabular digits over a dim 88:88 ghost.
        s.timeFont = s.dateFont = L"Bahnschrift Condensed";
        s.timeSize = 140;
        s.timeWeight = 400;
        s.dateSize = 32;
        s.dateWeight = 400;
        s.text = {1, 0.23f, 0.19f, 1};
        s.ghost = true;
        s.ghostColor = {1, 0.23f, 0.19f, 0.12f};
        s.glow = true;
        s.glowColor = {1, 0.1f, 0.05f, 0.9f};
        s.glowRadius = 10;
    } else if (name == L"rainbow" || name == L"sunset") {
        s.timeSize = 150;
        s.timeWeight = 800;
        s.dateSize = 40;
        s.dateWeight = 700;
        if (name == L"rainbow") {
            s.grad = {{1, 0.23f, 0.19f, 1}, {1, 0.8f, 0, 1}, {0.2f, 0.85f, 0.35f, 1},
                      {0, 0.6f, 1, 1},      {0.69f, 0.32f, 0.87f, 1}};
        } else {
            s.grad = {{1, 0.37f, 0.43f, 1}, {1, 0.6f, 0.2f, 1}, {1, 0.85f, 0.4f, 1}};
        }
        softShadow(0.4f, 8, 0, 5);
    } else if (name == L"chinese") {
        s.timeFont = s.dateFont = ZcPickFont({L"KaiTi", L"STKaiti", L"楷体",
                                              L"Microsoft YaHei"});
        s.timeSize = 96;
        s.timeWeight = 400;
        s.dateSize = 40;
        s.dateWeight = 400;
        s.layout = ZcLayoutKind::chinese;
        softShadow(0.55f, 6, 3, 4);
    } else if (name == L"sentence") {
        s.timeFont = s.dateFont = L"Microsoft YaHei UI Light";
        s.timeSize = 64;
        s.timeWeight = 300;
        s.dateSize = 26;
        s.dateWeight = 400;
        s.layout = ZcLayoutKind::sentence;
        s.weekday = true;
        softShadow(0.45f, 6, 0, 3);
    } else if (name == L"analog") {
        s.timeSize = 120;  // dial diameter ~ 2.2x
        s.timeWeight = 400;
        s.dateSize = 28;
        s.dateWeight = 500;
        s.layout = ZcLayoutKind::analog;
        softShadow(0.45f, 8, 0, 4);
    } else if (name == L"ring") {
        s.timeSize = 90;
        s.timeWeight = 600;
        s.dateSize = 26;
        s.dateWeight = 500;
        s.layout = ZcLayoutKind::ring;
        s.accent = {0.3f, 0.85f, 1, 1};
        softShadow(0.35f, 6, 0, 3);
    } else if (name == L"accent") {
        s.timeSize = 120;
        s.timeWeight = 300;
        s.dateSize = 30;
        s.dateWeight = 600;
        s.accentBar = true;
        s.weekday = true;
        softShadow(0.35f, 5, 0, 3);
    } else if (name == L"glitch") {
        s.timeFont = s.dateFont = L"Bahnschrift";
        s.timeSize = 140;
        s.timeWeight = 800;
        s.dateSize = 36;
        s.dateWeight = 700;
        s.dup = true;
        s.dupOffset = 5;
    }
}

void ZcLoadSettings() {
    ZcSettings z;
    z.use24 = Wh_GetIntSetting(L"use24Hour") != 0;
    z.seconds = Wh_GetIntSetting(L"showSeconds") != 0;

    std::wstring d = ZcStr(L"dateFormat");
    z.date = d == L"dmy"      ? ZcDate::dmy
             : d == L"iso"    ? ZcDate::iso
             : d == L"mdy"    ? ZcDate::mdy
             : d == L"cnFull" ? ZcDate::cnFull
             : d == L"none"   ? ZcDate::none
                              : ZcDate::cn;
    static const wchar_t* kAnchors[] = {L"topLeft", L"top",        L"topRight",
                                        L"left",    L"center",     L"right",
                                        L"bottomLeft", L"bottom", L"bottomRight"};
    std::wstring an = ZcStr(L"anchor");
    z.anchor = 4;
    for (int i = 0; i < 9; i++) {
        if (an == kAnchors[i]) z.anchor = i;
    }
    z.offsetX = Wh_GetIntSetting(L"offset.x");
    z.offsetY = Wh_GetIntSetting(L"offset.y");
    z.size = ZcClamp(Wh_GetIntSetting(L"size"), 50, 300);
    if (!Wh_GetIntSetting(L"size")) z.size = 100;
    z.monitor = Wh_GetIntSetting(L"monitor");
    z.resources = Wh_GetIntSetting(L"showResources") != 0;
    std::wstring cal = ZcStr(L"calendar");
    z.calendar = cal == L"right" ? 1 : cal == L"below" ? 2 : 0;
    z.lunar = Wh_GetIntSetting(L"lunar") != 0;

    ZcStyle& s = z.style;
    std::wstring style = ZcStr(L"style");
    if (style != L"custom") {
        ZcApplyPreset(style.empty() ? L"tilted" : style, s);
    } else {
        auto A = [](const wchar_t* k) { return std::wstring(L"advanced.") + k; };
        auto I = [&](const wchar_t* k) { return Wh_GetIntSetting(A(k).c_str()); };
        auto S = [&](const wchar_t* k) { return ZcStr(A(k).c_str()); };
        auto C = [&](const wchar_t* k, D2D1_COLOR_F def) {
            return ZcColor(A(k).c_str(), def);
        };
        s.timeFont = S(L"timeFont");
        s.dateFont = S(L"dateFont");
        if (s.timeFont.empty()) s.timeFont = L"Segoe UI Variable Display";
        if (s.dateFont.empty()) s.dateFont = s.timeFont;
        s.timeSize = I(L"timeFontSize");
        s.timeWeight = I(L"timeFontWeight");
        s.dateSize = I(L"dateFontSize");
        s.dateWeight = I(L"dateFontWeight");
        std::wstring a = S(L"textAlign");
        z.align = a == L"center" ? ZcAlign::center
                  : a == L"right" ? ZcAlign::right
                                  : ZcAlign::left;
        z.lineGap = ZcClamp(I(L"lineGap"), -200, 400);
        s.rotation = (float)ZcClamp(I(L"rotation"), -45, 45);
        s.shadow = I(L"shadow") != 0;
        s.shadowColor = C(L"shadowColor", {0, 0, 0, 0.7f});
        s.shadowBlur = (float)ZcClamp(I(L"shadowBlur"), 0, 100);
        s.shadowDx = (float)I(L"shadowOffsetX");
        s.shadowDy = (float)I(L"shadowOffsetY");
        s.glow = I(L"glow") != 0;
        s.glowColor = C(L"glowColor", {0, 0.9f, 1, 1});
        s.glowRadius = (float)ZcClamp(I(L"glowRadius"), 1, 100);
        s.outline = I(L"outline") != 0;
        s.outlineColor = C(L"outlineColor", {0, 0, 0, 1});
        s.outlineWidth = (float)ZcClamp(I(L"outlineWidth"), 1, 20);
        s.hollow = I(L"hollow") != 0;
        D2D1_COLOR_F g0, g1;
        if (ZcParseColor(S(L"gradientStart").c_str(), &g0) &&
            ZcParseColor(S(L"gradientEnd").c_str(), &g1)) {
            s.grad = {g0, g1};
        }
        s.flip = I(L"flipCards") != 0;
        s.cardColor = C(L"cardColor", {0.125f, 0.125f, 0.14f, 0.88f});
        s.glass = I(L"glassCard") != 0;
        s.accentBar = I(L"accentBar") != 0;
        s.weekday = I(L"showWeekday") != 0;
        std::wstring lay = S(L"layout");
        s.layout = lay == L"stacked"    ? ZcLayoutKind::stacked
                   : lay == L"sentence" ? ZcLayoutKind::sentence
                   : lay == L"chinese"  ? ZcLayoutKind::chinese
                   : lay == L"analog"   ? ZcLayoutKind::analog
                   : lay == L"ring"     ? ZcLayoutKind::ring
                                        : ZcLayoutKind::normal;
        s.dateAbove = lay == L"cover";
    }
    // Top-level color overrides apply to every style.
    D2D1_COLOR_F c;
    if (ZcParseColor(ZcStr(L"color").c_str(), &c)) {
        s.text = c;
        s.grad.clear();
    }
    if (ZcParseColor(ZcStr(L"accentColor").c_str(), &c)) s.accent = c;
    s.timeSize = ZcClamp(s.timeSize ? s.timeSize : 140, 8, 600);
    s.dateSize = ZcClamp(s.dateSize ? s.dateSize : 40, 6, 300);
    s.timeWeight = ZcClamp(s.timeWeight ? s.timeWeight : 400, 100, 950);
    s.dateWeight = ZcClamp(s.dateWeight ? s.dateWeight : 400, 100, 950);

    g_zc = z;
    Wh_Log(L"settings: style=%s size=%d seconds=%d res=%d cal=%d lunar=%d monitor=%d",
           style.c_str(), z.size, (int)z.seconds, (int)z.resources, z.calendar,
           (int)z.lunar, z.monitor);
}

////////////////////////////////////////////////////////////////////////////////
// Text

void ZcFormat(const SYSTEMTIME& t, std::wstring& hh, std::wstring& mm,
              std::wstring& ss, std::wstring& suffix, std::wstring& date) {
    static const wchar_t* kWeekShort[] = {L"周日", L"周一", L"周二", L"周三",
                                          L"周四", L"周五", L"周六"};
    static const wchar_t* kWeekLong[] = {L"星期日", L"星期一", L"星期二",
                                         L"星期三", L"星期四", L"星期五",
                                         L"星期六"};
    WCHAR b[64];
    int hour = t.wHour;
    suffix.clear();
    if (!g_zc.use24) {
        suffix = hour < 12 ? L"AM" : L"PM";
        hour %= 12;
        if (hour == 0) hour = 12;
    }
    swprintf_s(b, g_zc.use24 ? L"%02d" : L"%d", hour);
    hh = b;
    swprintf_s(b, L"%02d", t.wMinute);
    mm = b;
    swprintf_s(b, L"%02d", t.wSecond);
    ss = g_zc.seconds ? b : L"";
    int wd = t.wDayOfWeek % 7;
    switch (g_zc.date) {
        case ZcDate::mdy:
            swprintf_s(b, L"%02d/%02d/%04d", t.wMonth, t.wDay, t.wYear);
            break;
        case ZcDate::dmy:
            swprintf_s(b, L"%02d/%02d/%04d", t.wDay, t.wMonth, t.wYear);
            break;
        case ZcDate::iso:
            swprintf_s(b, L"%04d-%02d-%02d", t.wYear, t.wMonth, t.wDay);
            break;
        case ZcDate::cn:
            swprintf_s(b, L"%d月%d日 %s", t.wMonth, t.wDay, kWeekShort[wd]);
            break;
        case ZcDate::cnFull:
            swprintf_s(b, L"%d年%d月%d日 %s", t.wYear, t.wMonth, t.wDay,
                       kWeekLong[wd]);
            break;
        case ZcDate::none:
            b[0] = 0;
            break;
    }
    date = b;
}

ComPtr<IDWriteTextLayout> ZcLayout(const std::wstring& text,
                                   const std::wstring& font,
                                   int weight,
                                   float size) {
    ComPtr<IDWriteTextFormat> fmt;
    if (FAILED(g_zcDwrite->CreateTextFormat(
            font.c_str(), nullptr, (DWRITE_FONT_WEIGHT)weight,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size,
            L"zh-CN", &fmt))) {
        // Font name invalid: fall back to Segoe UI.
        if (FAILED(g_zcDwrite->CreateTextFormat(
                L"Segoe UI", nullptr, (DWRITE_FONT_WEIGHT)weight,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size,
                L"zh-CN", &fmt))) {
            return nullptr;
        }
    }
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(g_zcDwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                            fmt.Get(), 100000.0f, 100000.0f,
                                            &layout))) {
        return nullptr;
    }
    return layout;
}

D2D1_SIZE_F ZcMeasure(IDWriteTextLayout* layout) {
    DWRITE_TEXT_METRICS m{};
    if (layout) layout->GetMetrics(&m);
    return D2D1::SizeF(m.widthIncludingTrailingWhitespace, m.height);
}

struct ZcPiece {
    ComPtr<IDWriteTextLayout> layout;
    D2D1_POINT_2F pos;
    D2D1_SIZE_F size;
    bool card;
    bool ghost = false;  // decorative "88:88" drawn only in the main pass
    bool dim = false;    // secondary text (weekend, lunar, headers)
};

// Everything needed to draw one clock block at the origin.
struct ZcFrame {
    std::vector<ZcPiece> pieces;
    D2D1_SIZE_F size{};
    float scale = 1;
    SYSTEMTIME t{};
    bool dial = false;  // analog face or progress ring
    D2D1_POINT_2F dialC{};
    float dialR = 0;
    bool hasBar = false;
    D2D1_RECT_F bar{};
    D2D1_RECT_F glassRect{};
    struct Mini { D2D1_POINT_2F c; float r, frac; };
    std::vector<Mini> minis;  // small resource rings
    bool calHi = false;       // calendar "today" highlight
    D2D1_RECT_F calHiRect{};
};

// Lunar years 1999..2099: {days from 1999-01-01 to 正月初一, leap month (0 none), bit i = month i (in order, leap included) has 30 days}.
static const struct { int day; unsigned char leap; unsigned short big; } kZcLunar[] = {
    {46,0,0x0749}, {400,0,0x0693}, {754,4,0x152b}, {1138,0,0x052b}, {1492,0,0x0a5b}, {1847,2,0x155a},
    {2231,0,0x056a}, {2585,7,0x1b55}, {2970,0,0x0ba4}, {3324,0,0x0b49}, {3678,5,0x1a93}, {4062,0,0x0a95},
    {4416,0,0x052d}, {4770,4,0x0aad}, {5154,0,0x0ab5}, {5509,9,0x15aa}, {5893,0,0x05d2}, {6247,0,0x0da5},
    {6602,6,0x1d4a}, {6986,0,0x0d4a}, {7340,0,0x0c95}, {7694,4,0x152e}, {8078,0,0x0556}, {8432,0,0x0ab5},
    {8787,2,0x15b2}, {9171,0,0x06d2}, {9525,6,0x0ea5}, {9909,0,0x0725}, {10263,0,0x064b}, {10617,5,0x0c97},
    {11001,0,0x0cab}, {11356,0,0x055a}, {11710,3,0x0ad6}, {12094,0,0x0b69}, {12449,11,0x1752}, {12833,0,0x0b52},
    {13187,0,0x0b25}, {13541,6,0x1a4b}, {13925,0,0x0a4b}, {14279,0,0x04ab}, {14633,5,0x055b}, {15017,0,0x05ad},
    {15372,0,0x0b6a}, {15727,2,0x1b52}, {16111,0,0x0d92}, {16465,7,0x1d25}, {16849,0,0x0d25}, {17203,0,0x0a55},
    {17557,5,0x14ad}, {17941,0,0x04b6}, {18295,0,0x05b5}, {18650,3,0x0daa}, {19034,0,0x0ec9}, {19389,8,0x1e92},
    {19773,0,0x0e92}, {20127,0,0x0d26}, {20481,6,0x0a56}, {20864,0,0x0a57}, {21219,0,0x0556}, {21573,4,0x06d5},
    {21957,0,0x0755}, {22312,0,0x0749}, {22666,3,0x0e93}, {23050,0,0x0693}, {23404,7,0x152b}, {23788,0,0x052b},
    {24142,0,0x0a5b}, {24497,5,0x155a}, {24881,0,0x056a}, {25235,0,0x0b65}, {25590,4,0x174a}, {25974,0,0x0b4a},
    {26328,8,0x1a95}, {26712,0,0x0a95}, {27066,0,0x052d}, {27420,6,0x0aad}, {27804,0,0x0ab5}, {28159,0,0x05aa},
    {28513,4,0x0ba5}, {28897,0,0x0da5}, {29252,0,0x0d4a}, {29606,3,0x1c95}, {29990,0,0x0c96}, {30344,7,0x194e},
    {30728,0,0x0556}, {31082,0,0x0ab5}, {31437,5,0x15b2}, {31821,0,0x06d2}, {32175,0,0x0ea5}, {32530,4,0x0e4a},
    {32913,0,0x068b}, {33267,8,0x0c97}, {33651,0,0x04ab}, {34005,0,0x055b}, {34360,6,0x0ad6}, {34744,0,0x0b6a},
    {35099,0,0x0752}, {35453,4,0x1725}, {35837,0,0x0b45}, {36191,0,0x0a8b}, {36545,2,0x149b},
};

int ZcDaysFromCivil(int y, int m, int d) {
    y -= m <= 2;
    int era = (y >= 0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

// Solar date -> lunar month/day. Returns false outside 1999-02..2099.
bool ZcLunarDate(int y, int m, int d, int* lm, int* ld, bool* leap) {
    int days = ZcDaysFromCivil(y, m, d) - ZcDaysFromCivil(1999, 1, 1);
    int n = (int)(sizeof(kZcLunar) / sizeof(kZcLunar[0]));
    int i = n - 1;
    while (i >= 0 && kZcLunar[i].day > days) i--;
    if (i < 0) return false;
    int off = days - kZcLunar[i].day;
    int lp = kZcLunar[i].leap, count = lp ? 13 : 12;
    for (int k = 0; k < count; k++) {
        int len = 29 + ((kZcLunar[i].big >> k) & 1);
        if (off < len) {
            *leap = lp && k == lp;
            *lm = lp && k >= lp ? k : k + 1;
            *ld = off + 1;
            return true;
        }
        off -= len;
    }
    return false;
}

std::wstring ZcCnNum(int n, bool day) {
    static const wchar_t* d[] = {L"〇", L"一", L"二", L"三", L"四",
                                 L"五", L"六", L"七", L"八", L"九"};
    if (n < 10) return d[n];
    if (n == 10) return L"十";
    if (n < 20) return std::wstring(L"十") + d[n % 10];
    if (day && n > 20 && n < 30) return std::wstring(L"廿") + d[n % 10];
    std::wstring r = std::wstring(d[n / 10]) + L"十";
    if (n % 10) r += d[n % 10];
    return r;
}

const wchar_t* ZcPeriod(int h) {
    return h < 5 ? L"凌晨" : h < 12 ? L"上午" : h < 13 ? L"中午"
           : h < 18 ? L"下午" : L"晚上";
}

void ZcBuildFrame(float scale, ZcFrame& f) {
    const ZcStyle& s = g_zc.style;
    f.scale = scale;
    GetLocalTime(&f.t);
    const SYSTEMTIME& t = f.t;
    std::wstring hh, mm, ss, suffix, date;
    ZcFormat(t, hh, mm, ss, suffix, date);
    static const wchar_t* kWeek[] = {L"星期日", L"星期一", L"星期二", L"星期三",
                                     L"星期四", L"星期五", L"星期六"};
    static const wchar_t* kWeekEn[] = {L"Sunday",   L"Monday", L"Tuesday",
                                       L"Wednesday", L"Thursday", L"Friday",
                                       L"Saturday"};
    int wd = t.wDayOfWeek % 7;
    if (s.layout == ZcLayoutKind::chinese && g_zc.date != ZcDate::none) {
        date = ZcCnNum(t.wMonth, false) + L"月" + ZcCnNum(t.wDay, true) + L" " +
               kWeek[wd];
    } else if (s.weekday && !date.empty() && date.find(L"周") == std::wstring::npos &&
               date.find(L"星期") == std::wstring::npos) {
        date = (s.upper ? std::wstring(kWeekEn[wd]) : std::wstring(kWeek[wd])) +
               L"  " + date;
    }
    if (s.upper) {
        for (auto& c : date) c = (wchar_t)towupper(c);
    }

    float timeSize = s.timeSize * scale;
    std::vector<ZcPiece> row;
    float x = 0, rowH = 0;
    auto make = [&](const std::wstring& txt, float size) {
        ZcPiece p{ZcLayout(txt, s.timeFont, s.timeWeight, size), {}, {}, false};
        if (p.layout) p.size = ZcMeasure(p.layout.Get());
        return p;
    };
    auto add = [&](const std::wstring& txt, float size, bool card) {
        if (txt.empty()) return;
        ZcPiece p = make(txt, size);
        if (!p.layout) return;
        p.card = card;
        float pad = card ? size * 0.12f : 0;
        p.pos = D2D1::Point2F(x + pad, 0);
        x += p.size.width + pad * 2;
        rowH = rowH > p.size.height ? rowH : p.size.height;
        row.push_back(p);
    };
    float cardGap = timeSize * 0.08f;
    std::wstring digits = hh + L":" + mm + (ss.empty() ? L"" : L":" + ss);
    switch (s.layout) {
        case ZcLayoutKind::analog: {
            float d = timeSize * 2.2f;
            f.dial = true;
            f.dialR = d / 2;
            f.dialC = D2D1::Point2F(d / 2, d / 2);
            x = d;
            rowH = d;
            break;
        }
        case ZcLayoutKind::stacked: {
            ZcPiece a = make(hh, timeSize), b = make(mm, timeSize);
            if (a.layout && b.layout) {
                float w = a.size.width > b.size.width ? a.size.width : b.size.width;
                float step = timeSize * 0.92f;
                a.pos = D2D1::Point2F((w - a.size.width) / 2, 0);
                b.pos = D2D1::Point2F((w - b.size.width) / 2, step);
                row.push_back(a);
                row.push_back(b);
                x = w;
                rowH = step + b.size.height;
                if (!ss.empty()) {
                    ZcPiece c = make(ss, timeSize * 0.3f);
                    if (c.layout) {
                        c.pos = D2D1::Point2F(w + timeSize * 0.06f,
                                              rowH - c.size.height - timeSize * 0.15f);
                        x = c.pos.x + c.size.width;
                        row.push_back(c);
                    }
                }
            }
            break;
        }
        case ZcLayoutKind::chinese: {
            std::wstring txt = ZcCnNum(t.wHour, false) + L"时" +
                               (t.wMinute ? ZcCnNum(t.wMinute, false) + L"分" : L"整");
            if (t.wMinute && t.wMinute < 10) txt.insert(txt.size() - 2, L"零");
            if (g_zc.seconds) txt += ZcCnNum(t.wSecond, false) + L"秒";
            add(txt, timeSize, false);
            break;
        }
        case ZcLayoutKind::sentence: {
            int h12 = t.wHour % 12;
            if (h12 == 0) h12 = 12;
            WCHAR b[96];
            if (t.wMinute == 0)
                swprintf_s(b, L"现在是 %s %d 点整", ZcPeriod(t.wHour), h12);
            else
                swprintf_s(b, L"现在是 %s %d 点 %d 分", ZcPeriod(t.wHour), h12,
                           t.wMinute);
            add(b, timeSize, false);
            break;
        }
        default:
            if (s.flip) {
                add(hh, timeSize, true);
                x += cardGap;
                add(mm, timeSize, true);
                if (!ss.empty()) {
                    x += cardGap;
                    add(ss, timeSize, true);
                }
            } else {
                if (s.ghost) {
                    std::wstring g = digits;
                    for (auto& c : g) if (c >= L'0' && c <= L'9') c = L'8';
                    ZcPiece gp = make(g, timeSize);
                    if (gp.layout) {
                        gp.ghost = true;
                        gp.pos = D2D1::Point2F(0, 0);
                        row.push_back(gp);
                    }
                }
                add(digits, timeSize, false);
            }
            if (!suffix.empty()) {
                x += timeSize * 0.08f;
                add(suffix, timeSize * 0.3f, false);
                row.back().pos.y = rowH - row.back().size.height - timeSize * 0.12f;
            }
            break;
    }
    if (s.layout == ZcLayoutKind::ring && !row.empty()) {
        // Center the text inside a circle; the ring shows minute progress.
        float r = std::sqrt(x * x + rowH * rowH) / 2 + timeSize * 0.18f;
        float ox = r - x / 2, oy = r - rowH / 2;
        for (auto& p : row) {
            p.pos.x += ox;
            p.pos.y += oy;
        }
        f.dial = true;
        f.dialR = r;
        f.dialC = D2D1::Point2F(r, r);
        x = rowH = 2 * r;
    }
    float rowW = x;

    ZcPiece datePiece{};
    float blockW = rowW, blockH = rowH, dateGap = g_zc.lineGap * scale;
    if (!date.empty()) {
        datePiece.layout = ZcLayout(date, s.dateFont, s.dateWeight, s.dateSize * scale);
        if (datePiece.layout && s.dateTracking > 0) {
            ComPtr<IDWriteTextLayout1> l1;
            if (SUCCEEDED(datePiece.layout.As(&l1))) {
                float tr = s.dateTracking * scale;
                l1->SetCharacterSpacing(0, tr, 0, {0, (UINT32)date.size()});
            }
        }
        if (datePiece.layout) {
            datePiece.size = ZcMeasure(datePiece.layout.Get());
            if (s.dup || f.dial) dateGap += s.dateSize * scale * 0.3f;
            blockW = blockW > datePiece.size.width ? blockW : datePiece.size.width;
            blockH = rowH + dateGap + datePiece.size.height;
        }
    }
    auto alignX = [&](float w) {
        ZcAlign a = f.dial ? ZcAlign::center : g_zc.align;
        switch (a) {
            case ZcAlign::center: return (blockW - w) / 2;
            case ZcAlign::right: return blockW - w;
            default: return 0.0f;
        }
    };
    float rowY = (datePiece.layout && s.dateAbove) ? datePiece.size.height + dateGap : 0;
    float rowOff = alignX(rowW);
    for (auto& p : row) {
        p.pos.x += rowOff;
        p.pos.y += rowY;
        f.pieces.push_back(p);
    }
    f.dialC.x += rowOff;
    f.dialC.y += rowY;
    if (datePiece.layout) {
        datePiece.pos = D2D1::Point2F(alignX(datePiece.size.width),
                                      s.dateAbove ? 0 : rowH + dateGap);
        f.pieces.push_back(datePiece);
    }
    // ---- Resource usage (CPU / memory) ----
    if (g_zc.resources) {
        float ds = s.dateSize * scale;
        float y0 = blockH + ds * 0.5f;
        bool rings = s.layout == ZcLayoutKind::ring || s.layout == ZcLayoutKind::analog;
        float vals[2] = {g_zcCpu, g_zcMem};
        const wchar_t* names[2] = {L"CPU", L"内存"};
        if (rings) {
            float rr = ds * 1.05f, gap = ds * 1.2f;
            float total = rr * 4 + gap;
            float x0 = (blockW - total) / 2;
            float maxH = 0;
            for (int i = 0; i < 2; i++) {
                D2D1_POINT_2F c = D2D1::Point2F(x0 + rr + i * (2 * rr + gap), y0 + rr);
                f.minis.push_back({c, rr, vals[i] / 100.0f});
                WCHAR b[16];
                swprintf_s(b, L"%.0f%%", (double)vals[i]);
                ZcPiece p{ZcLayout(b, s.dateFont, s.dateWeight, ds * 0.55f), {}, {}, false};
                ZcPiece q{ZcLayout(names[i], s.dateFont, s.dateWeight, ds * 0.42f), {}, {}, false};
                if (p.layout && q.layout) {
                    p.size = ZcMeasure(p.layout.Get());
                    q.size = ZcMeasure(q.layout.Get());
                    p.pos = D2D1::Point2F(c.x - p.size.width / 2, c.y - p.size.height / 2);
                    q.pos = D2D1::Point2F(c.x - q.size.width / 2, c.y + rr + ds * 0.15f);
                    f.pieces.push_back(p);
                    f.pieces.push_back(q);
                    maxH = (std::max)(maxH, 2 * rr + ds * 0.15f + q.size.height);
                }
            }
            if (x0 < 0) {
                for (auto& p : f.pieces) p.pos.x -= x0;
                for (auto& m : f.minis) m.c.x -= x0;
                f.dialC.x -= x0;
                blockW = total;
            }
            blockH = y0 + maxH;
        } else {
            WCHAR b[64];
            swprintf_s(b, L"CPU %.0f%%  ·  内存 %.0f%%", (double)vals[0], (double)vals[1]);
            ZcPiece p{ZcLayout(b, s.dateFont, s.dateWeight, ds * 0.62f), {}, {}, false};
            if (p.layout) {
                p.size = ZcMeasure(p.layout.Get());
                blockW = (std::max)(blockW, p.size.width);
                p.pos = D2D1::Point2F(alignX(p.size.width), y0 - ds * 0.3f);
                blockH = p.pos.y + p.size.height;
                f.pieces.push_back(p);
            }
        }
    }

    // ---- Month calendar (right of or below the clock) ----
    if (g_zc.calendar) {
        float cs = (std::max)(12.0f, s.dateSize * scale * 0.62f);
        float cellW = cs * (g_zc.lunar ? 2.5f : 1.9f), cellH = cs * (g_zc.lunar ? 2.1f : 1.5f);
        float ox, oy;
        if (g_zc.calendar == 1) {
            ox = blockW + s.timeSize * scale * 0.35f;
            oy = 0;
        } else {
            ox = 0;
            oy = blockH + cs * 1.2f;
        }
        auto put = [&](const std::wstring& txt, float size, int weight, float cx, float y,
                       bool dim) {
            ZcPiece p{ZcLayout(txt, s.dateFont, weight, size), {}, {}, false};
            if (!p.layout) return;
            p.size = ZcMeasure(p.layout.Get());
            p.pos = D2D1::Point2F(cx - p.size.width / 2, y);
            p.dim = dim;
            f.pieces.push_back(p);
        };
        WCHAR b[48];
        swprintf_s(b, L"%d年%d月", t.wYear, t.wMonth);
        put(b, cs * 1.15f, (std::max)(s.dateWeight, 600), ox + cellW * 3.5f, oy, false);
        float gy = oy + cs * 2.0f;
        static const wchar_t* kHead[] = {L"一", L"二", L"三", L"四", L"五", L"六", L"日"};
        for (int i = 0; i < 7; i++)
            put(kHead[i], cs * 0.8f, s.dateWeight, ox + cellW * (i + 0.5f), gy, true);
        gy += cs * 1.4f;
        static const int kMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int days = kMonthDays[t.wMonth - 1];
        if (t.wMonth == 2 && ((t.wYear % 4 == 0 && t.wYear % 100) || t.wYear % 400 == 0))
            days = 29;
        // Weekday of the 1st, Monday-first column.
        int first = (ZcDaysFromCivil(t.wYear, t.wMonth, 1) + 3) % 7;  // 1970-01-01 = Thu
        if (first < 0) first += 7;
        static const wchar_t* kLm[] = {L"正", L"二", L"三", L"四", L"五", L"六",
                                       L"七", L"八", L"九", L"十", L"冬", L"腊"};
        int rows = (first + days + 6) / 7;
        for (int dday = 1; dday <= days; dday++) {
            int idx = first + dday - 1, col = idx % 7, row = idx / 7;
            float cx = ox + cellW * (col + 0.5f), y = gy + row * cellH;
            if (dday == t.wDay) {
                f.calHi = true;
                f.calHiRect = D2D1::RectF(cx - cellW * 0.46f, y - cs * 0.15f,
                                          cx + cellW * 0.46f, y + cellH - cs * 0.25f);
            }
            swprintf_s(b, L"%d", dday);
            put(b, cs, dday == t.wDay ? 700 : s.dateWeight, cx, y, col >= 5 && dday != t.wDay);
            int lm, ld;
            bool leap;
            if (g_zc.lunar && ZcLunarDate(t.wYear, t.wMonth, dday, &lm, &ld, &leap)) {
                std::wstring lt;
                if (ld == 1) {
                    lt = std::wstring(leap ? L"闰" : L"") + kLm[lm - 1] + L"月";
                } else if (ld <= 10) {
                    lt = std::wstring(L"初") + ZcCnNum(ld, false);
                } else if (ld == 20) {
                    lt = L"二十";
                } else if (ld == 30) {
                    lt = L"三十";
                } else {
                    lt = ZcCnNum(ld, true);
                }
                put(lt, cs * 0.55f, s.dateWeight, cx, y + cs * 1.25f, dday != t.wDay);
            }
        }
        float calW = cellW * 7, calH = gy - oy + rows * cellH;
        if (g_zc.calendar == 1) {
            blockW = ox + calW;
            blockH = (std::max)(blockH, calH);
        } else {
            blockW = (std::max)(blockW, calW);
            blockH = oy + calH;
        }
    }

    auto shiftAll = [&](float dx, float dy) {
        for (auto& p : f.pieces) {
            p.pos.x += dx;
            p.pos.y += dy;
        }
        for (auto& m : f.minis) {
            m.c.x += dx;
            m.c.y += dy;
        }
        f.dialC.x += dx;
        f.dialC.y += dy;
        f.calHiRect.left += dx; f.calHiRect.right += dx;
        f.calHiRect.top += dy; f.calHiRect.bottom += dy;
        f.bar.left += dx; f.bar.right += dx; f.bar.top += dy; f.bar.bottom += dy;
    };
    if (s.accentBar) {
        float bw = (std::max)(3.0f, timeSize * 0.035f), gap = timeSize * 0.16f;
        shiftAll(bw + gap, 0);
        f.hasBar = true;
        f.bar = D2D1::RectF(0, blockH * 0.06f, bw, blockH * 0.96f);
        blockW += bw + gap;
    }
    if (s.glass) {
        float pad = timeSize * 0.22f;
        shiftAll(pad, pad);
        blockW += pad * 2;
        blockH += pad * 2;
        f.glassRect = D2D1::RectF(0, 0, blockW, blockH);
    }
    f.size = D2D1::SizeF(blockW, blockH);
}

// Arc from 12 o'clock clockwise by `frac` of a full turn.
ComPtr<ID2D1PathGeometry> ZcArc(D2D1_POINT_2F c, float r, float frac) {
    ComPtr<ID2D1PathGeometry> g;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_zcD2dFactory->CreatePathGeometry(&g)) || FAILED(g->Open(&sink)))
        return nullptr;
    float a = frac * 6.2831853f;
    sink->BeginFigure(D2D1::Point2F(c.x, c.y - r), D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(c.x + r * std::sin(a), c.y - r * std::cos(a)),
        D2D1::SizeF(r, r), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
        frac > 0.5f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    return g;
}

// Records the block at the origin. main = full colors (gradient, ghost, cards,
// glass, accents); otherwise every shape is drawn in `color` as a mask.
ComPtr<ID2D1CommandList> ZcRecord(const ZcFrame& f, D2D1_COLOR_F color, bool main) {
    const ZcStyle& s = g_zc.style;
    float scale = f.scale;
    ComPtr<ID2D1CommandList> list;
    if (FAILED(g_zcDc->CreateCommandList(&list))) return nullptr;
    ComPtr<ID2D1Image> oldTarget;
    g_zcDc->GetTarget(&oldTarget);
    D2D1_MATRIX_3X2_F oldTransform;
    g_zcDc->GetTransform(&oldTransform);
    // Called inside the frame's BeginDraw/EndDraw: switching the target to a
    // command list records the drawing instead of rasterizing it.
    g_zcDc->SetTarget(list.Get());
    g_zcDc->SetTransform(D2D1::Matrix3x2F::Identity());
    ComPtr<ID2D1SolidColorBrush> brush, cardBrush, lineBrush, ghostBrush, accentBrush,
        trackBrush;
    g_zcDc->CreateSolidColorBrush(color, &brush);
    ComPtr<ID2D1SolidColorBrush> dimBrush;
    g_zcDc->CreateSolidColorBrush(
        D2D1::ColorF(s.text.r, s.text.g, s.text.b, s.text.a * 0.55f), &dimBrush);
    ComPtr<ID2D1Brush> fill = brush;
    if (main) {
        g_zcDc->CreateSolidColorBrush(s.ghostColor, &ghostBrush);
        g_zcDc->CreateSolidColorBrush(s.accent, &accentBrush);
        g_zcDc->CreateSolidColorBrush(s.track, &trackBrush);
        if (s.flip) {
            g_zcDc->CreateSolidColorBrush(s.cardColor, &cardBrush);
            g_zcDc->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.55f), &lineBrush);
        }
        if (s.grad.size() >= 2) {
            std::vector<D2D1_GRADIENT_STOP> stops;
            for (size_t i = 0; i < s.grad.size(); i++)
                stops.push_back({(float)i / (s.grad.size() - 1), s.grad[i]});
            ComPtr<ID2D1GradientStopCollection> sc;
            ComPtr<ID2D1LinearGradientBrush> lg;
            if (SUCCEEDED(g_zcDc->CreateGradientStopCollection(
                    stops.data(), (UINT32)stops.size(), &sc)) &&
                SUCCEEDED(g_zcDc->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, 0),
                        D2D1::Point2F(f.size.width, f.size.height * 0.6f)),
                    sc.Get(), &lg))) {
                fill = lg;
            }
        }
        if (s.glass) {
            ComPtr<ID2D1SolidColorBrush> gb, gl;
            g_zcDc->CreateSolidColorBrush(s.glassColor, &gb);
            g_zcDc->CreateSolidColorBrush(s.glassBorder, &gl);
            float rad = s.timeSize * scale * 0.18f;
            auto rr = D2D1::RoundedRect(f.glassRect, rad, rad);
            if (gb) g_zcDc->FillRoundedRectangle(rr, gb.Get());
            if (gl) {
                auto inner = rr;
                inner.rect.left += 0.5f; inner.rect.top += 0.5f;
                inner.rect.right -= 0.5f; inner.rect.bottom -= 0.5f;
                g_zcDc->DrawRoundedRectangle(inner, gl.Get(), 1.0f * scale);
            }
        }
    }
    ID2D1Brush* acc = main && accentBrush ? (ID2D1Brush*)accentBrush.Get() : brush.Get();
    if (f.hasBar && acc) {
        float w = f.bar.right - f.bar.left;
        g_zcDc->FillRoundedRectangle(D2D1::RoundedRect(f.bar, w / 2, w / 2), acc);
    }
    if (f.calHi && acc) {
        float rr = (f.calHiRect.bottom - f.calHiRect.top) * 0.22f;
        g_zcDc->FillRoundedRectangle(D2D1::RoundedRect(f.calHiRect, rr, rr), acc);
    }
    if (!f.minis.empty()) {
        ComPtr<ID2D1StrokeStyle> round;
        g_zcD2dFactory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND),
            nullptr, 0, &round);
        ID2D1Brush* trk = main && trackBrush ? (ID2D1Brush*)trackBrush.Get() : brush.Get();
        for (const auto& m : f.minis) {
            float sw = (std::max)(2.0f, m.r * 0.14f), r = m.r - sw / 2;
            g_zcDc->DrawEllipse(D2D1::Ellipse(m.c, r, r), trk, sw);
            if (m.frac > 0.999f) {
                g_zcDc->DrawEllipse(D2D1::Ellipse(m.c, r, r), acc, sw);
            } else if (m.frac > 0.005f) {
                if (auto g = ZcArc(m.c, r, m.frac))
                    g_zcDc->DrawGeometry(g.Get(), acc, sw, round.Get());
            }
        }
    }
    if (f.dial && brush) {
        D2D1_POINT_2F c = f.dialC;
        float r = f.dialR;
        ID2D1Brush* trk = main && trackBrush ? (ID2D1Brush*)trackBrush.Get() : brush.Get();
        if (s.layout == ZcLayoutKind::ring) {
            float sw = (std::max)(3.0f, r * 0.05f);
            g_zcDc->DrawEllipse(D2D1::Ellipse(c, r - sw / 2, r - sw / 2), trk, sw);
            float frac = (f.t.wMinute + (g_zc.seconds ? f.t.wSecond / 60.0f : 0)) / 60.0f;
            ComPtr<ID2D1StrokeStyle> round;
            g_zcD2dFactory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND),
                nullptr, 0, &round);
            if (frac > 0.999f) {
                g_zcDc->DrawEllipse(D2D1::Ellipse(c, r - sw / 2, r - sw / 2), acc, sw);
            } else if (frac > 0.001f) {
                if (auto g = ZcArc(c, r - sw / 2, frac))
                    g_zcDc->DrawGeometry(g.Get(), acc, sw, round.Get());
            }
        } else {
            // Analog face: rim, 60 ticks (12 long), hands.
            float rim = (std::max)(2.0f, r * 0.025f);
            g_zcDc->DrawEllipse(D2D1::Ellipse(c, r - rim / 2, r - rim / 2), fill.Get(), rim);
            for (int i = 0; i < 60; i++) {
                bool hour = i % 5 == 0;
                float a = i * 6.2831853f / 60, sn = std::sin(a), cs = std::cos(a);
                float r1 = r * 0.9f, r0 = r1 - r * (hour ? 0.12f : 0.04f);
                g_zcDc->DrawLine(D2D1::Point2F(c.x + r0 * sn, c.y - r0 * cs),
                                 D2D1::Point2F(c.x + r1 * sn, c.y - r1 * cs),
                                 hour ? fill.Get() : trk,
                                 hour ? r * 0.025f : r * 0.012f);
            }
            ComPtr<ID2D1StrokeStyle> round;
            g_zcD2dFactory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND),
                nullptr, 0, &round);
            float sec = g_zc.seconds ? f.t.wSecond : 0;
            float mins = f.t.wMinute + sec / 60.0f;
            float hrs = (f.t.wHour % 12) + mins / 60.0f;
            auto hand = [&](float turns, float len, float tail, float w, ID2D1Brush* b) {
                float a = turns * 6.2831853f, sn = std::sin(a), cs = std::cos(a);
                g_zcDc->DrawLine(D2D1::Point2F(c.x - tail * sn, c.y + tail * cs),
                                 D2D1::Point2F(c.x + len * sn, c.y - len * cs), b, w,
                                 round.Get());
            };
            hand(hrs / 12, r * 0.5f, r * 0.08f, r * 0.06f, fill.Get());
            hand(mins / 60, r * 0.74f, r * 0.08f, r * 0.04f, fill.Get());
            if (g_zc.seconds) hand(sec / 60, r * 0.8f, r * 0.15f, r * 0.015f, acc);
            g_zcDc->FillEllipse(D2D1::Ellipse(c, r * 0.04f, r * 0.04f),
                                g_zc.seconds ? acc : fill.Get());
        }
    }
    for (const auto& p : f.pieces) {
        if (p.ghost) {
            if (main && ghostBrush && p.layout)
                g_zcDc->DrawTextLayout(p.pos, p.layout.Get(), ghostBrush.Get());
            continue;
        }
        if (p.card && cardBrush) {
            float pad = p.size.height * 0.12f;
            D2D1_RECT_F r = D2D1::RectF(p.pos.x - pad, p.pos.y,
                                        p.pos.x + p.size.width + pad,
                                        p.pos.y + p.size.height);
            float radius = p.size.height * 0.08f;
            g_zcDc->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius),
                                         cardBrush.Get());
            float mid = (r.top + r.bottom) / 2;
            g_zcDc->DrawLine(D2D1::Point2F(r.left, mid), D2D1::Point2F(r.right, mid),
                             lineBrush.Get(), 2.0f * scale);
        }
        if (p.dim && main && dimBrush && p.layout) {
            g_zcDc->DrawTextLayout(p.pos, p.layout.Get(), dimBrush.Get());
        } else if (fill && p.layout) {
            g_zcDc->DrawTextLayout(p.pos, p.layout.Get(), fill.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }
    }
    g_zcDc->SetTarget(oldTarget.Get());
    g_zcDc->SetTransform(oldTransform);
    return SUCCEEDED(list->Close()) ? list : nullptr;
}

ComPtr<ID2D1Effect> ZcShadowEffect(ID2D1Image* input, D2D1_COLOR_F color,
                                   float blur) {
    ComPtr<ID2D1Effect> fx;
    if (FAILED(g_zcDc->CreateEffect(kZcClsidShadow, &fx))) return nullptr;
    fx->SetInput(0, input);
    // Standard deviation ~ radius / 2, the convention used by CSS blur.
    fx->SetValue(kZcShadowPropBlur, blur / 2.0f);
    fx->SetValue(kZcShadowPropColor,
                 D2D1::Vector4F(color.r, color.g, color.b, color.a));
    return fx;
}

void ZcDrawBlock(D2D1_POINT_2F center, float scale) {
    const ZcStyle& s = g_zc.style;
    ZcFrame f;
    ZcBuildFrame(scale, f);
    if (f.pieces.empty() && !f.dial) return;

    // Rotate around the block center, then move the center into place.
    D2D1_POINT_2F half = D2D1::Point2F(f.size.width / 2, f.size.height / 2);
    D2D1::Matrix3x2F place =
        D2D1::Matrix3x2F::Rotation(s.rotation, half) *
        D2D1::Matrix3x2F::Translation(center.x - half.x, center.y - half.y);

    ComPtr<ID2D1CommandList> content = ZcRecord(f, s.text, true);
    if (!content) return;

    // Hollow text: the shadow/glow come from the solid mask, the fill is
    // punched out afterwards so only the outline remains.
    ComPtr<ID2D1CommandList> solid =
        s.hollow ? ZcRecord(f, D2D1::ColorF(1, 1, 1, 1), false) : content;

    if (s.shadow && s.shadowColor.a > 0 && solid) {
        if (auto fx = ZcShadowEffect(solid.Get(), s.shadowColor, s.shadowBlur * scale)) {
            // Offset in screen space so the light comes from the same side
            // regardless of rotation.
            g_zcDc->SetTransform(place * D2D1::Matrix3x2F::Translation(
                                             s.shadowDx * scale, s.shadowDy * scale));
            g_zcDc->DrawImage(fx.Get());
        }
    }
    if (s.dup) {
        float o = s.dupOffset * scale;
        if (auto a = ZcRecord(f, s.dupA, false)) {
            g_zcDc->SetTransform(D2D1::Matrix3x2F::Translation(-o, -o * 0.4f) * place);
            g_zcDc->DrawImage(a.Get());
        }
        if (auto b = ZcRecord(f, s.dupB, false)) {
            g_zcDc->SetTransform(D2D1::Matrix3x2F::Translation(o, o * 0.4f) * place);
            g_zcDc->DrawImage(b.Get());
        }
    }
    g_zcDc->SetTransform(place);
    if (s.glow) {
        ComPtr<ID2D1CommandList> mask = ZcRecord(f, s.glowColor, false);
        if (mask) {
            // Wide soft halo plus a tighter bright core.
            if (auto fx = ZcShadowEffect(mask.Get(), s.glowColor, s.glowRadius * scale))
                g_zcDc->DrawImage(fx.Get());
            if (auto fx = ZcShadowEffect(mask.Get(), s.glowColor,
                                         s.glowRadius * scale * 0.35f))
                g_zcDc->DrawImage(fx.Get());
        }
    }
    if (s.outline || s.hollow) {
        D2D1_COLOR_F oc = s.outline ? s.outlineColor : s.text;
        ComPtr<ID2D1CommandList> mask = ZcRecord(f, oc, false);
        ComPtr<ID2D1Effect> fx;
        if (mask && SUCCEEDED(g_zcDc->CreateEffect(kZcClsidMorphology, &fx))) {
            UINT32 w = (UINT32)ZcClamp((int)std::lround(s.outlineWidth * scale), 1, 50);
            fx->SetInput(0, mask.Get());
            fx->SetValue(kZcMorphPropMode, (UINT32)kZcMorphModeDilate);
            fx->SetValue(kZcMorphPropWidth, w * 2 + 1);
            fx->SetValue(kZcMorphPropHeight, w * 2 + 1);
            g_zcDc->DrawImage(fx.Get());
        }
    }
    if (s.hollow) {
        if (solid) {
            g_zcDc->DrawImage(solid.Get(), D2D1_INTERPOLATION_MODE_LINEAR,
                              D2D1_COMPOSITE_MODE_DESTINATION_OUT);
        }
    } else {
        g_zcDc->DrawImage(content.Get());
    }
    g_zcDc->SetTransform(D2D1::Matrix3x2F::Identity());
}

////////////////////////////////////////////////////////////////////////////////
// Rendering (swap chain + DirectComposition as in Desktop Live Overlay)

bool ZcInitDirectX() {
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                   D3D11_SDK_VERSION, &g_zcD3d, nullptr, nullptr);
    if (FAILED(hr)) {
        // No usable GPU (e.g. remote session): fall back to WARP.
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                               D3D11_SDK_VERSION, &g_zcD3d, nullptr, nullptr);
    }
    if (FAILED(hr) || FAILED(g_zcD3d.As(&g_zcDxgiDevice)) ||
        FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&g_zcDxgiFactory))) ||
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 IID_PPV_ARGS(&g_zcD2dFactory))) ||
        FAILED(g_zcD2dFactory->CreateDevice(g_zcDxgiDevice.Get(),
                                            &g_zcD2dDevice)) ||
        FAILED(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(g_zcDwrite.GetAddressOf())))) {
        Wh_Log(L"InitDirectX failed: 0x%08X", (unsigned)hr);
        return false;
    }
    return true;
}

void ZcUninitDirectX() {
    g_zcDwrite.Reset();
    g_zcD2dDevice.Reset();
    g_zcD2dFactory.Reset();
    g_zcDxgiFactory.Reset();
    g_zcDxgiDevice.Reset();
    g_zcD3d.Reset();
}

void ZcReleaseSwapChain() {
    g_zcCompVisual.Reset();
    g_zcCompTarget.Reset();
    g_zcCompDevice.Reset();
    if (g_zcDc) g_zcDc->SetTarget(nullptr);
    g_zcTarget.Reset();
    g_zcDc.Reset();
    g_zcSwapChain.Reset();
}

bool ZcBindTarget() {
    ComPtr<IDXGISurface2> surface;
    if (FAILED(g_zcSwapChain->GetBuffer(0, IID_PPV_ARGS(&surface)))) return false;
    D2D1_BITMAP_PROPERTIES1 props = {};
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    g_zcTarget.Reset();
    if (FAILED(g_zcDc->CreateBitmapFromDxgiSurface(surface.Get(), props,
                                                   &g_zcTarget))) {
        return false;
    }
    g_zcDc->SetTarget(g_zcTarget.Get());
    // Work in physical pixels; DPI scaling is applied per monitor.
    g_zcDc->SetDpi(96.0f, 96.0f);
    return true;
}

bool ZcCreateSwapChain(UINT width, UINT height) {
    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = width ? width : 1;
    scd.Height = height ? height : 1;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    HRESULT hr = g_zcDxgiFactory->CreateSwapChainForComposition(
        g_zcDxgiDevice.Get(), &scd, nullptr, &g_zcSwapChain);
    if (FAILED(hr) ||
        FAILED(hr = g_zcD2dDevice->CreateDeviceContext(
                   D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_zcDc)) ||
        !ZcBindTarget() ||
        FAILED(hr = DCompositionCreateDevice(g_zcDxgiDevice.Get(),
                                             IID_PPV_ARGS(&g_zcCompDevice))) ||
        FAILED(hr = g_zcCompDevice->CreateTargetForHwnd(g_zcOverlayWnd, TRUE,
                                                        &g_zcCompTarget)) ||
        FAILED(hr = g_zcCompDevice->CreateVisual(&g_zcCompVisual)) ||
        FAILED(hr = g_zcCompVisual->SetContent(g_zcSwapChain.Get())) ||
        FAILED(hr = g_zcCompTarget->SetRoot(g_zcCompVisual.Get())) ||
        FAILED(hr = g_zcCompDevice->Commit())) {
        Wh_Log(L"CreateSwapChain failed: 0x%08X", (unsigned)hr);
        return false;
    }
    return true;
}

bool ZcResizeSwapChain(UINT width, UINT height) {
    if (!g_zcSwapChain || !g_zcDc) return false;
    g_zcDc->SetTarget(nullptr);
    g_zcTarget.Reset();
    if (FAILED(g_zcSwapChain->ResizeBuffers(0, width ? width : 1,
                                            height ? height : 1,
                                            DXGI_FORMAT_UNKNOWN, 0))) {
        return false;
    }
    return ZcBindTarget();
}

void ZcRender() {
    if (g_zcUnloading || !g_zcDc || !g_zcSwapChain || !g_zcOverlayWnd) return;

    HWND host = GetParent(g_zcOverlayWnd);
    RECT hostRc{};
    if (host) GetWindowRect(host, &hostRc);

    if (g_zc.resources) ZcSampleResources();
    g_zcDc->BeginDraw();
    g_zcDc->Clear(D2D1::ColorF(0, 0, 0, 0));
    for (HMONITOR m : ZcTargetMonitors()) {
        MONITORINFO mi{sizeof(mi)};
        if (!GetMonitorInfo(m, &mi)) continue;
        float scale = ZcMonitorScale(m);
        RECT r = mi.rcWork;
        OffsetRect(&r, -hostRc.left, -hostRc.top);
        int col = g_zc.anchor % 3, row = g_zc.anchor / 3;
        // Anchor the block center at 1/4, 1/2 or 3/4 of the work area.
        float fx[] = {0.22f, 0.5f, 0.78f};
        float fy[] = {0.2f, 0.5f, 0.8f};
        D2D1_POINT_2F c = D2D1::Point2F(
            r.left + (r.right - r.left) * fx[col] + g_zc.offsetX * scale,
            r.top + (r.bottom - r.top) * fy[row] + g_zc.offsetY * scale);
        ZcDrawBlock(c, scale * g_zc.size / 100.0f);
    }
    HRESULT hr = g_zcDc->EndDraw();
    if (hr == (HRESULT)D2DERR_RECREATE_TARGET) {
        Wh_Log(L"Device lost, recreating");
        RECT rc;
        GetClientRect(g_zcOverlayWnd, &rc);
        ZcReleaseSwapChain();
        ZcCreateSwapChain(rc.right, rc.bottom);
        return;
    }
    g_zcSwapChain->Present(0, 0);
}

// Wake up right after the next minute (or second) boundary only.
void ZcScheduleNext() {
    if (!g_zcOverlayWnd) return;
    SYSTEMTIME t;
    GetLocalTime(&t);
    UINT ms = g_zc.seconds ? 1000 - t.wMilliseconds
                           : (60 - t.wSecond) * 1000 - t.wMilliseconds;
    // Resource usage refreshes every ~2 s, only while it is shown.
    if (g_zc.resources && ms > 2000) ms = 2000 - t.wMilliseconds % 1000;
    SetTimer(g_zcOverlayWnd, ZC_TIMER_REFRESH, ms + 20, nullptr);
}

void ZcFitToHost() {
    if (!g_zcOverlayWnd) return;
    HWND host = GetParent(g_zcOverlayWnd);
    RECT rc;
    if (!host || !GetWindowRect(host, &rc)) return;
    SetWindowPos(g_zcOverlayWnd, nullptr, 0, 0, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
    ZcRender();
}

////////////////////////////////////////////////////////////////////////////////
// Windows

void ZcCreateOverlay();

LRESULT CALLBACK ZcOverlayProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER:
            if (wp == ZC_TIMER_REFRESH && !g_zcUnloading) {
                ZcRender();
                ZcScheduleNext();
                return 0;
            }
            break;
        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* pos = (const WINDOWPOS*)lp;
            if (!(pos->flags & SWP_NOSIZE) && !g_zcUnloading) {
                ZcResizeSwapChain(pos->cx, pos->cy);
                ZcRender();
            }
            break;
        }
        case WM_DESTROY:
            KillTimer(hWnd, ZC_TIMER_REFRESH);
            ZcReleaseSwapChain();
            g_zcOverlayWnd = nullptr;
            if (!g_zcUnloading && g_zcMessageWnd) {
                SetTimer(g_zcMessageWnd, ZC_TIMER_MSG_RECREATE, 500, nullptr);
            }
            return 0;
        case ZC_WM_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
        case ZC_WM_SETTINGS:
            ZcLoadSettings();
            ZcRender();
            ZcScheduleNext();
            return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

LRESULT CALLBACK ZcMessageProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_zcTaskbarCreatedMsg && g_zcTaskbarCreatedMsg) {
        // Shell restarted inside this process: rebind to the new WorkerW.
        Wh_Log(L"TaskbarCreated");
        if (!g_zcUnloading) SetTimer(hWnd, ZC_TIMER_MSG_RECREATE, 1000, nullptr);
        return 0;
    }
    switch (msg) {
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
            if (!g_zcUnloading) SetTimer(hWnd, ZC_TIMER_MSG_DISPLAY, 300, nullptr);
            return 0;
        case WM_TIMECHANGE:
            if (g_zcOverlayWnd && !g_zcUnloading) {
                ZcRender();
                ZcScheduleNext();
            }
            return 0;
        case WM_TIMER:
            KillTimer(hWnd, wp);
            if (g_zcUnloading) return 0;
            if (wp == ZC_TIMER_MSG_DISPLAY) {
                ZcFitToHost();
            } else if (wp == ZC_TIMER_MSG_RECREATE) {
                HWND worker = ZcGetWorkerW();
                if (g_zcOverlayWnd && worker &&
                    GetParent(g_zcOverlayWnd) != worker) {
                    DestroyWindow(g_zcOverlayWnd);  // re-arms this timer
                } else if (!g_zcOverlayWnd) {
                    ZcCreateOverlay();
                }
            }
            return 0;
        case ZC_WM_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            g_zcMessageWnd = nullptr;
            return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

bool g_zcOverlayClass = false, g_zcMessageClass = false;

bool ZcRegister(bool& done, PCWSTR name, WNDPROC proc) {
    if (done) return true;
    WNDCLASS wc = {};
    wc.lpfnWndProc = proc;
    wc.hInstance = ZcModule();
    wc.lpszClassName = name;
    done = RegisterClass(&wc) != 0;
    if (!done) Wh_Log(L"RegisterClass failed: %u", GetLastError());
    return done;
}

void ZcCreateOverlay() {
    if (g_zcOverlayWnd || g_zcUnloading) return;
    if (!g_zcLazyInit.exchange(true)) {
        g_zcInitOk = ZcInitDirectX();
    }
    if (!g_zcInitOk) return;
    HWND worker = ZcGetWorkerW();
    if (!worker) {
        Wh_Log(L"WorkerW not found");
        return;
    }
    if (!ZcRegister(g_zcOverlayClass, ZC_OVERLAY_CLASS, ZcOverlayProc)) return;
    RECT rc;
    GetWindowRect(worker, &rc);
    g_zcOverlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        ZC_OVERLAY_CLASS, nullptr, WS_CHILD | WS_VISIBLE | WS_DISABLED, 0, 0,
        rc.right - rc.left, rc.bottom - rc.top, worker, nullptr, ZcModule(),
        nullptr);
    if (!g_zcOverlayWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }
    Wh_Log(L"overlay created on %p (%ldx%ld)", worker, rc.right - rc.left,
           rc.bottom - rc.top);
    if (ZcCreateSwapChain(rc.right - rc.left, rc.bottom - rc.top)) {
        ZcRender();
        ZcScheduleNext();
    } else {
        ZcReleaseSwapChain();
    }
}

void ZcCreateMessageWindow() {
    if (g_zcMessageWnd) return;
    if (!ZcRegister(g_zcMessageClass, ZC_MESSAGE_CLASS, ZcMessageProc)) return;
    // Hidden top-level window: receives WM_DISPLAYCHANGE / TaskbarCreated.
    g_zcMessageWnd = CreateWindowEx(0, ZC_MESSAGE_CLASS, nullptr, 0, 0, 0, 0, 0,
                                    nullptr, nullptr, ZcModule(), nullptr);
}

void WINAPI ZcCreateAll(void*) {
    ZcCreateOverlay();
    ZcCreateMessageWindow();
}

////////////////////////////////////////////////////////////////////////////////
// Hook: the desktop FolderView appears (first start / explorer restart)

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t ZcCreateWindowExW_Original;

HWND WINAPI ZcCreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name,
                                   DWORD style, int x, int y, int w, int h,
                                   HWND parent, HMENU menu, HINSTANCE inst,
                                   LPVOID param) {
    HWND hWnd = ZcCreateWindowExW_Original(exStyle, cls, name, style, x, y, w,
                                           h, parent, menu, inst, param);
    if (!hWnd || g_zcUnloading || !ZcIsFolderView(hWnd)) return hWnd;
    Wh_Log(L"FolderView created");
    g_zcCreateTimer = SetTimer(nullptr, g_zcCreateTimer, 1000,
                               [](HWND, UINT, UINT_PTR id, DWORD) {
                                   KillTimer(nullptr, id);
                                   g_zcCreateTimer = 0;
                                   ZcCreateAll(nullptr);
                               });
    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Mod life cycle

BOOL Wh_ModInit() {
    Wh_Log(L">");
    ZcLoadSettings();
    g_zcTaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)ZcCreateWindowExW_Hook,
                       (void**)&ZcCreateWindowExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (HWND progman = ZcGetProgman()) {
        ZcRunFromWindowThread(progman, ZcCreateAll, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_zcUnloading = true;
    if (HWND progman = ZcGetProgman()) {
        ZcRunFromWindowThread(
            progman,
            [](void*) {
                if (g_zcCreateTimer) {
                    KillTimer(nullptr, g_zcCreateTimer);
                    g_zcCreateTimer = 0;
                }
            },
            nullptr);
    }
    if (g_zcOverlayWnd) SendMessage(g_zcOverlayWnd, ZC_WM_CLEANUP, 0, 0);
    if (g_zcMessageWnd) SendMessage(g_zcMessageWnd, ZC_WM_CLEANUP, 0, 0);
    if (g_zcOverlayClass) UnregisterClass(ZC_OVERLAY_CLASS, ZcModule());
    if (g_zcMessageClass) UnregisterClass(ZC_MESSAGE_CLASS, ZcModule());
    ZcUninitDirectX();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (g_zcOverlayWnd) {
        SendMessage(g_zcOverlayWnd, ZC_WM_SETTINGS, 0, 0);
    } else {
        ZcLoadSettings();
    }
}
