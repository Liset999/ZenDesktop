// One mod ID, two roles. Shell widgets stay in Explorer; upstream audio lives
// in its own Windhawk tool process and never initializes the shell hooks there.
bool g_zwShellRole = false;
HANDLE g_zwAudioLauncherThread = nullptr;
HANDLE g_zwAudioLauncherStop = nullptr;
HANDLE g_zwAudioLauncherWake = nullptr;

DWORD WINAPI ZwAudioLauncher(void*) {
    HANDLE waits[] = {g_zwAudioLauncherStop, g_zwAudioLauncherWake};
    ULONGLONG lastLaunchTick = 0;
    while (WaitForSingleObject(g_zwAudioLauncherStop, 0) != WAIT_OBJECT_0) {
        bool enabled = ZwAudioWanted();
        if (enabled) {
            HANDLE running = OpenMutexW(SYNCHRONIZE, FALSE, L"windhawk-tool-mod_" WH_MOD_ID);
            if (running) CloseHandle(running);
            else if (GetLastError() == ERROR_FILE_NOT_FOUND &&
                     (!lastLaunchTick || GetTickCount64() - lastLaunchTick >= 2000)) {
                ZenAudio::Wh_ModAfterInit();
                lastLaunchTick = GetTickCount64();
            }
        }
        if (WaitForMultipleObjects(2, waits, FALSE, enabled ? 1000 : INFINITE) == WAIT_OBJECT_0) break;
    }
    return 0;
}

void ZwStartAudioLauncher() {
    if (g_zwAudioLauncherThread) { SetEvent(g_zwAudioLauncherWake); return; }
    g_zwAudioLauncherStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_zwAudioLauncherWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_zwAudioLauncherStop && g_zwAudioLauncherWake)
        g_zwAudioLauncherThread = CreateThread(nullptr, 0, ZwAudioLauncher, nullptr, 0, nullptr);
    if (!g_zwAudioLauncherThread) {
        if (g_zwAudioLauncherStop) CloseHandle(g_zwAudioLauncherStop);
        if (g_zwAudioLauncherWake) CloseHandle(g_zwAudioLauncherWake);
        g_zwAudioLauncherStop = g_zwAudioLauncherWake = nullptr;
        Wh_Log(L"Audio launcher initialization failed");
    }
}

void ZwStopAudioLauncher() {
    if (g_zwAudioLauncherThread) {
        SetEvent(g_zwAudioLauncherStop);
        WaitForSingleObject(g_zwAudioLauncherThread, INFINITE);
        CloseHandle(g_zwAudioLauncherThread);
        CloseHandle(g_zwAudioLauncherStop);
        CloseHandle(g_zwAudioLauncherWake);
    }
    g_zwAudioLauncherThread = g_zwAudioLauncherStop = g_zwAudioLauncherWake = nullptr;
}

bool ZwIsShellProcess() {
    WCHAR path[MAX_PATH]{};
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return false;
    PCWSTR file = wcsrchr(path, L'\\');
    return !_wcsicmp(file ? file + 1 : path, L"explorer.exe");
}

BOOL Wh_ModInit() {
    g_zwShellRole = ZwIsShellProcess();
    Wh_Log(L"[widgets] init shell=%d bits=%u audio=%d", g_zwShellRole,
           (unsigned)(sizeof(void*) * 8), ZwAudioWanted());
    if (g_zwShellRole) {
#ifdef _WIN64
        return ZcModInit();
#else
        return FALSE;  // Shell widgets are supported only by 64-bit Explorer.
#endif
    }
    return ZenAudio::Wh_ModInit();
}

void Wh_ModAfterInit() {
    if (g_zwShellRole) {
#ifdef _WIN64
        ZcModAfterInit();
#endif
    }
    else if (ZenAudio::g_isToolModProcessLauncher) ZwStartAudioLauncher();
    else if (ZwAudioWanted()) ZenAudio::Wh_ModAfterInit();
}

void Wh_ModSettingsChanged() {
    if (g_zwShellRole) {
#ifdef _WIN64
        ZcModSettingsChanged();
#endif
    } else if (ZenAudio::g_isToolModProcessLauncher) {
        // The launcher remains loaded even when audio is initially disabled.
        // Enabling from this same settings page can therefore start the tool.
        ZwStartAudioLauncher();
    } else if (!ZwAudioWanted()) {
        ZenAudio::Wh_ModUninit();
    } else {
        ZenAudio::Wh_ModSettingsChanged();
    }
}

void Wh_ModUninit() {
    if (g_zwShellRole) {
#ifdef _WIN64
        ZcModUninit();
#endif
    }
    else {
        if (ZenAudio::g_isToolModProcessLauncher) ZwStopAudioLauncher();
        ZenAudio::Wh_ModUninit();
    }
}
