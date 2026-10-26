# ZenDesktop Mod 性能说明

本页记录 `fix/v-next-explorer-context-panel` 上对各 Windhawk Mod 的热路径优化（行为与默认开关不变）。

## 原则

- 缓存昂贵查找（`EnumWindows` / `FindWindow` / `OpenProcess`→进程名 / GDI 对象）
- 合并风暴式重挂（`WM_TASKBARCREATED` 等）
- 文案/状态未变时不 `InvalidateRect`
- 不改视觉效果；可选 Mod 仍默认关

## 按 Mod

| Mod | 版本 | 要点 |
|-----|------|------|
| 文件管理器美化 | 1.7.1 | `GetTargetWnds` 短缓存 |
| 设置美化 | 4.2.6 | Settings CoreWindow HWND 缓存 |
| 任务栏美化 | 4.2.9 | Tray/UI HWND 缓存；TaskbarCreated 500ms 防抖 |
| 开始菜单美化 | 4.1.11 | CoreWindow 缓存；TaskbarCreated 500ms 防抖 |
| 通知中心美化 | 4.2.3 | CoreWnd 列表短缓存 |
| 任务栏歌词 | 0.4.8 | PID→exe 缓存；任务栏 HWND 短缓存；设置按代际 TLS 复用 |
| 桌面图标显隐 | 4.1.6 | SHELLDLL_DefView HWND 缓存 |
| 窗口动画（可选） | 1.6.1 | 进程名 PID 二级缓存 |
| 果冻窗口（可选） | 0.115.5 | 桌面可见性轮询 300→500ms |
| 桌面小组件（可选） | 0.1.1 | 时钟未变不重绘；USERDATA；GDI 复用 |
| Dock（可选） | 0.1.1 | PID→名缓存；运行指示变化才重绘；GDI/悬停跟踪 |

未打包：滚轮调音量、任务栏图标大小。
