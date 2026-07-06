# ZenDesktop Premium 🪟✨

[![GitHub License](https://img.shields.io/github/license/Liset999/ZenDesktop?color=blue&style=flat-square)](LICENSE)
[![Platform Windows](https://img.shields.io/badge/Platform-Windows%2011-0078d4?style=flat-square&logo=windows)](https://microsoft.com/windows)
[![Engine Windhawk](https://img.shields.io/badge/Engine-Windhawk%20C%2B%2B-ff69b4?style=flat-square)](https://windhawk.net)

**ZenDesktop Premium v4.0.0** is a high-performance desktop styling suite for Windows 11. It blends 4 native Win32/C++ Windhawk mods with the robust external **ExplorerBlurMica** program, unified under a bespoke Python GUI frontend panel for real-time customizations with **zero background process bloat, zero UI lag, and 0% CPU overhead**.

[简体中文](#-简体中文) | [English](#-english-features)

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

### 6. 🎛️ ZenDesktop Customizer (`ZenDesktopCustomizer.py` - Frontend Panel)
* Developed by **Lanbo**, this CustomTkinter GUI wrapper provides a unified dashboard to configure taskbar, start menu, and notification center options.
* Integrates quick actions for ExplorerBlurMica (Register/Enable, Configure Visuals, and Uninstall/Disable) via elevated actions.

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

### Step 4: Run Visual Control Center
1. Launch **`run_customizer.bat`** to start the frontend configuration GUI.
2. Go to the **资源管理器 (Explorer)** tab, click **注册/启用透明效果 (Register & Enable)** to apply File Explorer blur.
3. Use other tabs to customize Taskbar, Start Menu, and Notification Center styles.

---

## 🇨🇳 简体中文

### 💎 v4.0.0 核心更新与优势
* **四合一 Mod + 资源管理器透明程序**：弃用了以前不稳定的 C++ 资源管理器插件，替换为更成熟稳定的外部程序 **ExplorerBlurMica** (原作者: **Maplespe**)。
* **前端控制中心 (ZenDesktop Customizer)**：由 Lanbo 编写的前端面板，支持一键注册/注销 ExplorerBlurMica、打开高级编辑器以及调节其他 Windhawk 插件参数。
* **双击隐藏状态保存**：改进了桌面图标隐藏 Mod，使其在重启系统或 Explorer 后依然可以记住并保持隐藏状态。
* **纯本地化编译**：脱离官方云服务器连接限制，即插即用，完美解决国内连不上网的困境。
* **极致性能**：利用进程级注入与 Hook 技术，不占用任何后台常驻进程，0% CPU 开销，近乎 0MB 内存占用。

### 🛠️ 快速开始
1. 运行 **`deploy.bat`**（管理员身份）自动配置 Windhawk 并注册 4 个本地插件。
2. 在 Windhawk 界面中，依次进入 4 个插件（任务栏、开始菜单、通知中心、双击隐藏图标）的源码页面，点击右上角的 **保存并编译**。
3. 双击 **`run_customizer.bat`** 启动视觉控制中心。
4. 在“资源管理器”标签页中点击 **注册/启用透明效果** (Register & Enable) 以应用资源管理器毛玻璃。

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
