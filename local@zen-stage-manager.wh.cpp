// ==WindhawkMod==
// @id           zen-stage-manager
// @name         Stage Manager for Windows
// @description  Replicates macOS Stage Manager workspace grouping and sidebar on Windows.
// @version      0.1
// @author       ZenDesktop Developer
// @compilerOptions -ldwmapi -lshell32 -luser32 -lgdi32 -ld2d1 -lwindowscodecs -lole32
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
#include <d2d1.h>
#include <wincodec.h>

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

std::vector<HTHUMBNAIL> g_activeThumbnails;

ID2D1Factory* g_pD2DFactory = nullptr;
ID2D1HwndRenderTarget* g_pRenderTarget = nullptr;
ID2D1SolidColorBrush* g_pCardBgBrush = nullptr;
ID2D1SolidColorBrush* g_pCardOutlineBrush = nullptr;
ID2D1SolidColorBrush* g_pShadowBrush = nullptr;
ID2D1SolidColorBrush* g_pBadgeBgBrush = nullptr;
IWICImagingFactory* g_pWICFactory = nullptr;

void ClearThumbnails() {
    for (HTHUMBNAIL hThumb : g_activeThumbnails) {
        DwmUnregisterThumbnail(hThumb);
    }
    g_activeThumbnails.clear();
}

void UpdateThumbnails(HWND hwndSidebar) {
    ClearThumbnails();
    
    std::lock_guard<std::mutex> lock(g_stagesMutex);
    int yOffset = 20;
    for (size_t i = 0; i < g_stages.size() && i < 5; ++i) {
        if (g_stages[i].hwnds.empty()) continue;
        HWND target = g_stages[i].hwnds.back(); // Top window
        
        HTHUMBNAIL hThumb = nullptr;
        if (SUCCEEDED(DwmRegisterThumbnail(hwndSidebar, target, &hThumb))) {
            DWM_THUMBNAIL_PROPERTIES dpr = { 0 };
            dpr.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
            dpr.fVisible = TRUE;
            dpr.opacity = 240;
            
            size_t winCount = g_stages[i].hwnds.size();
            if (winCount > 1) {
                dpr.rcDestination = RECT{ 16, yOffset + 6, 68, yOffset + 48 };
            } else {
                dpr.rcDestination = RECT{ 12, yOffset + 2, 68, yOffset + 48 };
            }
            
            DwmUpdateThumbnailProperties(hThumb, &dpr);
            g_activeThumbnails.push_back(hThumb);
        }
        yOffset += 80;
    }
}

void DiscardDeviceResources() {
    if (g_pRenderTarget) { g_pRenderTarget->Release(); g_pRenderTarget = nullptr; }
    if (g_pCardBgBrush) { g_pCardBgBrush->Release(); g_pCardBgBrush = nullptr; }
    if (g_pCardOutlineBrush) { g_pCardOutlineBrush->Release(); g_pCardOutlineBrush = nullptr; }
    if (g_pShadowBrush) { g_pShadowBrush->Release(); g_pShadowBrush = nullptr; }
    if (g_pBadgeBgBrush) { g_pBadgeBgBrush->Release(); g_pBadgeBgBrush = nullptr; }
}

void InitD2DAndWIC() {
    if (!g_pD2DFactory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    }
    if (!g_pWICFactory) {
        CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&g_pWICFactory)
        );
    }
}

void CleanupD2DAndWIC() {
    DiscardDeviceResources();
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
    if (g_pWICFactory) { g_pWICFactory->Release(); g_pWICFactory = nullptr; }
}

HRESULT CreateDeviceResources(HWND hwnd) {
    HRESULT hr = S_OK;
    if (!g_pRenderTarget) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
        hr = g_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            ),
            D2D1::HwndRenderTargetProperties(hwnd, size),
            &g_pRenderTarget
        );
        if (SUCCEEDED(hr)) {
            hr = g_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White, 0.15f),
                &g_pCardBgBrush
            );
            if (SUCCEEDED(hr)) {
                hr = g_pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(D2D1::ColorF::White, 0.35f),
                    &g_pCardOutlineBrush
                );
            }
            if (SUCCEEDED(hr)) {
                hr = g_pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(D2D1::ColorF::Black, 0.25f),
                    &g_pShadowBrush
                );
            }
        }
    }
    return hr;
}

void DrawHIcon(ID2D1HwndRenderTarget* pRT, HICON hIcon, float x, float y, float width, float height) {
    if (!hIcon || !g_pWICFactory) return;
    
    IWICBitmap* pWICBitmap = nullptr;
    HRESULT hr = g_pWICFactory->CreateBitmapFromHICON(hIcon, &pWICBitmap);
    if (SUCCEEDED(hr)) {
        ID2D1Bitmap* pD2D1Bitmap = nullptr;
        hr = pRT->CreateBitmapFromWicBitmap(pWICBitmap, nullptr, &pD2D1Bitmap);
        if (SUCCEEDED(hr)) {
            D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
            pRT->DrawBitmap(pD2D1Bitmap, rect);
            pD2D1Bitmap->Release();
        }
        pWICBitmap->Release();
    }
}

void OnPaint(HWND hwnd) {
    InitD2DAndWIC();
    HRESULT hr = CreateDeviceResources(hwnd);
    if (SUCCEEDED(hr)) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        g_pRenderTarget->BeginDraw();
        
        g_pRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        
        if (!g_pBadgeBgBrush && g_pRenderTarget) {
            g_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(0.1f, 0.1f, 0.1f, 0.85f),
                &g_pBadgeBgBrush
            );
        }
        
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        int yOffset = 20;
        for (size_t i = 0; i < g_stages.size() && i < 5; ++i) {
            if (g_stages[i].hwnds.empty()) continue;
            
            size_t winCount = g_stages[i].hwnds.size();
            
            if (winCount > 1) {
                if (winCount >= 3) {
                    D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                        D2D1::RectF(10.0f, static_cast<float>(yOffset), 66.0f, static_cast<float>(yOffset + 46)),
                        6.0f, 6.0f
                    );
                    g_pRenderTarget->FillRoundedRectangle(&rrect, g_pCardBgBrush);
                    g_pRenderTarget->DrawRoundedRectangle(&rrect, g_pCardOutlineBrush, 1.0f);
                }
                D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                    D2D1::RectF(12.0f, static_cast<float>(yOffset + 2), 68.0f, static_cast<float>(yOffset + 48)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&rrect, g_pCardBgBrush);
                g_pRenderTarget->DrawRoundedRectangle(&rrect, g_pCardOutlineBrush, 1.0f);
                
                D2D1_ROUNDED_RECT frontRrect = D2D1::RoundedRect(
                    D2D1::RectF(14.0f, static_cast<float>(yOffset + 4), 70.0f, static_cast<float>(yOffset + 50)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&frontRrect, g_pCardBgBrush);
                g_pRenderTarget->DrawRoundedRectangle(&frontRrect, g_pCardOutlineBrush, 1.5f);
            } else {
                D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                    D2D1::RectF(10.0f, static_cast<float>(yOffset), 70.0f, static_cast<float>(yOffset + 50)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&rrect, g_pCardBgBrush);
                g_pRenderTarget->DrawRoundedRectangle(&rrect, g_pCardOutlineBrush, 1.5f);
            }
            
            D2D1_ELLIPSE badge = D2D1::Ellipse(
                D2D1::Point2F(14.0f, static_cast<float>(yOffset + 4)),
                11.0f,
                11.0f
            );
            if (g_pBadgeBgBrush) {
                g_pRenderTarget->FillEllipse(&badge, g_pBadgeBgBrush);
            }
            g_pRenderTarget->DrawEllipse(&badge, g_pCardOutlineBrush, 1.0f);
            
            if (g_stages[i].hIcon) {
                DrawHIcon(g_pRenderTarget, g_stages[i].hIcon, 6.0f, static_cast<float>(yOffset - 4), 16.0f, 16.0f);
            }
            
            yOffset += 80;
        }
        
        hr = g_pRenderTarget->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            DiscardDeviceResources();
        }
        EndPaint(hwnd, &ps);
    }
}

#define WM_APPBAR_CALLBACK (WM_USER + 101)
#define WM_INITIALIZE_WINDOWS (WM_USER + 102)

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
    if (GetClassName(hwnd, className, 256) > 0) {
        if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0 ||
            wcscmp(className, L"Shell_TrayWnd") == 0 || wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
            return false;
        }
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
    HICON hIcon = GetWindowIcon(hwnd);
    
    HWND oldStageNewTop = NULL;
    std::wstring oldStageProcessName;
    bool needOldStageIconUpdate = false;
    bool alreadyInStage = false;
    bool addedToExisting = false;
    
    {
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        
        // Check if the window is already in some stage
        for (auto it = g_stages.begin(); it != g_stages.end(); ) {
            auto hwndIt = std::find(it->hwnds.begin(), it->hwnds.end(), hwnd);
            if (hwndIt != it->hwnds.end()) {
                // Check UWP migration condition
                if (_wcsicmp(it->processName.c_str(), L"ApplicationFrameHost.exe") == 0 &&
                    _wcsicmp(it->processName.c_str(), processName.c_str()) != 0) {
                    
                    it->hwnds.erase(hwndIt);
                    if (!it->hwnds.empty()) {
                        oldStageNewTop = it->hwnds.back();
                        oldStageProcessName = it->processName;
                        needOldStageIconUpdate = true;
                    }
                    if (it->hwnds.empty()) {
                        it = g_stages.erase(it);
                    } else {
                        ++it;
                    }
                    alreadyInStage = false;
                    break;
                } else {
                    alreadyInStage = true;
                    break;
                }
            } else {
                ++it;
            }
        }
        
        if (!alreadyInStage) {
            for (auto& stage : g_stages) {
                if (!stage.isCustom && _wcsicmp(stage.processName.c_str(), processName.c_str()) == 0) {
                    stage.hwnds.push_back(hwnd);
                    stage.hIcon = hIcon;
                    addedToExisting = true;
                    break;
                }
            }
            
            if (!addedToExisting) {
                WindowStage newStage;
                newStage.hwnds.push_back(hwnd);
                newStage.processName = processName;
                newStage.hIcon = hIcon;
                newStage.isCustom = false;
                g_stages.push_back(newStage);
            }
        }
    }
    
    if (needOldStageIconUpdate && oldStageNewTop != NULL) {
        HICON hOldStageIcon = GetWindowIcon(oldStageNewTop);
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        for (auto& stage : g_stages) {
            if (_wcsicmp(stage.processName.c_str(), oldStageProcessName.c_str()) == 0 &&
                !stage.hwnds.empty() && stage.hwnds.back() == oldStageNewTop) {
                stage.hIcon = hOldStageIcon;
                break;
            }
        }
    }
}

void ActivateWindowInStage(HWND hwnd, const std::wstring& processName) {
    HICON hIcon = GetWindowIcon(hwnd);
    bool found = false;
    
    HWND oldStageNewTop = NULL;
    std::wstring oldStageProcessName;
    bool needOldStageIconUpdate = false;
    
    {
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        for (auto it = g_stages.begin(); it != g_stages.end(); ) {
            auto hwndIt = std::find(it->hwnds.begin(), it->hwnds.end(), hwnd);
            if (hwndIt != it->hwnds.end()) {
                if (_wcsicmp(it->processName.c_str(), L"ApplicationFrameHost.exe") == 0 &&
                    _wcsicmp(it->processName.c_str(), processName.c_str()) != 0) {
                    
                    // UWP migration
                    it->hwnds.erase(hwndIt);
                    if (!it->hwnds.empty()) {
                        oldStageNewTop = it->hwnds.back();
                        oldStageProcessName = it->processName;
                        needOldStageIconUpdate = true;
                    }
                    if (it->hwnds.empty()) {
                        it = g_stages.erase(it);
                    } else {
                        ++it;
                    }
                    found = false;
                    break;
                } else {
                    // Standard activation
                    it->hwnds.erase(hwndIt);
                    it->hwnds.push_back(hwnd);
                    it->hIcon = hIcon;
                    found = true;
                    break;
                }
            } else {
                ++it;
            }
        }
    }
    
    if (needOldStageIconUpdate && oldStageNewTop != NULL) {
        HICON hOldStageIcon = GetWindowIcon(oldStageNewTop);
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        for (auto& stage : g_stages) {
            if (_wcsicmp(stage.processName.c_str(), oldStageProcessName.c_str()) == 0 &&
                !stage.hwnds.empty() && stage.hwnds.back() == oldStageNewTop) {
                stage.hIcon = hOldStageIcon;
                break;
            }
        }
    }
    
    if (!found) {
        AddWindowToStage(hwnd, processName);
    }
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
                    UpdateThumbnails(hwnd);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                break;
            }
            case HSHELL_REDRAW: {
                if (IsAppWindow(targetHwnd)) {
                    std::wstring processName = GetProcessName(targetHwnd);
                    AddWindowToStage(targetHwnd, processName);
                    Wh_Log(L"HSHELL_REDRAW: HWND=%p, Process=%s", targetHwnd, processName.c_str());
                    UpdateThumbnails(hwnd);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                break;
            }
            case HSHELL_WINDOWDESTROYED: {
                HWND newTopHwnd = NULL;
                std::wstring stageProcessName;
                bool needIconUpdate = false;

                {
                    std::lock_guard<std::mutex> lock(g_stagesMutex);
                    for (auto it = g_stages.begin(); it != g_stages.end(); ) {
                        auto& stage = *it;
                        auto hwndIt = std::find(stage.hwnds.begin(), stage.hwnds.end(), targetHwnd);
                        if (hwndIt != stage.hwnds.end()) {
                            stage.hwnds.erase(hwndIt);
                            Wh_Log(L"HSHELL_WINDOWDESTROYED: HWND=%p, Process=%s", targetHwnd, stage.processName.c_str());
                            if (!stage.hwnds.empty()) {
                                newTopHwnd = stage.hwnds.back();
                                stageProcessName = stage.processName;
                                needIconUpdate = true;
                            }
                        }
                        if (stage.hwnds.empty()) {
                            it = g_stages.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }

                if (needIconUpdate && newTopHwnd != NULL) {
                    HICON hNewIcon = GetWindowIcon(newTopHwnd);
                    
                    std::lock_guard<std::mutex> lock(g_stagesMutex);
                    for (auto& stage : g_stages) {
                        if (_wcsicmp(stage.processName.c_str(), stageProcessName.c_str()) == 0 &&
                            !stage.hwnds.empty() && stage.hwnds.back() == newTopHwnd) {
                            stage.hIcon = hNewIcon;
                            break;
                        }
                    }
                }
                UpdateThumbnails(hwnd);
                InvalidateRect(hwnd, NULL, TRUE);
                break;
            }
            case HSHELL_WINDOWACTIVATED: {
                if (IsAppWindow(targetHwnd)) {
                    std::wstring processName = GetProcessName(targetHwnd);
                    Wh_Log(L"HSHELL_WINDOWACTIVATED: HWND=%p, Process=%s", targetHwnd, processName.c_str());
                    ActivateWindowInStage(targetHwnd, processName);
                    UpdateThumbnails(hwnd);
                    InvalidateRect(hwnd, NULL, TRUE);
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
            
            PostMessage(hwnd, WM_INITIALIZE_WINDOWS, 0, 0);
            return 0;
        }

        case WM_INITIALIZE_WINDOWS: {
            InitializeExistingWindows();
            UpdateThumbnails(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_SIZE: {
            if (g_pRenderTarget) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
                g_pRenderTarget->Resize(size);
            }
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_PAINT: {
            OnPaint(hwnd);
            return 0;
        }

        case WM_DESTROY: {
            ClearThumbnails();
            CleanupD2DAndWIC();
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
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = SidebarWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ZenStageManagerClass";
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    if (!RegisterClass(&wc)) {
        g_bInitDone.store(true);
        CoUninitialize();
        return 0;
    }

    if (g_bExitRequest.load()) {
        g_bInitDone.store(true);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        CoUninitialize();
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
        CoUninitialize();
        return 0;
    }

    if (hwnd == NULL) {
        g_bInitDone.store(true);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        CoUninitialize();
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
    CoUninitialize();
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
