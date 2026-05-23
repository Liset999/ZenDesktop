# 🖥️ ZenDesktop

<p align="center">
  <strong style="font-size: 1.8em;">ZenDesktop</strong><br>
  <sub>A ultra-lightweight, high-performance, and native Windows enhancement utility to achieve the ultimate aesthetic desktop.</sub>
</p>

<p align="center">
  <a href="#-features">Features</a> •
  <a href="#-installation-modes">Installation Modes</a> •
  <a href="#-technical-highlights">Technical Highlights</a> •
  <a href="#-中文介绍">中文介绍</a> •
  <a href="#-license">License</a>
</p>

---

## 🌟 Overview

**ZenDesktop** is an extremely lightweight and native-feeling Windows utility designed to declutter your workspace and elevate your desktop aesthetics. It achieves two major enhancements using direct, non-invasive Windows APIs:

1. 🖱️ **Desktop Zen Mode**: Double-click empty space on your desktop to instantly hide/show desktop icons. It uses **process-native hit-testing** so double-clicking folders, files, or shortcuts continues to open them normally, while double-clicking the empty wallpaper toggles icon visibility.
2. 🎨 **Fluent Taskbar Effects**: Orchestrates taskbar visual styles with precision—supporting full transparency, frosted glass, or hardware-accelerated **Microsoft Acrylic** styling with seamless multi-monitor sync and flicker-free persistence.

With ZenDesktop, you can enjoy a beautifully minimal desktop experience with **zero clutter**.

---

## ✨ Features

- **High-Precision Hit Testing**: Leverages Windows List-View control API (`LVM_HITTEST`) to accurately detect empty wallpaper grids versus actual icons.
- **Native Implementation**: Uses undocumented system commands (`0x7402`) to toggle icon visibility naturally. Desktop layouts are preserved perfectly, with smooth native fade animations.
- **Hardware Accelerated Visuals**: Renders premium Acrylic visual materials using `SetWindowCompositionAttribute` from the DirectComposition system.
- **Multi-Monitor Cohesion**: Dynamically detects all connected displays, applying uniform visual styles without breaking when displays are added or removed.
- **Anti-Flicker Daemon**: Protects your styling against Explorer-initiated style resets (which usually occur when virtual desktops are switched or the Windows Control Center is toggled) with a sub-millisecond defensive hook.

---

## 🚀 Installation Modes

To suit both casual users and extreme performance purists, ZenDesktop is offered in two distribution formats:

### Mode A: Standard Portable App (With Taskbar Styling & System Tray)

This mode runs as a lightweight Python-based background daemon, providing visual taskbar styling (frosted glass, Acrylic, transparent) alongside the double-click-to-hide feature, complete with a system tray menu.

1. Go to the [Releases](https://github.com/Liset999/ZenDesktop/releases) page and download **`ZenDesktop_Portable.zip`**.
2. **Extract the entire zip folder** to a directory of your choice.
   > [!IMPORTANT]
   > Do **NOT** run the executable inside the zip without extracting, and do **NOT** move the executable out of its folder. It relies on the local `ext/` directory to coordinate taskbar styling safely without DLL version conflicts.
3. Run **`DesktopTaskbarHelper.exe`**.
4. Right-click the system tray icon (computer screen icon) to configure:
   - **Taskbar Style**: Toggle between *Transparent*, *Acrylic (frosted glass)*, *Blur*, and *Default*.
   - **Double Click Hide**: Enable or disable the double-click empty space detection.
   - **Start on Boot**: Save to registry to start automatically on Windows startup.

### Mode B: Pure C++ Windhawk Mod (For Power Users & Purists)

If you are already using [Windhawk](https://windhawk.net/), the popular Windows customization engine, you can run ZenDesktop as a native C++ mod. 

- **Zero background processes**: Runs natively inside `explorer.exe` process memory.
- **Extreme performance**: 0% CPU and **< 1MB RAM** footprint. No tray icons, no background daemons.

**How to Install:**
1. Open **Windhawk** on your computer.
2. Go to the **Developer Writing** tab (or click **Create Mod**).
3. Copy the entire contents of [ZenDesktop_Windhawk_Mod.wh.cpp](ZenDesktop_Windhawk_Mod.wh.cpp) from this repository and paste it into the Windhawk editor.
4. Click **Compile** and **Save**. Windhawk will immediately compile the mod and apply it to your system. Double-click empty space on your desktop to enjoy!

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
Developed purely with `pywin32` bindings, the standard portable mode runs as a headless Win32 daemon with a fully-native, elegant context tray menu for style customization and startup registry settings.

---

## 🇨🇳 中文介绍

**ZenDesktop** 是一款极致纯净、无任何视觉遮挡的 Windows 桌面与任务栏美化注入工具。通过调用 Windows 原生未公开 API 接口，实现以下核心功能：

### 🌟 核心特色：
- **双击桌面智能隐显**：基于 `LVM_HITTEST` 进程间坐标碰撞算法，能够百分之百精准地区分“空白网格”与“快捷方式图标”。双击真正的文件图标时触发系统原生“打开”动作，只有双击真正的壁纸空白处才会瞬间收起所有桌面图标。
- **流畅原生无缝过渡**：本工具不同于传统的“强制消隐窗口”手法，而是通过直接发送底层 `WM_COMMAND (0x7402)` 消息，让系统主动触发原生“显示桌面图标”切换，确保绝对不打乱你的桌面布局，更不会卡死 Explorer。
- **高级硬件加速亚克力效果**：采用微软原生 DirectComposition 级 `SetWindowCompositionAttribute` API 属性渲染，为您的任务栏赋予磨砂玻璃 (Blur) 及极其高级的霜降亚克力 (Acrylic) 光泽。
- **完美多屏与抗强制重置**：完美适配多个显示器。针对 Win11 切换虚拟桌面或打开控制中心强制还原任务栏的顽疾，内置了防闪烁自愈重绘引擎，秒级自动校准恢复。

---

## 🚀 两种安装/运行模式

为了同时满足普通用户和极致性能控极客，ZenDesktop 提供了两种使用方式：

### 模式 A：标准便携应用（带任务栏美化及系统托盘菜单）

该模式以超轻量的后台守护进程运行，提供丰富的任务栏视觉风格调整和开机自启托盘配置。

1. 前往 [Releases](https://github.com/Liset999/ZenDesktop/releases) 下载 **`ZenDesktop_Portable.zip`**。
2. **将整个压缩包完整解压**到你喜欢的目录中。
   > [!IMPORTANT]
   > **切勿**直接在压缩包内运行，也**不要**单独将可执行文件拖出来运行。程序依赖于同目录下的 `ext/` 文件夹来协调任务栏美化服务，以完美规避 Explorer 内存中的 DLL 版本冲突。
3. 双击运行解压得到的 **`DesktopTaskbarHelper.exe`**。
4. 在右下角系统托盘（电脑屏幕图标）中右键可进行菜单配置：
   - **Taskbar Style**：在 `Default(默认)`、`Transparent(全透明)`、`Blur(毛玻璃)`、`Acrylic(亚克力)` 四种视觉材质中一键切换。
   - **Double Click Hide**：开启或关闭双击空白处隐藏图标功能。
   - **Start on Boot**：勾选后，程序将安全写入当前用户的注册表启动项，开机静默运行。

### 模式 B：纯 C++ Windhawk 模组版（极客与性能控首选）

如果你正在使用 Windows 强大的模组管理器 [Windhawk](https://windhawk.net/)，我们提供了一个纯 C++ 编写的底层注入模组！

- **零后台进程**：直接编译注入至 `explorer.exe` 进程空间运行。
- **极致性能**：0% CPU 占用，**< 1MB 内存消耗**。没有系统托盘，没有多余后台。

**安装步骤：**
1. 启动电脑上的 **Windhawk**。
2. 点击 **Developer Writing（开发者编写）** 标签页（或点击 **Create Mod / 新建模组**）。
3. 复制本仓库中 [ZenDesktop_Windhawk_Mod.wh.cpp](ZenDesktop_Windhawk_Mod.wh.cpp) 文件的全部代码，粘贴到 Windhawk 编辑器中。
4. 点击 **Compile（编译）** 并 **Save（保存）**。Windhawk 会立即自动完成编译并热加载生效。双击桌面空白处即可体验！

---

## 🛡️ License

This project is open-sourced under the [MIT License](LICENSE). Feel free to customize, submit PRs, and build upon this core!
