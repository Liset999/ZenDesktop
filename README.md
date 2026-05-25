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
* **Apple Liquid Glass / Alternate**: Hyper-transparent 3D droplet glass with a subtle chromatic dispersion border (featuring diagonal red→orange→green→blue→purple gradient stops), Fresnel specular edge reflections, and a precise 2px corner radius compensation (perfectly matching floating macOS-like Dock layout).
  * **v2.7.0 Premium Aesthetics Refinement**:
    * *Zero Border Line on Full-Width*: Eliminated the top horizontal border line in full-width themes to achieve a perfectly borderless, pure liquid glass edge (specular border line is beautifully preserved only in floating Alternate Dock layouts).
    * *Date & Time Restoration*: Fully removed override styles for `TimeInnerTextBlock` and `DateInnerTextBlock`, restoring the native dual-line (time on top, date on bottom) align layout for Windows 11 system tray.
    * *ASCII Code Cleaning*: Stripped risky multi-byte character separators (like `═`) in file comments to prevent compiler token collapse (`missing terminating '"' character` syntax errors) under local system non-UTF-8 locales (e.g. GBK/ANSI).

### 2. 🚀 Start Menu Acrylic Styler (`local@zen-startmenu-acrylic`)
Syncs the Start Menu panel seamlessly with your taskbar theme, rendering native acrylic blur overlays over both redesigned and classic Start menu layouts, including expanded folders and search dialogs. Features **Apple Liquid Glass** preset with clear liquid body, diagonal glass light sheen, adapted folder plates, and expanded panels with sub-pixel alignment.

### 3. 🖱️ Double-Click to Toggle Icons (`local@zen-desktop-toggle-icons`)
A process-native desktop subclassing module. **Double-click empty desktop space to instantly hide/show icons.**
* Uses native Win32 hit-testing: double-clicking files or folders triggers their default actions normally.
* Intercepts messages directly inside `explorer.exe` shell views. Zero lag, zero background EXEs.
* **Note**: Now disabled by default in `deploy.bat` to guarantee 100% stability and prevent the known Explorer flashing/crash bugs on Windows 11 Build 26100 (24H2). You can still choose to manually compile and enable it inside the Windhawk UI if your build is compatible.

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
* **Apple Liquid Glass (苹果流体玻璃) 主题**：v2.7.0 重磅引入超写实拟真玻璃美学！完美的圆角对齐系统（外圆角 20px，内圆角 18px，子元素圆角 10px）搭配五彩斑斓的虹彩边缘与 Fresnel spec 高光，超透且清晰。
  * **v2.7.0 精致化视觉重构与修复（2026/05/25）**：
    * **极致去横线**：彻底消除全宽屏幕任务栏顶部的突兀渐变边框线，只在悬浮 Dock 卡片模式下保留高亮亮边，达成纯净无痕贴底的完美通透感。
    * **恢复原生双行日期**：清空所有对托盘时间与日期的样式劫持，完美还原 Win11 经典的上下双行（时间在上、日期在下）居右排版。
    * **消灭编译报错**：大扫除代码注释里所有导致 GBK 编码下编译失败的非 ASCII 字符（如 `═` 等），彻底铲除 `missing terminating '"'` 错误，实现 100% 顺畅本地秒编译。
* **100% 稳定性保障**：默认禁用易引发 Windows 11 24H2 / Build 26100 系统黑屏/闪屏的双击隐藏图标插件，稳定性达工业级，免去后顾之忧。

### 🛠️ 快速开始
1. 下载并安装 [Windhawk](https://windhawk.net) 引擎。
2. 鼠标右键点击本仓库中的 **`deploy.bat`**，选择 **以管理员身份运行**。
3. 打开 Windhawk 软件，进入 **ZenDesktop: Taskbar Acrylic Styler** 和 **ZenDesktop: Start Menu Acrylic Styler** 本地插件页，分别点击右上角的 **保存并编译**。
4. 编译完成后，在设置选项的 **Theme** 下拉菜单中挑选您喜爱的高级透明度与亚克力效果预设！

---

## 📄 License
This project is licensed under the GPL-3.0 License. See the [LICENSE](LICENSE) file for details.

Developed with ❤️ by **Lanbo**.

Special thanks to **m417z** for the original Windhawk Taskbar Styler and Start Menu Styler mods which served as the codebase foundation for these styling modules.
