# ZenDesktop 桌面小组件（架构）

> 状态：**MVP 脚手架**（v0.1.0）。不是 Rainmeter 克隆；先落地可扩展的宿主与两块示例卡片，再迭代真亚克力、插件化与更多组件。

## 目标

在桌面上放置可拖动的玻璃风小组件（时钟、状态卡等），外观尽量贴近 ZenDesktop 其它玻璃面板，且：

- **默认关闭**：作为 `optional/` 可选 Mod，`deploy` 只装源码，不自动开启、不自动启动。
- **可扩展**：宿主管窗口、拖拽、位置持久化与设置；具体卡片是独立绘制单元。
- **轻量**：注入 `explorer.exe` 的 Windhawk Mod，不另起常驻 exe（MVP）；后续若需要独立进程，见下方「演进」。

## 目录与入口

| 路径 | 作用 |
| --- | --- |
| `optional/local@zen-desktop-widgets.wh.cpp` | MVP 宿主 + 时钟 / 状态卡 |
| `docs/desktop-widgets.md` | 本文 |
| `docs/nexus-dock.md` | 下一阶段 Nexus 式 Dock 草图（本轮仅文档） |

## 架构（MVP）

```
explorer.exe
  └─ ZenDesktop Widgets Mod (Windhawk)
        ├─ Settings（Windhawk 设置页）
        │     showClock / showStatus / fontSize / clockFormat …
        ├─ WidgetHost
        │     创建/销毁窗口、定时刷新、读注册表位置
        ├─ ClockWidget     →  HH:mm + 日期
        └─ StatusWidget    →  简短状态文案（可关）
```

### 窗口模型

- 每块卡片一个 `WS_POPUP` 工具窗口：`WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE`。
- MVP 用 `SetWindowCompositionAttribute`（Acrylic blur behind）做半透明深色玻璃；文字用 GDI 画在客户区。
- 左键拖动客户区移动；松手后写入 `HKCU\Software\ZenDesktop\DesktopWidgets\`（`ClockX/Y`、`StatusX/Y`）。
- 不抢焦点、不进任务栏；资源管理器重启后由 Mod 重新创建窗口。

### 设置键（Windhawk）

| 键 | 含义 | 默认 |
| --- | --- | --- |
| `showClock` | 显示时钟 | 开 |
| `showStatus` | 显示状态卡 | 开 |
| `fontSize` | 主文字大小（像素） | 28 |
| `clockShowSeconds` | 时钟显示秒 | 关 |
| `statusText` | 状态卡自定义文案（空则用内置提示） | 空 |

## 如何试用

1. 运行 `deploy.bat`（或已部署则重新跑一次，只会补文件）。
2. Windhawk →「已安装的 Mod」→ 打开 **ZenDesktop: 桌面小组件** → 确认编译。
3. 桌面右上角附近应出现时钟与状态卡；拖到喜欢的位置。
4. 在 Mod 设置里开关卡片、改字号；改完立即生效。

## 已知限制（诚实说明）

- Acrylic 依赖系统「透明度效果」；关透明时退回半透明纯色底。
- 尚无插件市场、网页组件、天气/CPU 传感器、多显示器独立布局。
- 与「桌面双击隐藏图标」可并存；小组件窗口不是桌面图标 ListView。
- 某些动态壁纸 / Wallpaper Engine 场景下 z-order 可能需后续调整（WorkerW 挂接）。

## 演进路线

1. **v0.x**：本 MVP + 修玻璃与多显示器。
2. **v1**：卡片接口（尺寸、刷新间隔、设置页片段）；天气 / 电量等内置卡。
3. **可选独立 Host exe**：若注入 explorer 不稳定，再拆出 `zen-widgets.exe` 托盘宿主，Mod 仅负责与桌面协调。
4. **Dock**：见 [nexus-dock.md](nexus-dock.md)，与小组件共用玻璃绘制与拖拽基础设施更合适。

## 非目标（本阶段不做）

- Rainmeter 皮肤兼容、Lua/网页任意皮肤市场。
- 替代 Windows 11 自带小组件板。
