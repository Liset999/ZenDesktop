// ZenDesktop shared game / anti-cheat injection + foreign-window guard.
// Used by mods that inject broadly (@include *) and may animate windows that
// belong to other processes (e.g. windows-animations running inside explorer).
//
// Layers:
//   1) Windhawk @exclude / Engine Exclude — exact names + wildcards, pre-inject
//      Windhawk matches patterns with no '\' against the *file name only*
//      (case-insensitive); patterns with '\' against the full path. So
//      "*-Win64-Shipping.exe" does match Client-Win64-Shipping.exe. Launchers
//      like "Wuthering Waves.exe" / "launcher.exe" are NOT covered by that
//      wildcard and need explicit names or path patterns / runtime refuse.
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

// Case-insensitive substring search (needle must be non-empty).
inline bool PathContainsI(const WCHAR* haystack, const WCHAR* needle) {
    if (!haystack || !needle || !*needle) return false;
    const size_t nlen = wcslen(needle);
    for (const WCHAR* p = haystack; *p; ++p) {
        size_t i = 0;
        while (i < nlen && p[i] &&
               towlower(p[i]) == towlower(needle[i])) {
            ++i;
        }
        if (i == nlen) return true;
        if (!p[i] && i < nlen) break;
    }
    return false;
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

// Kuro Games / Wuthering Waves (鸣潮) and related host helpers. Exact names
// that are NOT covered by *-Win64-Shipping.exe (launchers, SDK, webview).
// Do NOT put bare "launcher.exe" here — too common; path heuristics catch it.
inline bool ExeNameLooksLikeKnownGameHost(const WCHAR* exeName) {
    if (!exeName || !*exeName) return false;
    static const WCHAR* kExact[] = {
        L"Wuthering Waves.exe",
        L"Client-Win64-Shipping.exe",
        L"launcher_main.exe",
        L"launcher_updater.exe",
        L"KRSDKExternal.exe",
        L"KRWebView.exe",
        L"KRInstallExternal.exe",
        L"ClearThirdParty.exe",
        L"DummyWindow.exe",
        // Other Kuro / common CN-launcher hosts seen beside ACE trees.
        L"KuroLauncher.exe",
        L"krlauncher.exe",
        L"KRLauncher.exe",
    };
    for (const WCHAR* name : kExact) {
        if (EqualsIgnoreCase(exeName, name)) return true;
    }
    static const WCHAR* kPatterns[] = {
        L"Wuthering*.exe",
        L"KRSDK*.exe",
        L"KRWeb*.exe",
        L"KRInstall*.exe",
    };
    for (const WCHAR* pat : kPatterns) {
        if (WildcardMatchI(exeName, pat)) return true;
    }
    return false;
}

inline bool ExeNameLooksLikeAntiCheat(const WCHAR* exeName) {
    if (!exeName || !*exeName) return false;
    static const WCHAR* kPatterns[] = {
        L"ACE-*.exe",
        L"ace-*.exe",
        L"Ace-*.exe",
        L"SGuard*.exe",
        L"SGuardSvc*.exe",
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
        L"CrossProxy.exe",
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
        // UE shipping companion DLL used by Kuro / WW (and similar trees).
        L"*-Win64-ShippingBase.dll",
        L"*-Win64-Shipping.dll",
        L"Client-Win64-ShippingBase.dll",
    };
    for (const WCHAR* pat : kPatterns) {
        if (WildcardMatchI(modFileName, pat)) return true;
    }
    return false;
}

// Install-tree / sibling markers: cheap GetFileAttributes / path substring,
// no module snapshot. Catches launcher.exe sitting next to the game payload
// before ACE DLLs are loaded.
inline bool PathLooksLikeGameTree(const WCHAR* fullPath) {
    if (!fullPath || !*fullPath) return false;
    if (PathContainsI(fullPath, L"\\Wuthering Waves\\")) return true;
    if (PathContainsI(fullPath, L"\\Wuthering Waves Game\\")) return true;
    if (PathContainsI(fullPath, L"\\鸣潮\\")) return true;
    if (PathContainsI(fullPath, L"\\AntiCheatExpert\\")) return true;
    if (PathContainsI(fullPath, L"\\Client\\Binaries\\Win64\\")) return true;
    if (PathContainsI(fullPath, L"\\Engine\\Binaries\\")) return true;
    if (PathContainsI(fullPath, L"\\Binaries\\Win64\\") &&
        PathContainsI(fullPath, L"-Win64-Shipping.exe")) {
        return true;
    }
    return false;
}

inline bool SiblingMarkerExists(const WCHAR* exeDir, const WCHAR* fileName) {
    if (!exeDir || !*exeDir || !fileName || !*fileName) return false;
    WCHAR probe[MAX_PATH + 64]{};
    if (wcslen(exeDir) + 1 + wcslen(fileName) >= ARRAYSIZE(probe)) return false;
    wcsncpy_s(probe, ARRAYSIZE(probe), exeDir, _TRUNCATE);
    const size_t len = wcslen(probe);
    if (len && probe[len - 1] != L'\\') {
        wcsncat_s(probe, ARRAYSIZE(probe), L"\\", _TRUNCATE);
    }
    wcsncat_s(probe, ARRAYSIZE(probe), fileName, _TRUNCATE);
    const DWORD attrs = GetFileAttributesW(probe);
    return attrs != INVALID_FILE_ATTRIBUTES &&
           !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

inline bool DirLooksLikeGameInstall(const WCHAR* exeDir) {
    if (!exeDir || !*exeDir) return false;
    // Unity / IL2CPP payload next to the host exe.
    if (SiblingMarkerExists(exeDir, L"UnityPlayer.dll")) return true;
    if (SiblingMarkerExists(exeDir, L"GameAssembly.dll")) return true;
    // UE / Kuro shipping + ACE payload beside launchers.
    if (SiblingMarkerExists(exeDir, L"Client-Win64-Shipping.exe")) return true;
    if (SiblingMarkerExists(exeDir, L"Client-Win64-ShippingBase.dll")) {
        return true;
    }
    if (SiblingMarkerExists(exeDir, L"ACE-Base64.dll")) return true;
    if (SiblingMarkerExists(exeDir, L"ACE-Base.dll")) return true;
    // AntiCheatExpert\ as a sibling folder.
    WCHAR aceDir[MAX_PATH + 32]{};
    if (wcslen(exeDir) + 18 < ARRAYSIZE(aceDir)) {
        wcsncpy_s(aceDir, ARRAYSIZE(aceDir), exeDir, _TRUNCATE);
        const size_t len = wcslen(aceDir);
        if (len && aceDir[len - 1] != L'\\') {
            wcsncat_s(aceDir, ARRAYSIZE(aceDir), L"\\", _TRUNCATE);
        }
        wcsncat_s(aceDir, ARRAYSIZE(aceDir), L"AntiCheatExpert", _TRUNCATE);
        const DWORD attrs = GetFileAttributesW(aceDir);
        if (attrs != INVALID_FILE_ATTRIBUTES &&
            (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            return true;
        }
    }
    return false;
}

inline bool CurrentExePathLooksLikeGameTree() {
    WCHAR path[MAX_PATH]{};
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return false;
    if (PathLooksLikeGameTree(path)) return true;
    WCHAR dir[MAX_PATH]{};
    wcsncpy_s(dir, ARRAYSIZE(dir), path, _TRUNCATE);
    WCHAR* slash = wcsrchr(dir, L'\\');
    if (slash) *slash = L'\0';
    return DirLooksLikeGameInstall(dir);
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
        L"Client-Win64-ShippingBase.dll",
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

inline bool QueryProcessExePath(DWORD pid, WCHAR* out, size_t outCount,
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
    DWORD len = static_cast<DWORD>(outCount);
    const bool ok = QueryFullProcessImageNameW(hProc, 0, out, &len) != FALSE;
    CloseHandle(hProc);
    return ok;
}

inline bool QueryProcessExeName(DWORD pid, WCHAR* out, size_t outCount,
                                bool* accessDenied) {
    if (accessDenied) *accessDenied = false;
    if (!out || outCount == 0) return false;
    out[0] = L'\0';
    WCHAR path[MAX_PATH]{};
    if (!QueryProcessExePath(pid, path, ARRAYSIZE(path), accessDenied)) {
        return false;
    }
    const WCHAR* name = wcsrchr(path, L'\\');
    wcsncpy_s(out, outCount, name ? name + 1 : path, _TRUNCATE);
    return true;
}

// Hard refuse for Wh_ModInit when this process itself is AC / helper.
// Always on (independent of skip_games): never install hooks inside ACE /
// EAC / BattlEye / SGuard helper processes.
inline bool ShouldHardRefuseInjection() {
    WCHAR exe[MAX_PATH]{};
    GetCurrentExeName(exe, ARRAYSIZE(exe));
    if (ExeNameLooksLikeAntiCheat(exe)) return true;
    // ACE payload living under AntiCheatExpert\ even if the exe name is odd.
    WCHAR path[MAX_PATH]{};
    if (GetModuleFileNameW(nullptr, path, ARRAYSIZE(path)) &&
        PathContainsI(path, L"\\AntiCheatExpert\\")) {
        return true;
    }
    return CurrentProcessHasMatchingModule(ModuleNameIsAntiCheat);
}

inline bool ShouldSkipAsGameProcess() {
    WCHAR exe[MAX_PATH]{};
    GetCurrentExeName(exe, ARRAYSIZE(exe));
    if (ExeNameLooksLikeUnrealShipping(exe)) return true;
    if (ExeNameLooksLikeKnownGameHost(exe)) return true;
    if (CurrentExePathLooksLikeGameTree()) return true;
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

    WCHAR path[MAX_PATH]{};
    bool accessDenied = false;
    const bool gotPath =
        QueryProcessExePath(pid, path, ARRAYSIZE(path), &accessDenied);
    const WCHAR* exeName = nullptr;
    WCHAR exeNameBuf[MAX_PATH]{};
    if (gotPath) {
        const WCHAR* slash = wcsrchr(path, L'\\');
        exeName = slash ? slash + 1 : path;
        if (ExeNameLooksLikeAntiCheat(exeName)) return true;
        if (skipGames) {
            if (ExeNameLooksLikeUnrealShipping(exeName)) return true;
            if (ExeNameLooksLikeKnownGameHost(exeName)) return true;
            if (PathLooksLikeGameTree(path)) return true;
        }
    } else if (!accessDenied) {
        // Fall back to name-only query for completeness (rare).
        if (QueryProcessExeName(pid, exeNameBuf, ARRAYSIZE(exeNameBuf),
                                &accessDenied)) {
            exeName = exeNameBuf;
            if (ExeNameLooksLikeAntiCheat(exeName)) return true;
            if (skipGames && ExeNameLooksLikeUnrealShipping(exeName)) {
                return true;
            }
            if (skipGames && ExeNameLooksLikeKnownGameHost(exeName)) {
                return true;
            }
        }
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
