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

HWND g_hwndSidebar = NULL;
// Thread handle for the UI window thread.
HANDLE g_hUIThread = NULL;
// Event handle to synchronize sidebar window initialization.
HANDLE g_hInitEvent = NULL;

LRESULT CALLBACK SidebarWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DESTROY:
            g_hwndSidebar = NULL;
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

DWORD WINAPI UIThreadProc(LPVOID lpParam) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = SidebarWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ZenStageManagerClass";
    RegisterClass(&wc);

    g_hwndSidebar = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        wc.lpszClassName, L"ZenStageManager",
        WS_POPUP | WS_VISIBLE,
        0, 0, 80, GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, wc.hInstance, NULL
    );

    if (g_hInitEvent) {
        SetEvent(g_hInitEvent);
    }

    if (g_hwndSidebar == NULL) {
        UnregisterClass(L"ZenStageManagerClass", GetModuleHandle(NULL));
        return 0;
    }

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            // handle error or exit
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(L"ZenStageManagerClass", GetModuleHandle(NULL));
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Stage Manager Initializing");

    g_hInitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    g_hUIThread = CreateThread(NULL, 0, UIThreadProc, NULL, 0, NULL);
    if (g_hUIThread == NULL) {
        Wh_Log(L"Failed to create UI thread");
        if (g_hInitEvent) {
            CloseHandle(g_hInitEvent);
            g_hInitEvent = NULL;
        }
        return FALSE;
    }

    if (g_hInitEvent) {
        WaitForSingleObject(g_hInitEvent, 1000);
        CloseHandle(g_hInitEvent);
        g_hInitEvent = NULL;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Stage Manager Uninitializing");
    if (g_hwndSidebar) {
        PostMessage(g_hwndSidebar, WM_CLOSE, 0, 0);
    }
    if (g_hUIThread) {
        WaitForSingleObject(g_hUIThread, 1000);
        CloseHandle(g_hUIThread);
    }
}
