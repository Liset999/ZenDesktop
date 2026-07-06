# ZenDesktop Stage Manager for Windows Design Spec

这份设计文档定义了在 Windows 11 上基于 Windhawk C++ 模组实现 macOS 风格“台前调度（Stage Manager）”的核心设计、接口规范与部署整合方案。

## 1. 目标与定位 (Goal & Position)

- **目标**：在 Windows 11 上实现高保真的 macOS 风格台前调度功能。
- **定位**：作为 **ZenDesktop** 桌面美化套件的第 5 个核心 Mod（`local@zen-stage-manager.wh.cpp`），保持 ZenDesktop 一贯的“极低内存开销、零后台进程、极简部署”特色。

## 2. 架构设计 (Architecture Design)

整个 Mod 运行在 `explorer.exe` 进程内。为保障性能和稳定性，采用**双线程模型**：

```
+--------------------------------------------------------+
|                      explorer.exe                      |
|                                                        |
|  +--------------------+      CreateThread      +----+  |
|  | Windhawk 主 Hook  | ---------------------> | UI |  |
|  |     (主线程)       |                        | 线 |  |
|  +--------------------+                        | 程 |  |
|                                                +----+  |
|                                                  |     |
|                                                  v     |
|                                           +----------+ |
|                                           |  AppBar  | |
|                                           | 侧边栏窗 | |
|                                           |    口    | |
|                                           +----------+ |
+--------------------------------------------------------+
```

### 2.1 线程模型与职责
- **主线程 (Hook Thread)**：执行 Windhawk 的初始化钩子，注册配置项。
- **UI 线程 (Sidebar Thread)**：调用 `CreateThread` 异步启动。创建并维护侧边栏窗口句柄（HWND），驱动 Win32 消息循环，并使用 Direct2D 进行 GPU 加速渲染。

### 2.2 窗口追踪与过滤 (Window Tracking & Filtering)
利用系统的 `RegisterShellHookWindow` 监听窗口状态：
- 仅管理具有任务栏显示属性的 Top-Level 窗口。
- **排除列表**：桌面窗体（Progman/WorkerW）、系统任务栏（Shell_TrayWnd）、开始菜单、控制中心面板、UWP 浮窗、已隐藏的窗口、以及 Mod 自身的侧边栏窗口。
- 窗口分组数据结构定义：
  ```cpp
  struct WindowStage {
      std::vector<HWND> hwnds;        // 当前舞台内包含的窗口句柄集合
      std::wstring processName;       // 绑定的主进程名（如 chrome.exe）
      HICON hIcon;                    // 舞台关联的应用图标
      bool isCustomGrouped;           // 是否为用户手动快捷键组合的舞台
  };
  ```

---

## 3. UI 界面与渲染引擎 (UI & Rendering)

### 3.1 侧边栏属性与停靠 (AppBar)
- **样式属性**：`WS_POPUP`（无边框）、`WS_EX_TOOLWINDOW`（不在 Alt+Tab 及任务栏展示）、`WS_EX_TOPMOST`（置顶）。
- **空间挤占 (AppBar)**：使用 `SHAppBarMessage` 注册为侧边栏（默认左侧，宽度 80px）。其他窗口最大化时将自动避开此区域。
- **任务栏避让**：当侧边栏与系统任务栏在同侧时，自动根据任务栏大小产生额外 Margin，防止遮挡任务栏元素。
- **亚克力模糊 (Acrylic)**：通过调用系统未公开 API `SetWindowCompositionAttribute`，直接为侧边栏背景渲染 Windows 11 原生的亚克力模糊玻璃效果。

### 3.2 缩略图映射 (DWM Live Thumbnails)
- 对后台非活跃舞台中的窗口，使用 `DwmRegisterThumbnail` API 获取其实时渲染画面的投影句柄 `HTHUMBNAIL`。
- 调用 `DwmUpdateThumbnailProperties` 设置投影的目的矩形，映射到侧边栏卡片上。
- **多窗口堆叠渲染**：若一个舞台包含多个窗口，在侧边栏底图绘制多重错开的卡片阴影，最顶层渲染该舞台内最近活跃的那个窗口的实时缩略图。

---

## 4. 舞台切换与热键交互 (Switching & Hotkeys)

### 4.1 舞台极速切换
1. 点击侧边栏的某个卡片，或检测到其他舞台的窗口被点击。
2. 禁用目标窗口的原生 Windows 最小化/还原动画，防止画面闪烁与路径冲突：
   ```cpp
   BOOL disable = TRUE;
   DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
   ```
3. 对当前活跃舞台所有窗口执行异步最小化 `ShowWindowAsync(hwnd, SW_MINIMIZE)`。
4. 对点击的目标舞台所有窗口执行异步恢复 `ShowWindowAsync(hwnd, SW_RESTORE)`。
5. 将目标舞台的顶部窗口设为焦点 `SetForegroundWindow(hwnd)`。
6. 恢复原窗口的原生动画配置。

### 4.2 快捷键窗口合并
- 调用 `RegisterHotKey` 监听全局热键（默认 `Win + G`）。
- 按下时，获取当前前台句柄 `GetForegroundWindow()`，将其从原舞台列表中剥离，并强制塞入当前的活跃舞台。

---

## 5. 配置、整合与一键部署 (Configuration & Deployment)

### 5.1 配置项规范
```cpp
// ==WindhawkMod==
// @setting      sidebar_edge     "左侧 (0) 或右侧 (1)"       0
// @setting      acrylic_opacity  "亚克力透明度 (0-255)"    180
// @setting      max_stages       "最多显示卡片数"            5
// @setting      hotkey_vk        "合并热键虚拟键码 (如 'G')"  0x47
// ==/WindhawkMod==
```
配置改变时，调用 `Wh_ModSettingsChanged` 重新分配屏幕空间并刷新 UI，无需重启进程。

### 5.2 部署步骤集成
1. 源码保存为 `d:/Warehouse/ZenDesktop_OneKeyDeploy/local@zen-stage-manager.wh.cpp`。
2. 在 `deploy.bat` 中加入如下指令，确保该 Mod 在一键部署时自动分发至 Windhawk 目录并写入启用状态：
   ```cmd
   copy /y "local@zen-stage-manager.wh.cpp" "%WINDHAWK_SCRIPTS_DIR%\local@zen-stage-manager.wh.cpp"
   reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\windows-stage-manager" /v Disabled /t REG_DWORD /d 0 /f
   ```
3. 在 `ZenDesktopCustomizer.py` 中新加 Tab 控件，绑定注册表配置路径，允许使用滑块实时精细调节透明度与停靠边缘。

---

## 6. 验证计划 (Verification Plan)

### 6.1 手动验证测试用例
- **用例 1（停靠与避让）**：启用 Mod，观察屏幕左侧是否被占用，最大化任意窗口检查是否完美避开侧边栏。将 Windows 任务栏移至左侧，检查侧边栏是否发生智能偏移。
- **用例 2（自动分组）**：打开 3 个记事本和 2 个资源管理器，观察侧边栏是否自动显示两个卡片组，并且各组在 hover 时正确显示数量与应用名。
- **用例 3（极速切换）**：点击卡片进行切换，检查旧窗口是否迅速最小化，新窗口是否迅速恢复到前台，检查有无闪烁。
- **用例 4（热键合并）**：在活跃的记事本窗口下按下 `Win + G`，检查该记事本窗口是否和另一组的窗口成功合并，形成一个新的混合舞台。
