// ==WindhawkMod==
// @id           zen-stage-manager
// @name         Stage Manager for Windows
// @description  Replicates macOS Stage Manager workspace grouping and sidebar on Windows.
// @version      0.1
// @author       ZenDesktop Developer
// @compilerOptions -ldwmapi -lshell32 -luser32 -lgdi32
// @include      explorer.exe
// ==/WindhawkMod==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <vector>
#include <atomic>
#include <string>
#include <mutex>
#include <algorithm>

std::atomic<HWND> g_hwndSidebar{ NULL };
std::atomic<bool> g_bInitDone{ false };
std::atomic<bool> g_bExitRequest{ false };
std::atomic<bool> g_bInitSuccess{ false };

// Thread handle for the UI window thread.
HANDLE g_hUIThread = NULL;

struct WindowStage {
    std::vector<HWND> hwnds;
    std::wstring processName;
    HICON hIcon;
    bool isCustom;
};

std::vector<WindowStage> g_stages;
std::mutex g_stagesMutex;
UINT g_uShellHookMsg = 0;

#define WM_APPBAR_CALLBACK (WM_USER + 101)

struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    int Attrib;
    PVOID pvData;
    int cbData;
};

typedef BOOL(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

void SetAcrylicEffect(HWND hwnd) {
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        pfnSetWindowCompositionAttribute SetWindowCompositionAttribute =
            (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) {
            ACCENT_POLICY accent = { 3, 2, 0x00FFFFFF, 0 }; // ACCENT_ENABLE_BLURBEHIND / Acrylic
            WINDOWCOMPOSITIONATTRIBDATA data = { 19, &accent, sizeof(accent) }; // WCA_ACCENT_POLICY
            SetWindowCompositionAttribute(hwnd, &data);
        }
    }
}

void PositionAppBar(HWND hwnd) {
    APPBARDATA abd = { sizeof(abd) };
    abd.hWnd = hwnd;
    abd.uEdge = ABE_LEFT; // Default left docking
    abd.rc.left = 0;
    abd.rc.top = 0;
    abd.rc.right = 80;
    abd.rc.bottom = GetSystemMetrics(SM_CYSCREEN);

    SHAppBarMessage(ABM_QUERYPOS, &abd);
    SHAppBarMessage(ABM_SETPOS, &abd);

    MoveWindow(hwnd, abd.rc.left, abd.rc.top,
               abd.rc.right - abd.rc.left,
               abd.rc.bottom - abd.rc.top, TRUE);
}

void RegisterAppBar(HWND hwnd) {
    APPBARDATA abd = { sizeof(abd) };
    abd.hWnd = hwnd;
    abd.uCallbackMessage = WM_APPBAR_CALLBACK;
    SHAppBarMessage(ABM_NEW, &abd);

    PositionAppBar(hwnd);
}

void UnregisterAppBar(HWND hwnd) {
    APPBARDATA abd = { sizeof(abd) };
    abd.hWnd = hwnd;
    SHAppBarMessage(ABM_REMOVE, &abd);
}

HICON GetWindowIcon(HWND hwnd) {
    HICON hIcon = NULL;
    DWORD_PTR dwResult = 0;
    
    // 1. Try to get the small icon set explicitly for the window
    if (SendMessageTimeout(hwnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 100, &dwResult)) {
        hIcon = (HICON)dwResult;
    }
    
    // 2. Try the big icon
    if (!hIcon) {
        if (SendMessageTimeout(hwnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 100, &dwResult)) {
            hIcon = (HICON)dwResult;
        }
    }
    
    // 3. Try to get the small class icon
    if (!hIcon) {
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
    }
    
    // 4. Try to get the big class icon
    if (!hIcon) {
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    }
    
    // 5. Fallback to default application icon
    if (!hIcon) {
        hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }
    
    return hIcon;
}

bool IsAppWindow(HWND hwnd) {
    if (hwnd == g_hwndSidebar.load()) return false;
    if (!IsWindowVisible(hwnd)) return false;
    
    // Exclude tool windows and windows with owners that don't have WS_EX_APPWINDOW
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner != NULL && !(exStyle & WS_EX_APPWINDOW)) {
        return false;
    }
    
    // Class check to skip desktop and system components
    wchar_t className[256];
    GetClassName(hwnd, className, 256);
    if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0 ||
        wcscmp(className, L"Shell_TrayWnd") == 0 || wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }
    
    // Must have some window text
    wchar_t windowText[256];
    GetWindowText(hwnd, windowText, 256);
    if (wcslen(windowText) == 0) return false;

    return true;
}

std::wstring GetProcessNameByPid(DWORD processId) {
    std::wstring processName = L"Unknown";
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (hProcess) {
        wchar_t path[MAX_PATH];
        DWORD size = MAX_PATH;
        if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
            wchar_t* filename = wcsrchr(path, L'\\');
            processName = filename ? (filename + 1) : path;
        }
        CloseHandle(hProcess);
    } else {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32W);
            if (Process32FirstW(hSnapshot, &pe32)) {
                do {
                    if (pe32.th32ProcessID == processId) {
                        processName = pe32.szExeFile;
                        break;
                    }
                } while (Process32NextW(hSnapshot, &pe32));
            }
            CloseHandle(hSnapshot);
        }
    }
    return processName;
}

struct UwpEnumData {
    DWORD pid;
    HWND hwndFound;
};

BOOL CALLBACK EnumUwpChildProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[256];
    if (GetClassName(hwnd, className, 256)) {
        if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
            UwpEnumData* data = (UwpEnumData*)lParam;
            GetWindowThreadProcessId(hwnd, &data->pid);
            data->hwndFound = hwnd;
            return FALSE; // Stop enumeration
        }
    }
    return TRUE;
}

std::wstring GetProcessName(HWND hwnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    std::wstring processName = GetProcessNameByPid(processId);
    
    if (_wcsicmp(processName.c_str(), L"ApplicationFrameHost.exe") == 0) {
        UwpEnumData data = { 0, NULL };
        EnumChildWindows(hwnd, EnumUwpChildProc, (LPARAM)&data);
        if (data.hwndFound && data.pid != 0) {
            processName = GetProcessNameByPid(data.pid);
        }
    }
    return processName;
}

void AddWindowToStage(HWND hwnd, const std::wstring& processName) {
    std::lock_guard<std::mutex> lock(g_stagesMutex);
    
    // Check if the window is already in some stage to avoid duplicates
    for (const auto& stage : g_stages) {
        if (std::find(stage.hwnds.begin(), stage.hwnds.end(), hwnd) != stage.hwnds.end()) {
            return;
        }
    }
    
    for (auto& stage : g_stages) {
        if (!stage.isCustom && _wcsicmp(stage.processName.c_str(), processName.c_str()) == 0) {
            stage.hwnds.push_back(hwnd);
            return;
        }
    }
    
    WindowStage newStage;
    newStage.hwnds.push_back(hwnd);
    newStage.processName = processName;
    newStage.hIcon = GetWindowIcon(hwnd);
    newStage.isCustom = false;
    g_stages.push_back(newStage);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsAppWindow(hwnd)) {
        std::wstring processName = GetProcessName(hwnd);
        AddWindowToStage(hwnd, processName);
    }
    return TRUE;
}

void InitializeExistingWindows() {
    EnumWindows(EnumWindowsProc, 0);
}

LRESULT CALLBACK SidebarWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_uShellHookMsg != 0 && msg == g_uShellHookMsg) {
        WPARAM event = wp & 0x7FFF;
        HWND targetHwnd = (HWND)lp;
        
        switch (event) {
            case HSHELL_WINDOWCREATED: {
                if (IsAppWindow(targetHwnd)) {
                    std::wstring processName = GetProcessName(targetHwnd);
                    AddWindowToStage(targetHwnd, processName);
                    Wh_Log(L"HSHELL_WINDOWCREATED: HWND=%p, Process=%s", targetHwnd, processName.c_str());
                }
                break;
            }
            case HSHELL_REDRAW: {
                if (IsAppWindow(targetHwnd)) {
                    std::wstring processName = GetProcessName(targetHwnd);
                    AddWindowToStage(targetHwnd, processName);
                    Wh_Log(L"HSHELL_REDRAW: HWND=%p, Process=%s", targetHwnd, processName.c_str());
                }
                break;
            }
            case HSHELL_WINDOWDESTROYED: {
                std::lock_guard<std::mutex> lock(g_stagesMutex);
                for (auto it = g_stages.begin(); it != g_stages.end(); ) {
                    auto& stage = *it;
                    auto hwndIt = std::find(stage.hwnds.begin(), stage.hwnds.end(), targetHwnd);
                    if (hwndIt != stage.hwnds.end()) {
                        stage.hwnds.erase(hwndIt);
                        Wh_Log(L"HSHELL_WINDOWDESTROYED: HWND=%p, Process=%s", targetHwnd, stage.processName.c_str());
                        if (!stage.hwnds.empty()) {
                            stage.hIcon = GetWindowIcon(stage.hwnds.back());
                        }
                    }
                    if (stage.hwnds.empty()) {
                        it = g_stages.erase(it);
                    } else {
                        ++it;
                    }
                }
                break;
            }
            case HSHELL_WINDOWACTIVATED: {
                if (IsAppWindow(targetHwnd)) {
                    std::wstring processName = GetProcessName(targetHwnd);
                    Wh_Log(L"HSHELL_WINDOWACTIVATED: HWND=%p, Process=%s", targetHwnd, processName.c_str());
                }
                break;
            }
        }
        return 0;
    }
    
    switch (msg) {
        case WM_CREATE: {
            RegisterAppBar(hwnd);
            SetAcrylicEffect(hwnd);
            
            g_uShellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
            RegisterShellHookWindow(hwnd);
            
            InitializeExistingWindows();
            return 0;
        }

        case WM_DESTROY: {
            DeregisterShellHookWindow(hwnd);
            UnregisterAppBar(hwnd);
            
            {
                std::lock_guard<std::mutex> lock(g_stagesMutex);
                g_stages.clear();
            }
            
            g_hwndSidebar.store(NULL);
            PostQuitMessage(0);
            return 0;
        }

        case WM_APPBAR_CALLBACK:
            if (wp == ABN_POSCHANGED) {
                PositionAppBar(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

DWORD WINAPI UIThreadProc(LPVOID lpParam) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = SidebarWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ZenStageManagerClass";
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    if (!RegisterClass(&wc)) {
        g_bInitDone.store(true);
        return 0;
    }

    if (g_bExitRequest.load()) {
        g_bInitDone.store(true);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        wc.lpszClassName, L"ZenStageManager",
        WS_POPUP | WS_VISIBLE,
        0, 0, 80, GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, wc.hInstance, NULL
    );
    g_hwndSidebar.store(hwnd);

    if (g_bExitRequest.load()) {
        HWND hwndToDestroy = g_hwndSidebar.exchange(NULL);
        if (hwndToDestroy) {
            DestroyWindow(hwndToDestroy);
        }
        g_bInitDone.store(true);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    if (hwnd == NULL) {
        g_bInitDone.store(true);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    g_bInitSuccess.store(true);
    g_bInitDone.store(true);

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    HWND hwndRemaining = g_hwndSidebar.exchange(NULL);
    if (hwndRemaining && IsWindow(hwndRemaining)) {
        DestroyWindow(hwndRemaining);
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Stage Manager Initializing");

    g_hwndSidebar.store(NULL);
    g_bInitDone.store(false);
    g_bExitRequest.store(false);
    g_bInitSuccess.store(false);

    g_hUIThread = CreateThread(NULL, 0, UIThreadProc, NULL, 0, NULL);
    if (g_hUIThread == NULL) {
        Wh_Log(L"Failed to create UI thread");
        return FALSE;
    }

    for (int i = 0; i < 200; ++i) {
        if (g_bInitDone.load()) {
            break;
        }
        Sleep(5);
    }

    if (!(g_bInitDone.load() && g_bInitSuccess.load())) {
        Wh_Log(L"Initialization failed or timed out");
        g_bExitRequest.store(true);

        HWND hwnd = g_hwndSidebar.load();
        if (hwnd) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }

        if (g_hUIThread) {
            PostThreadMessage(GetThreadId(g_hUIThread), WM_QUIT, 0, 0);
            DWORD waitResult = WaitForSingleObject(g_hUIThread, 3000);
            if (waitResult == WAIT_TIMEOUT) {
                Wh_Log(L"Warning: UI thread did not exit within 3 seconds during init failure cleanup.");
            }
            CloseHandle(g_hUIThread);
            g_hUIThread = NULL;
        }
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Stage Manager Uninitializing");

    g_bExitRequest.store(true);

    HWND hwnd = g_hwndSidebar.load();
    if (hwnd) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    if (g_hUIThread) {
        PostThreadMessage(GetThreadId(g_hUIThread), WM_QUIT, 0, 0);
        DWORD waitResult = WaitForSingleObject(g_hUIThread, 3000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Warning: UI thread did not exit within 3 seconds.");
        }
        CloseHandle(g_hUIThread);
        g_hUIThread = NULL;
    }
}
