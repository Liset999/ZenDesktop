# ZenDesktop 桌面小组件

> 状态：**社区方案优先**。一键部署默认拷贝 **Desktop Live Overlay**（m417z）；自制玻璃卡片 MVP 已降级到 `optional/deprecated/`，不再随 deploy 安装。

## 推荐（请用这个）

### Desktop Live Overlay

| 项 | 内容 |
| --- | --- |
| Windhawk `@id` | `desktop-live-overlay`（本仓库 `optional/local@desktop-live-overlay.wh.cpp`） |
| 作者 | [m417z](https://github.com/m417z) |
| 版本（随仓库打包） | 1.2 |
| 许可证 | **GPL-3.0** |
| 上游 | https://github.com/m417z/my-windhawk-mods （`mods/desktop-live-overlay.wh.cpp`） |
| Windhawk 目录 | https://windhawk.net/mods/desktop-live-overlay |

**做什么：** 在桌面图标**后方**绘制可配置的实时内容（时间、日期、系统指标、天气等），成熟、可维护，比自绘一堆 popup 卡片更接近「系统级小组件」体验。

### 其他推荐（社区 / 外置，不强制打包）

| 方案 | 许可 | 说明 |
| --- | --- | --- |
| [Dynamic Island](https://windhawk.net/mods/dynamic-island)（dixel-dxt） | MIT | 仿灵动岛通知/媒体条；体积大，建议在 Windhawk 目录直接装 |
| [Rainmeter](https://www.rainmeter.net/) + 皮肤 | 程序 GPL-2.0 | 最丰富的桌面皮肤生态；与 Windhawk 并行 |
| ElevenClock 等时钟类 | 各异 | 任务栏时钟增强，不是桌面卡片 |
| PowerToys FancyZones | MIT | 窗口布局，不是小组件 |

灵感对照（产品形态，非代码抄袭）：macOS Widgets 的信息密度与圆角节奏；小米超级背屏式「描述即编排」——若要用自然语言选模块，优先在社区 Overlay / Rainmeter 皮肤上扩展，而不是继续堆自制卡。

## 如何启用（ZenDesktop 部署后）

1. 运行 `deploy.bat`。
2. Windhawk →「已安装的 Mod」→ **Desktop Live Overlay**（`local@desktop-live-overlay`）→ 打开并编译。
3. 在 Mod 设置里配置要显示的内容与布局；需系统「透明度效果」时观感更好。
4. 也可在 Windhawk 搜 `desktop-live-overlay` 装官方包。

## 自制 MVP（已废弃，仅回退）

路径：`optional/deprecated/local@zen-desktop-widgets.wh.cpp`（曾 v0.2.1）。

- 自绘可拖动玻璃卡（时钟/状态/天气演示/日历/电量/倒计时/便签 + 预设/关键词编排）。
- **deploy 不再拷贝**；需要时手动从 `optional/deprecated/` 拷回再部署。
- 保留原因：无社区 Overlay 时的临时回退；**新功能请改社区方案或上游 PR，不要继续扩这套卡片。**

## 与其它 ZenDesktop Mod

| 组合 | 说明 |
| --- | --- |
| 桌面双击隐藏图标 | 可同时开；Overlay 画在图标层后，隐藏图标后内容仍可见 |
| 任务栏音乐与歌词 | 无关；歌词在任务栏 Widgets 槽，不是桌面 Overlay |
| 已废弃自制 Dock | 勿叠用；Dock 请改用 TAI |
