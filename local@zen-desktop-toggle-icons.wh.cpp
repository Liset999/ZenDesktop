// ==WindhawkMod==
// @id              zen-desktop-toggle-icons
// @name            ZenDesktop: Double Click to Hide Icons
// @description     Native C++ Windhawk mod to hide/show desktop icons by double-clicking. Auto-hides on system-wide inactivity and restores on any user input.
// @version         3.1.0
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZenDesktop: Double Click to Hide Icons

Have you ever wanted a clean, clutter-free desktop but found it annoying to right-click -> View -> Show desktop icons every time?

**ZenDesktop** brings the ultimate native solution to Windows. It allows you to **double-click empty space on your desktop** to instantly hide or show your icons, and optionally **automatically hide icons** after a configurable period of system inactivity.

### 🌟 Key Features:
* **Zero UI Overhead**: Embedded natively inside `explorer.exe` process space. No background EXE running, no taskbar/tray icons, 0% CPU and virtually 0MB extra RAM.
* **Process-Native Hit-Testing**: When you double-click, it performs a real-time ListView hit-test. If you double-click a file, folder, or shortcut, the default "open" action is triggered normally. If you double-click empty space, it toggles your desktop icons.
* **System-Wide Inactivity Detection**: Uses `GetLastInputInfo()` to track ALL user input (keyboard, mouse anywhere on screen) — icons only auto-hide when you are truly idle, not just away from the desktop.
* **Smart Auto-Restore**: When icons are auto-hidden, any user input (mouse or keyboard anywhere) instantly restores them. Manually hidden icons are never auto-restored.
* **Dynamic Hooking**: Automatically subclasses the shell views, meaning even if your Explorer crashes, restarts, or you plug/unplug monitors, the mod remains fully active and stable.
* **Completely Safe**: Uses native Windows `0x7402` (WM_COMMAND) toggle signals, ensuring Windows handles the fade animations and desktop state natively without breaking icon grid arrangements.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableAutoHide: true
  $name: "Auto-Hide Icons on Inactivity (自动隐藏)"
  $description: "Automatically hides desktop icons after the system has been idle for the specified duration."
- autoHideDelay: 5
  $name: "Auto-Hide Delay in Seconds (延迟秒数)"
  $description: "Seconds of system-wide inactivity before icons are hidden. Range: 3–60 seconds."
- showIconsOnAnyInput: true
  $name: "Restore Icons on Any Input (任意输入恢复)"
  $description: "When icons were auto-hidden, any mouse or keyboard input instantly restores them. Does NOT affect manually hidden icons."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <windhawk_utils.h>

#define TIMER_AUTOHIDE   1001
#define WM_REFRESH_TIMER (WM_USER + 5001)

// ─────────────────────────────────────────────────────────────────────────────
// Forward declarations
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopListViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK DesktopShellViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
BOOL    CALLBACK EnumWindowsProc(HWND, LPARAM);

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t Real_CreateWindowExW;

// ─────────────────────────────────────────────────────────────────────────────
// Global state
// ─────────────────────────────────────────────────────────────────────────────

// Debounce guard: prevents toggle from firing more than once per 500 ms.
static DWORD g_lastToggleTime = 0;

// When icons were auto-hidden (non-zero). Zero means icons were manually hidden
// or are currently visible. This is the key to distinguishing auto-hide from
// manual toggle so we never auto-restore a deliberate hide.
static DWORD g_autoHiddenAt = 0;

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
    g_settings.enableAutoHide     = Wh_GetIntSetting(L"enableAutoHide") != 0;
    g_settings.autoHideDelay      = Wh_GetIntSetting(L"autoHideDelay");
    if (g_settings.autoHideDelay < 3)  g_settings.autoHideDelay = 3;
    if (g_settings.autoHideDelay > 60) g_settings.autoHideDelay = 60;
    g_settings.showIconsOnAnyInput = Wh_GetIntSetting(L"showIconsOnAnyInput") != 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Desktop icon visibility helper
// ─────────────────────────────────────────────────────────────────────────────
static bool IsDesktopIconsVisible()
{
    HWND hwndShell = NULL;

    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman)
        hwndShell = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (!hwndShell) {
        HWND hwndWorkerW = NULL;
        while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL) {
            hwndShell = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hwndShell) break;
        }
    }

    if (hwndShell) {
        HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (hwndListView)
            return IsWindowVisible(hwndListView) != 0;
    }
    return true; // assume visible if window hierarchy not found
}

// ─────────────────────────────────────────────────────────────────────────────
// Timer management
// ─────────────────────────────────────────────────────────────────────────────
static void UpdateTimerState(HWND hwndShell)
{
    if (hwndShell)
        PostMessageW(hwndShell, WM_REFRESH_TIMER, 0, 0);
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
// Safe toggle helper — posts WM_COMMAND 0x7402 with debounce
// Returns true if toggle was dispatched.
// ─────────────────────────────────────────────────────────────────────────────
static bool TryToggleIcons(HWND hwndShellView, bool isAutoHide)
{
    DWORD now = GetTickCount();
    if (now - g_lastToggleTime <= 500)
        return false;

    g_lastToggleTime = now;

    if (isAutoHide) {
        // Auto-hide: record the moment so we can auto-restore later.
        g_autoHiddenAt = now;
    } else {
        // Manual toggle: clear auto-hide stamp so restore logic is suppressed.
        g_autoHiddenAt = 0;
    }

    PostMessageW(hwndShellView, WM_COMMAND, 0x7402, 0);
    return true;
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
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc, 0);
            UpdateTimerState(hwndShell);

            HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            if (hwndListView)
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwndListView, DesktopListViewSubclassProc, 0);
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
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc);
        HWND lv = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (lv) WindhawkUtils::RemoveWindowSubclassFromAnyThread(lv, DesktopListViewSubclassProc);
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
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopShellViewSubclassProc, 0);
            UpdateTimerState(hWnd);
        }
        else if (wcscmp(lpClassName, L"SysListView32") == 0) {
            WCHAR parentClass[256] = {};
            if (hWndParent &&
                GetClassNameW(hWndParent, parentClass, 256) &&
                wcscmp(parentClass, L"SHELLDLL_DefView") == 0)
            {
                WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListViewSubclassProc, 0);
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
        static DWORD lastClickTime = 0;
        static POINT lastClickPt   = {};

        bool isDblClick = (uMsg == WM_LBUTTONDBLCLK);

        // Fallback manual double-click detection for windows without CS_DBLCLKS.
        if (uMsg == WM_LBUTTONDOWN) {
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

        if (isDblClick) {
            // Native LVM_HITTEST to check if click landed on empty space.
            LVHITTESTINFO ht = {};
            ht.pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

            if (ht.iItem == -1) {
                // Empty space — manual toggle (not auto-hide).
                HWND hwndParent = GetParent(hWnd);
                if (hwndParent)
                    TryToggleIcons(hwndParent, /*isAutoHide=*/false);
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
//     • Run the 500 ms auto-hide/restore polling timer.
//     • Immediate restore on mouse-over-desktop (when icons are auto-hidden).
//     • Double-click restore (always works, including manually hidden icons).
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopShellViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    // ── One-time timer bootstrap (runs the first time this proc is called) ────
    // NOTE: timerInitialized is a static shared across all HWND instances of
    // this proc. WM_REFRESH_TIMER handles the per-HWND case (multi-monitor).
    static bool timerInitialized = false;
    if (!timerInitialized && g_settings.enableAutoHide) {
        if (SetTimer(hWnd, TIMER_AUTOHIDE, 500, NULL))
            timerInitialized = true;
    }

    // ── Cross-thread timer refresh (settings change / new window subclassed) ──
    if (uMsg == WM_REFRESH_TIMER) {
        if (g_settings.enableAutoHide) {
            timerInitialized = (SetTimer(hWnd, TIMER_AUTOHIDE, 500, NULL) != 0);
        } else {
            KillTimer(hWnd, TIMER_AUTOHIDE);
            timerInitialized = false;
        }
        return 0;
    }

    // ── Immediate desktop-mouse restore (only when auto-hidden) ──────────────
    // SHELLDLL_DefView receives WM_MOUSEMOVE / WM_LBUTTONDOWN only when the
    // SysListView32 is hidden (icons hidden), making this the right place.
    if (g_autoHiddenAt != 0 && g_settings.enableAutoHide) {
        if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN) {
            // Restore immediately — don't wait for the next timer tick.
            if (!IsDesktopIconsVisible())
                TryToggleIcons(hWnd, /*isAutoHide=*/false);
            // g_autoHiddenAt is cleared inside TryToggleIcons (isAutoHide=false).
        }
    }

    // ── Auto-hide / auto-restore timer tick (every 500 ms) ───────────────────
    if (uMsg == WM_TIMER && wParam == TIMER_AUTOHIDE) {
        if (g_settings.enableAutoHide) {
            // Use GetLastInputInfo() for SYSTEM-WIDE inactivity — not just
            // desktop activity. This prevents hiding icons while the user is
            // actively using other applications.
            LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
            GetLastInputInfo(&lii);
            DWORD now = GetTickCount();

            if (IsDesktopIconsVisible()) {
                // ── Auto-hide check ──────────────────────────────────────────
                DWORD idleMs = now - lii.dwTime;
                if (idleMs >= (DWORD)g_settings.autoHideDelay * 1000)
                    TryToggleIcons(hWnd, /*isAutoHide=*/true);
            }
            else if (g_autoHiddenAt != 0 && g_settings.showIconsOnAnyInput) {
                // ── Auto-restore check (timer-based, catches non-desktop input) ─
                // If any input occurred AFTER we auto-hid the icons → restore.
                // lii.dwTime is the tick of the last system input.
                // g_autoHiddenAt is the tick when we sent the hide command.
                // We add a 200 ms grace period so the hide animation finishes
                // before we consider restoring.
                if ((lii.dwTime - g_autoHiddenAt) > 200 &&
                    lii.dwTime > g_autoHiddenAt)
                {
                    TryToggleIcons(hWnd, /*isAutoHide=*/false);
                }
            }
        }
        return 0;
    }

    // ── Double-click restore (works for both auto-hidden AND manually hidden) ──
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        static DWORD lastClickTime = 0;
        static POINT lastClickPt   = {};

        bool isDblClick = (uMsg == WM_LBUTTONDBLCLK);

        if (uMsg == WM_LBUTTONDOWN) {
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

        if (isDblClick) {
            TryToggleIcons(hWnd, /*isAutoHide=*/false);
            return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk lifecycle
// ─────────────────────────────────────────────────────────────────────────────
bool Wh_ModInit()
{
    LoadSettings();

    if (!Wh_SetFunctionHook(
            (void*)CreateWindowExW,
            (void*)Hook_CreateWindowExW,
            (void**)&Real_CreateWindowExW))
        return false;

    SubclassExistingWindows();
    return true;
}

void Wh_ModUninit()
{
    UnsubclassWindows();
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
    UpdateAllTimers();
}
