# ZenDesktop - 开发者备忘录与技术沉淀

> 注：`ExplorerBlurMica/`、`ExplorerBlurMica_Source/` 目录已从仓库删除，下文提到它们的内容仅作历史记录；旧版卸载用 `tools/uninstall-ExplorerBlurMica.bat`。

> **v4.2 策略更新（2026-10）**：文件管理器透明以 Windhawk mod `local@zen-explorer-styler` 为准，**不再推荐用户安装或注册 ExplorerBlurMica**。若用户机器上仍注册着 EBM，应引导其运行 `tools/uninstall-ExplorerBlurMica.bat` 卸载后再使用 styler。下文中大量 EBM / clearWinUIBg / 黑杠排查记录保留作历史，不要再当作当前产品建议写进 README 或发布文案。`deploy.ps1` 不得重新注册 EBM。

这份文档记录了 ZenDesktop Mod 的架构解析、各个核心文件的作用，以及在开发过程中特别是针对 Windows API 及 Windhawk 环境踩过的坑和技术解决方案。

## 0. 当前接手状态（2026-08-20，读完这一节就能接着干）

**产品对外仍是 v4.0.0 套件；任务栏源码已升到 v4.1.1。用户自己 deploy。**

**2026-08-21 转向**：文件管理器透明停手，继续用 `D:\Download\show\文件管理器透明`。设置页 **5.4.3**：选中「主页」渐变是 HostBackdrop 用在小控件上。没有其它开源工具能透 Settings 内容。编译 5.4.3 后关开设置。

### 用户原话（三件事）
1. 装了 ExplorerBlurMica 之后，**每次开机第一次打开文件管理器会重启 Explorer**，再点一次才能开。
2. 每次开机任务栏 mod「不生效」：条子透明，**托盘 / 音量图标是 Win 原生实底**。用户确认：**在 Windhawk 里把任务栏 mod 关开一次就好**（不是重启 Explorer）。
3. 要把最大化层重新并进任务栏 mod（参考 `D:\Download\ZenDesktop_OneKeyDeploy_v4.0.0_Online-32054bd01e29\local@zen-taskbar-acrylic.wh.cpp` v3.5.0）。旧实现连续最大化/最小化会崩 Explorer。代码风格跟 [ponytail](https://github.com/DietrichGebert/ponytail)：先读根因，复用现成，最少 diff。

### 已核验的根因
| 症状 | 根因 |
|---|---|
| 开机托盘/音量原生，开关 mod 立刻好 | TAP 只注入一次（`g_initialized.exchange(true)`）。开机时托盘 XAML island 还没建完；关开一次等于 `UninitializeSettingsAndTap` + 再 `InitializeSettingsAndTap`，岛已经在了。开始菜单早就有 `TaskbarCreated` 重试，任务栏没有。音量图标在 explorer 托盘上，所以只开任务栏 mod 就能修好，不必动通知中心。 |
| 开机第一次开文件管理器重启 Explorer | ExplorerBlurMica 是 COM BHO。`CObjectWithSite` 构造函数里立刻 `OnWindowLoad()` → `MH_Initialize` + hook。第一个 `CabinetWClass` 还在创建。失败路径弹 `MessageBox` 或 `FreeLibraryAndExitThread`，Explorer 被干掉。官方 README 自己也写「Reopen the Explorer window」。 |
| v3.5.0 连续 max/min 崩 | WinEvent `EVENT_OBJECT_LOCATIONCHANGE` 回调里对 Explorer UI 线程 `SendMessage`（`RunFromWindowThread`）。笔记 2.35–2.39。涂色必须走 `ZenTaskbarRuntimeBgBrush` 局部刷新，禁止拆 TAP。Win32 `SetWindowCompositionAttribute` 在 Win11 XAML 任务栏上会涂成黑底（2.23–2.29），helper 的涂色不能搬过来，只能搬它的独立线程检测。 |

### 2026-10-15 开机托盘不生效 + 文件管理器打不开（4.1.2，未在 Windows 实测）
* **主嫌疑：XAML 诊断只有一个位置。** 任务栏 mod 用 `InitializeXamlDiagnosticsEx` 注 TAP；包里现在的 427KB ExplorerBlurMica 2.0.x 也调它（不在导入表，是 GetProcAddress 动态取的；字符串里有 `InitializeXamlDiagnosticsEx`、`MBox::InitializeUWPXamlDiagnostics`、`xamldiagnostics.cpp`，UWP 和 WinUI 两套都挂）。第一次开文件管理器时 EBM 抢走位置，之后新出来的托盘图标就没人涂 → 托盘原生；关开 mod = 重新抢回来。上游：windhawk-mods#1262、#3720（2.0.1 才开始冲突，1.0.7 没有）。
* **第二嫌疑：Alert 弹窗卡住文件管理器。** 我们的 fork 默认 `xamlDiagnosticsHandling: alert`，`MessageBox` 弹在 EBM 调用的线程上，也就是第一个 `CabinetWClass` 的线程 → 窗口出不来。4.1.2 默认改 `allow`，deploy 把空值/alert 改成 allow。
* EBM 自己也有「开机第一次打开就崩」：Maplespe/ExplorerBlurMica#209（24H2，ucrtbase `0xc0000409`，未修）、#173。
* 4.1.2：开机重试 2.5s → 8s → 20s（不拆 TAP，只补没初始化的线程）；`[ZenDiag]` 日志记录谁在抢 XAML 诊断。
* 可选兜底：`deploy.ps1 -StartupCheck check|restart|remove`，计划任务 `\ZenDesktop\StartupCheck`，每次开机最多重启一次 explorer，日志 `%LOCALAPPDATA%\ZenDesktop\startup-check.log`。
* 二分步骤见 `TEST_v4.2.md` 第 5 轮。
* **更正**：仓库 `ExplorerBlurMica_Source` 是 1.0 系，**全源码没有一处调用 `InitializeXamlDiagnosticsEx`**（也没有任何 XAML/WinUI 代码）。抢 XAML 诊断的只有仓库里那个 427KB 的 2.0.1 二进制（版本资源 2.0.1.0，`config.ini` 的 `clearWinUIBg` 就是它的 WinUI 透明功能），没有源码，改不了。所以源码版 = 不冲突，代价是 WinUI 文件列表不透明。
* 源码版（1.0.7.0）加固（dllmain.cpp）：ESC 改 `GetAsyncKeyState`（延迟线程上 GetKeyState 看不到）；已 hook 时新窗口直接 `RefreshConfig()`；hook 失败后清掉线程句柄允许下个窗口重试；hook 成功后 PIN 住 DLL；延迟线程持有模块引用；`delayHookMs` 可配（默认 800）；DllMain 里 `substr` 越界保护；`CurrentBuildNumber` 读取补结尾 0。Actions 现在也在 `v4.2-draft` push 时编。

### 这一轮已经改了的文件
* `local@zen-taskbar-acrylic.wh.cpp` **4.0.0 → 4.1.1**
  * 开机 2.5s 后自动走一遍和「关开 mod」相同的 `ReapplyTaskbarStyles(false)`
  * 复用开始菜单的 `TaskbarCreated` 隐藏窗口（Explorer 崩完重拉也要重挂 TAP）
  * 最大化层：v3.5.0 的 RuntimeBgBrush + helper 的独立 WinEvent 线程 + `SendMessageTimeout(SMTO_ABORTIFHUNG, 200ms)`，不再从 WinEvent `SendMessage`
* `deploy.ps1`：部署时把 `local@taskbar-background-helper` 写成 `Disabled=1`，避免两套 WinEvent 叠在 explorer 上（那会黑底）
* `ExplorerBlurMica_Source/`：构造函数不再 hook；`SetSite` 后 800ms 再 `OnWindowLoad`；`MH_ERROR_ALREADY_INITIALIZED` 当成功；禁止 MessageBox / 杀线程
* **GitHub Actions 已编过（2026-08-20）**：commit `a63cc5e`，run https://github.com/Liset999/ZenDesktop/actions/runs/32387477393 `success`。用户下载 zip 后已覆盖 `ExplorerBlurMica/ExplorerBlurMica.dll`（217088 bytes，SHA256 `D527ED4E6A1877D4EE8F60CDFDA1DBF4D810B479B8E5D77EB398F6531DF31B27`）。BHO 实际写在 Maplespe GUID `{B44BD3C8-...}`。
* **主列表发白、侧栏能透（2026-08-21）**：用户 `config.ini` 已是 `effect=3`、亮色 `a=0`，不是参数不够。Win11 24H2 文件列表/命令栏是 WinUI；仓库 `ExplorerBlurMica_Source` 是 1.0 系，GitHub 编出的 217KB DLL 清不掉这块。v4.0.0 包里的 **427KB** DLL（Maplespe 2.0.x / 23H2 WinUI3）才是以前「看起来全透」的那份。已把 427KB 拷回 `ExplorerBlurMica.dll`，217KB 延迟 hook 版备份为 `ExplorerBlurMica.dll.delayhook-217k`。换 DLL 后需重新 `register.cmd`。标题栏失焦变实色仍是系统限制。

### 新对话接手清单（按顺序）
1. 任务栏：Windhawk 源代码页打开 `local@zen-taskbar-acrylic`，确认版本 **4.1.1**，保存并编译，重启 Explorer。
2. 验证开机：重启 Windows（不要只重启 Explorer）。托盘/音量应在约 2.5 秒内自己变透明，不必再手动关开。
3. 验证最大化：连续放大/缩小窗口，任务栏应变厚且 Explorer 不崩、Toolkit 不报不稳定。若 helper 仍在 Windhawk 里启用，先关掉。
4. ExplorerBlurMica：用 VS 打开 `ExplorerBlurMica_Source/ExplorerBlurMica.sln` 编 x64 Release，把 DLL 拷到 `ExplorerBlurMica/`，管理员跑 `register.cmd`，再测「开机第一次打开文件管理器」。
5. 未核验：实机开机、连续 max/min、新 DLL。不要在没编译任务栏 4.1.1 之前判断视觉结果（笔记 2.61：源码同步 ≠ 已加载 DLL）。

### 不要再做的事
* **绝对不要让 deploy.bat / deploy.ps1 负责编译或开启 Mod！** deploy 仅负责把 mod 源码和元数据装到 Windhawk 里面，所有 Mod 首次导入一律默认关闭（`Disabled=1`）。由用户在 Windhawk UI 中自行按需开启并确认编译。
* 不要把文件管理器再做成 Windhawk XAML Styler / TP 自研引擎（2.41–2.52 已经走穿过）
* 不要在 WinEvent 上 `UninitializeSettingsAndTap`
* 不要给 Win11 任务栏套 helper 的 Win32 accent
* 不要对 Settings 用 `ACCENT_POLICY` / `DwmExtendFrameIntoClientArea(-1)`（冻输入）
* 代码风格继续 ponytail：复用开始菜单 TaskbarCreated 和 helper 线程，不新写第三套检测


---


## 1. 核心文件解析

当前目录下的文件构成了 ZenDesktop 美化套件的核心：

### Windhawk Mods (Core Injection Code)
*   **`local@zen-taskbar-acrylic.wh.cpp`**
    *   **Role**: Taskbar transparency and custom visual styling mod. v4.1.0 起内置最大化状态层 + 开机 TAP 延迟重试。
    *   **Tech**: Injected into `explorer.exe` taskbar component. Overrides `TrayNotifyWnd` / `ReBarWindow32` default background rendering to apply acrylic/blur effects.
    *   **Current version**: v4.1.4
*   **`local@zen-notificationcenter-acrylic.wh.cpp`**
    *   **Role**: Notification Center transparency and style injection mod.
    *   **Tech**: Injected into the notification center process (`ShellExperienceHost.exe` / `ShellHost.exe`) to make backgrounds transparent and remove unwanted borders/shadows.
    *   **Current version**: v2.8.0
*   **`local@zen-startmenu-acrylic.wh.cpp`**
    *   **Role**: Start Menu pure transparency and advanced acrylic rendering mod.
    *   **Tech**: Injected into `StartMenuExperienceHost.exe`. Hooks system draw APIs and applies DWM (Desktop Window Manager) background effects, removing the default background brush.
    *   **Current version**: v2.7.0
*   **`local@zen-desktop-toggle-icons.wh.cpp`**
    *   **Role**: Double-click to hide/show desktop icons; system-level global input detection for auto-hide and auto-restore (Issue #2).
    *   **Tech**: Subclasses `SHELLDLL_DefView` and `SysListView32`. Tracks system-wide activity via `GetLastInputInfo()`, sends `0x7402` (WM_COMMAND) to toggle. `g_autoHiddenAt` distinguishes manual vs. auto-hide.
    *   **Current version**: v3.1.0

### 部署与工具脚本
*   **`deploy.bat`**
    *   **作用**：**一键部署脚本**。它是用户的入口，负责检查管理员权限，将 Mod 源码复制到 Windhawk 的 `Data\\Scripts` 目录，并通过注册表 (`HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods`) 自动将 Mod 状态设置为 `Disabled=0`，最后唤起 Windhawk 并重启资源管理器。
*   **`restart_explorer.bat`**
    *   **作用**：用于安全平滑地重启 Windows 资源管理器（先结束进程，再重新启动）。
*   **`create_nc_mod.py`** / **`overwrite_nc_mod.py`** / **`patch_date_and_line.py`**
    *   **作用**：开发辅助构建脚本，主要用于自动化替换 C++ 源码中的特定变量、修复时间戳和快速打补丁更新代码逻辑。

---

## 2. 技术踩坑与经验总结

在开发和完善 `zen-desktop-toggle-icons` 及部署脚本的过程中，我们总结了以下至关重要的经验教训：

### 2.1 UI 线程与定时器 (Timer) 的安全限制
*   **问题场景**：在桌面图标的悬停和自动淡出功能中，我们需要使用 `SetTimer` 定时检查鼠标位置。最初在 Windhawk 的初始化回调（Helper 线程）中直接调用了 `SetTimer`，导致 UI 的消息循环被阻塞，甚至引发资源管理器卡死且定时器不生效。
*   **经验沉淀**：**永远不要在非 UI 线程中为 UI 窗口创建定时器或处理核心界面消息。** `SetTimer` 必须在目标窗口所属的线程中被调用。
*   **解决方案**：采用消息投递模式。我们定义了自定义消息 `WM_REFRESH_TIMER` (`WM_USER + 5001`)，在 Windhawk 初始化时使用 `PostMessage` 将指令发给桌面视图窗口，当窗口在其自身的 UI 线程中处理该消息时，再去安全地执行 `SetTimer`。

### 2.2 同步与异步消息的死锁陷阱
*   **问题场景**：为了切换桌面图标的显示/隐藏状态，需要向系统发送 `WM_COMMAND, 0x7402` 消息。如果使用同步函数 `SendMessageW`，有时系统会因为内部锁的问题导致 `explorer.exe` 发生死锁冻结。
*   **经验沉淀**：跨进程或针对系统保留底层视图（如 `SHELLDLL_DefView`）发送状态变更指令时，存在很高的死锁风险。
*   **解决方案**：将 `SendMessageW` 全部替换为异步的 **`PostMessageW`**。将指令丢入消息队列后立即返回，由 UI 线程自行排队安全消费，完美避开了相互等待引发的死锁问题。

### 2.3 健壮的窗口查找算法
*   **问题场景**：早期的代码通过 `FindWindow(L"Progman", ...)` 来寻找桌面底层窗口。但遇到安装了 Wallpaper Engine 或其他桌面美化软件的环境时，父子窗口的层级关系会被重构（如被下放到 `WorkerW` 下），导致 `FindWindow` 失效，Mod 在一些环境下不生效。
*   **经验沉淀**：不能依赖硬编码的层级结构。
*   **解决方案**：改用 `EnumWindows` 全局遍历。只要目标窗口属于 `explorer.exe` 进程，且拥有类名为 `SHELLDLL_DefView` 的子窗口，我们就认定其为合法的桌面视图。这种动态遍历的方法极大地提高了在复杂桌面环境下的兼容性。

### 2.4 批处理与自动化注册表部署
*   **问题场景**：希望用户实现"一键启用"所有 Mod。但 Windhawk 的脚本生效往往需要用户手动打开 UI 点击启用。
*   **经验沉淀**：在缺乏 GUI 交互的自动化部署中，通过配置系统底层设置是最佳出路。在批处理中写脚本，**永远不要急于添加 `>nul 2>&1`屏蔽输出**，这会掩盖掉很多诸如权限被拒绝的关键错误。
*   **解决方案**：`deploy.bat` 中不仅复制源码，还通过 `reg add "HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\zen-desktop-toggle-icons" /v Disabled /t REG_DWORD /d 0 /f` 等命令强制写入配置。这样实现了"覆盖源码即激活"的无缝对接。

### 2.5 纯底层模拟双击 (Double Click) 逻辑
*   **问题场景**：我们要实现"桌面空白处双击"。系统的 ListView 控件本身拥有 `CS_DBLCLKS` 样式可以直接捕获 `WM_LBUTTONDBLCLK`，但在其父级 `SHELLDLL_DefView` 进行 Subclass 时，由于父窗口没这个样式，它是收不到原生双击消息的。
*   **经验沉淀**：必须手动接管消息拦截。
*   **解决方案**：手动监听每次 `WM_LBUTTONDOWN` 鼠标左键按下消息。记录当前的时间戳 (`GetTickCount()`) 和坐标点。当第二次按下时，调用系统的 `GetDoubleClickTime()` 判断时间差是否合规，并且利用 `GetSystemMetrics(SM_CXDOUBLECLK)` 判断两次点击的物理距离误差，借此来从逻辑上模拟并判断一次有效的"双击"。

### 2.6 自动隐藏活动检测的正确范围（v3.1.0 经验）
*   **问题场景**：v2.9.0 中的自动隐藏功能在实际使用中完全失效。根因是活动追踪只依赖 `g_lastDesktopActivity`——一个仅在鼠标进入 `SHELLDLL_DefView` 或 `SysListView32` 时才更新的变量。用户在浏览器、任务栏等其他窗口操作时，`g_lastDesktopActivity` 不更新，倒计时照常走，导致图标在用户**明明在活跃使用电脑**时被错误隐藏。
*   **经验沉淀**：**"桌面活动"≠"系统活动"。** 自动隐藏的语义应该是用户整体空闲，而不仅仅是不碰桌面。
*   **解决方案**：用 **`GetLastInputInfo()`** 替换 `g_lastDesktopActivity`。该 API 直接返回系统级最后一次输入（鼠标/键盘）的 tick，无论鼠标在哪个窗口，只要用户在操作就不会触发隐藏。

### 2.7 手动隐藏 vs. 自动隐藏的语义区分（v3.1.0 经验）
*   **问题场景**：用户双击桌面空白处**故意**隐藏图标后，鼠标稍微移动一下，图标马上又出现了，完全违背操作意图。根因是旧版本对"图标为何被隐藏"毫无记录，恢复逻辑一律触发。
*   **经验沉淀**：Toggle 操作有两种语义——"我想清空桌面专注工作"（手动）和"系统因为我没动才隐藏的"（自动）。恢复逻辑必须区分这两种场景。
*   **解决方案**：引入全局变量 **`g_autoHiddenAt`**（DWORD tick）。
    *   自动隐藏时：`g_autoHiddenAt = GetTickCount()`，标记本次为自动行为。
    *   任何手动切换（双击）时：`g_autoHiddenAt = 0`，清除标记。
    *   恢复逻辑检查：只有 `g_autoHiddenAt != 0` 时才执行自动恢复，对手动隐藏完全无感。
    *   所有切换逻辑统一收口到 **`TryToggleIcons(hwnd, isAutoHide)`** 辅助函数，避免分散在多处造成状态不一致。

### 2.8 鼠标恢复的范围与时效性（v3.1.0 经验）
*   **问题场景**：v2.9.0 仅在 `SHELLDLL_DefView` 的 `WM_MOUSEMOVE` 中尝试恢复图标，而 `SHELLDLL_DefView` 在图标可见时收不到鼠标消息（此时 `SysListView32` 是命中目标），图标隐藏后才能收到。这导致"鼠标移到其他窗口再移回桌面"这种路径完全无法触发恢复。
*   **解决方案**：双路并行恢复策略：
    1. **即时路径**：`SHELLDLL_DefView` 的 `WM_MOUSEMOVE` / `WM_LBUTTONDOWN` → 鼠标进入桌面区域立即恢复（零延迟）。
    2. **定时轮询路径**：在 500ms 的 `WM_TIMER` 回调中，用 `GetLastInputInfo()` 检测是否有发生在 `g_autoHiddenAt` 之后的系统输入，有则恢复。覆盖鼠标在**其他窗口**移动这种情形，最大延迟 ≤ 500ms。

### 2.9 Windhawk 官方社区代码同步机制（发布与开源）
*   **问题场景**：ZenDesktop 套件中的部分核心 Mod（如桌面图标双击隐藏 `zen-desktop-toggle-icons`）也是公开发布在 Windhawk 官方模组库中的开源项目。如果只在本地 `ZenDesktop_OneKeyDeploy` 维护，会导致官方社区版本（即用户通过 Windhawk 商店搜索下载的版本）停滞不前。
*   **经验沉淀**：建立清晰的 **双仓维护机制 (Dual-Repository Workflow)**。
    1. **私有/套件仓 (`ZenDesktop_OneKeyDeploy`)**：作为主开发库。负责所有快速迭代、联调测试以及部署脚本（`deploy.bat`）的集成。
    2. **开源/同步仓 (`windhawk-mods`)**：作为发布库（fork 自官方 `ramensoftware/windhawk-mods`）。当主仓版本稳定（如达成 v3.1.0 里程碑）后，手动将 `local@zen-desktop-toggle-icons.wh.cpp` 的内容复制、重命名（去掉 `local@`）并覆盖到同步仓的 `mods/` 目录下。
*   **解决方案与规范**：每次在主仓完成版本跃升（如修复了自动隐藏 Bug），必须执行以下发布流程：
    1. 在 `ZenDesktop_OneKeyDeploy` 中验证全部代码和 `deploy.bat` 脚本逻辑。
    2. 使用文件复制或命令（如 `Copy-Item ...`）将 `.wh.cpp` 源码完整覆盖到 `windhawk-mods/mods/` 中对应的文件。
    3. 在 `windhawk-mods` 仓内执行 Git Commit (e.g., `update zen-desktop-toggle-icons to v3.1.0`)。
    4. 执行 `git push` 提交到远程 GitHub 分支，随后在 GitHub 上发起 Pull Request（PR）合并到 Windhawk 官方主线。这种模式保障了高级用户（通过一键部署包）和普通社区用户（通过 Mod 商店）都能用到最稳定的代码，且避免了混淆。

### 2.10 开始菜单文字隐藏与悬浮显示（v3.3.0 经验）
*   **问题场景**：用户希望实现“纯图标/清爽桌面”风格的开始菜单，隐藏应用和文件夹的文字标签，仅在鼠标悬浮时显示。最开始的实现直接将文字的 `Foreground` 颜色强制设为 `Transparent`。但这带来了两个严重问题：
    1. 阻断了悬浮显现：在 WinUI 中，显式设置的元素属性优先级远高于 VisualState 动画，导致即使触发了悬浮状态，文字依然保持 `Transparent`，无法显示。
    2. 主题偏色缺陷：自定义玻璃主题下，默认文字颜色由于逻辑缺失易变为黑色导致暗色背景下完全隐形。
*   **经验沉淀**：
    1. **属性重写冲突**：显式的属性重写（如 C++ 底层动态设置的属性）会覆盖 XAML VisualState 的 Setter 动画。要想基于父容器的悬浮状态控制子元素显隐，必须利用视觉状态组绑定，而不是粗暴地将颜色设为 Transparent。
    2. **防排版抖动（Layout Shifting）**：使用 `Visibility=Collapsed` 会导致元素脱离文档流，悬浮时文字突然出现会使图标产生极为难看的抖动（排版剧烈位移）。**使用 `Opacity`（透明度度）是实现高端微动画显隐的唯一优雅解法**，既保全了固定网格占位，又实现了视觉上的瞬时切换。
*   **解决方案**：
    1. **视觉状态组 (VSG) 联动**：利用 Windhawk 支持的层级和 VSG 选择器语法，如 `StartDocked.AppListViewItem > Grid@CommonStates > * > TextBlock#AppDisplayName`，将文字标签的 `Opacity` 属性与父级容器的 `CommonStates` 状态机直接绑定。
    2. **三阶下拉配置重构**：从简单的开关升级为 **“显示文字” (Opacity 默认值)**、**“隐藏文字” (Opacity 恒定为 0)**、**“悬浮显示文字” (Opacity@Normal=0, Opacity@PointerOver=1)** 三阶下拉选择。
    3. **完美保色显隐**：悬浮显现时保留文字原生的前景色刷（包括我们为 Custom Glass 补全的 `$CommonFgBrush` 机制），隐藏时通过 `Opacity=0` 保证完美对齐且不抖动。

### 2.11 动态画刷 XAML 解析失败导致的静默失效（v3.3.0 经验）
*   **问题场景**：在配置“任务栏 (Taskbar)”或“通知中心 (Notification Center)”的自定义玻璃主题时，无论用户如何调节“毛玻璃模糊度 (Blur)”或“背景填充浓度 (Opacity)”为 0，任务栏界面依然纹丝不动地显示为不透明的深灰色，完全没有发生任何透明或模糊度的变化。
*   **经验沉淀**：
    1. **静默降级机制**：在 Windhawk 的 XAML 解析与注入机制中，如果动态生成的 XAML 标记字符串（如 `<SolidColorBrush>` 或 `<WindhawkBlur>`）存在**任何细微的语法错误（如缺少引号）**，DWM 渲染引擎不会抛出崩溃或报错框，而是会直接**静默失败 (Silent Failure)** 并回退到 Windows 系统默认的实色背景渲染。这在开发中极具迷惑性，容易被误判为“钩子失效”或“线程被锁”。
    2. **字符串拼接的引号陷阱**：在拼接 XML 属性时，必须极其严密地校对转移引号 `\"`。
*   **问题根源**：在 C++ 生成自定义 XML 字符串时，缺失了关键的闭合双引号：
    - 错误生成：`<SolidColorBrush Color="#121212 Opacity="0.50" />`（颜色属性未闭合，XAML 解析器报错并回退）。
    - 错误生成：`<WindhawkBlur BlurAmount="30 TintColor="#121214 TintOpacity="0.50" />`（所有中间属性的闭合引号丢失）。
*   **解决方案**：
    - 在 `local@zen-taskbar-acrylic.wh.cpp` 与 `local@zen-notificationcenter-acrylic.wh.cpp` 的 `ProcessAllStylesFromSettings()` 拼接处，补全了所有丢失的转义引号 `\"`，生成严丝合缝的合法 XML 结构：
      `customBrush = L"<SolidColorBrush Color=\"" + finalColor + L"\" Opacity=\"" + opacityStr + L"\" />";`
      `customBrush = L"<WindhawkBlur BlurAmount=\"" + blurStr + L"\" TintColor=\"" + finalColor + L"\" TintOpacity=\"" + opacityStr + L"\" ... />";`
    - 更新后，XAML 引擎顺利解析动态画刷，成功实现了零延迟的透明度与毛玻璃纯平滑联动！

### 2.12 高频属性更新与 XamlBlurBrush 实例化性能瓶颈（v3.3.0 经验）
*   **问题场景**：在视觉状态切换（如悬停、焦点或点击动画）或频繁的布局大小更新时，Windhawk 会反复调用 `SetOrClearValue` 来重新套用自定义样式。由于 WinUI 控件属性更改通知非常频繁，这会导致 `XamlBlurBrush`（包含复杂的 WinRT Composition 效果图层）在每次属性通知时都被从头重新创建，导致显著的界面微卡顿（Frame Drop）与资源管理器 CPU/功耗飙升。
*   **经验沉淀**：
    1.  **高频对象重构代价极高**：在 WinRT/XAML 框架下，反复重新构建动态 XAML 字符串并通过 XAML 解析器创建毛玻璃画刷是极其昂贵的。只要其参数（模糊度、颜色等）没有实质性改变，就应该完全跳过重构和重新链接 Composition 渲染层的过程。
    2.  **精细参数缓存比较**：为了实现这一点，必须在自定义结构体中实现深度比较操作。通过为 `XamlBlurBrushParams` 实现 `operator==` 并将其缓存进每个属性自定义状态中，可以在高频调用中秒级拦截重复的重新实例化。
*   **解决方案**：
    -   在 `local@zen-startmenu-acrylic.wh.cpp` 与 `local@zen-taskbar-acrylic.wh.cpp` 中：
        1. 为 `XamlBlurBrushParams` 结构体实现了完整的 `operator==` 逻辑，支持模糊度、色彩值、透明度及主题资源的完整比对。
        2. 在 `ElementPropertyCustomizationState` 中引入了 `std::optional<XamlBlurBrushParams> lastBlurParams` 缓存字段。
        3. 修改了 `SetOrClearValue` 函数的签名，传入缓存指针。若发现本次要生成的画刷参数与上一次一致，则直接 `return` 拦截，完美杜绝了无效的对象再造。
        4. 在清除自定义样式的路径中，同步调用 `.reset()` 清除缓存，保证状态切换周期的严密闭环。
    -   更新后，高频 VisualState 切换过程中的卡顿感与异常耗电被彻底解决，毛玻璃动态主题渲染达到了纯天然的平滑度！

### 2.13 视觉美学与优化逻辑的解耦原则（v3.3.0 经验）
*   **问题场景**：在合并 PR #4 性能优化代码时，优化补丁中夹带了对主题常量（Style Constants）的强行改动——为了降低开销，PR 将毛玻璃模糊度 `BlurAmount` 从 `18` 和 `10` 暴降为 `2` 和 `1`，并将饱和度强制改回 `1.0`。这导致原本流光溢彩、高折射率的 **Apple Liquid Glass** 主题瞬间退化为扁平简陋的“普通透明塑料板”，完全丧失了液态玻璃的高级感。
*   **经验沉淀**：
    1.  **美学参数与性能优化解耦**：性能优化应该专注于“减少无效的对象分配”、“缓存匹配”和“提早拦截退出”等**架构层面的逻辑重构**，而不应该通过牺牲视觉效果（如削减模糊半径、砍掉渐变线等）来换取流畅度。以牺牲设计品质为代价的流畅，在美化模组开发中是不可接受的退步。
    2.  **模块化精准回退策略**：在集成多份改动时，必须清晰地区分“核心渲染引擎优化”与“主题/资源配置文件”。当发现视觉品质降级时，可通过 Git 快速定位并**仅回退主题常量与 XAML 样式定义**，而完整保留底层的 XamlBlurBrush 缓存比对逻辑。
*   **解决方案**：
    -   全面回退了 `local@zen-startmenu-acrylic.wh.cpp` 和 `local@zen-taskbar-acrylic.wh.cpp` 中被篡改的全部 Apple 玻璃主题常量、渐变色刷以及边界厚度设置，恢复其原本高模糊度（18/10）、高饱和度（2.0/2.2）以及棱镜色散边框的完美美学设计。
    -   100% 保留了本次新开发的 XamlBlurBrush 缓存匹配引擎，实现了“顶级液态折射画质”与“零延迟、无卡顿高帧率”的完美共存。

### 2.14 开始菜单搜索栏常规态与活动态 of 视觉美学统一（v3.3.0 经验，v3.4.0 拓展）
*   **问题场景**：在 Apple 玻璃主题以及后续的**自定义玻璃 (SimplyTransparent) 与所有磨砂半透明主题**下，开始菜单未激活时（正常态），搜索框表现为刺眼的**纯白色实色方块**；而当点击文件夹打开时（活动态），搜索框却突然神奇地变成了**与背景融为一体的流光玻璃**。这种两极分化的不一致极大破坏了高级感。
*   **深层根源**：
    1.  **内层控件模板遮挡**：搜索框底层虽然有我们定制的毛玻璃背景容器（`TaskbarSearchBackground`），但其内部的核心切换按钮（`SearchBoxToggleButton`）自带一个模板边框（`Border`）。在 Windows 默认浅色主题下，该 Border 被赋予了纯白实色，如同一膏药把底部的半透玻璃完全遮死。
    2.  **禁用状态机的“副作用变透”**：当点击文件夹展开时，开始菜单触发了全局模态。背后的搜索按钮被 Windows 强制切换到 **`Disabled` 状态**。根据 WinUI 的默认控件状态机，该按钮在 Disabled 时背景会置为 `Transparent`，结果“无意中”摘掉了这层白色膏药，让底部的玻璃图层重见天日。
*   **解决方案与拓展（v3.4.0 沉淀）**：
    -   **初代修复**：在 `local@zen-startmenu-acrylic.wh.cpp` 的标准与经典 Apple 主题中，直接将 `StartMenu.SearchBoxToggleButton` 和 `StartDocked.SearchBoxToggleButton` 下所有内部 `Border`（含 `BorderElement`）的 `Background` 和 `BorderBrush` 显式强制设为 `Transparent`。
    -   **全主题拓展**：由于自定义玻璃（SimplyTransparent）以及各种透明度磨砂主题（TranslucentStartMenu 系列预设）都是以基础的 `g_themeTranslucentStartMenu`（标准和经典版）为底图加载的，因此在 v3.4.0 中，我们将这一套透明覆写规则直接合入到了两个 `g_themeTranslucentStartMenu` 基础主题中，从而一次性修复了所有半透明主题的白框 Bug。
    -   **最终效果**：这一重构彻底移除了常规状态下的实白遮挡，使搜索框在**所有玻璃/半透明主题**、在**任何运行状态**下，都始终如一地展现高折射率的液态玻璃美感。

### 2.15 任务栏高频 Composition 缓存 Bug 与稳健回退策略（v3.4.0 经验）
*   **问题场景**：在集成 PR #4 的 `XamlBlurBrush` 高频属性缓存逻辑后，虽然开始菜单运行完美，但在特定 Windows 环境或多显示器切换、资源管理器高频刷新时，任务栏偶尔会出现重绘卡死或画刷丢失的视觉 Bug。
*   **深层根源**：任务栏（`explorer.exe`）的视觉元素和生命周期远比开始菜单（`StartMenuExperienceHost.exe`）复杂。任务栏频繁涉及系统通知区域重绘、多显示器扩展栏以及各种第三方美化软件的 XAML 诊断拦截。在这种环境下，精细缓存的比对逻辑极易被 Windows 本身的重绘流打断或造成死锁。
*   **解决方案**：
    -   **稳定性优先原则**：在追求绝对性能和保障绝对稳定性之间，应优先确保桌面核心组件（任务栏）的 100% 稳健。
    -   **模块化回退**：开始菜单模组（`zen-startmenu-acrylic`）完整保留 `XamlBlurBrush` 的高频参数比较和失效判定以维持流畅度；而任务栏模组（`zen-taskbar-acrylic`）果断回退至极度稳定且经过广泛实测验证的前一版本（去除缓存结构，采用 Windows 原生重绘流机制），同时在头部保持 `3.4.0` 的版本对齐。

### 2.16 大文件推送限制与 GitHub 仓库轻量化设计（v3.4.0 经验）
*   **问题场景**：在将本地编译的发布包 ZIP 推送至 GitHub 时，由于离线安装包中带有完整的离线编译器导致压缩包大小达到了 `142MB`，触发了 GitHub 单文件不得超过 `100MB` 的硬限制，导致 `git push` 被远程服务器拒绝。
*   **经验沉淀**：编译出的最终二进制文件、打包的 ZIP 压缩包不应进入 Git 的历史版本追踪中，尤其是超过 50MB 的大文件。这不仅会造成 Git 仓库体积极速膨胀，还会直接破坏推送流程。
*   **解决方案**：
    -   在 `.gitignore` 中彻底将 `*.zip` 以及 `*.exe` 文件列入忽略，确保只追踪精简的 C++ 源码、编译脚本和部署引导。
    -   最终的 release 二进制 ZIP 文件只在本地由脚本生成，并通过 GitHub 网页端 Releases 界面以 Releases 附件形式手动上传发布。

### 2.17 连续切换设置导致的 Explorer 崩溃问题与防抖机制（v3.4.0 经验）
*   **问题场景**：用户在 Windhawk UI 的下拉菜单中快速、连续地切换背景颜色等设置时，`explorer.exe` 会发生崩溃并被 Windows 自动重启。
*   **深层根源**：任务栏等 Mod 在 `Wh_ModSettingsChanged()` 热重载机制中，会调用 `UninitializeForCurrentThread()` 卸载旧样式并立刻通过 `InitializeForCurrentThread()` 加载新样式。如果用户在几百毫秒内连续切换，高频的卸载/加载循环会导致 XAML 元素在还没完全恢复原状时又被再次强行卸载重置，导致 WinUI 内部状态机崩溃，引发了宿主进程（`explorer.exe`）的强制重启。
*   **解决方案**：
    -   在所有三个核心美化 Mod (`zen-taskbar-acrylic`, `zen-startmenu-acrylic`, `zen-notificationcenter-acrylic`) 的 `Wh_ModSettingsChanged()` 头部引入了**防抖 (Debounce) 机制**。
    -   利用 `std::atomic<DWORD>` 作为自增计数器。每次调用时递增并记录当前序列号，随后休眠 300ms。休眠结束后，检查全局计数器是否依然等于当前序列号。如果不等，说明在这 300ms 内又有新的设置更改触发，当前这次中间状态的变更直接 `return` 丢弃。
    -   只有在最后一次停止切换超过 300ms 时，才会真正触发代价昂贵的 `UninitializeSettingsAndTap()` 和后续的完全重载流程。这不仅杜绝了崩溃，也显著减轻了系统的瞬时渲染压力。

### 2.18 Windhawk 编译缓存机制与一键部署脚本重编译失效问题（v3.4.0 经验）
*   **问题场景**：开发者在一键部署脚本（`deploy.bat`）中升级替换了新的 C++ 源码文件（如 `.wh.cpp`），并成功复制到了 Windhawk 的 `ModsSource` 目录下并重启了 Windhawk 进程。然而，运行部署脚本后，即使对 `.wh.cpp` 进行了修改（如添加了 300ms 防抖机制等重大稳定性修复），系统的热重载和编译流程完全没有被触发，Windhawk 依旧顽固地加载并运行前一个版本的旧代码，导致新补丁无法生效。
*   **深层根源**：
    1.  **DLL 编译路径缓存**：Windhawk 拥有独立的模块管理机制，当一个 Mod 在本地完成编译后，其生成的物理 `.dll` 路径会被永久写入系统注册表中：
        `HKLM\SOFTWARE\Windhawk\Engine\Mods\<mod-id>` 项下的 **`LibraryFileName`** 键（例如 `local@zen-taskbar-acrylic_3.4.0_441282.dll`）。
    2.  **跳过源文件检查**：当 Windhawk 服务或主程序重启时，它会优先读取注册表中的 `LibraryFileName`。如果该键指向一个真实存在的 `.dll`，Windhawk **会无条件直接加载该 DLL，而根本不会去检查或比对 `ModsSource` 目录下的 `.wh.cpp` 是否已被修改**。
    3.  **脚本简化导致的缓存残留**：在 v4.0.0 一键部署脚本简化重构期间，由于删除了清空注册表键值的步骤，旧的 `LibraryFileName` 指针在覆盖源码文件后仍残留在注册表中。这使得每次重启 Windhawk 时都直接静默加载了旧版 DLL 缓存，使得本地的源码覆盖部署实质上沦为了“无用功”。
*   **解决方案**：
    -   **动态 ID 提取与注册表缓存清零**：
        1. 在 `deploy.bat` 的一键部署复制循环中，使用 Batch 的环境变量延迟扩展机制，通过截取语法 `set "MOD_NAME=%%~nf"` 与 `set "MOD_ID=!MOD_NAME:.wh=!"` 动态从文件名（如 `local@zen-taskbar-acrylic.wh.cpp`）中提取出标准 Mod ID（如 `local@zen-taskbar-acrylic`）。
        2. 在源码文件成功复制到 `ModsSource` 目录后，立即执行 `reg add` 指令强制将 `HKLM\SOFTWARE\Windhawk\Engine\Mods\!MOD_ID!` 下的 `LibraryFileName` 键值重置为空字符串 `""`：
           `reg add "HKLM\SOFTWARE\Windhawk\Engine\Mods\!MOD_ID!" /v "LibraryFileName" /t REG_SZ /d "" /f >nul 2>&1`
        3. 重启 Windhawk 后，引擎检测到注册表中的编译链接为空，将**立刻唤起本地编译器对最新覆盖的 C++ 源文件进行全量热编译**，彻底打通了“源码修改 -> 脚本一键部署 -> 全自动重编译生效”的完美极简开发者闭环！

### 2.19 全新安装场景下的注册表空键冲突与核心服务重启策略（v4.0.0 经验）
*   **问题场景**：在 v3.4.0 引入了“强制重编译清空注册表”（见 2.18）的机制后，老用户升级时一切正常。但当用户**全新安装 Windhawk** 并运行部署脚本时，Windhawk UI 虽然显示模组，但点击编译后无法注入；同时脚本提示成功，但实际模组毫无反应。
*   **深层根源**：
    1.  **进程层级与服务脱节**：旧版脚本的 `taskkill /f /im windhawk.exe` 仅仅杀死了 Windhawk 的前台图形界面（UI），而真正负责编译、注入和文件监听的核心系统服务（`Windhawk Service`）一直在后台运行，并没有被杀死。由于服务未重启，它根本不知道 `ModsSource` 里的文件被替换了，也不会重新读取注册表。
    2.  **暴力注入导致的引擎状态错乱**：在全新安装 Windhawk 时，系统里根本不存在 `HKLM\SOFTWARE\Windhawk\Engine\Mods\<mod-id>` 这些注册表项（因为模组从未被启用过）。但旧版脚本使用了无条件的 `reg add` 强行创建了这些注册表项，且里面**只有空的 `LibraryFileName`，缺失了 `Enabled` 等其它引擎初始化标志**。当后台的核心引擎检测到这个半残的注册表时，发生了逻辑死锁，判定该模组“已安装但状态异常”，直接拒绝后续的编译和注入流程。
*   **重构设计与解决方案（设计思路）**：
    -   **防御性注册表操作（先判定再执行）**：针对升级用户和全新安装用户实行差异化逻辑。在清空 `LibraryFileName` 之前，先利用 `reg query` 判断该模组的注册表是否存在。
        -   **如果存在（老用户升级）**：执行清空操作，迫使引擎重启时重新编译。
        -   **如果不存在（新用户全新安装）**：**绝对不主动创建注册表**，仅将 `.wh.cpp` 源文件复制到 `ModsSource`，把初始化的工作交还给 Windhawk 引擎自己处理。
    -   **彻底的生命周期接管**：弃用局限性极大的 `taskkill`，改用底层的 `net stop Windhawk /y` 和 `net start Windhawk`。在复制文件和修改注册表前，彻底挂起核心引擎服务；在所有 I/O 操作完成后，再重新启动核心服务。这保证了引擎重启时能 100% 重新拉取所有源码文件 and 注册表状态，实现了真正的“一键完美部署”。

### 2.20 终极零手工“直接注入”与 UI 图形列表完美同步机制（v4.0.0 突破）
*   **问题场景**：用户期望“一键脚本直接注入并自动启用”，完全不希望有任何手动的 Windhawk UI 交互（如点击“新建模组”、“编译并安装”等）。在 v4.0.0 重构中，为了彻底打破该壁垒，我们需要在脚本中直接“装配”好一切所需元数据，并在重启时无缝触发自动编译与注入，且必须解决“UI 列表不刷新”的痛点。
*   **深层根源**：
    1.  **进程筛选器与元数据绑定**：Windhawk 服务在后台编译和加载模组时，必须依赖注册表 `HKLM\SOFTWARE\Windhawk\Engine\Mods\<mod-id>` 下的元数据。其中，**`Include`**（目标进程过滤）是重中之重，它指示了模组注入到哪些宿主。在官方设计中，`Include` 实际上是一个普通的字符串类型键（**`REG_SZ`**），如果有多行 `@include` 描述，官方会使用竖线 **`|`** 进行拼接（例如 `"ShellExperienceHost.exe|ShellHost.exe"`）。如果缺少此键或类型错误（如误用多字符串 `REG_MULTI_SZ`），引擎将因无法解析进程范围而死锁或报错。
    2.  **动态版本兼容**：不同模组在发展过程中其版本（`Version`）各不相同（如 `3.4.0` 或其他）。如果写死固定版本，当引擎加载到与 cpp 源码头不符的注册表声明时，可能触发完整性或兼容性验证失败。
    3.  **前台 UI 的缓存限制（`userprofile.json`）**：Windhawk 的前端 UI 并不是每次启动都实时扫描注册表或文件，而是极度依赖位于 `%ProgramData%\Windhawk\userprofile.json` 中的**本地 UI 缓存文件**。直接在底层注入注册表和文件，虽然服务层能成功在后台编译并注入 `explorer.exe`，但由于 `userprofile.json` 缓存未更新，前台 UI 的“已安装”列表依然会显示为空，给用户造成“注入失败”的重大误导。
*   **重构设计与解决方案（设计思路）**：
    -   **PowerShell 动态元数据提取引擎**：弃用复杂且难以维护的 Batch，核心逻辑由 [deploy.ps1](file:///d:/Repository/ZenDesktop_OneKeyDeploy/deploy.ps1) 接管。通过健壮的正则表达式，动态提取 `.wh.cpp` 的真实 `@version`、`@architecture`，以及自动提取多行 `@include` 并以 `|` 拼接为标准的 `REG_SZ` 写入注册表，实现与 Windhawk 引擎设计的 100% 贴合。
    -   **安全创建 ModsWritable 键**：在 `HKLM\SOFTWARE\Windhawk\Engine\ModsWritable\<mod-id>` 中创建对应的运行时子键，确保引擎运行环境完整。
    -   **强力 UI 缓存强刷（终极痛点解决）**：在服务停止状态下，脚本主动检测并强行删除 `userprofile.json` 缓存。在 Windhawk 服务和 UI 重启后，UI 引擎由于检测到缓存缺失，将**无缝、重新扫描 `ModsSource` 文件夹下所有源文件头，自动重建缓存**。这使所有模组不仅能够完全在后台静默编译并注入，还能 **100% 完美呈现在 Windhawk UI 列表中**！实现了“零手工交互、全自动直接注入、前后台完美同步”的极致体验！

### 2.21 文件管理器 WinUI 全局画刷过度透明导致的可读性灾难（vNext 经验）
*   **问题场景**：在把第三方 File Explorer 透明背景 Mod 改造成 `local@zen-fileexplorer-acrylic.wh.cpp` 后，文件管理器出现大面积发白、左侧导航栏文字和图标变得极淡、内容区像被白色蒙版覆盖的现象。用户怀疑是 Windows 透明效果或系统主题设置导致。
*   **深层根源**：
    1.  **文件管理器不是简单的单层窗口**：Windows 11 File Explorer 的主窗口由 DWM 背景、Win32 子窗口、WinUI/XAML 资源字典和多个内容岛共同组成。把某个背景刷置为透明，并不一定能透到底层壁纸，很多时候只会露出 Explorer 自己的浅色宿主层。
    2.  **全局 WinUI 画刷污染范围过大**：早期实现为了追求“全透明”，把 `LayerFillColorDefaultBrush`、`LayerOnMicaBaseAltFillColorDefaultBrush`、`CardBackgroundFillColorDefaultBrush`、`SystemControlBackgroundChromeMediumLowBrush` 等通用资源直接替换为 `Transparent`。这些资源不仅影响文件列表，还会影响导航栏、卡片、命令栏和系统默认内容层，导致浅色主题下白底吃掉文字对比度。
    3.  **进程内资源覆盖不自动回滚**：Windhawk Mod 在 Explorer 进程内修改 XAML 资源后，即使源码随后修复，旧进程里已被覆盖的资源也可能继续存在，必须通过重新应用安全资源或重启 Explorer 才能彻底恢复。
*   **经验沉淀**：
    1.  **透明化必须默认保守**：文件管理器属于高频生产力界面，默认效果应优先保证可读性和可恢复性。真正危险的“文件列表透明 / 导航树透明 / 命令栏透明”应该是显式开关，而不是默认开启。
    2.  **不要默认覆盖全局背景资源**：针对 Explorer，宁可先只设置窗口级 DWM/Mica/Accent 背景，也不要一上来改 WinUI 全局资源字典。全局资源覆盖必须只用于受控、可解释、可关闭的范围。
    3.  **需要可读性保护层**：当实验透明效果导致画刷污染时，新版本必须能主动把关键背景和文字画刷刷回安全颜色，而不是只停止继续透明。
*   **解决方案**：
    -   将 `local@zen-fileexplorer-acrylic.wh.cpp` 的默认 `theme` 从 `AppleLiquidGlass` 改为更稳定的 `Mica`，避免新用户一启用就进入高风险透明态。
    -   新增 **`transparentFileList: false`**，文件列表透明默认关闭。只有用户显式打开时，才对子项资源和 `SysListView32` / `SHELLDLL_DefView` 做透明化处理。
    -   新增 **`readabilityGuard: true`**。在 `ApplyReadabilityGuardResources()` 中，根据系统明暗主题，把关键背景资源恢复为安全白/暗底，并把默认文字资源恢复为对应的深/浅前景色。这样旧版本造成的透明画刷污染可以被新版主动对冲。
    -   删除默认路径里对 `LayerFillColorDefaultBrush`、`LayerOnMicaBaseAltFillColorDefaultBrush`、`CardBackgroundFillColorDefaultBrush` 等全局资源的透明覆盖，把这些危险操作从默认体验中移除。
    -   文件列表、导航树、命令栏、XAML bridge 子类化都改成受设置开关控制，避免为了背景效果破坏核心交互界面。
*   **操作建议**：
    -   如果文件管理器已经出现发白/文字变淡，先在 Windhawk 中禁用 File Explorer Mod 或切换到 `Mica`，确保 `transparentFileList`、`applyToNavPane`、`applyToCommandBar` 关闭，然后重启 Explorer。
    -   在没有真实 Windhawk 编译和用户截图复验前，**不要急着重构建 release 包**。先把源码行为做稳，再统一构建最终包。

### 2.22 部署脚本与 Windhawk 平台编译的职责边界（vNext 决策）
*   **问题场景**：为了解决源码替换后未即时生效的问题，曾尝试让 `deploy.ps1` 解析 Windhawk 自带编译器路径、调用 `clang++` 生成 `_zendeploy.dll`，再把注册表 `LibraryFileName` 指向脚本生成的 DLL。
*   **决策结论**：该方向放弃。ZenDesktop 一键脚本只负责 **注入/替换源码与注册表元数据**，不负责本地编译。编译、保存、错误查看和最终 DLL 生成应交给 Windhawk 平台/UI 完成。
*   **原因**：
    1.  Windhawk 平台已经拥有完整的编译、日志和错误展示流程，用户也更容易在 UI 中确认某个 mod 是否编译成功。
    2.  脚本侧手动拼接 `WH_MOD_ID`、`WH_MOD_VERSION`、compiler options、include/lib 路径时，PowerShell 到 clang 的引号/反斜杠转义非常容易出错。
    3.  脚本生成私有 DLL 会制造额外状态，例如 `_zendeploy.dll` 文件残留、`LibraryFileName` 指向脚本产物、平台 UI 与实际加载 DLL 不一致，后续排障成本更高。
*   **当前规范**：
    -   `deploy.ps1` 复制 `.wh.cpp` 到 Windhawk 的 `ModsSource`，解析 `@version`、`@architecture`、多 `@include` 并写入注册表。
    -   `LibraryFileName` 必须写为空字符串 `""`，表示交还给 Windhawk 平台编译。
    -   脚本可以清 `userprofile.json` 以刷新 UI 缓存，但不要调用 clang，也不要生成或注册 `_zendeploy.dll`。

### 2.23 任务栏 `Blur=0 + Opacity=0` 变黑的清透明语义（vNext 经验）
*   **问题场景**：任务栏设置为 `Custom Glass`、`Mist Grey`、`Blur=0`、`Opacity=0` 后，底部任务栏没有呈现预期的清透明效果，而是显示为一条纯黑底。
*   **复盘结论**：旧仓库的工作路径在 `blurPreset == 0` 时使用 `<SolidColorBrush ... Opacity="0" />`，这条路径在 Win11 XAML 任务栏上是可用的。错误来自 vNext 改动把 `Blur=0` 强行改成 `<WindhawkBlur BlurAmount="0" ...>`，同时又给 `Shell_TrayWnd` 叠加了 Win32 `SetWindowCompositionAttribute` 兼容层，导致底部露出或覆盖成黑色宿主层。
*   **解决方案**：恢复旧仓库的 `Blur=0` 语义：基础背景模糊为 0 时继续生成 `SolidColorBrush`，并用 `Opacity` 表达透明度。只有当基础模糊或最大化状态层把最终 blur 提升到大于 0 时，才生成 `<WindhawkBlur BlurAmount="...">`。
*   **维护规范**：不要在未经实机验证的情况下替换旧仓库已经跑通的透明语义。`WindhawkBlur BlurAmount=0`、`SolidColorBrush Opacity=0` 和 Win32 accent 不是等价物，尤其不能在 Win11 XAML 任务栏和 `Shell_TrayWnd` 上混用。

### 2.24 任务栏最大化状态层的模糊度语义（vNext 经验）
*   **问题场景**：`Maximized Window Layer` 下的 `Layer Blur` 看起来像另一套逻辑，数值调大并不一定像上方 `Blur` 一样变得更模糊。
*   **深层根源**：旧实现主要通过 Win32 `SetWindowCompositionAttribute` 给 `Shell_TrayWnd` 加兼容层。该接口基本只能表达“透明/亚克力开关”和颜色透明度，不能像 XAML `WindhawkBlur` 一样精确传入 `BlurAmount`。因此 `Layer Blur` 虽然被读取，却没有完整接入 Windows 11 XAML 任务栏的真实模糊半径。
*   **解决方案**：普通任务栏背景和最大化状态层统一通过 `BuildTaskbarBlurBrush()` 生成 `<WindhawkBlur BlurAmount="...">`。当前台最大化窗口覆盖任务栏所在显示器时，XAML 任务栏会重建背景规则，并使用 `max(baseBlur, maximizedLayerBlur)` 与 `max(baseOpacity, maximizedLayerOpacity)`，保证数值越大越模糊、越厚实。
*   **维护规范**：最大化状态层只应增强基础玻璃层，不应降低用户当前的基础模糊和浓度设置。Win10/经典任务栏仍保留 Win32 accent fallback，但 Win11 XAML 任务栏必须优先走 `WindhawkBlur BlurAmount` 语义。
*   **文案规范**：`Layer Opacity` 是浓度/厚度设置，不是清晰度设置。选项名和说明文字不得再写“数值越大越清晰”，避免误导用户把透明度问题当成模糊半径问题。
*   **设置读取注意**：Windhawk 数字输入框可能出现空值。最大化层设置使用 `GetIntSettingOrDefault()` 先按字符串读取，空值/非法值回默认值；只有用户明确填入 `0` 时才解释为“不模糊”。
*   **Win11 黑底修正**：`SetWindowCompositionAttribute` 兼容层只能用于 Windows 10/经典任务栏。不能只依赖 `Windows.UI.Composition.DesktopWindowContentBridge` 是否已经创建，因为 Win11 启动早期可能暂时查不到 XAML 桥窗口。应先用 `RtlGetVersion` 判断 build >= 22000，Win11 上直接禁用经典 accent，让 XAML 侧的 `WindhawkBlur` 独占背景，否则会出现纯黑任务栏。

### 2.25 任务栏 `theme=""` 导致样式未应用的黑底回归（vNext 经验）
*   **问题场景**：Windhawk UI 中任务栏 Mod 已启用并加载 DLL，但任务栏仍然是纯黑。检查注册表发现 `HKLM\SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic\Settings` 下 `theme` 为空字符串，`blurPreset=0`、`opacityPreset=0`。
*   **深层根源**：`ProcessAllStylesFromSettings()` 原本只在 `theme == "TranslucentTaskbar"` 时选择 `g_themeTranslucentTaskbar`。当 `theme` 为空时，代码把它解释为设置中的 `None`，不会注册任何任务栏 XAML 目标样式，因此 Windows 自己的深色任务栏继续显示为黑色。
*   **解决方案**：运行时把缺失或空字符串的 `theme` 视为损坏/缺省配置，回退到 `TranslucentTaskbar`。部署脚本也只在 taskbar mod 的 `Settings\theme` 缺失或为空时补写 `TranslucentTaskbar`，不覆盖用户已选择的非空主题。
*   **维护规范**：任务栏套件默认必须有一个有效视觉风格。`"" = None` 作为官方样式选项可以保留给高级用户，但一键部署和空设置恢复路径不能让它成为默认运行态。

### 2.26 任务栏颜色控制与 `Opacity=0` 透明语义（vNext 经验）
*   **问题场景**：Windhawk 中切换 `Visual Style` 能看到任务栏样式变化，但切换 `Color Preset` 不明显；当 `blurPreset=0` 且 `opacityPreset=0` 时任务栏显示为黑色。
*   **深层根源**：上一版的颜色控制主要通过替换主题变量（如 `CommonBgBrush`、`Background`、`Apple_Background`）实现。部分主题的可见任务栏表面是 `Taskbar.TaskbarFrame > Grid#RootGrid` 或 `Grid#SystemTrayFrameGrid` 上的背景，颜色变量不一定覆盖到实际表面。另外，`Opacity=0` 不能被修正成非零浓度，它在语义上必须表示透明。
*   **解决方案**：保留上一版变量替换链路，但当用户选择非默认颜色时，同时对任务栏实际可见的 RootGrid/SystemTray 背景应用显式覆盖；当 `Blur=0` 且 `Opacity=0` 时，不生成透明度为 0 的颜色刷，而是清空 RootGrid/SystemTray 背景并把 BackgroundFill 透明化，保持 `0 = 透明` 的语义。
*   **维护规范**：不要为了让颜色“看得见”而把 `Opacity=0` 暗中提升到 50 等非零值。透明态应该通过清空/透明化实际背景层实现，而不是把不可见颜色刷盖到 WinUI 宿主黑底上。

### 2.27 任务栏 Mod 回退到上一版源码（vNext 经验）
*   **问题场景**：最大化状态层、Win10/经典兼容层和颜色覆盖实验叠加后，任务栏在实机上仍出现黑底和颜色设置不符合预期的问题。
*   **处理结果**：`local@zen-taskbar-acrylic.wh.cpp` 已整体替换回 `ZenDesktop_OneKeyDeploy_v3.4.0_Online\local@zen-taskbar-acrylic.wh.cpp`，SHA256 与上一版源码一致。部署脚本中针对 taskbar 的 `theme` 自动修复块也已移除，避免一键注入额外改写任务栏用户设置。
*   **维护规范**：后续任务栏功能应先基于上一版可用源码做单点、小步验证。尤其不要同时改 Win11 XAML 样式、Win32 accent fallback、最大化检测和颜色变量覆盖链路。

### 2.28 任务栏实验改动恢复前必须先重启 Explorer 复核（vNext 经验）
*   **问题场景**：任务栏实验逻辑回退到上一版后，重启 Explorer 才发现任务栏 Mod 正常加载，说明此前“纯黑/颜色不变”的一部分现象来自 Explorer 进程旧状态或未重新加载 DLL，而不一定全是源码逻辑错误。
*   **处理结果**：在确认 Explorer 重启是关键变量后，可以恢复实验改动，但恢复范围必须完整：源码里的最大化状态层、Win10/经典任务栏 fallback、颜色覆盖与 `0 = 透明` 语义要一起恢复，生命周期中的 `StartTaskbarStateHooks()` / `RefreshClassicTaskbarBackdrops()` 也必须恢复，否则会出现“设置项在 UI 里，但运行时代码没挂上”的半恢复状态。
*   **维护规范**：以后任务栏视觉调试按顺序执行：注入源码 -> Windhawk UI 保存/编译 -> 重启 Explorer -> 再判断视觉结果。不要在未重启 Explorer 的情况下连续推翻源码结论。

### 2.29 任务栏 `0 = 透明` 不能通过清空 RootGrid 实现（vNext 经验）
*   **问题场景**：重启 Explorer 并确认任务栏 Mod DLL 已加载后，`TranslucentTaskbar + GlassWhite + Blur=0 + Opacity=0` 仍然显示为黑色任务栏。
*   **深层根源**：实验版为了让 `0 = 透明` 更“彻底”，在 `baseBlur == 0 && baseOpacity == 0` 时新增了 `Taskbar.TaskbarFrame > Grid#RootGrid` / `Grid#SystemTrayFrameGrid` 的 `Background:=` 清空，以及 `BackgroundFill` 的 `Fill=Transparent` 覆盖。实机证明这会露出 WinUI/Shell 宿主黑底。上一版可用逻辑并没有清空 RootGrid，而是继续通过主题变量把 `CommonBgBrush` 替换成 `<SolidColorBrush ... Opacity="0" />`。
*   **解决方案**：恢复上一版透明语义：基础 `Blur=0` 时始终生成 `SolidColorBrush`，即使 `Opacity=0` 也不要清空 RootGrid/SystemTray。颜色显式覆盖只在 `baseBlur > 0` 或 `baseOpacity > 0` 时应用到 RootGrid/SystemTray；纯透明态只走主题变量替换链路。最大化层如果最终产生 `WindhawkBlur` 且 tint opacity 为 0，则给 blur brush 加透明 fallback，避免失败时回落为黑底。
*   **维护规范**：不要把“透明”理解为删除背景资源。Win11 任务栏上“透明 brush”和“清空 background property”不是等价操作；前者可由 Windhawk/XAML 管线正确接管，后者可能直接暴露系统黑底。

### 2.30 任务栏实验必须从 3.4 原版基线逐步恢复（vNext 调试策略）
*   **问题场景**：即使删除 RootGrid 清空逻辑并重启 Explorer，任务栏仍然保持黑色。用户明确要求“从 3.4 版本一点点来”。
*   **处理结果**：`local@zen-taskbar-acrylic.wh.cpp` 已再次机械恢复为 `ZenDesktop_OneKeyDeploy_v3.4.0_Online\local@zen-taskbar-acrylic.wh.cpp` 的 3.4 原版，SHA256 一致。当前 Windhawk `ModsSource` 里的 taskbar 源码也与该 3.4 原版一致，实验项如 `maximizedTaskbarLayer`、`classicTaskbarFallback`、`StartTaskbarStateHooks()`、`dwmapi` 均不存在。
*   **调试规范**：后续不要一次性恢复所有实验改动。必须按“基线确认 -> 单点实验 -> 编译保存 -> 重启 Explorer -> 截图/状态确认”的节奏推进。第 1 个建议恢复点只做最大化状态层的 UI 设置和读取，不碰 RootGrid/SystemTray 显式覆盖，也不碰 Win32 accent fallback。

### 2.31 任务栏逐步恢复 Step 1：只恢复最大化层设置与读取
*   **基线结果**：用户确认 3.4 原版任务栏在当前设置下已经恢复透明，说明黑底来自后续实验改动，而不是 Windows 系统设置或 3.4 原版源码。
*   **本步改动**：只新增 `maximizedTaskbarLayer`、`maximizedLayerOpacity`、`maximizedLayerBlur` 三个设置项，以及 `g_settings` 字段、`ClampPercent()`、`ClampBlurAmount()`、`GetIntSettingOrDefault()` 和 `LoadSettings()` 中的读取逻辑。
*   **未恢复内容**：本步不改变 `ProcessAllStylesFromSettings()`，不添加最大化窗口检测，不添加 `StartTaskbarStateHooks()`，不添加 `dwmapi` / Win32 accent fallback，也不新增 RootGrid/SystemTray 显式背景覆盖。
*   **验证目标**：Windhawk 保存/编译并重启 Explorer 后，任务栏应继续保持 3.4 基线透明效果，同时设置页能看到最大化层相关设置。若本步也导致黑底，问题范围就收敛到设置元数据或读取逻辑；若正常，再进入 Step 2。

### 2.32 任务栏逐步恢复 Step 2：只恢复最大化状态检测 hooks
*   **Step 1 结果**：用户确认只恢复设置项和读取逻辑后任务栏仍然透明，说明设置元数据和空值读取不是黑底根因。
*   **本步改动**：新增 `g_taskbarMaximizedLayerActive`、当前进程任务栏窗口枚举、前台窗口是否在同显示器最大化的判断，以及 `SetWinEventHook` 的前台窗口/位置变化监听。生命周期中只调用 `StartTaskbarStateHooks()` / `StopTaskbarStateHooks()`，用于更新内部状态和日志。
*   **未恢复内容**：本步仍不改变 `ProcessAllStylesFromSettings()`，不把 `g_taskbarMaximizedLayerActive` 接入任何 brush，不添加 `dwmapi` / Win32 accent fallback，也不新增 RootGrid/SystemTray 背景覆盖。
*   **验证目标**：开关关闭时应完全等同 3.4 基线；开关开启时也只应产生状态检测，不改变任务栏外观。若本步保持透明，再进入 Step 3，把最大化状态层接入现有 `CommonBgBrush` 替换链路，而不是 RootGrid 清空或 Win32 accent。

### 2.33 任务栏逐步恢复 Step 3：只接入原 3.4 `CommonBgBrush` 路径
*   **Step 2 结果**：用户确认恢复最大化状态检测 hooks 后任务栏仍然透明，说明 WinEvent 检测与生命周期挂载不是黑底根因。
*   **本步改动**：在 `ProcessAllStylesFromSettings()` 原有动态 brush 构建位置增加 `effectiveBlurVal` / `effectiveOpVal`。只有当 `g_taskbarMaximizedLayerActive` 为真时，才用 `max(baseBlur, maximizedLayerBlur)` 与 `max(baseOpacity, maximizedLayerOpacity)` 生成原本就会替换 `CommonBgBrush` 的 brush。状态变化时调用 `ReapplyTaskbarStylesForMaximizedLayerState()` 重新应用现有 XAML 样式。
*   **未恢复内容**：本步不添加 RootGrid/SystemTray 显式背景覆盖，不清空 `BackgroundFill`，不添加 `dwmapi` / Win32 accent fallback，也不引入 classic taskbar fallback。
*   **验证目标**：开关关闭时仍保持 3.4 基线透明；开关开启且前台窗口最大化时，任务栏只通过 `CommonBgBrush` 路径增强背景。如果此步变黑，根因就集中在 `WindhawkBlur`/`SolidColorBrush` 参数组合或状态变化时重套样式；如果透明且最大化层有效，再考虑是否需要 Step 4 做更精细颜色覆盖。

### 2.34 任务栏逐步恢复 Step 3 编译顺序修正
*   **问题场景**：Windhawk 编译 Step 3 时报告 `use of undeclared identifier 'ClampBlurAmount'` 和 `use of undeclared identifier 'ClampPercent'`。
*   **深层根源**：`ProcessAllStylesFromSettings()` 位于源码中部，而 Step 1 新增的 `ClampPercent()` / `ClampBlurAmount()` helper 定义在文件底部 `LoadSettings()` 前。Step 3 第一次在 `ProcessAllStylesFromSettings()` 内调用这些 helper，导致 C++ 编译时尚未看到声明。
*   **解决方案**：在 `ProcessAllStylesFromSettings()` 前增加 `int ClampPercent(int value, int fallback);` 和 `int ClampBlurAmount(int value, int fallback);` 前置声明，不移动实现，避免大范围重排源码。
*   **验证结果**：修复后注入源码，`C:\ProgramData\Windhawk\ModsSource\local@zen-taskbar-acrylic.wh.cpp` 与仓库源码 SHA256 一致，Explorer 已加载新生成的 `local@zen-taskbar-acrylic_3.4.0_438063.dll`。

### 2.35 任务栏逐步恢复 Step 3 防抖：避免频繁重套 XAML 导致 Toolkit 报警
*   **问题场景**：Step 3 最大化层功能可以工作，但频繁放大/缩小窗口时会弹出 Windhawk Toolkit “检测到系统不稳定”的提示，严重时 Explorer 会重启。
*   **深层根源**：`RefreshTaskbarMaximizedLayerState()` 每次检测到最大化状态翻转都会立即调用 `ReapplyTaskbarStylesForMaximizedLayerState()`，而该函数会执行 `UninitializeSettingsAndTap()` 并重新初始化 XAML 自定义规则。快速放大/缩小时，WinEvent 会密集触发，等同于短时间内反复卸载/重挂 Explorer 任务栏 XAML 样式，容易被 Toolkit 判定为不稳定。
*   **解决方案**：把即时重套改为 `PTP_TIMER` 防抖调度。状态变化只更新 `g_taskbarMaximizedLayerActive` 并调用 `ScheduleTaskbarStyleReapplyForMaximizedLayerState()`；定时器延迟 900ms 后只执行最后一次刷新。新增 `g_taskbarStyleReapplyPending` 与 `g_taskbarStyleReapplyRunning`，避免刷新过程重入，并在 `StopTaskbarStateHooks()` 中提前清理 pending 与关闭定时器。
*   **维护规范**：任务栏 XAML 样式重套属于高风险操作，不能直接挂在高频 WinEvent 上。任何未来状态联动都必须“事件只改状态，渲染延迟合并”，并优先保证 Explorer 稳定。

### 2.36 Windhawk Mod 开发注入与状态巡检自动化
*   **问题场景**：调试任务栏最大化层时，今天反复手工执行了同一组动作：复制单个 `.wh.cpp` 到 `C:\ProgramData\Windhawk\ModsSource`、清空 `LibraryFileName`、对比 SHA256、查看 taskbar 设置、确认 Explorer 是否加载了旧 DLL。这些动作如果手敲，容易漏一步并误判视觉结果。
*   **自动化方案**：新增 `dev-windhawk-mod.ps1`。默认处理 `local@zen-taskbar-acrylic`，也可以用 `-ModId` 指定其他本地 mod。脚本只做开发注入和状态巡检，不调用 clang，不构建包，不替代 Windhawk UI 保存/编译。
*   **使用方式**：`powershell -NoProfile -ExecutionPolicy Bypass -File .\dev-windhawk-mod.ps1` 注入默认任务栏源码；`-StatusOnly` 只巡检不写入；`-RestartExplorer` 在注入或巡检后顺手重启 Explorer。
*   **维护规范**：以后单个 mod 调试优先用这个脚本，避免再手动拼管理员 PowerShell、注册表命令和哈希检查。全量发布或打包仍使用 `deploy.ps1` / `build_releases.py`，不要把开发巡检脚本混进 release 包职责里。

### 2.37 最大化层防抖不能牺牲进入最大化的即时效果
*   **问题场景**：Step 3 直接重套版本在实机上确认“放大/缩小可以”，但为了避免 Toolkit 报警改成 `PTP_TIMER` 延迟后，用户反馈最大化后不再产生模糊效果。
*   **深层根源**：最大化进入态是用户最关注的视觉反馈，不能和退出态一样完全依赖延迟 timer。实机上 timer 调度虽然理论正确，但在 Explorer/Windhawk 的 XAML 重套路径里不如直接调用稳定，容易让状态已变而样式没及时重建。
*   **解决方案**：恢复之前可用路径的一半：进入最大化时立即调用 `ReapplyTaskbarStylesForMaximizedLayerState()`；退出最大化或重入冲突时才走延迟合并。这样保留最大化模糊的即时可见性，同时减少快速缩小时的反复卸载/重挂次数。
*   **维护规范**：后续不要把“进入增强态”的视觉反馈完全放进长延迟防抖。高频风险主要来自连续翻转，退出态可以合并，进入态必须优先保证可见。

### 2.38 最大化层即时重套必须有冷却窗口
*   **问题场景**：进入最大化恢复即时重套后，最大化模糊路径可见，但用户连续放大/缩小时再次触发 Windhawk Toolkit 不稳定提示，严重时 Explorer 重启。
*   **深层根源**：`ReapplyTaskbarStylesForMaximizedLayerState()` 本质是高成本的 XAML 样式卸载/重挂。即使只在进入最大化时立即执行，连续放大/缩小也会在短时间内多次进入增强态，仍会密集重套 Explorer 任务栏样式。
*   **解决方案**：增加 `kImmediateReapplyCooldownMs`。只有冷却窗口外的第一次进入最大化允许立即重套；冷却窗口内的后续状态变化全部走 `ScheduleTaskbarStyleReapplyForMaximizedLayerState()` 延迟合并，只应用最后状态。
*   **维护规范**：视觉状态联动需要同时满足两个条件：第一次进入态要快，连续翻转要少。以后调整冷却时间时优先围绕实机稳定性验证，不要只看单次最大化效果。

### 2.39 对标 TranslucentTB：最大化状态只做局部属性刷新
*   **问题场景**：即使加入防抖和冷却窗口，连续最大化/还原仍会触发 Windhawk Toolkit “系统不稳定”提示，并导致 Explorer 反复重启或卡顿。
*   **对标结论**：TranslucentTB 的动态任务栏状态不是在每次窗口事件里卸载/重建整套 XAML 样式，而是维护任务栏/窗口状态，然后对目标任务栏执行轻量的属性刷新。其源码里也明确避免过于频繁地向 Explorer 发送恢复默认外观相关消息。
*   **深层根源**：`UninitializeSettingsAndTap()`、`UninitializeForCurrentThread()`、`InitializeForCurrentThread()` 属于设置热重载/卸载路径，不应挂在 `EVENT_SYSTEM_FOREGROUND` 或 `EVENT_OBJECT_LOCATIONCHANGE` 这类高频 WinEvent 上。防抖只能降低频率，不能改变“高风险重建”的本质。
*   **解决方案**：把 `CommonBgBrush` 从初始化时写死的 brush 改成运行时变量 `{{ZenTaskbarRuntimeBgBrush}}`。最大化状态变化时只更新 `ZenTaskbarRuntimeBgBrush`，由现有 `PropagateStyleVariableChange()` 对已经命中的 `BackgroundFill` 等属性做局部重算和 `SetValue`，不再重启 XAML Diagnostics/TAP，也不再清空并重建所有自定义规则。
*   **检测策略**：状态检测从只看前台窗口改为按 z-order 扫描顶层可见用户窗口，寻找当前任务栏显示器上的最大化窗口，方向更接近 TranslucentTB 的“每个任务栏/显示器有自己的窗口状态”模型。当前实现仍用全局增强状态，后续若要完全 per-monitor，需要把 XamlRoot 与对应任务栏窗口/显示器稳定映射后再做。
*   **语言规范**：新增设置项必须和 ZenDesktop 既有风格统一，使用“中文主标题 (English)”和中文说明，不能保留 `future step`、纯英文或“数值越大越清晰”这类开发阶段/语义错误文案。

### 2.40 通知中心 Win+A 液态玻璃覆盖不足与圆角统一
*   **问题场景**：通知中心外壳已有玻璃效果，但 Win+A 快捷设置面板里的开关块、底部按钮、滑块区仍像系统默认样式，通知卡片和日历区圆角也不统一。
*   **深层根源**：`ControlCenterWindow` 已经在 mod 注入范围内，问题不是窗口未匹配，而是 `g_themeLiquidGlass` 仍主要覆盖旧的 `QuickActions.AccessibleToggleButton` 路径。Windows 11 新版快捷设置实际更多使用 `ControlCenter.PaginatedToggleButton`、`ContentPresenter#ContentPresenter@CommonStates`、`ContentControl#SlidersGroup`、`FooterGrid` 等控件，Apple 变体只替换常量时无法命中这些实际表面。
*   **解决方案**：在 `g_themeLiquidGlass` 中补齐 Win+A 的新版控件选择器，对 `PaginatedToggleButton` 的 `Normal/PointerOver/Pressed/Checked` 状态统一使用 `ElementBackground`、`AccentBackground`、`ElementBackground2`，并给滑块区域、底部栏、通知卡片和分组卡片补 `ElementCornerRadius` 与玻璃边框。Apple 变体常量对齐任务栏/开始菜单的 20/10 主外壳与内元素圆角。
*   **视觉规范**：通知中心面积比任务栏按钮大，彩色折射边不能直接复用任务栏的高 alpha 彩虹边。外壳边框只保留低透明度的细彩色高光，内元素边框以白/暗灰玻璃描边为主，避免整圈红橙蓝紫过饱和。
*   **语言规范**：通知中心主题选项与任务栏/开始菜单保持一致：`LiquidGlass` 标为“液态玻璃 - 原版 (Liquid Glass Original)”，`AppleLiquidGlass` 标为“液态玻璃 (Liquid Glass)”，`AppleLiquidGlassClassic` 标为“液态玻璃 - 经典 (Liquid Glass Classic)”。
*   **验证规范**：调试通知中心时只注入 `local@zen-notificationcenter-acrylic.wh.cpp`，保持 `LibraryFileName=""`，然后在 Windhawk 源代码页保存/编译；如果视觉不刷新，再重启 Explorer 或 ShellExperienceHost。不要为了 Win+A 样式问题去改任务栏运行态逻辑。

### 2.41 文件管理器透明模块改用官方社区 File Explorer Styler 基座
*   **问题场景**：此前 `local@zen-fileexplorer-acrylic.wh.cpp` 基于一个第三方透明背景小 mod 改造，实机出现文件列表发白、左侧文字变淡、全局 WinUI 画刷过度透明等可读性问题。继续在这套代码上补丁会不断碰到 Explorer 多层 WinUI/Win32/DWM 表面不一致的问题。
*   **社区调研结论**：Windhawk 官方社区已有 m417z 的 `Windows 11 File Explorer Styler`，当前 v1.4，目标同样是 `explorer.exe`，内置 `Translucent Explorer11`、`MicaBar`、`WindowGlass`、`LiquidGlass` 等主题，并提供 `backgroundTranslucentEffect` 与 `backgroundTranslucentEffectRegion` 控制。其配套 `windows-11-file-explorer-styling-guide` 也持续收集 File Explorer XAML 目标和主题。
*   **处理方案**：本地 `local@zen-fileexplorer-acrylic.wh.cpp` 保留 ZenDesktop mod id 和中文设置外壳，但源码主体替换为 m417z 官方社区 `Windows 11 File Explorer Styler v1.4`。默认主题改为 `Translucent Explorer11`，通透区域默认保持 `Frame Only`，避免再次默认透明化文件列表。
*   **维护规范**：文件管理器属于高频生产力界面，默认策略必须保守。需要整窗透明时，应通过官方 `backgroundTranslucentEffectRegion=entireWindow` 做显式测试，并优先在深色主题验证；不要再把全局 WinUI 背景画刷批量改透明作为默认方案。

### 2.42 资源管理器右键菜单与系统菜单行为模块接入
*   **需求场景**：用户希望继续补齐资源管理器相关体验，包括 Windows 11 右键菜单默认显示完整菜单，以及资源管理器窗口图标/系统菜单的旧式行为。
*   **社区调研结论**：Windhawk 官方社区已有 m417z 的 `Classic context menu on Windows 11`，当前 v1.0.2，目标为 `explorer.exe`，通过阻止新版 mini menu presenter 让资源管理器默认显示经典右键菜单；同时已有 aubymori 的 `Old Explorer System Menu Behavior`，当前 v1.0.0，目标同为 `explorer.exe`，恢复窗口图标右键当前文件夹菜单、拖拽创建快捷方式等经典行为。
*   **处理方案**：新增 `local@zen-explorer-context-menu.wh.cpp` 与 `local@zen-explorer-system-menu.wh.cpp` 两个独立模块。两者均保留社区核心 hook 逻辑，只改 ZenDesktop 本地 id、名称、中文说明、版本和 README 外壳，不和 File Explorer Styler 合并，避免把视觉 XAML 注入与 Win32/COM 行为 hook 混在一个模块里。
*   **风险边界**：本轮没有默认接入 `dark-menus`。该社区模块的目标是 `@include *`，会影响所有 Win32 程序菜单，风险明显高于仅注入 `explorer.exe` 的右键/系统菜单模块。若后续要做深色右键菜单，应单独做可选模块并明确默认策略，而不是跟经典右键菜单绑定。
*   **部署规范**：`deploy.ps1` 按 `local@*.wh.cpp` 自动扫描，新增文件无需额外脚本逻辑；`build_releases.py` 需要显式加入新源码文件，避免未来打包遗漏。开发期仍保持 `LibraryFileName=""`，由 Windhawk 源代码页保存/编译。

### 2.43 文件管理器 DIY 画刷拼接的 C++ 编译错误
*   **问题场景**：文件管理器 DIY 玻璃设置接入后，Windhawk 编译时报 `expected ';' at end of declaration`，定位到 `borderBrush` 和 `elementBorderBrush` 的 `<LinearGradientBrush ...>` 生成处。
*   **深层根源**：C++ 不会把 `std::wstring(...)` 后面紧邻的宽字符串字面量自动拼接成表达式，必须显式使用 `+`。同时，`L"..." + std::wstring` 这类“字面量在左侧”的拼接也不可靠，容易被解析成指针算术或直接编译失败。
*   **解决方案**：所有动态 XAML 字符串拼接必须以 `std::wstring(L"...")` 起手，后续每段都显式 `+` 连接。已修正 `StripAlphaFromRgbColor()`、`BuildZenDiyBrush()`、`borderBrush`、`elementBorderBrush` 与 `AdjustTypeName()` 中的同类写法，并用文本扫描确认没有同类残留。
*   **维护规范**：以后新增动态 XAML 字符串时，避免用裸字面量作为 `+` 左操作数；长 XML 片段优先拆成 `std::wstring(...) + ...` 的明确表达式，否则 Windhawk 编译错误会只指向很靠后的拼接行，定位成本高。

### 2.44 文件管理器整窗透明与设置简化
*   **问题场景**：用户期望的是资源管理器“整窗可透明、可自动调节”，而不是只有标题栏/命令栏一小块透明。旧设置仍偏向官方 File Explorer Styler 的框架透明模型，并暴露了颜色、模糊、浓度、亮度、外圆角、内圆角、描边、内容区开关等过多调试旋钮。
*   **深层根源**：仅设置 `backgroundTranslucentEffectRegion=Frame Only` 或只改上方 `CommandBar`/`NavigationBar` 容器，无法覆盖 Home/Gallery/Details/RootContainer 等内容区表面。用户已经确认 Windows 需要完整深色模式才能让这些区域透明且可读，因此默认策略可以从“保守框架透明”切换为“深色模式优先的整窗玻璃”。
*   **解决方案**：默认主题改为 `CustomLiquidGlass`，新增 `zenGlassCoverage=auto` 作为日常范围控制。`auto` 与 `fullWindow` 都会走 `DwmExtendFrameIntoClientArea` 整窗扩展，只有 `frameOnly` 才退回框架模式。自定义主题下追加整窗覆盖规则，给 Home/Gallery/Details/RootContainer/NavigationView 等核心容器统一套用 `MainContentBG` 或透明背景。
*   **设置规范**：日常设置压缩为 6 个核心项：视觉风格、透明范围、玻璃颜色、模糊度、背景浓度、圆角。元素圆角、描边强度和内容区玻璃不再单独暴露，而是根据圆角、模糊和浓度自动计算，减少用户在 Windhawk 设置页里反复猜参数。
*   **部署规范**：`deploy.ps1` 和 `dev-windhawk-mod.ps1` 对旧的 `Translucent Explorer11` / `Mica` / `MicaAlt` 文件管理器设置执行一次性迁移，改为 `CustomLiquidGlass + auto + acrylic`，但不覆盖用户后续主动选择的其它主题。

### 2.45 文件管理器整窗透明必须同时处理 XAML 表面与 TP 式宿主窗口层
*   **问题场景**：用户在深色模式下启用文件管理器整窗玻璃后，Home/列表区域仍然呈现深色实底，看起来不是全透明。截图显示上方框架可被样式影响，但内容区背后仍有 Explorer 宿主层暗底。
*   **对比结论**：m417z 的官方 File Explorer Styler 擅长处理 WinUI/XAML 控件样式，但只清 XAML 表面时，底下的 Explorer `CabinetWClass`/子窗口/DWM composition 仍可能提供实色背景。TPGoFighting 旧版透明 mod 的关键价值在于额外处理 `SetWindowCompositionAttribute`、DWM backdrop、`SHELLDLL_DefView`、`DirectUIHWND`、`SysListView32`、XAML island host 等 Win32 子窗口擦背景路径。
*   **解决方案**：保留官方 Styler 作为 XAML 基座，同时接入一个收窄版 TP 宿主透明层：仅在 `CustomGlass` / `CustomLiquidGlass` 且 `zenGlassCoverage != frameOnly` 时启用；对 `CabinetWClass` 设置 DWM backdrop `none` 与 SWCA accent；对子窗口拦截 `WM_ERASEBKGND` 和常见 `WM_CTLCOLOR*`，并把 `SysListView32` 背景设为 `CLR_NONE`。
*   **风险边界**：TP 原版会更宽地处理 Explorer 进程中的相关子窗口。本地实现必须限制在 `CabinetWClass` 根窗口树下，避免误伤桌面、任务栏或同进程其它 shell 窗口。设置变化和卸载时要清理子类化与 accent，防止透明状态残留。
*   **设置规范**：文件管理器设置键名必须和任务栏/开始菜单统一，使用 `bgColorMode`、`blurPreset`、`opacityPreset`、`luminosityPreset`、`cornerRadiusPreset`。旧 `zenGlassColorMode` / `zenGlassBlur` / `zenGlassOpacity` / `zenGlassLuminosity` / `zenGlassCornerRadius` 只能作为源码内隐藏兼容键读取，不能要求用户必须跑部署脚本迁移；源码要支持用户直接在 Windhawk 新建 mod 后粘贴使用。
*   **验证规范**：同步源码后保持 `LibraryFileName=""`，由 Windhawk 源代码页保存/编译。若仍不透明，下一步优先看 `SetWindowCompositionAttribute` 是否成功、`DwmSetWindowAttribute` 是否被官方 backdrop 再次覆盖，以及目标子窗口是否确实位于 `CabinetWClass` 根窗口树下。

### 2.46 文件管理器 TP 宿主透明层需要链接 GDI32
*   **问题场景**：接入 TP 式子窗口透明处理后，Windhawk 编译通过但链接失败，报 `undefined symbol: __declspec(dllimport) SetBkMode` 和 `undefined symbol: __declspec(dllimport) GetStockObject`。
*   **深层根源**：`SetBkMode()`、`GetStockObject()` 属于 GDI32 API。旧 TP 源码的 `@compilerOptions` 包含 `-lgdi32`，而当前官方 File Explorer Styler 基座原本不需要这些函数，接入 TP 子窗口 `WM_CTLCOLOR*` 处理后漏补链接库。
*   **解决方案**：在 `local@zen-fileexplorer-acrylic.wh.cpp` 的 `@compilerOptions` 中加入 `-lgdi32`。这只是链接依赖修复，不改变透明逻辑。
*   **维护规范**：以后从社区/旧版 mod 移植 Win32 绘制或窗口背景处理代码时，必须同步检查 `@compilerOptions`，尤其是 `gdi32`、`user32`、`uxtheme`、`dwmapi` 这类链接库，避免“源码编译过、链接阶段失败”的误判。

### 2.47 文件管理器 `0/0` 透明不能使用 TransparentBlurBehind
*   **问题场景**：文件管理器 mod 编译并加载后，用户设置 `blurPreset=0`、`opacityPreset=0`，但整窗显示为一整块青绿色，而不是透明背景。
*   **深层根源**：此前把 `blur=0` 映射到 `ACCENT_ENABLE_TRANSPARENTBLURBEHIND`，即使 `GradientColor` alpha 为 0，也可能让系统 accent/colorization 底色显露，形成整块色彩洗底。这和用户语义里的“0 = 透明”不一致。
*   **解决方案**：`blurPreset=0 && opacityPreset=0` 时严格走 TP 原版 `Clear` 语义：先 `ClearAccent()`，再把 DWM backdrop 设为 `DWMSBT_NONE`，不再设置任何 SWCA material。只有 `blur>0` 时才使用 `ACCENT_ENABLE_ACRYLICBLURBEHIND`；`blur=0 && opacity>0` 才用 `ACCENT_ENABLE_GRADIENT` 作为纯色填充。
*   **维护规范**：ZenDesktop 统一语义里 `0` 必须表示尽量透明，不能偷偷映射到系统材质或 accent 洗色。任何模块的 `blur=0/opacity=0` 都要优先走清除背景路径，而不是用“透明材质”兜底。

### 2.48 文件管理器青色洗底后回到首次成功的 TP 小模块基线
*   **问题场景**：m417z File Explorer Styler 基座接入 TP 宿主透明层后，用户仍看到整窗青绿色洗底。源码与 Windhawk 平台哈希一致、DLL 已加载，说明问题不是未同步或未编译，而是当前 XAML Styler 基座仍有资源/目标样式在内容区绘制主题色。
*   **对比结论**：用户提到“第一次成功但还不能调清晰度”的版本，对应 `v3.4.0` 包内 34KB 的 TPGoFighting 风格小模块，而不是 228KB 的 m417z XAML Styler。该基线通过 `SetWindowCompositionAttribute`、DWM backdrop、`CabinetWClass` 子类化、`SHELLDLL_DefView` / `SysListView32` / WinUI bridge 擦背景实现透明，没有大批 XAML 主题规则，因此不会产生 Styler 主题色洗底。
*   **解决方案**：`local@zen-fileexplorer-acrylic.wh.cpp` 回到 TP 小模块基线，并只补最小能力：版本升到 `3.5.2`；识别 `CustomLiquidGlass`、`LiquidGlass`、`WindowGlass`、`Translucent Explorer11` 等旧/新主题值；新增 `zenGlassCoverage=fullWindow/frameOnly`；`fullWindow` 自动打开文件列表、导航树和命令栏透明；`blurPreset=0 && opacityPreset=0` 严格走 Clear。
*   **维护规范**：文件管理器全透明优先保留 TP 透明机制，不要再把 m417z 大 XAML Styler 作为默认基座。若未来需要更复杂的 UI 样式，只能一项一项补到 TP 基线上，且每次都先验证 `0/0/fullWindow` 不再出现青色、黑色或白色洗底。

### 2.49 教程包 ExplorerBlurMica 的完整清背景模型
*   **问题场景**：回到 TP 小模块后仍无法复现教程包的透明效果。用户提供 `E:\Download\文件资源管理器透明附教程`，其中 `ExplorerBlurMica.dll` 可通过 `register.cmd` 注册并配合 `config.ini` 生效。
*   **对比结论**：ExplorerBlurMica 不是单纯的 SWCA/DWM 透明层。DLL 字符串和项目 README 均显示它还处理 DirectUI `Element::PaintBackground`、WinUI VisualTree/XAML Diagnostics、`HomeViewRootGrid`、`DetailsViewControlRootGrid`、`GalleryRootGrid`、地址栏和命令栏背景。此前 Windhawk 小模块只处理宿主窗口和少量子窗口，缺少这些真正清掉内容区洗底的层。
*   **解决方案**：File Explorer mod 新增 `ExplorerBlurMica` 外部透明引擎模式，默认优先查找教程目录、`C:\Release`、`D:\Release` 等位置的 `ExplorerBlurMica.dll`。找到后不调用 `DllRegisterServer`，而是直接 `LoadLibrary + DllGetClassObject + IClassFactory::CreateInstance` 在当前 Explorer 进程里挂载；同时根据 Windhawk 设置写入同目录 `config.ini`，保持 `clearAddress=true`、`clearBarBg=true`、`clearWinUIBg=true`、`showLine=true`。
*   **维护规范**：外部引擎加载成功时不要再同时运行内置 TP fallback，避免两套 hook 同时改同一批窗口和 XAML/DirectUI 表面。内置 TP 只作为找不到 `ExplorerBlurMica.dll` 或用户手动选择 `BuiltInTP` 时的备用方案。

### 2.50 文件管理器透明重构为单文件内置引擎
*   **问题场景**：用户明确要求不要再走 TP 小模块路线，也不要依赖 `ExplorerBlurMica.dll` 外部引擎；目标是一个 Windhawk `.cpp` mod 直接解决文件管理器透明，并且设置项数量接近任务栏/开始菜单。
*   **处理方案**：`local@zen-fileexplorer-acrylic.wh.cpp` 重写为 `v3.6.0` 单文件内置引擎，删除 `explorerEngine`、`externalEnginePath`、`externalEffect`、`DllGetClassObject`、外部 DLL 查找和配置写入逻辑。新实现保留一个 Explorer-only 的清背景核心：DWM/Accent 宿主 backdrop、WinUI 资源透明覆盖、GDI/DirectUI 背景绘制过滤，并且所有过滤都限制在 `CabinetWClass` 窗口树内。
*   **设置规范**：设置压缩为 `theme`、`bgColorMode`、`blurPreset`、`opacityPreset`、`luminosityPreset`、`textColorMode`。旧的 `zenGlassCoverage`、外部引擎路径和教程 effect 不再被源码读取；用户即使直接在 Windhawk 新建 mod 粘贴源码，也不需要运行部署脚本迁移设置。
*   **验证记录**：使用 Windhawk 自带 `clang++` 完成 `-fsyntax-only` 预检和 DLL 链接试编译；`git diff --check` 通过。开发同步仍保持 `LibraryFileName=""`，由 Windhawk UI 源代码页保存/编译。

### 2.51 文件管理器透明全面移植的第一步是把 XAML Diagnostics 接进单文件 mod
*   **问题场景**：`v3.6.0` 单文件版即使增加了更多 GDI/Theme 绘制过滤，本质上仍然主要工作在 Explorer 宿主窗口和普通子窗口层；这不足以可靠清掉 Windows 11 文件管理器内容区的 WinUI/XamlIslands/Composition 表面。
*   **结构对齐**：ExplorerBlurMica 的公开工程虽然缺少 `DirectUITweaker.cpp`、`visualtreewatcher.cpp`、`module.cpp` 等关键 `.cpp`，但 `ExplorerBlurMica.vcxproj`、`tapsite.h`、`visualtreewatcher.h`、`HookDef.h` 明确暴露了它的核心层次：`InitializeXamlDiagnosticsEx`、`TAPSite`、`VisualTreeWatcher`、`IVisualTreeService3`、`IXamlDiagnostics`。
*   **处理方案**：在 `local@zen-fileexplorer-acrylic.wh.cpp` 内直接加入 `DllGetClassObject` / `DllCanUnloadNow` 导出、`ZenDesktopTAPSite`、`VisualTreeWatcher` 和 `InitializeXamlDiagnosticsEx` 调用，让当前 Windhawk mod 自己充当 TAP DLL，而不是再依赖外部 `ExplorerBlurMica.dll`。回调里优先对 `HomeViewRootGrid`、`DetailsViewControlRootGrid`、`GalleryRootGrid`、`Navigation*`、`CommandBar*`、`Address*` 等命名表面及其子树执行透明背景清理，并保留原有 DWM/Accent/GDI 兜底路径。
*   **维护规范**：只要运行 `dev-windhawk-mod.ps1` 注入源码，`LibraryFileName` 就会被清空，Explorer 里的旧 DLL 会失效；调试视觉行为时必须在 Windhawk 源代码页重新保存/编译一次，然后再重启 Explorer。否则看到的界面不是新源码运行结果。

### 2.52 文件管理器重新切回官方 Windows 11 File Explorer Styler 基座
*   **问题场景**：用户最终明确要求停止维护自定义透明引擎，直接采用 Windhawk 官方社区中已经能稳定做出透明效果的 `Windows 11 File Explorer Styler`。诉求不是再单独造一套效果，而是把它默认调成“整窗全局应用”。
*   **处理方案**：`local@zen-fileexplorer-acrylic.wh.cpp` 直接覆盖为官方 `windows-11-file-explorer-styler.wh.cpp` 源码，再进行本地薄改：`@id` 改回 `zen-fileexplorer-acrylic`，名称改为 ZenDesktop 外壳，版本升到 `3.7.0`。默认 `theme` 改为 `Translucent Explorer11`，`backgroundTranslucentEffect` 改为 `acrylic`，`backgroundTranslucentEffectRegion` 改为 `entireWindow`，`xamlDiagnosticsHandling` 改为 `block`。
*   **兼容规范**：为了兼容旧 ZenDesktop 注册表值，`GetSelectedTheme()` 增加旧值映射：`CustomGlass` / `WindowGlass` -> `WindowGlass`，`CustomLiquidGlass` / `LiquidGlass` -> `LiquidGlass`，`Mica` / `MicaAlt` -> `MicaBar`。这样用户已有设置不会因为切换回官方基座而掉到 `None`。
*   **编译兼容**：官方源码中多处把 lambda 直接传给 `CreateThread`、`SetWindowsHookExW`、`EnumWindows`、`RunFromWindowThread`、`CreateThreadpoolTimer`。在当前 Windhawk clang/MinGW 环境下这些写法不能直接通过，所以必须改成显式 `WINAPI` / `CALLBACK` 函数。`StartStatsTimer()` 在本地版中保留函数壳但禁用实际 stats 提交，同时从 `Wh_ModInit()` / `Wh_ModUninit()` 去掉调用，避免本地调试时引入额外网络行为。

### 2.53 文件管理器设置页中文化并补自定义整窗玻璃参数
*   **需求场景**：用户希望文件管理器设置页像任务栏/开始菜单一样更直观，至少常用项要中文化，并能直接调“模糊程度”，而不是去高级设置手写 `styleConstants`。
*   **处理方案**：将 `theme`、`backgroundTranslucentEffect`、`backgroundTranslucentEffectRegion` 等常用设置改为中文标题与中文选项显示；同时新增 `zenCustomBlurAmount`、`zenCustomTintOpacity`、`zenCustomTintLuminosityOpacity` 三个简单参数。默认 `theme=CustomGlass`，在本地版中将它解释为“自定义整窗玻璃”。
*   **实现方式**：不重写官方风格引擎，而是沿用 `WindowGlass` 主题的目标规则，在 `ProcessAllStylesFromSettings()` 阶段动态覆写 `Background`、`MainContentBG`、`Background2` 三个 style constant，把它们生成为 `WindhawkBlur` 画刷。这样 `BlurAmount` 和透明度参数直接由普通 Windhawk 设置项驱动，`TintOpacity=0` 时就能做到尽量全透。

### 2.54 文件管理器 v3.8.0 — 做减法修复五大 Bug
*   **问题场景**：v3.7.0 的文件管理器 mod 存在 5 个问题：(1) 视觉风格很多不起作用；(2) 背景材质名称（亚克力）与其他 mod（自定义玻璃）命名不统一；(3) 玻璃颜色仅在"主页/快速访问"生效，切换到普通文件夹后失效；(4) 自定义模糊强度不生效；(5) 玻璃亮度雾化不生效。
*   **根因分析**：
    *   **问题 3/4/5 同根因**：`g_themeWindowGlass`（`CustomGlass` 复用它）的 `targetStyles` 缺少 `Grid#DetailsViewControlRootGrid` 和 `StackPanel#DetailsViewThumbnail > Grid` 两个目标。`ProcessAllStylesFromSettings()` 把 WindhawkBlur 画刷注入 `$MainContentBG` 常量，但该常量只在 `Grid#HomeViewRootGrid` 生效。导航到普通文件夹时，`DetailsViewControlRootGrid` 保持系统默认不透明背景，遮住了 DWM 层的 acrylic 和 XAML 层的 blur brush。Translucent Explorer11 和 LiquidGlass 主题不受影响，因为它们都有 `DetailsViewControlRootGrid → Transparent` 目标。
    *   **问题 1**：11 个视觉风格中大多来自上游社区未经验证，做减法砍到只保留 4 个（无/自定义玻璃/整窗通透/液态玻璃）。
    *   **问题 2**：其他三个 mod 用"自定义玻璃 (Custom Glass)"，文件管理器用"自定义整窗玻璃 (ZenDesktop Custom Glass)"和"亚克力 (Acrylic)"，不统一。
*   **修复方案**：
    *   `g_themeWindowGlass.targetStyles` 添加 `Grid#DetailsViewControlRootGrid → Background:=$MainContentBG` 和 `StackPanel#DetailsViewThumbnail > Grid → Background:=$MainContentBG`。注意必须用 `$MainContentBG` 而非 `Transparent`，否则 CustomGlass 注入的 WindhawkBlur 画刷不会应用。
    *   `theme` 设置选项从 11 → 4，`GetSelectedTheme()` 中砍掉废弃分支，加兜底回退。
    *   `CustomGlass` 中文名从"自定义整窗玻璃"改为"自定义玻璃 (Custom Glass)"，与其他 mod 统一。
    *   隐藏并删除 `backgroundTranslucentEffect` 与 `backgroundTranslucentEffectRegion` 设置（已硬编码为 acrylic + entireWindow，避免暴露给用户增加困惑）。
*   **版本升级**：`3.7.0` → `3.8.0`。

### 2.55 视觉风格主题选项重命名 (直观反映命令栏表现)
*   **需求场景**：虽然做减法砍到了 3 个核心主题，但由于主题英文原名为 `CustomGlass` / `Translucent Explorer11` / `LiquidGlass`，其原中文命名 “自定义玻璃” / “整窗通透” / “液态玻璃” 依旧无法让用户直观地看出每个主题在“资源管理器命令栏 (Command Bar) / 工具栏”上的不同渲染行为，从而引发混淆。
*   **重命名方案**：
    *   **自定义玻璃 (CustomGlass)**：修改为 `去文留图`，因为其真实渲染行为是隐藏命令栏上的文字标签，只保留图标。
    *   **整窗通透 (Translucent Explorer11)**：修改为 `原版全透`，因为其真实渲染行为是保持 Windows 11 原版默认样式（图标和文字均保留），但整个窗口背景透明。
    *   **液态玻璃 (LiquidGlass)**：修改为 `无图留文`，因为其真实渲染行为是隐藏命令栏上的所有图标，只保留文字标签。
*   **修改范围**：同步修改 `local@zen-fileexplorer-acrylic.wh.cpp` 的 `$options` 及其设置说明文案，以及 Python 调色面板 GUI (`ZenDesktopCustomizer.py`) 里的选项映射字典，保证界面文字完全一致。

### 2.56 修复资源管理器列表表头与边框不透明的 Bug
*   **问题场景**：用户在“去文留图” (CustomGlass) 或“原版全透” (Translucent Explorer11) 模式下，将模糊和色彩浓度调至 0 时，发现虽然文件列表大面积实现了完全透明，但**列标题栏 (ListView Header - 名称/修改日期/类型/大小)** 仍带有系统默认的固色背景（如黑底），且列表外部带有粗黑色矩形外框，侧边栏与主窗口交界处也存有黑色分割线。
*   **解决方案**：
    *   **XAML 样式注入**：在 `g_themeWindowGlass` 和 `g_themeTranslucent_Explorer11` 的 `targetStyles` 中，将 `Grid#DetailsViewControlRootGrid` 以及 `StackPanel#DetailsViewThumbnail` 相关的 `BorderThickness` 设为 `0`、`BorderBrush` 设为 `Transparent`；同时，对 XAML 列表表头元素 `ListViewHeaderItemPresenter`、`ListViewHeaderItem` 和 `GridViewHeaderItemPresenter` 增加目标样式覆写，使它们的 `Background`、`BorderBrush` 设为 `Transparent`，`BorderThickness` 设为 `0`。
    *   **系统主题资源覆盖**：在 `themeResourceVariables` 中注入关键的主题画刷覆盖值（如 `ListViewHeaderItemPresenterBackground`、`ListViewBorderBrush`、`NavigationViewBorderBrush`、`SplitViewPaneBorderBrush` 等）并统一设置为 `<SolidColorBrush Color="Transparent"/>`，彻底阻断 WinUI 系统资源把不透明固色或默认分割线线色重新渲染在列表表头、列表外框与导航侧边栏分界线上。

### 2.57 引入底层 UxTheme API 挂钩解决 Win32 嵌套控件透明失效问题
*   **问题场景**：虽然在 2.56 中加入了 XAML 样式注入 and 资源覆盖，但文件列表（Details View）下部的 SysHeader32（名称/修改日期/大小）列标题栏以及左右内容区与侧边栏的分割线（Splitter）在很多 Windows 11 系统版本下依然顽固显示为黑底或黑色粗实线。
*   **深层根源**：
    1.  **Win32 宿主嵌套隔离**：资源管理器中的列表内容区和分割线在底层并不是由 WinUI 3 直接绘制，而是经典的 Win32 窗口（如 `SysHeader32` 窗口类）或 DirectUI 元素被桥接嵌套在 XAML 容器内。
    2.  **主题引擎直接绘制**：这些 Win32 控件会越过 XAML，直接调用系统的 `uxtheme.dll` 来绘制其表头背景和边框线。因此，在 XAML 层覆盖背景或者覆盖 XAML 主题资源对它们完全无效。
*   **解决方案**：
    *   在 C++ 模组内直接拦截并挂钩（Hook）`uxtheme.dll` 导出的核心渲染 API：
        1.  **句柄跟踪（高DPI适配）**：Hook `OpenThemeData`、`OpenThemeDataForDpi`（用于多监视器 DPI 适配环境下的主题获取）和 `CloseThemeData`，在哈希表中记录被创建的 `HTHEME` 句柄以及对应的类名列表（如 `Header`、`ListView`、`Splitter`）。如果仅挂钩 `OpenThemeData` 会因为 DPI 适配重定向而漏掉高DPI环境下的主题句柄注册。
        2.  **阻断背景渲染**：Hook `DrawThemeBackground` 和 `DrawThemeBackgroundEx`。当检测到当前绘制是来自于 `Header` 的列背景（非排序箭头等关键交互图元）或 `Splitter`（分割线）、或列表（`ListView`/`TreeView`）的容器外边框（Part 0）时，模组直接拦截请求并返回 `S_OK`。
    *   通过让主题引擎“不绘制背景”，成功使传统的 Win32/DirectUI 列标题栏和分割线变为完全透明，与外层的毛玻璃/全透材质完美融合。

### 2.58 设置页失焦变实色：HostBackdrop 焦点回退 + 搜索框“遮盖层”经验迁移（v5.2.0）
*   **问题场景**：`SystemSettings.exe` 自定义玻璃在**获得焦点**时 HostBackdrop 玻璃正常；**失去焦点**后整窗退回不透明实色。用户要求一直透明，并提示开始菜单曾经修过“焦点/状态”问题。
*   **对照开始菜单（笔记 2.14，搜索框）**：
    *   开始菜单修的不是整窗失焦，而是**搜索框**在 Normal 态被内部 `Border` 纯白实色盖住，活动/Disabled 态才露出底下玻璃。
    *   **可迁移原则**：“看起来失焦变实色”经常是 **(A) 上层/下层实色遮盖** 或 **(B) 玻璃材质自身进入 Fallback**，两类要分开治。
    *   搜索框答案是 **(A)**：强制 `SearchBoxToggleButton` 内层 `Border` → `Transparent`。
*   **设置页根因（B 为主）**：
    1.  旧路径主面板用 `AcrylicBrush BackgroundSource=HostBackdrop`：焦点时正常，**inactive 时系统强制 FallbackColor**。
    2.  早期把 chrome 换成 **本地** `CreateBackdropBrush` 的 WindhawkBlur 会整窗不透明——那是因为本地 blur 只磨窗口内部实色，采不到桌面。
    3.  开始菜单主背景用 WindhawkBlur 且 `ShouldUseFallback` **不看焦点**，所以不会失焦变实色。
    4.  **禁止**对 Settings 使用 `ACCENT_POLICY` / `DwmExtendFrameIntoClientArea(-1)`：会冻输入。
*   **v5.2.0 第一轮**：仅 FORCE_ACTIVE + NCACTIVATE（ExplorerBlurMica 模式），chrome 仍 Acrylic HostBackdrop。
*   **v5.3.0 全力多层（见 2.59）**。

### 2.59 设置页 always-glass 多层火力（v5.3.0）
*   **目标**：失焦也一直透明/玻璃，不惜叠多层；不碰浅色模式大改。
*   **层 A — 解锁 host 采样**：`DwmSetWindowAttribute(DWMWA_USE_HOSTBACKDROPBRUSH=TRUE)` + `DWMSBT_NONE` 去 Mica。
*   **层 B — chrome 改 host WindhawkBlur**：
    *   `XamlBlurBrush::CreateEffectBrush` **优先** `Compositor.CreateHostBackdropBrush()`，失败再回退 `CreateBackdropBrush`。
    *   `BuildChromeGlassBrush` 生成 `<WindhawkBlur BlurAmount="28" .../>`（无 FallbackColor → 不因失焦 solid-fill）。
    *   解释旧“WindhawkBlur 不透明”：用的是 **local** BackdropBrush；host 采样才是开始菜单同款。
*   **层 C — 强制活动外观**：`WCA_FORCE_ACTIVEWINDOW_APPEARANCE` + `WM_NCACTIVATE=TRUE` + `WM_ACTIVATE/WINDOWPOSCHANGED/SHOWWINDOW` 再刷 + **1.5s 定时器** re-assert（防系统偷改）。
*   **层 D — 遮盖层全杀**（2.14 原则）：扩展 `themeResourceVariables`（Mica/Acrylic/NavigationView 底板等）与更多 Grid/Border → Transparent；内容区仍铺 `$CommonBgBrush` 防左右不对称实色。
*   **安全边界**：
    *   仍 **禁止 ACCENT_POLICY / ExtendFrame**。
    *   subclass / DWM 属性只打 **本进程** HWND。
    *   卸载 / 非玻璃主题 `RemoveZenGlassFocusFix` + 关 host backdrop。
*   **验证清单**：
    1. Windhawk 编译显示 `5.3.0`，日志应有 `CreateHostBackdropBrush OK` 与 `focus-fix ON`。
    2. 设置有焦点：玻璃正常。
    3. 点桌面失焦：仍透/仍玻璃，不是灰/白实心。
    4. 调 Opacity/Blur 仍 soft-reload 有反应。
*   **版本**：`5.2.0` → `5.3.0`。

### 2.60 v5.3.0 CreateHostBackdropBrush 导致设置页完全不透明（紧急回退 v5.3.1）
*   **现象**：用户反馈 5.3.0 后「连透明都不是」——有焦点也不透。
*   **根因（代码层）**：把主 chrome 从已验证的 `AcrylicBrush HostBackdrop` 换成 `WindhawkBlur + CreateHostBackdropBrush`。在 `SystemSettings.exe` 上 host Composition 采样**并没有**给出桌面玻璃。
*   **回退（5.3.1）**：主面板恢复 HostBackdrop；禁止再试 WindhawkBlur 主 chrome。

### 2.61 「调什么都没用 + 完全不透明」= DLL 丢失 / 未编译（部署致命坑）
*   **现象**：用户改 Opacity/Blur/主题全无反应，设置页像系统原版实色。
*   **实机诊断**：
    *   源码 ModsSource 已是 `5.3.1`，但注册表：
      `LibraryFileName=local@windows-11-settings-styler-fork_5.1.2_111220.dll`，`Version=5.1.2`。
    *   `Engine\Mods\64\` 里**该 DLL 文件不存在**（部署脚本清缓存时删掉了）。
    *   `SystemSettings` 进程只加载了 `windhawk.dll`，**没有** settings-styler 的 mod DLL → 样式引擎完全没跑。
*   **根因**：部署只同步了 `.wh.cpp` 并删除旧 DLL，但**没有**触发 Windhawk 重新编译，也没有把 `LibraryFileName` 指到新 DLL。引擎按注册表找旧文件 → 加载失败 → 静默无效果。
*   **修复**：
    1. 用 Windhawk 自带 clang 手动编译出  
       `local@windows-11-settings-styler-fork_5.3.1_manual.dll`。
    2. 写入 `Engine\Mods\64\`。
    3. 注册表 `LibraryFileName` / `Version` 更新为 `5.3.1`。
    4. 重启 `SystemSettings`；`tasklist /m` 确认已注入 `...5.3.1_manual.dll`。
*   **部署铁律**：
    *   **禁止**只删 `Engine\Mods\64\*settings-styler*.dll` 而不编译。
    *   清 DLL 时必须同时把 `LibraryFileName` 置空 `""`，并提示用户在 Windhawk UI **保存/编译**；或脚本直接 clang 编出新 DLL 再写回注册表。
    *   「调参无反应」优先查：`LibraryFileName` 文件是否存在、进程是否加载了对应 DLL，而不是先改 XAML。
*   **版本**：引擎侧已挂载 `5.3.1_manual`（2026-07-25）。
*   **用户确认**：自行在 Windhawk UI 编译后，**有焦点透明已恢复**。说明 5.3.0 时期“完全不透”主要是 **DLL 未加载**，不是用户环境永久坏了。

### 2.62 失焦强化 v5.3.2（你编译，我不代编）
*   **前提**：主 chrome 继续 HostBackdrop（有焦点已验证）。
*   **改动**：强制活动加强 + 误用了「同色半透明 Fallback wash」。

### 2.63 失焦变「颜色预设纯色」（极光幻青）— Fallback 着色叠层（v5.3.3）
*   **现象**：有焦点透明正常；失焦后整窗变成用户选的 **极光幻青** 等纯色。
*   **5.3.3 尝试**：`FallbackColor=#00000000` + `AlwaysUseFallback=False` — **用户反馈仍纯色**，说明 OS inactive 配方不只听 FallbackColor，仍会按 Tint 喷实色。

### 2.64 失焦主动切「清空 chrome」brush（v5.3.4）
*   **策略**：不再指望 HostBackdrop 失焦仍玻璃。
    *   **有焦点** `g_zenWindowLooksActive=true`：`BuildChromeGlassBrush` → HostBackdrop Acrylic（已验证能透）。
    *   **失焦** `g_zenWindowLooksActive=false`：同一函数返回 `<SolidColorBrush Color="#00000000"/>`，卡片路径同样清空。
    *   `WM_ACTIVATE` / `WM_ACTIVATEAPP` 翻转标志 → 50ms 后 `SoftReloadStylesKeepingTap()` 重套样式。
*   **预期**：失焦不再出现极光幻青实心；可能只是「全透/空」，模糊只在聚焦时有。
*   **用户操作**：编译 **5.3.4** → 关开设置 → 测失焦。

### 2.65 开机任务栏只透条子、托盘/音量仍原生（v4.1.0）
*   **用户确认**：Windhawk 里把任务栏 mod 关开一次立刻好，不必重启 Explorer。
*   **根因**：`InitializeSettingsAndTap()` 用 `g_initialized.exchange(true)` 锁死，开机 AfterInit 时托盘 island 还没建完。开始菜单已有 `TaskbarCreated` 全量重试，任务栏没有。
*   **截图**：深色圆角图标墙是溢出/文件夹弹出层，和托盘实底是同一类「后建出来的表面没挂上 TAP」。
*   **音量**：用户说开关**任务栏** mod 就好，说明是托盘上的 Control Center 按钮，不是 `ShellExperienceHost` 的 Win+A 面板。所以 v4.1.0 没改通知中心。
*   **修复**：`ReapplyTaskbarStyles()` 就是关开一次的同一条路径。`Wh_ModAfterInit` 后 2.5s `ScheduleBootStyleReinit`；另开隐藏窗口听 `TaskbarCreated`（ExplorerBlurMica 崩完重拉 Explorer 也要重挂）。
*   **验证**：冷启动，等约 3 秒，托盘/音量应变透明。日志应有 `Boot delayed TAP reinit`。

### 2.66 最大化层并回任务栏且连续 max/min 不崩（v4.1.0）
*   **基线**：`D:\Download\ZenDesktop_OneKeyDeploy_v4.0.0_Online-32054bd01e29\local@zen-taskbar-acrylic.wh.cpp`（v3.5.0）已有 `ZenTaskbarRuntimeBgBrush` + 最大化设置。仓库 v4.0.0 砍了 hook，只留了死变量。
*   **崩点**：v3.5.0 在 AfterInit 线程上 `SetWinEventHook(LOCATIONCHANGE)`，回调里 `RunFromWindowThread` = **SendMessage** 进 Explorer UI。连续放大等于高频重入 XAML。
*   **拼法（ponytail，两块现成的）**：
    *   检测：`taskbar-background-helper` 的独立消息泵线程（OUTOFCONTEXT 事件打到这个线程，不打 UI 线程）
    *   涂色：v3.5.0 的 `ZenTaskbarRuntimeBgBrush` / `PropagateStyleVariableChange`，最大化时 `max(base, layer)`
    *   跳 UI：`SendMessageTimeout(..., SMTO_ABORTIFHUNG, 200)`，卡住就跳过这一帧
*   **明确不用**：helper 的 `SetWindowCompositionAttribute`（Win11 黑底）；`UninitializeSettingsAndTap` 挂 WinEvent。
*   **部署**：`deploy.ps1` 把 helper 设 `Disabled=1`。两套同时开会黑底。
*   **验证**：连续最大化/还原 10 次，任务栏应变厚、Explorer 不重启、Toolkit 不报不稳定。

### 2.67 ExplorerBlurMica 开机第一次打开文件管理器重启 Explorer
*   **根因**：BHO `CObjectWithSite` 构造函数同步 `OnWindowLoad()` → MinHook + 全量 API hook，发生在第一个柜员窗口构造中。失败则 `MessageBox` 或 `FreeLibraryAndExitThread`。
*   **源码修复**（`ExplorerBlurMica_Source/`）：
    *   构造函数空；`SetSite` 成功后 `CreateThread` + Sleep(800) 再 hook
    *   `MH_Initialize` 允许 `MH_ERROR_ALREADY_INITIALIZED`
    *   `_HookError_` 不再弹窗；失败不杀线程
*   **二进制未更新**：本机无 MSBuild。`ExplorerBlurMica/ExplorerBlurMica.dll` 仍是旧文件（工作区里这份还从 427KB 变成过 219KB，是另一次替换，不是这次补丁）。不重编 DLL，用户开机第一次打开照样崩。
*   **编译**：`ExplorerBlurMica_Source/ExplorerBlurMica.sln` x64 Release → 覆盖 `ExplorerBlurMica/ExplorerBlurMica.dll` → 管理员 `register.cmd`。或跑 `.github/workflows/build_dll.yml`。
*   **验证**：注册新 DLL 后冷启动，第一次点文件管理器不应再重启 Explorer。按住 ESC 仍可跳过加载（原作者逃生）。

### 2.68 调查约束（ponytail）
*   用户要求所有代码先对照 https://github.com/DietrichGebert/ponytail：先读再写、复用现成、禁止新架构。
*   本轮没有新检测框架、没有重做文件管理器引擎、没有把 helper 的 Win32 涂色搬到 Win11。

### 2.72 多显示器：一屏最大化，所有任务栏都变厚（v4.1.2）
*   **根因**：`g_taskbarMaximizedLayerActive` 是进程级 OR。任一显示器有最大化窗口就给**所有** XAML 线程换 `ZenTaskbarRuntimeBgBrush`。
*   **修**：按窗口所在 `HMONITOR` 分别判断，`thread_local` 决定刷子。每次刷新都更新所有任务栏岛（不能只在全局 OR 翻转时刷，否则 A 屏最大化后 B 屏再最大化/还原不会单独更新）。
*   **4.1.2 仍失败**：主副任务栏经常在**同一条 UI 线程**、各有 XamlRoot。`thread_local` 一次赋值会把该线程上所有岛刷成同一种。4.1.3 对每个 `StyleVariableState` 用 AutomationPeer 屏幕矩形定位显示器，再单独设 `ZenTaskbarRuntimeBgBrush`。

### 2.71 设置页浅色太透看不清、失焦变纯色、优化差（调查，2026-08-21，未改代码）
*   **用户**：浅色也能透，但太透看不清字；失焦变回纯色；整体优化不好。禁止 Translucent Windows。
*   **浅色看不清**：默认 `opacityPreset=0` + `luminosityPreset=0`，浅色 Tint 是 `#FFFFFF`、TintOpacity `0.00`。玻璃层几乎没衬底，浅色主题的深色字叠在壁纸上。`textColorMode` 默认不改字色。卡片 `#18FFFFFF` 在浅色里几乎看不见。深色主题浅字+深壁纸反而能读。
*   **失焦纯色**：v5.3.4 **故意**在 `WM_ACTIVATE` inactive 时把 chrome/cards 换成 `SolidColorBrush #00000000`，再 `SoftReloadStylesKeepingTap`。这会拆掉 HostBackdrop，露出系统 Mica/浅色底板 → 就是「纯色」。若没编 5.3.4，则 HostBackdrop inactive 按 Tint 喷实色（2.63）。两条路用户看到的都是实色，不是玻璃。
*   **优化差**：每次焦点变化都 SoftReload 整套 TAP；聚焦时还每 500ms `EnumWindows` 重刷 DWM。切窗口会顿。
*   **5.4.0 已改（2026-08-21）**：失焦不再换 `SolidColorBrush`、不再 SoftReload；一直 HostBackdrop + 始终 FORCE_ACTIVE / NCACTIVATE TRUE。去掉 500ms EnumWindows。浅色 `opacityPreset<42` 自动加到 42 白霜。用户须 Windhawk 编译后关开设置。
*   **5.4.1**：主页「试用 Microsoft 365」白卡片不是 `EntityItem`，走 `TwoSegmentsHeroUserControl` / `ReactNativeExperienceViewControl` / `SettingsCard`。已接到同一套 `$CardBgBrush`。
*   **5.4.2**：用户反馈圆角不齐、有的白有的是调色。白雾来自 `CardBackgroundFillColorDefault=#18FFFFFF` 和 `SettingsGroup` 再套一层 `$CardBgBrush`。组容器改 Transparent、Presenter 改 Transparent，条目统一 CornerRadius=8；主题卡色资源清零。
*   **5.4.3 调查（深色正常、浅色不行、选中「主页」橙褐渐变）**：
    *   图2 渐变 = 把 **HostBackdrop 用在 NavigationViewItem 这种小控件** 上，DWM 按控件矩形采样壁纸，选中条变成一条 smear。HostBackdrop 只能铺在整页/整栏，选中项必须用半透明 Solid。
    *   浅色 HostBackdrop + 白 Tint 的系统配方和深色不一样，社区也没有第二套能透 Settings 内容的引擎。
    *   **对照过的其它工具（都不能替代这个 fork）**：
        | 工具 | 作用 | Settings 内容 |
        |---|---|---|
        | Translucent Windows | Win32 DWM | 作者写明不影响 UWP/WinUI |
        | MicaForEveryone | Win32 标题栏 Mica | 不管 SystemSettings 内容 |
        | DWMBlurGlass | DWM 边框/标题栏 | 同上 |
        | 官方 m417z Settings Styler | 本 fork 的上游 | 布局主题，玻璃仍要自己写 HostBackdrop |
        | Rectify11 | 整套主题 | 太重，不单修设置页 |
    *   结论：设置页玻璃只能继续走 SystemSettings TAP。下一刀是禁止 HostBackdrop 进侧栏 item。

### 2.70 配置工具源码丢失，PowerShell 重做（2026-08-21）
*   原 `config-editor-wpf.exe` 是 185MB 自包含 WPF，源码不在仓库/本机/GitHub。
*   新入口：`ExplorerBlurMica/打开配置工具.cmd` → `config-editor.ps1`。
*   注册检测 CLSID 固定 `{B44BD3C8-E597-4E08-AE43-246CE24698E7}`。点注册会弹 UAC 跑 `regsvr32`。

### 2.69 设置页「高级」必须在最后（v4.1.1）
*   用户截图：Control styles / Resource variables 插在最大化层前面。
*   YAML 顺序就是 Windhawk 设置页顺序。把 `styleConstants` / `controlStyles` / `themeResourceVariables` / `xamlDiagnosticsHandling` 全部放到最大化三项之后。

### 2.73 开机/后建溢出栏与缩略图预览框保持原生未插入（v4.2.7）
*   **现象**：开机时任务栏条子透明，但点击托盘向上箭头打开的溢出栏（Overflow Flyout）以及任务栏图标悬浮预览框（Thumbnail Preview）均为 Windows 原生实底，没有插入透明/亚克力效果。
*   **根因**：
    1. **线程初始化短路**：`VisualTreeWatcher::OnVisualTreeChange` 遇到 `!g_initializedForThread` 时直接 `return S_OK;`。由于溢出窗口（`TopLevelWindowForOverflowXamlIsland`）和预览窗口（`TaskListThumbnailWnd` / `XamlExplorerHostIslandWindow`）创建较晚或在特定线程调度下，该线程上的 `g_initializedForThread` 为 `false`，导致其 XAML 元素被全量丢弃未走 `ApplyCustomizations`。
    2. **窗口类名枚举缺失与缓冲区截断**：`GetXamlHostWnds` 中原 `WCHAR szClassName[32]`，而 Win11 溢出栏类名 `TopLevelWindowForOverflowXamlIsland` 长达 35 字符，被直接截断为 31 字符；且原有判断中未包含 `TopLevelWindowForOverflowXamlIsland`、`NotifyIconOverflowWindow`、`TaskListThumbnailWnd` 等类名。
    3. **`OnWindowCreated` 漏捕获**：`DesktopWindowContentBridge` 仅识别了父窗口为 `Shell_TrayWnd`，未识别以溢出窗口/副屏/预览窗口为父级的桥接窗口。
*   **修复**：
    *   `OnVisualTreeChange` 遇到 `!g_initializedForThread` 自动调用 `InitializeForCurrentThread()` 加载当前线程的样式与规则表，杜绝任何未初始化线程抛出的 XAML 元素被漏掉。
    *   扩大缓冲区至 128 字符，并在 `GetXamlHostWnds`、`OnWindowCreated`、`IsIgnoredShellWindow` 中完整支持 `TopLevelWindowForOverflowXamlIsland`、`NotifyIconOverflowWindow`、`TaskListThumbnailWnd`。
    *   版本升至 **v4.2.7**。

### 2.74 设置应用右侧接缝与美化设置混乱全面修复（v5.5.0）
*   **现象**：
    1. 用户反馈侧栏导航项（如「主页」）右侧与内容区分界处存在明显的纵向色差截断/矩形接缝，两边颜色不一致；
    2. Windhawk 设置项选项残缺（缺少多个预设风格）、描述晦涩、红绿蓝字色未生效、高级分类排序混乱。
*   **根因分析**：
    1. **多重 HostBackdrop 叠层倍增与透明度割裂**：`g_themeTranslucentSettings` 原先在 `RootPageGrid`、`PermanentNavigationView`、`RootGrid`、`SplitView#RootSplitView`、`ContentRoot`、`ContentGrid`、`Border#ContentGridBorder`、`Frame#PermanentNavRootFrame`、`ScrollViewer#SettingsPageHostPanel` 等 9 个层级重复赋予 `$CommonBgBrush`，而左栏 `PaneRoot` 又被赋予了 `$PaneBgBrush`（且计算时额外增加了 `chromeOp + 4` 的透明度）。在侧栏导航项右边缘与内容区交界处，左右叠层数量与透明度不同，暴露出底层的色差接缝。
    2. **侧栏选中项直角生硬覆盖**：`Grid#LayoutRoot@Selected` 被赋予矩形 `<SolidColorBrush Color="#28FFFFFF" />`，但 `Grid` 缺乏 `CornerRadius`，选中条右侧呈直角截断切面。
*   **修复方案**：
    1. **单层全窗通透架构**：仅在顶级 `Grid#RootPageGrid` 铺设单层 `$CommonBgBrush`，其余所有中间容器与子面板一律设为 `Background=Transparent`，`PaneBackground=Transparent`，`BorderThickness=0`。消除 `paneOp` 透明度差值，左右全窗完全融为平滑单一玻璃底板。
    2. **侧栏导航项优雅高亮**：`SettingsNavigationViewItem` 设 `Margin=4,2,4,2`，`CornerRadius=6`；`NavigationViewItemPresenter` 悬停/选中统一使用 6px 平滑圆角和半透底色，左侧指示器统一标准胶囊尺寸。
    3. **浅色环境下卡片与展开项发白不透明根因及修复**：
        *   **根因 A**：`ProcessAllStylesFromSettings` 中原存在 `if (lightTheme && opVal < 42) opVal = 42;`，强行锁死浅色模式最低 42% 浓度，导致用户即使选择 0% 浓度也无法完全透明。
        *   **根因 B**：`BuildCardBrush` 在 `blurPreset == 0` 时错误返回了子控件级别的 `AcrylicBrush BackgroundSource="HostBackdrop"`，在 WinUI 浅色模式下会触发底层的白色发光遮罩与多重叠加采样，使卡片直接发白成为纯色实底。
        *   **根因 C**：账户页等页面的 `SystemSettings.View.SettingsExpander`、`ExpanderToggleButton`、`RevealedContent`、`TwoSegmentsHeroUserControl` 内部未被全量清空默认浅色实色画刷。
        *   **修复**：移除 42% 浓度锁死，赋予用户 0%~100% 完全通透控制权；`BuildCardBrush` 在 `blur==0` 时在 `opacity==0` 返回 `SolidColorBrush #00000000`（完全透明），`opacity>0` 时转为纯正带 Alpha 的 `SolidColorBrush`，彻底根除发白；全面覆盖 `SettingsExpander`、`ExpanderToggleButton`、`RevealedContent`、`TwoSegmentsHeroUserControl` 等控件及其子 Grid，并全量注入所有浅色模式相关资源变量清零。
    4. **重构美化设置项与全彩文字对齐**：补全 10 种视觉风格、14 种 Emoji 规范配色预设、层级清晰的浓度/模糊/亮度选项；**文字颜色全面与任务栏/开始菜单等 Mod 对齐**（支持 12 种模式：主题默认、强制纯白、强制深黑、跟随系统强调色、🔴 红色、🟢 绿色、🔵 蓝色、🟡 黄色、🟠 橙色、🟣 紫色、🌸 粉色、💠 青色），并在 C++ 底层同步作用于 `TextBlock`、`FontIcon`、`PathIcon` 与 `ContentPresenter`；高级选项汉化并沉底。
    5. **卡片规范**：统一所有卡片/推荐位圆角为 8px，杜绝 `Expander` 与 `SettingsGroup` 内部嵌套产生的双层白底。
*   **版本**：升级至 **v5.5.0**，通过 Windhawk 自带 Clang 编译生成 DLL 并即时热挂载。

### 2.75 全套 Mod 版本统一至 v4.1.0 & 简介开头注入开发者关注支持与原作者致谢（v4.1.0）
*   **版本统一**：将 ZenDesktop 矩阵下的全部 6 款核心 Mod 统一版本号为 **v4.1.0**：
    1. `ZenDesktop: Desktop Icon Toggle and Auto-Hide` (v4.1.0)
    2. `ZenDesktop: Notification Center Acrylic Styler` (v4.1.0)
    3. `ZenDesktop: Settings Acrylic Styler` (v4.1.0)
    4. `ZenDesktop: Start Menu Acrylic Styler` (v4.1.0)
    5. `ZenDesktop: Taskbar Acrylic Styler` (v4.1.0)
    6. `MacOS Minimize Animation` (v4.1.0)
*   **简介开头规范**：
    *   在每个 Mod 的 `// ==WindhawkModReadme==` 开头置顶注入 Lanboo 个人社交主页与赞助链接：
        *   📺 **B站**：`https://space.bilibili.com/1280632999`
        *   🎵 **抖音**：`https://v.douyin.com/Isl7QPpKZZ4/`
        *   ⚡ **爱发电**：`https://ifdian.net/a/Lanboo`
    *   清晰标明原作者出处（如 `m417z`、`Abdullah Masood & Potassiumuncher`）及 Lanboo 在该 Mod 中所做的深度重构、二开特性与定制优化细节。

---
*此文档由开发者手动维护，AI 辅助整理，旨在帮助开发者和后续维护者快速理解本项目的技术架构与踩坑细节。*
*最后更新：全套 Mod 版本统一为 v4.1.0、注入开发者链接/原作者说明、及 macOS 动画完整中文汉化（v4.1.0，2026-08-24）*



## 2.76 模组名称中文规范与统一命名 (v4.1.0)
已将 ZenDesktop 系列核心模组的显示名称统一规范，保留 ZenDesktop:  前缀并使用清晰标准的中文命名：
1. local@zen-desktop-toggle-icons.wh.cpp -> **ZenDesktop: 桌面双击隐藏图标**
2. local@zen-notificationcenter-acrylic.wh.cpp -> **ZenDesktop: 通知栏美化**
3. local@windows-11-settings-styler-fork.wh.cpp -> **ZenDesktop: 设置美化**
4. local@zen-startmenu-acrylic.wh.cpp -> **ZenDesktop: 开始菜单美化**
5. local@zen-taskbar-acrylic.wh.cpp -> **ZenDesktop: 任务栏美化**
6. local@macos-minimize-animation.wh.cpp -> **macOS 神奇效果最小化动画**


## 2.77 macOS 动画模组 Electron/UWP/DirectComposition 软件启动与还原白屏修复 (v4.1.0)
### 问题原因深度分析：
1. **DirectX/DirectComposition 交换链遮挡失效**：Electron/Chromium 核心软件（如 Antigravity / VS Code / 浏览器）以及 UWP/DirectX 软件（如联想管家 Lenovo Vantage）在启动或从任务栏展开时，如果经历了 DWM Cloaking 遮罩隐藏或分层窗口属性变换，DirectX 11/12 GPU Compositor 会进入遮挡/挂起状态 (DXGI_STATUS_OCCLUDED)。
2. **缺乏帧刷新与强制重绘信号**：动画播放完毕解除遮罩（Uncloak）时，Windows 默认不会主动向硬件加速进程发送 WM_SIZE 或重构信号，导致 GPU Swapchain 没有恢复呈现，画面停留在初始空白/纯白帧。
3. **PW_RENDERFULLCONTENT 截取降级**：新启动软件的首帧在 DirectComposition 尚未就绪时截取可能返回空白，且没有降级方案。

### 修复措施：
1. **后置强制重绘与框架状态刷新 (SWP_FRAMECHANGED & RedrawWindow)**：
   在 MacGenieAnimThread、MacGenieAnimThreadClassic 以及 MacGenieUndoRisingHide、MacGenieLaunchThread 的解除隐藏/还原节点中，统一注入 SetWindowPos(..., SWP_FRAMECHANGED | SWP_DRAWFRAME) 与 RedrawWindow(..., RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME)，强制 DWM 与 Chromium/DirectX 合成器在动画结束瞬间立即重建 Swapchain 并全量重绘。
2. **PrintWindow 动态降级容错**：当 PW_RENDERFULLCONTENT 截取失败时自动回退至标准 GDI 截取，避免传递全白/空缓冲区。


## 2.78 腾讯 ACE 反作弊 (英雄联盟 LOL / 腾讯游戏) 拦截冲突根因与全量排除防护 (v4.1.0)
### 报错现象：
- 打开英雄联盟 (LOL) 或通过 WeGame 启动游戏时弹出：`警告 (3, 1051, 150002) 检测到系统环境存在异常，请关闭不必要的程序，重启机器后再试。` 导致游戏闪退。

### 根因分析：
1. **全局注入被 ACE (Anti-Cheat Expert) 扫描拦截**：
   - 包含 `// @include *` 的全局模组（如 `local@macos-minimize-animation.wh.cpp`）在未指定 `@exclude` 时，Windhawk 引擎会尝试将钩子 DLL 注入系统中创建的全部进程。
   - 当启动英雄联盟客户端（`LeagueClient.exe` / `LeagueClientUx.exe` / `League of Legends.exe`）及腾讯反作弊驱动（`ACE-BASE.sys` / `SGuard64.exe`）时，ACE 的反注入内核驱动检测到非白名单 DLL / 内存钩子，判定为环境异常并阻断启动。

### 解决方案（双层白名单隔离）：
1. **Mod 级 `@exclude` 元数据硬防护**：
   在 `local@macos-minimize-animation.wh.cpp` 元数据中全量加入游戏与反作弊进程排除列表（包括 `LeagueClient*.exe`, `League of Legends.exe`, `RiotClient*.exe`, `wegame.exe`, `SGuard*.exe`, `TenSafe*.exe`, `VALORANT*.exe`, `vgc.exe`, `YuanShen.exe`, `StarRail.exe`, `steam*.exe` 等）。
2. **Windhawk 引擎级全局排除 (`HKLM:\SOFTWARE\Windhawk\Engine\Settings`)**：
   在 `deploy.ps1` 部署脚本和系统注册表中，将 `Exclude` 全局排除项设置为上述游戏进程全名单，并将 `InjectIntoGames` 设为 `0`，确保 Windhawk 核心引擎永远不会触碰或注入任何游戏与反作弊进程。
