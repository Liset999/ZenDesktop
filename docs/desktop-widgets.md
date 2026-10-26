# ZenDesktop 桌面小组件（架构）

> 状态：**v0.2 内置模板 + 本地编排 MVP**。不是 Rainmeter 克隆；可选 Mod，部署默认关闭。

## 目标

在桌面上放置可拖动的玻璃风小组件，外观贴近 ZenDesktop 其它玻璃面板，且：

- **默认关闭**：`optional/`，`deploy` 只装源码，不自动开启。
- **可扩展**：宿主管窗口、拖拽、位置持久化与设置；卡片是独立绘制单元。
- **轻量**：注入 `explorer.exe`；刷新仅在文案变化时重绘；天气为演示数据（不在 explorer 内拉网）。

## 目录与入口

| 路径 | 作用 |
| --- | --- |
| `optional/local@zen-desktop-widgets.wh.cpp` | 宿主 + 内置模板 + 编排 |
| `docs/desktop-widgets.md` | 本文 |
| `docs/nexus-dock.md` | Nexus 式 Dock（独立 optional Mod） |

## 内置模板（v0.2）

| 卡片 | 设置开关 | 内容 | 刷新 |
| --- | --- | --- | --- |
| 时钟 | `showClock` | 时间 + 日期星期；`clockShowSeconds` / `clock12Hour` | 1s 或 15s |
| 状态 | `showStatus` | `statusText`（空则内置提示） | 设置变更 |
| 天气（演示） | `showWeather` | `weatherCity` + `weatherDemoText`，副行标注「演示」 | 静态 |
| 日历 | `showCalendar` | 日号 +「年月 · 周X」 | 60s |
| 系统一览 | `showSystem` | `GetSystemPowerStatus` 电量 / 交流供电 | 30s |
| 倒计时 | `showCountdown` | `countdownLabel` + `countdownDate`（YYYY-MM-DD）剩余天 | 60s |
| 便签 | `showSticky` | `stickyText` | 设置变更 |

玻璃：`SetWindowCompositionAttribute` Acrylic；关透明时半透明深色底。拖动写入 `HKCU\Software\ZenDesktop\DesktopWidgets\`（每卡独立 `*X`/`*Y`）。

## 卡片编排（灵感：小米「超级背屏」AI 应用卡）

小米 18 Pro「百变背屏 / AI 生成应用卡」的核心体验是：

1. **精选预置卡**（喝水打卡、倒计时、计时等上百种）可直接添加；
2. **一句话 / 图片描述** → 云端（或端侧）大模型生成专属应用卡；
3. 在「妙享背屏」里管理、排序、移除。

ZenDesktop **不复制产品、不调用小米服务**，只借鉴「预置模板 + 用自然语言选卡」的交互，在 Windows 桌面做轻量 MVP：

| 设置 | 作用 |
| --- | --- |
| `presetPack` | 预设套装：`manual` / `daily`（时钟+天气+日历）/ `focus`（时钟+倒计时+便签）/ `system`（时钟+电量+状态）/ `all` |
| `aiDescribe` | 「描述想要的卡片」：中英文关键词 → 打开匹配模板（时钟/天气/日历/电量/倒计时/便签/状态…） |
| `aiLayoutMode` | `append` 追加匹配项；`replace` 只开匹配项 |

**本进程内不做 LLM、不联网推理。** 规则引擎足够覆盖「我想要时钟和天气」「假期倒计时」这类句子。真生成式布局（本地 Ollama / 其它 API 返回 JSON 布局再实例化）留到后续，见下方路线图。

### 关键词示例

- 「时钟和天气」→ 时钟 + 天气演示  
- 「倒计时考试」→ 倒计时（可再改目标日期）  
- 「电量便签」→ 系统一览 + 便签  
- 「calendar sticky」→ 日历 + 便签  

## 架构

```
explorer.exe
  └─ ZenDesktop Widgets Mod
        ├─ Settings：presetPack / aiDescribe / 各 show* / 字段
        ├─ ApplyPresetAndAi()  → 解析套装 + 关键词
        ├─ WidgetHost          → 窗口 / 拖拽 / HKCU / 定时器
        └─ Templates           → Clock / Status / Weather / Calendar /
                                 System / Countdown / Sticky
```

## 如何试用

1. `deploy.bat`（或已部署则重跑补文件）。
2. Windhawk → **ZenDesktop: 桌面小组件** → 打开并编译。
3. 用「卡片预设套装」或「描述想要的卡片」选布局；或手动开各卡。
4. 拖到合适位置；改设置立即重建卡片。

## 已知限制

- 天气为**演示文案**，非 Open-Meteo / 系统天气；避免在 explorer 注入路径做脆弱 HTTP。
- 系统一览目前只做电量（CPU% 轮询成本更高，未做）。
- 描述编排是关键词规则，不是语义理解；无匹配时保持原开关并打日志。
- Acrylic 依赖系统「透明度效果」。
- 动态壁纸下 z-order 可能需后续 WorkerW 挂接。

## 演进路线

1. **v0.2（本版）**：多内置模板 + 预设 + 描述→模板 MVP。  
2. **v0.3**：可选本地 HTTP（用户自建）拉天气；卡片尺寸/主题色设置。  
3. **v1**：卡片接口（刷新间隔、设置片段）；可选「本地 LLM API」：把 `aiDescribe` POST 到 `http://127.0.0.1:...`，解析返回的 template id 列表。  
4. **可选独立 Host exe**：若注入不稳定再拆托盘宿主。  
5. **Dock**：见 [nexus-dock.md](nexus-dock.md)。

## 非目标（本阶段不做）

- Rainmeter / Lua / 网页任意皮肤市场。  
- 替代 Windows 11 自带小组件板。  
- 进程内嵌云端大模型或商业 AI SDK。
