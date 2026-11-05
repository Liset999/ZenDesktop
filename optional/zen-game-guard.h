// ZenDesktop shared game / anti-cheat injection + foreign-window guard.
// Used by mods that inject broadly (@include *) and may animate windows that
// belong to other processes (e.g. windows-animations running inside explorer).
//
// Layers:
//   1) Windhawk @exclude / Engine Exclude — exact names + wildcards, pre-inject
//   2) Wh_ModInit refuse — do not load hooks into game / AC processes
//   3) Per-HWND skip — explorer must NOT cloak / PrintWindow / DWM-mutate a
//      foreign game window; process @exclude alone cannot stop that
//
// Limit: kernel ACE can still scan explorer.exe itself (where taskbar / styler
// mods live). Skipping game windows does not stop that. If ACE is installed
// and the system BSODs, disable ZenDesktop / Windhawk before playing.

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include <wchar.h>

namespace ZenGameGuard {

inline void GetCurrentExeName(WCHAR* out, size_t outCount) {
    if (!out || outCount == 0) return;
    out[0] = L'\0';
    WCHAR path[MAX_PATH]{};
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return;
    const WCHAR* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    wcsncpy_s(out, outCount, name, _TRUNCATE);
}

inline bool EqualsIgnoreCase(const WCHAR* a, const WCHAR* b) {
    return a && b && _wcsicmp(a, b) == 0;
}

// Wildcard match: '*' matches any sequence. Case-insensitive.
inline bool WildcardMatchI(const WCHAR* text, const WCHAR* pattern) {
    if (!text || !pattern) return false;
    const WCHAR* t = text;
    const WCHAR* p = pattern;
    const WCHAR* star = nullptr;
    const WCHAR* fallback = nullptr;
    while (*t) {
        if (*p == L'*') {
            star = p++;
            fallback = t;
            continue;
        }
        if (*p && (towlower(*p) == towlower(*t))) {
            ++p;
            ++t;
            continue;
        }
        if (star) {
            p = star + 1;
            t = ++fallback;
            continue;
        }
        return false;
    }
    while (*p == L'*') ++p;
    return *p == L'\0';
}

inline bool ExeNameLooksLikeUnrealShipping(const WCHAR* exeName) {
    if (!exeName || !*exeName) return false;
    static const WCHAR* kPatterns[] = {
        L"*-Win64-Shipping.exe",
        L"*-Win32-Shipping.exe",
        L"*-WinGDK-Shipping.exe",
        L"*-Shipping.exe",
        L"UnrealEditor*.exe",
        L"UE4Editor*.exe",
        L"UE5Editor*.exe",
    };
    for (const WCHAR* pat : kPatterns) {
        if (WildcardMatchI(exeName, pat)) return true;
    }
    return false;
}

// Process-name stem as stored by windows-animations GetProcessNameCached:
// file name only, lowercased, cut at the first '.' (so no .exe).
inline bool NormalizedStemLooksLikeUnrealShipping(const WCHAR* stem) {
    if (!stem || !*stem) return false;
    static const WCHAR* kSuffixes[] = {
        L"-win64-shipping",
        L"-win32-shipping",
        L"-wingdk-shipping",
        L"-shipping",
    };
    const size_t len = wcslen(stem);
    for (const WCHAR* suf : kSuffixes) {
        const size_t sl = wcslen(suf);
        if (len >= sl && _wcsicmp(stem + (len - sl), suf) == 0) return true;
    }
    if (WildcardMatchI(stem, L"unrealeditor*")) return true;
    if (WildcardMatchI(stem, L"ue4editor*")) return true;
    if (WildcardMatchI(stem, L"ue5editor*")) return true;
    return false;
}

inline bool ExeNameLooksLikeAntiCheat(const WCHAR* exeName) {
    if (!exeName || !*exeName) return false;
    static const WCHAR* kPatterns[] = {
        L"ACE-*.exe",
        L"ace-*.exe",
        L"SGuard*.exe",
        L"TenSafe*.exe",
        L"EasyAntiCheat*.exe",
        L"BEService*.exe",
        L"BEClient*.exe",
        L"vgc.exe",
        L"vgtray.exe",
        L"xhunter*.exe",
        L"TPHelper*.exe",
        L"TPSvc*.exe",
        L"TenProxy*.exe",
        L"GameLoader.exe",
        L"Ace-*.exe",
    };
    for (const WCHAR* pat : kPatterns) {
        if (WildcardMatchI(exeName, pat)) return true;
    }
    return false;
}

inline bool ModuleNameIsAntiCheat(const WCHAR* modFileName) {
    if (!modFileName || !*modFileName) return false;
    static const WCHAR* kPatterns[] = {
        L"ACE-Base*.dll", L"ACE-CTS*.dll", L"ACE-SSC*.dll", L"ACE-DFS*.dll",
        L"ACE-*.dll",
        L"SGuard*.dll",
        L"EasyAntiCheat*.dll",
        L"BEClient*.dll", L"BEService*.dll",
        L"vgkbootstatus.dll",
        L"xhunter*.dll",
        L"TenioDL.dll", L"TGuard.dll",
    };
    for (const WCHAR* pat : kPatterns) {
        if (WildcardMatchI(modFileName, pat)) return true;
    }
    return false;
}

inline bool ModuleNameIsGameEngine(const WCHAR* modFileName) {
    if (!modFileName || !*modFileName) return false;
    static const WCHAR* kPatterns[] = {
        L"UnityPlayer.dll",
        L"GameAssembly.dll",
        L"UE4Editor-Core.dll",
        L"UnrealEditor-Core.dll",
        L"EOSSDK-Win64-Shipping.dll",
    };
    for (const WCHAR* pat : kPatterns) {
        if (WildcardMatchI(modFileName, pat)) return true;
    }
    return false;
}

inline bool CurrentProcessHasMatchingModule(
    bool (*pred)(const WCHAR*)) {
    static const WCHAR* kCandidates[] = {
        L"ACE-Base.dll", L"ACE-Base64.dll", L"ACE-CTS.dll", L"ACE-CTS64.dll",
        L"ACE-SSC.dll", L"ACE-SSC64.dll", L"ACE-DFS64.dll",
        L"SGuardAgent64.dll", L"SGuardAgent32.dll", L"SGuard.dll",
        L"EasyAntiCheat_EOS.dll", L"EasyAntiCheat.dll",
        L"EOSSDK-Win64-Shipping.dll",
        L"BEClient_x64.dll", L"BEClient.dll", L"BEService_x64.dll",
        L"vgkbootstatus.dll", L"xhunter1.dll", L"TenioDL.dll", L"TGuard.dll",
        L"UnityPlayer.dll", L"GameAssembly.dll",
        L"UE4Editor-Core.dll", L"UnrealEditor-Core.dll",
    };
    for (const WCHAR* mod : kCandidates) {
        if (GetModuleHandleW(mod) && pred(mod)) return true;
    }
    return false;
}

inline bool ForeignProcessHasMatchingModule(DWORD pid, bool wantAntiCheat,
                                            bool wantEngine) {
    if (!pid || pid == GetCurrentProcessId()) return false;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap == INVALID_HANDLE_VALUE) {
        // Access denied is common under kernel AC — caller decides policy.
        return false;
    }
    MODULEENTRY32W me{};
    me.dwSize = sizeof(me);
    bool found = false;
    if (Module32FirstW(snap, &me)) {
        do {
            const WCHAR* file = me.szModule;
            if (wantAntiCheat && ModuleNameIsAntiCheat(file)) {
                found = true;
                break;
            }
            if (wantEngine && ModuleNameIsGameEngine(file)) {
                found = true;
                break;
            }
        } while (Module32NextW(snap, &me));
    }
    CloseHandle(snap);
    return found;
}

// Borderless / exclusive-style top-level window covering a monitor.
// PrintWindow + DWMWA_CLOAK on these (typical UE4/D3D games) is unsafe even
// when the animation code runs in explorer.
inline bool IsMonitorCoveringBorderlessWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) return false;
    // Has a normal captioned frame → ordinary app window, allow animation.
    if ((style & WS_CAPTION) == WS_CAPTION && (style & WS_THICKFRAME)) {
        return false;
    }
    RECT wr{};
    if (!GetWindowRect(hWnd, &wr)) return false;
    const int ww = wr.right - wr.left;
    const int wh = wr.bottom - wr.top;
    if (ww < 640 || wh < 480) return false;

    HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) return false;
    const RECT& mr = mi.rcMonitor;
    const int mw = mr.right - mr.left;
    const int mh = mr.bottom - mr.top;
    if (mw <= 0 || mh <= 0) return false;

    // Cover >= 95% of the monitor and sit within 8px of its edges.
    const bool covers =
        ww * 100 >= mw * 95 && wh * 100 >= mh * 95 &&
        wr.left <= mr.left + 8 && wr.top <= mr.top + 8 &&
        wr.right >= mr.right - 8 && wr.bottom >= mr.bottom - 8;
    if (!covers) return false;

    // Prefer popup / layered / topmost game surfaces; still skip huge
    // caption-less cover windows (borderless fullscreen).
    const LONG_PTR ex = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (style & WS_POPUP) return true;
    if (ex & (WS_EX_TOPMOST | WS_EX_LAYERED)) return true;
    if ((style & WS_CAPTION) != WS_CAPTION) return true;
    return false;
}

inline bool QueryProcessExeName(DWORD pid, WCHAR* out, size_t outCount,
                                bool* accessDenied) {
    if (accessDenied) *accessDenied = false;
    if (!out || outCount == 0) return false;
    out[0] = L'\0';
    if (!pid) return false;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) {
        if (accessDenied && GetLastError() == ERROR_ACCESS_DENIED) {
            *accessDenied = true;
        }
        return false;
    }
    WCHAR path[MAX_PATH]{};
    DWORD len = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(hProc, 0, path, &len) != FALSE;
    CloseHandle(hProc);
    if (!ok) return false;
    const WCHAR* name = wcsrchr(path, L'\\');
    wcsncpy_s(out, outCount, name ? name + 1 : path, _TRUNCATE);
    return true;
}

// Hard refuse for Wh_ModInit when this process itself is AC / helper.
inline bool ShouldHardRefuseInjection() {
    WCHAR exe[MAX_PATH]{};
    GetCurrentExeName(exe, ARRAYSIZE(exe));
    if (ExeNameLooksLikeAntiCheat(exe)) return true;
    return CurrentProcessHasMatchingModule(ModuleNameIsAntiCheat);
}

inline bool ShouldSkipAsGameProcess() {
    WCHAR exe[MAX_PATH]{};
    GetCurrentExeName(exe, ARRAYSIZE(exe));
    if (ExeNameLooksLikeUnrealShipping(exe)) return true;
    return CurrentProcessHasMatchingModule(ModuleNameIsGameEngine);
}

// Per-HWND gate for animation / cloak / PrintWindow against foreign windows.
// skipGames: when true (default), also skip UE/Unity/borderless-fullscreen and
// processes that deny OpenProcess (typical kernel-AC games).
inline bool ShouldSkipTargetWindow(HWND hWnd, bool skipGames) {
    if (!hWnd || !IsWindow(hWnd)) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return false;

    WCHAR exeName[MAX_PATH]{};
    bool accessDenied = false;
    const bool gotName = QueryProcessExeName(pid, exeName, ARRAYSIZE(exeName),
                                             &accessDenied);

    if (gotName) {
        if (ExeNameLooksLikeAntiCheat(exeName)) return true;
        if (skipGames && ExeNameLooksLikeUnrealShipping(exeName)) return true;
    }

    if (pid == GetCurrentProcessId()) {
        if (ShouldHardRefuseInjection()) return true;
        if (skipGames && ShouldSkipAsGameProcess()) return true;
    } else {
        if (ForeignProcessHasMatchingModule(pid, /*AC=*/true, /*engine=*/false)) {
            return true;
        }
        if (skipGames &&
            ForeignProcessHasMatchingModule(pid, /*AC=*/false, /*engine=*/true)) {
            return true;
        }
        // Protected process: cannot even query image name. Cloaking it from
        // explorer is a common UE4 / ACE crash path.
        if (skipGames && accessDenied) return true;
    }

    if (skipGames && IsMonitorCoveringBorderlessWindow(hWnd)) return true;
    return false;
}

}  // namespace ZenGameGuard
