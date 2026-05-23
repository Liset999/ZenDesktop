# 🖥️ ZenDesktop

<p align="center">
  <strong style="font-size: 1.5em;">ZenDesktop (DesktopTaskbar-Fusion)</strong><br>
  <sub>A ultra-lightweight, high-performance, and native Windows enhancement utility to achieve the ultimate aesthetic desktop.</sub>
</p>

<p align="center">
  <a href="#-features">Features</a> •
  <a href="#-technical-highlights">Technical Highlights</a> •
  <a href="#-quick-start">Quick Start</a> •
  <a href="#-中文说明">中文介绍</a> •
  <a href="#-license">License</a>
</p>

---

## 🌟 Overview

**ZenDesktop** is an extremely lightweight and native-feeling Windows utility designed to declutter your workspace and elevate your desktop aesthetics. It achieves two major enhancements using direct, non-invasive Windows APIs:

1. 🖱️ **Desktop Zen Mode**: Double-click empty space on your desktop to instantly hide/show desktop icons. It uses **process-native hit-testing** so double-clicking folders, files, or shortcuts continues to open them normally, while double-clicking the empty wallpaper toggles icon visibility.
2. 🎨 **Fluent Taskbar Effects**: Orchestrates taskbar visual styles with precision—supporting full transparency, frosted glass, or hardware-accelerated **Microsoft Acrylic** styling with seamless multi-monitor sync and flicker-free persistence.

With a running footprint of **< 15MB RAM and 0% CPU**, ZenDesktop sits quietly in your system tray without bloated UI frameworks (like Electron or Qt).

---

## ✨ Features

- **High-Precision Hit Testing**: Leverages Windows List-View control API (`LVM_HITTEST`) to accurately detect empty wallpaper grids versus actual icons.
- **Native Implementation**: Uses undocumented system commands (`0x7402`) to toggle icon visibility naturally. Desktop layouts are preserved perfectly, with smooth native fade animations.
- **Hardware Accelerated Visuals**: Renders premium Acrylic visual materials using `SetWindowCompositionAttribute` from the DirectComposition system.
- **Multi-Monitor Cohesion**: Dynamically detects all connected displays, applying uniform visual styles without breaking when displays are added or removed.
- **Anti-Flicker Daemon**: Protects your styling against Explorer-initiated style resets (which usually occur when virtual desktops are switched or the Windows Control Center is toggled) with a sub-millisecond defensive hook.

---

## 🛠️ Technical Highlights

### 1. Dual-Process Empty Space Detection
Instead of listening to naive coordinate-based clicks, ZenDesktop performs cross-process memory inspection against `SysListView32`.

```
[Global Mouse Hook] ---> Left Double Click Detected
                               |
                   [Get Window Class Under Mouse]
                               |
                Is it "SysListView32" (Desktop Grid)?
                     /                   \
                   YES                    NO (WorkerW / ShellView)
                   /                         \
    [Cross-Process LVM_HITTEST]           [Direct Toggle] 
    Query icon under mouse coordinates            |
           /                 \            (Guarantees restore works)
    Hit An Icon           Hit Empty Space
       /                       \
  [Do Nothing]           [Toggle Icons] ---> WM_COMMAND (0x7402)
 (Default Open)          (Clean Fade Out)
```

### 2. Zero-Overhead Tray & Reg Config
Developed purely in Python with `pywin32` bindings and built with `PyInstaller`, ZenDesktop runs as a headless Win32 daemon with a fully-native, elegant context tray menu for style customization and startup registry settings.

---

## 🚀 Quick Start

### For Users (Pre-compiled Portable EXE)
1. Head over to the **Releases** tab and download the latest `ZenDesktop.exe`.
2. Place it anywhere and double-click to run. It will instantly initialize your taskbar and nestle into your system tray.
3. Right-click the system tray icon to choose your favorite Taskbar styling (Acrylic / Transparent / Blur / Default) or toggle startup behavior.

### For Developers
1. Clone the repository:
   ```bash
   git clone https://github.com/Lanbo-creative/ZenDesktop.git
   cd ZenDesktop
   ```
2. Install Windows OS binding dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Run the daemon:
   ```bash
   python app.py
   ```

---

## 🇨🇳 中文说明

**ZenDesktop** 是一款极致纯净、无任何视觉遮挡的 Windows 桌面与任务栏美化注入工具。通过调用 Windows 原生未公开 API 接口，实现以下核心功能：

### 🌟 核心特色：
- **双击桌面智能隐显**：基于 `LVM_HITTEST` 进程间坐标碰撞算法，能够百分之百精准地区分“空白网格”与“快捷方式图标”。双击真正的文件图标时触发系统原生“打开”动作，只有双击真正的壁纸空白处才会瞬间收起所有桌面图标。
- **流畅原生无缝过度**：本工具不同于传统的“强制消隐窗口”手法，而是通过直接发送底层 `WM_COMMAND (0x7402)` 消息，让系统主动触发原生“显示桌面图标”切换，确保绝对不打乱你的桌面布局，更不会卡死 Explorer。
- **高级硬件加速亚克力效果**：采用微软原生 DirectComposition 级 `SetWindowCompositionAttribute` API 属性渲染，为您的任务栏赋予磨砂玻璃 (Blur) 及极其高级的霜降亚克力 (Acrylic) 光泽。
- **完美多屏与抗强制重置**：完美适配多个显示器。针对 Win11 切换虚拟桌面或打开控制中心强制还原任务栏的顽疾，内置了 200ms 防闪烁自愈重绘引擎，秒级自动校准恢复。
- **极致轻量绿色**：基于原生 Win32 API 交互与 `pystray` 轻量化系统托盘泵，无需搭载任何大体量 UI 框架（如 PyQt 或 Electron）。运行时内存仅仅占用 **< 15MB，CPU 占用率无限接近 0%**。

### 🚀 快速启动（中文）：
1. 直接在项目 `dist/` 文件夹下找到编译好的 `DesktopTaskbarHelper.exe`。
2. 双击运行即可。程序会自动收纳到 Windows 系统右下角的**任务托盘**中（电脑屏幕图标）。
3. 右键托盘图标可进行菜单配置：
   - **Taskbar Style**：在 `Default(默认)`、`Transparent(全透明)`、`Blur(毛玻璃)`、`Acrylic(亚克力)` 四种视觉材质中一键切换。
   - **Double Click Hide**：勾选或取消勾选该功能。
   - **Start on Boot**：勾选后，程序将安全写入当前用户的注册表启动项，开机静默运行。

---

## 🛡️ License

This project is open-sourced under the [MIT License](LICENSE). Feel free to customize, submit PRs, and build upon this core!
