// ==WindhawkMod==
// @id              zen-desktop-clock
// @name            ZenDesktop: 桌面时钟
// @name:zh-CN      ZenDesktop: 桌面时钟
// @description     A stylish clock and date drawn on the desktop behind the icons: tilt, soft drop shadow, glow, outline and flip-card presets.
// @description:zh-CN 在桌面图标后方显示花样时钟与日期：倾斜、柔和投影、霓虹光晕、描边、翻页卡片等预设。
// @version         1.0.0
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

- **样式预设**：倾斜阴影（白色大字、约 -8° 倾斜、深色柔和投影）、极简、霓虹（光晕）、翻页（静态翻页卡片）、自定义（使用下方全部详细设置）。
- 时间 12/24 小时制、可显示秒；日期格式 `10/28/2026`、`2026-10-28`、`10月28日 周三`，或不显示。
- 每行可单独设字体、字号、字重；文字颜色、旋转角度、真模糊投影（颜色/模糊半径/偏移/不透明度）、光晕、描边。
- 位置：九宫格锚点 + 偏移；可选指定显示器或所有显示器；按显示器 DPI 缩放。
- 只在分钟（或开了秒时每秒）变化时重绘，CPU 占用极低；资源管理器重启后自动恢复。

基于 m417z 的 Desktop Live Overlay（GPL-3.0）的桌面宿主与渲染框架。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: tilted
  $name: "Style preset"
  $name:zh-CN: "样式预设"
  $description: "Presets override the color/rotation/shadow/glow/outline/font settings below. Choose Custom to use them."
  $description:zh-CN: "选择预设时会覆盖下方颜色、旋转、投影、光晕、描边和字体设置；选「自定义」才使用下方设置。"
  $options:
  - tilted: "Tilted shadow"
  - minimal: "Minimal"
  - neon: "Neon glow"
  - flip: "Flip cards"
  - custom: "Custom"
  $options:zh-CN:
  - tilted: "倾斜阴影"
  - minimal: "极简"
  - neon: "霓虹"
  - flip: "翻页"
  - custom: "自定义"
- use24Hour: true
  $name: "24-hour time"
  $name:zh-CN: "24 小时制"
- showSeconds: false
  $name: "Show seconds"
  $name:zh-CN: "显示秒"
  $description: "Redraws every second instead of every minute."
  $description:zh-CN: "开启后每秒重绘一次，否则每分钟一次。"
- dateFormat: mdy
  $name: "Date format"
  $name:zh-CN: "日期格式"
  $options:
  - mdy: "10/28/2026"
  - dmy: "28/10/2026"
  - iso: "2026-10-28"
  - cn: "10月28日 周三"
  - cnFull: "2026年10月28日 星期三"
  - none: "Hidden"
  $options:zh-CN:
  - mdy: "10/28/2026"
  - dmy: "28/10/2026"
  - iso: "2026-10-28"
  - cn: "10月28日 周三"
  - cnFull: "2026年10月28日 星期三"
  - none: "不显示"
- textAlign: left
  $name: "Line alignment"
  $name:zh-CN: "行对齐"
  $options:
  - left: "Left"
  - center: "Center"
  - right: "Right"
  $options:zh-CN:
  - left: "左对齐"
  - center: "居中"
  - right: "右对齐"
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
  - top: "顶部居中"
  - topRight: "右上"
  - left: "左侧居中"
  - center: "正中"
  - right: "右侧居中"
  - bottomLeft: "左下"
  - bottom: "底部居中"
  - bottomRight: "右下"
- offsetX: 0
  $name: "Horizontal offset (px at 100%)"
  $name:zh-CN: "水平偏移（100% 缩放下像素）"
  $description: "Positive moves right. Scaled by the monitor DPI."
  $description:zh-CN: "正数向右。会按显示器缩放比例换算。"
- offsetY: -80
  $name: "Vertical offset (px at 100%)"
  $name:zh-CN: "垂直偏移（100% 缩放下像素）"
  $description: "Positive moves down."
  $description:zh-CN: "正数向下。"
- monitor: 1
  $name: "Monitor"
  $name:zh-CN: "显示器"
  $description: "1 = first monitor, 2 = second… 0 = all monitors."
  $description:zh-CN: "1 = 第一个显示器，2 = 第二个……0 = 所有显示器都显示。"
- lineGap: 4
  $name: "Gap between lines (px)"
  $name:zh-CN: "两行间距（像素）"
- timeFontFamily: "Segoe UI Variable Display"
  $name: "Time font"
  $name:zh-CN: "时间字体"
  $description: "(Custom only) Any installed font name."
  $description:zh-CN: "（仅自定义）填写已安装的字体名。"
- timeFontSize: 140
  $name: "Time font size"
  $name:zh-CN: "时间字号"
- timeFontWeight: 600
  $name: "Time font weight"
  $name:zh-CN: "时间字重"
  $description: "100–900. 400 = regular, 700 = bold."
  $description:zh-CN: "100–900，400 为常规，700 为粗体。"
- dateFontFamily: "Segoe UI Variable Display"
  $name: "Date font"
  $name:zh-CN: "日期字体"
  $description: "(Custom only)"
  $description:zh-CN: "（仅自定义）"
- dateFontSize: 40
  $name: "Date font size"
  $name:zh-CN: "日期字号"
- dateFontWeight: 500
  $name: "Date font weight"
  $name:zh-CN: "日期字重"
- textColor: "#FFFFFFFF"
  $name: "Text color"
  $name:zh-CN: "文字颜色"
  $description: "(Custom only) #RRGGBB or #AARRGGBB."
  $description:zh-CN: "（仅自定义）#RRGGBB 或 #AARRGGBB。"
- rotation: -8
  $name: "Rotation (degrees)"
  $name:zh-CN: "旋转角度（度）"
  $description: "(Custom only) Negative tilts counter-clockwise, -45 to 45."
  $description:zh-CN: "（仅自定义）负数逆时针倾斜，范围 -45 到 45。"
- shadowEnabled: true
  $name: "Drop shadow"
  $name:zh-CN: "投影"
  $description: "(Custom only)"
  $description:zh-CN: "（仅自定义）"
- shadowColor: "#000000"
  $name: "Shadow color"
  $name:zh-CN: "投影颜色"
- shadowBlur: 8
  $name: "Shadow blur radius"
  $name:zh-CN: "投影模糊半径"
- shadowOffsetX: 6
  $name: "Shadow offset X"
  $name:zh-CN: "投影水平偏移"
- shadowOffsetY: 8
  $name: "Shadow offset Y"
  $name:zh-CN: "投影垂直偏移"
- shadowOpacity: 70
  $name: "Shadow opacity (%)"
  $name:zh-CN: "投影不透明度（%）"
- glowEnabled: false
  $name: "Glow"
  $name:zh-CN: "光晕"
  $description: "(Custom only)"
  $description:zh-CN: "（仅自定义）"
- glowColor: "#00E5FF"
  $name: "Glow color"
  $name:zh-CN: "光晕颜色"
- glowRadius: 12
  $name: "Glow radius"
  $name:zh-CN: "光晕半径"
- outlineEnabled: false
  $name: "Outline"
  $name:zh-CN: "描边"
  $description: "(Custom only)"
  $description:zh-CN: "（仅自定义）"
- outlineColor: "#000000"
  $name: "Outline color"
  $name:zh-CN: "描边颜色"
- outlineWidth: 2
  $name: "Outline width (px)"
  $name:zh-CN: "描边宽度（像素）"
- flipCards: false
  $name: "Flip-card background"
  $name:zh-CN: "翻页卡片底"
  $description: "(Custom only) Draw hours/minutes on dark flip cards."
  $description:zh-CN: "（仅自定义）把时、分画在深色翻页卡片上。"
- cardColor: "#E0202024"
  $name: "Card color"
  $name:zh-CN: "卡片颜色"
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
};

struct ZcSettings {
    bool use24 = true, seconds = false;
    ZcDate date = ZcDate::mdy;
    ZcAlign align = ZcAlign::left;
    int anchor = 4;  // 0..8, row-major 3x3
    int offsetX = 0, offsetY = -80, monitor = 1, lineGap = 4;
    ZcStyle style;
} g_zc;

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

void ZcApplyPreset(const std::wstring& name, ZcStyle& s) {
    const wchar_t* kFont = L"Segoe UI Variable Display";
    s.timeFont = kFont;
    s.dateFont = kFont;
    s.text = {1, 1, 1, 1};
    s.shadow = s.glow = s.outline = s.flip = false;
    s.rotation = 0;
    if (name == L"tilted") {
        // White large text, tilted ~-8 degrees, dark soft shadow.
        s.timeSize = 150;
        s.timeWeight = 700;
        s.dateSize = 44;
        s.dateWeight = 600;
        s.rotation = -8;
        s.shadow = true;
        s.shadowColor = {0, 0, 0, 0.75f};
        s.shadowBlur = 10;
        s.shadowDx = 6;
        s.shadowDy = 10;
    } else if (name == L"minimal") {
        s.timeFont = L"Segoe UI Variable Display Light";
        s.dateFont = kFont;
        s.timeSize = 120;
        s.timeWeight = 200;
        s.dateSize = 30;
        s.dateWeight = 400;
        s.text = {1, 1, 1, 0.92f};
        s.shadow = true;
        s.shadowColor = {0, 0, 0, 0.35f};
        s.shadowBlur = 4;
        s.shadowDx = 0;
        s.shadowDy = 2;
    } else if (name == L"neon") {
        s.timeFont = L"Bahnschrift";
        s.dateFont = L"Bahnschrift";
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
        s.dateFont = kFont;
        s.timeSize = 120;
        s.timeWeight = 700;
        s.dateSize = 34;
        s.dateWeight = 600;
        s.flip = true;
        s.cardColor = {0.11f, 0.11f, 0.125f, 0.92f};
        s.shadow = true;
        s.shadowColor = {0, 0, 0, 0.6f};
        s.shadowBlur = 8;
        s.shadowDx = 0;
        s.shadowDy = 6;
    }
}

void ZcLoadSettings() {
    ZcSettings z;
    z.use24 = Wh_GetIntSetting(L"use24Hour") != 0;
    z.seconds = Wh_GetIntSetting(L"showSeconds") != 0;

    std::wstring d = ZcStr(L"dateFormat");
    z.date = d == L"dmy"      ? ZcDate::dmy
             : d == L"iso"    ? ZcDate::iso
             : d == L"cn"     ? ZcDate::cn
             : d == L"cnFull" ? ZcDate::cnFull
             : d == L"none"   ? ZcDate::none
                              : ZcDate::mdy;
    std::wstring a = ZcStr(L"textAlign");
    z.align = a == L"center"  ? ZcAlign::center
              : a == L"right" ? ZcAlign::right
                              : ZcAlign::left;
    static const wchar_t* kAnchors[] = {L"topLeft", L"top",        L"topRight",
                                        L"left",    L"center",     L"right",
                                        L"bottomLeft", L"bottom", L"bottomRight"};
    std::wstring an = ZcStr(L"anchor");
    z.anchor = 4;
    for (int i = 0; i < 9; i++) {
        if (an == kAnchors[i]) {
            z.anchor = i;
        }
    }
    z.offsetX = Wh_GetIntSetting(L"offsetX");
    z.offsetY = Wh_GetIntSetting(L"offsetY");
    z.monitor = Wh_GetIntSetting(L"monitor");
    z.lineGap = ZcClamp(Wh_GetIntSetting(L"lineGap"), -200, 400);

    ZcStyle& s = z.style;
    std::wstring style = ZcStr(L"style");
    if (style != L"custom") {
        ZcApplyPreset(style.empty() ? L"tilted" : style, s);
    } else {
        s.timeFont = ZcStr(L"timeFontFamily");
        s.dateFont = ZcStr(L"dateFontFamily");
        if (s.timeFont.empty()) s.timeFont = L"Segoe UI Variable Display";
        if (s.dateFont.empty()) s.dateFont = s.timeFont;
        s.timeSize = Wh_GetIntSetting(L"timeFontSize");
        s.timeWeight = Wh_GetIntSetting(L"timeFontWeight");
        s.dateSize = Wh_GetIntSetting(L"dateFontSize");
        s.dateWeight = Wh_GetIntSetting(L"dateFontWeight");
        s.text = ZcColor(L"textColor", {1, 1, 1, 1});
        s.rotation = (float)ZcClamp(Wh_GetIntSetting(L"rotation"), -45, 45);
        s.shadow = Wh_GetIntSetting(L"shadowEnabled") != 0;
        s.shadowColor = ZcColor(L"shadowColor", {0, 0, 0, 1});
        s.shadowColor.a *=
            ZcClamp(Wh_GetIntSetting(L"shadowOpacity"), 0, 100) / 100.0f;
        s.shadowBlur = (float)ZcClamp(Wh_GetIntSetting(L"shadowBlur"), 0, 100);
        s.shadowDx = (float)Wh_GetIntSetting(L"shadowOffsetX");
        s.shadowDy = (float)Wh_GetIntSetting(L"shadowOffsetY");
        s.glow = Wh_GetIntSetting(L"glowEnabled") != 0;
        s.glowColor = ZcColor(L"glowColor", {0, 0.9f, 1, 1});
        s.glowRadius = (float)ZcClamp(Wh_GetIntSetting(L"glowRadius"), 1, 100);
        s.outline = Wh_GetIntSetting(L"outlineEnabled") != 0;
        s.outlineColor = ZcColor(L"outlineColor", {0, 0, 0, 1});
        s.outlineWidth = (float)ZcClamp(Wh_GetIntSetting(L"outlineWidth"), 1, 20);
        s.flip = Wh_GetIntSetting(L"flipCards") != 0;
        s.cardColor = ZcColor(L"cardColor", {0.125f, 0.125f, 0.14f, 0.88f});
    }
    s.timeSize = ZcClamp(s.timeSize, 8, 600);
    s.dateSize = ZcClamp(s.dateSize, 6, 300);
    s.timeWeight = ZcClamp(s.timeWeight ? s.timeWeight : 400, 100, 950);
    s.dateWeight = ZcClamp(s.dateWeight ? s.dateWeight : 400, 100, 950);

    g_zc = z;
    Wh_Log(L"settings: style=%s seconds=%d monitor=%d rot=%.0f", style.c_str(),
           (int)z.seconds, z.monitor, (double)s.rotation);
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
};

// Lay out the clock block at the origin; returns its size.
D2D1_SIZE_F ZcBuildPieces(float scale, std::vector<ZcPiece>& pieces) {
    const ZcStyle& s = g_zc.style;
    SYSTEMTIME t;
    GetLocalTime(&t);
    std::wstring hh, mm, ss, suffix, date;
    ZcFormat(t, hh, mm, ss, suffix, date);

    float timeSize = s.timeSize * scale;
    std::vector<ZcPiece> row;
    float x = 0, rowH = 0;
    auto add = [&](const std::wstring& txt, float size, bool card) {
        if (txt.empty()) return;
        ZcPiece p{ZcLayout(txt, s.timeFont, s.timeWeight, size), {}, {}, card};
        if (!p.layout) return;
        p.size = ZcMeasure(p.layout.Get());
        float pad = card ? size * 0.12f : 0;
        p.pos = D2D1::Point2F(x + pad, 0);
        x += p.size.width + pad * 2;
        rowH = rowH > p.size.height ? rowH : p.size.height;
        row.push_back(p);
    };
    float cardGap = timeSize * 0.08f;
    if (s.flip) {
        add(hh, timeSize, true);
        x += cardGap;
        add(mm, timeSize, true);
        if (!ss.empty()) {
            x += cardGap;
            add(ss, timeSize, true);
        }
    } else {
        std::wstring txt = hh + L":" + mm + (ss.empty() ? L"" : L":" + ss);
        add(txt, timeSize, false);
    }
    if (!suffix.empty()) {
        x += timeSize * 0.08f;
        float sfx = timeSize * 0.3f;
        add(suffix, sfx, false);
        // Bottom-align the AM/PM marker with the digits.
        row.back().pos.y = rowH - row.back().size.height - timeSize * 0.12f;
    }
    float rowW = x;

    ZcPiece datePiece{};
    float blockW = rowW, blockH = rowH;
    if (!date.empty()) {
        datePiece.layout = ZcLayout(date, s.dateFont, s.dateWeight,
                                    s.dateSize * scale);
        if (datePiece.layout) {
            datePiece.size = ZcMeasure(datePiece.layout.Get());
            blockW = blockW > datePiece.size.width ? blockW
                                                   : datePiece.size.width;
            blockH = rowH + g_zc.lineGap * scale + datePiece.size.height;
        }
    }
    auto alignX = [&](float w) {
        switch (g_zc.align) {
            case ZcAlign::center: return (blockW - w) / 2;
            case ZcAlign::right: return blockW - w;
            default: return 0.0f;
        }
    };
    float rowOff = alignX(rowW);
    for (auto& p : row) {
        p.pos.x += rowOff;
        pieces.push_back(p);
    }
    if (datePiece.layout) {
        datePiece.pos =
            D2D1::Point2F(alignX(datePiece.size.width), rowH + g_zc.lineGap * scale);
        pieces.push_back(datePiece);
    }
    return D2D1::SizeF(blockW, blockH);
}

// Records the clock content (cards + text in one color) at the origin.
ComPtr<ID2D1CommandList> ZcRecord(const std::vector<ZcPiece>& pieces,
                                  D2D1_COLOR_F color,
                                  bool withCards,
                                  float scale) {
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
    ComPtr<ID2D1SolidColorBrush> brush, cardBrush, lineBrush;
    g_zcDc->CreateSolidColorBrush(color, &brush);
    const ZcStyle& s = g_zc.style;
    if (withCards) {
        g_zcDc->CreateSolidColorBrush(s.cardColor, &cardBrush);
        g_zcDc->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.55f), &lineBrush);
    }
    for (const auto& p : pieces) {
        if (p.card && cardBrush) {
            float pad = p.size.height * 0.12f;
            D2D1_RECT_F r = D2D1::RectF(p.pos.x - pad, p.pos.y,
                                        p.pos.x + p.size.width + pad,
                                        p.pos.y + p.size.height);
            float radius = p.size.height * 0.08f;
            g_zcDc->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius),
                                         cardBrush.Get());
            float mid = (r.top + r.bottom) / 2;
            g_zcDc->DrawLine(D2D1::Point2F(r.left, mid),
                             D2D1::Point2F(r.right, mid), lineBrush.Get(),
                             2.0f * scale);
        }
        if (brush && p.layout) {
            g_zcDc->DrawTextLayout(p.pos, p.layout.Get(), brush.Get(),
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
    std::vector<ZcPiece> pieces;
    D2D1_SIZE_F size = ZcBuildPieces(scale, pieces);
    if (pieces.empty()) return;

    // Rotate around the block center, then move the center into place.
    D2D1_POINT_2F half = D2D1::Point2F(size.width / 2, size.height / 2);
    D2D1::Matrix3x2F place =
        D2D1::Matrix3x2F::Rotation(s.rotation, half) *
        D2D1::Matrix3x2F::Translation(center.x - half.x, center.y - half.y);

    ComPtr<ID2D1CommandList> content = ZcRecord(pieces, s.text, s.flip, scale);
    if (!content) return;

    if (s.shadow && s.shadowColor.a > 0) {
        if (auto fx = ZcShadowEffect(content.Get(), s.shadowColor,
                                     s.shadowBlur * scale)) {
            // Offset in screen space so the light comes from the same side
            // regardless of rotation.
            g_zcDc->SetTransform(
                place * D2D1::Matrix3x2F::Translation(s.shadowDx * scale,
                                                      s.shadowDy * scale));
            g_zcDc->DrawImage(fx.Get());
        }
    }
    g_zcDc->SetTransform(place);
    if (s.glow) {
        ComPtr<ID2D1CommandList> mask = ZcRecord(pieces, s.glowColor, false, scale);
        if (mask) {
            // Wide soft halo plus a tighter bright core.
            if (auto fx = ZcShadowEffect(mask.Get(), s.glowColor,
                                         s.glowRadius * scale)) {
                g_zcDc->DrawImage(fx.Get());
            }
            if (auto fx = ZcShadowEffect(mask.Get(), s.glowColor,
                                         s.glowRadius * scale * 0.35f)) {
                g_zcDc->DrawImage(fx.Get());
            }
        }
    }
    if (s.outline) {
        ComPtr<ID2D1CommandList> mask =
            ZcRecord(pieces, s.outlineColor, false, scale);
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
    g_zcDc->DrawImage(content.Get());
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
        ZcDrawBlock(c, scale);
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
