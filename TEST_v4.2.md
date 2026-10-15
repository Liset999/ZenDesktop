# v4.2 测试清单（Lanbo 照着打勾）

每一轮测试前都重启整台电脑。一次只多开一样。

## 第 0 轮：先看这里
> 逃生办法：文件管理器打不开时，打开它的同时一直按住 `Esc`，ExplorerBlurMica 就不会加载，进去后再跑 `uninstall.cmd`。

开机托盘不透明、文件管理器打不开的排查，统一按下面「第 5 轮」来做。

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
  - [ ] B：保持「允许共存 (Allow)」，换成 GitHub Actions 编的源码版（v4.2-draft 推上去后，Actions → Build ExplorerBlurMica DLL → 下载 `ExplorerBlurMica-DLL`，覆盖 `ExplorerBlurMica/ExplorerBlurMica.dll`，管理员跑 `register.cmd`；仓库里 427KB 那个是 7 月 6 日的 2.0.1 旧版）。源码版完全不用 XAML 诊断，不会跟任务栏抢；代价是 Win11 的 WinUI 文件列表/命令栏不会变透明。重启两次
        开机文件管理器还是打不开的话，在 `config.ini` 的 `[config]` 里把 `delayHookMs` 从 800 调到 2000 再试
- [ ] 5.5 还是要手动重启 explorer 才行 → 装开机自检兜底：
      `deploy.ps1 -StartupCheck check`，不行再换 `-StartupCheck restart`。
      日志在 `%LOCALAPPDATA%\ZenDesktop\startup-check.log`，关掉用 `deploy.ps1 -StartupCheck remove`
- [ ] 顺便看一眼「事件查看器 → Windows 日志 → 应用程序」里开机后有没有 `explorer.exe` 的 1000 号错误，
      故障模块是 `ucrtbase.dll` / `0xc0000409` 的话，和 ExplorerBlurMica#209 是同一个问题

## 出问题时
截图 + 第几轮第几条 + Windhawk 日志，发群里。
