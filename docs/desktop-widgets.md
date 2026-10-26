# ZenDesktop 桌面小组件（架构）

> 状态：**v0.2.1 macOS 风格布局 + 内置模板 + 本地编排 MVP**。不是 Rainmeter 克隆；可选 Mod，部署默认关闭。

## 目标

在桌面上放置可拖动的玻璃风小组件，外观贴近 ZenDesktop 其它玻璃面板，并参考 **macOS Desktop / Notification Center Widgets** 的视觉节奏，且：

- **默认关闭**：`optional/`，`deploy` 只装源码，不自动开启。
- **可扩展**：宿主管窗口、拖拽、位置持久化与设置；卡片是独立绘制单元。
- **轻量**：注入 `explorer.exe`；刷新仅在文案变化时重绘；天气为演示数据（不在 explorer 内拉网）。

## 目录与入口

| 路径 | 作用 |
| --- | --- |
| `optional/local@zen-desktop-widgets.wh.cpp` | 宿主 + 内置模板 + 编排 + macOS 风格布局 |
| `docs/desktop-widgets.md` | 本文 |
| `docs/nexus-dock.md` | Nexus 式 Dock（独立 optional Mod） |

## macOS-inspired layout（v0.2.1）

视觉与交互参考 macOS 桌面/通知中心小组件，**不是**克隆私有资源或 API：

| 项 | 行为 |
| --- | --- |
| **尺寸档** `widgetSize` | `small` ≈ 168²；`medium` ≈ 340×168（日历/倒计时略方）；`large` ≈ 340²。设置里中文「小 / 中 / 大」。 |
| **层级** | 顶部 **caption**（小、半粗、次要色）→ 中部 **value**（大、Semibold）→ 可选 **footer**。状态/便签为 caption + 折行正文。 |
| **内边距** | 随尺寸档 16 / 20 / 24，各卡一致。 |
| **字体** | `Segoe UI Variable Display`（数值）/ `Text`（标题与正文）；失败回退 `Segoe UI`（Windows 上接近 SF 的系统替代）。 |
| **圆角** | Win11 `DWMWA_WINDOW_CORNER_PREFERENCE = ROUND`；旧系统 `CreateRoundRectRgn`（半径随档约 22–32）。 |
| **玻璃** | Acrylic tint；读 `AppsUseLightTheme` 切换深/浅字色与底色。 |
| **堆叠默认位** | 右缘一列，卡间 `14px`；坐标可 `snapToGrid`（16px）在拖放松手时对齐。 |
| **字号** | `fontSize=0` 随档自动（约 28/36/52）；非 0 则覆盖主数值字号。 |

保留：拖动、`HKCU\Software\ZenDesktop\DesktopWidgets\`、中文设置、`presetPack` / `aiDescribe`、deploy 默认关、天气不在 explorer 内 HTTP。

## 内置模板（v0.2）

| 卡片 | 设置开关 | 内容 | 刷新 |
| --- | --- | --- | --- |
| 时钟 | `showClock` | caption「时钟」+ 大号时间 + 日期脚注；`clockShowSeconds` / `clock12Hour` | 1s 或 15s |
| 状态 | `showStatus` | caption + `statusText`（空则内置提示） | 设置变更 |
| 天气（演示） | `showWeather` | 城市 caption + 演示文案 value +「演示数据」 | 静态 |
| 日历 | `showCalendar` | 年月 caption + 日号 value + 星期 footer | 60s |
| 系统一览 | `showSystem` | 「电量」+ 百分比 + 充电/续航脚注（`GetSystemPowerStatus`） | 30s |
| 倒计时 | `showCountdown` | `countdownLabel` + 天数 value +「天剩余」等 | 60s |
| 便签 | `showSticky` | caption + `stickyText` 折行 | 设置变更 |

玻璃：`SetWindowCompositionAttribute` Acrylic；关透明时半透明实色底。拖动写入 HKCU（每卡独立 `*X`/`*Y`）。

## 卡片编排（灵感：小米「超级背屏」AI 应用卡）

小米「百变背屏 / AI 生成应用卡」的核心体验是：预置卡 + 一句话描述生成/选卡。ZenDesktop **不复制产品、不调用小米服务**，只借鉴交互，在 Windows 桌面做轻量 MVP：

| 设置 | 作用 |
| --- | --- |
| `presetPack` | `manual` / `daily` / `focus` / `system` / `all` |
| `aiDescribe` | 中英文关键词 → 打开匹配模板 |
| `aiLayoutMode` | `append` / `replace` |

**本进程内不做 LLM、不联网推理。** 真生成式布局（本地 Ollama 等）见路线图。

### 关键词示例

- 「时钟和天气」→ 时钟 + 天气演示  
- 「倒计时考试」→ 倒计时  
- 「电量便签」→ 系统一览 + 便签  
- 「calendar sticky」→ 日历 + 便签  

## 架构

```
explorer.exe
  └─ ZenDesktop Widgets Mod
        ├─ Settings：presetPack / aiDescribe / widgetSize / snapToGrid / 各 show*
        ├─ ApplyPresetAndAi()  → 解析套装 + 关键词
        ├─ Theme + Fonts       → 浅/深色、Variable Display/Text
        ├─ WidgetHost          → 圆角 / 玻璃 / 拖拽 / 网格吸附 / HKCU / 定时器
        └─ Templates           → Clock / Status / Weather / Calendar /
                                 System / Countdown / Sticky
                                 （caption / value / footer 绘制）
```

## 如何试用

1. `deploy.bat`（或已部署则重跑补文件）。
2. Windhawk → **ZenDesktop: 桌面小组件** → 打开并编译。
3. 选「卡片尺寸」小/中/大；用预设套装或「描述想要的卡片」；或手动开各卡。
4. 拖到合适位置（可开网格吸附）；改设置立即重建卡片。

## 已知限制

- 天气为**演示文案**，非 Open-Meteo / 系统天气；避免在 explorer 注入路径做脆弱 HTTP。
- 系统一览目前只做电量（CPU% 轮询成本更高，未做）。
- 描述编排是关键词规则，不是语义理解；无匹配时保持原开关并打日志。
- Acrylic 依赖系统「透明度效果」；圆角在旧系统为区域裁剪，可能与 Acrylic 边缘略有差异。
- 浅/深色在设置变更时重读；系统主题热切换需再改一次 Mod 设置或重载才会刷新。
- 动态壁纸下 z-order 可能需后续 WorkerW 挂接。
- 尺寸档为固定 footprint，长文案可能裁切（可调大档或改文案）。

## 演进路线

1. **v0.2**：多内置模板 + 预设 + 描述→模板 MVP。  
2. **v0.2.1（本版）**：macOS 风格布局（尺寸档、层级、圆角、浅深色、吸附）。  
3. **v0.3**：可选本地 HTTP（用户自建）拉天气；每卡独立尺寸/主题色。  
4. **v1**：卡片接口；可选本地 LLM API 解析 `aiDescribe`。  
5. **可选独立 Host exe**；**Dock** 见 [nexus-dock.md](nexus-dock.md)。

## 非目标（本阶段不做）

- Rainmeter / Lua / 网页任意皮肤市场。  
- 替代 Windows 11 自带小组件板。  
- 进程内嵌云端大模型或商业 AI SDK。  
- 逐像素复刻 Apple 私有资源。
