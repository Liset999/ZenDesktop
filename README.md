# ZenDesktop Premium 🪟✨

[![GitHub License](https://img.shields.io/github/license/Liset999/ZenDesktop?color=blue&style=flat-square)](LICENSE)
[![Platform Windows](https://img.shields.io/badge/Platform-Windows%2011-0078d4?style=flat-square&logo=windows)](https://microsoft.com/windows)
[![Engine Windhawk](https://img.shields.io/badge/Engine-Windhawk%20C%2B%2B-ff69b4?style=flat-square)](https://windhawk.net)

**ZenDesktop Premium v4.0.0** is a high-performance desktop styling suite for Windows 11. It blends 4 native Win32/C++ Windhawk mods with the robust external **ExplorerBlurMica** program, with all real-time customizations handled natively via the Windhawk UI **with zero background process bloat, zero UI lag, and 0% CPU overhead**.

[简体中文](#-简体中文) | [English](#-english-features)

---

## 🚀 Latest Performance & UI Tuning Updates / 最新性能与界面调优更新

### 🌐 English
* **Visual Tree Hot-Path Optimization**: Removed all verbose synchronous debug logging (`Wh_Log`) from the XAML event callbacks in all three active mods (`local@zen-notificationcenter-acrylic`, `local@zen-startmenu-acrylic`, `local@zen-taskbar-acrylic`). This completely eliminates UI rendering freezes and lag when opening the Start Menu, Notification Center (Win+A), or Taskbar.
* **Taskbar Mod Streamlining**: Completely removed the redundant maximized window detection logic and its associated settings (`maximizedTaskbarLayer`, etc.) from the Taskbar mod. This functionality is now fully offloaded to the independent `taskbar-background-helper` mod, keeping the main Taskbar mod clean and focused.
* **Start Menu Folder Style Fixes**:
  * Moved the folder customization settings immediately below "开始菜单高度" (Start Menu Height) in the settings panel.
  * Corrected the styling target to specific visual containers (`StartMenu.FolderModal > Grid > Border` and `StartMenu.ExpandedFolderList > Grid > Border`) to prevent background overriding.
  * Fixed the "Follow Main" preset bug, allowing folder-specific parameters to correctly override the cloned main background brush.
* **Explorer Transparency legibility Tuning**: Adjusted `ExplorerBlurMica`'s light mode transparency configuration to a high-contrast frosted white glass overlay (`a=185` in `config.ini`). This ensures black folder text is 100% readable in Windows Light Mode while retaining a beautiful, Apple-like frosted glass look.

### 🇨🇳 中文
* **XAML 渲染热路径优化**：清除了三大核心 Mod（通知中心、开始菜单、任务栏）中 `OnVisualTreeChange` 回调函数中的所有同步调试日志（`Wh_Log`），彻底解决了打开 Win+A 控制台、开始菜单或任务栏时的瞬间卡顿和空白框延迟渲染问题。
* **任务栏 Mod 瘦身与架构解耦**：彻底移除了任务栏 Mod 中多余的最大化检测代码与选项，将其完全交由独立的 `taskbar-background-helper` 辅助 Mod 负责，保持主任务栏 Mod 的纯净。
* **开始菜单文件夹自定义修复**：
  * 将文件夹自定义选项在设置面板中上移至“开始菜单高度”正下方，方便用户配置。
  * 修正了文件夹背景色的应用目标容器，解决了自定义文件夹背景被主背景覆盖的问题。
  * 修复了“跟随主面板”选项下文件夹模糊度、浓度及亮度设置失效的 Bug。
* **资源管理器浅色可读性微调**：将 `ExplorerBlurMica` 浅色模式下的背景不透明度调整为 **`185`（乳白冰砂玻璃）**，既保留了通透的壁纸质感，又为黑色文字提供了足够的白底对比度，完美解决浅色模式下文字看不清、失去焦点闪白的硬伤。

---

## 🌟 English Features

### 1. 🎛️ Taskbar Acrylic Styler (`local@zen-taskbar-acrylic`)
A native Windows 11 Taskbar beautification module offering fine-grained, premium transparency & blur presets:
* **Clear**: 100% full transparency (only taskbar icons remain).
* **Acrylic (High / Standard / Low)**: Real-time high-fidelity WinUI 3 acrylic glass effect.
* **Apple Liquid Glass**: Hyper-transparent 3D droplet glass with a subtle chromatic dispersion border (featuring diagonal red→orange→green→blue→purple gradient stops), Fresnel specular edge reflections, and a precise 2px corner radius compensation (perfectly matching floating macOS-like Dock layout).

### 2. 🔔 Notification Center Acrylic Styler (`local@zen-notificationcenter-acrylic`)
A premium notification and action center acrylic glass styler co-created by **Lanbo** and **m417z**:
* Brings high-fidelity real-time acrylic and frosted glass effects to the Windows 11 Notification Center, calendar panel, and Quick Settings/System Tray panels.
* Perfectly synchronized with your desktop theme presets, retaining flawless WinUI shadows and smooth animations.

### 3. 🚀 Start Menu Acrylic Styler (`local@zen-startmenu-acrylic`)
Syncs the Start Menu panel seamlessly with your taskbar theme, rendering native acrylic blur overlays over both redesigned and classic Start menu layouts. Features **Apple Liquid Glass** preset with a clear liquid body and expanded folder plates.

### 4. 🖱️ Desktop Icon Toggle and Auto-Hide (`local@zen-desktop-toggle-icons`)
A process-native desktop subclassing module. **Double-click empty desktop space to instantly hide/show icons, and state is preserved across system or explorer restarts.**
* **Persistent Hide State**: Uses local registry keys to ensure desktop icons remain hidden even after system restarts.
* **Full-Screen Window Guard**: Suspends capture tracking when running full-screen games, video players, or active presentations.
* **Zero-Latency Filtering**: Blocks synthetic `WM_MOUSEMOVE` messages triggered by `SysListView32` when toggling visibility to eliminate screen flicker.

### 5. 📁 File Explorer Transparency (`ExplorerBlurMica` - External Program)
* **Author Acknowledgement**: Developed by **Maplespe** ([Maplespe/ExplorerBlurMica](https://github.com/Maplespe/ExplorerBlurMica)).
* Adds exquisite visual effects like Blur, Acrylic, and Mica to Windows 10/11 File Explorer.
* Visual customization via config editor and registered as a native shell extension DLL (`regsvr32`).

---

## 📥 Installation & Deployment Guide

> [!IMPORTANT]
> The suite utilizes **pure local compilation** (`local@` prefix), which means your system compiles the C++ code natively. It is 100% offline-ready, safe, and completely bypasses official Windhawk mod server connection failures!

### Step 1: Install Windhawk
Install Windhawk on your Windows 11 PC using the provided setup.

### Step 2: One-Key Local Registry & Mod Deployment
1. Right-click **`deploy.bat`** and select **Run as Administrator** (以管理员身份运行).
2. The script will automatically stop the Windhawk service, register the 4 premium local mods, enable local compilation, reset compiler caching keys, and restart Windhawk safely.

### Step 3: Fast Native Compilation
1. Open the **Windhawk** user interface. You will see 4 newly registered local mods in your home dashboard.
2. Click into each mod and click **Save / Compile** (保存并编译). The engine will compile the native C++ code in ~10 seconds.

### Step 4: Configure via Windhawk UI
1. In the **Windhawk** UI, open each mod's **Settings** page to customize Taskbar, Start Menu, Notification Center, and Desktop Icon options.
2. To enable File Explorer transparency, run **`ExplorerBlurMica\register.cmd`** as Administrator.
3. To configure File Explorer visuals, launch **`ExplorerBlurMica\config-editor-wpf.exe`**.

---

## 🇨🇳 简体中文

### 💎 v4.0.0 核心更新与优势
* **四合一 Mod + 资源管理器透明程序**：弃用了以前不稳定的 C++ 资源管理器插件，替换为更成熟稳定的外部程序 **ExplorerBlurMica** (原作者: **Maplespe**)。
* **Windhawk UI 集中调参**：所有美化插件的参数（透明色、浓度、模糊等）均通过 Windhawk 设置页直接调节；ExplorerBlurMica 的注册/注销则通过其目录下的 register.cmd / uninstall.cmd 完成。
* **双击隐藏状态保存**：改进了桌面图标隐藏 Mod，使其在重启系统或 Explorer 后依然可以记住并保持隐藏状态。
* **纯本地化编译**：脱离官方云服务器连接限制，即插即用，完美解决国内连不上网的困境。
* **极致性能**：利用进程级注入与 Hook 技术，不占用任何后台常驻进程，0% CPU 开销，近乎 0MB 内存占用。

### 🛠️ 快速开始
1. 运行 **`deploy.bat`**（管理员身份）自动配置 Windhawk 并注册 4 个本地插件。
2. 在 Windhawk 界面中，依次进入 4 个插件（任务栏、开始菜单、通知中心、双击隐藏图标）的源码页面，点击右上角的 **保存并编译**。
3. 在 Windhawk 界面中，进入各插件（任务栏、开始菜单、通知中心、双击隐藏图标）的设置页面，调节透明色、浓度、模糊等参数。
4. 如需启用资源管理器透明，以管理员身份运行 **`ExplorerBlurMica\register.cmd`**；如需调整其参数，运行 `ExplorerBlurMica\config-editor-wpf.exe`。

---

## 💖 Support / 赞助支持

If you love this project and want to support its active development, you can sponsor me here:
如果您喜欢这个项目，并希望支持它的持续开发，欢迎赞助支持我：

👉 **[Sponsor on AFDian / 爱发电赞助通道](https://ifdian.net/a/Lanboo)**

---

## 📄 License
This project is licensed under the GPL-3.0 License. See the [LICENSE](LICENSE) file for details.

Developed with ❤️ by **Lanbo**.

Special thanks to:
* **Maplespe** for the outstanding [ExplorerBlurMica](https://github.com/Maplespe/ExplorerBlurMica) project.
* **m417z** for the original Windhawk Taskbar Styler, Notification Center Styler, and Start Menu Styler mods.
