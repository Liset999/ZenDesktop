# ZenDesktop 桌面小组件 3.0.5

音频可视化正式整合为小组件内的一项功能，采用 Tourne’Table 2.1.1 的上游引擎。用户只需一个 `zen-desktop-widgets` Mod，不需要另外安装 Tourne’Table。时钟、日历、资源占用保留原实现。

## 使用与升级

1. 如果先前单独安装过 Tourne’Table，先在 Windhawk 关闭它，避免同时出现两个音频进程。更新前也暂时关闭旧版「桌面小组件」，编译完成后再开启；已有设置会保留。
2. 按常规运行项目部署脚本；只想更新小组件时，可在仓库根目录以管理员 PowerShell 运行：

   ```powershell
   powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\update-desktop-widgets.ps1
   ```

3. 在 Windhawk 重新打开「ZenDesktop：桌面小组件」的编辑页，手动编译。更新脚本仅备份并安装源码、元数据及缺失设置，不自动编译或开启 Mod。
4. 在同一设置页启用音频可视化。默认任务栏预设为 24 根圆角绿蓝渐变音柱，宽度 3、间距 3、最大高度 22，位置 25% / 50%，60 FPS。
5. 显示位置可选「任务栏」「桌面小组件（透明）」「桌面图标后方」，同时显示在一处。桌面推荐把音柱高度调至 100–140。「桌面小组件」适合动态壁纸，保留原面板的缩放和位置记录，去掉窗口边框、系统材质和绘制底色。
6. 平时固定且鼠标穿透；打开页面顶部「拖动摆放」才可拖动桌面音频，拖角可缩放。关闭后锁定并保存位置，任务栏不受拖动开关影响。「重置位置」也清除桌面音频的拖拽记录。

时钟高级设置继续位于页面最后，24 小时制仍在时钟主设置中。

音频设置参考任务栏歌词：先放启用、显示位置、形状、高度，再列「颜色与外观」，最后「高级设置」（显示器、位置、灵敏度、平滑、帧率）。3.0.1 的相关设置迁移到新分组，保留显式零值及已有选择。

3.0.3 将渐变起始、结束颜色改为中文下拉菜单，可选绿、天蓝、青、蓝、紫、粉、红、橙、金黄、白、灰。两端选择相同颜色即为纯色，不需要填写十六进制色值；底层颜色键与取值不变。

3.0.5 在「显示位置」下面加了「任务栏左右位置（像素）」：只在任务栏模式生效，按像素在「水平位置（%）」的基础上左右平移，负数向左、正数向右，并限制在任务栏范围内，不会把音柱推出屏幕。任务栏图标、搜索框或状态文字挤在一起时，用它把音柱挪到空白处；值为 0 时位置和以前的版本完全一致，旧的百分比设置继续有效。

3.0.4 让任务栏音频窗口只占据音柱实际绘制的区域（`SetWindowRgn`），不再是覆盖整个虚拟桌面的空窗口，避免窗口动画的截图判断把这块空白当成遮挡。

## 运行结构

- 同一 Mod ID 同时匹配 `explorer.exe` 和 `windhawk.exe`。
- 同时生成 x86 与 x86-64：本机 Windhawk 启动程序为 32 位，Explorer 为 64 位。3.0.0 仅声明 x86-64，导致音频启动器没有加载；3.0.1 补齐双架构。部署元数据也必须保留两项架构。
- Explorer 只执行时钟、日历、资源小组件的生命周期；不在 Explorer 初始化音频引擎。
- Windhawk 中的启动器按「启用音频可视化」开关启动上游的独立 tool-mod 进程。关闭音频只停止这个进程，不停止时钟等组件。
- 音频的中文设置通过桥接映射到上游原有字段。未展示的功能保留固定上游默认值；上游英文快捷菜单默认关闭，避免绕开中文设置页。
- 对 Windows 11 24H2 的挂载做有限适配：当壁纸位于 Progman 的 WorkerW 子窗口时，音频放在该 WorkerW 之上、图标之下。这个整合适配尚需在真实 Wallpaper Engine 环境验收。

## 设置迁移

部署只填充缺失的新键，保留已有时钟设置、音频开关及用户已保存的新设置。旧音柱数量、颜色、灵敏度与桌面高度转换到新引擎；旧 `both` 位置改为任务栏，因为上游一个实例对应一个位置。旧键保留用于回退。

## 开发与来源

```powershell
python -m pip install PyYAML
python tools/build-desktop-widgets.py
python tools/tests/check_widgets_integration.py --compiler-dir 'E:\Tools\Windhawk\Compiler'
python tools/tests/check_widgets_build.py --compiler-dir 'E:\Tools\Windhawk\Compiler'
python tools/tests/check_widgets_audio.py --compiler-dir 'E:\Tools\Windhawk\Compiler'
powershell -NoProfile -File tools/tests/check_widget_settings.ps1
```

- `dev/widgets/desktop-core.wh.cpp`：时钟等组件的维护源。
- `dev/widgets/audio-settings.yaml`：公开的中文音频设置与默认预设。
- `dev/widgets/lifecycle.cpp`：单 Mod 的进程角色分流。
- `dev/widgets/panel-host.cpp`：透明桌面窗口的边框、材质与鼠标穿透适配；音频捕获和渲染仍沿用上游。
- `dev/widgets/vendor/`：固定 SHA256 的 Tourne’Table 原版源码、来源信息及 MIT 许可证。
- `optional/local@zen-desktop-widgets.wh.cpp`：可独立安装和发布的生成文件，无私有头文件或 Python 运行依赖。编辑开发源后必须重新生成，不要直接修改生成区块。

上游来源：[Tourne’Table 社区源码](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/tourne-table-desktop-audio-visualizer.wh.cpp)，作者 USER// TOURNE，音频实现沿用其代码；上游进一步致谢 Salyts、GR0UD。MIT 授权全文嵌入生成源码，并随发行文件保留。时钟部分包含 m417z 的 GPL-3.0 代码，因此整合文件与 ZenDesktop 项目整体按 GPL-3.0 分发。

## 验收边界

3.0.2 的 x86 / x86-64 实际 DLL 链接、入口导出、生命周期分流及原版 FFT/设置桥接回归均通过。隐藏测试窗口验证了透明窗口样式、锁定时穿透、开启拖动后的移动/角落命中，以及任务栏隔离；迁移验证了分组后颜色、数量、位置和显式零值保留。

原版及 3.0.1 整合版的任务栏、面板音频已由用户确认显示；截图中的旧面板有系统底色。3.0.2 的透明效果、实际拖动缩放、位置保存与桌面层级仍需用户在 Windhawk 编译加载后验证，隐藏窗口及 FFT 测试不能替代视觉验收。

3.0.5 的左右位置只在任务栏宿主上生效：真实 DLL 链接、双架构生命周期分流、设置桥接、FFT 回归、透明窗口与鼠标穿透、任务栏窗口区域这几项均有自动化验证；音柱在真实任务栏上落在哪里、是否真的避开图标，仍要由用户在 Windhawk 编译加载后确认。
