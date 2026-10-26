# Deprecated optional mods

这些自制 MVP **不再由 `deploy.ps1` 安装**（脚本只扫描 `optional/*.wh.cpp`，不递归子目录）。

| 文件 | 原用途 | 请改用 |
| --- | --- | --- |
| `local@zen-dock.wh.cpp` | 自绘 Nexus 式启动栏 | `optional/local@taskbar-dock-like.wh.cpp`（TAI，DarkionAvey，GPL-3.0） |
| `local@zen-desktop-widgets.wh.cpp` | 自绘桌面玻璃卡片 | `optional/local@desktop-live-overlay.wh.cpp`（m417z，GPL-3.0） |

手动启用（不推荐）：复制回 `optional/` 后重新 `deploy.bat`，或在 Windhawk「创建 Mod」里粘贴源码。
