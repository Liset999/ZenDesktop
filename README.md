# ZenDesktop Premium 🪟✨

[![GitHub License](https://img.shields.io/github/license/Liset999/ZenDesktop?color=blue&style=flat-square)](LICENSE)
[![Platform Windows](https://img.shields.io/badge/Platform-Windows%2011-0078d4?style=flat-square&logo=windows)](https://microsoft.com/windows)
[![Engine Windhawk](https://img.shields.io/badge/Engine-Windhawk%20C%2B%2B-ff69b4?style=flat-square)](https://windhawk.net)

**ZenDesktop Premium** is a high-performance, native Win32/C++ desktop styling suite for Windows 11. It brings elite-level desktop aesthetics with **zero background processes, zero UI lag, and 0% CPU overhead**.

Previously powered by TranslucentTB, this repository has been **completely rewritten and upgraded** to utilize a process-native C++ hooking architecture via **Windhawk**, offering unmatched system integration, stability, and premium aesthetics.

[简体中文](#-简体中文) | [English](#-english-features)

---

## 🌟 Premium Features

### 1. 🎛️ Taskbar Acrylic Styler (`local@zen-taskbar-acrylic`)
A native Windows 11 Taskbar beautification module offering fine-grained, premium transparency & blur presets:
* **Clear**: 100% full transparency (only taskbar icons remain).
* **Light Fog (High / Standard / Low)**: An elegant misty frost overlay with configurable遮罩 (opacity).
* **Acrylic (High / Standard / Low)**: Real-time high-fidelity WinUI 3 acrylic glass effect.
* **Dark Glass (High / Standard / Low)**: Sleek, premium dark mode smoked glass.
* **Frosted White**: Snow-white premium frosted glass design.

### 2. 🚀 Start Menu Acrylic Styler (`local@zen-startmenu-acrylic`)
Syncs the Start Menu panel seamlessly with your taskbar theme, rendering native acrylic blur overlays over both redesigned and classic Start menu layouts, including expanded folders and search dialogs.

### 3. 🖱️ Double-Click to Toggle Icons (`local@zen-desktop-toggle-icons`)
A process-native desktop subclassing module. **Double-click empty desktop space to instantly hide/show icons.**
* Uses native Win32 hit-testing: double-clicking files or folders triggers their default actions normally.
* Intercepts messages directly inside `explorer.exe` shell views. Zero lag, zero background EXEs.

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
3. The script will automatically stop the Windhawk service, register the 3 premium local mods, enable local compilation, and restart Windhawk safely.

### Step 3: Fast Native Compilation
1. Open the **Windhawk** user interface. You will see 3 newly registered local mods in your home dashboard.
2. Click into **ZenDesktop: Taskbar Acrylic Styler** and click **Save / Compile** (保存并编译). The engine will compile the native C++ code in ~10 seconds.
3. Repeat the compilation step for **ZenDesktop: Start Menu Acrylic Styler**.
4. In the settings dropdown under **Theme**, choose your favorite transparency preset!

---

## 📁 Repository Structure
```
ZenDesktop/
├── local@zen-taskbar-acrylic.wh.cpp        # C++ Source Code (Taskbar)
├── local@zen-startmenu-acrylic.wh.cpp      # C++ Source Code (Start Menu)
├── local@zen-desktop-toggle-icons.wh.cpp    # C++ Source Code (Icon Toggle)
├── deploy.bat                              # One-Key Admin Deployment Script
├── Readme.txt                              # Compact Chinese User Guide
└── README.md                               # GitHub Homepage
```

---

## 🇨🇳 简体中文

### 💎 核心优势
* **纯本地化编译**：脱离官方云服务器连接限制，即插即用，完美解决国内连不上网的困境。
* **彻底告别乱码**：全面采用标准纯英及 ASCII 标签，完全杜绝 Windows 编码带来的乱码 Bug。
* **极致性能**：利用进程级注入与 Hook 技术，不占用任何后台常驻进程，0% CPU 开销，近乎 0MB 内存占用。

### 🛠️ 快速开始
1. 下载并安装 [Windhawk](https://windhawk.net) 引擎。
2. 鼠标右键点击本仓库中的 **`deploy.bat`**，选择 **以管理员身份运行**。
3. 打开 Windhawk 软件，进入 **ZenDesktop: Taskbar Acrylic Styler** 和 **ZenDesktop: Start Menu Acrylic Styler** 本地插件页，分别点击右上角的 **保存并编译**。
4. 编译完成后，在设置选项的 **Theme** 下拉菜单中挑选您喜爱的高级透明度与亚克力效果预设！

---

## 📄 License
This project is licensed under the GPL-3.0 License. See the [LICENSE](LICENSE) file for details.

Developed with ❤️ by **Lanbo & Antigravity Premium Design**.
