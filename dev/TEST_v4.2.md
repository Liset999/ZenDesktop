# v4.2 测试清单（Lanbo 照着打勾）

每一轮测试前都重启整台电脑。一次只多开一样。

## 第 0 轮：先看这里
> 文件管理器透明请用 **ZenDesktop: 文件管理器美化**（`zen-explorer-styler`）。**不要**再注册 ExplorerBlurMica。
> 若以前注册过：管理员运行 `tools\uninstall-ExplorerBlurMica.bat`，再重启。

## 第 1 轮：基础包（不开任何可选项）
- [ ] 运行 `deploy.bat`，不报错；**没有**弹出 ExplorerBlurMica 重新注册窗口
- [ ] Windhawk 里能看到 `zen-explorer-styler`，默认关着；编译并开启后，文件管理器命令栏/地址栏可透明
- [ ] 桌面空白处右键：菜单应有模糊玻璃底（不是实心白/灰）；改「右键菜单：模糊」和「色调颜色」能立刻反映
- [ ] 文件管理器里对文件右键：同样有玻璃效果；「右键菜单：玻璃效果」关掉后恢复系统默认
- [ ] Shift+右键 /「显示更多选项」的经典菜单：本 mod 不改（预期）
- [ ] 浅色模式下文件管理器仍透，没有整块发黑
- [ ] 原来的任务栏、开始菜单、通知中心亚克力都正常；开始菜单浅色无白雾
- [ ] 任务栏 4.2：最大化一层开/关、多显示器、虚拟桌面切换行为正常

## 第 2 轮：只加果冻窗口
- [ ] `deploy.ps1 -Optional "wobbly-windows"`，有 `dwm.exe` 加入包含列表的提示
- [ ] 拖动窗口有果冻效果，不卡、不闪
- [ ] 打开英雄联盟 / Steam 游戏，进得去，没有反作弊报错

## 第 3 轮：再加关窗效果
- [ ] `deploy.ps1 -Optional "wobbly-windows,windows-animations"`
- [ ] 旧 mac 最小化 mod 已关闭；窗口动画里开启最小化/还原，样式选「macOS 神奇效果」时只播一次
- [ ] 关闭样式各试一遍（含 1.5 新增：碎片坠落、缩小淡出、弹性收起、三维翻转、向上滑走）
- [ ] 最小化样式试「从点击处放大」「弹性弹出」「三维倾斜」以及 Windows 10 / 神奇效果
- [ ] 按几次 Win+D，再最小化、还原：没有闪烁，动画没被做两次

## 第 4 轮：任务栏歌词
- [ ] 关掉系统小组件，开启 `zen-taskbar-lyrics`，播歌时有封面/歌词/三按钮
- [ ] （可选）按 `tools/ai-status` 接上 MCP，AI 状态能显示

## 第 5 轮：README / 发版材料
- [ ] 照着 README 能看懂该下哪个 zip；文中**没有**「去注册 ExplorerBlurMica」的步骤
- [ ] `RELEASE_NOTES_v4.2.md` / `RELEASE_POSTS_v4.2.md` 内容与实测一致后再发

## 出问题时
截图 + 第几轮第几条 + Windhawk 日志，发群里。
