# ZenDesktop Nexus 式 Dock

> 状态：**社区方案优先**。一键部署默认拷贝成熟社区 Mod **TAI (taskbar as island)**；自制 MVP 已降级到 `optional/deprecated/`，不再随 deploy 安装。

## 推荐（请用这个）

### TAI — Taskbar as Island（Windows 11）

| 项 | 内容 |
| --- | --- |
| Windhawk `@id` | `taskbar-dock-like`（本仓库文件名 `optional/local@taskbar-dock-like.wh.cpp`） |
| 作者 | [DarkionAvey](https://github.com/DarkionAvey) |
| 版本（随仓库打包） | 1.5.247 |
| 许可证 | 基于 **GPL-3.0**（与 ZenDesktop 同许可证族；版权归原作者） |
| 上游 | https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed |
| Windhawk 目录 | https://windhawk.net/mods/taskbar-dock-like |

**做什么：** 把系统任务栏改成居中、浮起、可动画的「岛式」Dock，**保留原生任务栏能力**（开始、搜索、托盘、跳转列表等），比从零写一条 popup 启动栏靠谱得多。

**与 ZenDesktop 的关系：**

- 任务栏美化（亚克力主题）仍可叠用；若视觉冲突，先关任务栏美化的「Dock 风格主题 / 空白穿透」，或按 TAI 文档调间距。
- **不要**再开已废弃的「ZenDesktop: Dock」自制 MVP（两者目标重叠）。

### 其他可选（不随包 vendoring）

| 方案 | 许可 | 说明 |
| --- | --- | --- |
| [Aero Dock](https://github.com/kawaxi/Aero-Dock) | MIT | 独立程序，仿 macOS Dock；不走 Windhawk |
| [Cairo Shell](https://github.com/cairoshell/cairoshell) | Apache-2.0 | 完整桌面 shell 替换，比 Dock 重 |
| RocketDock / ObjectDock 等经典 | 各异 | 老方案，Win11 兼容性自行评估 |

## 如何启用（ZenDesktop 部署后）

1. 运行 `deploy.bat`（会按哈希只更新变化的文件；可选 Mod **默认关**）。
2. 打开 Windhawk →「已安装的 Mod」→ 找到 **TAI (taskbar as island) for Windows 11**（`local@taskbar-dock-like`）→ 打开并等待编译成功。
3. 也可在 Windhawk 社区直接搜 `taskbar-dock-like` 安装官方包（不必用本仓库 vendored 副本）。

## 自制 MVP（已废弃）

路径：`optional/deprecated/local@zen-dock.wh.cpp`（v0.1.0）。

- 自绘底部玻璃 popup 启动栏，**不替代**任务栏；功能远弱于 TAI。
- **deploy 不再拷贝**；仅作对照 / 应急。若要手动装：把该文件拷回 `optional/` 再跑一次 deploy，或在 Windhawk 里「从本地文件创建」。
- 原因：社区已有成熟、可维护的方案，不应平地起高楼。

## 明确不做（本阶段）

- 默认关 / 强制替换 Windows 任务栏为不可逆方案。
- 打包滚轮调音量、任务栏高度/图标大小类 Mod。
