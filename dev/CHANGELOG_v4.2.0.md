# ZenDesktop v4.2.0 更新说明（草稿）

> ⚠️ 草稿：以下改动尚未在 Windows 上实测，发布前需逐条验证。

## 🛠️ 修复
- **文件管理器**：改用 `zen-explorer-styler`，不再推荐注册 ExplorerBlurMica；若已注册请卸载后再启用 styler。
- **窗口动画 1.5**：新增碎片坠落、缩小淡出、弹性收起、三维翻转、向上滑走，以及从点击处放大、弹性弹出、三维倾斜。
- **任务栏歌词 0.2**：左下角音乐面板 + AI 状态 MCP。
- **deploy**：只装 mod、管进程列表、缺省补默认值；不编译、不开启、不重注册 EBM。

## ✨ 新增（可选安装）
- **Wobbly Windows 果冻窗口**（作者 lalimatyus，GPL-3.0）：KDE/Compiz 风格，拖动窗口会像果冻一样晃。需要在 Windhawk 进程包含列表里加入 `dwm.exe`。
- **Windows Animations 关窗动画**（作者 ReDrag，MIT）：碎裂、溶解、老电视关机等关窗效果。ZenDesktop 默认**只开关窗效果**，最小化、还原（含神灯）、启动和 Alt+Tab 切换动画默认关闭；检测到已启用 macOS 最小化动画时，部署脚本会强制关掉它的最小化/还原，避免两个 Mod 抢同一个窗口。
- 两个第三方 Mod 保留上游版本号（1.3.7 / 0.115），简介第一行标注「ZenDesktop v4.2 收录」。
- 部署时两个可选 Mod 和其他 Mod 一样装进 Windhawk，默认关闭，由你在 Windhawk 里自己打开；无人值守时可用 `deploy.bat -Optional "wobbly-windows,windows-animations"` 直接打开指定的 Mod。

## 🧪 测试顺序（发布前）
1. 只开 Wobbly Windows（果冻窗口），**重启整台电脑**（不要只重启资源管理器）。
2. 再开 Windows Animations 的关窗效果，再次重启整台电脑。
3. 每步确认无异常后截图，作为 #10 的效果预览图放进 `docs/previews/`。

## 📖 文档
- README 新增「下载哪个压缩包」对照表，不用再猜 4 个 zip 该下哪个。
- README 新增效果预览区（#10），截图陆续补上。
- 新增 `optional/THIRD_PARTY_NOTICES.md`，注明第三方 Mod 的作者和许可证。

## 🔜 下一版预告
- ZenDesktop 管理器：一键切换预设、内置「修复」按钮。
