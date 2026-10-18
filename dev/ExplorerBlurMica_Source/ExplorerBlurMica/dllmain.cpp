// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "framework.h"
#include "ShellLoader.h"
#include "HookDef.h"

HMODULE g_hModule = nullptr;
bool m_isLoaded = false;      //explorer加载标志
bool m_isInitHook = false;    //Hook初始化标志
ULONG_PTR m_gdiplusToken = 0; //GDI初始化标志
int g_sysBuildNumber = 22000; //系统build版本号
bool g_colorMode = true;      //系统颜色模式 是否为Light

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH && !g_hModule) 
    {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);

        if (GetModuleHandleW(L"SettingSyncHost.exe")
            || GetModuleHandleW(L"SkyDrive.exe")
            || GetModuleHandleW(L"FileManager.exe")
            || GetModuleHandleW(L"vmtoolsd.exe")
            || GetModuleHandleW(L"svchost.exe")
            || GetModuleHandleW(L"SearchIndexer.exe")
            || GetModuleHandleW(L"WSHost.exe")
            || GetModuleHandleW(L"wmpnetwk.exe")
            || GetModuleHandleW(L"svchost.exe")
            || GetModuleHandleW(L"dllhost.exe")
            || GetModuleHandleW(L"spoolsv.exe"))
        {
            //FreeLibraryAndExitThread(hModule, 0);
            return FALSE;
        }

        //防止别的程序意外加载
        wchar_t pName[MAX_PATH];
        GetModuleFileNameW(NULL, pName, MAX_PATH);
        std::wstring path = std::wstring(pName);
        // ZenDesktop: guard substr (short/failed path would throw in DllMain -> abort)
        std::wstring name = path.length() >= 12 ? path.substr(path.length() - 12, 12) : path;
        if (ConvertTolower(name) == L"explorer.exe")
            m_isLoaded = true;
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        if (m_isInitHook)
            Hook::HookDetachAll();
        if (m_gdiplusToken)
            Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }
    return TRUE;
}

void OnWindowLoad()
{
    //如果按住ESC键则不加载扩展
    // ZenDesktop: runs on the delay thread now, GetKeyState would not see the key
    if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) || !m_isLoaded)
        return;

    if (!m_gdiplusToken)
    {
        //初始化 Gdiplus Init GdiPlus
        Gdiplus::GdiplusStartupInput StartupInput;
        int ret = Gdiplus::GdiplusStartup(&m_gdiplusToken, &StartupInput, nullptr);
    }

    //在开机的时候系统就会加载此动态库 那时候启用HOOK是会失败的 等创建窗口的时候再初始化HOOK
    if (!m_isInitHook)
    {
        m_isInitHook = true;

        //获取系统版本号
        HKEY tkey = nullptr;
        if (ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", 0, KEY_READ, &tkey))
        {
            wchar_t dwValue[MAX_PATH] = { 0 };
            DWORD dwSzType = REG_SZ;
            DWORD dwSize = (MAX_PATH - 1) * sizeof(wchar_t); // ZenDesktop: bytes, keep a terminator
            RegQueryValueExW(tkey, L"CurrentBuildNumber", nullptr, &dwSzType, (LPBYTE)&dwValue, &dwSize);
            dwValue[MAX_PATH - 1] = 0;
            int buildNumber = _wtoi(dwValue);
            if (buildNumber != 0)
                g_sysBuildNumber = buildNumber;
            RegCloseKey(tkey);
        }

        //创建钩子 CreateHook
        // MH_ERROR_ALREADY_INITIALIZED is OK: this copy of MinHook may
        // already have been initialized on a previous window. Never
        // MessageBox / FreeLibraryAndExitThread inside explorer.exe —
        // that is what restarted Explorer on the first cabinet window
        // after boot.
        MH_STATUS mh = MH_Initialize();
        if (mh == MH_OK || mh == MH_ERROR_ALREADY_INITIALIZED)
        {
            // ZenDesktop: once hooks point into this DLL it must never be
            // unloaded (COM may call DllCanUnloadNow when the last cabinet
            // window closes -> explorer would jump into unmapped code).
            HMODULE pinned = nullptr;
            GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                (LPCWSTR)&OnWindowLoad, &pinned);
            Hook::HookAttachAll();
            MH_EnableHook(MH_ALL_HOOKS);
        }
        else
        {
            m_isInitHook = false;
            return;
        }
    }

    RefreshConfig();
}

static HANDLE volatile g_delayHookThread = nullptr;

// ZenDesktop: config.ini [config] delayHookMs (default 800, 0..10000).
static DWORD GetDelayHookMs()
{
    std::wstring v = GetIniString(GetCurDllDir() + L"\\config.ini", L"config", L"delayHookMs");
    if (v.empty())
        return 800;
    int ms = _wtoi(v.c_str());
    if (ms < 0) ms = 0;
    if (ms > 10000) ms = 10000;
    return (DWORD)ms;
}

DWORD WINAPI DelayHookThreadProc(LPVOID)
{
    Sleep(GetDelayHookMs());
    OnWindowLoad();
    // ZenDesktop: allow a retry from the next window if hooking failed.
    HANDLE self = InterlockedExchangePointer(&g_delayHookThread, nullptr);
    if (self)
        CloseHandle(self);
    // Drop the reference taken in ScheduleOnWindowLoad (no-op once pinned).
    FreeLibraryAndExitThread(g_hModule, 0);
    return 0;
}

void ScheduleOnWindowLoad()
{
    if (m_isInitHook)
    {
        // ZenDesktop: hooks already up -> just pick up config.ini changes for
        // this new window (the old constructor path did this via OnWindowLoad).
        RefreshConfig();
        return;
    }
    if (g_delayHookThread)
        return;
    // ZenDesktop: hold a module reference while the delay thread sleeps,
    // otherwise COM can unload the DLL under it (DllCanUnloadNow == S_OK).
    HMODULE self = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            (LPCWSTR)&DelayHookThreadProc, &self))
        return;
    // Create suspended so the handle is published before the thread can clear it.
    HANDLE th = CreateThread(nullptr, 0, DelayHookThreadProc, nullptr, CREATE_SUSPENDED, nullptr);
    if (!th)
    {
        FreeLibrary(self);
        return;
    }
    InterlockedExchangePointer(&g_delayHookThread, th);
    ResumeThread(th);
}
