// ==WindhawkMod==
// @id           windows-stage-manager
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
HANDLE g_hUIThread = NULL;
bool g_bExitThread = false;

LRESULT CALLBACK SidebarWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DESTROY:
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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Stage Manager Initializing");
    g_hUIThread = CreateThread(NULL, 0, UIThreadProc, NULL, 0, NULL);
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
