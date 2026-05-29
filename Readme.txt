========================================================================
             TPDesktop Premium One-Key Deploy Package v2.8.0
========================================================================

欢迎使用 TPDesktop 极简桌面美化套件！本套件包含以下四个本地插件：
1. ZenDesktop: Taskbar Acrylic Styler (精细化亚克力任务栏，支持多档透明度调节)
2. ZenDesktop: Start Menu Acrylic Styler (精细化亚克力开始菜单，支持多档透明度调节)
3. ZenDesktop: Double Click to Hide Icons (双击桌面空白处智能隐藏/显示图标)
4. ZenDesktop: File Explorer Transparent Background (文件资源管理器全透明背景)

========================================================================
✨ v2.8.0 重磅更新（TP 制作）：
========================================================================
* 🆕 【文件资源管理器全透明背景】
  全新轻量级模组，让文件夹背景彻底透明化，桌面壁纸透过文件列表区域清晰可见！
  - 采用 Win32 窗口子类化 + DWM 组合方案，无 XAML Diagnostics 依赖
  - 保留 Windows 11 Mica 壁纸效果，视觉通透又优雅
  - 零资源开销，直接注入 explorer.exe，无后台进程
  - 支持多标签页、窗口新建/恢复、主题切换等完整生命周期
* 🆕 【项目更名 TPDesktop】
  由 TP (TPGoFighting) 制作并维护，感谢 Lanbo 的 ZenDesktop 原作基础。

========================================================================
✨ v2.7.0 更新（Lanbo 制作）：
========================================================================
* 🆕 【Apple Liquid Glass 苹果晶莹流体玻璃主题】
  倾力引入全新的超写实苹果拟真美学主题（AppleLiquidGlass 与 任务栏 macOS 拟真 Dock 风格的 AppleLiquidGlass_variant_Alternate）！
  - 拥有完美的 2px 圆角补偿对齐系统（外圆角 20px，内圆角 18px，子元素圆角 10px）。
  - 五彩斑斓的斜向虹彩边缘（Linear 渐变，融合苹果官方红/橙/绿/蓝/紫五彩光泽），以及 3D Fresnel Specular 高光反射面。
  - 极致清澈透明，去除任何浓重乳白色雾化感，背景及壁纸字字清晰、通透度爆表！
* 🆕 【稳定性重磅优化（黑屏修复）】
  - 默认在 deploy.bat 中将 "双击桌面隐藏图标" 插件设置为禁用状态（Disabled=1）。
  - 彻底解决了 Windows 11 Build 26100 (24H2) 等新版本系统上注入 explorer.exe 偶尔引发的黑屏、闪屏或崩溃问题，系统稳定性达 100%。用户可在 Windhawk 中根据系统兼容性按需手动编译启用。
* 🆕 【文字颜色模式 (Text Color Mode)】
  彻底解决全透明/高透模式下，文字在白色背景（如浏览器、文件夹）前看不清的痛点！支持 Default / Force White / Force Dark Gray / System-aware 四种高反差对比选项。

========================================================================
安装与部署指南（最简 2 步）：
========================================================================

第一步：一键部署配置
   鼠标右键点击 【deploy.bat】，选择【以管理员身份运行】 (Run as Administrator)。
   * 如果您的电脑上未安装 Windhawk，脚本会自动唤起安装程序，请按提示完成安装。
   * 安装完成后，脚本会自动停止服务并极速导入全部 4 个本地美化 Mod 并完成配置。

第二步：在 Windhawk 中激活与自定义
   1. 打开 Windhawk 软件，您会看到 4 个本地插件（taskbar、startmenu、fileexplorer）。
   2. 依次点击每个插件，再点击右上角的【保存并编译】，等待约 10-30 秒编译完成。
   3. 点击 Start Menu Acrylic Styler 插件的 Settings (设置) 按钮。
   4. 在 "Text Color Mode" 下拉菜单中选择 【Force Dark Gray】（或 System-aware），点击 Save 保存。
   5. 尽情享受丝滑、纯净、高可读性的全新亚克力桌面美化！

========================================================================
文件说明：
========================================================================
  windhawk_setup.exe                     - Windhawk 官方安装包（支持在线/离线，已安全内置本地编译器）
  deploy.bat                             - 一键自适应智能部署脚本（以管理员身份运行）
  local@zen-taskbar-acrylic.wh.cpp       - 任务栏亚克力插件源码
  local@zen-startmenu-acrylic.wh.cpp     - 开始菜单亚克力插件源码
  local@zen-desktop-toggle-icons.wh.cpp  - 双击隐藏图标插件源码
  local@zen-fileexplorer-transparent.wh.cpp - 文件资源管理器透明背景插件源码
  Readme.txt                             - 本说明文件
========================================================================
设计美学 & 优势：
* 纯本地化编译，完全脱离官方服务器下载限制，即插即用。
* 彻底解决 Windows 系统下的编码乱码 bug，所有选项整洁优雅。
* 完美继承 Windows 11 原生亚克力（Acrylic）与毛玻璃微动特效，内存和 CPU 几乎 0 开销。
* 新增文件资源管理器全透明背景，桌面壁纸透出文件夹区域。

特别鸣谢：感谢原作者 m417z 创作的 Windhawk 任务栏与开始菜单美化 Mod。
========================================================================
© 2026 TP (TPGoFighting). Based on ZenDesktop by Lanbo. All rights reserved.
