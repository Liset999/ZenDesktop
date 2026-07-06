// ==WindhawkMod==
// @id              zen-explorer-context-menu
// @name            ZenDesktop: Explorer Context Menu Selector
// @name:zh-CN      ZenDesktop：资源管理器右键菜单选择器
// @description     Choose between the default Windows 11 context menu and the full classic context menu.
// @description:zh-CN 可选择使用 Windows 11 默认现代右键菜单，或完整经典右键菜单。
// @version         3.5.1
// @author          m417z, Lanbo
// @github          https://github.com/m417z, https://github.com/Liset999
// @homepage        https://github.com/Liset999/ZenDesktop_OneKeyDeploy
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZenDesktop: Explorer Context Menu Selector

Choose the File Explorer right-click behavior from settings, similar to the
Start Menu visual style selector.

The default mode keeps the Windows 11 modern menu. If you select the full
classic menu, you can hold Ctrl while right-clicking to temporarily show the
modern Windows 11 menu.

Based on m417z's Classic context menu on Windows 11 community mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- contextMenuMode: "Windows11"
  $name: "右键菜单模式 (Context Menu Mode)"
  $description: >-
    选择资源管理器右键菜单行为。默认保持 Windows 11 现代菜单；需要完整菜单时再切换为经典完整菜单。
  $options:
  - Windows11: "Windows 11 默认现代菜单 (Default Modern Menu)"
  - ClassicFull: "经典完整菜单 (Full Classic Menu)"
- overrideWithCtrl: true
  $name: "Ctrl 临时切换 (Ctrl Override)"
  $description: >-
    当菜单模式选择“经典完整菜单”时，按住 Ctrl 再右键可临时显示 Windows 11 现代右键菜单。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shlwapi.h>

enum class ContextMenuMode {
    Windows11,
    ClassicFull,
};

struct {
    ContextMenuMode contextMenuMode;
    bool overrideWithCtrl;
} g_settings;

bool ShouldForceClassicMenu() {
    if (g_settings.contextMenuMode != ContextMenuMode::ClassicFull) {
        return false;
    }

    return !(g_settings.overrideWithCtrl && GetKeyState(VK_CONTROL) < 0);
}

using IUnknown_QueryService_t = decltype(&IUnknown_QueryService);
IUnknown_QueryService_t IUnknown_QueryService_Original;
HRESULT WINAPI IUnknown_QueryService_Hook(IUnknown* punk,
                                          const GUID& guidService,
                                          const IID& riid,
                                          void** ppvOut) {
    // GUIDs from shell32.dll, CDefView::TryGetContextMenuPresenter.

    // {B306C5B1-B4F2-473C-B6FF-701B246CE2D2}
    constexpr GUID guidServiceTarget = {
        0xb306c5b1,
        0xb4f2,
        0x473c,
        {0xb6, 0xff, 0x70, 0x1b, 0x24, 0x6c, 0xe2, 0xd2}};

    // {706461D1-AC5F-4730-BFE3-CAC6CAD5EF5E}
    constexpr GUID riidTarget = {
        0x706461d1,
        0xac5f,
        0x4730,
        {0xbf, 0xe3, 0xca, 0xc6, 0xca, 0xd5, 0xef, 0x5e}};

    // {37A472F7-63CF-4CCF-A88B-5231A3C7D8B6}
    // Changed to this version in update KB5052093 of Windows 11 version 24H2.
    constexpr GUID riidTarget2 = {
        0x37a472f7,
        0x63cf,
        0x4ccf,
        {0xa8, 0x8b, 0x52, 0x31, 0xa3, 0xc7, 0xd8, 0xb6}};

    if (IsEqualGUID(guidService, guidServiceTarget) &&
        (IsEqualGUID(riid, riidTarget) || IsEqualGUID(riid, riidTarget2))) {
        Wh_Log(L">");

        if (ShouldForceClassicMenu()) {
            Wh_Log(L"Disallowing new menu");
            return E_FAIL;
        }
    }

    return IUnknown_QueryService_Original(punk, guidService, riid, ppvOut);
}

using CNscTree_ShouldShowMiniMenu_t = bool(WINAPI*)(void* pThis, void* param1);
CNscTree_ShouldShowMiniMenu_t CNscTree_ShouldShowMiniMenu_Original;
bool WINAPI CNscTree_ShouldShowMiniMenu_Hook(void* pThis, void* param1) {
    Wh_Log(L">");

    if (ShouldForceClassicMenu()) {
        Wh_Log(L"Disallowing new menu");
        return false;
    }

    return CNscTree_ShouldShowMiniMenu_Original(pThis, param1);
}

bool HookExplorerFrameSymbols() {
    HMODULE module = LoadLibrary(L"explorerframe.dll");
    if (!module) {
        Wh_Log(L"Couldn't load explorerframe.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {LR"(private: bool __cdecl CNscTree::ShouldShowMiniMenu(struct _TREEITEM *))"},
            &CNscTree_ShouldShowMiniMenu_Original,
            CNscTree_ShouldShowMiniMenu_Hook,
            true,
        },
    };

    return HookSymbols(module, explorerFrameDllHooks,
                       ARRAYSIZE(explorerFrameDllHooks));
}

void LoadSettings() {
    PCWSTR contextMenuMode = Wh_GetStringSetting(L"contextMenuMode");
    g_settings.contextMenuMode =
        wcscmp(contextMenuMode, L"ClassicFull") == 0
            ? ContextMenuMode::ClassicFull
            : ContextMenuMode::Windows11;
    Wh_FreeStringSetting(contextMenuMode);

    g_settings.overrideWithCtrl = Wh_GetIntSetting(L"overrideWithCtrl");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookExplorerFrameSymbols()) {
        Wh_Log(L"Error hooking explorer frame symbols");
        return FALSE;
    }

    HMODULE shcoreModule = LoadLibrary(L"shcore.dll");
    if (!shcoreModule) {
        Wh_Log(L"Error loading shcore.dll");
        return FALSE;
    }

    IUnknown_QueryService_t pIUnknown_QueryService =
        (IUnknown_QueryService_t)GetProcAddress(shcoreModule,
                                                "IUnknown_QueryService");
    if (!pIUnknown_QueryService) {
        Wh_Log(L"Error getting IUnknown_QueryService");
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(pIUnknown_QueryService,
                                       IUnknown_QueryService_Hook,
                                       &IUnknown_QueryService_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
