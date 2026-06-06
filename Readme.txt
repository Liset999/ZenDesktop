========================================================================
             ZenDesktop Premium One-Key Deploy Package v4.0.0
========================================================================

欢迎使用 ZenDesktop 极简桌面美化套件 v4.0.0！本套件包含以下四个本地插件与一个外部透明增强程序：
1. ZenDesktop: Taskbar Acrylic Styler (精细化亚克力任务栏，支持多档透明度调节)
2. ZenDesktop: Notification Center Acrylic Styler (精细化亚克力通知中心，Lanbo & m417z 联手共创)
3. ZenDesktop: Start Menu Acrylic Styler (精细化亚克力开始菜单，支持多档透明度调节)
4. ZenDesktop: Desktop Icon Toggle and Auto-Hide (双击切换图标，并可在重启后仍然保持隐藏状态)
5. ExplorerBlurMica: 资源管理器透明 (由 Maplespe 开发的优秀外部程序，支持 Blur/Acrylic/Mica 效果)

★ 特别说明：
资源管理器透明程序 (ExplorerBlurMica) 并非本套件原创，其原作者为 GitHub 上的 Maplespe。
本套件作者 Lanbo 专门为此设计和编写了“ZenDesktop 视觉控制中心”前端图形化配置页面 (ZenDesktopCustomizer.py)，使得用户可以更简单、更方便、更直观地一键启用、注销和调整所有的透明及玻璃美化效果。

========================================================================
✨ v4.0.0 重磅更新说明：
========================================================================
* 🆕 【资源管理器透明程序整合与前端控制面板】
  - 弃用了原有不稳定的 Windhawk 资源管理器透明 Mod，改为集成成熟的外部程序 ExplorerBlurMica (原作者: Maplespe)。
  - 在前端控制面板中新增了资源管理器透明的一键注册/启用、高级配置打开、一键注销/停用功能，极大降低了用户的使用门槛。
* 🆕 【桌面双击隐藏重启保持状态】
  - 改进了桌面图标隐藏插件，现在隐藏状态在系统重启或资源管理器重启后依然能完美保持。
* 🆕 【前端 GUI 视觉调整与防抖集成】
  - 内置了由 Lanbo 编写的 CustomTkinter 实时调色与个性化面板，所有设置实时保存，一键同步，性能极其优异。

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

第三步：使用视觉控制中心进行日常设置与资源管理器透明
   1. 双击运行 【run_customizer.bat】 启动前端调色面板（首次运行会自动下载 customtkinter 依赖，请保持联网）。
   2. 在“资源管理器”标签页中，点击【注册/启用透明效果】以开启窗口毛玻璃/亚克力背景（首次启用需要管理员授权）。
   3. 点击【打开高级透明配置】可调用可视化编辑器调整资源管理器的模糊半径、颜色等。
   4. 您还可以通过面板一键调整任务栏、开始菜单、通知中心的透明色、浓度、模糊等参数。

========================================================================
文件说明：
========================================================================
  windhawk_setup.exe                     - Windhawk 官方在线安装包
  windhawk_setup_offline.exe             - Windhawk 官方离线安装包 (仅离线包提供)
  deploy.bat                             - 一键自适应智能部署脚本（以管理员身份运行）
  deploy.ps1                             - 部署脚本核心逻辑
  ZenDesktopCustomizer.py                - 视觉控制中心前端面板 (由 Lanbo 编写)
  run_customizer.bat                     - 视觉控制中心快捷启动入口
  ExplorerBlurMica/                      - 资源管理器透明程序目录 (原作者: Maplespe)
    ├─ ExplorerBlurMica.dll              - 透明效果核心 DLL 动态链接库
    ├─ config-editor-wpf.exe            - 可视化透明设置编辑器
    ├─ register.cmd                      - 注册并启用透明的脚本
    └─ uninstall.cmd                     - 注销并还原透明的脚本
  local@zen-taskbar-acrylic.wh.cpp       - 任务栏亚克力插件源码
  local@zen-notificationcenter-acrylic.wh.cpp - 通知中心亚克力插件源码
  local@zen-startmenu-acrylic.wh.cpp     - 开始菜单亚克力插件源码
  local@zen-desktop-toggle-icons.wh.cpp   - 桌面图标切换与隐藏插件源码
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
