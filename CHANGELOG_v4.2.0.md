# ZenDesktop v4.2.0 更新说明（草稿）

> ⚠️ 草稿：以下改动尚未在 Windows 上实测，发布前需逐条验证。

## 🛠️ 修复
- **文件管理器顶部黑杠**：一键部署结束时自动重新注册 ExplorerBlurMica 并重启资源管理器，缓解切换窗口后标题栏/标签栏变黑的问题（ExplorerBlurMica #138）。README 新增排查说明，仍有问题可重启电脑或尝试 FileBrowsWindowPatch。

## ✨ 新增（可选安装）
- **Wobbly Windows 果冻窗口**（作者 lalimatyus，GPL-3.0）：KDE/Compiz 风格，拖动窗口会像果冻一样晃。需要在 Windhawk 进程包含列表里加入 `dwm.exe`。
- **Windows Animations 关窗动画**（作者 ReDrag，MIT）：碎裂、溶解、老电视关机等关窗效果。ZenDesktop 默认**只开关窗效果**，最小化、还原（含神灯）、启动和 Alt+Tab 切换动画默认关闭；检测到已启用 macOS 最小化动画时，部署脚本会强制关掉它的最小化/还原，避免两个 Mod 抢同一个窗口。
- 两个第三方 Mod 保留上游版本号（1.3.7 / 0.115），简介第一行标注「ZenDesktop v4.2 收录」。
- 部署时逐个询问是否安装，默认不装；也支持 `deploy.bat -Optional "wobbly-windows,windows-animations"` 无人值守安装。

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
