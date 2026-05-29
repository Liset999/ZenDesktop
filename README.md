# ZenDesktop Premium 🪟✨

[![GitHub License](https://img.shields.io/github/license/Liset999/ZenDesktop?color=blue&style=flat-square)](LICENSE)
[![Platform Windows](https://img.shields.io/badge/Platform-Windows%2011-0078d4?style=flat-square&logo=windows)](https://microsoft.com/windows)
[![Engine Windhawk](https://img.shields.io/badge/Engine-Windhawk%20C%2B%2B-ff69b4?style=flat-square)](https://windhawk.net)

**ZenDesktop Premium** is a high-performance, native Win32/C++ desktop styling suite for Windows 11. It brings elite-level desktop aesthetics with **zero background processes, zero UI lag, and 0% CPU overhead**.

Previously powered by TranslucentTB, this repository has been **completely rewritten and upgraded** to utilize a process-native C++ hooking architecture via **Windhawk**, offering unmatched system integration, stability, and premium aesthetics.

[简体中文](#-简体中文) | [English](#-english-features)

---

## 🌟 Premium Features

### 1. 🎛️ Taskbar Acrylic Styler (`local@zen-taskbar-acrylic`) [v3.0.0]
A native Windows 11 Taskbar beautification module offering fine-grained, premium transparency & blur presets:
* **Clear**: 100% full transparency (only taskbar icons remain).
* **Acrylic (High / Standard / Low)**: Real-time high-fidelity WinUI 3 acrylic glass effect.
* **Apple Liquid Glass / Alternate**: Hyper-transparent 3D droplet glass with a subtle chromatic dispersion border (featuring diagonal red→orange→green→blue→purple gradient stops), Fresnel specular edge reflections, and a precise 2px corner radius compensation (perfectly matching floating macOS-like Dock layout).
* **v3.0.0 Updates**: Swapped out all non-ASCII character markers to prevent locale-specific compiler token collapse (`missing terminating '"' character` errors) and aligned the metadata fully to version `3.0.0`.

### 2. 🔔 Notification Center Acrylic Styler (`local@zen-notificationcenter-acrylic`) [v3.0.0 NEW]
A premium notification and action center acrylic glass styler co-created by **Lanbo** and **m417z**:
* Brings high-fidelity real-time acrylic and frosted glass effects to the Windows 11 Notification Center, calendar panel, and Quick Settings/System Tray panels.
* Perfectly synchronized with your desktop theme presets, retaining flawless WinUI shadows and smooth animations.

### 3. 🚀 Start Menu Acrylic Styler (`local@zen-startmenu-acrylic`) [v3.0.0]
Syncs the Start Menu panel seamlessly with your taskbar theme, rendering native acrylic blur overlays over both redesigned and classic Start menu layouts, including expanded folders and search dialogs. Features **Apple Liquid Glass** preset with a clear liquid body, diagonal glass light sheen, adapted folder plates, and expanded panels with sub-pixel alignment.

### 4. 🖱️ Double-Click to Toggle Icons (`local@zen-desktop-toggle-icons`) [v3.0.0 Reconstructed]
A process-native desktop subclassing module. **Double-click empty desktop space to instantly hide/show icons.**
* **Asynchronous Message Pipeline**: Implements `PostMessageW` instead of synchronous notifications to eliminate Win32 message queue blocking and prevent Windows Explorer (`explorer.exe`) hangs.
* **Full-Screen Window Guard**: Automatically suspends double-click capture and auto-hide tracking when running full-screen games, video players, or active presentations, ensuring zero gameplay or media interference.
* **Physical Coordinate Guard (Zero-Latency)**: Instantly records and filters physical mouse coordinates during state transitions. This blocks synthetic `WM_MOUSEMOVE` messages triggered natively by `SysListView32` when toggling visibility, eliminating screen flicker with absolute zero latency.
* **Dynamic CS_DBLCLKS Support**: Automatically queries target window classes, ensuring zero interference with native double-click behaviors of standard system elements and third-party apps.
* **Robust Lifecycle Verification**: Incorporates double-buffered window subclass state checking on module destruction, preventing detached dangling hooks and ensuring Explorer stability.
* **Smart Auto-Restore & Auto-Hide**: Icons auto-restore instantly on keyboard/mouse input. Active user double-click toggles are respected and will not be overridden by auto-restore loops.

---

## 💡 Why Windhawk C++ Native Hooks over TranslucentTB?

| Feature | ZenDesktop (Windhawk C++) | TranslucentTB |
| :--- | :--- | :--- |
| **Execution Path** | Injected directly inside `explorer.exe` | Separate background `.exe` process |
| **CPU Overhead** | **0%** (uses native OS rendering cycles) | Periodic poll / active rendering |
| **RAM Footprint** | **~0 MB** (virtual memory mapped) | 15MB - 40MB background footprint |
| **Reliability** | Native hook recovery on explorer crashes | Prone to UI disconnects / crashes |
| **Aesthetic Depth** | Fine-grained custom BlurAmount & Tint | Standard OS transparency calls |

---

## 📥 Installation & Deployment Guide

> [!IMPORTANT]
> The suite utilizes **pure local compilation** (`local@` prefix), which means your system compiles the C++ code natively. It is 100% offline-ready, safe, and completely bypasses official Windhawk mod server connection failures!

### Step 1: Install Windhawk
Download and install [Windhawk](https://windhawk.net) on your Windows 11 PC.

### Step 2: One-Key Local Registry & Mod Deployment
1. Download this repository and extract it.
2. Right-click **`deploy.bat`** and select **Run as Administrator** (以管理员身份运行).
3. The script will automatically stop the Windhawk service, register the 4 premium local mods, enable local compilation, reset compiler caching keys, and restart Windhawk safely.

### Step 3: Fast Native Compilation
1. Open the **Windhawk** user interface. You will see 4 newly registered local mods in your home dashboard.
2. Click into **ZenDesktop: Taskbar Acrylic Styler** and click **Save / Compile** (保存并编译). The engine will compile the native C++ code in ~10 seconds.
3. Repeat the compilation step for the other active mods (**Start Menu Acrylic Styler**, **Notification Center Acrylic Styler**).
4. In the settings dropdown under **Theme**, choose your favorite transparency preset!

---

## 📁 Repository Structure
```
ZenDesktop/
├── local@zen-taskbar-acrylic.wh.cpp          # C++ Source Code (Taskbar Acrylic Styler)
├── local@zen-notificationcenter-acrylic.wh.cpp # C++ Source Code (Notification Center Acrylic Styler)
├── local@zen-startmenu-acrylic.wh.cpp        # C++ Source Code (Start Menu Acrylic Styler)
├── local@zen-desktop-toggle-icons.wh.cpp      # C++ Source Code (Double-Click Icon Toggle)
├── deploy.bat                                # One-Key Admin Deployment Script
├── Readme.txt                                # Compact Chinese User Guide
└── README.md                                 # GitHub Homepage
```

---

## 🇨🇳 简体中文

### 💎 核心优势
* **纯本地化编译**：脱离官方云服务器连接限制，即插即用，完美解决国内连不上网的困境。
* **彻底告别乱码**：全面采用标准纯英及 ASCII 标签，完全杜绝 Windows 编码带来的乱码 Bug。
* **极致性能**：利用进程级注入与 Hook 技术，不占用任何后台常驻进程，0% CPU 开销，近乎 0MB 内存占用。
* **Apple Liquid Glass (苹果流体玻璃) 主题**：引入超写实拟真玻璃美学！完美的圆角对齐系统（外圆角 20px，内圆角 18px，子元素圆角 10px）搭配五彩斑斓的虹彩边缘与 Fresnel spec 高光，超透且清晰。
* **v3.0.0 精构化通知中心 (2026/05/29)**：
  * **深度共创**：携手 Windhawk 官方作者 m417z 深度共创通知中心亚克力插件，完美覆盖 Win11 通知中心、日历及系统控制中心托盘。
* **v3.0.0 桌面双击隐藏重构 (2026/05/30)**：
  * **异步管道（PostMessageW）**：彻底打通异步消息管道，绝不阻塞 Win32 主消息循环，彻底避免因消息堆积引起的桌面假死和资源管理器未响应。
  - **全屏程序避让守护**：智能侦测当前是否存在全屏运行的游戏或影音程序。全屏状态下自动挂起动作拦截，防止玩游戏时误触隐藏桌面图标。
  - **物理坐标去闪拦截（物理坐标哨兵）**：利用高精度物理坐标过滤，完美抹除因 `SysListView32` 切换显示状态触发的系统模拟 `WM_MOUSEMOVE` 闪烁，实现零延迟、零画幅撕裂的平滑过渡。
  - **动态 CS_DBLCLKS 支持**：动态查询系统窗口类型，保护系统自带及第三方应用的双击逻辑不受任何干扰。
  - **稳健生命周期（双重销毁校验）**：在模块卸载时通过双重安全校验，平滑销毁子类化钩子，100% 避免钩子残留造成的 Explorer 崩溃。

### 🛠️ 快速开始
1. 下载并安装 [Windhawk](https://windhawk.net) 引擎。
2. 鼠标右键点击本仓库中的 **`deploy.bat`**，选择 **以管理员身份运行**。
3. 打开 Windhawk 软件，进入各本地插件页（Taskbar, Start Menu, Notification Center），点击右上角的 **保存并编译**。
4. 编译完成后，在设置选项的 **Theme** 下拉菜单中挑选您喜爱的高级透明度与亚克力效果预设！

---

## 📄 License
This project is licensed under the GPL-3.0 License. See the [LICENSE](LICENSE) file for details.

Developed with ❤️ by **Lanbo**.

Special thanks to **m417z** for the original Windhawk Taskbar Styler, Notification Center Styler, and Start Menu Styler mods which served as the codebase foundation for these styling modules.
