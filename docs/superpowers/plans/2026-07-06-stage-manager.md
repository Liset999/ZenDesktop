# ZenDesktop Stage Manager for Windows Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a macOS Stage Manager replica on Windows 11 as a C++ Windhawk Mod, integrated into the ZenDesktop deployment script and Python GUI customizer.

**Architecture:** The mod runs inside `explorer.exe`, spawns an asynchronous UI thread that registers an AppBar sidebar, intercepts window events via `RegisterShellHookWindow`, renders live previews via DWM Thumbnail APIs, and toggles window visibility to group active workflows.

**Tech Stack:** C++20, Win32 API, DWM API, Direct2D, Python (CustomTkinter), Batch scripting.

## Global Constraints
- Target platform: Windows 11 (build 22000+)
- Mod filename: `local@zen-stage-manager.wh.cpp`
- ZenDesktop Customizer Python script: `ZenDesktopCustomizer.py`
- Deployment script: `deploy.bat`
- Memory overhead: < 10MB
- Background processes: 0 (lives in `explorer.exe` process)

---

### Task 1: Scaffolding and Mod Boilerplate

**Files:**
- Create: `d:/Warehouse/ZenDesktop_OneKeyDeploy/local@zen-stage-manager.wh.cpp`

**Interfaces:**
- Consumes: None
- Produces: `Wh_ModInit` (Mod entry), `Wh_ModUninit` (Mod exit), `UIThreadProc` (UI thread handler)

- [ ] **Step 1: Write the Windhawk Mod boilerplate**
  Write the Mod file containing Windhawk metadata header, basic standard includes, `Wh_ModInit`, `Wh_ModUninit`, and a background thread `UIThreadProc` that registers a basic window class and starts a Win32 message loop.

  ```cpp
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
  ```

- [ ] **Step 2: Verify the template structure**
  Verify that the code contains no syntactic errors and compiles cleanly using the C++ compiler. Note that Windhawk compiling will be verified in deployment.
  
- [ ] **Step 3: Commit**
  ```bash
  git add local@zen-stage-manager.wh.cpp
  git commit -m "feat: scaffolding for stage manager mod"
  ```

---

### Task 2: AppBar Side Docking and composition settings

**Files:**
- Modify: `d:/Warehouse/ZenDesktop_OneKeyDeploy/local@zen-stage-manager.wh.cpp`

**Interfaces:**
- Consumes: `UIThreadProc` from Task 1
- Produces: `RegisterAppBar`, `UnregisterAppBar`, Acrylic blur background rendering

- [ ] **Step 1: Add undocumented Acrylic APIs and AppBar helpers**
  Update the code with helper functions to register the window as an AppBar and query/set position, adjusting for taskbar margins. Implement Undocumented `SetWindowCompositionAttribute` for Acrylic glass styling.

  ```cpp
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

  void RegisterAppBar(HWND hwnd) {
      APPBARDATA abd = { sizeof(abd) };
      abd.hWnd = hwnd;
      abd.uCallbackMessage = WM_APPBAR_CALLBACK;
      SHAppBarMessage(ABM_NEW, &abd);

      abd.uEdge = ABE_LEFT; // Default left docking
      abd.rc.left = 0;
      abd.rc.top = 0;
      abd.rc.right = 80;
      abd.rc.bottom = GetSystemMetrics(SM_CYSCREEN);

      SHAppBarMessage(ABM_QUERYPOS, &abd);
      SHAppBarMessage(ABM_SETPOS, &abd);
  }

  void UnregisterAppBar(HWND hwnd) {
      APPBARDATA abd = { sizeof(abd) };
      abd.hWnd = hwnd;
      SHAppBarMessage(ABM_REMOVE, &abd);
  }
  ```

- [ ] **Step 2: Integrate into WndProc**
  Modify `SidebarWndProc` to trigger `RegisterAppBar` and `SetAcrylicEffect` on `WM_CREATE`, and call `UnregisterAppBar` on `WM_DESTROY`.

- [ ] **Step 3: Commit**
  ```bash
  git commit -am "feat: add appbar registration and acrylic composition background"
  ```

---

### Task 3: Window Tracking and Stage Filtering

**Files:**
- Modify: `d:/Warehouse/ZenDesktop_OneKeyDeploy/local@zen-stage-manager.wh.cpp`

**Interfaces:**
- Consumes: Windows Shell Hook Messages
- Produces: `g_stages` structure, `IsAppWindow` filter function, ShellHook listener

- [ ] **Step 1: Add filtering logic and structures**
  Define `WindowStage` and window grouping vector. Implement `IsAppWindow` to filter out windows that shouldn't appear in the sidebar (tooltips, invisible windows, desktop elements, explorer taskbar).

  ```cpp
  struct WindowStage {
      std::vector<HWND> hwnds;
      std::wstring processName;
      HICON hIcon;
      bool isCustom;
  };
  std::vector<WindowStage> g_stages;

  bool IsAppWindow(HWND hwnd) {
      if (!IsWindowVisible(hwnd)) return false;
      
      // Exclude tool windows and windows without WS_EX_APPWINDOW
      LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
      if (exStyle & WS_EX_TOOLWINDOW) return false;
      
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
  ```

- [ ] **Step 2: Add Shell Hook registration and messages**
  Hook `RegisterShellHookWindow` on `WM_CREATE`. Process `WM_SHELLHOOKMESSAGE` in `SidebarWndProc` to track `HSHELL_WINDOWCREATED`, `HSHELL_WINDOWDESTROYED` and `HSHELL_WINDOWACTIVATED` events, calling update functions to maintain stages list.

- [ ] **Step 3: Commit**
  ```bash
  git commit -am "feat: add window filtering rules and register global shell hook window"
  ```

---

### Task 4: DWM Thumbnail Previews and Rendering

**Files:**
- Modify: `d:/Warehouse/ZenDesktop_OneKeyDeploy/local@zen-stage-manager.wh.cpp`

**Interfaces:**
- Consumes: `g_stages`
- Produces: `UpdateThumbnails` (DWM registration & layouts), `DrawSidebar` (D2D render)

- [ ] **Step 1: Implement DWM Thumbnail registration and layout**
  Write logic to calculate thumbnail render targets inside the sidebar client area. Loop through the top 5 stages, register each window via `DwmRegisterThumbnail` and position them stacked together inside card bounds.

  ```cpp
  std::vector<HTHUMBNAIL> g_activeThumbnails;

  void ClearThumbnails() {
      for (HTHUMBNAIL hThumb : g_activeThumbnails) {
          DwmUnregisterThumbnail(hThumb);
      }
      g_activeThumbnails.clear();
  }

  void UpdateThumbnails(HWND hwndSidebar) {
      ClearThumbnails();
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
              dpr.rcDestination = RECT{ 10, yOffset, 70, yOffset + 50 }; // 60x50 card bounds
              DwmUpdateThumbnailProperties(hThumb, &dpr);
              g_activeThumbnails.push_back(hThumb);
          }
          yOffset += 80;
      }
  }
  ```

- [ ] **Step 2: Add Direct2D card outlines and rendering**
  Implement Direct2D render calls in `WM_PAINT` to draw card frames, window stacking shadows, and application icons.

- [ ] **Step 3: Commit**
  ```bash
  git commit -am "feat: add DWM Live Thumbnail registration and Direct2D layout rendering"
  ```

---

### Task 5: Stage Switching and Global Hotkey Grouping

**Files:**
- Modify: `d:/Warehouse/ZenDesktop_OneKeyDeploy/local@zen-stage-manager.wh.cpp`

**Interfaces:**
- Consumes: Keyboard events, mouse click events on sidebar
- Produces: `SwitchToStage`, `RegisterHotKey` listener

- [ ] **Step 1: Implement SwitchToStage logic**
  Implement target activation logic. Temporarily disable system minimize animations on the source/destination windows, call `ShowWindowAsync(hwnd, SW_MINIMIZE)` for active stage windows, and call `ShowWindowAsync(hwnd, SW_RESTORE)` / `SetForegroundWindow(hwnd)` for target stage.

  ```cpp
  int g_activeStage = -1;

  void SwitchToStage(int targetIndex) {
      if (targetIndex < 0 || targetIndex >= (int)g_stages.size()) return;
      
      BOOL disableAnim = TRUE;
      BOOL enableAnim = FALSE;

      // Minimize current active stage windows without Win32 transitions
      if (g_activeStage >= 0 && g_activeStage < (int)g_stages.size()) {
          for (HWND hwnd : g_stages[g_activeStage].hwnds) {
              DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disableAnim, sizeof(disableAnim));
              ShowWindowAsync(hwnd, SW_MINIMIZE);
              DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &enableAnim, sizeof(enableAnim));
          }
      }

      // Restore target stage windows
      for (HWND hwnd : g_stages[targetIndex].hwnds) {
          DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disableAnim, sizeof(disableAnim));
          ShowWindowAsync(hwnd, SW_RESTORE);
          DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &enableAnim, sizeof(enableAnim));
      }
      
      if (!g_stages[targetIndex].hwnds.empty()) {
          SetForegroundWindow(g_stages[targetIndex].hwnds.back());
      }
      
      g_activeStage = targetIndex;
  }
  ```

- [ ] **Step 2: Wire up hotkey (Win+G) and mouse click handlers**
  Register hotkey `Win + G` on `WM_CREATE`. In WndProc `WM_HOTKEY` handler, get `GetForegroundWindow()`, find its current stage, and move it to the active stage. Add mouse hit-testing in `WM_LBUTTONDOWN` to check which card is clicked and trigger `SwitchToStage`.

- [ ] **Step 3: Commit**
  ```bash
  git commit -am "feat: implement stage switching and register Win+G grouping hotkey"
  ```

---

### Task 6: Deploy integration and Python Customizer

**Files:**
- Modify: `d:/Warehouse/ZenDesktop_OneKeyDeploy/deploy.bat`
- Modify: `d:/Warehouse/ZenDesktop_OneKeyDeploy/ZenDesktopCustomizer.py`

**Interfaces:**
- Consumes: Mod properties
- Produces: Automated deployment registry setup, Customizer UI Control Tab

- [ ] **Step 1: Update deploy.bat**
  Add instructions to copy `local@zen-stage-manager.wh.cpp` to the Windhawk Scripts folder and write a registry entry activating it.
  
  ```cmd
  :: 在 deploy.bat 适当位置加入:
  copy /y "local@zen-stage-manager.wh.cpp" "%WINDHAWK_SCRIPTS_DIR%\local@zen-stage-manager.wh.cpp"
  reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-stage-manager" /v Disabled /t REG_DWORD /d 0 /f
  ```

- [ ] **Step 2: Update ZenDesktopCustomizer.py**
  Add a Tab panel named "台前调度" to ZenDesktopCustomizer. It should read/write settings like `sidebar_edge` and `acrylic_opacity` under registry path `HKCU\Software\ZenDesktop\StageManager`.

- [ ] **Step 3: Commit**
  ```bash
  git commit -am "feat: integrate stage manager into deploy.bat and Python customizer GUI"
  ```
