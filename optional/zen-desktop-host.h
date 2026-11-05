// ZenDesktop shared desktop-host helper.
// Used by zen-desktop-clock and zen-audio-bars (same WorkerW / Progman attach).
//
// Model follows Wallpaper Engine / Lively / Ivy-Windows-Wallpaper:
//   https://docs.wallpaperengine.io/
//   https://github.com/Ivy-Interactive/Ivy-Windows-Wallpaper
//   https://github.com/lively-community/lively/issues/2074  (24H2 layout)
//
// 1. Send 0x052C ONCE to Progman to raise the wallpaper layer.
// 2. Find SHELLDLL_DefView.
// 3. Pre-24H2: WorkerW is a top-level sibling after the DefView host; parent
//    overlays to that WorkerW.
// 4. 24H2+: Progman has WS_EX_NOREDIRECTIONBITMAP; WorkerW is a child of
//    Progman under DefView. Parent overlays to Progman and place them under
//    DefView but above WorkerW / WPEDesktopDX11Window.
// 5. Cache the host. Re-query only when the cached HWNDs die or TaskbarCreated
//    fires. Never re-send 0x052C on a timer — that fights Wallpaper Engine and
//    makes the desktop flash.

#pragma once

#include <windows.h>

// Requires Windhawk (Wh_Log) — include after windhawk_utils.h in the mod.
struct ZenDesktopHost {
    HWND progman = nullptr;
    HWND defView = nullptr;   // SHELLDLL_DefView
    HWND worker = nullptr;    // wallpaper WorkerW (may equal progman as fallback)
    HWND weWindow = nullptr;  // WPEDesktopDX11Window if present
    HWND parent = nullptr;    // CreateWindowEx parent for the overlay
    bool is24H2 = false;      // Progman owns WorkerW as a child
    bool valid = false;
};

inline ZenDesktopHost g_zenDesktopHost;
inline bool g_zenDesktopHostReady = false;

inline bool ZenHwndAlive(HWND h) {
    return h && IsWindow(h);
}

inline bool ZenClassIs(HWND h, PCWSTR cls) {
    if (!h) return false;
    WCHAR buf[64] = L"";
    GetClassNameW(h, buf, ARRAYSIZE(buf));
    return _wcsicmp(buf, cls) == 0;
}

inline HWND ZenGetProgman() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (!progman) return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(progman, &pid);
    return pid == GetCurrentProcessId() ? progman : nullptr;
}

inline HWND ZenFindDefView(HWND progman) {
    if (HWND d = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr))
        return d;
    // Pre-24H2: DefView can live under a top-level WorkerW.
    HWND worker = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lp) -> BOOL {
            if (FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
                *(HWND*)lp = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&worker);
    if (worker) {
        return FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
    }
    return nullptr;
}

inline HWND ZenFindLegacyWorkerW() {
    // Classic trick: the WorkerW created after the window that hosts DefView.
    HWND found = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lp) -> BOOL {
            if (!FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr))
                return TRUE;
            HWND worker = FindWindowExW(nullptr, hWnd, L"WorkerW", nullptr);
            if (worker) {
                *(HWND*)lp = worker;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&found);
    return found;
}

inline HWND ZenFindWpeWindow(HWND searchRoot) {
    // Wallpaper Engine's desktop surface. May be a child of WorkerW or Progman.
    if (!searchRoot) return nullptr;
    HWND child = nullptr;
    while ((child = FindWindowExW(searchRoot, child, nullptr, nullptr))) {
        if (ZenClassIs(child, L"WPEDesktopDX11Window")) return child;
        if (HWND nested = ZenFindWpeWindow(child)) return nested;
    }
    return nullptr;
}

inline void ZenInvalidateDesktopHost() {
    g_zenDesktopHost = {};
    g_zenDesktopHostReady = false;
}

inline bool ZenDesktopHostStillValid() {
    if (!g_zenDesktopHostReady || !g_zenDesktopHost.valid) return false;
    const ZenDesktopHost& h = g_zenDesktopHost;
    if (!ZenHwndAlive(h.progman) || !ZenHwndAlive(h.parent)) return false;
    if (h.defView && !ZenHwndAlive(h.defView)) return false;
    if (h.worker && !ZenHwndAlive(h.worker)) return false;
    // WE may come and go; that alone does not invalidate the host.
    return true;
}

// Query (and cache) the desktop host. Sends 0x052C only on a cold lookup.
inline ZenDesktopHost ZenGetDesktopHost() {
    if (ZenDesktopHostStillValid()) return g_zenDesktopHost;

    ZenDesktopHost host{};
    host.progman = ZenGetProgman();
    if (!host.progman) {
        ZenInvalidateDesktopHost();
        return host;
    }

    // Detect 24H2+ raised-desktop layout (Progman has no GDI redirection bitmap).
    LONG_PTR ex = GetWindowLongPtrW(host.progman, GWL_EXSTYLE);
    host.is24H2 = (ex & WS_EX_NOREDIRECTIONBITMAP) != 0;

    Wh_Log(L"[zen-host] querying desktop host (0x052C), 24H2=%d",
           host.is24H2 ? 1 : 0);
    // Raise the wallpaper layer once. Both (0xD,0)/(0xD,1) and (0,0) forms
    // are used by WE / Lively across builds.
    SendMessageW(host.progman, 0x052C, 0xD, 0);
    SendMessageW(host.progman, 0x052C, 0xD, 1);

    host.defView = ZenFindDefView(host.progman);

    if (host.is24H2) {
        host.worker =
            FindWindowExW(host.progman, nullptr, L"WorkerW", nullptr);
        if (!host.worker) {
            SendMessageW(host.progman, 0x052C, 0, 0);
            host.worker =
                FindWindowExW(host.progman, nullptr, L"WorkerW", nullptr);
        }
        // 24H2: parent wallpaper surfaces to Progman, under DefView.
        host.parent = host.progman;
    } else {
        host.worker = ZenFindLegacyWorkerW();
        if (!host.worker) {
            SendMessageW(host.progman, 0x052C, 0, 0);
            host.worker = ZenFindLegacyWorkerW();
        }
        if (!host.worker) {
            host.worker =
                FindWindowExW(host.progman, nullptr, L"WorkerW", nullptr);
        }
        host.parent = host.worker ? host.worker : host.progman;
    }

    host.weWindow = ZenFindWpeWindow(host.parent);
    if (!host.weWindow && host.worker && host.worker != host.parent)
        host.weWindow = ZenFindWpeWindow(host.worker);
    if (!host.weWindow)
        host.weWindow = ZenFindWpeWindow(host.progman);

    host.valid = ZenHwndAlive(host.parent);
    g_zenDesktopHost = host;
    g_zenDesktopHostReady = host.valid;
    Wh_Log(L"[zen-host] progman=%p defView=%p worker=%p parent=%p we=%p",
           host.progman, host.defView, host.worker, host.parent, host.weWindow);
    return host;
}

// Place overlay correctly relative to DefView / WorkerW / Wallpaper Engine.
// Call once after create and rarely afterwards. Returns true if siblings exist
// (so a slow re-check timer can stay armed). raiseStreak backs off if another
// window keeps re-raising itself — no tug-of-war with WE.
inline bool ZenPlaceOverlay(HWND overlay, int* raiseStreak) {
    if (!ZenHwndAlive(overlay)) return false;
    ZenDesktopHost host = ZenGetDesktopHost();
    if (!host.valid) return false;

    HWND parent = GetParent(overlay);
    if (parent != host.parent && ZenHwndAlive(host.parent)) {
        SetParent(overlay, host.parent);
        parent = host.parent;
    }

    const UINT flags =
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSENDCHANGING;

    if (host.is24H2 && ZenHwndAlive(host.defView)) {
        // Under icons, above wallpaper WorkerW / WE.
        SetWindowPos(overlay, host.defView, 0, 0, 0, 0, flags);
        if (raiseStreak) *raiseStreak = 0;
        return ZenHwndAlive(host.weWindow) || ZenHwndAlive(host.worker);
    }

    // Legacy / WorkerW child: sit at the top of that host's children so WE
    // (also a child) does not cover us. Back off if WE keeps reclaiming top.
    HWND first = GetWindow(parent, GW_CHILD);
    bool covered = first && first != overlay;
    if (covered) {
        int streak = raiseStreak ? *raiseStreak : 0;
        if (streak < 3) {
            // Prefer inserting just above WE when we know it; else HWND_TOP.
            HWND after = HWND_TOP;
            if (ZenHwndAlive(host.weWindow) && GetParent(host.weWindow) == parent) {
                // Place ourselves above WE: SetWindowPos(hwnd, insertAfter)
                // puts hwnd BELOW insertAfter, so pass the window currently
                // above WE, or use HWND_TOP.
                after = HWND_TOP;
            }
            SetWindowPos(overlay, after, 0, 0, 0, 0, flags);
            Wh_Log(L"[zen-host] raised overlay above %p", first);
        } else if (streak == 3) {
            Wh_Log(L"[zen-host] z-order taken by %p, stop fighting", first);
        }
        if (raiseStreak) (*raiseStreak)++;
    } else if (raiseStreak) {
        *raiseStreak = 0;
    }
    return GetWindow(overlay, GW_HWNDNEXT) != nullptr ||
           GetWindow(parent, GW_CHILD) != overlay;
}

inline RECT ZenHostClientRect(const ZenDesktopHost& host) {
    RECT rc = {};
    HWND target = host.parent ? host.parent : host.progman;
    if (target) GetClientRect(target, &rc);
    if (rc.right <= 0 || rc.bottom <= 0) {
        rc.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if (rc.right <= 0) rc.right = GetSystemMetrics(SM_CXSCREEN);
        if (rc.bottom <= 0) rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    return rc;
}
