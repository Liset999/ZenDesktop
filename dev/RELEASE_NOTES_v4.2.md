# ZenDesktop v4.2 发布说明

面向用户的更新摘要。仓库分支：`v4.2-draft`。装完请**重启整台电脑**，不要只重启资源管理器。

## 主要变化

### 文件管理器：用「文件管理器美化」替代 ExplorerBlurMica
- 新增 Windhawk mod `local@zen-explorer-styler`（ZenDesktop: 文件管理器美化）。
- 色调、不透明度、地址栏/搜索框、文字颜色可在 Windhawk 里调，浅色模式也做了适配。
- **不再推荐安装或注册 ExplorerBlurMica**。若以前注册过，请管理员运行 `tools\uninstall-ExplorerBlurMica.bat`，再重启资源管理器 / 电脑，避免和 styler、任务栏抢 XAML 诊断。
- 仓库已删除 EBM 源码/二进制目录，deploy **不会**再替你注册它。

### 开始菜单：浅色模式白雾
- `zen-startmenu-acrylic` 修复浅色模式下开始菜单发白、发雾的问题。

### 任务栏 4.2
- 在上游 Taskbar Styler v1.10 基础上重写，修好最大化层监听（按显示器、忽略其他虚拟桌面）。
- 去掉重复的重试/重建钩子，设置缺失时回落到默认主题（不再变成全透明）。
- 设置键名未改，原有配置可继续用。

### 窗口动画 1.4 → 1.5
- **1.4.0**：合并原「macOS 最小化动画」为「macOS 神奇效果」样式；旧 mac mod 挪到 `legacy/`，deploy 不再安装。
- **1.5.0**：新增关闭样式——碎片坠落、缩小淡出、弹性收起、三维翻转、向上滑走；新增最小化/还原/启动样式——从点击处放大、弹性弹出、三维倾斜。缩向任务栏继续用「Windows 10」或神奇效果。
- 随机样式池仍是原版效果；新样式需在设置里手动选。

### Deploy 边界（与你的定义一致）
- 只：合并进程包含/排除列表（含 `dwm.exe`）、把 mod 装进 Windhawk、**缺省才补默认值**。
- 不：编译、开启、覆盖你已有的设置。
- 初次安装的 mod 默认关闭（`Disabled=1`）；你开过的开关会保留。

### 任务栏音乐与歌词 0.2
- `zen-taskbar-lyrics`：占用原小组件位置，封面 + 歌词 + 上一首/播放暂停/下一首。
- 本机 AI 状态通过 `tools/ai-status` MCP（`set_status`）写入本地文件后显示；简介里附有提示词与 Claude Code / Codex / opencode 配置示例。

### 捐助
- 各 mod 简介与 README 保留[爱发电](https://ifdian.net/a/Lanboo)链接和微信赞赏码。

## 升级步骤（简）

1. 备份 Windhawk 里各 mod 的设置（可选）。
2. 以管理员运行 `deploy.bat`（或 `deploy.ps1`）。可选 mod 按提示选择；无人值守可用 `-Optional "wobbly-windows,windows-animations"`。
3. 打开 Windhawk，对新增/更新的 mod 点「保存并编译」，再按需开启。
4. 若仍装着 ExplorerBlurMica：管理员运行 `tools\uninstall-ExplorerBlurMica.bat`，然后启用「文件管理器美化」。
5. 窗口动画：关掉旧 mac 最小化 mod；最小化样式若要用神奇效果，手动选「macOS 神奇效果」。
6. **重启电脑**。

## 已知限制

- 新动画效果未附预览动图；请在本机切换样式目测。
- 三维翻转 / 三维倾斜为 CPU 近似透视，不是完整网格 3D。
- 歌词面板、新动画均需在 Windhawk 内编译后才能用；作者侧未在你的 Windows 上实测。

## 相关文件

- 短文案 / B 站与抖音：`RELEASE_POSTS_v4.2.md`
- 测试清单：`TEST_v4.2.md`
- 文字规范：`docs/MOD_STYLE_GUIDE.md`
