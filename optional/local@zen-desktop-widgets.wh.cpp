// ==WindhawkMod==
// @id              zen-desktop-widgets
// @name            ZenDesktop: 桌面小组件
// @name:zh-CN      ZenDesktop: 桌面小组件
// @description     Optional desktop clock and status cards with a glass look. Drag to move. Default off.
// @description:zh-CN 可选的桌面时钟与状态卡片，玻璃风格，可拖动。默认关闭。
// @version         0.1.1
// @donateUrl       https://ifdian.net/a/Lanboo
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -lole32 -ldwmapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

# ZenDesktop: 桌面小组件

在桌面上放置可拖动的玻璃风卡片。当前是 **MVP 脚手架**（见仓库 `docs/desktop-widgets.md`）：内置时钟与状态卡两块，设置里可开关与改字号。不是 Rainmeter 替代品。

## 功能

- 时钟卡片：大号时间 + 日期；可选显示秒。
- 状态卡片：自定义短文案，或显示内置提示。
- 左键拖动卡片移动；位置写入当前用户注册表，资源管理器重启后恢复。
- 玻璃底依赖系统「透明度效果」；关闭透明时退回半透明深色底。

## 设置说明

- **显示时钟 / 显示状态卡**：各自开关。默认都开。
- **主文字大小（像素）**：范围 16–64。默认 28。
- **时钟显示秒**：默认关闭。
- **状态卡文案**：留空则用内置「ZenDesktop 小组件 MVP」。

## 兼容性与已知问题

- 可选 Mod，部署后默认关闭；在 Windhawk 中打开并编译后才会出现。
- 注入 `explorer.exe`。与「桌面双击隐藏图标」可同时使用。
- 动态壁纸场景下层级可能异常，后续版本再挂 WorkerW。
- 架构与演进说明见 [docs/desktop-widgets.md](https://github.com/Liset999/ZenDesktop/blob/main/docs/desktop-widgets.md)。

## 更新日志

- **0.1.1**：性能：时钟文案未变不重绘、USERDATA 找控件、GDI 笔刷复用、状态默认文案零分配。
- **0.1.0**：MVP：时钟 + 状态卡、拖动与位置记忆、Windhawk 设置。

## 致谢与许可

- ZenDesktop 原创 optional Mod，GPL-3.0。
- 开发者：Lanbo（[哔哩哔哩](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo)）
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showClock: true
  $name: "Show clock"
  $name:zh-CN: "显示时钟"
  $description: "Show the clock card on the desktop. Default: on."
  $description:zh-CN: "在桌面显示时钟卡片。默认开启。"
- showStatus: true
  $name: "Show status card"
  $name:zh-CN: "显示状态卡"
  $description: "Show the status text card. Default: on."
  $description:zh-CN: "在桌面显示状态卡片。默认开启。"
- fontSize: 28
  $name: "Font size (px)"
  $name:zh-CN: "主文字大小（像素）"
  $description: "Primary text size in pixels, 16–64. Default: 28."
  $description:zh-CN: "主文字大小，范围 16–64 像素。默认 28。"
- clockShowSeconds: false
  $name: "Clock shows seconds"
  $name:zh-CN: "时钟显示秒"
  $description: "Include seconds in the clock. Default: off."
  $description:zh-CN: "时钟是否显示秒。默认关闭。"
- statusText: ""
  $name: "Status text"
  $name:zh-CN: "状态卡文案"
  $description: "Custom status line. Empty uses the built-in MVP tip."
  $description:zh-CN: "状态卡自定义文案。留空则用内置 MVP 提示。"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <string>
#include <vector>

#ifndef WCA_ACCENT_POLICY
#define WCA_ACCENT_POLICY 19
#endif

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
};

struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    int GradientColor;  // ABGR
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    int Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
static SetWindowCompositionAttribute_t g_pSetWindowCompositionAttribute = nullptr;

static constexpr wchar_t kRegPath[] = L"Software\\ZenDesktop\\DesktopWidgets";
static constexpr wchar_t kClassName[] = L"ZenDesktop.DesktopWidget";
static constexpr UINT_PTR kTimerRefresh = 1;
static constexpr int kPad = 18;

enum class WidgetKind { Clock, Status };

struct Settings {
    bool showClock = true;
    bool showStatus = true;
    int fontSize = 28;
    bool clockShowSeconds = false;
    std::wstring statusText;
} g_settings;

struct Widget {
    WidgetKind kind;
    HWND hwnd = nullptr;
    int x = 80;
    int y = 80;
    int width = 280;
    int height = 110;
    bool dragging = false;
    POINT dragStart{};
    POINT winStart{};
    wchar_t lastLine1[64]{};
    wchar_t lastLine2[128]{};
};

static std::vector<Widget> g_widgets;
static ATOM g_atom = 0;
static HFONT g_fontLarge = nullptr;
static HFONT g_fontSmall = nullptr;
static HBRUSH g_brushCardBg = nullptr;
static bool g_classRegistered = false;
static constexpr wchar_t kDefaultStatusText[] = L"ZenDesktop 小组件 MVP";

static void LoadSettings();
static void RebuildFonts();
static void ApplyGlass(HWND hwnd);
static void EnsureWidgets();
static void DestroyAllWidgets();
static void PersistPosition(const Widget& w);
static void LoadPosition(Widget& w);
static LRESULT CALLBACK WidgetWndProc(HWND, UINT, WPARAM, LPARAM);
static void PaintWidget(HWND hwnd, Widget* w);
static Widget* FindWidget(HWND hwnd);
static std::wstring DefaultStatusText();

static int Clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void LoadSettings() {
    g_settings.showClock = Wh_GetIntSetting(L"showClock") != 0;
    g_settings.showStatus = Wh_GetIntSetting(L"showStatus") != 0;
    g_settings.fontSize = Clamp(Wh_GetIntSetting(L"fontSize"), 16, 64);
    g_settings.clockShowSeconds = Wh_GetIntSetting(L"clockShowSeconds") != 0;

    g_settings.statusText.clear();
    if (PCWSTR raw = Wh_GetStringSetting(L"statusText")) {
        g_settings.statusText = raw;
        Wh_FreeStringSetting(raw);
    }
}

static const wchar_t* DefaultStatusText() {
    return kDefaultStatusText;
}

static void RebuildFonts() {
    if (g_fontLarge) {
        DeleteObject(g_fontLarge);
        g_fontLarge = nullptr;
    }
    if (g_fontSmall) {
        DeleteObject(g_fontSmall);
        g_fontSmall = nullptr;
    }
    g_fontLarge = CreateFontW(
        -g_settings.fontSize, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Display");
    if (!g_fontLarge) {
        g_fontLarge = CreateFontW(
            -g_settings.fontSize, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    }
    int smallSize = Clamp(g_settings.fontSize * 45 / 100, 12, 28);
    g_fontSmall = CreateFontW(
        -smallSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
}

static void ApplyGlass(HWND hwnd) {
    if (!g_pSetWindowCompositionAttribute) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            g_pSetWindowCompositionAttribute =
                reinterpret_cast<SetWindowCompositionAttribute_t>(
                    GetProcAddress(user32, "SetWindowCompositionAttribute"));
        }
    }

    // ABGR: alpha ~0x99, tint close to ZenDesktop dark glass (#232323)
    const int tint = static_cast<int>(0x99232323);

    if (g_pSetWindowCompositionAttribute) {
        ACCENT_POLICY policy{};
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        policy.GradientColor = tint;
        WINDOWCOMPOSITIONATTRIBDATA data{};
        data.Attribute = WCA_ACCENT_POLICY;
        data.Data = &policy;
        data.SizeOfData = sizeof(policy);
        if (g_pSetWindowCompositionAttribute(hwnd, &data)) {
            return;
        }
        policy.AccentState = ACCENT_ENABLE_BLURBEHIND;
        g_pSetWindowCompositionAttribute(hwnd, &data);
        return;
    }

    // Fallback: layered translucent dark fill
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE,
                      GetWindowLongPtrW(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 200, LWA_ALPHA);
}

static const wchar_t* RegNameX(WidgetKind k) {
    return k == WidgetKind::Clock ? L"ClockX" : L"StatusX";
}
static const wchar_t* RegNameY(WidgetKind k) {
    return k == WidgetKind::Clock ? L"ClockY" : L"StatusY";
}

static void PersistPosition(const Widget& w) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRegPath, 0, nullptr, 0, KEY_WRITE, nullptr, &key,
                        nullptr) != ERROR_SUCCESS) {
        return;
    }
    DWORD x = static_cast<DWORD>(w.x);
    DWORD y = static_cast<DWORD>(w.y);
    RegSetValueExW(key, RegNameX(w.kind), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&x),
                   sizeof(x));
    RegSetValueExW(key, RegNameY(w.kind), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&y),
                   sizeof(y));
    RegCloseKey(key);
}

static void LoadPosition(Widget& w) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegPath, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return;
    }
    DWORD type = 0;
    DWORD x = 0, y = 0;
    DWORD size = sizeof(DWORD);
    if (RegQueryValueExW(key, RegNameX(w.kind), nullptr, &type, reinterpret_cast<LPBYTE>(&x),
                         &size) == ERROR_SUCCESS &&
        type == REG_DWORD) {
        w.x = static_cast<int>(x);
    }
    size = sizeof(DWORD);
    if (RegQueryValueExW(key, RegNameY(w.kind), nullptr, &type, reinterpret_cast<LPBYTE>(&y),
                         &size) == ERROR_SUCCESS &&
        type == REG_DWORD) {
        w.y = static_cast<int>(y);
    }
    RegCloseKey(key);
}

static Widget* FindWidget(HWND hwnd) {
    if (!hwnd) return nullptr;
    if (auto* w = reinterpret_cast<Widget*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA))) {
        return w;
    }
    for (auto& w : g_widgets) {
        if (w.hwnd == hwnd) {
            return &w;
        }
    }
    return nullptr;
}

static void EnsureCardGdi() {
    if (!g_brushCardBg) {
        g_brushCardBg = CreateSolidBrush(RGB(0x23, 0x23, 0x23));
    }
}

static void DestroyCardGdi() {
    if (g_brushCardBg) {
        DeleteObject(g_brushCardBg);
        g_brushCardBg = nullptr;
    }
}

static void RebindWidgetUserData() {
    for (auto& w : g_widgets) {
        if (w.hwnd && IsWindow(w.hwnd)) {
            SetWindowLongPtrW(w.hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&w));
        }
    }
}

static void MeasureClock(HDC hdc, int* outW, int* outH) {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    wchar_t line1[64];
    wchar_t line2[64];
    if (g_settings.clockShowSeconds) {
        swprintf_s(line1, L"%02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);
    } else {
        swprintf_s(line1, L"%02u:%02u", st.wHour, st.wMinute);
    }
    swprintf_s(line2, L"%04u-%02u-%02u", st.wYear, st.wMonth, st.wDay);

    SIZE s1{}, s2{};
    HFONT old = static_cast<HFONT>(SelectObject(hdc, g_fontLarge));
    GetTextExtentPoint32W(hdc, line1, static_cast<int>(wcslen(line1)), &s1);
    SelectObject(hdc, g_fontSmall);
    GetTextExtentPoint32W(hdc, line2, static_cast<int>(wcslen(line2)), &s2);
    SelectObject(hdc, old);

    *outW = (s1.cx > s2.cx ? s1.cx : s2.cx) + kPad * 2;
    *outH = s1.cy + s2.cy + kPad * 2 + 8;
    *outW = Clamp(*outW, 180, 480);
    *outH = Clamp(*outH, 80, 220);
}

static void MeasureStatus(HDC hdc, int* outW, int* outH) {
    const wchar_t* text = g_settings.statusText.empty() ? DefaultStatusText()
                                                        : g_settings.statusText.c_str();
    SIZE s{};
    HFONT old = static_cast<HFONT>(SelectObject(hdc, g_fontSmall));
    GetTextExtentPoint32W(hdc, text, static_cast<int>(wcslen(text)), &s);
    SelectObject(hdc, old);
    *outW = Clamp(s.cx + kPad * 2, 160, 520);
    *outH = Clamp(s.cy + kPad * 2, 48, 160);
}

static void PaintWidget(HWND hwnd, Widget* w) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc{};
    GetClientRect(hwnd, &rc);

    EnsureCardGdi();
    FillRect(hdc, &rc, g_brushCardBg);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0xF2, 0xF2, 0xF2));

    if (w->kind == WidgetKind::Clock) {
        SYSTEMTIME st{};
        GetLocalTime(&st);
        wchar_t line1[64];
        wchar_t line2[128];
        if (g_settings.clockShowSeconds) {
            swprintf_s(line1, L"%02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);
        } else {
            swprintf_s(line1, L"%02u:%02u", st.wHour, st.wMinute);
        }
        static const wchar_t* kWeek[] = {L"星期日", L"星期一", L"星期二", L"星期三",
                                         L"星期四", L"星期五", L"星期六"};
        swprintf_s(line2, L"%04u-%02u-%02u  %s", st.wYear, st.wMonth, st.wDay,
                   kWeek[st.wDayOfWeek % 7]);

        RECT r1 = rc;
        r1.top += kPad;
        r1.left += kPad;
        r1.right -= kPad;
        HFONT old = static_cast<HFONT>(SelectObject(hdc, g_fontLarge));
        DrawTextW(hdc, line1, -1, &r1, DT_LEFT | DT_TOP | DT_SINGLELINE);

        SIZE s1{};
        GetTextExtentPoint32W(hdc, line1, static_cast<int>(wcslen(line1)), &s1);
        RECT r2 = rc;
        r2.top = r1.top + s1.cy + 6;
        r2.left += kPad;
        r2.right -= kPad;
        SelectObject(hdc, g_fontSmall);
        SetTextColor(hdc, RGB(0xC8, 0xC8, 0xC8));
        DrawTextW(hdc, line2, -1, &r2, DT_LEFT | DT_TOP | DT_SINGLELINE);
        SelectObject(hdc, old);
    } else {
        const wchar_t* text = g_settings.statusText.empty() ? DefaultStatusText()
                                                            : g_settings.statusText.c_str();
        RECT r = rc;
        InflateRect(&r, -kPad, -kPad);
        HFONT old = static_cast<HFONT>(SelectObject(hdc, g_fontSmall));
        DrawTextW(hdc, text, -1, &r, DT_LEFT | DT_VCENTER | DT_WORDBREAK);
        SelectObject(hdc, old);
    }

    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Widget* w = FindWidget(hwnd);

    switch (msg) {
        case WM_NCHITTEST:
            return HTCLIENT;

        case WM_LBUTTONDOWN:
            if (w) {
                SetCapture(hwnd);
                w->dragging = true;
                GetCursorPos(&w->dragStart);
                w->winStart = {w->x, w->y};
            }
            return 0;

        case WM_MOUSEMOVE:
            if (w && w->dragging && (wParam & MK_LBUTTON)) {
                POINT pt{};
                GetCursorPos(&pt);
                w->x = w->winStart.x + (pt.x - w->dragStart.x);
                w->y = w->winStart.y + (pt.y - w->dragStart.y);
                SetWindowPos(hwnd, nullptr, w->x, w->y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;

        case WM_LBUTTONUP:
            if (w && w->dragging) {
                w->dragging = false;
                ReleaseCapture();
                PersistPosition(*w);
            }
            return 0;

        case WM_TIMER:
            if (wParam == kTimerRefresh && w && w->kind == WidgetKind::Clock) {
                SYSTEMTIME st{};
                GetLocalTime(&st);
                wchar_t line1[64];
                wchar_t line2[128];
                if (g_settings.clockShowSeconds) {
                    swprintf_s(line1, L"%02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(line1, L"%02u:%02u", st.wHour, st.wMinute);
                }
                swprintf_s(line2, L"%04u-%02u-%02u", st.wYear, st.wMonth, st.wDay);
                if (wcscmp(line1, w->lastLine1) != 0 || wcscmp(line2, w->lastLine2) != 0) {
                    wcscpy_s(w->lastLine1, line1);
                    wcscpy_s(w->lastLine2, line2);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            if (w) {
                PaintWidget(hwnd, w);
            }
            return 0;

        case WM_DESTROY:
            if (w) {
                KillTimer(hwnd, kTimerRefresh);
                w->hwnd = nullptr;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool EnsureClass() {
    if (g_classRegistered) {
        return true;
    }
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WidgetWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    g_atom = RegisterClassExW(&wc);
    if (!g_atom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"[ZenWidgets] RegisterClassEx failed: %lu", GetLastError());
        return false;
    }
    g_classRegistered = true;
    return true;
}

static HWND CreateWidgetWindow(Widget& w) {
    if (!EnsureClass()) {
        return nullptr;
    }

    HDC screen = GetDC(nullptr);
    int width = 240;
    int height = 100;
    if (w.kind == WidgetKind::Clock) {
        MeasureClock(screen, &width, &height);
    } else {
        MeasureStatus(screen, &width, &height);
    }
    ReleaseDC(nullptr, screen);
    w.width = width;
    w.height = height;

    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName, L"ZenDesktop Widget", WS_POPUP,
        w.x, w.y, w.width, w.height, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!hwnd) {
        Wh_Log(L"[ZenWidgets] CreateWindowEx failed: %lu", GetLastError());
        return nullptr;
    }

    w.hwnd = hwnd;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&w));
    ApplyGlass(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (w.kind == WidgetKind::Clock) {
        UINT interval = g_settings.clockShowSeconds ? 1000 : 15000;
        SetTimer(hwnd, kTimerRefresh, interval, nullptr);
    }
    InvalidateRect(hwnd, nullptr, FALSE);
    return hwnd;
}

static void DestroyAllWidgets() {
    for (auto& w : g_widgets) {
        if (w.hwnd && IsWindow(w.hwnd)) {
            DestroyWindow(w.hwnd);
        }
        w.hwnd = nullptr;
    }
    g_widgets.clear();
}

static void EnsureWidgets() {
    DestroyAllWidgets();
    RebuildFonts();
    g_widgets.reserve(2);

    int primaryRight = GetSystemMetrics(SM_CXSCREEN);

    if (g_settings.showClock) {
        Widget clock{};
        clock.kind = WidgetKind::Clock;
        clock.x = primaryRight - 320;
        clock.y = 48;
        LoadPosition(clock);
        g_widgets.push_back(clock);
        if (!CreateWidgetWindow(g_widgets.back())) {
            g_widgets.pop_back();
        }
    }

    if (g_settings.showStatus) {
        Widget status{};
        status.kind = WidgetKind::Status;
        status.x = primaryRight - 320;
        status.y = 180;
        LoadPosition(status);
        g_widgets.push_back(status);
        if (!CreateWidgetWindow(g_widgets.back())) {
            g_widgets.pop_back();
        }
    }

    RebindWidgetUserData();
    Wh_Log(L"[ZenWidgets] Active widgets: %zu", g_widgets.size());
}

BOOL Wh_ModInit() {
    Wh_Log(L"[ZenWidgets] === Wh_ModInit v0.1.1 ===");
    LoadSettings();
    EnsureWidgets();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[ZenWidgets] === Wh_ModUninit ===");
    DestroyAllWidgets();
    DestroyCardGdi();
    if (g_fontLarge) {
        DeleteObject(g_fontLarge);
        g_fontLarge = nullptr;
    }
    if (g_fontSmall) {
        DeleteObject(g_fontSmall);
        g_fontSmall = nullptr;
    }
    if (g_classRegistered) {
        UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
        g_classRegistered = false;
        g_atom = 0;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[ZenWidgets] === Settings changed ===");
    LoadSettings();
    EnsureWidgets();
}
