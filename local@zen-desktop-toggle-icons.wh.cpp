// ==WindhawkMod==
// @id              zen-desktop-toggle-icons
// @name            ZenDesktop: 桌面双击隐藏图标
// @name:zh-CN      ZenDesktop: 桌面双击隐藏图标
// @description     Double-click empty desktop space to show or hide icons, with optional auto-hide when the system is idle.
// @description:zh-CN 双击桌面空白处显示或隐藏图标，可在系统闲置时自动隐藏。
// @version         4.1.2
// @donateUrl       https://ifdian.net/a/Lanboo
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZenDesktop: 桌面双击隐藏图标

双击桌面空白处即可显示或隐藏桌面图标，也可以在系统闲置一段时间后自动隐藏。直接运行在 `explorer.exe` 中，不需要额外的后台程序。如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」。

## 功能

- 双击桌面空白处切换图标显示；双击文件、文件夹或快捷方式时照常打开。
- 系统闲置达到设定时长后自动隐藏图标，检测的是全局键盘和鼠标输入。
- 自动隐藏后任意输入即可恢复；双击手动隐藏的图标不会被自动恢复。
- 资源管理器重启、插拔显示器后自动重新挂接。
- 通过隐藏桌面 `SysListView32` 实现切换；找不到该窗口时回退到系统「显示桌面图标」命令，不打乱图标排列。

## 设置说明

- **闲置时自动隐藏图标**：关闭后只保留双击切换。默认开启。
- **自动隐藏延迟（秒）**：范围 3–60 秒。默认 5。
- **任意输入时恢复图标**：仅对自动隐藏生效；双击手动隐藏的图标不会被恢复。默认开启。

## 兼容性与已知问题

- 适用于 Windows 11，注入 `explorer.exe`。
- 如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」（系统菜单里是这一项，勾选为显示、取消勾选为隐藏）。系统会记住该状态；本 mod 双击隐藏后也会自行记住，资源管理器重启后仍会保持隐藏。
- 如果想让任务栏也在闲置时淡出，可以配合任务栏类 mod 使用。

## 更新日志

- **4.1.2**：补充开机隐藏图标的使用说明；核对设置与说明文字。
- **4.1.1**：规范设置界面与说明文字，行为不变。
- **4.1.0**：随 ZenDesktop v4.1 发布。

## 致谢与许可

- ZenDesktop 原创 mod。
- 开发者：Lanbo（[哔哩哔哩](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo)）

## 捐助支持

如果这个 mod 对你有帮助，可以通过爱发电或微信赞赏支持作者 Lanbo。

- [爱发电](https://ifdian.net/a/Lanboo)

![微信赞赏码](https://raw.githubusercontent.com/Liset999/ZenDesktop/v4.2-draft/docs/donate-wechat.png)

图片无法显示时，可打开：https://raw.githubusercontent.com/Liset999/ZenDesktop/v4.2-draft/docs/donate-wechat.png
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableAutoHide: true
  $name: "Auto-hide icons when idle"
  $name:zh-CN: "闲置时自动隐藏图标"
  $description: "Hide desktop icons after the system has been idle for the delay below. Default: on."
  $description:zh-CN: "系统闲置达到下方延迟后自动隐藏桌面图标。默认开启。"
- autoHideDelay: 5
  $name: "Auto-hide delay (seconds)"
  $name:zh-CN: "自动隐藏延迟（秒）"
  $description: "System-wide idle time before icons are hidden, 3–60 seconds. Default: 5."
  $description:zh-CN: "隐藏图标前需要的全局闲置时间，范围 3–60 秒。默认 5。"
- showIconsOnAnyInput: true
  $name: "Restore icons on any input"
  $name:zh-CN: "任意输入时恢复图标"
  $description: "When icons were hidden automatically, any mouse or keyboard input restores them. Icons hidden by double-click are not affected. Default: on."
  $description:zh-CN: "仅在图标由自动隐藏后生效：任意鼠标或键盘输入都会恢复显示；双击手动隐藏的图标不受影响。默认开启。"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#define TIMER_AUTOHIDE             1001
#define TIMER_RESTORE_POLL_MS       500
#define TIMER_FULLSCREEN_RECHECK_MS 1000

static UINT g_msgRefreshTimer = 0;
static UINT g_msgManualToggle = 0;
static UINT g_msgAutoHide = 0;
static UINT g_msgAutoRestore = 0;

// ─────────────────────────────────────────────────────────────────────────────
// Forward declarations
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopListViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK DesktopShellViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
BOOL    CALLBACK EnumWindowsProc(HWND, LPARAM);

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t Real_CreateWindowExW;

// ─────────────────────────────────────────────────────────────────────────────
// Global settings
// ─────────────────────────────────────────────────────────────────────────────
struct Settings {
    bool enableAutoHide;
    int  autoHideDelay;       // seconds, clamped 3–60
    bool showIconsOnAnyInput; // restore on any input after auto-hide
} g_settings;

// ─────────────────────────────────────────────────────────────────────────────
// Settings
// ─────────────────────────────────────────────────────────────────────────────
static void LoadSettings()
{
    g_settings.enableAutoHide      = Wh_GetIntSetting(L"enableAutoHide") != 0;
    g_settings.autoHideDelay       = Wh_GetIntSetting(L"autoHideDelay");
    if (g_settings.autoHideDelay < 3)  g_settings.autoHideDelay = 3;
    if (g_settings.autoHideDelay > 60) g_settings.autoHideDelay = 60;
    g_settings.showIconsOnAnyInput = Wh_GetIntSetting(L"showIconsOnAnyInput") != 0;

    Wh_Log(L"[ZenDesktop] Settings loaded: enableAutoHide=%d, autoHideDelay=%d, showIconsOnAnyInput=%d",
           (int)g_settings.enableAutoHide, g_settings.autoHideDelay, (int)g_settings.showIconsOnAnyInput);
}

// ─────────────────────────────────────────────────────────────────────────────
// Timer management — post a registered refresh message to SHELLDLL_DefView so the timer
// is always created/destroyed on the window-owning thread.
// ─────────────────────────────────────────────────────────────────────────────
static void ClearAutoHideTimer(HWND hwndShell)
{
    KillTimer(hwndShell, TIMER_AUTOHIDE);
    RemovePropW(hwndShell, L"ZenTimerInit");
}

static bool SetAutoHideTimer(HWND hwndShell, UINT intervalMs)
{
    if (intervalMs == 0)
        intervalMs = 1;

    if (SetTimer(hwndShell, TIMER_AUTOHIDE, intervalMs, NULL)) {
        SetPropW(hwndShell, L"ZenTimerInit", UlongToHandle(1));
        return true;
    }

    ClearAutoHideTimer(hwndShell);
    return false;
}

static void ScheduleAutoHideTimer(HWND hwndShell)
{
    if (!hwndShell || !g_settings.enableAutoHide) {
        if (hwndShell)
            ClearAutoHideTimer(hwndShell);
        return;
    }

    HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
    if (!hwndListView) {
        ClearAutoHideTimer(hwndShell);
        return;
    }

    bool isVisible = IsWindowVisible(hwndListView) != 0;
    if (isVisible) {
        LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
        if (!GetLastInputInfo(&lii)) {
            SetAutoHideTimer(hwndShell, TIMER_FULLSCREEN_RECHECK_MS);
            return;
        }

        DWORD now = GetTickCount();
        DWORD idleMs = now - lii.dwTime;
        DWORD thresholdMs = (DWORD)g_settings.autoHideDelay * 1000;
        DWORD intervalMs = idleMs >= thresholdMs ? 1 : thresholdMs - idleMs;
        SetAutoHideTimer(hwndShell, intervalMs);
        return;
    }

    DWORD autoHiddenAt = HandleToUlong(GetPropW(hwndShell, L"ZenAutoHiddenAt"));
    if (autoHiddenAt != 0 && g_settings.showIconsOnAnyInput) {
        SetAutoHideTimer(hwndShell, TIMER_RESTORE_POLL_MS);
    } else {
        ClearAutoHideTimer(hwndShell);
    }
}

static void UpdateTimerState(HWND hwndShell)
{
    if (hwndShell && g_msgRefreshTimer)
        PostMessageW(hwndShell, g_msgRefreshTimer, 0, 0);
}

static void UpdateAllTimers()
{
    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman) {
        HWND hwndShell = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) UpdateTimerState(hwndShell);
    }
    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL) {
        HWND hwndShell = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) UpdateTimerState(hwndShell);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Full-Screen Application Guard
// ─────────────────────────────────────────────────────────────────────────────
static bool IsFullscreenWindowActive()
{
    QUERY_USER_NOTIFICATION_STATE state;
    if (FAILED(SHQueryUserNotificationState(&state))) {
        return false;
    }

    switch (state) {
        case QUNS_NOT_PRESENT:
        case QUNS_BUSY:
        case QUNS_RUNNING_D3D_FULL_SCREEN:
            return true;
    }

    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Icon visibility helpers
//
// ManualToggleIcons  — user double-clicked: toggle and clear auto-hide marker.
// AutoHideIcons      — timer fired: hide only if visible, set auto-hide marker.
// AutoRestoreIcons   — input detected: restore only auto-hidden icons.
//
// "ZenAutoHiddenAt" window property (stores GetTickCount) distinguishes
// auto-hidden from manually-hidden state.
// ─────────────────────────────────────────────────────────────────────────────
static bool GetPersistedHideState()
{
    HKEY hKey;
    DWORD value = 0;
    DWORD size = sizeof(DWORD);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\ZenDesktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"HideDesktopIcons", nullptr, nullptr, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    return value != 0;
}

static void SetPersistedHideState(bool hide)
{
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\ZenDesktop", 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        DWORD value = hide ? 1 : 0;
        RegSetValueExW(hKey, L"HideDesktopIcons", 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

static void ManualToggleIcons(HWND hwndShellView)
{
    HWND hwndListView = FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);
    if (hwndListView) {
        bool isVisible = IsWindowVisible(hwndListView) != 0;
        bool newVisible = !isVisible;
        ShowWindow(hwndListView, newVisible ? SW_SHOW : SW_HIDE);
        SetPersistedHideState(!newVisible);
        Wh_Log(L"[ZenDesktop] ManualToggle: %s", newVisible ? L"SHOW" : L"HIDE");
    } else {
        // Fallback: use the native shell toggle if ListView handle is missing
        PostMessageW(hwndShellView, WM_COMMAND, 0x7402, 0);
        Wh_Log(L"[ZenDesktop] ManualToggle: fallback WM_COMMAND 0x7402");
    }
    // Clear auto-hide marker — this was a deliberate user action
    RemovePropW(hwndShellView, L"ZenAutoHiddenAt");
    RemovePropW(hwndShellView, L"ZenMouseX");
    RemovePropW(hwndShellView, L"ZenMouseY");
    ScheduleAutoHideTimer(hwndShellView);
}

static void AutoHideIcons(HWND hwndShellView)
{
    HWND hwndListView = FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);
    if (hwndListView && IsWindowVisible(hwndListView)) {
        ShowWindow(hwndListView, SW_HIDE);
        DWORD now = GetTickCount();
        // Avoid storing 0 (which means "not set"); extremely unlikely but be safe
        if (now == 0) now = 1;
        SetPropW(hwndShellView, L"ZenAutoHiddenAt", UlongToHandle(now));
        
        // Store current physical mouse coordinates for zero-latency coordinate guard
        POINT pt = {};
        GetCursorPos(&pt);
        SetPropW(hwndShellView, L"ZenMouseX", UlongToHandle((DWORD)pt.x));
        SetPropW(hwndShellView, L"ZenMouseY", UlongToHandle((DWORD)pt.y));

        Wh_Log(L"[ZenDesktop] AutoHide: icons hidden at tick=%u, mouse=(%d,%d)", now, pt.x, pt.y);
    }
    ScheduleAutoHideTimer(hwndShellView);
}

static void AutoRestoreIcons(HWND hwndShellView)
{
    HWND hwndListView = FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);
    if (hwndListView && !IsWindowVisible(hwndListView)) {
        ShowWindow(hwndListView, SW_SHOW);
        Wh_Log(L"[ZenDesktop] AutoRestore: icons restored");
    }
    RemovePropW(hwndShellView, L"ZenAutoHiddenAt");
    RemovePropW(hwndShellView, L"ZenMouseX");
    RemovePropW(hwndShellView, L"ZenMouseY");
    ScheduleAutoHideTimer(hwndShellView);
}

// ─────────────────────────────────────────────────────────────────────────────
// Window enumeration / subclassing
// ─────────────────────────────────────────────────────────────────────────────
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM)
{
    WCHAR className[256] = {};
    if (!GetClassNameW(hWnd, className, 256)) return TRUE;

    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0) {
        HWND hwndShell = FindWindowExW(hWnd, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) {
            Wh_Log(L"[ZenDesktop] Subclassing ShellView=%p (parent=%s)", hwndShell, className);
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc, 0);
            UpdateTimerState(hwndShell);

            HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            if (hwndListView) {
                Wh_Log(L"[ZenDesktop] Subclassing ListView=%p", hwndListView);
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwndListView, DesktopListViewSubclassProc, 0);
                if (GetPersistedHideState()) {
                    ShowWindow(hwndListView, SW_HIDE);
                    Wh_Log(L"[ZenDesktop] Initialized desktop icons to HIDE based on persisted registry state.");
                }
            }
        }
    }
    return TRUE;
}

static void SubclassExistingWindows() { EnumWindows(EnumWindowsProc, 0); }

static void UnsubclassWindows()
{
    auto Cleanup = [](HWND hwndShell) {
        if (!hwndShell) return;
        KillTimer(hwndShell, TIMER_AUTOHIDE);
        RemovePropW(hwndShell, L"ZenTimerInit");
        RemovePropW(hwndShell, L"ZenAutoHiddenAt");
        RemovePropW(hwndShell, L"ZenMouseX");
        RemovePropW(hwndShell, L"ZenMouseY");
        
        HWND lv = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (lv) {
            DWORD pid = 0;
            // Verify window validity and process ownership before restoring visibility
            if (IsWindow(lv) && GetWindowThreadProcessId(lv, &pid) && pid == GetCurrentProcessId()) {
                if (!IsWindowVisible(lv)) {
                    ShowWindow(lv, SW_SHOW);
                    Wh_Log(L"[ZenDesktop] Cleanup: restored SysListView32 visibility");
                }
            }
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(lv, DesktopListViewSubclassProc);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc);
    };

    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman)
        Cleanup(FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL));

    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL)
        Cleanup(FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL));
}

// ─────────────────────────────────────────────────────────────────────────────
// CreateWindowExW hook — dynamically subclass new desktop windows
// ─────────────────────────────────────────────────────────────────────────────
HWND WINAPI Hook_CreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = Real_CreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (wcscmp(lpClassName, L"SHELLDLL_DefView") == 0) {
            Wh_Log(L"[ZenDesktop] Hook: new SHELLDLL_DefView=%p created", hWnd);
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopShellViewSubclassProc, 0);
            UpdateTimerState(hWnd);
        }
        else if (wcscmp(lpClassName, L"SysListView32") == 0) {
            WCHAR parentClass[256] = {};
            if (hWndParent &&
                GetClassNameW(hWndParent, parentClass, 256) &&
                wcscmp(parentClass, L"SHELLDLL_DefView") == 0)
            {
                Wh_Log(L"[ZenDesktop] Hook: new desktop ListView=%p created", hWnd);
                WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListViewSubclassProc, 0);
                if (GetPersistedHideState()) {
                    ShowWindow(hWnd, SW_HIDE);
                    Wh_Log(L"[ZenDesktop] Hook initialized desktop icons to HIDE based on persisted registry state.");
                }
            }
        }
    }
    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// Subclass Proc: SysListView32 (Desktop icon ListView)
//   Receives events only when icons are VISIBLE (ListView is the hit target).
//   Responsibility: detect double-click on empty desktop space → manual toggle.
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopListViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        bool hasDblClkStyle = (GetClassLongPtrW(hWnd, GCL_STYLE) & CS_DBLCLKS) != 0;
        bool isDblClick = false;

        if (hasDblClkStyle) {
            isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        } else {
            if (uMsg == WM_LBUTTONDBLCLK) {
                isDblClick = true;
            } else if (uMsg == WM_LBUTTONDOWN) {
                static DWORD lastClickTime = 0;
                static POINT lastClickPt   = {};
                DWORD now          = GetTickCount();
                DWORD dblClickTime = GetDoubleClickTime();
                POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };

                if (now - lastClickTime <= dblClickTime &&
                    abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                    abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2)
                {
                    isDblClick = true;
                } else {
                    lastClickTime = now;
                    lastClickPt   = pt;
                }
            }
        }

        if (isDblClick) {
            // Native LVM_HITTEST to check if click landed on empty space.
            LVHITTESTINFO ht = {};
            ht.pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

            if (ht.iItem == -1) {
                // Empty space — post manual toggle command asynchronously
                HWND hwndParent = GetParent(hWnd);
                if (hwndParent) {
                    PostMessageW(hwndParent, g_msgManualToggle, 0, 0);
                }
                return 0; // suppress default double-click action
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Subclass Proc: SHELLDLL_DefView
//   Receives events when icons are HIDDEN (ListView hidden → DefView is hit).
//   Responsibilities:
//     • Run the dynamically scheduled auto-hide/restore timer.
//     • Immediate restore on mouse move over desktop (when icons are auto-hidden).
//     • Immediate restore on click on desktop (when icons are auto-hidden).
//     • Double-click restore (always works, including manually hidden icons).
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopShellViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    // ── One-time timer bootstrap ─────────────────────────────────────────────
    bool timerInitialized = GetPropW(hWnd, L"ZenTimerInit") != NULL;
    if (!timerInitialized && g_settings.enableAutoHide) {
        ScheduleAutoHideTimer(hWnd);
    }

    // ── Cross-thread timer refresh (settings change / new window subclassed) ──
    if (uMsg == g_msgRefreshTimer) {
        ScheduleAutoHideTimer(hWnd);
        Wh_Log(L"[ZenDesktop] Timer refreshed for ShellView=%p", hWnd);
        return 0;
    }

    // ── Async Operations execution ───────────────────────────────────────────
    if (uMsg == g_msgManualToggle) {
        ManualToggleIcons(hWnd);
        return 0;
    }
    if (uMsg == g_msgAutoHide) {
        AutoHideIcons(hWnd);
        return 0;
    }
    if (uMsg == g_msgAutoRestore) {
        AutoRestoreIcons(hWnd);
        return 0;
    }

    // ── Auto-hide / auto-restore timer tick ──────────────────────────────────
    if (uMsg == WM_TIMER && wParam == TIMER_AUTOHIDE) {
        if (!g_settings.enableAutoHide) {
            ScheduleAutoHideTimer(hWnd);
            return 0;
        }

        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (!hwndListView) {
            ScheduleAutoHideTimer(hWnd);
            return 0;
        }

        bool isVis = IsWindowVisible(hwndListView) != 0;

        LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
        if (!GetLastInputInfo(&lii)) {
            ScheduleAutoHideTimer(hWnd);
            return 0;
        }
        DWORD now = GetTickCount();

        DWORD autoHiddenAt = HandleToUlong(GetPropW(hWnd, L"ZenAutoHiddenAt"));

        if (isVis) {
            // Check full screen guard
            if (IsFullscreenWindowActive()) {
                SetAutoHideTimer(hWnd, TIMER_FULLSCREEN_RECHECK_MS);
                return 0; // Skip auto-hide if a full-screen window is active (video/game/PPT)
            }
            DWORD idleMs = now - lii.dwTime;
            if (idleMs >= (DWORD)g_settings.autoHideDelay * 1000) {
                Wh_Log(L"[ZenDesktop] Timer: idle %ums >= %us threshold -> AutoHide",
                       idleMs, g_settings.autoHideDelay);
                PostMessageW(hWnd, g_msgAutoHide, 0, 0);
                return 0;
            }
        }
        else if (autoHiddenAt != 0 && g_settings.showIconsOnAnyInput) {
            // Check system-wide physical input
            if (lii.dwTime > autoHiddenAt) {
                Wh_Log(L"[ZenDesktop] Timer: new system-wide input detected -> AutoRestore");
                PostMessageW(hWnd, g_msgAutoRestore, 0, 0);
                return 0;
            }
        }

        ScheduleAutoHideTimer(hWnd);
        return 0;
    }

    // ── Immediate mouse/click restore (only when auto-hidden + restore-on-input enabled) ──
    // Zero-latency coordinate-based physical mouse movement detection
    {
        DWORD autoHiddenAt = HandleToUlong(GetPropW(hWnd, L"ZenAutoHiddenAt"));
        if (autoHiddenAt != 0 && g_settings.showIconsOnAnyInput) {
            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN) {
                Wh_Log(L"[ZenDesktop] Immediate click restore on DefView (uMsg=%u)", uMsg);
                PostMessageW(hWnd, g_msgAutoRestore, 0, 0);
            }
            else if (uMsg == WM_MOUSEMOVE) {
                POINT ptCurrent = {};
                GetCursorPos(&ptCurrent);
                DWORD savedX = HandleToUlong(GetPropW(hWnd, L"ZenMouseX"));
                DWORD savedY = HandleToUlong(GetPropW(hWnd, L"ZenMouseY"));

                if (ptCurrent.x != (int)savedX || ptCurrent.y != (int)savedY) {
                    Wh_Log(L"[ZenDesktop] Physical mouse move detected: (%d,%d) vs (%d,%d) -> AutoRestore",
                           ptCurrent.x, ptCurrent.y, (int)savedX, (int)savedY);
                    PostMessageW(hWnd, g_msgAutoRestore, 0, 0);
                }
            }
        }
    }

    // ── Double-click restore (works for both auto-hidden AND manually hidden) ──
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        bool hasDblClkStyle = (GetClassLongPtrW(hWnd, GCL_STYLE) & CS_DBLCLKS) != 0;
        bool isDblClick = false;

        if (hasDblClkStyle) {
            isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        } else {
            if (uMsg == WM_LBUTTONDBLCLK) {
                isDblClick = true;
            } else if (uMsg == WM_LBUTTONDOWN) {
                static DWORD lastClickTime = 0;
                static POINT lastClickPt   = {};
                DWORD now          = GetTickCount();
                DWORD dblClickTime = GetDoubleClickTime();
                POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };

                if (now - lastClickTime <= dblClickTime &&
                    abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                    abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2)
                {
                    isDblClick = true;
                } else {
                    lastClickTime = now;
                    lastClickPt   = pt;
                }
            }
        }

        if (isDblClick) {
            PostMessageW(hWnd, g_msgManualToggle, 0, 0);
            return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk lifecycle
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit()
{
    Wh_Log(L"[ZenDesktop] === Wh_ModInit v3.4.0 ===");
    g_msgRefreshTimer = RegisterWindowMessageW(L"Windhawk.ZenDesktop.DesktopIconToggle.RefreshTimer");
    g_msgManualToggle = RegisterWindowMessageW(L"Windhawk.ZenDesktop.DesktopIconToggle.ManualToggle");
    g_msgAutoHide = RegisterWindowMessageW(L"Windhawk.ZenDesktop.DesktopIconToggle.AutoHide");
    g_msgAutoRestore = RegisterWindowMessageW(L"Windhawk.ZenDesktop.DesktopIconToggle.AutoRestore");
    if (!g_msgRefreshTimer || !g_msgManualToggle || !g_msgAutoHide || !g_msgAutoRestore) {
        Wh_Log(L"[ZenDesktop] FAILED to register private window messages");
        return FALSE;
    }

    LoadSettings();

    if (!Wh_SetFunctionHook(
            (void*)CreateWindowExW,
            (void*)Hook_CreateWindowExW,
            (void**)&Real_CreateWindowExW)) {
        Wh_Log(L"[ZenDesktop] FAILED to hook CreateWindowExW");
        return FALSE;
    }

    SubclassExistingWindows();
    Wh_Log(L"[ZenDesktop] Init complete");
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"[ZenDesktop] === Wh_ModUninit ===");
    UnsubclassWindows();
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"[ZenDesktop] === Settings changed ===");
    LoadSettings();
    UpdateAllTimers();
}
