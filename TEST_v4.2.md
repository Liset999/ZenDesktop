# v4.2 测试清单（Lanbo 照着打勾）

每一轮测试前都重启整台电脑。一次只多开一样。

## 第 0 轮：开机后打不开文件管理器 / 托盘图标没 hook 上（先排查这个）
每一步都重启整台电脑两次，记下两次的结果。
- [ ] 先确认 `ExplorerBlurMica/ExplorerBlurMica.dll` 已换成 Actions 里 `a63cc5e` 之后编好的新版（文件日期不是 7 月 6 日），并用管理员身份跑过 `register.cmd`
- [ ] 换好新 DLL 后：开机直接打开文件管理器，能打开吗？
- [ ] 还打不开的话：运行 ExplorerBlurMica 的 `uninstall.cmd` 停用它，再重启两次。能打开了 → 是它的问题
- [ ] 文件管理器正常、只是托盘图标没出来：在 Windhawk 里把任务栏 mod 一个个禁用，每关一个就重启一次，记下关掉哪个以后图标恢复了
- [ ] 打开 Windhawk 主界面后托盘图标才出来 → 这是 Windhawk 注入太晚（windhawk #197），等 Liset 加的重试版本

| 步骤 | 第 1 次开机 | 第 2 次开机 | 备注 |
|---|---|---|---|

## 第 1 轮：基础包（不开任何可选项）
- [ ] 运行 `deploy.bat`，不报错
- [ ] ExplorerBlurMica 重新注册的窗口弹出来，并且显示成功
- [ ] 重启后打开文件管理器，顶上没有黑杠
- [ ] 原来的任务栏、开始菜单、通知中心亚克力都正常
- [ ] mac 最小化动画正常

## 第 2 轮：只加果冻窗口
- [ ] `deploy.ps1 -Optional "wobbly-windows"`，有 dwm.exe 注入的提示
- [ ] 拖动窗口有果冻效果，不卡、不闪
- [ ] 打开英雄联盟 / Steam 游戏，进得去，没有反作弊报错

## 第 3 轮：再加关窗效果
- [ ] `deploy.ps1 -Optional "wobbly-windows,windows-animations"`
- [ ] Windhawk 设置页里：只有「Animate closing」是开的，最小化、还原、启动、切换都关着
- [ ] 关窗有灭霸响指效果
- [ ] 最小化只有 mac 神灯动画，没有被做两次
- [ ] 6 种关窗效果都切一遍，截图或录 GIF（顺便交 #10）

## 第 4 轮：README
- [ ] 照着 README 里那张表，能看懂 4 个 zip 该下哪个

## 第 5 轮：开机托盘不透明 / 文件管理器打不开，找出是谁（二分）
每一步都要**重启两次整台电脑**（两次都一样才算数），开机后先别手动重启 explorer。
每次开机记三件事：①托盘/音量图标透明没有 ②第一次点开文件管理器能不能开 ③有没有弹「XAML diagnostics」的框。

先开日志：Windhawk 里 `local@zen-taskbar-acrylic` → 高级 → 打开日志。开机后搜 `[ZenDiag]`，
如果看到 `Other XAML diagnostics consumer: ...ExplorerBlurMica.dll`，基本就是两者抢「XAML 诊断」那个位置。

- [ ] 5.0 基线：任务栏 mod 版本是 **4.1.2**；重启两次，记下 ①②③
- [ ] 5.1 只关 ExplorerBlurMica：管理员跑 `ExplorerBlurMica\uninstall.cmd`，重启两次。
      ①②都好了 → 是 ExplorerBlurMica；跳到 5.4
- [ ] 5.2 ExplorerBlurMica 还是关着，再在 Windhawk 里一个一个关任务栏相关 mod，每关一个重启两次：
  - [ ] `local@zen-taskbar-acrylic`
  - [ ] `local@macos-minimize-animation`
  - [ ] `local@zen-desktop-toggle-icons`
  - [ ] 可选的 `local@windows-animations`
  - [ ] 不是我们的 mod（Windhawk 里其他注入 explorer.exe 的）也一个个关
- [ ] 5.3 全关了还坏 → 不是 mod 的问题，查 开机自启的软件（TranslucentTB 之类也用 XAML 诊断）
- [ ] 5.4 确认是 ExplorerBlurMica 后，装回去，二选一试：
  - [ ] A：任务栏 mod 设置「XAML diagnostics」= **自动阻止 (Block)**，重启两次 → 托盘应该好；看文件列表还透不透
  - [ ] B：保持「允许共存 (Allow)」，换回 217KB 的延迟 hook 版 `ExplorerBlurMica.dll.delayhook-217k`（改名覆盖后跑 `register.cmd`），重启两次
- [ ] 5.5 还是要手动重启 explorer 才行 → 装开机自检兜底：
      `deploy.ps1 -StartupCheck check`，不行再换 `-StartupCheck restart`。
      日志在 `%LOCALAPPDATA%\ZenDesktop\startup-check.log`，关掉用 `deploy.ps1 -StartupCheck remove`
- [ ] 顺便看一眼「事件查看器 → Windows 日志 → 应用程序」里开机后有没有 `explorer.exe` 的 1000 号错误，
      故障模块是 `ucrtbase.dll` / `0xc0000409` 的话，和 ExplorerBlurMica#209 是同一个问题

## 出问题时
截图 + 第几轮第几条 + Windhawk 日志，发群里。
