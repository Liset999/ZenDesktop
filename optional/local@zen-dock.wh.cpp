// ==WindhawkMod==
// @id              zen-dock
// @name            ZenDesktop: Dock
// @name:zh-CN      ZenDesktop: Dock
// @description     Optional Nexus-style glass app launcher bar. Companion to the taskbar; default off.
// @description:zh-CN 可选的 Nexus 风格玻璃快捷栏。与系统任务栏并存，不替代任务栏。默认关闭。
// @version         0.1.0
// @donateUrl       https://ifdian.net/a/Lanboo
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -lole32 -ldwmapi -lshell32 -lshlwapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

# ZenDesktop: Dock

一条底部居中（可改贴边）的玻璃快捷应用栏，风格贴近 ZenDesktop 其它面板。**不会隐藏或替换系统任务栏**，只是额外的启动器；详见仓库 `docs/nexus-dock.md`。

## 功能

- 玻璃底板 + 钉选应用图标；点击启动或聚焦已开窗口。
- 悬停高亮（可选轻微放大）；运行中在图标下显示小圆点。
- 钉选列表在设置里按路径填写（`.exe` / `.lnk`）；空列表时用内置常用项。
- 位置相对工作区（避开任务栏）；可改底部/左侧/右侧。

## 设置说明

- **边缘位置**：底部居中、左侧、右侧。默认底部居中。
- **图标大小（像素）**：范围 28–64。默认 40。
- **悬停放大**：悬停时图标略放大。默认开启。
- **显示运行指示点**：该应用已有可见窗口时显示圆点。默认开启。
- **钉选应用路径**：每行一个完整路径。留空则使用记事本、资源管理器、计算器、cmd 等内置默认。

## 兼容性与已知问题

- 可选 Mod，部署后默认关闭；在 Windhawk 中打开并编译后才会出现。
- 注入 `explorer.exe`。与「任务栏美化」可同时开，视觉上会有两条栏——可自行藏系统任务栏，或只开其一。
- 全屏游戏时尽量自动隐藏（查询用户通知状态）；个别全屏模式可能仍露出。
- MVP **不做** macOS 式「从哪来回哪去」最小化轨迹，也不做复杂多级放大鱼眼。
- 架构与演进见 [docs/nexus-dock.md](https://github.com/Liset999/ZenDesktop/blob/main/docs/nexus-dock.md)。

## 更新日志

- **0.1.0**：MVP：玻璃 Dock、钉选启动、悬停高亮、运行指示、三边位置。

## 致谢与许可

- ZenDesktop 原创 optional Mod，GPL-3.0。
- 开发者：Lanbo（[哔哩哔哩](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo)）
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- edge: bottom
  $name: "Dock edge"
  $name:zh-CN: "边缘位置"
  $description: "Where the dock sits on the primary work area. Default: bottom center."
  $description:zh-CN: "Dock 相对主显示器工作区的位置。默认为底部居中。"
  $options:
  - bottom: "底部居中"
  - left: "左侧"
  - right: "右侧"
- iconSize: 40
  $name: "Icon size (px)"
  $name:zh-CN: "图标大小（像素）"
  $description: "Pinned icon size in pixels, 28–64. Default: 40."
  $description:zh-CN: "钉选图标大小，范围 28–64 像素。默认 40。"
- hoverMagnify: true
  $name: "Hover magnify"
  $name:zh-CN: "悬停放大"
  $description: "Slightly enlarge the icon under the cursor. Default: on."
  $description:zh-CN: "悬停时略微放大指针下的图标。默认开启。"
- showRunningDot: true
  $name: "Running indicator"
  $name:zh-CN: "显示运行指示点"
  $description: "Show a small dot under icons whose app has a visible window. Default: on."
  $description:zh-CN: "该应用已有可见窗口时在图标下显示小圆点。默认开启。"
- autoHideFullscreen: true
  $name: "Hide in fullscreen"
  $name:zh-CN: "全屏时隐藏"
  $description: "Hide the dock while a fullscreen app is active when possible. Default: on."
  $description:zh-CN: "在可能时于全屏应用期间隐藏 Dock。默认开启。"
- pinnedApps: [""]
  $name: "Pinned app paths"
  $name:zh-CN: "钉选应用路径"
  $description: "One full path per line (.exe or .lnk). Empty list uses built-in defaults (Notepad, Explorer, Calculator, cmd)."
  $description:zh-CN: "每行一个完整路径（.exe 或 .lnk）。全部留空则使用内置默认（记事本、资源管理器、计算器、cmd）。"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <algorithm>

#pragma comment(lib, "shlwapi.lib")

#ifndef QUERY_USER_NOTIFICATION_STATE
typedef enum {
    QUNS_NOT_PRESENT = 1,
    QUNS_BUSY = 2,
    QUNS_RUNNING_D3D_FULL_SCREEN = 3,
    QUNS_PRESENTATION_MODE = 4,
    QUNS_ACCEPTS_NOTIFICATIONS = 5,
    QUNS_QUIET_TIME = 6,
    QUNS_APP = 7
} QUERY_USER_NOTIFICATION_STATE;
#endif

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
    int GradientColor;
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    int Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
static SetWindowCompositionAttribute_t g_pSetWindowCompositionAttribute = nullptr;

static constexpr wchar_t kRegPath[] = L"Software\\ZenDesktop\\Dock";
static constexpr wchar_t kClassName[] = L"ZenDesktop.DockBar";
static constexpr UINT_PTR kTimerRefresh = 1;
static constexpr UINT_PTR kTimerFullscreen = 2;
static constexpr int kPadX = 14;
static constexpr int kPadY = 10;
static constexpr int kGap = 10;
static constexpr int kMarginFromWorkArea = 8;

enum class DockEdge { Bottom, Left, Right };

struct Settings {
    DockEdge edge = DockEdge::Bottom;
    int iconSize = 40;
    bool hoverMagnify = true;
    bool showRunningDot = true;
    bool autoHideFullscreen = true;
    std::vector<std::wstring> pinnedPaths;
} g_settings;

struct PinItem {
    std::wstring path;
    std::wstring exeNameLower;  // file name lower, for running match
    HICON icon = nullptr;
    bool running = false;
};

static HWND g_hwnd = nullptr;
static ATOM g_atom = 0;
static bool g_classRegistered = false;
static std::vector<PinItem> g_pins;
static int g_hoverIndex = -1;
static bool g_hiddenFullscreen = false;
static int g_dockW = 0;
static int g_dockH = 0;

static void LoadSettings();
static void ApplyGlass(HWND hwnd);
static void RebuildPins();
static void DestroyPins();
static void LayoutAndShow();
static LRESULT CALLBACK DockWndProc(HWND, UINT, WPARAM, LPARAM);
static void PaintDock(HWND hwnd);
static void UpdateRunningFlags();
static void LaunchOrFocus(const PinItem& pin);
static int HitTestIcon(int x, int y);
static void IconRect(int index, RECT* out, bool magnified);
static bool IsFullscreenBusy();

static int Clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static std::wstring ToLowerCopy(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

static std::wstring FileNameLower(const std::wstring& path) {
    const wchar_t* base = PathFindFileNameW(path.c_str());
    std::wstring name = base ? base : path;
    return ToLowerCopy(name);
}

static std::vector<std::wstring> DefaultPinnedPaths() {
    wchar_t windir[MAX_PATH]{};
    GetWindowsDirectoryW(windir, MAX_PATH);
    std::wstring w = windir;
    std::vector<std::wstring> out;
    out.push_back(w + L"\\explorer.exe");
    out.push_back(w + L"\\System32\\notepad.exe");
    out.push_back(w + L"\\System32\\calc.exe");
    out.push_back(w + L"\\System32\\cmd.exe");
    return out;
}

static void LoadSettings() {
    g_settings.iconSize = Clamp(Wh_GetIntSetting(L"iconSize"), 28, 64);
    g_settings.hoverMagnify = Wh_GetIntSetting(L"hoverMagnify") != 0;
    g_settings.showRunningDot = Wh_GetIntSetting(L"showRunningDot") != 0;
    g_settings.autoHideFullscreen = Wh_GetIntSetting(L"autoHideFullscreen") != 0;

    g_settings.edge = DockEdge::Bottom;
    if (PCWSTR edge = Wh_GetStringSetting(L"edge")) {
        if (_wcsicmp(edge, L"left") == 0) g_settings.edge = DockEdge::Left;
        else if (_wcsicmp(edge, L"right") == 0) g_settings.edge = DockEdge::Right;
        Wh_FreeStringSetting(edge);
    }

    g_settings.pinnedPaths.clear();
    for (int i = 0; i < 64; i++) {
        PCWSTR value = Wh_GetStringSetting(L"pinnedApps[%d]", i);
        const bool present = value && *value;
        if (present) {
            std::wstring p = value;
            // trim
            const size_t first = p.find_first_not_of(L" \t\"");
            const size_t last = p.find_last_not_of(L" \t\"");
            if (first != std::wstring::npos) {
                p = p.substr(first, last - first + 1);
                if (!p.empty()) g_settings.pinnedPaths.push_back(p);
            }
        }
        if (value) Wh_FreeStringSetting(value);
        if (!present) break;
    }
    if (g_settings.pinnedPaths.empty()) {
        g_settings.pinnedPaths = DefaultPinnedPaths();
    }
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
    const int tint = static_cast<int>(0xA0232323);
    if (g_pSetWindowCompositionAttribute) {
        ACCENT_POLICY policy{};
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        policy.GradientColor = tint;
        WINDOWCOMPOSITIONATTRIBDATA data{};
        data.Attribute = WCA_ACCENT_POLICY;
        data.Data = &policy;
        data.SizeOfData = sizeof(policy);
        if (g_pSetWindowCompositionAttribute(hwnd, &data)) return;
        policy.AccentState = ACCENT_ENABLE_BLURBEHIND;
        g_pSetWindowCompositionAttribute(hwnd, &data);
        return;
    }
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE,
                      GetWindowLongPtrW(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 210, LWA_ALPHA);
}

static void DestroyPins() {
    for (auto& p : g_pins) {
        if (p.icon) {
            DestroyIcon(p.icon);
            p.icon = nullptr;
        }
    }
    g_pins.clear();
}

static HICON LoadPinIcon(const std::wstring& path, int /*size*/) {
    HICON large = nullptr;
    HICON small = nullptr;
    UINT extracted = ExtractIconExW(path.c_str(), 0, &large, &small, 1);
    if (extracted > 0) {
        HICON use = large ? large : small;
        if (large && small && large != small) {
            DestroyIcon(small);
        }
        if (use) return use;
    }
    SHFILEINFOW sfi{};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                       SHGFI_ICON | SHGFI_LARGEICON) && sfi.hIcon) {
        return sfi.hIcon;
    }
    HICON stock = static_cast<HICON>(LoadIconW(nullptr, IDI_APPLICATION));
    return stock ? CopyIcon(stock) : nullptr;
}

static void RebuildPins() {
    DestroyPins();
    for (const auto& path : g_settings.pinnedPaths) {
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"[ZenDock] skip missing: %s", path.c_str());
            continue;
        }
        PinItem item{};
        item.path = path;
        item.exeNameLower = FileNameLower(path);
        // For .lnk, still use the lnk path for ShellExecute; icon from lnk
        item.icon = LoadPinIcon(path, g_settings.iconSize);
        g_pins.push_back(std::move(item));
    }
    Wh_Log(L"[ZenDock] pins: %zu", g_pins.size());
}

struct EnumRunCtx {
    std::vector<std::wstring>* names;
    std::vector<char>* running;  // parallel flags
};

static BOOL CALLBACK EnumRunProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    if (GetWindow(hwnd, GW_OWNER)) return TRUE;
    LONG ex = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return TRUE;
    wchar_t title[4];
    if (GetWindowTextW(hwnd, title, 4) == 0) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return TRUE;
    wchar_t path[MAX_PATH]{};
    DWORD size = MAX_PATH;
    BOOL ok = QueryFullProcessImageNameW(h, 0, path, &size);
    CloseHandle(h);
    if (!ok) return TRUE;
    std::wstring name = FileNameLower(path);
    auto* ctx = reinterpret_cast<EnumRunCtx*>(lParam);
    for (size_t i = 0; i < ctx->names->size(); i++) {
        if ((*ctx->names)[i] == name) (*ctx->running)[i] = 1;
    }
    return TRUE;
}

static void UpdateRunningFlags() {
    if (!g_settings.showRunningDot || g_pins.empty()) return;
    std::vector<std::wstring> names;
    names.reserve(g_pins.size());
    for (auto& p : g_pins) names.push_back(p.exeNameLower);
    std::vector<char> flags(g_pins.size(), 0);
    EnumRunCtx ctx{&names, &flags};
    EnumWindows(EnumRunProc, reinterpret_cast<LPARAM>(&ctx));
    for (size_t i = 0; i < g_pins.size(); i++) {
        g_pins[i].running = flags[i] != 0;
    }
}

// Resolve .lnk target exe name for better running match
static bool IsFullscreenBusy() {
    if (!g_settings.autoHideFullscreen) return false;
    QUERY_USER_NOTIFICATION_STATE state = QUNS_NOT_PRESENT;
    if (FAILED(SHQueryUserNotificationState(&state))) return false;
    return state == QUNS_RUNNING_D3D_FULL_SCREEN ||
           state == QUNS_BUSY ||
           state == QUNS_PRESENTATION_MODE;
}

static void ComputeDockSize(int* outW, int* outH) {
    const int n = static_cast<int>(g_pins.size());
    const int icon = g_settings.iconSize;
    const int magExtra = g_settings.hoverMagnify ? icon / 5 : 0;
    if (g_settings.edge == DockEdge::Bottom) {
        *outW = kPadX * 2 + (n > 0 ? n * icon + (n - 1) * kGap : icon) + magExtra;
        *outH = kPadY * 2 + icon + magExtra + 8;  // room for running dot
    } else {
        *outW = kPadX * 2 + icon + magExtra + 8;
        *outH = kPadY * 2 + (n > 0 ? n * icon + (n - 1) * kGap : icon) + magExtra;
    }
    *outW = Clamp(*outW, 64, 1200);
    *outH = Clamp(*outH, 48, 1200);
}

static void PlaceDockWindow() {
    if (!g_hwnd) return;
    RECT work{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    ComputeDockSize(&g_dockW, &g_dockH);
    int x = 0, y = 0;
    if (g_settings.edge == DockEdge::Bottom) {
        x = work.left + ((work.right - work.left) - g_dockW) / 2;
        y = work.bottom - g_dockH - kMarginFromWorkArea;
    } else if (g_settings.edge == DockEdge::Left) {
        x = work.left + kMarginFromWorkArea;
        y = work.top + ((work.bottom - work.top) - g_dockH) / 2;
    } else {
        x = work.right - g_dockW - kMarginFromWorkArea;
        y = work.top + ((work.bottom - work.top) - g_dockH) / 2;
    }
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, g_dockW, g_dockH,
                 SWP_NOACTIVATE);
}

static void IconRect(int index, RECT* out, bool magnified) {
    const int icon = g_settings.iconSize;
    const int draw = magnified ? icon + icon / 5 : icon;
    const int n = static_cast<int>(g_pins.size());
    if (g_settings.edge == DockEdge::Bottom) {
        int totalIcons = n * icon + (n > 0 ? (n - 1) * kGap : 0);
        int startX = (g_dockW - totalIcons) / 2;
        int x = startX + index * (icon + kGap) - (draw - icon) / 2;
        int y = (g_dockH - draw - 8) / 2;
        out->left = x;
        out->top = y;
        out->right = x + draw;
        out->bottom = y + draw;
    } else {
        int totalIcons = n * icon + (n > 0 ? (n - 1) * kGap : 0);
        int startY = (g_dockH - totalIcons) / 2;
        int y = startY + index * (icon + kGap) - (draw - icon) / 2;
        int x = (g_dockW - draw - 8) / 2;
        out->left = x;
        out->top = y;
        out->right = x + draw;
        out->bottom = y + draw;
    }
}

static int HitTestIcon(int x, int y) {
    for (int i = 0; i < static_cast<int>(g_pins.size()); i++) {
        RECT rc{};
        IconRect(i, &rc, false);
        InflateRect(&rc, 4, 4);
        if (x >= rc.left && x < rc.right && y >= rc.top && y < rc.bottom) return i;
    }
    return -1;
}

static void LaunchOrFocus(const PinItem& pin) {
    // Try find running main window for same exe name
    struct FindCtx {
        std::wstring name;
        HWND found;
    } ctx{pin.exeNameLower, nullptr};

    auto enumFocus = [](HWND hwnd, LPARAM lp) -> BOOL {
        auto* c = reinterpret_cast<FindCtx*>(lp);
        if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER)) return TRUE;
        LONG ex = GetWindowLongW(hwnd, GWL_EXSTYLE);
        if (ex & WS_EX_TOOLWINDOW) return TRUE;
        wchar_t title[4];
        if (GetWindowTextW(hwnd, title, 4) == 0) return TRUE;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!h) return TRUE;
        wchar_t path[MAX_PATH]{};
        DWORD size = MAX_PATH;
        BOOL ok = QueryFullProcessImageNameW(h, 0, path, &size);
        CloseHandle(h);
        if (!ok) return TRUE;
        if (FileNameLower(path) == c->name) {
            c->found = hwnd;
            return FALSE;
        }
        return TRUE;
    };
    EnumWindows(enumFocus, reinterpret_cast<LPARAM>(&ctx));
    if (ctx.found) {
        if (IsIconic(ctx.found)) ShowWindow(ctx.found, SW_RESTORE);
        SetForegroundWindow(ctx.found);
        return;
    }

    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = pin.path.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"[ZenDock] ShellExecuteEx failed %lu for %s", GetLastError(),
               pin.path.c_str());
    }
}

static void PaintDock(HWND hwnd) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc{};
    GetClientRect(hwnd, &rc);

    HBRUSH brush = CreateSolidBrush(RGB(0x23, 0x23, 0x23));
    FillRect(hdc, &rc, brush);
    DeleteObject(brush);

    // subtle border
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0x55, 0x55, 0x55));
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 18, 18);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);

    for (int i = 0; i < static_cast<int>(g_pins.size()); i++) {
        const bool hot = (i == g_hoverIndex);
        const bool mag = hot && g_settings.hoverMagnify;
        RECT ir{};
        IconRect(i, &ir, mag);

        if (hot) {
            HBRUSH hb = CreateSolidBrush(RGB(0x45, 0x45, 0x45));
            RECT bg = ir;
            InflateRect(&bg, 6, 6);
            FillRect(hdc, &bg, hb);
            DeleteObject(hb);
        }

        if (g_pins[i].icon) {
            DrawIconEx(hdc, ir.left, ir.top, g_pins[i].icon, ir.right - ir.left,
                       ir.bottom - ir.top, 0, nullptr, DI_NORMAL);
        }

        if (g_settings.showRunningDot && g_pins[i].running) {
            int cx = (ir.left + ir.right) / 2;
            int cy = ir.bottom + 3;
            HBRUSH dot = CreateSolidBrush(RGB(0x6C, 0xC6, 0xFF));
            HGDIOBJ ob = SelectObject(hdc, dot);
            HGDIOBJ op = SelectObject(hdc, GetStockObject(NULL_PEN));
            Ellipse(hdc, cx - 3, cy - 3, cx + 3, cy + 3);
            SelectObject(hdc, ob);
            SelectObject(hdc, op);
            DeleteObject(dot);
        }
    }

    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK DockWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTCLIENT;

        case WM_MOUSEMOVE: {
            TRACKMOUSEEVENT tme{};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int hit = HitTestIcon(x, y);
            if (hit != g_hoverIndex) {
                g_hoverIndex = hit;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hoverIndex != -1) {
                g_hoverIndex = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int hit = HitTestIcon(x, y);
            if (hit >= 0 && hit < static_cast<int>(g_pins.size())) {
                LaunchOrFocus(g_pins[hit]);
            }
            return 0;
        }

        case WM_TIMER:
            if (wParam == kTimerRefresh) {
                UpdateRunningFlags();
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == kTimerFullscreen) {
                bool busy = IsFullscreenBusy();
                if (busy != g_hiddenFullscreen) {
                    g_hiddenFullscreen = busy;
                    ShowWindow(hwnd, busy ? SW_HIDE : SW_SHOWNOACTIVATE);
                    if (!busy) PlaceDockWindow();
                }
            }
            return 0;

        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            PlaceDockWindow();
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            PaintDock(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, kTimerRefresh);
            KillTimer(hwnd, kTimerFullscreen);
            g_hwnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool EnsureClass() {
    if (g_classRegistered) return true;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DockWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    g_atom = RegisterClassExW(&wc);
    if (!g_atom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"[ZenDock] RegisterClassEx failed: %lu", GetLastError());
        return false;
    }
    g_classRegistered = true;
    return true;
}

static void DestroyDockWindow() {
    if (g_hwnd && IsWindow(g_hwnd)) {
        DestroyWindow(g_hwnd);
    }
    g_hwnd = nullptr;
}

static void LayoutAndShow() {
    DestroyDockWindow();
    RebuildPins();
    if (g_pins.empty()) {
        Wh_Log(L"[ZenDock] no pins, skip window");
        return;
    }
    if (!EnsureClass()) return;

    ComputeDockSize(&g_dockW, &g_dockH);
    g_hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        kClassName, L"ZenDesktop Dock", WS_POPUP,
        0, 0, g_dockW, g_dockH, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g_hwnd) {
        Wh_Log(L"[ZenDock] CreateWindowEx failed: %lu", GetLastError());
        return;
    }
    ApplyGlass(g_hwnd);
    PlaceDockWindow();
    UpdateRunningFlags();
    g_hiddenFullscreen = false;
    if (IsFullscreenBusy()) {
        g_hiddenFullscreen = true;
        ShowWindow(g_hwnd, SW_HIDE);
    } else {
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    }
    SetTimer(g_hwnd, kTimerRefresh, 2000, nullptr);
    SetTimer(g_hwnd, kTimerFullscreen, 1500, nullptr);
    InvalidateRect(g_hwnd, nullptr, FALSE);
}

BOOL Wh_ModInit() {
    Wh_Log(L"[ZenDock] === Wh_ModInit v0.1.0 ===");
    LoadSettings();
    LayoutAndShow();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[ZenDock] === Wh_ModUninit ===");
    DestroyDockWindow();
    DestroyPins();
    if (g_classRegistered) {
        UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
        g_classRegistered = false;
        g_atom = 0;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[ZenDock] === Settings changed ===");
    LoadSettings();
    LayoutAndShow();
}
