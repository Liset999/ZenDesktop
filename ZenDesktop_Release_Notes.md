# 🚀 ZenDesktop v1.1.0 里程碑发布 (Milestone Release)

欢迎来到 **ZenDesktop v1.1.0**！这是一个非常重要的里程碑版本。在这个版本中，我们解决了在高分辨率屏幕、多实例并发运行、以及跨进程内存读取等方面的数个核心 Bug，大幅提升了系统的运行稳定性与使用体验。

---

## 🌟 版本亮点 (Highlights)

### 1. 🛠️ 彻底杜绝多开（单例锁机制）
* **Named Mutex 互斥锁**：引入 Windows 命名互斥量机制 `Global\\ZenDesktop_SingleInstance_Mutex`。
* **优雅防重复**：如果检测到已有后端实例在运行，新启动的程序会自动退出并给出温馨弹窗提醒。这彻底解决了重复启动导致鼠标双击钩子（Hook）被多次注册、造成双击行为混乱的严重问题。

### 2. 🖥️ 完美适配高分屏（DPI 感知）
* **Per-Monitor DPI Awareness**：加入了最高级别的 Windows 进程 DPI 感知配置。
* **高精度坐标定位**：彻底解决了在 2K/4K/150% DPI 缩放屏幕上，鼠标点击桌面左侧图标时会被误判定为点击“桌面空白处”而导致误隐藏的 Bug。

### 3. 🧠 64位内存结构对齐修正
* **结构体字段对齐**：修正了 `desktop_handler.py` 中 `LVHITTESTINFO` 的 C 语言结构体定义，补全了缺失的 4 字节 `iGroup` 字段。
* **完美兼容 Win11**：确保在 64 位 Windows 系统下通过 `ReadProcessMemory` 跨进程读取 Explorer 内存时的绝对准确，彻底杜绝了概率性双击失效或坐标错乱。

### 4. 📦 纯绿色便携化分发 (ZenDesktop_Portable.zip)
* **避开 Explorer 锁死限制**：不再强行把 TranslucentTB 的动态链接库（DLL）打包进单个 EXE。
* **兄弟文件夹联动**：将 TranslucentTB 放置在独立的 `ext/` 文件夹下，与 `DesktopTaskbarHelper.exe` 协同工作，彻底解决因 PyInstaller 临时文件夹路径变动而导致的“版本不匹配，请重启电脑”报错。

---

## 💾 快速开始 (Quick Start)

1. 下载下方的 **`ZenDesktop_Portable.zip`** 压缩包。
2. 解压到一个你喜欢的地方（如 `D:\ZenDesktop`）。
3. 双击运行 **`DesktopTaskbarHelper.exe`**，程序将自动在后台静默运行并注册开机自启。
4. **如何使用**：
   * **双击桌面空白处**：瞬间隐藏所有桌面图标，让壁纸呼吸；再次双击，图标优雅回归。
   * **双击快捷方式**：正常打开程序，绝不误触！
   * **完美透明任务栏**：与 TranslucentTB 联动，开机即享丝滑的透明任务栏特效。

---

感谢大家的支持！如果你在使用过程中遇到任何问题，欢迎在 GitHub 上提交 Issue 或贡献 Pull Request。🎉
