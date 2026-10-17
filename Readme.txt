========================================================================
             ZenDesktop Premium One-Key Deploy Package v4.1.0
========================================================================

欢迎使用 ZenDesktop 极简桌面美化套件 v4.1.0！

👨‍💻 开发者信息 & 关注支持：
- 📺 哔哩哔哩：https://space.bilibili.com/1280632999
- 🎵 抖音：https://v.douyin.com/Isl7QPpKZZ4/
- ⚡ 爱发电：https://ifdian.net/a/Lanboo

本套件包含以下 6 大核心本地插件与 1 个外部透明增强程序：
1. ZenDesktop: 任务栏美化 (精细化亚克力/毛玻璃任务栏，支持多档透明度与最大化自适应)
2. ZenDesktop: 通知栏美化 (精细化亚克力通知中心、快捷设置和日历面板)
3. ZenDesktop: 开始菜单美化 (精细化亚克力开始菜单，支持尺寸自定义与文件夹独立调节)
4. ZenDesktop: 桌面双击隐藏图标 (双击空白处显示/隐藏图标，支持闲置自动隐藏与平滑淡入淡出)
5. ZenDesktop: 设置美化 (Windows 11 设置 Win+I 界面全透明/磨砂玻璃，深度适配亮色/暗色模式)
6. macOS 神奇效果最小化动画 (基于 Direct2D 网格变形的 macOS 经典 Genie 神奇缩放动画)
7. zen-explorer-styler: 文件管理器美化（Windhawk mod，替代 ExplorerBlurMica）

========================================================================
✨ v4.1.0 重磅更新说明：
========================================================================
* 🆕 【全系列模组命名与中文本地化规范】
  - 模组统一采用规范的中文命名与中文简介，保留 `ZenDesktop: ` 前缀。
  - 所有模组设置项均完成双语对照翻译，包括提示说明、选项下拉框与范围限制。
* 🆕 【全新 Direct2D macOS 神奇效果动画引擎】
  - 引入原作者最新 Direct2D 网格变形引擎（Potassiumuncher & Abdullah Masood）。
  - 支持网格细分密度调节（8-96）、自动隐藏任务栏唤出、排除程序列表及多显示器适配。
* 🆕 【设置美化亮色模式全面透明优化】
  - 彻底修复亮色模式下设置卡片泛白不透明的问题，统一 Glass 模式全景磨砂质感。
* 🆕 【一键部署脚本 (deploy.ps1) 深度重构】
  - 自动清理历史废弃插件并重置 UI 缓存，确保在 Windhawk 中即刻展现全中文模组列表。

========================================================================
安装与部署指南（最简 3 步）：
========================================================================

第一步：一键部署 Windhawk 插件
   鼠标右键点击 【deploy.bat】，选择【以管理员身份运行】 (Run as Administrator)。
   * 如果您的电脑上未安装 Windhawk，脚本会自动唤起安装程序，请按提示完成安装。
   * 脚本会自动停止服务并极速导入 6 个本地美化 Mod 并完成初始化配置。

第二步：在 Windhawk 中保存并编译
   1. 打开 Windhawk 软件，进入“已安装的 Mods”界面。
   2. 依次进入各个插件的源码页面，点击右上角的【保存并编译】 (Save / Compile) 按钮。
   3. 编译完成后，插件即刻被激活。

第三步：在 Windhawk 中调整参数并启用文件管理器美化
   1. 打开 Windhawk 软件，进入各美化插件的设置页面，调整透明色、浓度、模糊等参数，保存即生效。
   2. 文件管理器透明：在 Windhawk 中编译并启用「ZenDesktop: 文件管理器美化」。
   3. 若以前注册过 ExplorerBlurMica：管理员运行 ExplorerBlurMica\uninstall.cmd 取消注册后再启用 styler。
   4. 色调、不透明度等请在 Windhawk 该 mod 的设置页调整。

========================================================================
文件说明：
========================================================================
  windhawk_setup.exe                          - Windhawk 官方在线安装包
  windhawk_setup_offline.exe                  - Windhawk 官方离线安装包 (仅离线包提供)
  deploy.bat                                  - 一键自适应智能部署脚本（以管理员身份运行）
  deploy.ps1                                  - 部署脚本核心逻辑 (v4.1.0)
  build_releases.py                           - 一键打包发布脚本 (v4.1.0)
  ExplorerBlurMica/                           - 【历史遗留，v4.2 不推荐使用】原 ExplorerBlurMica 目录 (Maplespe)
    ├─ ExplorerBlurMica.dll                   - （请改用 zen-explorer-styler；若已注册请 uninstall.cmd）
    ├─ config-editor-wpf.exe                 - 可视化透明设置编辑器
    ├─ register.cmd                           - 【勿用】历史注册脚本；请改用 uninstall.cmd 取消注册
    └─ uninstall.cmd                          - 注销并还原透明的脚本
  local@zen-taskbar-acrylic.wh.cpp            - 任务栏美化插件源码
  local@zen-notificationcenter-acrylic.wh.cpp - 通知中心美化插件源码
  local@zen-startmenu-acrylic.wh.cpp          - 开始菜单美化插件源码
  local@zen-desktop-toggle-icons.wh.cpp        - 桌面双击隐藏图标插件源码
  local@windows-11-settings-styler-fork.wh.cpp- 设置应用美化插件源码
  local@macos-minimize-animation.wh.cpp        - macOS 神奇效果最小化动画源码
  Readme.txt                                  - 本说明文件
  README.md                                   - 仓库主页文件

========================================================================
特别鸣谢：
========================================================================
1. 文件管理器透明现以 zen-explorer-styler 为准；历史版本曾使用 Maplespe 的 ExplorerBlurMica，致谢保留。
2. 感谢 Windhawk 官方作者 m417z 创作的 Windhawk 基础框架及系列 Mod。
3. 感谢 Abdullah Masood & Potassiumuncher 创作的 MacOS Minimize Animation 神奇动画。
========================================================================
© 2026 Lanbo. All rights reserved.
