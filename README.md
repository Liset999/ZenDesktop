# ZenDesktop Premium 🪟✨

[![GitHub License](https://img.shields.io/github/license/Liset999/ZenDesktop?color=blue&style=flat-square)](LICENSE)
[![Platform Windows](https://img.shields.io/badge/Platform-Windows%2011-0078d4?style=flat-square&logo=windows)](https://microsoft.com/windows)
[![Engine Windhawk](https://img.shields.io/badge/Engine-Windhawk%20C%2B%2B-ff69b4?style=flat-square)](https://windhawk.net)

**ZenDesktop Premium v4.1.0** is a high-performance desktop styling suite for Windows 11. It blends native Win32/C++ Windhawk mods with the robust external **ExplorerBlurMica** program, with all real-time customizations handled natively via the Windhawk UI **with zero background process bloat, zero UI lag, and 0% CPU overhead**.

👨‍💻 **Developer Links & Support / 关注与支持**:
- 📺 **Bilibili**: https://space.bilibili.com/1280632999
- 🎵 **Douyin**: https://v.douyin.com/Isl7QPpKZZ4/
- ⚡ **Afdian**: https://ifdian.net/a/Lanboo

[简体中文](#-简体中文) | [English](#-english-features)

---

## 🚀 Latest Updates / 最新更新 (v4.1.0)

### 🌐 English
* **Standardized Chinese Localization & Mod Naming**: Unified all core mod display names and descriptions with full bilingual settings translation in Windhawk.
* **Modern Direct2D macOS Minimize Animation**: Integrated the latest Direct2D mesh warp genie engine by Abdullah Masood & Potassiumuncher, supporting customizable mesh resolution (8-96 tiles), taskbar auto-hide unhiding, and process exclusions.
* **Windows Settings Light-Mode Transparency Fix**: Enhanced Settings Styler with full recursive border styling, resolving opaque white cards in light mode for a seamless frosted glass look.
* **Refined One-Key Deploy Script (`deploy.ps1`)**: Auto-detects and deploys all 6 active mods, automatically cleans up legacy deprecated helper mods, and resets the UI cache for instant Chinese mod card display.

### 🇨🇳 中文
* **全系列模组命名与中文本地化规范**：模组统一采用规范的中文命名与中文简介（保留 `ZenDesktop: ` 前缀），所有配置项均已完成中英双语对照翻译。
* **全新 Direct2D macOS 神奇效果动画引擎**：深度整合 Potassiumuncher & Abdullah Masood 的 Direct2D 网格变形引擎，支持网格细分密度调节、自动隐藏任务栏唤出、排除程序列表及多显示器适配。
* **设置美化亮色模式全面透明优化**：彻底修复亮色模式下设置卡片泛白不透明的问题，统一 Glass 模式全景磨砂质感。
* **一键部署引擎 (`deploy.ps1`) 升级**：自动扫描并注册 6 大核心 Mod，自动清理历史废弃插件并重置 UI 缓存，确保在 Windhawk 中即刻展现全中文模组列表。

---

## 🌟 English Features

### 1. 🎛️ Taskbar Acrylic Styler (`local@zen-taskbar-acrylic` / `ZenDesktop: 任务栏美化`)
* High-performance WinUI 3 acrylic and frosted glass effects for the Windows 11 taskbar.
* Clear, Acrylic, and Apple Liquid Glass presets with integrated auto-adaptation for maximized windows.

### 2. 🔔 Notification Center Acrylic Styler (`local@zen-notificationcenter-acrylic` / `ZenDesktop: 通知栏美化`)
* Real-time acrylic and frosted glass effects for Notification Center, Calendar panel, and Quick Settings (Win+A).
* Retains flawless WinUI shadows and smooth opening/closing transitions.

### 3. 🚀 Start Menu Acrylic Styler (`local@zen-startmenu-acrylic` / `ZenDesktop: 开始菜单美化`)
* Native acrylic blur overlays over the Windows 11 Start Menu, with built-in width/height custom sizing and independent folder opacity/blur controls.

### 4. 🖱️ Desktop Icon Toggle (`local@zen-desktop-toggle-icons` / `ZenDesktop: 桌面双击隐藏图标`)
* Double-click empty desktop space to instantly hide/show icons with smooth fade animations. State is saved and preserved across system restarts.

### 5. ⚙️ Settings Acrylic Styler (`local@windows-11-settings-styler-fork` / `ZenDesktop: 设置美化`)
* Full acrylic/frosted glass customization for the Windows 11 Settings app (Win+I), deeply adapted for both Light and Dark modes.

### 6. 🎨 macOS Genie Minimize Animation (`local@macos-minimize-animation` / `macOS 神奇效果最小化动画`)
* Fluid macOS-style Genie minimize and restore animations powered by a high-performance Direct2D mesh engine.

### 7. 📁 File Explorer Transparency (`ExplorerBlurMica`)
* Author: **Maplespe** ([Maplespe/ExplorerBlurMica](https://github.com/Maplespe/ExplorerBlurMica)).
* Adds Blur, Acrylic, and Mica effects to Windows 10/11 File Explorer.

---

## 📥 Installation & Deployment Guide

> [!IMPORTANT]
> The suite utilizes **pure local compilation** (`local@` prefix), which means your system compiles the C++ code natively. It is 100% offline-ready, safe, and completely bypasses official Windhawk mod server connection failures!

### Step 1: Run One-Key Deploy
1. Right-click **`deploy.bat`** and select **Run as Administrator** (以管理员身份运行).
2. The script will automatically configure Windhawk, register the 6 local mods, initialize default settings, and restart the Windhawk service.

### Step 2: Native Compilation in Windhawk
1. Open the **Windhawk** UI dashboard.
2. Click into each mod and click **Save / Compile** (保存并编译). The engine will compile the native C++ code in ~5-10 seconds.

### Step 3: Configure via Windhawk UI
1. In the **Windhawk** UI, open each mod's **Settings** page to customize visual options in real-time.
2. To enable File Explorer transparency, run **`ExplorerBlurMica\register.cmd`** as Administrator.

---

## 🇨🇳 简体中文

### 🛠️ 快速开始
1. 鼠标右键点击 **`deploy.bat`**，选择 **以管理员身份运行**。
2. 脚本将自动检测 Windhawk、部署 6 大核心本地 Mod、清理历史插件并刷新缓存。
3. 打开 Windhawk 软件，依次进入各插件源码页面，点击右上角的 **保存并编译** 即可完成激活！
4. 如需启用资源管理器透明，以管理员身份运行 **`ExplorerBlurMica\register.cmd`**。

---

## 💖 Support / 赞助支持

- 📺 **哔哩哔哩**: https://space.bilibili.com/1280632999
- 🎵 **抖音**: https://v.douyin.com/Isl7QPpKZZ4/
- ⚡ **爱发电**: https://ifdian.net/a/Lanboo

---

## 📄 License
This project is licensed under the GPL-3.0 License. See the [LICENSE](LICENSE) file for details.

Developed with ❤️ by **Lanbo**.
Special thanks to:
* **Maplespe** for ExplorerBlurMica.
* **m417z** for Windhawk core framework.
* **Abdullah Masood & Potassiumuncher** for macOS Minimize Animation.
