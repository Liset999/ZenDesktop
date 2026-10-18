# ZenDesktop Premium 🪟✨

[![GitHub License](https://img.shields.io/github/license/Liset999/ZenDesktop?color=blue&style=flat-square)](LICENSE)
[![Platform Windows](https://img.shields.io/badge/Platform-Windows%2011-0078d4?style=flat-square&logo=windows)](https://microsoft.com/windows)
[![Engine Windhawk](https://img.shields.io/badge/Engine-Windhawk%20C%2B%2B-ff69b4?style=flat-square)](https://windhawk.net)

**ZenDesktop Premium v4.2** is a desktop styling suite for Windows 11 built as native Win32/C++ Windhawk mods. File Explorer transparency uses **`zen-explorer-styler`** (no ExplorerBlurMica). Real-time options live in the Windhawk UI.

👨‍💻 **Developer Links & Support / 关注与支持**:
- 📺 **Bilibili**: https://space.bilibili.com/1280632999
- 🎵 **Douyin**: https://v.douyin.com/Isl7QPpKZZ4/
- ⚡ **Afdian**: https://ifdian.net/a/Lanboo

[简体中文](#-简体中文) | [English](#-english-features)

---

## 🚀 Latest Updates / 最新更新 (v4.2)

### 🌐 English
* **Standardized Chinese Localization & Mod Naming**: Unified all core mod display names and descriptions with full bilingual settings translation in Windhawk.
* **Modern Direct2D macOS Minimize Animation**: Integrated the latest Direct2D mesh warp genie engine by Abdullah Masood & Potassiumuncher, supporting customizable mesh resolution (8-96 tiles), taskbar auto-hide unhiding, and process exclusions.
* **Windows Settings Light-Mode Transparency Fix**: Enhanced Settings Styler with full recursive border styling, resolving opaque white cards in light mode for a seamless frosted glass look.
* **Refined One-Key Deploy Script (`deploy.ps1`)**: Auto-detects and deploys all core and optional mods, automatically cleans up legacy deprecated helper mods, and resets the UI cache for instant Chinese mod card display.

### 🇨🇳 中文
* **全系列模组命名与中文本地化规范**：模组统一采用规范的中文命名与中文简介（保留 `ZenDesktop: ` 前缀），所有配置项均已完成中英双语对照翻译。
* **全新 Direct2D macOS 神奇效果动画引擎**：深度整合 Potassiumuncher & Abdullah Masood 的 Direct2D 网格变形引擎，支持网格细分密度调节、自动隐藏任务栏唤出、排除程序列表及多显示器适配。
* **设置美化亮色模式全面透明优化**：彻底修复亮色模式下设置卡片泛白不透明的问题，统一 Glass 模式全景磨砂质感。
* **一键部署引擎 (`deploy.ps1`) 升级**：自动扫描并注册全部核心与可选 Mod，自动清理历史废弃插件并重置 UI 缓存，确保在 Windhawk 中即刻展现全中文模组列表。

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

### 4a. 🎵 任务栏音乐与歌词 (`local@zen-taskbar-lyrics` / `ZenDesktop: 任务栏音乐与歌词`)

小组件关掉以后，在任务栏左下角放音乐面板（封面、歌词、上一首／播放暂停／下一首），可选显示本机 AI 编程助手状态。接 AI 状态见 `tools/ai-status/README.md`。

### 4. 🖱️ Desktop Icon Toggle (`local@zen-desktop-toggle-icons` / `ZenDesktop: 桌面双击隐藏图标`)
* Double-click empty desktop space to instantly hide/show icons with smooth fade animations. State is saved and preserved across system restarts.

### 5. ⚙️ Settings Acrylic Styler (`local@windows-11-settings-styler-fork` / `ZenDesktop: 设置美化`)
* Full acrylic/frosted glass customization for the Windows 11 Settings app (Win+I), deeply adapted for both Light and Dark modes.

### 6. 🎨 macOS Genie Minimize Animation (`local@macos-minimize-animation` / `macOS 神奇效果最小化动画`)

> v4.2 起已并入「窗口动画」(`local@windows-animations`) 的「macOS 神奇效果」最小化样式，deploy 不再安装本 mod；旧文件保留在 `legacy/`。
* Fluid macOS-style Genie minimize and restore animations powered by a high-performance Direct2D mesh engine.

### 7. 📁 File Explorer Styler (`local@zen-explorer-styler` / `ZenDesktop: 文件管理器美化`)
* Windhawk mod for translucent File Explorer (command bar, address bar, tabs) with adjustable tint and text color, plus optional WindhawkBlur glass on the Win11 desktop/Explorer context menu.
* **Do not install ExplorerBlurMica alongside it.** If you registered EBM before, run `ExplorerBlurMica\uninstall.cmd` as Administrator, then enable this mod.

---

## 📦 下载哪个压缩包？/ Which zip should I download?

| 文件 | 适合谁 |
|---|---|
| `ZenDesktop_OneKeyDeploy_v4.2.0.zip` | **大多数人选这个**。内含 Windhawk 安装器（联网安装）+ 全部 Mod |
| `ZenDesktop_OneKeyDeploy_v4.2.0_Offline.zip` | 网络差 / 装 Windhawk 总失败的人。内含离线版 Windhawk 安装器，体积更大 |
| `ZenDesktop_OneKeyDeploy_v4.2.0_Online.zip` | **已经装好 Windhawk** 的人。只含 Mod 和部署脚本，最小 |
| （无需单独的 ExplorerBlurMica 包） | 文件管理器透明请用包内的 `local@zen-explorer-styler`。若以前注册过 EBM，请先卸载再启用该 mod。 |

> 三个 OneKeyDeploy 包只选一个。

## 🧩 可选 Mod（v4.2 新增）/ Optional mods

`deploy.bat` 会把下面两个可选 mod 也装进 Windhawk，和其他 mod 一样默认关闭，想用就在 Windhawk 里自己打开：

- **果冻窗口（Wobbly Windows）**：KDE/Compiz 风格果冻窗口。作者 lalimatyus，GPL-3.0。它要注入 `dwm.exe`，deploy 已经把 `dwm.exe` 加进 Windhawk 的「进程包含列表」，打开后需要重启整台电脑。
- **窗口动画（Windows Animations）**：关窗与最小化/还原/启动动画（含 macOS 神奇效果与 v1.5 新样式）。作者 ReDrag 等，MIT。如果以前装过独立的「macOS 神奇效果最小化动画」，请关掉它，避免播两次。

无人值守安装时可以直接打开指定的 mod：`deploy.bat -Optional "wobbly-windows,windows-animations"`；只会打开列出的 mod，其他 mod 的开关保持原样。署名与许可见 `optional/THIRD_PARTY_NOTICES.md`。

## 🖼️ 效果预览 / Previews (#10)

> TODO：截图/动图待补，放在 `docs/previews/` 下。

| Mod | 预览 |
|---|---|
| 任务栏美化 | `docs/previews/taskbar.png`（待补） |
| 通知栏美化 | `docs/previews/notification-center.png`（待补） |
| 开始菜单美化 | `docs/previews/start-menu.png`（待补） |
| 桌面双击隐藏图标 | `docs/previews/toggle-icons.gif`（待补） |
| 设置美化 | `docs/previews/settings.png`（待补） |
| macOS 神奇效果最小化 | `docs/previews/genie-minimize.gif`（待补） |
| 文件管理器美化 | `docs/previews/explorer.png`（待补） |
| Wobbly Windows（可选） | `docs/previews/wobbly.gif`（待补） |
| Windows Animations（可选） | `docs/previews/close-animation.gif`（待补） |

## 📥 Installation & Deployment Guide

> [!IMPORTANT]
> The suite utilizes **pure local compilation** (`local@` prefix), which means your system compiles the C++ code natively. It is 100% offline-ready, safe, and completely bypasses official Windhawk mod server connection failures!

### Step 1: Run One-Key Deploy
1. Right-click **`deploy.bat`** and select **Run as Administrator** (以管理员身份运行).
2. The script will automatically configure Windhawk, register the 7 core mods and 2 optional mods (all disabled by default), initialize default settings, and restart the Windhawk service.

### Step 2: Native Compilation in Windhawk
1. Open the **Windhawk** UI dashboard.
2. Click into each mod and click **Save / Compile** (保存并编译). The engine will compile the native C++ code in ~5-10 seconds.

### Step 3: Configure via Windhawk UI
1. In the **Windhawk** UI, open each mod's **Settings** page to customize visual options in real-time.
2. Enable **ZenDesktop: 文件管理器美化** (`zen-explorer-styler`) in Windhawk. If ExplorerBlurMica was registered before, uninstall it first.

---

## 🇨🇳 简体中文

### 需要什么

- Windows 11。
- [Windhawk](https://windhawk.net)（建议用最新版）。没装的话，完整包里的 `deploy.bat` 会先打开包内的 Windhawk 安装程序；只含 Mod 的 `_Online` 包需要你先自己装好。
- 管理员权限（deploy 要写 Windhawk 的系统设置）。

### 快速开始

1. 把压缩包**完整解压**到任意文件夹（路径里有空格或中文都可以），不要在压缩软件里直接运行。
2. 双击 **`deploy.bat`**，在弹出的权限提示里点「是」。窗口最后显示「一键部署完成」后按任意键关闭。
3. 打开 Windhawk，在「已安装的 Mod」里能看到以 `ZenDesktop:` 开头的几个 mod，**默认都是关闭的**。
4. 想用哪个就点进去打开开关。Windhawk 提示需要编译时，按提示编译（或进入源码页点右上角「保存并编译」），等几秒即可。
5. 在每个 mod 的「设置」页调整效果，保存后立即生效。全部调好后建议重启一次电脑。

再次运行 `deploy.bat`（比如更新到新版本）是安全的：已打开的 mod 保持打开，已改过的设置不会被覆盖，只会补上新增设置的默认值。

### 包含的 Mod

| Mod | 作用 |
|---|---|
| 任务栏美化 | 任务栏透明/亚克力/液态玻璃，托盘溢出窗口和音量浮窗一起美化 |
| 开始菜单美化 | 开始菜单与搜索页的透明玻璃效果，可调尺寸 |
| 通知栏美化 | 通知中心、日历、快捷设置（Win+A）的玻璃效果，适配浅色模式 |
| 设置美化 | 「设置」应用（Win+I）的玻璃背景，浅色模式可加白底 |
| 文件管理器美化 | 文件管理器和右键菜单透明，可调色调与文字颜色 |
| 任务栏音乐与歌词 | 任务栏左下角显示正在播放的歌曲、歌词和播放按钮，可选显示 AI 编程助手状态（见 `tools/ai-status/README.md`） |
| 桌面双击隐藏图标 | 双击桌面空白处隐藏/显示图标 |
| 果冻窗口（可选） | 拖动窗口时像果冻一样晃动 |
| 窗口动画（可选） | 关窗、最小化、还原、启动动画 |

文件管理器美化不要和 ExplorerBlurMica 同时使用。若以前注册过 ExplorerBlurMica，请先管理员运行 `ExplorerBlurMica\uninstall.cmd` 再启用。

### 卸载与恢复

1. 在 Windhawk「已安装的 Mod」里，把 ZenDesktop 的 mod 关掉即可恢复系统原样；不想保留就点「移除」。
2. 运行 `restart_explorer.bat`（或重启电脑），让任务栏、开始菜单恢复系统默认。
3. 如果启用过开机自检：管理员运行 `deploy.ps1 -StartupCheck remove`。
4. 如果接过 AI 状态：删除 `%LOCALAPPDATA%\ZenDesktop` 文件夹，并把当时粘贴到各 AI 助手配置里的片段删掉。
5. deploy 往 Windhawk「设置 → 高级设置」的进程包含列表加了 `dwm.exe`，排除列表加了一些游戏和反作弊进程。这些不影响系统，想还原可以在那里手动删掉。

---

## 🩺 常见问题 / Troubleshooting

- **文件管理器不透明 / 命令栏发黑**：请只用 `zen-explorer-styler`，不要同时开 ExplorerBlurMica。若仍注册着 EBM，先 `uninstall.cmd` 再重启。浅色模式地址栏不透时，在 styler 设置里检查色调预设与「字段不透明度」。

## 📄 License
This project is licensed under the GPL-3.0 License. See the [LICENSE](LICENSE) file for details.

Developed with ❤️ by **Lanbo**.
Special thanks to:
* **m417z** for Windhawk and the upstream File Explorer / Taskbar Styler mods.
* **Maplespe** for historical ExplorerBlurMica work (no longer bundled as the recommended path).
* **Abdullah Masood & Potassiumuncher** for macOS Minimize Animation.
* **lalimatyus** for Wobbly Windows (GPL-3.0, optional).
* **ReDrag** for Windows Animations (MIT, optional).

## 捐助支持

如果 ZenDesktop 对你有帮助，可以通过爱发电或微信赞赏支持作者 Lanbo。

- [爱发电](https://ifdian.net/a/Lanboo)

![微信赞赏码](docs/donate-wechat.png)

图片无法显示时，可打开：https://raw.githubusercontent.com/Liset999/ZenDesktop/v4.2-draft/docs/donate-wechat.png
