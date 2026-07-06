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
#include <vector>
#include <atomic>

std::atomic<HWND> g_hwndSidebar{ NULL };
std::atomic<bool> g_bInitDone{ false };
std::atomic<bool> g_bExitRequest{ false };
std::atomic<bool> g_bInitSuccess{ false };

// Thread handle for the UI window thread.
HANDLE g_hUIThread = NULL;

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

LRESULT CALLBACK SidebarWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            RegisterAppBar(hwnd);
            SetAcrylicEffect(hwnd);
            return 0;

        case WM_DESTROY:
            UnregisterAppBar(hwnd);
            g_hwndSidebar.store(NULL);
            PostQuitMessage(0);
            return 0;

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
