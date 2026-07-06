// ==WindhawkMod==
// @id           zen-stage-manager
// @name         Stage Manager for Windows
// @description  Replicates macOS Stage Manager workspace grouping and sidebar on Windows.
// @version      0.2
// @author       ZenDesktop Developer
// @compilerOptions -ldwmapi -lshell32 -luser32 -lgdi32 -ld2d1 -lwindowscodecs -lole32
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Stage Manager for Windows

Brings macOS-style Stage Manager to Windows 11. Automatically groups windows by application into stages, displays them in a sidebar with live previews, and lets you instantly switch between workspaces with a single click.

### Features:
- **AppBar Sidebar**: Docks to the side of the screen and automatically resists maximized windows.
- **Live DWM Thumbnails**: Real-time window previews using DirectX Desktop Window Manager.
- **Auto-Grouping**: Windows from the same app are automatically grouped into stages.
- **One-Click Switching**: Click a stage card to instantly minimize the current workspace and restore the target.
- **Win+G Grouping**: Press `Win + G` to move the current foreground window into the active stage.
- **Acrylic Glass**: Native Windows 11 acrylic blur background for a premium look.
- **Zero Bloat**: Runs entirely inside `explorer.exe` — no background processes, 0% CPU overhead.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sidebarEdge: 0
  $name: Sidebar Position
  $name:zh-CN: 侧边栏位置
  $description: "Which side of the screen the Stage Manager sidebar docks to."
  $description:zh-CN: "台前调度侧边栏停靠在屏幕的哪一侧。"
  $options:
    - 0: Left
    - 0:zh-CN: 左侧
    - 1: Right
    - 1:zh-CN: 右侧
- sidebarWidth: 80
  $name: Sidebar Width (px)
  $name:zh-CN: 侧边栏宽度 (像素)
  $description: "Width of the sidebar in pixels. Range: 60-160."
  $description:zh-CN: "侧边栏的宽度，单位为像素，范围 60-160。"
- acrylicOpacity: 180
  $name: Acrylic Opacity (0-255)
  $name:zh-CN: 亚克力透明度 (0-255)
  $description: "Opacity of the acrylic blur background. 0 = fully transparent, 255 = fully opaque."
  $description:zh-CN: "亚克力模糊背景的透明度，0=完全透明，255=完全不透明。"
- maxVisibleStages: 5
  $name: Maximum Visible Stages
  $name:zh-CN: 最多显示舞台数
  $description: "Maximum number of stage cards displayed in the sidebar at once."
  $description:zh-CN: "侧边栏中最多同时显示的舞台卡片数量。"
- enableHotkey: true
  $name: Enable Win+G Grouping Hotkey
  $name:zh-CN: 启用 Win+G 合并热键
  $description: "When enabled, pressing Win + G moves the foreground window into the active stage."
  $description:zh-CN: "启用后，按下 Win + G 可将前台窗口合并到当前活跃舞台中。"
- animationDisabled: true
  $name: Disable Window Animation During Switching
  $name:zh-CN: 切换时禁用窗口动画
  $description: "Disables minimize/restore animations when switching stages for snappier transitions."
  $description:zh-CN: "切换舞台时禁用最小化/还原动画，获得更干脆的切换体验。"
*/
// ==/WindhawkModSettings==

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
#include <windowsx.h>

#define WM_STAGE_SETTINGS_CHANGED (WM_USER + 103)

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
    ID2D1Bitmap* pD2DBitmap = nullptr;

    WindowStage() = default;
    ~WindowStage() {
        if (pD2DBitmap) {
            p2dRelease(pD2DBitmap);
        }
    }

    // Helper to safely release interfaces
    template<typename T>
    void p2dRelease(T*& ptr) {
        if (ptr) {
            ptr->Release();
            ptr = nullptr;
        }
    }

    // Move constructor
    WindowStage(WindowStage&& other) noexcept 
        : hwnds(std::move(other.hwnds)),
          processName(std::move(other.processName)),
          hIcon(other.hIcon),
          isCustom(other.isCustom),
          pD2DBitmap(other.pD2DBitmap) {
        other.pD2DBitmap = nullptr;
    }

    // Move assignment
    WindowStage& operator=(WindowStage&& other) noexcept {
        if (this != &other) {
            if (pD2DBitmap) pD2DBitmap->Release();
            hwnds = std::move(other.hwnds);
            processName = std::move(other.processName);
            hIcon = other.hIcon;
            isCustom = other.isCustom;
            pD2DBitmap = other.pD2DBitmap;
            other.pD2DBitmap = nullptr;
        }
        return *this;
    }

    // Disable copy constructor and assignment
    WindowStage(const WindowStage&) = delete;
    WindowStage& operator=(const WindowStage&) = delete;
};

std::vector<WindowStage> g_stages;
std::mutex g_stagesMutex;
UINT g_uShellHookMsg = 0;
int g_activeStageIndex = -1;

struct ModSettings {
    int  sidebarEdge;        // 0 = left, 1 = right
    int  sidebarWidth;       // px, 60-160
    int  acrylicOpacity;     // 0-255
    int  maxVisibleStages;   // 1-10
    bool enableHotkey;       // Win+G
    bool animationDisabled;  // disable transitions during switch
} g_settings;

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

struct ThumbnailEntry {
    HWND target;
    int  yOffset;
    size_t winCount;
};

void UpdateThumbnails(HWND hwndSidebar) {
    ClearThumbnails();

    // Collect stage data under the mutex, then release before calling DWM APIs.
    std::vector<ThumbnailEntry> entries;
    {
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        int yOffset = 20;
        int maxStages = g_settings.maxVisibleStages;
        if (maxStages < 1) maxStages = 5;
        for (size_t i = 0; i < g_stages.size() && (int)i < maxStages; ++i) {
            if (g_stages[i].hwnds.empty()) continue;
            ThumbnailEntry e;
            e.target   = g_stages[i].hwnds.back(); // Top window
            e.yOffset  = yOffset;
            e.winCount = g_stages[i].hwnds.size();
            entries.push_back(e);
            yOffset += 80;
        }
    } // mutex released here

    int baseLeft = (g_settings.sidebarWidth - 60) / 2;

    // Perform all DWM API calls outside the lock.
    for (const auto& e : entries) {
        HTHUMBNAIL hThumb = nullptr;
        if (SUCCEEDED(DwmRegisterThumbnail(hwndSidebar, e.target, &hThumb))) {
            DWM_THUMBNAIL_PROPERTIES dpr = { 0 };
            dpr.dwFlags  = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
            dpr.fVisible = TRUE;
            dpr.opacity  = 240;
            dpr.fSourceClientAreaOnly = TRUE;

            if (e.winCount > 1) {
                dpr.rcDestination = RECT{ baseLeft + 6, e.yOffset + 6, baseLeft + 58, e.yOffset + 48 };
            } else {
                dpr.rcDestination = RECT{ baseLeft + 2, e.yOffset + 2, baseLeft + 58, e.yOffset + 48 };
            }

            DwmUpdateThumbnailProperties(hThumb, &dpr);
            g_activeThumbnails.push_back(hThumb);
        }
    }
}

void LoadSettings() {
    g_settings.sidebarEdge = Wh_GetIntSetting(L"sidebarEdge");
    if (g_settings.sidebarEdge < 0) g_settings.sidebarEdge = 0;
    if (g_settings.sidebarEdge > 1) g_settings.sidebarEdge = 1;

    g_settings.sidebarWidth = Wh_GetIntSetting(L"sidebarWidth");
    if (g_settings.sidebarWidth < 60)  g_settings.sidebarWidth = 60;
    if (g_settings.sidebarWidth > 160) g_settings.sidebarWidth = 160;

    g_settings.acrylicOpacity = Wh_GetIntSetting(L"acrylicOpacity");
    if (g_settings.acrylicOpacity < 0)   g_settings.acrylicOpacity = 0;
    if (g_settings.acrylicOpacity > 255) g_settings.acrylicOpacity = 255;

    g_settings.maxVisibleStages = Wh_GetIntSetting(L"maxVisibleStages");
    if (g_settings.maxVisibleStages < 1)  g_settings.maxVisibleStages = 1;
    if (g_settings.maxVisibleStages > 10) g_settings.maxVisibleStages = 10;

    g_settings.enableHotkey      = Wh_GetIntSetting(L"enableHotkey") != 0;
    g_settings.animationDisabled = Wh_GetIntSetting(L"animationDisabled") != 0;

    Wh_Log(L"[StageMgr] Settings: edge=%d, width=%d, opacity=%d, maxStages=%d, hotkey=%d, animDisabled=%d",
           g_settings.sidebarEdge, g_settings.sidebarWidth, g_settings.acrylicOpacity,
           g_settings.maxVisibleStages, (int)g_settings.enableHotkey, (int)g_settings.animationDisabled);
}

int FindStageIndexByHwnd(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_stagesMutex);
    for (size_t i = 0; i < g_stages.size(); ++i) {
        auto it = std::find(g_stages[i].hwnds.begin(), g_stages[i].hwnds.end(), hwnd);
        if (it != g_stages[i].hwnds.end()) {
            return (int)i;
        }
    }
    return -1;
}

void SwitchToStage(int targetIndex) {
    if (targetIndex < 0) return;

    std::vector<HWND> currentStageWindows;
    std::vector<HWND> targetStageWindows;
    HWND targetTopWindow = NULL;

    {
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        if (targetIndex >= (int)g_stages.size()) return;
        if (targetIndex == g_activeStageIndex) return;

        if (g_activeStageIndex >= 0 && g_activeStageIndex < (int)g_stages.size()) {
            currentStageWindows = g_stages[g_activeStageIndex].hwnds;
        }
        targetStageWindows = g_stages[targetIndex].hwnds;
        if (!targetStageWindows.empty()) {
            targetTopWindow = targetStageWindows.back();
        }
    }

    BOOL disableAnim = TRUE;
    BOOL enableAnim = FALSE;
    BOOL shouldDisable = g_settings.animationDisabled;

    for (HWND hwnd : currentStageWindows) {
        if (IsWindow(hwnd)) {
            if (shouldDisable) {
                DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disableAnim, sizeof(disableAnim));
            }
            ShowWindowAsync(hwnd, SW_MINIMIZE);
            if (shouldDisable) {
                DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &enableAnim, sizeof(enableAnim));
            }
        }
    }

    for (HWND hwnd : targetStageWindows) {
        if (IsWindow(hwnd)) {
            if (shouldDisable) {
                DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disableAnim, sizeof(disableAnim));
            }
            ShowWindowAsync(hwnd, SW_RESTORE);
            if (shouldDisable) {
                DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &enableAnim, sizeof(enableAnim));
            }
        }
    }

    if (targetTopWindow && IsWindow(targetTopWindow)) {
        SetForegroundWindow(targetTopWindow);
    }

    g_activeStageIndex = targetIndex;
}

HICON GetWindowIcon(HWND hwnd);
HRESULT CreateD2DBitmapFromHIcon(HICON hIcon, ID2D1HwndRenderTarget* pRT, ID2D1Bitmap** ppBitmap);

void MoveWindowToActiveStage(HWND hwndToMove) {
    if (!hwndToMove || !IsWindow(hwndToMove)) return;

    HICON hIcon = GetWindowIcon(hwndToMove);
    ID2D1Bitmap* pNewBitmap = nullptr;
    if (hIcon && g_pRenderTarget) {
        CreateD2DBitmapFromHIcon(hIcon, g_pRenderTarget, &pNewBitmap);
    }

    int sourceIndex = -1;
    int targetIndex = g_activeStageIndex;

    {
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        for (size_t i = 0; i < g_stages.size(); ++i) {
            auto it = std::find(g_stages[i].hwnds.begin(), g_stages[i].hwnds.end(), hwndToMove);
            if (it != g_stages[i].hwnds.end()) {
                sourceIndex = (int)i;
                break;
            }
        }

        if (sourceIndex >= 0 && sourceIndex != targetIndex &&
            targetIndex >= 0 && targetIndex < (int)g_stages.size()) {

            auto& sourceStage = g_stages[sourceIndex];
            auto& targetStage = g_stages[targetIndex];

            auto it = std::find(sourceStage.hwnds.begin(), sourceStage.hwnds.end(), hwndToMove);
            if (it != sourceStage.hwnds.end()) {
                sourceStage.hwnds.erase(it);
            }

            targetStage.hwnds.push_back(hwndToMove);
            targetStage.isCustom = true;

            if (hIcon) {
                targetStage.hIcon = hIcon;
                if (targetStage.pD2DBitmap) {
                    targetStage.pD2DBitmap->Release();
                }
                targetStage.pD2DBitmap = pNewBitmap;
                pNewBitmap = nullptr; // transferred
            }

            if (sourceStage.hwnds.empty()) {
                g_stages.erase(g_stages.begin() + sourceIndex);
                if (g_activeStageIndex > sourceIndex) {
                    g_activeStageIndex--;
                }
            }
        }
    }

    if (pNewBitmap) {
        pNewBitmap->Release();
    }
}

void DiscardDeviceResources() {
    // Release per-stage cached bitmaps
    {
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        for (auto& stage : g_stages) {
            if (stage.pD2DBitmap) { stage.pD2DBitmap->Release(); stage.pD2DBitmap = nullptr; }
        }
    }
    if (g_pRenderTarget) { g_pRenderTarget->Release(); g_pRenderTarget = nullptr; }
    if (g_pCardBgBrush) { g_pCardBgBrush->Release(); g_pCardBgBrush = nullptr; }
    if (g_pCardOutlineBrush) { g_pCardOutlineBrush->Release(); g_pCardOutlineBrush = nullptr; }
    if (g_pShadowBrush) { g_pShadowBrush->Release(); g_pShadowBrush = nullptr; }
    if (g_pBadgeBgBrush) { g_pBadgeBgBrush->Release(); g_pBadgeBgBrush = nullptr; }
}

void InitD2DAndWIC() {
    if (!g_pD2DFactory) {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
        if (FAILED(hr)) {
            Wh_Log(L"D2D1CreateFactory failed: 0x%08X", hr);
            return;
        }
    }
    if (!g_pWICFactory) {
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&g_pWICFactory)
        );
        if (FAILED(hr)) {
            Wh_Log(L"CoCreateInstance WICImagingFactory failed: 0x%08X", hr);
        }
    }
}

// Helper: create or update the cached D2D bitmap for a stage's icon.
HRESULT CreateD2DBitmapFromHIcon(HICON hIcon, ID2D1HwndRenderTarget* pRT, ID2D1Bitmap** ppBitmap) {
    if (!hIcon || !g_pWICFactory || !pRT || !ppBitmap) return E_INVALIDARG;
    *ppBitmap = nullptr;
    IWICBitmap* pWICBitmap = nullptr;
    HRESULT hr = g_pWICFactory->CreateBitmapFromHICON(hIcon, &pWICBitmap);
    if (SUCCEEDED(hr)) {
        hr = pRT->CreateBitmapFromWicBitmap(pWICBitmap, nullptr, ppBitmap);
        pWICBitmap->Release();
    }
    return hr;
}

void CleanupD2DAndWIC() {
    DiscardDeviceResources();
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
    if (g_pWICFactory) { g_pWICFactory->Release(); g_pWICFactory = nullptr; }
}

HRESULT CreateDeviceResources(HWND hwnd) {
    if (!g_pD2DFactory) return E_FAIL;
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
            if (SUCCEEDED(hr)) {
                hr = g_pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(0.1f, 0.1f, 0.1f, 0.85f),
                    &g_pBadgeBgBrush
                );
            }
        }
    }
    return hr;
}

// Draw a pre-cached D2D bitmap at the specified position/size.
void DrawHIcon(ID2D1HwndRenderTarget* pRT, ID2D1Bitmap* pBitmap, float x, float y, float width, float height) {
    if (!pBitmap || !pRT) return;
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
    pRT->DrawBitmap(pBitmap, rect);
}

void OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);
    HRESULT hr = CreateDeviceResources(hwnd);
    if (SUCCEEDED(hr)) {
        g_pRenderTarget->BeginDraw();

        g_pRenderTarget->Clear(D2D1::ColorF(0.15f, 0.15f, 0.15f, 0.6f));

        static int paintCount = 0;
        if (paintCount++ % 50 == 0) {
            Wh_Log(L"StageMgr OnPaint: stages=%d, paintCount=%d", (int)g_stages.size(), paintCount);
        }

        std::lock_guard<std::mutex> lock(g_stagesMutex);
        float baseLeft = (g_settings.sidebarWidth - 60) / 2.0f;
        int yOffset = 20;
        int maxStages = g_settings.maxVisibleStages;
        if (maxStages < 1) maxStages = 5;
        for (size_t i = 0; i < g_stages.size() && (int)i < maxStages; ++i) {
            if (g_stages[i].hwnds.empty()) continue;

            size_t winCount = g_stages[i].hwnds.size();

            if (winCount > 1) {
                if (winCount >= 3) {
                    // Shadow for back card (3+ windows)
                    D2D1_ROUNDED_RECT shadowRrect0 = D2D1::RoundedRect(
                        D2D1::RectF(baseLeft + 2.0f, static_cast<float>(yOffset + 2), baseLeft + 58.0f, static_cast<float>(yOffset + 48)),
                        6.0f, 6.0f
                    );
                    g_pRenderTarget->FillRoundedRectangle(&shadowRrect0, g_pShadowBrush);

                    D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                        D2D1::RectF(baseLeft, static_cast<float>(yOffset), baseLeft + 56.0f, static_cast<float>(yOffset + 46)),
                        6.0f, 6.0f
                    );
                    g_pRenderTarget->FillRoundedRectangle(&rrect, g_pCardBgBrush);
                    g_pRenderTarget->DrawRoundedRectangle(&rrect, g_pCardOutlineBrush, 1.0f);
                }
                // Shadow for middle card
                D2D1_ROUNDED_RECT shadowRrect1 = D2D1::RoundedRect(
                    D2D1::RectF(baseLeft + 4.0f, static_cast<float>(yOffset + 4), baseLeft + 60.0f, static_cast<float>(yOffset + 50)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&shadowRrect1, g_pShadowBrush);

                D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                    D2D1::RectF(baseLeft + 2.0f, static_cast<float>(yOffset + 2), baseLeft + 58.0f, static_cast<float>(yOffset + 48)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&rrect, g_pCardBgBrush);
                g_pRenderTarget->DrawRoundedRectangle(&rrect, g_pCardOutlineBrush, 1.0f);

                // Shadow for front card
                D2D1_ROUNDED_RECT shadowFront = D2D1::RoundedRect(
                    D2D1::RectF(baseLeft + 6.0f, static_cast<float>(yOffset + 6), baseLeft + 62.0f, static_cast<float>(yOffset + 52)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&shadowFront, g_pShadowBrush);

                D2D1_ROUNDED_RECT frontRrect = D2D1::RoundedRect(
                    D2D1::RectF(baseLeft + 4.0f, static_cast<float>(yOffset + 4), baseLeft + 60.0f, static_cast<float>(yOffset + 50)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&frontRrect, g_pCardBgBrush);
                g_pRenderTarget->DrawRoundedRectangle(&frontRrect, g_pCardOutlineBrush, 1.5f);
            } else {
                // Shadow for single-window card
                D2D1_ROUNDED_RECT shadowRrect = D2D1::RoundedRect(
                    D2D1::RectF(baseLeft + 2.0f, static_cast<float>(yOffset + 2), baseLeft + 62.0f, static_cast<float>(yOffset + 52)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&shadowRrect, g_pShadowBrush);

                D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                    D2D1::RectF(baseLeft, static_cast<float>(yOffset), baseLeft + 60.0f, static_cast<float>(yOffset + 50)),
                    6.0f, 6.0f
                );
                g_pRenderTarget->FillRoundedRectangle(&rrect, g_pCardBgBrush);
                g_pRenderTarget->DrawRoundedRectangle(&rrect, g_pCardOutlineBrush, 1.5f);
            }

            // Badge: center at (baseLeft + 4.0f, yOffset + 4 + 11) so the badge top is at yOffset + 4
            D2D1_ELLIPSE badge = D2D1::Ellipse(
                D2D1::Point2F(baseLeft + 4.0f, static_cast<float>(yOffset + 4 + 11)),
                11.0f,
                11.0f
            );
            if (g_pBadgeBgBrush) {
                g_pRenderTarget->FillEllipse(&badge, g_pBadgeBgBrush);
            }
            g_pRenderTarget->DrawEllipse(&badge, g_pCardOutlineBrush, 1.0f);

            if (g_stages[i].pD2DBitmap) {
                DrawHIcon(g_pRenderTarget, g_stages[i].pD2DBitmap, baseLeft - 7.0f, static_cast<float>(yOffset + 4), 22.0f, 22.0f);
            }

            yOffset += 80;
        }

        hr = g_pRenderTarget->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            DiscardDeviceResources();
        }
    }
    EndPaint(hwnd, &ps);
}

#define WM_APPBAR_CALLBACK (WM_USER + 101)
#define WM_INITIALIZE_WINDOWS (WM_USER + 102)
#define ID_HOTKEY_GROUP_WINDOW 1001

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
            BYTE alpha = (BYTE)g_settings.acrylicOpacity;
            int gradientColor = (alpha << 24) | 0x00FFFFFF;
            ACCENT_POLICY accent = { 3, 2, gradientColor, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { 19, &accent, sizeof(accent) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
    }
}

void PositionAppBar(HWND hwnd) {
    APPBARDATA abd = { sizeof(abd) };
    abd.hWnd = hwnd;
    abd.uEdge = (g_settings.sidebarEdge == 1) ? ABE_RIGHT : ABE_LEFT;

    int width = g_settings.sidebarWidth;
    if (width < 60) width = 80;

    abd.rc.top = 0;
    abd.rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    if (abd.uEdge == ABE_LEFT) {
        abd.rc.left = 0;
        abd.rc.right = width;
    } else {
        abd.rc.right = GetSystemMetrics(SM_CXSCREEN);
        abd.rc.left = abd.rc.right - width;
    }

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
    ID2D1Bitmap* pNewBitmap = nullptr;
    if (hIcon && g_pRenderTarget) {
        CreateD2DBitmapFromHIcon(hIcon, g_pRenderTarget, &pNewBitmap);
    }
    
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
                    if (stage.pD2DBitmap) {
                        stage.pD2DBitmap->Release();
                    }
                    stage.pD2DBitmap = pNewBitmap;
                    pNewBitmap = nullptr; // transferred
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
                newStage.pD2DBitmap = pNewBitmap;
                pNewBitmap = nullptr; // transferred
                g_stages.push_back(std::move(newStage));
            }
        }
    }
    
    if (needOldStageIconUpdate && oldStageNewTop != NULL) {
        HICON hOldStageIcon = GetWindowIcon(oldStageNewTop);
        ID2D1Bitmap* pOldBitmap = nullptr;
        if (hOldStageIcon && g_pRenderTarget) {
            CreateD2DBitmapFromHIcon(hOldStageIcon, g_pRenderTarget, &pOldBitmap);
        }
        
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        for (auto& stage : g_stages) {
            if (_wcsicmp(stage.processName.c_str(), oldStageProcessName.c_str()) == 0 &&
                !stage.hwnds.empty() && stage.hwnds.back() == oldStageNewTop) {
                stage.hIcon = hOldStageIcon;
                if (stage.pD2DBitmap) {
                    stage.pD2DBitmap->Release();
                }
                stage.pD2DBitmap = pOldBitmap;
                pOldBitmap = nullptr; // transferred
                break;
            }
        }
        if (pOldBitmap) {
            pOldBitmap->Release();
        }
    }
    
    if (pNewBitmap) {
        pNewBitmap->Release();
    }
}

void ActivateWindowInStage(HWND hwnd, const std::wstring& processName) {
    HICON hIcon = GetWindowIcon(hwnd);
    ID2D1Bitmap* pNewBitmap = nullptr;
    if (hIcon && g_pRenderTarget) {
        CreateD2DBitmapFromHIcon(hIcon, g_pRenderTarget, &pNewBitmap);
    }
    
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
                    if (it->pD2DBitmap) {
                        it->pD2DBitmap->Release();
                    }
                    it->pD2DBitmap = pNewBitmap;
                    pNewBitmap = nullptr; // transferred
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
        ID2D1Bitmap* pOldBitmap = nullptr;
        if (hOldStageIcon && g_pRenderTarget) {
            CreateD2DBitmapFromHIcon(hOldStageIcon, g_pRenderTarget, &pOldBitmap);
        }
        
        std::lock_guard<std::mutex> lock(g_stagesMutex);
        for (auto& stage : g_stages) {
            if (_wcsicmp(stage.processName.c_str(), oldStageProcessName.c_str()) == 0 &&
                !stage.hwnds.empty() && stage.hwnds.back() == oldStageNewTop) {
                stage.hIcon = hOldStageIcon;
                if (stage.pD2DBitmap) {
                    stage.pD2DBitmap->Release();
                }
                stage.pD2DBitmap = pOldBitmap;
                pOldBitmap = nullptr; // transferred
                break;
            }
        }
        if (pOldBitmap) {
            pOldBitmap->Release();
        }
    }
    
    if (pNewBitmap) {
        pNewBitmap->Release();
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
                    ID2D1Bitmap* pNewBitmap = nullptr;
                    if (hNewIcon && g_pRenderTarget) {
                        CreateD2DBitmapFromHIcon(hNewIcon, g_pRenderTarget, &pNewBitmap);
                    }
                    
                    std::lock_guard<std::mutex> lock(g_stagesMutex);
                    for (auto& stage : g_stages) {
                        if (_wcsicmp(stage.processName.c_str(), stageProcessName.c_str()) == 0 &&
                            !stage.hwnds.empty() && stage.hwnds.back() == newTopHwnd) {
                            stage.hIcon = hNewIcon;
                            if (stage.pD2DBitmap) {
                                stage.pD2DBitmap->Release();
                            }
                            stage.pD2DBitmap = pNewBitmap;
                            pNewBitmap = nullptr; // transferred
                            break;
                        }
                    }
                    if (pNewBitmap) pNewBitmap->Release();
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
                    int idx = FindStageIndexByHwnd(targetHwnd);
                    if (idx >= 0) {
                        g_activeStageIndex = idx;
                    }
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
            LoadSettings();
            InitD2DAndWIC();
            RegisterAppBar(hwnd);
            SetAcrylicEffect(hwnd);
            
            g_uShellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
            RegisterShellHookWindow(hwnd);

            if (g_settings.enableHotkey) {
                RegisterHotKey(hwnd, ID_HOTKEY_GROUP_WINDOW, MOD_WIN, 0x47);
            }

            Wh_Log(L"StageMgr WM_CREATE: hwnd=%p, sidebarEdge=%d, width=%d, opacity=%d",
                   hwnd, g_settings.sidebarEdge, g_settings.sidebarWidth, g_settings.acrylicOpacity);
            
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

        case WM_LBUTTONDOWN: {
            int xPos = GET_X_LPARAM(lp);
            int yPos = GET_Y_LPARAM(lp);

            std::vector<int> visibleStageIndices;
            {
                std::lock_guard<std::mutex> lock(g_stagesMutex);
                int maxStages = g_settings.maxVisibleStages;
                if (maxStages < 1) maxStages = 5;
                for (size_t i = 0; i < g_stages.size() && (int)visibleStageIndices.size() < maxStages; ++i) {
                    if (!g_stages[i].hwnds.empty()) {
                        visibleStageIndices.push_back((int)i);
                    }
                }
            }

            int baseLeft = (g_settings.sidebarWidth - 60) / 2;
            int yOffset = 20;
            for (size_t i = 0; i < visibleStageIndices.size(); ++i) {
                int cardTop = yOffset;
                int cardBottom = yOffset + 50;
                int cardLeft = baseLeft;
                int cardRight = baseLeft + 60;

                if (xPos >= cardLeft && xPos <= cardRight &&
                    yPos >= cardTop && yPos <= cardBottom) {
                    SwitchToStage(visibleStageIndices[i]);
                    return 0;
                }
                yOffset += 80;
            }
            return 0;
        }

        case WM_HOTKEY: {
            if (wp == ID_HOTKEY_GROUP_WINDOW) {
                HWND foreground = GetForegroundWindow();
                if (foreground && IsAppWindow(foreground)) {
                    int fgIndex = FindStageIndexByHwnd(foreground);
                    if (fgIndex >= 0 && fgIndex != g_activeStageIndex && g_activeStageIndex >= 0) {
                        MoveWindowToActiveStage(foreground);
                        UpdateThumbnails(hwnd);
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                }
            }
            return 0;
        }

        case WM_DESTROY: {
            if (g_settings.enableHotkey) {
                UnregisterHotKey(hwnd, ID_HOTKEY_GROUP_WINDOW);
            }
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

        case WM_STAGE_SETTINGS_CHANGED: {
            bool oldHotkeyEnabled = g_settings.enableHotkey;
            LoadSettings();

            if (oldHotkeyEnabled != g_settings.enableHotkey) {
                if (g_settings.enableHotkey) {
                    RegisterHotKey(hwnd, ID_HOTKEY_GROUP_WINDOW, MOD_WIN, 0x47);
                } else {
                    UnregisterHotKey(hwnd, ID_HOTKEY_GROUP_WINDOW);
                }
            }

            UnregisterAppBar(hwnd);
            RegisterAppBar(hwnd);
            SetAcrylicEffect(hwnd);

            UpdateThumbnails(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
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

    LoadSettings();

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

    int width = g_settings.sidebarWidth;
    if (width < 60) width = 80;
    int left = (g_settings.sidebarEdge == 1) ? (GetSystemMetrics(SM_CXSCREEN) - width) : 0;

    HWND hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"ZenStageManager",
        WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        left, 0, width, GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, wc.hInstance, NULL
    );

    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
    }
    g_hwndSidebar.store(hwnd);

    Wh_Log(L"StageMgr UI: CreateWindowEx = %p", hwnd);
    if (!hwnd) {
        Wh_Log(L"StageMgr UI: CreateWindowEx failed, error=%lu", GetLastError());
    }

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

void Wh_ModSettingsChanged() {
    Wh_Log(L"[StageMgr] Settings changed");
    HWND hwnd = g_hwndSidebar.load();
    if (hwnd) {
        PostMessage(hwnd, WM_STAGE_SETTINGS_CHANGED, 0, 0);
    }
}
