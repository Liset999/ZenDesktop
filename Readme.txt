========================================================================
             ZenDesktop 一键部署包 v5.0.0
========================================================================

作者：Lanbo
- 哔哩哔哩：https://space.bilibili.com/1280632999
- 抖音：https://v.douyin.com/Isl7QPpKZZ4/
- 爱发电：https://ifdian.net/a/Lanboo
- 问题反馈：https://github.com/Liset999/ZenDesktop/issues

ZenDesktop 是一套 Windows 11 桌面美化 mod，运行在 Windhawk 上。

========================================================================
包含的 Mod（装好后默认全部关闭，在 Windhawk 里按需打开）
========================================================================
  ZenDesktop: 任务栏美化         任务栏透明/亚克力/液态玻璃
  ZenDesktop: 开始菜单美化       开始菜单与搜索页玻璃效果，可调尺寸
  ZenDesktop: 通知栏美化         通知中心、日历、快捷设置玻璃效果
  ZenDesktop: 设置美化           「设置」应用（Win+I）玻璃背景
  ZenDesktop: 文件管理器美化     文件管理器与右键菜单透明
  ZenDesktop: 任务栏音乐与歌词   任务栏左下角显示歌曲、歌词和播放按钮
  ZenDesktop: 桌面双击隐藏图标   双击桌面空白处隐藏/显示图标
  （可选）果冻窗口               拖动窗口时像果冻一样晃动
  （可选）窗口动画               关窗、最小化、还原、启动动画

========================================================================
安装步骤
========================================================================
1. 把压缩包完整解压到任意文件夹（路径有空格、中文都可以）。
2. 双击 deploy.bat，在权限提示里点「是」。
   - 如果电脑上还没装 Windhawk，并且包里带有 windhawk_setup.exe，
     脚本会先打开它，按提示装完后脚本会自动继续。
   - 只含 Mod 的 _Online 包不带安装程序，请先从 https://windhawk.net 安装 Windhawk。
3. 窗口显示「一键部署完成」后按任意键关闭。
4. 打开 Windhawk →「已安装的 Mod」，点进想用的 mod 打开开关；
   提示需要编译时按提示编译（或在源码页点右上角「保存并编译」）。
5. 在各 mod 的「设置」页调整效果，保存即生效。全部调好后建议重启一次电脑。

再次运行 deploy.bat（例如更新新版本）不会关掉你已打开的 mod，
也不会覆盖你改过的设置，只补上新设置的默认值。

从 v4.1 升级：先双击 tools\uninstall-ExplorerBlurMica.bat（会请求管理员权限），
卸载旧版 ExplorerBlurMica（EBM），它会自动重启资源管理器；然后再运行 deploy.bat。
v4.2 的文件管理器透明改由「文件管理器美化」mod 负责，不再需要 EBM。

========================================================================
卸载与恢复
========================================================================
一键卸载：双击 uninstall.bat，在权限提示里点「是」，确认后会：
  - 从 Windhawk 移除全部 ZenDesktop mod（源码、编译文件、mod 记录和设置）；
  - 删除开机自检计划任务（如果装过）；
  - 询问是否删除 %LOCALAPPDATA%\ZenDesktop（AI 状态文件、自检日志）；
  - 询问是否重启资源管理器。
Windhawk 本身和你自己装的其他 mod 不受影响。

还需要手动处理的：
1. deploy 往 Windhawk「设置 → 高级设置」的进程包含列表加了 dwm.exe、
   排除列表加了一些游戏进程，卸载不会改动，想还原请手动删掉。
2. 接过 AI 状态的：删掉当时粘贴到 AI 助手配置里的 zen-status 片段。

只想暂时停用：在 Windhawk 里关掉对应 mod，再运行 restart_explorer.bat。

========================================================================
文件说明
========================================================================
  deploy.bat / deploy.ps1         一键部署（把 mod 装进 Windhawk，不编译、不自动打开）
  uninstall.bat / uninstall.ps1   一键卸载（从 Windhawk 移除 ZenDesktop mod，不卸载 Windhawk）
  restart_explorer.bat            重启资源管理器
  local@*.wh.cpp                  各 mod 源码
  optional\                       可选 mod（果冻窗口、窗口动画）及第三方许可说明
  tools\ai-status\                任务栏 AI 编程助手状态（可选，见其中的 README.md）
  tools\uninstall-ExplorerBlurMica.bat  卸载 v4.1 的旧版 ExplorerBlurMica（从 v4.1 升级时运行）
  tools\ZenDesktop-StartupCheck.ps1  开机自检（可选，deploy.ps1 -StartupCheck check）
  windhawk_setup*.exe             Windhawk 官方安装程序（仅完整包/离线包）
  LICENSE                         GPL-3.0 许可证全文

========================================================================
注意
========================================================================
- 文件管理器美化不要和 ExplorerBlurMica 同时使用。以前注册过的，
  请先运行 tools\uninstall-ExplorerBlurMica.bat 卸载。
- 果冻窗口需要注入 dwm.exe，deploy 已把它加进 Windhawk 的进程包含列表，
  打开果冻窗口后要重启整台电脑。

========================================================================
致谢与许可
========================================================================
- Windhawk 与上游 Styler 系列 mod：m417z（GPL-3.0）。
- 果冻窗口：lalimatyus（GPL-3.0）。
- 窗口动画：ReDrag（MIT），macOS 神奇效果：Abdullah Masood、Potassiumuncher。
- 各 mod 的详细许可见其说明中的「致谢与许可」，第三方许可见 optional\THIRD_PARTY_NOTICES.md。
- ZenDesktop 以 GPL-3.0 发布，许可证全文见 LICENSE。
========================================================================
