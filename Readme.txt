========================================================================
             ZenDesktop Premium One-Key Deploy Package v4.0.0
========================================================================

欢迎使用 ZenDesktop 极简桌面美化套件 v4.0.0！本套件包含以下六个本地插件与一个外部透明增强程序：
1. ZenDesktop: Taskbar Acrylic Styler (精细化亚克力任务栏，支持多档透明度调节)
2. ZenDesktop: Notification Center Acrylic Styler (精细化亚克力通知中心，Lanbo & m417z 联手共创)
3. ZenDesktop: Start Menu Acrylic Styler (精细化亚克力开始菜单，支持多档透明度调节，且内置尺寸自定义与文件夹独立调节)
4. ZenDesktop: Desktop Icon Toggle and Auto-Hide (双击切换图标，并可在重启后仍然保持隐藏状态)
5. ZenDesktop: Taskbar Background Helper (最大化任务栏辅助，在最大化时智能切换任务栏为深色/模糊/亚克力)
6. ZenDesktop: MacOS Minimize Animation (macOS 极简切换与窗口最小化 Genie 神奇效果动画)
7. ExplorerBlurMica: 资源管理器透明 (由 Maplespe 开发的优秀外部程序，支持 Blur/Acrylic/Mica 效果)

★ 特别说明：
资源管理器透明程序 (ExplorerBlurMica) 并非本套件原创，其原作者为 GitHub 上的 Maplespe。
所有透明及玻璃美化效果的启用、注销与参数调整均直接通过 Windhawk 软件界面完成，无需额外工具。

========================================================================
✨ v4.0.0 重磅更新说明：
========================================================================
* 🆕 【开始菜单大小与文件夹独立自定义】
  - 新增了开始菜单面板的宽度与高度自定义调节参数，用户可直接在 Mod 设置中自定义尺寸。
  - 文件夹现在支持完全独立的透明度、背景色和模糊度调节，且修复了“跟随主面板”失效的 Bug。
* 🆕 【系统级深度性能优化 (XAML 渲染热路径)】
  - 清除了核心美化 Mod 中高频 UI 重绘事件的调试日志，大幅提升 XAML 渲染性能。
  - 彻底解决了此前打开 Win+A 通知中心、开始菜单或任务栏时的选项缺失、加载延迟或瞬间空白白屏的卡顿问题。
* 🆕 【资源管理器透明程序整合与配置前端】
  - 集成 ExplorerBlurMica 用于资源管理器透明。由于 Windows 底层限制，无法实现完全 100% 透明（有模糊效果），且失去焦点时标题栏会回退为纯色。
  - 特意配备了专属的可视化配置前端工具 (config-editor-wpf.exe)，方便用户一键注册、注销以及直观调节透明色与模糊半径。
* 🆕 【引入两大全新辅助 Mod（最大化处理与 macOS 动画）】
  - 引入了 `macos-minimize-animation`：实现媲美 Mac 的窗口神奇效果最小化与恢复动画，流畅度极佳。
  - 引入了 `taskbar-background-helper`：解耦了原任务栏 Mod 内置的最大化检测，窗口最大化时任务栏可秒级切换为深色/模糊/亚克力，彻底解决了原先的响应延迟与性能瓶颈。

========================================================================
安装与部署指南（最简 3 步）：
========================================================================

第一步：一键部署 Windhawk 插件
   鼠标右键点击 【deploy.bat】，选择【以管理员身份运行】 (Run as Administrator)。
   * 如果您的电脑上未安装 Windhawk，脚本会自动唤起安装程序，请按提示完成安装。
   * 脚本会自动停止服务并极速导入 4 个本地美化 Mod 并完成初始化配置。

第二步：在 Windhawk 中保存并编译
   1. 打开 Windhawk 软件，您会看到 4 个已注册的本地插件（Taskbar, StartMenu, NotificationCenter, DesktopIconToggle）。
   2. 依次进入各个插件的源码页面，点击右上角的【保存并编译】 (Save / Compile) 按钮。
   3. 编译完成后，插件即刻被激活。

第三步：在 Windhawk 中调整参数与启用资源管理器透明
   1. 打开 Windhawk 软件，进入各美化插件（任务栏、开始菜单、通知中心、双击隐藏图标）的设置页面，调整透明色、浓度、模糊等参数，保存即生效。
   2. 如需启用资源管理器透明：进入 ExplorerBlurMica 目录，右键以管理员身份运行 register.cmd（首次启用需要管理员授权）。
   3. 如需调整资源管理器透明的模糊半径、颜色等：运行 ExplorerBlurMica 目录下的 config-editor-wpf.exe 可视化编辑器。
   4. 如需注销资源管理器透明：运行 ExplorerBlurMica 目录下的 uninstall.cmd。

========================================================================
文件说明：
========================================================================
  windhawk_setup.exe                     - Windhawk 官方在线安装包
  windhawk_setup_offline.exe             - Windhawk 官方离线安装包 (仅离线包提供)
  deploy.bat                             - 一键自适应智能部署脚本（以管理员身份运行）
  deploy.ps1                             - 部署脚本核心逻辑
  ExplorerBlurMica/                      - 资源管理器透明程序目录 (原作者: Maplespe)
    ├─ ExplorerBlurMica.dll              - 透明效果核心 DLL 动态链接库
    ├─ config-editor-wpf.exe            - 可视化透明设置编辑器
    ├─ register.cmd                      - 注册并启用透明的脚本
    └─ uninstall.cmd                     - 注销并还原透明的脚本
  local@zen-taskbar-acrylic.wh.cpp       - 任务栏亚克力插件源码
  local@zen-notificationcenter-acrylic.wh.cpp - 通知中心亚克力插件源码
  local@zen-startmenu-acrylic.wh.cpp     - 开始菜单亚克力插件源码
  local@zen-desktop-toggle-icons.wh.cpp   - 桌面图标切换与隐藏插件源码
  local@macos-minimize-animation.wh.cpp   - macOS神奇效果最小化动画插件源码
  local@taskbar-background-helper.wh.cpp  - 窗口最大化任务栏背景辅助插件源码
  Readme.txt                             - 本说明文件
  README.md                              - 仓库主页文件

========================================================================
特别鸣谢：
========================================================================
1. 感谢原作者 Maplespe 创作的开源项目 ExplorerBlurMica，为资源管理器提供极致的亚克力与 Mica 效果。
2. 感谢 Windhawk 官方作者 m417z 创作的 Windhawk 各基础 Mod。
3. 感谢 aubymori 创作的 Classic window behavior 基础 Mod。
========================================================================
© 2026 Lanbo. All rights reserved.
