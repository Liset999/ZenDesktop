# ZenDesktop Nexus 式 Dock（草图）

> 状态：**仅文档**。需求池项「像 Nexus 的 Dock」；本轮不实现代码。

## 产品意向

一条可放在屏幕边缘（默认底部居中）的快捷应用栏：

- 玻璃底板，风格与 ZenDesktop 任务栏 / 开始菜单一致。
- 固定常用应用图标；点击启动 / 聚焦。
- 悬停放大（可选）、运行中指示点。
- **默认关闭**；与系统任务栏可并存（用户自行藏任务栏或保留）。

## 与现有组件的关系

| 组件 | 关系 |
| --- | --- |
| 任务栏美化 | 美化系统任务栏；Dock 是**另一条**快捷栏，不替代任务栏注入。 |
| 桌面小组件 | 可复用玻璃绘制、拖拽、位置持久化。建议 Dock 作为同一 optional 宿主的第二种「壳」，或独立 `optional/local@zen-dock.wh.cpp`。 |
| 任务栏高度 / 图标大小（暂不打包） | 调的是系统任务栏；Dock 图标尺寸单独设置。 |

## 推荐实现路径（未开工）

1. **探测**：`Shell_TrayWnd` 矩形，避免与任务栏重叠；多显示器选主屏或鼠标所在屏。
2. **窗口**：与小组件相同的 popup + Acrylic；`WS_EX_TOPMOST` 可选（默认跟普通窗口 z-order，或钉在桌面层）。
3. **图标来源**：用户在设置里添加 `.lnk` / 路径；MVP 可先读 `%APPDATA%\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar` 作只读预览（不写系统 pinned）。
4. **启动**：`ShellExecuteEx`；已运行则 `AllowSetForegroundWindow` + 还原。
5. **动画**：悬停缩放可后做；先保证点击与拖整条 Dock 换边（底/左/右）。

## 风险

- 与「任务栏美化」同时开时，视觉上两条栏可能打架——设置里要写清「建议二选一或拉远间距」。
- 全屏游戏应自动隐藏（`SHQueryUserNotificationState`）。
- 不在本阶段做 macOS 风格「从哪来回哪去」最小化轨迹（已放弃的大蓝图项）。

## 验收草案（将来实现时）

- [ ] optional Mod，deploy 默认关。
- [ ] 至少能钉 3 个应用并启动。
- [ ] 位置与钉选持久化。
- [ ] 中文设置与作者头符合 `docs/MOD_STYLE_GUIDE.md`。
