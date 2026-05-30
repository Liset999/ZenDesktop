# ZenDesktop - 开发者备忘录与技术沉淀

这份文档记录了 ZenDesktop Mod 的架构解析、各个核心文件的作用，以及在开发过程中特别是针对 Windows API 及 Windhawk 环境踩过的坑和技术解决方案。

## 1. 核心文件解析

当前目录下的文件构成了 ZenDesktop 美化套件的核心：

### Windhawk Mods (Core Injection Code)
*   **`local@zen-taskbar-acrylic.wh.cpp`**
    *   **Role**: Taskbar transparency and custom visual styling mod.
    *   **Tech**: Injected into `explorer.exe` taskbar component. Overrides `TrayNotifyWnd` / `ReBarWindow32` default background rendering to apply acrylic/blur effects.
    *   **Current version**: v2.7.0
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
*   **`ZenDesktopCustomizer.py`**
    *   **作用**：提供一个可视化的 UI 工具（基于 Python 构建），允许用户快速微调各组件的参数（如透明度、模糊强度），而无需直接修改 C++ 源码。
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

---
*此文档由开发者手动维护，AI 辅助整理，旨在帮助开发者和后续维护者快速理解本项目的技术架构与踩坑细节。*
*最后更新：v3.3.0（2026-05-30）*


