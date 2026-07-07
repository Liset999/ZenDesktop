# ZenDesktop Premium 🪟✨

[![GitHub License](https://img.shields.io/github/license/Liset999/ZenDesktop?color=blue&style=flat-square)](LICENSE)
[![Platform Windows](https://img.shields.io/badge/Platform-Windows%2011-0078d4?style=flat-square&logo=windows)](https://microsoft.com/windows)
[![Engine Windhawk](https://img.shields.io/badge/Engine-Windhawk%20C%2B%2B-ff69b4?style=flat-square)](https://windhawk.net)

**ZenDesktop Premium v4.0.0** is a high-performance desktop styling suite for Windows 11. It blends native Win32/C++ Windhawk mods with the robust external **ExplorerBlurMica** program, with all real-time customizations handled natively via the Windhawk UI **with zero background process bloat, zero UI lag, and 0% CPU overhead**.

[简体中文](#-简体中文) | [English](#-english-features)

---

## 🚀 Latest Performance & UI Tuning Updates / 最新性能与界面调优更新

### 🌐 English
* **Start Menu Folder Independent Customization**: Folders now support separate background color, opacity, tint, and acrylic blur configs. Fixed preset override issues in "Follow Main Panel" mode.
* **XAML Render Hot-Path & Stuttering Fix**: Removed synchronous debugging telemetry from the main rendering loops of the Start Menu, Taskbar, and Notification Center mods. This prevents UI stuttering, delays, and empty option blocks when opening the Quick Settings panel (Win+A) or Start Menu.
* **Built-in Start Menu Custom Sizing**: Merged dimension settings directly into the Start Menu Acrylic Styler. Users can now adjust the width and height of the Start Menu panel natively.
* **File Explorer Transparency & Visual Config Editor**: Integrated `ExplorerBlurMica`. Due to Windows OS restrictions, File Explorer cannot be made 100% fully transparent (always retains frosted blur) and the title bar reverts to a solid color when losing focus. Provided a custom-made WPF frontend (`config-editor-wpf.exe`) to easily configure visual parameters.
* **New Companion Mods Support**:
  * **macos-minimize-animation**: Renders high-fidelity, fluid macOS-style Genie window minimize/restore and transition animations.
  * **taskbar-background-helper**: Automatically transitions the taskbar background to dark, blurred, or acrylic when a window is maximized. This replaces our old custom maximized window logic, solving previous performance bottlenecks.

### 🇨🇳 中文
* **开始菜单文件夹独立样式自定义**：开始菜单内的文件夹现在支持独立调节背景颜色、透明度、浓度与毛玻璃模糊度；修复了开启“跟随主面板”选项时文件夹特定参数失效的 Bug。
* **XAML 渲染热路径优化与卡顿修复**：清除了三大核心 Mod（开始菜单、任务栏、通知中心）中高频 UI 重绘事件的调试日志，大幅提升渲染性能，彻底解决了打开控制栏（Win+A 快捷设置）、开始菜单或任务栏时的选项缺失、加载延迟或瞬间空白白屏的卡顿问题。
* **开始菜单支持自定义大小**：直接在开始菜单 Mod 设置中整合了宽度和高度的自定义功能，用户可根据屏幕分辨率和个人喜好自由缩放开始菜单面板。
* **资源管理器透明化与专属配置前端**：集成了外部透明程序 `ExplorerBlurMica`。受 Windows 底层限制，资源管理器无法实现 100% 纯透明（会有模糊），且窗口失去焦点时标题栏会回退为纯色。为此配备了专属的参数调节前端（`config-editor-wpf.exe`），极大降低了用户调节模糊半径与颜色的门槛。
* **引入两大全新辅助 Mod 支持**：
  * **macos-minimize-animation**：切换与最小化程序时，实现媲美 macOS 的 Genie（神奇效果）动画，流畅度极佳。
  * **taskbar-background-helper**：窗口最大化时任务栏可秒级切换为深色/模糊/亚克力。替换了原任务栏 Mod 内置的最大化检测逻辑，彻底解决了旧版的响应延迟与性能瓶颈。

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

### 6. 🎨 macOS Genie Minimize Animation (`local@macos-minimize-animation`)
* Brings the iconic macOS-style genie minimize and restore (open) animations to every window on Windows 11/10.

### 7. 🎛️ Taskbar Background Helper (`local@taskbar-background-helper`)
* Automatically turns the taskbar background to dark, blurred, or acrylic whenever a window is maximized, keeping the main Taskbar mod clean and high-performance.

---

## 📥 Installation & Deployment Guide

> [!IMPORTANT]
> The suite utilizes **pure local compilation** (`local@` prefix), which means your system compiles the C++ code natively. It is 100% offline-ready, safe, and completely bypasses official Windhawk mod server connection failures!

### Step 1: Install Windhawk
Install Windhawk on your Windows 11 PC using the provided setup.

### Step 2: One-Key Local Registry & Mod Deployment
1. Right-click **`deploy.bat`** and select **Run as Administrator** (以管理员身份运行).
2. The script will automatically stop the Windhawk service, register the 6 premium local mods, enable local compilation, reset compiler caching keys, and restart Windhawk safely.

### Step 3: Fast Native Compilation
1. Open the **Windhawk** user interface. You will see 6 newly registered local mods in your home dashboard.
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
