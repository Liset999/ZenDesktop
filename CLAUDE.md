# ZenDesktop 项目开发与部署规范

- 2026-11-07 歌词 0.5.18 天气失败后以请求结束时间为起点每 60 秒重试，定位失败同样覆盖，成功后恢复 weatherRefreshMin。保留上次成功天气，配置变更仍立即获取。`check_lyrics_weather_retry.py --compiler-dir E:/Tools/Windhawk/Compiler` 抽取实际调度表达式验证 28 项通过，完整 stdin 语法通过（既有 winsock2 include 顺序警告）、metadata 与 diff 检查通过。仅修改仓库，尚未部署和实机断网验收。

- 2026-11-07 用户进一步确认仅重启Explorer挂钩快，重启机器挂钩慢，必须区分两种启动场景。用户12:31日志中新Explorer PID18992：46.635 ModInit、46.644 AfterInit、48.269首个任务栏桥接窗口、49.131 watcher构造、49.152开始收到BackgroundFill、49.280托盘anchor、49.628任务栏anchor、51.428健康检查无需重试；无订阅失败、80070490/8000FFFF或full reapply。该日志验证Explorer热重启恢复正常，无法据此判定整机开机延迟原因，仍需整机启动阶段日志。

- 2026-11-07 用户重启后反馈任务栏4.4.4仍先显示系统底色再透明，启动延迟目标尚未达到。已从5.0.0 Offline发布包提取任务栏4.3.0源码对比：Wh_ModInit/AfterInit首轮入口、InitializeForCurrentThread与尺寸引擎Init/AfterInit一致，不能把延迟直接归因于4.4新增定时器。本机已安装源码与仓库4.4.4一致，注册DLL为4.4.4（11:59:49编译），Explorer始于12:23:56，Windhawk服务Running/Auto，尺寸引擎两项均0，LoggingEnabled=0。本次未取得启动时序日志，未修改加载逻辑或运行设置。后续需记录ModInit、窗口创建、TAP注入、订阅及首个背景应用时序，再区分服务注入延迟与XAML样式应用延迟。

- 2026-11-07 任务栏启动日志确认：11:44:49.990 ModInit，52.687 TaskbarCreated 全量重挂，52.957 watcher 建立、52.976 又撤销，随后483次80070490；11:45:00.756 的8秒补挂后托盘anchor出现，20秒检查健康。该日志无8000FFFF，不能认定弹窗同源。任务栏4.4.4在TaskbarCreated保留已发起的TAP初始化，旧watcher停止后忽略回调，worker创建前持引用，异步订阅失败先清除初始化标记并走一次有界重试，失败再提示重启。check_taskbar_startup.py与完整stdin语法、metadata通过；实机延迟与偶发弹窗仍待编译验收。

- 2026-11-07 歌词 0.5.17 针对侧栏像素偏移增加几何约束。旧竖向布局把全高 Start／LaunchBand 的底边当开始按钮底边，或把全高 TrayNotifyWnd 的顶边当托盘边界，最终 clamp 后偏移失效。开始区仅避让上半栏内的紧凑矩形，全高托盘走原后备定位。`python tools/tests/check_lyrics_vertical_offset.py --compiler-dir <Compiler目录>` 直接抽取源码定位块，234 项检查旧代码 72 失败、修改后全部通过，完整 stdin 语法与元数据通过。当前会话无法读取实际任务栏窗口，Win26H2 实际窗口树与视觉效果尚未确认，修复仅在仓库源码。

## 核心设计铁律（MUST ALWAYS FOLLOW）

### Windhawk 源码加载与编译约束（2026-11-05）

- `WindhawkMod` 元数据区只能放 `// @字段 值`，普通说明注释放到区块外；运行 `python tools/tests/check_mod_metadata.py` 验证。
- Windhawk 将源码作为标准输入交给编译器，不能依赖 `ModsSource` 中的相邻头文件。窗口动画内嵌游戏保护头，修改 `optional/zen-game-guard.h` 后运行 `python tools/sync_animation_guard.py` 同步。
- 使用 `python tools/tests/check_animation_stdin.py --compiler-dir <Windhawk的Compiler目录>` 从空临时目录验证完整源码语法与内嵌头同步；该检查不生成 DLL、不启用 Mod，不能代替实际动画验证。
- 动画窗口仅可在任务栏有 `WS_EX_TOPMOST` 时插到任务栏后面；以普通任务栏作 `hWndInsertAfter` 会把动画降到非置顶层，导致恢复动画被前台软件遮挡。用 `tools/tests/check_animation_zorder.py --compiler-dir <Compiler目录>` 在不显示的独立桌面上验证两种任务栏状态；需要桌面创建权限，不触碰用户窗口。
- 首次最小化不得等待 UIA 任务栏按钮查询；显示/获得焦点时后台预查询，未完成时采用缓存/几何估算。`tools/tests/check_animation_cold_lookup.py --compiler-dir <Compiler目录>` 用慢提供器验证入口不阻塞、在途请求去重与缓存回填；不要把该测试的耗时当作实际动画延迟。
- 透明材质的屏幕捕获只能作为优先路径，不能作为动画播放的硬性前提。1.6.8 把截图不可用直接变成跳过动画，导致多个日常软件动画消失；1.6.9 恢复窗口所属线程上的 `PrintWindow` 回退（不跨线程强制调用）。`check_animation_backdrop_capture.py` 必须覆盖截图被遮挡、还原无缓存仍保留捕获路径；测试通过不等于真实材质已视觉验收。
- 2026-11-06 实机只读探针发现 `DesktopAudioVisOverlay_local@zen-desktop-widgets` 被动画的整窗矩形遮挡判断视为 blocker。不能据此直接忽略所有透明覆盖窗口，需确认实际绘制/遮挡区域。设置使用 XAML `HostBackdrop` AcrylicBrush、`DWMSBT_NONE` 和 CoreWindow 保持激活逻辑；文件管理器走控件透明加 DWM/Accent。两者不可仅以 `SYSTEMBACKDROP_TYPE >= 2` 判断材质是否存在。
- 设置最小化的外层 HWND 属于 `ApplicationFrameHost.exe` / `ApplicationFrameWindow`，内容 CoreWindow 属于 `SystemSettings.exe`；本机探针已观察到最小化宿主和 shell-cloaked 的独立 CoreWindow。合成截图偏好要识别宿主内的 Settings CoreWindow，不能只匹配目标 HWND 的进程名。不要把仅支持设置的 DWM 属性读取失败后的零值当成实际状态。
- 1.6.8 的连续采样重写离屏测试更快，但用户实机反馈更卡，1.6.10 已撤回并恢复之前的 Direct2D 神奇效果。`check_animation_macgenie.py` 对原渲染器只做离屏非空画面与耗时检查；旧连续采样器的逐像素测试不能用于声称原渲染器质量通过。后续必须测实际呈现帧间隔、对比真实窗口画面，再决定是否保留优化。
- 用户已多次反馈动画回归：禁止继续叠加未经实机验证的“性能优化”或把透明捕获失败变为禁用动画。优先恢复已知行为，再单独记录材质捕获问题；不要把一个维度的测试通过概括为全部问题已解决。
- 2026-11-06 对比发布包确认 5.1.0 使用动画 1.6.3、5.2.0 使用 1.6.12；后者再次带入 `!composedWindowSnapshot` 的 PrintWindow 回退限制。只读探针读到 Chrome 的 `DWMWA_SYSTEMBACKDROP_TYPE=2`、游戏排除结果为 false，说明 Chrome 也会进入材质分支。1.6.13 删除该限制，捕获回归由 10 项失败变为 0；尚未部署或实际播放验收。后续打包前必须运行 `check_animation_backdrop_capture.py`，不能只查版本号或更新日志。
- 2026-11-06 用户实机确认 1.6.13 的 Chrome 动画恢复，但文件管理器/设置动画背景为纯色。1.6.14 按窗口用途区分：`RequiresShellComposedWindowSnapshot` 仅匹配 Explorer、Settings 及其宿主，并设为 `screenOnlyCapture`，失败时原生过渡；其他材质窗口（含 Chrome）保留 PrintWindow 回退。新回归覆盖两类窗口最小化、关闭、无缓存还原，修改前 9 项失败、修改后 0 项失败。现场读取文件管理器截图资格通过、设置宿主被 Chromium 类窗口遮挡，仅能证明探测时状态，不能断言最小化瞬间 blocker 已确认。当前 1.6.14 仅仓库修复，未实机播放验收。


1. **Deploy 职责边界**：
   - `deploy.bat` / `deploy.ps1` **只负责把 Mod 源码和元数据装到 Windhawk 里面**。
   - **绝对不负责编译，绝对不负责开启！**
   - 所有部署的 Mod（核心 Mod 与可选 Mod）在初次部署时，默认状态必须全部为 **关闭状态（Disabled = 1）**。
   - 严禁通过脚本在后台强行开启任何 Mod（除非命令行参数显式指定）。
   - 严禁通过脚本触发自动编译或后台强行编译。
   - 必须将选择权、开启权、编译权完全交由用户在 Windhawk UI 界面中按需自行开启与确认编译。
   - 若用户此前已在 Windhawk 中手动调整过开关状态，重新部署时必须保留用户的现有状态。

2. **全中文本地化规范**：
   - 所有引入或更新的 Mod 必须包含完整的中文本地化元数据：
     - `// @name:zh-CN`
     - `// @description:zh-CN`
     - 完整的 `// ==WindhawkModSettings==` 中文翻译（`$name:zh-CN`, `$description:zh-CN`）。
   - 禁止在 UI 或控制台中向普通用户展示未翻译的英文设置项。

3. **文件管理器透明（v4.2）**：
   - 产品路径是 Windhawk mod `local@zen-explorer-styler`，**不要**再引导用户安装或注册 ExplorerBlurMica。
   - `deploy.ps1` **不得**重新注册 EBM，也不得静默篡改用户机器上残留的 EBM 注册（可提示用户自行运行 `tools/uninstall-ExplorerBlurMica.bat`）。
   - 注意 XAML 诊断接口：同一时间只能有一个使用者。styler 与残留的 EBM / 其他 Styler 类 mod 可能互相干扰；用户侧应卸掉 EBM 后再用 styler。

- 2026-11-06 用户反馈 1.6.14 文件管理器/设置展开为 macOS、最小化为 Windows 原生，目标仍未实现。25 秒只读采样（`.diagnostics/animation-transition-probe.log`）观察文件管理器最小化前截图资格通过，故不能把全部问题归因于桌面遮挡。外部采样无法确认钩子内的瞬时截图失败/入口拦截/会话冲突，下一步需临时启用 Mod 日志获取失败原因后再改逻辑；禁止把原生回退称为完成修复。

- 2026-11-06 用户日志确认 1.6.14 最小化失败 blocker 为无 owner 的 `DesktopAudioVisOverlay_local@zen-desktop-widgets`。同 HWND 只读探针得到窗口 4800×1910、有效 region (473,1540)-(725,1590)，旧检查因 owner=null 拒绝读取 region。1.6.15 移除 owner 前提，仅对该精确类名读取有效窗口 region；无 region 仍保守整窗检查。独立桌面无 owner 回归修改前透明外框错误阻挡、修改后通过，真实音柱与普通前景窗继续阻挡。材质回归、stdin 语法、metadata 均通过，最终动画效果待用户验收。

- 2026-11-06 1.6.15 用户日志仍为同一 blocker。只读实测 `PhysicalToLogicalPointForPerMonitorDPI` 对目标 HWND 转换音频区域端点返回 0，导致区域再次丢失。1.6.16 将 overlay region 映射改为复用 `BuildTaskbarCoordinateTransform(target, overlay)` 与 `MapPhysicalTaskbarRectToAnimationSpace`，避免依赖目标窗外点转换。回归用失败的点转换桩与真实矩形映射函数复现红/绿，透明外框放行、真实重叠仍阻挡；stdin、材质、元数据检查通过。尚待用户管理员同步源码并编译实机验收。

- 2026-11-07 用户新日志确认设置最小化 blocker 为 `ZenImeCaretHintOverlay`，非音频层。输入法提示为真实短暂绘制气泡，不能盲目当透明窗口忽略。动画 1.6.17 对该 blocker 通过注册消息 `ZenDesktop.DismissCaretHintForCapture.v1` 请求协作隐藏（50ms有界等待，只尝试一次），确认后 DWM 同步并重新检查所有遮挡；输入法提示 0.2.5 在 overlay 线程处理消息、清除 pending/focus 提示并隐藏。源文件仍为 `zen-ime-caret-hint-0.2.4.wh.cpp`，内部版本0.2.5。独立桌面消息测试与音频区域测试通过，两个完整源码语法、材质捕获与元数据通过。两Mod均待同步编译，Chrome本轮还无失败日志，不能断言其原因相同。

- 2026-11-07 用户确认设置与文件管理器在1.6.17/输入法提示协作修复后恢复，Chrome仍无macOS动画。Chrome尚未取得本轮失败日志，保留单独排查。当前LoggingEnabled=1；附加DbgViewMini读取器启动后立即退出，未取得任何日志，不能据此认定Chrome没有注入或没有调用动画。

- 2026-11-07 Chrome用户确认操作无动画Mod日志。只读检查主进程PID27236始于11月6日23:10:16，无helper角色；动画1.6.17启用且日志开启，显式排除无Chrome，mod-status无该PID任何记录。进程模块查询未取得可用模块证据，不能把空输出称为已证实未注入。下一步用户正常退出并重开Chrome，验证新主进程加载/日志，避免继续修改捕获逻辑；设置与文件管理器已获用户实机确认恢复。

- 2026-11-07 用户确认正常退出并重新打开Chrome后macOS动画恢复。本轮Chrome问题与旧主进程加载状态相关，重启应用即可恢复；无模块枚举证据，不进一步断言具体注入失败机制。设置、文件管理器、Chrome均已有用户实机恢复反馈。后续应用操作完全无Mod日志时，优先检查进程生命周期与Mod加载，再判断捕获逻辑。

### 音频条验证约束（2026-11-05）

- 2026-11-07 用户反馈 OS26 液态玻璃/紧凑版在 26H2 浅深色、显式模糊/云母下首次应用内容文字不可见。Explorer styler 1.7.10 移除 `ExtTextOutWithAlpha` 对 `options=0` 的错误早退。离屏真实 GDI 回归 `tools/tests/check_explorer_text_alpha.py --compiler-dir E:\Tools\Windhawk\Compiler` 修改前普通文字三色 alpha 均为零，修改后黑/白/自定义色 × 普通/裁剪/字形/不透明裁剪共12组通过，完整源码 stdin 语法和元数据通过。此证据确认普通文字 alpha 漏处理，尚不能确认用户26H2首次应用全部症状已解决。当前仅仓库源码修复，未部署、未实际画面验收。

- 用户已明确要求音频可视化优先采用 Windhawk 社区现成 Mod，不再从零开发音频引擎。先用社区原版完成真实桌面/任务栏显示验证，再考虑中文本地化或集成。当前候选首选 Tourne'Table；不要把下载/静态核对称为安装或实际显示通过。

- 内容刷新调用 `UpdateLayeredWindow` 时使用空目标位置指针，位置由布局阶段管理；禁止把分层子窗口的 `GetWindowRect` 屏幕坐标再次当作更新位置传入。
- 桌面音频条必须按所选显示器的可用区域放置，不得直接贴整个虚拟桌面的底边。
- 布局回归必须使用带 Windows 8/10 兼容清单的真实分层子窗口，以及非零坐标的父窗口；普通子窗口测试会漏掉分层呈现导致的漂移。
- DSP/像素/位置测试通过，只能报告对应验证结果；未经 Explorer 中实际观察，不得声称最终画面已确认无闪烁或已可见。

### 小组件 3.0 维护入口

- 面向普通用户的音频颜色使用中文预设下拉菜单，不要求输入 # 十六进制色值；保留底层键和值以兼容已有配置。
- 桌面音频应与时钟、日历共用顶部「拖动摆放」「重置位置」：默认鼠标穿透、位置锁定，只在拖动开启时移动/缩放；透明且无窗口边框，不恢复上游独立面板材质。音频设置参考任务栏歌词，常用项在前，外观与高级分组在后，调整键路径必须迁移保留旧值。
- Windhawk 启动程序可能是 32 位，Explorer 是 64 位；统一 Mod 必须声明 x86 与 x86-64 两个架构，安装脚本将全部架构用 `|` 连接。只编译 x64 会导致时钟正常但音频启动器不加载。shell 实现仅在 `_WIN64` 编译，音频与分流需分别验证两种架构。
- 用户已确认原版 Tourne’Table 的独立面板与任务栏可见，并明确要求融合进同一个小组件 Mod。生成文件是 `optional/local@zen-desktop-widgets.wh.cpp`；开发源在 `dev/widgets/`，使用 `python tools/build-desktop-widgets.py` 重建，不直接改生成区块。
- 音频引擎快照保持原始字节与 SHA256，适配集中在生成器和生命周期桥接；保留上游 MIT 署名/许可证以及整合文件 GPL-3.0。
- 任务栏覆盖窗口的区域发布（`SyncTaskbarOverlayRegion`、`g_taskbarOverlayRegion`、`WM_WINDOWPOSCHANGED`/`WM_DESTROY` 里的三处挂钩）必须留在 `tools/build-desktop-widgets.py` 的生成补丁里。它决定动画 Mod 的截图遮挡判断是否把音柱窗口当 blocker；曾只存在于生成文件，一旦重生成就会静默丢失。改动后用 `python tools/tests/check_animation_overlay_region.py --compiler-dir <Compiler目录>` 验证。
- 任务栏音频的左右位置是「像素偏移 + 上游百分比」：主设置项 `audio.taskbarOffset`（`position.taskbarOffsetX`）只在 `hostTaskbar` 生效，0 时不得改变原位置；不要改成绝对像素定位，否则旧配置会跳动。
- 不操作用户 Windows 界面，不修改运行中的 Windhawk 设置，不自动安装/启用诊断 DLL；手动验收由用户完成。用户本轮授权的是仓库代码整合，尚未授权发布当前未验收整合版。
