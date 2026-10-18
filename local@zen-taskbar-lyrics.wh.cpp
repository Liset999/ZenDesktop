// ==WindhawkMod==
// @id              zen-taskbar-lyrics
// @name            ZenDesktop: 任务栏音乐与歌词
// @name:zh-CN      ZenDesktop: 任务栏音乐与歌词
// @description     Compact media panel in the taskbar Widgets slot: album art, synced lyrics, playback buttons, and AI agent status.
// @description:zh-CN 在任务栏左下角（小组件的位置）放一个音乐小面板：专辑封面、同步歌词、上一首／播放暂停／下一首，还能显示本机 AI 编程助手的状态。
// @version         0.4.0
// @author          Lanbo
// @github          https://github.com/Liset999
// @donateUrl       https://ifdian.net/a/Lanboo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinhttp -lruntimeobject -lgdiplus -lole32 -loleaut32 -lshell32 -lshlwapi -luser32 -lgdi32 -ladvapi32 -ldwmapi -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/v4.2-draft/docs/donate-wechat.png)

# ZenDesktop: 任务栏音乐与歌词

## 简介
把 Windows 11 的小组件关掉以后，任务栏左下角会空出一块。这个 mod 在那里放一个音乐小面板，同时可以显示本机 AI 编程助手（Claude Code、Codex、opencode 等）正在做什么。

开发者：Lanbo（ZenDesktop）。哔哩哔哩：https://space.bilibili.com/1280632999 ，抖音：https://v.douyin.com/Isl7QPpKZZ4/ ，爱发电：https://ifdian.net/a/Lanboo

## 功能
- 专辑封面：来自系统媒体会话，播放器提供才有。
- 歌词：显示当前这一句（网易云、QQ 音乐、LRCLIB），第二行可显示翻译或「歌名 - 歌手」。查不到歌词时显示歌名和歌手。
- 播放控制：上一首、播放／暂停、下一首，鼠标移上去会高亮；播放器不支持的按钮会变灰。
- 点封面或文字可以把播放器切到前台（可在设置里关掉）。
- 没在放歌时默认显示「未在播放」，也可改成隐藏。全屏游戏、视频、PPT 时默认不隐藏，可在设置里开启。
- 文字颜色默认跟随系统深浅色（浅色模式用深色字），可选的颜色和其他 ZenDesktop mod 一样。
- 位置：任务栏图标居中时放在最左边；图标靠左时自动挪到托盘左边（和系统小组件按钮的规则一样），不会压住开始按钮。
- 常驻：面板直接嵌在任务栏里，跟着任务栏移动、自动隐藏，打开新程序时不会被任务栏盖住而闪烁。
- AI 助手状态：读取 `%LOCALAPPDATA%\ZenDesktop\ai-status\` 里每个助手一个的 JSON 文件。放歌时显示在第二行，没放歌时单独显示。多个助手可以排成一行，也可以轮流显示。

## AI 助手状态怎么接
ZenDesktop 仓库的 `tools/ai-status/` 里有现成的脚本，详细步骤看那里的 README。先在 PowerShell 里运行一次：

```
powershell -NoProfile -ExecutionPolicy Bypass -File <ZenDesktop 文件夹>\tools\ai-status\install.ps1
```

它会把脚本复制到 `%LOCALAPPDATA%\ZenDesktop\ai-status-tools\`，并打印出填好路径的配置片段。它不会改任何助手的配置文件，下面的片段要你自己粘贴。以下示例里的 `C:\Users\你的用户名` 换成你自己的路径。

### 1. MCP 服务（让助手自己汇报「在做什么」）
提供一个工具 `set_status`，助手调用它写入状态和一句说明。

Claude Code（命令行运行一次）：

```
claude mcp add --scope user zen-status -- powershell.exe -NoProfile -ExecutionPolicy Bypass -File C:\Users\你的用户名\AppData\Local\ZenDesktop\ai-status-tools\zen-status-mcp.ps1 -Agent claude-code
```

Codex（`~/.codex/config.toml`）：

```
[mcp_servers.zen-status]
command = "powershell.exe"
args = ["-NoProfile", "-ExecutionPolicy", "Bypass", "-File", 'C:\Users\你的用户名\AppData\Local\ZenDesktop\ai-status-tools\zen-status-mcp.ps1', "-Agent", "codex"]
```

opencode（`~/.config/opencode/opencode.json` 里的 `mcp`）：

```
"mcp": {
  "zen-status": {
    "type": "local",
    "command": ["powershell.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", "C:\\Users\\你的用户名\\AppData\\Local\\ZenDesktop\\ai-status-tools\\zen-status-mcp.ps1", "-Agent", "opencode"],
    "enabled": true
  }
}
```

### 2. 给助手的说明（粘贴到 AGENTS.md 或 CLAUDE.md）
Codex 和 opencode 读 `AGENTS.md`，Claude Code 读 `CLAUDE.md`（全局的放在 `~/.claude/CLAUDE.md`）。

```
## 任务栏状态汇报（zen-status）
本机任务栏会显示你的工作状态。请用 zen-status 这个 MCP 服务的 set_status 工具汇报：
- 开始处理一个新任务时：state 设为 "working"，message 用一句不超过 20 个字的中文说明在做什么，例如「重写任务栏 mod 的最大化层」。
- 需要我回答问题、确认或批准时：state 设为 "waiting"，message 写清楚在等什么。
- 任务完成时：state 设为 "done"，message 用一句话总结结果。
- 遇到无法继续的错误时：state 设为 "error"，message 写原因。
只在状态变化时调用，不要每一步都调用。message 里不要写密码、令牌、手机号等隐私信息。
```

### 3. 钩子（不用助手配合，自动更新状态）
- Claude Code：把 `tools/ai-status/claude-code/settings-snippet.json` 里的 `hooks` 合并进 `~/.claude/settings.json`。
- Codex：把 `tools/ai-status/codex/hooks.json` 放到 `~/.codex/hooks.json`（已有的话合并），然后在 Codex 里用 `/hooks` 信任这些钩子；也可以只用 `config.toml` 里的 `notify`，它只在一轮回答结束时触发。
- opencode：把 `tools/ai-status/opencode/zen-status.js` 复制到 `~/.config/opencode/plugins/`。
- 其他没有钩子的工具：在设置「按进程名显示」里填进程名（例如 `gemini.exe`），进程在运行时显示「运行中」。

## 设置说明
- 位置：小组件位置（推荐）、固定在最左边、固定在托盘左边。
- 水平偏移：按 100% 缩放的像素，高 DPI 自动放大，范围 -500–2000。
- 没在放歌时：默认显示「未在播放」，可选隐藏面板。有 AI 助手在工作时仍会显示。
- 面板宽度：紧凑 260、标准 340、宽 440、超宽 560，或自定义 120–2000。
- 面板玻璃：默认开启。只对「淡淡一层」「卡片」有用：开启时是叠在任务栏玻璃上的一层半透明色，关闭时是几乎不透明的底色。选「无底色」时这个开关没有作用。
- 多显示器：默认关闭（只主屏）。开启后每个任务栏各显示一个面板，媒体与 AI 状态共用。
- 第一行、第二行：歌词、翻译、歌名歌手的组合。
- 文字颜色、面板底色、字体、字号（10–16 磅）、粗体。面板底色默认「无底色」：完全透明，没有底色、边框和阴影，文字、封面和按钮直接画在任务栏上，看起来就是任务栏的一部分，只有鼠标移上去时有很淡的高亮。
- 歌词偏移：毫秒，正数提前，范围 -60000–60000。
- 全屏和游戏时隐藏：默认关闭。开启后全屏游戏、视频、PPT 放映时隐藏（多显示器只藏全屏那块屏）。
- AI 状态：一行列出、轮流显示或关闭；「已完成」保留多久；按进程名显示。

## 兼容性与已知问题
- 支持 Windows 11 任务栏（底部）。默认只在主显示器显示；开启「多显示器」后，每个任务栏（含副屏 `Shell_SecondaryTrayWnd`）各有一个面板。
- 小组件没关时会和小组件按钮重叠，请在「设置 → 个性化 → 任务栏」里关掉小组件。
- 任务栏图标靠左时，托盘的位置靠估算，如果和托盘重叠，调「水平偏移」。
- 面板是任务栏（`Shell_TrayWnd`／`Shell_SecondaryTrayWnd`）的子窗口，排在任务栏自己的 XAML 内容上面。子窗口用不了系统亚克力，所以面板自己不做模糊，透出来的是任务栏本身的材质；「淡淡一层」「卡片」是在上面叠一层半透明色。
- 文字颜色选得和任务栏深浅接近时（比如浅色模式选白色），会自动加一层很淡的阴影保证看得清；默认颜色不加。
- 面板不出来时：在 Windhawk 这个 mod 的「高级设置」里打开调试日志，再看日志输出里 `[ui]`、`[media]` 开头的行，会写明「没找到任务栏」「没有媒体会话」「判定为全屏」等原因；正常时会有一行 `attached as child of Shell_TrayWnd`。没在放歌且「没在放歌时」选了「隐藏面板」时不显示是正常的；默认是显示「未在播放」。
- 点「打开播放器」只能切换到有可见窗口的播放器；缩到托盘里的播放器切不出来。
- 网易云、QQ 音乐的歌词接口是非官方的，随时可能失效。
- 歌词缓存在 `%LOCALAPPDATA%\ZenDesktop\lyrics\`。所有联网和媒体控制都在后台线程，不会卡任务栏。媒体会话与 AI 状态在多显示器间共用一份。

## 更新日志
- **0.4.0**：面板改为嵌在任务栏里的子窗口（在任务栏自己的线程上创建），不再浮在任务栏上面，打开新程序时不再闪烁，跟着任务栏移动和自动隐藏。「无底色」改为完全透明，没有底色、边框和阴影，和任务栏材质一致。「面板玻璃」改为只决定「淡淡一层」「卡片」是半透明还是不透明。
- **0.3.3**：修复打开新程序时面板闪一下（任务栏把面板压到下面、切歌瞬间误判没在放歌、玻璃模式下先涂黑再画字）；「面板底色」在玻璃开启时改为调玻璃浓淡，三档都能看出区别；「没在放歌时」挪到设置第三项。
- **0.3.2**：默认改为没在放歌时显示「未在播放」（不再默认隐藏）；「全屏和游戏时隐藏」改为默认关闭。
- **0.3.1**：修复「开了没反应」：桌面或最大化窗口（Windhawk、网易云、微信等）在前台时被误判成全屏而隐藏；面板改为不挂靠任务栏的置顶窗口（参考 Taskbar Music Lounge），在每个环节写 Windhawk 日志，方便排查。
- **0.3.0**：可选面板玻璃（AccentBlurBehind 亚克力），可选多显示器（每个任务栏一个面板）。

## 致谢与许可
- 歌词源和控制面板的设计参考了 TaskbarLyrics（https://github.com/ANYNC/TaskbarLyrics ，MIT 许可，© 2026 anync），没有复制其代码。
- 歌词数据来自 LRCLIB、网易云音乐、QQ 音乐，版权归原权利人所有。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- positionMode: widgets
  $name: Position
  $name:zh-CN: 位置
  $description: Widgets slot = left edge when icons are centered, left of the tray when icons are left-aligned.
  $description:zh-CN: 「小组件位置」：任务栏图标居中时放在最左边，图标靠左时自动挪到托盘左边，不压开始按钮。
  $options:
  - widgets: 小组件位置（推荐）
  - left: 固定在任务栏最左边
  - auto_tray: 固定在托盘左边
- xOffset: 12
  $name: Horizontal offset (px)
  $name:zh-CN: 水平偏移（像素）
  $description: Distance from the taskbar edge or tray, in 100% scale pixels. Range -500–2000. Default 12.
  $description:zh-CN: 离任务栏左边或托盘的距离，按 100% 缩放的像素算，高 DPI 自动放大。范围 -500–2000。默认 12。
- whenIdle: show
  $name: When nothing is playing
  $name:zh-CN: 没在放歌时
  $description: Default shows "Not playing". AI status still shows when an agent is active.
  $description:zh-CN: 默认为显示「未在播放」。有 AI 助手在工作时仍会显示。
  $options:
  - hide: 隐藏面板
  - show: 显示「未在播放」
- sizePreset: standard
  $name: Panel width
  $name:zh-CN: 面板宽度
  $description: Width in 100% scale pixels.
  $description:zh-CN: 按 100% 缩放的像素算。
  $options:
  - compact: 紧凑（260）
  - standard: 标准（340）
  - wide: 宽（440）
  - extra: 超宽（560）
  - custom: 自定义（用下面的宽度）
- width: 340
  $name: Custom width (px)
  $name:zh-CN: 自定义宽度（像素）
  $description: Only used when the panel width is Custom. Range 120–2000.
  $description:zh-CN: 只有「面板宽度」选「自定义」时才用。范围 120–2000。
- showArt: true
  $name: Show album art
  $name:zh-CN: 显示专辑封面
  $description: Album art comes from the system media session, only if the player provides it.
  $description:zh-CN: 封面来自系统媒体会话，播放器提供才有，没有时显示音符。
- showButtons: true
  $name: Show playback buttons
  $name:zh-CN: 显示播放控制按钮
  $description: Previous, play/pause and next.
  $description:zh-CN: 上一首、播放／暂停、下一首。
- textMode: lyrics
  $name: First line
  $name:zh-CN: 第一行显示
  $description: What the main line shows.
  $description:zh-CN: 主行显示的内容。
  $options:
  - lyrics: 当前歌词（没有就显示歌名）
  - title: 只显示歌名
- secondLine: auto
  $name: Second line
  $name:zh-CN: 第二行显示
  $description: Translation comes from NetEase or QQ Music when available. AI status replaces this line while an agent is active.
  $description:zh-CN: 翻译来自网易云或 QQ 音乐。有 AI 助手在工作时，这一行改为显示 AI 状态。
  $options:
  - auto: 有翻译显示翻译，没有显示歌名和歌手
  - translation: 只显示翻译
  - info: 歌名和歌手
  - none: 不显示，只要一行
- clickAction: open
  $name: Click on art or text
  $name:zh-CN: 点封面或文字
  $description: Bring the playing app to the front, or let clicks pass through.
  $description:zh-CN: 把正在播放的软件切到前台，或者不响应（点击穿透）。
  $options:
  - open: 打开播放器
  - none: 不响应
- textColorMode: default
  $name: Text color
  $name:zh-CN: 文字颜色
  $description: Text and button color. Theme default and System-aware both follow light/dark mode.
  $description:zh-CN: 文字和按钮的颜色。「主题默认」和「跟随系统」都跟着系统深浅色走：浅色模式深色字，深色模式白字。
  $options:
  - default: 主题默认
  - white: 白色
  - dark: 深色
  - system: 跟随系统
  - red: 红色
  - green: 绿色
  - blue: 蓝色
  - yellow: 黄色
  - orange: 橙色
  - purple: 紫色
  - pink: 粉色
  - cyan: 青色
- panelBackground: none
  $name: Panel background
  $name:zh-CN: 面板底色
  $description: None = fully transparent, drawn straight on the taskbar's own material. Subtle and card follow light/dark mode.
  $description:zh-CN: 「无底色」完全透明，没有底色和边框，直接画在任务栏上。「淡淡一层」「卡片」跟随系统深浅色。
  $options:
  - none: 无底色（最通透）
  - subtle: 淡淡一层
  - card: 卡片（最浓）
- fontPreset: msyh
  $name: Font
  $name:zh-CN: 字体
  $description: Font for lyrics and titles.
  $description:zh-CN: 歌词和歌名用的字体。
  $options:
  - msyh: 微软雅黑 UI
  - segoe: Segoe UI
  - dengxian: 等线
  - custom: 自定义（下面填写）
- customFont: ""
  $name: Custom font name
  $name:zh-CN: 自定义字体名
  $description: Only used when the font is Custom, for example "HarmonyOS Sans SC".
  $description:zh-CN: 只有「字体」选「自定义」时才用，例如「HarmonyOS Sans SC」。
- fontSize: 12
  $name: Font size
  $name:zh-CN: 字号
  $description: Size of the first line in points; the second line is a little smaller.
  $description:zh-CN: 第一行的字号（磅），第二行会小一点。
  $options:
  - 10: 10 磅
  - 11: 11 磅
  - 12: 12 磅（推荐）
  - 13: 13 磅
  - 14: 14 磅
  - 16: 16 磅
- bold: false
  $name: Bold
  $name:zh-CN: 粗体
  $description: Bold first line.
  $description:zh-CN: 第一行用粗体。
- offsetMs: 0
  $name: Lyrics offset (ms)
  $name:zh-CN: 歌词偏移（毫秒）
  $description: Positive shows lyrics earlier, negative later. Range -60000–60000.
  $description:zh-CN: 正数让歌词提前显示，负数推后。范围 -60000–60000。
- hideInFullscreen: false
  $name: Hide in fullscreen and games
  $name:zh-CN: 全屏和游戏时隐藏
  $description: Default off. Hides during fullscreen games, videos and presentations. With multi-monitor, only the panel on the fullscreen monitor hides.
  $description:zh-CN: 默认关闭。开启后全屏游戏、视频、PPT 放映时隐藏。开启多显示器时，只隐藏全屏所在那块屏幕上的面板。
- panelGlass: true
  $name: Panel glass
  $name:zh-CN: 面板玻璃
  $description: Only for Subtle and Card: on = a translucent layer over the taskbar glass, off = a nearly solid background. No effect with None. Default on.
  $description:zh-CN: 只对「淡淡一层」「卡片」有用：开启时是叠在任务栏玻璃上的半透明色，关闭时是几乎不透明的底色。「无底色」时不起作用。默认开启。
- multiMonitor: false
  $name: Multi-monitor
  $name:zh-CN: 多显示器
  $description: Off = primary taskbar only. On = one panel per taskbar including secondary monitors. Default off.
  $description:zh-CN: 关闭时只在主任务栏显示；开启后每个任务栏（含副屏）各有一个面板。默认关闭。
- enableNetease: true
  $name: NetEase lyrics
  $name:zh-CN: 使用网易云歌词源
  $description: Unofficial API, may stop working.
  $description:zh-CN: 非官方接口，可能随时失效。
- enableQQ: true
  $name: QQ Music lyrics
  $name:zh-CN: 使用 QQ 音乐歌词源
  $description: Unofficial API, may stop working.
  $description:zh-CN: 非官方接口，可能随时失效。
- enableLrclib: true
  $name: LRCLIB lyrics
  $name:zh-CN: 使用 LRCLIB 歌词源
  $description: Public lyrics database, good for English songs.
  $description:zh-CN: 公开歌词库，英文歌比较全。
- aiStatus: compact
  $name: AI agent status
  $name:zh-CN: AI 助手状态
  $description: Reads %LOCALAPPDATA%\ZenDesktop\ai-status\*.json written by the adapters in tools/ai-status.
  $description:zh-CN: 读取 %LOCALAPPDATA%\ZenDesktop\ai-status\ 里的状态文件（由 tools/ai-status 里的脚本写入）。
  $options:
  - compact: 一行列出所有助手
  - rotate: 轮流显示（每 4 秒换一个，带说明）
  - off: 关闭
- aiDoneSeconds: "120"
  $name: Keep showing Done for
  $name:zh-CN: 「已完成」保留多久
  $description: After this the finished agent disappears from the panel.
  $description:zh-CN: 超过这个时间，完成的助手就不再显示。
  $options:
  - "30": 30 秒
  - "120": 2 分钟
  - "600": 10 分钟
  - "0": 一直显示
- aiProcesses: ""
  $name: Show by process name
  $name:zh-CN: 按进程名显示
  $description: For tools without hooks. Comma separated process names, for example gemini.exe, aider.exe. Shows Running while the process exists.
  $description:zh-CN: 给没有钩子的工具用。填进程名，用逗号分隔，例如 gemini.exe, aider.exe。进程在运行时显示「运行中」。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <tlhelp32.h>
#include <commctrl.h>

#include <windhawk_utils.h>

#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <condition_variable>
#include <cstring>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace winrt::Windows::Media::Control;

// ─────────────────────────────────────────────────────────────────────────────
// Settings. Defaults below mirror the settings block; they are used for every
// value when the mod's settings were never written (e.g. installed by deploy.ps1
// without opening the settings page), detected by an empty positionMode string.
// ─────────────────────────────────────────────────────────────────────────────
enum class PosMode { Widgets, Left, Tray };
enum class SecondLine { Auto, Translation, Info, None };
enum class PanelBg { None, Subtle, Card };
enum class AiMode { Off, Compact, Rotate };

struct Settings {
    PosMode posMode = PosMode::Widgets;
    int xOffset = 12;
    int width = 340;
    bool showArt = true;
    bool showButtons = true;
    bool titleOnly = false;
    SecondLine secondLine = SecondLine::Auto;
    bool showIdle = true;
    bool clickOpens = true;
    std::wstring textColorMode = L"default";
    PanelBg panelBg = PanelBg::None;
    std::wstring font = L"Microsoft YaHei UI";
    int fontSize = 12;
    bool bold = false;
    int offsetMs = 0;
    bool hideInFullscreen = false;
    bool panelGlass = true;
    bool multiMonitor = false;
    bool enableNetease = true;
    bool enableQQ = true;
    bool enableLrclib = true;
    AiMode aiMode = AiMode::Compact;
    int aiDoneSeconds = 120;   // 0 = keep showing
    std::wstring aiProcesses;  // process-presence fallback, comma separated
};
static std::mutex g_settingsLock;
static Settings g_settings;

static std::wstring GetStr(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static void LoadSettings() {
    Settings s;  // defaults
    std::wstring mode = GetStr(L"positionMode");
    if (mode.empty()) {
        Wh_Log(L"settings not written yet, using defaults");
    } else {
        s.posMode = mode == L"left" ? PosMode::Left : mode == L"auto_tray" ? PosMode::Tray : PosMode::Widgets;
        s.xOffset = std::clamp(Wh_GetIntSetting(L"xOffset"), -500, 2000);
        std::wstring size = GetStr(L"sizePreset");
        if (size == L"compact") s.width = 260;
        else if (size == L"wide") s.width = 440;
        else if (size == L"extra") s.width = 560;
        else if (size == L"custom") { int w = Wh_GetIntSetting(L"width"); s.width = w > 0 ? std::clamp(w, 120, 2000) : 340; }
        else s.width = 340;
        s.showArt = Wh_GetIntSetting(L"showArt") != 0;
        s.showButtons = Wh_GetIntSetting(L"showButtons") != 0;
        s.titleOnly = GetStr(L"textMode") == L"title";
        std::wstring sl = GetStr(L"secondLine");
        s.secondLine = sl == L"translation" ? SecondLine::Translation : sl == L"info" ? SecondLine::Info
                     : sl == L"none" ? SecondLine::None : SecondLine::Auto;
        {
            std::wstring wi = GetStr(L"whenIdle");
            s.showIdle = wi != L"hide";  // default show; empty/unknown => show
        }
        s.clickOpens = GetStr(L"clickAction") != L"none";
        s.textColorMode = GetStr(L"textColorMode");
        if (s.textColorMode.empty()) s.textColorMode = L"default";
        std::wstring bg = GetStr(L"panelBackground");
        s.panelBg = bg == L"subtle" ? PanelBg::Subtle : bg == L"card" ? PanelBg::Card : PanelBg::None;
        std::wstring preset = GetStr(L"fontPreset"), custom = GetStr(L"customFont");
        if (preset == L"segoe") s.font = L"Segoe UI";
        else if (preset == L"dengxian") s.font = L"DengXian";
        else if (preset == L"custom" && !custom.empty()) s.font = custom;
        int fs = Wh_GetIntSetting(L"fontSize");
        s.fontSize = fs > 0 ? std::clamp(fs, 6, 48) : 12;
        s.bold = Wh_GetIntSetting(L"bold") != 0;
        s.offsetMs = std::clamp(Wh_GetIntSetting(L"offsetMs"), -60000, 60000);
        s.hideInFullscreen = Wh_GetIntSetting(L"hideInFullscreen") != 0;
        s.panelGlass = Wh_GetIntSetting(L"panelGlass") != 0;
        s.multiMonitor = Wh_GetIntSetting(L"multiMonitor") != 0;
        s.enableNetease = Wh_GetIntSetting(L"enableNetease") != 0;
        s.enableQQ = Wh_GetIntSetting(L"enableQQ") != 0;
        s.enableLrclib = Wh_GetIntSetting(L"enableLrclib") != 0;
        std::wstring ai = GetStr(L"aiStatus");
        s.aiMode = ai == L"off" ? AiMode::Off : ai == L"rotate" ? AiMode::Rotate : AiMode::Compact;
        std::wstring done = GetStr(L"aiDoneSeconds");
        s.aiDoneSeconds = done.empty() ? 120 : std::clamp(_wtoi(done.c_str()), 0, 86400);
        s.aiProcesses = GetStr(L"aiProcesses");
    }
    std::lock_guard<std::mutex> lk(g_settingsLock);
    g_settings = s;
}

static Settings GetSettings() {
    std::lock_guard<std::mutex> lk(g_settingsLock);
    return g_settings;
}

static std::atomic<bool> g_stop{false};
static HANDLE g_stopEvent = nullptr;  // manual-reset; signalled on uninit, wakes sleeping threads
static HANDLE g_cmdEvent = nullptr;   // auto-reset; UI thread -> media thread "a command is queued"

static bool SleepOrStop(DWORD ms) {
    return WaitForSingleObject(g_stopEvent, ms) == WAIT_OBJECT_0;
}

// ─────────────────────────────────────────────────────────────────────────────
// String helpers
// ─────────────────────────────────────────────────────────────────────────────
static std::string ToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}
static std::wstring FromUtf8(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}
static std::wstring UrlEncode(const std::wstring& w) {
    static const char* hex = "0123456789ABCDEF";
    std::wstring out;
    for (unsigned char c : ToUtf8(w)) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') out += (wchar_t)c;
        else { out += L'%'; out += (wchar_t)hex[c >> 4]; out += (wchar_t)hex[c & 15]; }
    }
    return out;
}
static void AppendCodepointUtf8(std::string& out, unsigned cp) {
    if (cp < 0x80) out += (char)cp;
    else if (cp < 0x800) { out += (char)(0xC0 | (cp >> 6)); out += (char)(0x80 | (cp & 0x3F)); }
    else if (cp < 0x10000) { out += (char)(0xE0 | (cp >> 12)); out += (char)(0x80 | ((cp >> 6) & 0x3F)); out += (char)(0x80 | (cp & 0x3F)); }
    else if (cp < 0x110000) { out += (char)(0xF0 | (cp >> 18)); out += (char)(0x80 | ((cp >> 12) & 0x3F)); out += (char)(0x80 | ((cp >> 6) & 0x3F)); out += (char)(0x80 | (cp & 0x3F)); }
}

// QQ lyrics may contain HTML entities (&#58; &#x27; &amp; ...).
static std::string HtmlUnescape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] != '&') { out += s[i]; continue; }
        size_t semi = s.find(';', i);
        if (semi == std::string::npos || semi - i > 10) { out += s[i]; continue; }
        std::string ent = s.substr(i + 1, semi - i - 1);
        unsigned long cp = 0;
        bool ok = true;
        if (ent.size() > 1 && ent[0] == '#') {
            char* e = nullptr;
            cp = (ent[1] == 'x' || ent[1] == 'X') ? strtoul(ent.c_str() + 2, &e, 16) : strtoul(ent.c_str() + 1, &e, 10);
            ok = e && *e == '\0' && cp > 0 && cp < 0x110000;
        } else if (ent == "amp") cp = '&';
        else if (ent == "lt") cp = '<';
        else if (ent == "gt") cp = '>';
        else if (ent == "quot") cp = '"';
        else if (ent == "apos") cp = '\'';
        else if (ent == "nbsp") cp = ' ';
        else ok = false;
        if (!ok) { out += s[i]; continue; }
        AppendCodepointUtf8(out, (unsigned)cp);
        i = semi;
    }
    return out;
}

static bool Base64Decode(const std::string& in, std::string& out) {
    auto val = [](char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+' || c == '-') return 62;
        if (c == '/' || c == '_') return 63;
        return -1;
    };
    out.clear();
    unsigned buf = 0;
    int bits = 0;
    for (char c : in) {
        if (c == '=' || c == '\r' || c == '\n' || c == ' ') continue;
        int v = val(c);
        if (v < 0) return false;
        buf = ((buf << 6) | (unsigned)v) & 0xFFFFFF;
        bits += 6;
        if (bits >= 8) { bits -= 8; out += (char)((buf >> bits) & 0xFF); }
    }
    return true;
}

// Lowercase, drop spaces/punctuation and bracketed suffixes like "(Live)". For fuzzy title/artist compare.
static std::wstring NormalizeForMatch(const std::wstring& in) {
    std::wstring out;
    int depth = 0;
    for (wchar_t c : in) {
        if (c == L'(' || c == L'\xFF08' || c == L'[' || c == L'\x3010') { ++depth; continue; }
        if (c == L')' || c == L'\xFF09' || c == L']' || c == L'\x3011') { if (depth) --depth; continue; }
        if (depth) continue;
        if (iswspace(c) || iswpunct(c) || c == L'\x30FB' || c == L'\x00B7') continue;
        out += (wchar_t)towlower(c);
    }
    return out.empty() ? in : out;
}

// ─────────────────────────────────────────────────────────────────────────────
// Small JSON parser. Depth-limited; numbers keep their raw text so 64-bit ids survive.
// ─────────────────────────────────────────────────────────────────────────────
struct JValue {
    enum Type { Null, Bool, Number, String, Array, Object } type = Null;
    bool b = false;
    std::string s;                 // String value, or raw text of Number
    std::vector<JValue> items;     // Array elements, or Object values
    std::vector<std::string> keys; // Object keys (parallel to items)

    static const JValue& NullValue() { static const JValue n; return n; }
    const JValue& operator[](const char* key) const {
        if (type == Object)
            for (size_t i = 0; i < keys.size(); ++i)
                if (keys[i] == key) return items[i];
        return NullValue();
    }
    const JValue& at(size_t i) const {
        return (type == Array && i < items.size()) ? items[i] : NullValue();
    }
    size_t size() const { return type == Array ? items.size() : 0; }
    std::string str() const { return (type == String || type == Number) ? s : std::string(); }
    long long num() const { return (type == Number || type == String) ? strtoll(s.c_str(), nullptr, 10) : 0; }
};

class JsonParser {
public:
    explicit JsonParser(const std::string& t) : t_(t) {}
    bool Parse(JValue& out) {
        Ws();
        if (!Value(out, 0)) return false;
        Ws();
        return p_ == t_.size();
    }
private:
    const std::string& t_;
    size_t p_ = 0;
    void Ws() { while (p_ < t_.size() && (t_[p_] == ' ' || t_[p_] == '\t' || t_[p_] == '\r' || t_[p_] == '\n')) ++p_; }
    bool Lit(const char* w) {
        size_t n = strlen(w);
        if (t_.compare(p_, n, w) != 0) return false;
        p_ += n;
        return true;
    }
    bool Hex4(unsigned& v) {
        if (p_ + 4 > t_.size()) return false;
        v = 0;
        for (int i = 0; i < 4; ++i) {
            char c = t_[p_++];
            v <<= 4;
            if (c >= '0' && c <= '9') v |= (unsigned)(c - '0');
            else if (c >= 'a' && c <= 'f') v |= (unsigned)(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') v |= (unsigned)(c - 'A' + 10);
            else return false;
        }
        return true;
    }
    bool Str(std::string& out) {
        if (p_ >= t_.size() || t_[p_] != '"') return false;
        ++p_;
        out.clear();
        while (p_ < t_.size()) {
            char c = t_[p_++];
            if (c == '"') return true;
            if (c != '\\') { out += c; continue; }
            if (p_ >= t_.size()) return false;
            char e = t_[p_++];
            switch (e) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                case 'u': {
                    unsigned cp;
                    if (!Hex4(cp)) return false;
                    if (cp >= 0xD800 && cp <= 0xDBFF && p_ + 1 < t_.size() && t_[p_] == '\\' && t_[p_ + 1] == 'u') {
                        p_ += 2;
                        unsigned lo;
                        if (!Hex4(lo)) return false;
                        if (lo >= 0xDC00 && lo <= 0xDFFF) cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                        else { AppendCodepointUtf8(out, 0xFFFD); cp = lo; }
                    }
                    AppendCodepointUtf8(out, cp);
                    break;
                }
                default: return false;
            }
        }
        return false;
    }
    bool Value(JValue& v, int depth) {
        if (depth > 64 || p_ >= t_.size()) return false;
        char c = t_[p_];
        if (c == '{' || c == '[') {
            bool obj = c == '{';
            char close = obj ? '}' : ']';
            v.type = obj ? JValue::Object : JValue::Array;
            ++p_; Ws();
            if (p_ < t_.size() && t_[p_] == close) { ++p_; return true; }
            while (true) {
                Ws();
                if (obj) {
                    std::string k;
                    if (!Str(k)) return false;
                    Ws();
                    if (p_ >= t_.size() || t_[p_++] != ':') return false;
                    Ws();
                    v.keys.push_back(std::move(k));
                }
                v.items.emplace_back();
                if (!Value(v.items.back(), depth + 1)) return false;
                Ws();
                if (p_ >= t_.size()) return false;
                if (t_[p_] == ',') { ++p_; continue; }
                if (t_[p_] == close) { ++p_; return true; }
                return false;
            }
        }
        if (c == '"') { v.type = JValue::String; return Str(v.s); }
        if (Lit("true")) { v.type = JValue::Bool; v.b = true; return true; }
        if (Lit("false")) { v.type = JValue::Bool; v.b = false; return true; }
        if (Lit("null")) { v.type = JValue::Null; return true; }
        if (c == '-' || (c >= '0' && c <= '9')) {
            size_t st = p_++;
            while (p_ < t_.size() && (isdigit((unsigned char)t_[p_]) || t_[p_] == '.' || t_[p_] == 'e' ||
                                      t_[p_] == 'E' || t_[p_] == '+' || t_[p_] == '-')) ++p_;
            v.type = JValue::Number;
            v.s = t_.substr(st, p_ - st);
            return true;
        }
        return false;
    }
};

// Parses JSON, tolerating a JSONP wrapper like `callback({...})`.
static bool ParseJson(const std::string& text, JValue& out) {
    out = JValue();
    if (JsonParser(text).Parse(out)) return true;
    size_t a = text.find_first_of("{["), b = text.find_last_of("}]");
    if (a == std::string::npos || b == std::string::npos || b <= a) return false;
    std::string inner = text.substr(a, b - a + 1);
    out = JValue();
    return JsonParser(inner).Parse(out);
}

// ─────────────────────────────────────────────────────────────────────────────
// HTTP (WinHTTP, synchronous, 3s timeouts, background threads only)
// ─────────────────────────────────────────────────────────────────────────────
static const wchar_t kBrowserUA[] =
    L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
static const wchar_t kZenUA[] = L"ZenDesktop-TaskbarLyrics/0.2.0 (https://github.com/Liset999)";

static thread_local bool t_gotResponse = false;  // any HTTP response seen (distinguishes offline from not-found)

static bool HttpGet(const wchar_t* host, const std::wstring& path, std::string& body, DWORD* status,
                    const wchar_t* extraHeaders = nullptr, const wchar_t* userAgent = kZenUA) {
    bool ok = false;
    *status = 0;
    body.clear();
    if (g_stop) return false;
    HINTERNET ses = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
                                WINHTTP_NO_PROXY_BYPASS, 0);
    if (!ses) ses = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
                                WINHTTP_NO_PROXY_BYPASS, 0);
    if (!ses) return false;
    WinHttpSetTimeouts(ses, 3000, 3000, 3000, 3000);
    HINTERNET con = WinHttpConnect(ses, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET req = con ? WinHttpOpenRequest(con, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                             WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE) : nullptr;
    if (req && WinHttpSendRequest(req, extraHeaders ? extraHeaders : WINHTTP_NO_ADDITIONAL_HEADERS,
                                  extraHeaders ? (DWORD)-1L : 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(req, nullptr)) {
        t_gotResponse = true;
        DWORD sz = sizeof(DWORD);
        WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, status, &sz, WINHTTP_NO_HEADER_INDEX);
        DWORD avail = 0;
        ok = true;
        while (WinHttpQueryDataAvailable(req, &avail) && avail) {
            if (g_stop) { ok = false; break; }
            std::string chunk(avail, '\0');
            DWORD read = 0;
            if (!WinHttpReadData(req, chunk.data(), avail, &read)) { ok = false; break; }
            body.append(chunk.data(), read);
            if (body.size() > 2 * 1024 * 1024) { ok = false; break; }
        }
    }
    if (req) WinHttpCloseHandle(req);
    if (con) WinHttpCloseHandle(con);
    WinHttpCloseHandle(ses);
    return ok && *status == 200;
}

static bool HttpGetJson(const wchar_t* host, const std::wstring& path, JValue& out,
                        const wchar_t* extraHeaders = nullptr, const wchar_t* userAgent = kZenUA) {
    std::string body;
    DWORD status = 0;
    if (!HttpGet(host, path, body, &status, extraHeaders, userAgent)) return false;
    return ParseJson(body, out);
}

// ─────────────────────────────────────────────────────────────────────────────
// Lyrics providers
// ─────────────────────────────────────────────────────────────────────────────
struct LyricsQuery {
    std::wstring title, artist, album;
    int durationSec = 0;      // 0 = unknown
    std::wstring sourceApp;   // GSMTC SourceAppUserModelId, e.g. "cloudmusic.exe", "QQMusic.exe"
};

struct LyricsResult {
    std::string lrc;          // UTF-8 line-synced LRC (original)
    std::string translation;  // UTF-8 LRC translation, may be empty
};

class ILyricsProvider {
public:
    virtual ~ILyricsProvider() = default;
    virtual const wchar_t* Name() const = 0;
    // Runs on the lyrics worker thread; may block for a few seconds. Empty lrc = not found.
    virtual LyricsResult Fetch(const LyricsQuery& q) = 0;
};

static bool LooksLikeSyncedLrc(const std::string& s) {
    // Any "[d...:" time tag (metadata tags like [ti:..] usually come first).
    for (size_t p = s.find('['); p != std::string::npos && p + 3 < s.size(); p = s.find('[', p + 1))
        if (isdigit((unsigned char)s[p + 1]) && (s[p + 2] == ':' || (isdigit((unsigned char)s[p + 2]) && s[p + 3] == ':')))
            return true;
    return false;
}

// Candidate matching shared by search-based providers.
struct LyricsCandidate {
    std::string id;
    std::wstring title, artist;
    int durationSec = 0;
};

// Score: duration within 3s is required when both durations are known; title/artist similarity breaks ties.
// Returns nullptr when nothing is plausible (better show "title - artist" than wrong lyrics).
static const LyricsCandidate* PickCandidate(const std::vector<LyricsCandidate>& cands, const LyricsQuery& q) {
    std::wstring qt = NormalizeForMatch(q.title), qa = NormalizeForMatch(q.artist);
    const LyricsCandidate* best = nullptr;
    int bestScore = INT_MIN;
    for (auto& c : cands) {
        int score = 0;
        std::wstring ct = NormalizeForMatch(c.title), ca = NormalizeForMatch(c.artist);
        bool titleEq = !qt.empty() && ct == qt;
        bool titleLike = titleEq || (!qt.empty() && !ct.empty() && (ct.find(qt) != std::wstring::npos || qt.find(ct) != std::wstring::npos));
        bool artistLike = qa.empty() || (!ca.empty() && (ca.find(qa) != std::wstring::npos || qa.find(ca) != std::wstring::npos));
        if (q.durationSec > 0 && c.durationSec > 0) {
            int d = abs(c.durationSec - q.durationSec);
            if (d > 3) continue;
            score += 30 - d * 5;
        } else if (!titleLike) {
            continue;  // no duration to verify -> require title similarity
        }
        score += titleEq ? 40 : (titleLike ? 20 : 0);
        score += artistLike ? 20 : 0;
        if (score > bestScore) { bestScore = score; best = &c; }
    }
    return best;
}

static std::wstring SearchKeyword(const LyricsQuery& q) {
    return q.artist.empty() ? q.title : q.title + L" " + q.artist;
}

static std::wstring JoinArtists(const JValue& arr) {
    std::wstring out;
    for (size_t i = 0; i < arr.size(); ++i) {
        std::wstring n = FromUtf8(arr.at(i)["name"].str());
        if (n.empty()) continue;
        if (!out.empty()) out += L"/";
        out += n;
    }
    return out;
}

class LrclibProvider : public ILyricsProvider {
public:
    const wchar_t* Name() const override { return L"lrclib"; }
    LyricsResult Fetch(const LyricsQuery& q) override {
        LyricsResult r;
        JValue j;
        // 1) exact match (LRCLIB tolerates ±2s on duration)
        std::wstring path = L"/api/get?track_name=" + UrlEncode(q.title) + L"&artist_name=" + UrlEncode(q.artist);
        if (!q.album.empty()) path += L"&album_name=" + UrlEncode(q.album);
        if (q.durationSec > 0) path += L"&duration=" + std::to_wstring(q.durationSec);
        if (HttpGetJson(L"lrclib.net", path, j)) {
            r.lrc = j["syncedLyrics"].str();
            if (LooksLikeSyncedLrc(r.lrc)) return r;
        }
        if (g_stop) return {};
        // 2) search; reuse duration/title matching
        path = L"/api/search?track_name=" + UrlEncode(q.title) + L"&artist_name=" + UrlEncode(q.artist);
        if (!HttpGetJson(L"lrclib.net", path, j)) return {};
        std::vector<LyricsCandidate> cands;
        std::vector<std::string> lyrics;
        for (size_t i = 0; i < j.size() && i < 20; ++i) {
            const JValue& it = j.at(i);
            std::string synced = it["syncedLyrics"].str();
            if (!LooksLikeSyncedLrc(synced)) continue;
            cands.push_back({std::to_string(lyrics.size()), FromUtf8(it["trackName"].str()),
                             FromUtf8(it["artistName"].str()), (int)(atof(it["duration"].str().c_str()) + 0.5)});
            lyrics.push_back(std::move(synced));
        }
        if (const LyricsCandidate* c = PickCandidate(cands, q)) r.lrc = lyrics[(size_t)atoi(c->id.c_str())];
        else r.lrc.clear();
        return r;
    }
};

// NetEase Cloud Music. Unofficial API, may change; all request details stay inside this class.
// search: music.163.com/api/cloudsearch/pc?type=1&limit=8&offset=0&s=..  -> result.songs[].{id,name,ar[].name,dt(ms)}
// lyric:  music.163.com/api/song/lyric?id=..&lv=1&tv=-1                -> lrc.lyric, tlyric.lyric
class NeteaseProvider : public ILyricsProvider {
public:
    const wchar_t* Name() const override { return L"netease"; }
    LyricsResult Fetch(const LyricsQuery& q) override {
        static const wchar_t kHdr[] = L"Referer: https://music.163.com/\r\n";
        JValue j;
        std::wstring path = L"/api/cloudsearch/pc?type=1&limit=8&offset=0&s=" + UrlEncode(SearchKeyword(q));
        if (!HttpGetJson(L"music.163.com", path, j, kHdr, kBrowserUA)) return {};
        const JValue& songs = j["result"]["songs"];
        std::vector<LyricsCandidate> cands;
        for (size_t i = 0; i < songs.size(); ++i) {
            const JValue& s = songs.at(i);
            std::string id = s["id"].str();
            if (id.empty()) continue;
            cands.push_back({id, FromUtf8(s["name"].str()), JoinArtists(s["ar"]), (int)((s["dt"].num() + 500) / 1000)});
        }
        const LyricsCandidate* c = PickCandidate(cands, q);
        if (!c || g_stop) return {};
        path = L"/api/song/lyric?lv=1&tv=-1&id=" + FromUtf8(c->id);
        if (!HttpGetJson(L"music.163.com", path, j, kHdr, kBrowserUA)) return {};
        LyricsResult r;
        r.lrc = j["lrc"]["lyric"].str();
        r.translation = j["tlyric"]["lyric"].str();
        if (!LooksLikeSyncedLrc(r.lrc)) return {};
        if (!LooksLikeSyncedLrc(r.translation)) r.translation.clear();
        return r;
    }
};

// QQ Music. Unofficial API, may change; all request details stay inside this class.
// search: c.y.qq.com/soso/fcgi-bin/search_for_qq_cp?format=json&...&w=..  -> data.song.list[].{songmid,songname,singer[].name,interval(s)}
// lyric:  c.y.qq.com/lyric/fcgi-bin/fcg_query_lyric_new.fcg?songmid=..&g_tk=5381&format=json&nobase64=1&platform=yqq -> lyric, trans
// Needs Referer https://y.qq.com/. Lyric text can come back base64 (if nobase64 is ignored) and/or HTML-entity-encoded.
class QQMusicProvider : public ILyricsProvider {
public:
    const wchar_t* Name() const override { return L"qqmusic"; }
    LyricsResult Fetch(const LyricsQuery& q) override {
        static const wchar_t kHdr[] = L"Referer: https://y.qq.com/\r\n";
        JValue j;
        std::wstring path = L"/soso/fcgi-bin/search_for_qq_cp?format=json&n=8&p=1&t=0"
                            L"&remoteplace=txt.yqq.song&platform=yqq.json&w=" + UrlEncode(SearchKeyword(q));
        if (!HttpGetJson(L"c.y.qq.com", path, j, kHdr, kBrowserUA)) return {};
        const JValue& list = j["data"]["song"]["list"];
        std::vector<LyricsCandidate> cands;
        for (size_t i = 0; i < list.size(); ++i) {
            const JValue& s = list.at(i);
            std::string mid = s["songmid"].str();
            if (mid.empty()) continue;
            cands.push_back({mid, FromUtf8(s["songname"].str()), JoinArtists(s["singer"]), (int)s["interval"].num()});
        }
        const LyricsCandidate* c = PickCandidate(cands, q);
        if (!c || g_stop) return {};
        path = L"/lyric/fcgi-bin/fcg_query_lyric_new.fcg?g_tk=5381&format=json&inCharset=utf8&outCharset=utf-8"
               L"&nobase64=1&platform=yqq&songmid=" + UrlEncode(FromUtf8(c->id));
        if (!HttpGetJson(L"c.y.qq.com", path, j, kHdr, kBrowserUA)) return {};
        LyricsResult r;
        r.lrc = Decode(j["lyric"].str());
        r.translation = Decode(j["trans"].str());
        if (!LooksLikeSyncedLrc(r.lrc)) return {};
        if (!LooksLikeSyncedLrc(r.translation)) r.translation.clear();
        return r;
    }
private:
    static std::string Decode(const std::string& raw) {
        if (raw.empty()) return {};
        std::string text = raw;
        if (!LooksLikeSyncedLrc(text)) {
            std::string dec;
            if (Base64Decode(text, dec) && LooksLikeSyncedLrc(dec)) text = dec;
        }
        return HtmlUnescape(text);
    }
};

// Provider order by the playing app (same idea as TaskbarLyrics): the app's own service first.
// Unknown app -> QQ, NetEase, LRCLIB. (Kugou not implemented in v0.1.)
static std::vector<ILyricsProvider*> OrderProviders(const std::wstring& sourceApp,
                                                    const std::vector<std::unique_ptr<ILyricsProvider>>& all,
                                                    const Settings& s) {
    std::wstring app;
    for (wchar_t c : sourceApp) app += (wchar_t)towlower(c);
    std::vector<const wchar_t*> order;
    if (app.find(L"cloudmusic") != std::wstring::npos || app.find(L"netease") != std::wstring::npos)
        order = {L"netease", L"qqmusic", L"lrclib"};
    else
        order = {L"qqmusic", L"netease", L"lrclib"};
    std::vector<ILyricsProvider*> out;
    for (const wchar_t* name : order) {
        if ((!s.enableNetease && !wcscmp(name, L"netease")) || (!s.enableQQ && !wcscmp(name, L"qqmusic")) ||
            (!s.enableLrclib && !wcscmp(name, L"lrclib")))
            continue;
        for (auto& p : all) if (!wcscmp(p->Name(), name)) out.push_back(p.get());
    }
    return out;
}

// ─────────────────────────────────────────────────────────────────────────────
// Cache: memory + %LOCALAPPDATA%\ZenDesktop\lyrics\<fnv64>.lrc (+ <fnv64>.t.lrc for translation).
// Empty .lrc file = negative result ("not found"), retried after 1 day.
// ─────────────────────────────────────────────────────────────────────────────
static std::wstring CacheKey(const LyricsQuery& q) {
    std::string s = ToUtf8(q.title + L"\x1F" + q.artist);
    unsigned long long h = 1469598103934665603ULL;
    for (unsigned char c : s) { h ^= c; h *= 1099511628211ULL; }
    wchar_t buf[17];
    swprintf_s(buf, L"%016llx", h);
    return buf;
}
static std::wstring CacheDir() {
    static std::wstring dir;  // only touched from the lyrics thread
    if (!dir.empty()) return dir;
    PWSTR base = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &base))) {
        std::wstring d = std::wstring(base) + L"\\ZenDesktop\\lyrics";
        int rc = SHCreateDirectoryExW(nullptr, d.c_str(), nullptr);
        if (rc == ERROR_SUCCESS || rc == ERROR_ALREADY_EXISTS || rc == ERROR_FILE_EXISTS) dir = d;
    }
    CoTaskMemFree(base);
    return dir;
}
// Returns: 1 = hit (out may be empty for negative entry), 0 = miss.
static int ReadCacheFile(const std::wstring& path, std::string& out, bool checkNegativeAge) {
    out.clear();
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return 0;
    LARGE_INTEGER size{};
    FILETIME mtime{};
    GetFileSizeEx(f, &size);
    GetFileTime(f, nullptr, nullptr, &mtime);
    int hit = 0;
    if (size.QuadPart == 0) {
        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        ULONGLONG age = (((ULONGLONG)now.dwHighDateTime << 32) | now.dwLowDateTime) -
                        (((ULONGLONG)mtime.dwHighDateTime << 32) | mtime.dwLowDateTime);
        hit = (!checkNegativeAge || age < 864000000000ULL) ? 1 : 0;  // 1 day in 100ns
    } else if (size.QuadPart < 4 * 1024 * 1024) {
        out.resize((size_t)size.QuadPart);
        DWORD read = 0;
        hit = (ReadFile(f, out.data(), (DWORD)out.size(), &read, nullptr) && read == out.size()) ? 1 : 0;
        if (!hit) out.clear();
    }
    CloseHandle(f);
    return hit;
}
static void WriteCacheFile(const std::wstring& path, const std::string& data) {
    std::wstring tmp = path + L".tmp";
    HANDLE f = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD w = 0;
    bool ok = data.empty() || (WriteFile(f, data.data(), (DWORD)data.size(), &w, nullptr) && w == data.size());
    CloseHandle(f);
    if (ok) MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING);
    else DeleteFileW(tmp.c_str());
}
static bool DiskCacheRead(const std::wstring& key, LyricsResult& out) {
    std::wstring dir = CacheDir();
    if (dir.empty()) return false;
    if (!ReadCacheFile(dir + L"\\" + key + L".lrc", out.lrc, true)) return false;
    if (!out.lrc.empty()) ReadCacheFile(dir + L"\\" + key + L".t.lrc", out.translation, false);
    return true;
}
static void DiskCacheWrite(const std::wstring& key, const LyricsResult& r) {
    std::wstring dir = CacheDir();
    if (dir.empty()) return;
    if (!r.translation.empty()) WriteCacheFile(dir + L"\\" + key + L".t.lrc", r.translation);
    WriteCacheFile(dir + L"\\" + key + L".lrc", r.lrc);
}

// ─────────────────────────────────────────────────────────────────────────────
// LRC parser (plain line-synced LRC; word-level <..> tags are stripped)
// ─────────────────────────────────────────────────────────────────────────────
struct LyricLine { int timeMs; std::wstring text; };
struct Lyrics {
    std::vector<LyricLine> lines;
    std::vector<LyricLine> translation;
};

static std::vector<LyricLine> ParseLrc(const std::string& utf8) {
    std::vector<LyricLine> lines;
    int globalOffset = 0;  // [offset:+/-ms], positive = lyrics earlier
    std::wstring all = FromUtf8(utf8);
    size_t start = 0;
    while (start < all.size()) {
        size_t nl = all.find(L'\n', start);
        std::wstring ln = all.substr(start, nl == std::wstring::npos ? std::wstring::npos : nl - start);
        start = nl == std::wstring::npos ? all.size() : nl + 1;
        if (!ln.empty() && ln.back() == L'\r') ln.pop_back();
        std::vector<int> times;
        size_t p = 0;
        while (p < ln.size() && ln[p] == L'[') {
            size_t close = ln.find(L']', p);
            if (close == std::wstring::npos) break;
            std::wstring tag = ln.substr(p + 1, close - p - 1);
            if (!tag.empty() && iswdigit(tag[0])) {
                wchar_t* e = nullptr;
                long mm = wcstol(tag.c_str(), &e, 10);
                if (e && *e == L':') {
                    long ss = wcstol(e + 1, &e, 10);
                    int frac = 0, fracDigits = 0;
                    if (e && (*e == L'.' || *e == L':')) {
                        const wchar_t* f = e + 1;
                        while (iswdigit(*f) && fracDigits < 3) { frac = frac * 10 + (*f - L'0'); ++f; ++fracDigits; }
                    }
                    while (fracDigits > 0 && fracDigits < 3) { frac *= 10; ++fracDigits; }
                    if (mm >= 0 && mm < 600 && ss >= 0 && ss < 60) times.push_back((int)(mm * 60 + ss) * 1000 + frac);
                }
            } else if (tag.rfind(L"offset:", 0) == 0) {
                globalOffset = _wtoi(tag.c_str() + 7);
            }
            p = close + 1;
        }
        if (times.empty()) continue;
        std::wstring text = ln.substr(p);
        for (size_t a; (a = text.find(L'<')) != std::wstring::npos;) {
            size_t b = text.find(L'>', a);
            if (b == std::wstring::npos || b - a > 12) break;
            text.erase(a, b - a + 1);
        }
        while (!text.empty() && iswspace(text.back())) text.pop_back();
        while (!text.empty() && iswspace(text.front())) text.erase(text.begin());
        for (int t : times) lines.push_back({t, text});
    }
    for (auto& l : lines) l.timeMs -= globalOffset;
    std::stable_sort(lines.begin(), lines.end(), [](const LyricLine& a, const LyricLine& b) { return a.timeMs < b.timeMs; });
    return lines;
}

static const LyricLine* FindLine(const std::vector<LyricLine>& lines, int posMs) {
    auto it = std::upper_bound(lines.begin(), lines.end(), posMs,
                               [](int v, const LyricLine& l) { return v < l.timeMs; });
    if (it == lines.begin()) return nullptr;
    return &*(it - 1);
}

// Translation line whose timestamp matches the original within 300ms.
static const LyricLine* FindTranslation(const std::vector<LyricLine>& tr, int timeMs) {
    const LyricLine* l = FindLine(tr, timeMs + 300);
    return (l && abs(l->timeMs - timeMs) <= 300 && !l->text.empty()) ? l : nullptr;
}


// ─────────────────────────────────────────────────────────────────────────────
// Shared playback state (written by media/lyrics threads, read by UI thread)
// ─────────────────────────────────────────────────────────────────────────────
struct PlaybackState {
    bool hasSession = false;
    bool playing = false;
    std::wstring title, artist;
    std::wstring sourceApp;        // GSMTC SourceAppUserModelId
    long long positionMs = 0;      // position reported by GSMTC
    ULONGLONG sampledAtTick = 0;   // GetTickCount64 at which positionMs was valid
    double rate = 1.0;
    bool canPrev = true, canNext = true, canPlayPause = true;
    std::wstring trackKey;         // title \x1F artist
    std::shared_ptr<const Lyrics> lyrics;  // null = none/pending
    std::shared_ptr<const std::vector<BYTE>> thumb;  // encoded image bytes (PNG/JPEG), null = none
    unsigned thumbVersion = 0;     // bumped whenever thumb changes
};
static std::mutex g_stateLock;
static PlaybackState g_state;

// Lyrics worker: single pending request, latest wins.
static std::mutex g_reqLock;
static std::condition_variable g_reqCv;
static LyricsQuery g_pendingReq;
static bool g_hasPendingReq = false;

static void RequestLyrics(const LyricsQuery& q) {
    {
        std::lock_guard<std::mutex> lk(g_reqLock);
        g_pendingReq = q;
        g_hasPendingReq = true;
    }
    g_reqCv.notify_one();
}

static void LyricsThread() {
    std::unordered_map<std::wstring, std::shared_ptr<const Lyrics>> mem;
    std::vector<std::unique_ptr<ILyricsProvider>> providers;
    providers.push_back(std::make_unique<NeteaseProvider>());
    providers.push_back(std::make_unique<QQMusicProvider>());
    providers.push_back(std::make_unique<LrclibProvider>());

    while (!g_stop) {
        LyricsQuery q;
        {
            std::unique_lock<std::mutex> lk(g_reqLock);
            g_reqCv.wait(lk, [] { return g_hasPendingReq || g_stop.load(); });
            if (g_stop) break;
            q = g_pendingReq;
            g_hasPendingReq = false;
        }
        std::wstring key = CacheKey(q);
        std::shared_ptr<const Lyrics> result;
        auto it = mem.find(key);
        if (it != mem.end()) {
            result = it->second;
        } else {
            LyricsResult r;
            const wchar_t* from = L"disk cache";
            if (!DiskCacheRead(key, r)) {
                from = L"none";
                t_gotResponse = false;
                for (ILyricsProvider* p : OrderProviders(q.sourceApp, providers, GetSettings())) {
                    if (g_stop) break;
                    Wh_Log(L"[lyrics] trying %s for '%s' (app=%s)", p->Name(), q.title.c_str(), q.sourceApp.c_str());
                    r = p->Fetch(q);
                    if (!r.lrc.empty()) { from = p->Name(); break; }
                }
                if (g_stop) break;
                // Offline (no provider got any HTTP response): don't cache "not found", retry next time.
                if (!r.lrc.empty() || t_gotResponse) DiskCacheWrite(key, r);
                else from = L"none (offline, not cached)";
            }
            Wh_Log(L"[lyrics] '%s' -> %s", q.title.c_str(), from);
            auto parsed = std::make_shared<Lyrics>();
            parsed->lines = ParseLrc(r.lrc);
            parsed->translation = ParseLrc(r.translation);
            if (!parsed->lines.empty()) result = parsed;
            if (mem.size() > 200) mem.clear();
            if (result || wcscmp(from, L"none (offline, not cached)") != 0) mem[key] = result;
        }
        std::lock_guard<std::mutex> lk(g_stateLock);
        if (g_state.trackKey == q.title + L"\x1F" + q.artist) g_state.lyrics = result;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Playback commands: the UI thread only queues them; the media thread runs the
// WinRT calls (they can block), so neither Explorer's UI thread nor ours waits.
// ─────────────────────────────────────────────────────────────────────────────
enum class MediaCmd : int { None = 0, Prev, PlayPause, Next, OpenPlayer };
static std::atomic<int> g_pendingCmd{0};

static void PostMediaCmd(MediaCmd c) {
    g_pendingCmd = (int)c;  // latest wins; double clicks within one poll collapse into one
    if (g_cmdEvent) SetEvent(g_cmdEvent);
    if (c == MediaCmd::PlayPause) {
        std::lock_guard<std::mutex> lk(g_stateLock);  // optimistic icon flip; next poll corrects it
        if (g_state.hasSession) {
            long long now = (long long)GetTickCount64();
            if (g_state.playing) g_state.positionMs += (long long)((double)(now - (long long)g_state.sampledAtTick) * g_state.rate);
            g_state.sampledAtTick = (ULONGLONG)now;
            g_state.playing = !g_state.playing;
        }
    }
}

static std::wstring ExeNameOfPid(DWORD pid) {
    std::wstring out;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return out;
    wchar_t path[MAX_PATH];
    DWORD n = MAX_PATH;
    if (QueryFullProcessImageNameW(h, 0, path, &n)) {
        const wchar_t* name = wcsrchr(path, L'\\');
        out = name ? name + 1 : path;
    }
    CloseHandle(h);
    return out;
}

// Desktop players report their exe name as AUMID (QQMusic.exe, cloudmusic.exe, Spotify.exe, msedge...).
// Packaged apps report "Family!App" and are activated through IApplicationActivationManager.
struct FindPlayerCtx { std::wstring exe; HWND best = nullptr; };
static BOOL CALLBACK FindPlayerProc(HWND hwnd, LPARAM lp) {
    auto* c = (FindPlayerCtx*)lp;
    if (GetWindow(hwnd, GW_OWNER) || (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) return TRUE;
    if (!IsWindowVisible(hwnd)) return TRUE;  // tray-hidden players: don't resurrect random hidden windows
    if (GetWindowTextLengthW(hwnd) == 0) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    std::wstring exe = ExeNameOfPid(pid);
    if (exe.empty()) return TRUE;
    std::wstring bare = exe;
    if (bare.size() > 4 && _wcsicmp(bare.c_str() + bare.size() - 4, L".exe") == 0) bare.resize(bare.size() - 4);
    if (_wcsicmp(exe.c_str(), c->exe.c_str()) == 0 || _wcsicmp(bare.c_str(), c->exe.c_str()) == 0) {
        c->best = hwnd;  // EnumWindows walks top-down in Z order: first match is the topmost one
        return FALSE;
    }
    return TRUE;
}

static void OpenPlayer(const std::wstring& aumid) {
    if (aumid.empty()) return;
    if (aumid.find(L'!') != std::wstring::npos) {
        IApplicationActivationManager* am = nullptr;
        // CLSID_ApplicationActivationManager, defined here so no extra uuid import library is needed.
        static const CLSID kClsidAam = {0x45BA127D, 0x10A8, 0x46EA, {0x8A, 0xB7, 0x56, 0xEA, 0x90, 0x78, 0x94, 0x3C}};
        if (SUCCEEDED(CoCreateInstance(kClsidAam, nullptr, CLSCTX_LOCAL_SERVER,
                                       IID_PPV_ARGS(&am)))) {
            DWORD pid = 0;
            HRESULT hr = am->ActivateApplication(aumid.c_str(), nullptr, AO_NONE, &pid);
            Wh_Log(L"ActivateApplication(%s) 0x%08X", aumid.c_str(), (unsigned)hr);
            (void)hr;
            am->Release();
        }
        return;
    }
    FindPlayerCtx ctx;
    ctx.exe = aumid;
    if (const wchar_t* slash = wcsrchr(aumid.c_str(), L'\\')) ctx.exe = slash + 1;
    EnumWindows(FindPlayerProc, (LPARAM)&ctx);
    if (!ctx.best) { Wh_Log(L"no visible window for %s", aumid.c_str()); return; }
    if (IsIconic(ctx.best)) ShowWindowAsync(ctx.best, SW_RESTORE);  // async: a hung player can't block us
    SetForegroundWindow(ctx.best);  // allowed: Explorer received the click that triggered this
}

static void RunMediaCmd(GlobalSystemMediaTransportControlsSessionManager const& mgr, MediaCmd c) {
    if (!mgr) return;
    auto session = mgr.GetCurrentSession();
    if (!session) return;
    switch (c) {
        case MediaCmd::Prev: session.TrySkipPreviousAsync().get(); break;
        case MediaCmd::Next: session.TrySkipNextAsync().get(); break;
        case MediaCmd::PlayPause: {
            auto pb = session.GetPlaybackInfo();
            auto ctl = pb.Controls();
            bool playing = pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            if (ctl.IsPlayPauseToggleEnabled()) session.TryTogglePlayPauseAsync().get();
            else if (playing) session.TryPauseAsync().get();
            else session.TryPlayAsync().get();
            break;
        }
        case MediaCmd::OpenPlayer: OpenPlayer(std::wstring{session.SourceAppUserModelId()}); break;
        default: break;
    }
}

// Reads the GSMTC thumbnail into memory (max 4 MB). Returns null when absent/unreadable.
static std::shared_ptr<const std::vector<BYTE>> ReadThumbnail(
    GlobalSystemMediaTransportControlsSessionMediaProperties const& props) {
    using namespace winrt::Windows::Storage::Streams;
    auto ref = props.Thumbnail();
    if (!ref) return nullptr;
    auto stream = ref.OpenReadAsync().get();
    if (!stream) return nullptr;
    uint64_t size = stream.Size();
    if (size == 0 || size > 4 * 1024 * 1024) return nullptr;
    DataReader reader(stream);
    uint32_t got = reader.LoadAsync((uint32_t)size).get();
    if (got == 0) return nullptr;
    auto bytes = std::make_shared<std::vector<BYTE>>(got);
    reader.ReadBytes(winrt::array_view<uint8_t>(bytes->data(), bytes->data() + got));
    return bytes;
}

// ─────────────────────────────────────────────────────────────────────────────
// GSMTC media thread. Polls every 500ms, or immediately after a queued command
// (no WinRT event handlers: simpler lifetime on unload).
// ─────────────────────────────────────────────────────────────────────────────
static void MediaThread() {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (...) {
    }
    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    std::wstring lastKey, lastLoggedApp;
    bool loggedNoSession = false;
    long long lastReportedPos = -1;
    ULONGLONG trackSince = 0, lastThumbTry = 0;
    int thumbTries = 0;
    bool thumbLoaded = false, thumbRefreshed = false;
    // When a new app starts (browser, game launcher, video player) Windows may briefly make
    // its empty session "current", or the player republishes its session with an empty
    // title. 0.3.2 cleared hasSession on the first such poll, so the panel flashed to
    // "Not playing" (or hid) and back. Only report "no media" once it lasts 1.5 s.
    ULONGLONG noMediaSince = 0;
    auto markNoMedia = [&]() {
        ULONGLONG now = GetTickCount64();
        if (!noMediaSince) noMediaSince = now;
        if (now - noMediaSince >= 1500) {
            std::lock_guard<std::mutex> lk(g_stateLock);
            g_state.hasSession = false;
        }
    };
    while (!g_stop) {
        try {
            if (!mgr) {
                mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
                Wh_Log(L"[media] GSMTC manager %s", mgr ? L"ready" : L"null");
            }
            if (MediaCmd c = (MediaCmd)g_pendingCmd.exchange(0); c != MediaCmd::None) {
                try { RunMediaCmd(mgr, c); } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"command %d failed 0x%08X", (int)c, (unsigned)e.code());
                }
                // Give the player a moment to apply it, so the next poll doesn't undo the optimistic icon.
                if (SleepOrStop(300)) break;
            }
            GlobalSystemMediaTransportControlsSession session{nullptr};
            if (mgr) session = mgr.GetCurrentSession();
            if (!session) {
                if (!lastLoggedApp.empty() || !loggedNoSession) {
                    Wh_Log(L"[media] no media session (player not running or no system media controls)");
                    lastLoggedApp.clear();
                    loggedNoSession = true;
                }
                markNoMedia();
            } else {
                auto props = session.TryGetMediaPropertiesAsync().get();
                auto tl = session.GetTimelineProperties();
                auto pb = session.GetPlaybackInfo();
                auto ctl = pb.Controls();
                std::wstring title{props.Title()}, artist{props.Artist()}, album{props.AlbumTitle()};
                std::wstring app{session.SourceAppUserModelId()};
                if (app != lastLoggedApp) {
                    Wh_Log(L"[media] session: %s, title empty=%d", app.c_str(), title.empty() ? 1 : 0);
                    lastLoggedApp = app;
                    loggedNoSession = false;
                }
                if (title.empty()) {
                    // Transient (session switching / player republishing); keep the last track.
                    markNoMedia();
                    HANDLE w2[2] = {g_stopEvent, g_cmdEvent};
                    if (WaitForMultipleObjects(2, w2, FALSE, 500) == WAIT_OBJECT_0) break;
                    continue;
                }
                noMediaSince = 0;
                long long posMs = tl.Position().count() / 10000;
                long long durMs = (tl.EndTime() - tl.StartTime()).count() / 10000;
                bool playing = pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                double rate = 1.0;
                if (auto r = pb.PlaybackRate()) rate = r.Value();
                if (rate <= 0.0 || rate > 4.0) rate = 1.0;
                long long ageMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                                      winrt::clock::now() - tl.LastUpdatedTime()).count();
                std::wstring key = title + L"\x1F" + artist;
                bool newTrack = key != lastKey;
                ULONGLONG now = GetTickCount64();
                if (newTrack) { trackSince = now; thumbTries = 0; thumbLoaded = false; thumbRefreshed = false; lastThumbTry = 0; }

                // Thumbnail: players often publish it a moment after the title. Try up to 8x (1/s) until
                // one loads, then re-read once ~3s into the track to catch a late cover update.
                std::shared_ptr<const std::vector<BYTE>> thumb;
                bool thumbChanged = false;
                bool wantThumb = (!thumbLoaded && thumbTries < 8 && now - lastThumbTry >= 1000) ||
                                 (thumbLoaded && !thumbRefreshed && now - trackSince >= 3000);
                if (wantThumb && !title.empty()) {
                    if (thumbLoaded) thumbRefreshed = true; else ++thumbTries;
                    lastThumbTry = now;
                    try { thumb = ReadThumbnail(props); } catch (...) { thumb = nullptr; }
                    if (thumb) { thumbLoaded = true; thumbChanged = true; }
                }
                {
                    std::lock_guard<std::mutex> lk(g_stateLock);
                    g_state.hasSession = true;
                    g_state.playing = playing;
                    g_state.rate = rate;
                    g_state.title = title;
                    g_state.artist = artist;
                    g_state.sourceApp = app;
                    g_state.canPrev = ctl.IsPreviousEnabled();
                    g_state.canNext = ctl.IsNextEnabled();
                    g_state.canPlayPause = ctl.IsPlayPauseToggleEnabled() || ctl.IsPlayEnabled() || ctl.IsPauseEnabled();
                    if (newTrack) {
                        g_state.trackKey = key;
                        g_state.lyrics = nullptr;
                        if (g_state.thumb) { g_state.thumb = nullptr; ++g_state.thumbVersion; }
                    }
                    if (thumbChanged && (!g_state.thumb || *g_state.thumb != *thumb)) {
                        g_state.thumb = thumb;
                        ++g_state.thumbVersion;
                    }
                    if (posMs != lastReportedPos || newTrack || !playing) {
                        if (ageMs < 0 || ageMs > 60000 || !playing) ageMs = 0;
                        g_state.positionMs = posMs;
                        g_state.sampledAtTick = now - (ULONGLONG)ageMs;
                        lastReportedPos = posMs;
                    }
                }
                if (newTrack) {
                    lastKey = key;
                    if (!title.empty()) {
                        LyricsQuery q;
                        q.title = title;
                        q.artist = artist;
                        q.album = album;
                        q.durationSec = durMs > 0 ? (int)((durMs + 500) / 1000) : 0;
                        q.sourceApp = app;
                        RequestLyrics(q);
                    }
                }
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"GSMTC error 0x%08X", (unsigned)e.code());
            mgr = nullptr;  // re-request after sleep/resume or service restart
        } catch (...) {
            Wh_Log(L"[media] unknown exception, re-requesting GSMTC");
            mgr = nullptr;
        }
        HANDLE waits[2] = {g_stopEvent, g_cmdEvent};
        if (WaitForMultipleObjects(2, waits, FALSE, 500) == WAIT_OBJECT_0) break;
    }
    mgr = nullptr;
    winrt::uninit_apartment();
}

// ─────────────────────────────────────────────────────────────────────────────
// AI agent status. Each local coding agent (Claude Code, Codex, opencode, ...) writes
// %LOCALAPPDATA%\ZenDesktop\ai-status\<agent>.json through the adapters in the
// ZenDesktop repo (tools/ai-status). Schema (all optional except state):
//   {"agent":"claude-code","name":"Claude Code","state":"working|tool|waiting|done|error|idle",
//    "message":"...","detail":"...","project":"...","updated":<unix seconds>}
// Optional fallback: configured process names show as "running" while the process exists.
// Read on its own thread once a second; the UI thread only copies the result.
// ─────────────────────────────────────────────────────────────────────────────
enum class AiState { Waiting = 0, Error, Working, Tool, Running, Done };
struct AiEntry {
    std::wstring key, name, message, detail, project;
    AiState state = AiState::Working;
    long long updated = 0;
};
static std::mutex g_aiLock;
static std::shared_ptr<const std::vector<AiEntry>> g_ai;

static long long UnixNow() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULONGLONG t = ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (long long)(t / 10000000ULL) - 11644473600LL;
}
static long long FileTimeToUnix(const FILETIME& ft) {
    ULONGLONG t = ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (long long)(t / 10000000ULL) - 11644473600LL;
}

static std::wstring AiStatusDir() {
    std::wstring dir;
    PWSTR base = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &base))) dir = std::wstring(base) + L"\\ZenDesktop\\ai-status";
    CoTaskMemFree(base);
    return dir;
}

static std::wstring PrettyAgentName(const std::wstring& key) {
    std::wstring k;
    for (wchar_t c : key) k += (wchar_t)towlower(c);
    if (k.find(L"claude") != std::wstring::npos) return L"Claude";
    if (k.find(L"codex") != std::wstring::npos) return L"Codex";
    if (k.find(L"opencode") != std::wstring::npos) return L"opencode";
    if (k.find(L"gemini") != std::wstring::npos) return L"Gemini";
    if (k.find(L"cursor") != std::wstring::npos) return L"Cursor";
    return key;
}

static std::vector<std::wstring> SplitList(const std::wstring& s) {
    std::vector<std::wstring> out;
    std::wstring cur;
    for (wchar_t c : s + L",") {
        if (c == L',' || c == L';' || c == L'\xFF0C' || c == L'\x3001' || c == L'\n') {
            while (!cur.empty() && iswspace(cur.back())) cur.pop_back();
            size_t a = 0;
            while (a < cur.size() && iswspace(cur[a])) ++a;
            if (a < cur.size()) out.push_back(cur.substr(a));
            cur.clear();
        } else {
            cur += c;
        }
    }
    return out;
}

static void ProcessNames(std::vector<std::wstring>& out) {
    out.clear();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do { out.push_back(pe.szExeFile); } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
}

// Status files are rewritten by other processes (temp file + replace); open with full sharing so we never
// block their replace, and treat any failure as "skip this file this round".
static bool ReadSmallFileShared(const std::wstring& path, std::string& out) {
    out.clear();
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    bool ok = GetFileSizeEx(f, &size) && size.QuadPart > 0 && size.QuadPart <= 64 * 1024;
    if (ok) {
        out.resize((size_t)size.QuadPart);
        DWORD read = 0;
        ok = ReadFile(f, out.data(), (DWORD)out.size(), &read, nullptr) && read == out.size();
    }
    CloseHandle(f);
    if (!ok) out.clear();
    return ok;
}

static void AiThread() {
    std::wstring dir = AiStatusDir();
    std::vector<std::wstring> procs;
    ULONGLONG lastProcScan = 0;
    std::shared_ptr<const std::vector<AiEntry>> last;
    while (!g_stop) {
        Settings s = GetSettings();
        auto list = std::make_shared<std::vector<AiEntry>>();
        if (s.aiMode != AiMode::Off && !dir.empty()) {
            long long now = UnixNow();
            WIN32_FIND_DATAW fd;
            HANDLE h = FindFirstFileW((dir + L"\\*.json").c_str(), &fd);
            if (h != INVALID_HANDLE_VALUE) {
                do {
                    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                    if (fd.nFileSizeHigh || fd.nFileSizeLow == 0 || fd.nFileSizeLow > 64 * 1024) continue;
                    std::string body;
                    if (!ReadSmallFileShared(dir + L"\\" + fd.cFileName, body)) continue;
                    if (body.size() >= 3 && (unsigned char)body[0] == 0xEF) body.erase(0, 3);  // UTF-8 BOM (PowerShell 5)
                    JValue j;
                    if (!ParseJson(body, j) || j.type != JValue::Object) continue;
                    AiEntry e;
                    std::wstring file = fd.cFileName;
                    e.key = FromUtf8(j["agent"].str());
                    if (e.key.empty()) e.key = file.substr(0, file.size() - 5);
                    e.name = FromUtf8(j["name"].str());
                    if (e.name.empty()) e.name = PrettyAgentName(e.key);
                    e.message = FromUtf8(j["message"].str());
                    e.detail = FromUtf8(j["detail"].str());
                    e.project = FromUtf8(j["project"].str());
                    e.updated = j["updated"].num();
                    if (e.updated <= 0) e.updated = FileTimeToUnix(fd.ftLastWriteTime);
                    std::string st = j["state"].str();
                    long long age = now - e.updated, maxAge;
                    if (st == "waiting") { e.state = AiState::Waiting; maxAge = 3600; }
                    else if (st == "error") { e.state = AiState::Error; maxAge = 600; }
                    else if (st == "tool") { e.state = AiState::Tool; maxAge = 1800; }
                    else if (st == "working" || st == "thinking" || st == "running") { e.state = AiState::Working; maxAge = 1800; }
                    else if (st == "done") { e.state = AiState::Done; maxAge = s.aiDoneSeconds > 0 ? s.aiDoneSeconds : LLONG_MAX / 4; }
                    else continue;  // idle / unknown: not shown
                    if (age > maxAge) continue;  // stale: agent probably exited without a final hook
                    list->push_back(std::move(e));
                } while (FindNextFileW(h, &fd));
                FindClose(h);
            }
            // Process-presence fallback for agents without a hook.
            std::vector<std::wstring> wanted = SplitList(s.aiProcesses);
            if (!wanted.empty()) {
                ULONGLONG tick = GetTickCount64();
                if (tick - lastProcScan >= 3000 || lastProcScan == 0) { ProcessNames(procs); lastProcScan = tick; }
                for (auto& w : wanted) {
                    std::wstring exe = w;
                    if (exe.size() < 4 || _wcsicmp(exe.c_str() + exe.size() - 4, L".exe") != 0) exe += L".exe";
                    bool running = std::any_of(procs.begin(), procs.end(),
                                               [&](const std::wstring& p) { return _wcsicmp(p.c_str(), exe.c_str()) == 0; });
                    if (!running) continue;
                    std::wstring bare = exe.substr(0, exe.size() - 4);
                    std::wstring pretty = PrettyAgentName(bare);
                    bool covered = std::any_of(list->begin(), list->end(),
                                               [&](const AiEntry& e) { return _wcsicmp(e.name.c_str(), pretty.c_str()) == 0; });
                    if (covered) continue;  // a hook already reports this agent
                    AiEntry e;
                    e.key = bare;
                    e.name = pretty;
                    e.state = AiState::Running;
                    e.updated = now;
                    list->push_back(std::move(e));
                }
            }
            std::stable_sort(list->begin(), list->end(), [](const AiEntry& a, const AiEntry& b) {
                if (a.state != b.state) return (int)a.state < (int)b.state;
                return a.updated > b.updated;
            });
        }
        {
            std::lock_guard<std::mutex> lk(g_aiLock);
            g_ai = list;
        }
        if (SleepOrStop(1000)) break;
    }
}

static std::shared_ptr<const std::vector<AiEntry>> GetAi() {
    std::lock_guard<std::mutex> lk(g_aiLock);
    return g_ai;
}

static const wchar_t* AiStateLabel(AiState s) {
    switch (s) {
        case AiState::Waiting: return L"等你回复";
        case AiState::Error: return L"出错了";
        case AiState::Working: return L"工作中";
        case AiState::Tool: return L"执行中";
        case AiState::Running: return L"运行中";
        case AiState::Done: return L"已完成";
    }
    return L"";
}
static Gdiplus::ARGB AiStateColor(AiState s) {
    switch (s) {
        case AiState::Waiting: return 0xFFFFB020;  // amber: needs you
        case AiState::Error: return 0xFFFF4D4F;
        case AiState::Working:
        case AiState::Tool: return 0xFF2EA8FF;
        case AiState::Running: return 0xFF8C8C8C;
        case AiState::Done: return 0xFF34C759;
    }
    return 0xFF8C8C8C;
}

// ─────────────────────────────────────────────────────────────────────────────
// Display (0.4.0): one layered CHILD window per taskbar (primary Shell_TrayWnd, optional
// Shell_SecondaryTrayWnd), created on the taskbar's own UI thread via RunFromWindowThread
// (the WH_CALLWNDPROC + registered-message helper used by taskbar-tray-show-on-hover /
// taskbar-multirow in windhawk-mods). Up to 0.3.3 the panel was a topmost popup laid over
// the taskbar (Taskbar Music Lounge style); the taskbar re-raises itself whenever it
// re-lays out, so the popup lost the z-order race and blinked. As a child placed HWND_TOP
// among the taskbar's children (above the XAML island's DesktopWindowContentBridge, the
// same embedding TrafficMonitor uses on Win11) the panel moves, auto-hides and hides with
// the taskbar and can't be covered by it. DWM acrylic doesn't apply to child windows, so
// the panel is per-pixel alpha (WS_EX_LAYERED child + UpdateLayeredWindow, Win8+): with
// "no background" only text / art / buttons are drawn over the taskbar's own material.
// Window procedures for the panels run on the taskbar thread; our own UI thread only
// scans for taskbars (2 s timer, TaskbarCreated, settings) and marshals create / destroy.
// Media session and AI status are shared; fullscreen hide is per-monitor when possible.
// ─────────────────────────────────────────────────────────────────────────────
static const wchar_t kWndClass[] = L"ZenTaskbarLyricsWnd";
static const wchar_t kHelperClass[] = L"ZenTaskbarLyricsHelper";
static std::atomic<DWORD> g_uiThreadId{0};
static constexpr UINT WM_ZEN_SETTINGS = WM_APP + 1;  // thread message to our UI thread
static constexpr UINT WM_ZEN_SYNC = WM_APP + 2;      // thread message to our UI thread
static constexpr UINT WM_ZEN_REFRESH = WM_APP + 3;   // posted to a panel: wParam 1 = force re-render
static constexpr UINT_PTR TIMER_TICK = 1;
static constexpr UINT_PTR TIMER_SYNC = 2;

// Same helper as ramensoftware/windhawk-mods taskbar-tray-show-on-hover.wh.cpp.
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);
static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_PARAM { RunFromWindowThreadProc_t proc; void* procParam; };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runMsg) {
                    RUN_PARAM* p = (RUN_PARAM*)cwp->lParam;
                    p->proc(p->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, tid);
    if (!hook) return false;
    RUN_PARAM param{proc, procParam};
    SendMessageW(hWnd, runMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

static const wchar_t* const kGameProcs[] = {
    L"League of Legends.exe", L"VALORANT-Win64-Shipping.exe", L"YuanShen.exe", L"GenshinImpact.exe",
    L"StarRail.exe", L"ZenlessZoneZero.exe", L"cs2.exe", L"dota2.exe", L"r5apex.exe",
    L"PUBG.exe", L"TslGame.exe", L"NarakaBladepoint.exe", L"DeltaForceClient-Win64-Shipping.exe",
};

static bool ForegroundIsGame() {
    HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    if (!fg || !GetWindowThreadProcessId(fg, &pid) || pid == GetCurrentProcessId()) return false;
    std::wstring name = ExeNameOfPid(pid);
    for (auto g : kGameProcs) if (_wcsicmp(name.c_str(), g) == 0) return true;
    return false;
}

// Hide this panel only when fullscreen / game is on the same monitor (or global presentation).
// 0.3.0 treated any captionless window covering 90% of the monitor as fullscreen. That
// matched the desktop (Progman/WorkerW, which is foreground after clicking the desktop)
// and every maximized frameless app (Windhawk, NetEase Cloud Music, WeChat, VS Code), so
// the panel was hidden almost all the time. Now: the shell's own windows never count, and
// the window must cover the whole monitor rect (a maximized window stops at the work area).
static bool IsShellWindow(HWND h) {
    wchar_t cls[64]{};
    if (!h || !GetClassNameW(h, cls, 64)) return false;
    return _wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
           _wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
           _wcsicmp(cls, kWndClass) == 0;
}

static bool CoversWholeMonitor(HWND fg, HMONITOR mon) {
    MONITORINFO mi{sizeof(mi)};
    RECT wr{};
    if (!GetMonitorInfoW(mon, &mi) || !GetWindowRect(fg, &wr)) return false;
    if (IsZoomed(fg) && (GetWindowLongW(fg, GWL_STYLE) & WS_CAPTION)) return false;
    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top && wr.right >= mi.rcMonitor.right &&
           wr.bottom >= mi.rcMonitor.bottom;
}

static bool ShouldHideForFullscreen(HMONITOR mon, bool hideSetting, const wchar_t** why) {
    if (!hideSetting) return false;
    QUERY_USER_NOTIFICATION_STATE st = QUNS_NOT_PRESENT;
    bool haveSt = SUCCEEDED(SHQueryUserNotificationState(&st));
    if (haveSt && st == QUNS_PRESENTATION_MODE) { *why = L"fullscreen: presentation mode"; return true; }

    HWND fg = GetForegroundWindow();
    if (!fg || IsShellWindow(fg) || !IsWindowVisible(fg)) return false;
    DWORD cloaked = 0;  // a starting app's window can be foreground while still cloaked
    if (SUCCEEDED(DwmGetWindowAttribute(fg, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    if (pid == GetCurrentProcessId()) return false;  // desktop, taskbar, Explorer windows
    HMONITOR fgMon = MonitorFromWindow(fg, MONITOR_DEFAULTTONULL);
    if (!fgMon || fgMon != mon) return false;

    if (ForegroundIsGame()) { *why = L"fullscreen: known game in foreground"; return true; }
    bool covers = CoversWholeMonitor(fg, mon);
    if (haveSt && st == QUNS_RUNNING_D3D_FULL_SCREEN) { *why = L"fullscreen: D3D exclusive"; return true; }
    if (covers) { *why = L"fullscreen: foreground window covers the whole monitor"; return true; }
    return false;
}

// Windhawk injects into every explorer.exe, including "separate process" folder windows.
// Only the process that owns the taskbar shows the panel, otherwise there would be two.
// Enumerate instead of FindWindow: status-bar tools (YASB etc.) also register Shell_TrayWnd
// (windhawk-mods issue #1704).
static bool OwnedByThisProcess(HWND h) {
    DWORD pid = 0;
    return h && GetWindowThreadProcessId(h, &pid) && pid == GetCurrentProcessId();
}

static bool IsSecondaryTaskbar(HWND h) {
    wchar_t cls[64]{};
    return h && GetClassNameW(h, cls, 64) && _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

static std::vector<HWND> FindTaskbars(bool multi) {
    struct Ctx { std::vector<HWND> primary, secondary; } ctx;
    EnumWindows(
        [](HWND h, LPARAM lp) -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lp);
            wchar_t cls[64]{};
            if (!GetClassNameW(h, cls, 64) || !OwnedByThisProcess(h)) return TRUE;
            if (_wcsicmp(cls, L"Shell_TrayWnd") == 0) c->primary.push_back(h);
            else if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) c->secondary.push_back(h);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
    std::vector<HWND> out;
    if (!ctx.primary.empty()) out.push_back(ctx.primary[0]);
    if (multi) out.insert(out.end(), ctx.secondary.begin(), ctx.secondary.end());
    return out;
}
static DWORD ReadDword(const wchar_t* subkey, const wchar_t* value, DWORD def) {
    DWORD v = 0, cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, subkey, value, RRF_RT_REG_DWORD, nullptr, &v, &cb) == ERROR_SUCCESS) return v;
    return def;
}

struct Env {
    bool lightTaskbar = false;
    bool alignLeft = false;
    ULONGLONG regAt = 0;
};
static Env g_env;

static void RefreshEnv(bool force) {
    ULONGLONG now = GetTickCount64();
    if (force || now - g_env.regAt >= 2000) {
        g_env.lightTaskbar = ReadDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                                       L"SystemUsesLightTheme", 0) != 0;
        g_env.alignLeft = ReadDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                                    L"TaskbarAl", 1) == 0;
        g_env.regAt = now;
    }
}

static Gdiplus::ARGB ResolveTextColor(const std::wstring& mode, bool light) {
    if (mode == L"white") return 0xFFFFFFFF;
    if (mode == L"dark") return 0xFF000000;
    if (mode == L"red") return 0xFFFF3333;
    if (mode == L"green") return 0xFF33FF33;
    if (mode == L"blue") return 0xFF3399FF;
    if (mode == L"yellow") return 0xFFFFCC00;
    if (mode == L"orange") return 0xFFFF8800;
    if (mode == L"purple") return 0xFFCC66FF;
    if (mode == L"pink") return 0xFFFF66B2;
    if (mode == L"cyan") return 0xFF33FFFF;
    return light ? 0xFF1A1A1A : 0xFFFFFFFF;
}
static Gdiplus::ARGB WithAlpha(Gdiplus::ARGB c, BYTE a) { return (c & 0x00FFFFFF) | ((Gdiplus::ARGB)a << 24); }
static bool IsLightColor(Gdiplus::ARGB c) {
    int r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
    return (r * 299 + g * 587 + b * 114) / 1000 > 150;
}

struct Seg { std::wstring text; Gdiplus::ARGB color = 0; bool dot = false; };
using Line = std::vector<Seg>;
static Line TextLine(const std::wstring& t) { return t.empty() ? Line{} : Line{Seg{t, 0, false}}; }
static std::wstring LineKey(const Line& l) {
    std::wstring k;
    for (auto& s : l) { k += s.text; k += L'\x1E'; k += std::to_wstring(s.color); k += s.dot ? L'*' : L'-'; }
    return k;
}

struct Content {
    bool media = false;
    bool playing = false;
    bool canPrev = false, canNext = false, canPlayPause = false;
    bool aiOnly = false;
    bool idle = false;
    Line line1, line2;
    std::shared_ptr<const std::vector<BYTE>> thumb;
    unsigned thumbVersion = 0;
};

static Line AiLine(const std::vector<AiEntry>& ai, const Settings& s, bool forFirstLine, Line* second) {
    Line l;
    if (ai.empty()) return l;
    if (s.aiMode == AiMode::Rotate || ai.size() == 1) {
        const AiEntry& e = ai[(size_t)((GetTickCount64() / 4000) % ai.size())];
        l.push_back({L"\x25CF ", AiStateColor(e.state), true});
        std::wstring t = e.name + L" " + AiStateLabel(e.state);
        if (e.state == AiState::Tool && !e.detail.empty()) t += L"\xFF1A" + e.detail;
        if (ai.size() > 1) t += L"  (" + std::to_wstring(ai.size()) + L")";
        std::wstring msg = !e.message.empty() ? e.message : e.project;
        if (forFirstLine && second) {
            l.push_back({t, 0, false});
            if (!msg.empty()) *second = TextLine(msg);
        } else {
            if (!msg.empty()) t += L" \x00B7 " + msg;
            l.push_back({t, 0, false});
        }
        return l;
    }
    for (size_t i = 0; i < ai.size(); ++i) {
        l.push_back({(i ? L"  \x25CF " : L"\x25CF "), AiStateColor(ai[i].state), true});
        l.push_back({ai[i].name + L" " + AiStateLabel(ai[i].state), 0, false});
    }
    if (forFirstLine && second) {
        const AiEntry& top = ai[0];
        std::wstring msg = !top.message.empty() ? top.message : top.project;
        if (!msg.empty()) *second = TextLine(top.name + L"\xFF1A" + msg);
    }
    return l;
}

static Content BuildContent(const Settings& s) {
    Content c;
    auto aiPtr = s.aiMode != AiMode::Off ? GetAi() : nullptr;
    static const std::vector<AiEntry> kNoAi;
    const std::vector<AiEntry>& ai = aiPtr ? *aiPtr : kNoAi;
    std::wstring lyric, translation;
    bool haveLyric = false;
    {
        std::lock_guard<std::mutex> lk(g_stateLock);
        if (g_state.hasSession) {
            c.media = true;
            c.playing = g_state.playing;
            c.canPrev = g_state.canPrev;
            c.canNext = g_state.canNext;
            c.canPlayPause = g_state.canPlayPause;
            c.thumb = g_state.thumb;
            c.thumbVersion = g_state.thumbVersion;
            std::wstring info = g_state.artist.empty() ? g_state.title : g_state.title + L" - " + g_state.artist;
            if (!s.titleOnly && g_state.lyrics) {
                long long pos = g_state.positionMs;
                if (g_state.playing) pos += (long long)((double)(GetTickCount64() - g_state.sampledAtTick) * g_state.rate);
                pos += s.offsetMs;
                pos = std::clamp<long long>(pos, INT_MIN / 2, INT_MAX / 2);
                if (const LyricLine* l = FindLine(g_state.lyrics->lines, (int)pos)) {
                    haveLyric = true;
                    lyric = l->text.empty() ? std::wstring(L"\x266A") : l->text;
                    if (!l->text.empty())
                        if (const LyricLine* t = FindTranslation(g_state.lyrics->translation, l->timeMs)) translation = t->text;
                }
            }
            if (haveLyric) {
                c.line1 = TextLine(lyric);
                if (s.secondLine == SecondLine::Auto) c.line2 = TextLine(!translation.empty() ? translation : info);
                else if (s.secondLine == SecondLine::Translation) c.line2 = TextLine(translation);
                else if (s.secondLine == SecondLine::Info) c.line2 = TextLine(info);
            } else {
                c.line1 = TextLine(g_state.title);
                if (s.secondLine == SecondLine::Auto || s.secondLine == SecondLine::Info)
                    c.line2 = TextLine(g_state.artist);
                if (c.line1.empty()) c.line1 = TextLine(info);
            }
        }
    }
    if (c.media) {
        if (!ai.empty()) c.line2 = AiLine(ai, s, false, nullptr);
    } else if (!ai.empty()) {
        c.aiOnly = true;
        c.line1 = AiLine(ai, s, true, &c.line2);
    } else if (s.showIdle) {
        c.idle = true;
        c.line1 = TextLine(L"\x266A \x672A\x5728\x64AD\x653E");
    }
    return c;
}

static void RoundRectPath(Gdiplus::GraphicsPath& p, Gdiplus::REAL x, Gdiplus::REAL y, Gdiplus::REAL w, Gdiplus::REAL h, Gdiplus::REAL r) {
    r = std::min(r, std::min(w, h) / 2);
    Gdiplus::REAL d = r * 2;
    p.Reset();
    if (d <= 0.5f) { p.AddRectangle(Gdiplus::RectF(x, y, w, h)); return; }
    p.AddArc(x, y, d, d, 180, 90);
    p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}
static void FillRound(Gdiplus::Graphics& g, Gdiplus::ARGB c, const Gdiplus::RectF& r, Gdiplus::REAL rad) {
    Gdiplus::GraphicsPath p;
    RoundRectPath(p, r.X, r.Y, r.Width, r.Height, rad);
    Gdiplus::SolidBrush b{Gdiplus::Color(c)};
    g.FillPath(&b, &p);
}

enum Hit { HitNone = -1, HitPrev = 0, HitPlay = 1, HitNext = 2, HitBody = 3 };

static void DrawIcon(Gdiplus::Graphics& g, int which, bool playing, Gdiplus::REAL cx, Gdiplus::REAL cy, Gdiplus::REAL s, Gdiplus::ARGB color) {
    Gdiplus::SolidBrush b{Gdiplus::Color(color)};
    if (which == HitPlay) {
        if (playing) {
            Gdiplus::REAL bw = s * 0.26f, gap = s * 0.22f;
            FillRound(g, color, Gdiplus::RectF(cx - gap / 2 - bw, cy - s / 2, bw, s), bw * 0.3f);
            FillRound(g, color, Gdiplus::RectF(cx + gap / 2, cy - s / 2, bw, s), bw * 0.3f);
        } else {
            Gdiplus::PointF tri[3] = {{cx - s * 0.36f, cy - s * 0.52f}, {cx - s * 0.36f, cy + s * 0.52f}, {cx + s * 0.52f, cy}};
            g.FillPolygon(&b, tri, 3);
        }
        return;
    }
    Gdiplus::REAL dir = which == HitPrev ? -1.0f : 1.0f;
    Gdiplus::REAL bar = s * 0.15f;
    Gdiplus::PointF tri[3] = {{cx - dir * s * 0.40f, cy - s * 0.42f}, {cx - dir * s * 0.40f, cy + s * 0.42f}, {cx + dir * s * 0.28f, cy}};
    g.FillPolygon(&b, tri, 3);
    Gdiplus::REAL bx = which == HitPrev ? cx - s * 0.46f : cx + s * 0.31f;
    FillRound(g, color, Gdiplus::RectF(bx, cy - s * 0.42f, bar, s * 0.84f), bar * 0.4f);
}

struct UiState {
    int hover = HitNone, pressed = HitNone;
    bool tracking = false;
    RECT hitRects[4]{};
    bool hitEnabled[4]{};
    std::wstring lastSig;
    bool visible = false;
    std::unique_ptr<Gdiplus::Bitmap> art;
    unsigned artVersion = ~0u;
    int artPx = 0;
    int posX = INT_MIN, posY = INT_MIN;  // last position, client coordinates of the taskbar
    ULONGLONG hideSince = 0;             // first tick a soft hide reason was seen (debounce)
    unsigned zFixes = 0;                 // times a taskbar child was found above the panel
};

struct Panel {
    HWND hwnd = nullptr;   // child of tray, owned by the taskbar thread
    HWND tray = nullptr;
    bool secondary = false;
    bool subclassed = false;
    UiState ui;
    std::wstring lastWhy;  // last hide reason / "shown" key, so the log only records changes
};

// Panels are touched from the taskbar thread (window procedures, create / destroy) and,
// read-only plus erasing dead entries, from our UI thread. Never hold this lock while
// marshalling to another thread.
static std::recursive_mutex g_panelsLock;
static std::vector<std::unique_ptr<Panel>> g_panels;
static HINSTANCE g_uiInst = nullptr;

static Panel* PanelOf(HWND hwnd) {
    for (auto& p : g_panels) if (p->hwnd == hwnd) return p.get();
    return nullptr;
}
static Panel* PanelOfTray(HWND tray) {
    for (auto& p : g_panels) if (p->tray == tray) return p.get();
    return nullptr;
}
static std::unique_ptr<Gdiplus::Bitmap> DecodeArt(const std::vector<BYTE>& bytes, int px, Gdiplus::REAL radius) {
    IStream* stm = SHCreateMemStream(bytes.data(), (UINT)bytes.size());
    if (!stm) return nullptr;
    std::unique_ptr<Gdiplus::Bitmap> out;
    {
        Gdiplus::Bitmap src(stm);
        if (src.GetLastStatus() == Gdiplus::Ok && src.GetWidth() > 0 && src.GetHeight() > 0) {
            UINT sw = src.GetWidth(), sh = src.GetHeight(), side = std::min(sw, sh);
            Gdiplus::Bitmap scaled(px, px, PixelFormat32bppPARGB);
            {
                Gdiplus::Graphics g(&scaled);
                g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
                g.DrawImage(&src, Gdiplus::RectF(0, 0, (Gdiplus::REAL)px, (Gdiplus::REAL)px),
                            (Gdiplus::REAL)((sw - side) / 2), (Gdiplus::REAL)((sh - side) / 2),
                            (Gdiplus::REAL)side, (Gdiplus::REAL)side, Gdiplus::UnitPixel);
            }
            out = std::make_unique<Gdiplus::Bitmap>(px, px, PixelFormat32bppPARGB);
            Gdiplus::Graphics g(out.get());
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            g.Clear(Gdiplus::Color(0, 0, 0, 0));
            Gdiplus::TextureBrush tb(&scaled);
            Gdiplus::GraphicsPath p;
            RoundRectPath(p, 0, 0, (Gdiplus::REAL)px, (Gdiplus::REAL)px, radius);
            g.FillPath(&tb, &p);
        }
    }
    stm->Release();
    return out;
}

// shadow = 0: none. Only used when the text has little contrast with the taskbar.
static void DrawLine(Gdiplus::Graphics& g, const Line& line, const Gdiplus::Font& font, const Gdiplus::RectF& r,
                     Gdiplus::ARGB textColor, Gdiplus::ARGB shadow, Gdiplus::REAL shadowOff) {
    Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
    fmt.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    Gdiplus::REAL x = r.X;
    BYTE ta = (BYTE)((textColor >> 24) & 255);
    Gdiplus::SolidBrush sh{Gdiplus::Color(WithAlpha(shadow, (BYTE)(((shadow >> 24) & 255) * ta / 255)))};
    for (size_t i = 0; i < line.size(); ++i) {
        const Seg& sg = line[i];
        Gdiplus::REAL avail = r.X + r.Width - x;
        if (avail < 4) break;
        Gdiplus::RectF box;
        g.MeasureString(sg.text.c_str(), -1, &font, Gdiplus::RectF(0, 0, 10000, r.Height), &fmt, &box);
        Gdiplus::RectF dst(x, r.Y, std::min(box.Width + 1, avail), r.Height);
        Gdiplus::ARGB col = sg.color ? sg.color : textColor;
        if (shadow && !sg.dot) {
            Gdiplus::RectF ds(dst.X, dst.Y + shadowOff, dst.Width, dst.Height);
            g.DrawString(sg.text.c_str(), -1, &font, ds, &fmt, &sh);
        }
        Gdiplus::SolidBrush b{Gdiplus::Color(col)};
        g.DrawString(sg.text.c_str(), -1, &font, dst, &fmt, &b);
        x += box.Width;
        if (box.Width + 1 > avail) break;
    }
}

// What is drawn under the content. "none" draws nothing at all: no tint, no blur, no
// border, so the panel is just text / art / buttons on the taskbar's own material.
// With panel glass on, subtle / card are translucent layers of the taskbar's own glass;
// with it off they are the older, nearly opaque painted backgrounds.
struct PanelLook { Gdiplus::ARGB fill = 0, edge = 0; };
static PanelLook LookFor(PanelBg bg, bool glass, bool light) {
    if (bg == PanelBg::None) return {};
    if (glass) {
        if (bg == PanelBg::Subtle) return light ? PanelLook{0x40FFFFFF, 0x12000000} : PanelLook{0x14FFFFFF, 0x14FFFFFF};
        return light ? PanelLook{0x99FFFFFF, 0x1E000000} : PanelLook{0x66141414, 0x22FFFFFF};
    }
    if (bg == PanelBg::Subtle) return light ? PanelLook{0x8CFFFFFF, 0x1C000000} : PanelLook{0x60000000, 0x1CFFFFFF};
    return light ? PanelLook{0xEEF9F9F9, 0x38000000} : PanelLook{0xE8262626, 0x38FFFFFF};
}

struct Layout {
    int cx = 0, cy = 0, w = 0, h = 0;  // client coordinates inside the taskbar
    int dpi = 96;
    int pad = 0, art = 0, btn = 0, cur = 0, textRight = 0;
    RECT artRc{};
    RECT btnRc[3]{};
    bool showArt = false, showButtons = false, bodyClickable = false;
    Gdiplus::ARGB text = 0xFFFFFFFF;
    Gdiplus::ARGB shadow = 0;
    bool light = false;
    Content content;
};

static bool ComputeLayout(Panel& panel, const Settings& s, Layout& L, bool forceEnv, const wchar_t** why) {
    RefreshEnv(forceEnv);
    HWND tray = panel.tray;
    if (!tray || !IsWindow(tray)) { *why = L"taskbar window gone"; return false; }
    RECT tr{}, cr{};
    if (!GetWindowRect(tray, &tr) || !GetClientRect(tray, &cr)) { *why = L"GetWindowRect(taskbar) failed"; return false; }
    if (!IsWindowVisible(tray)) { *why = L"taskbar not visible"; return false; }
    // Auto-hide needs no check any more: the child slides out with the taskbar.

    L.content = BuildContent(s);
    bool nothing = !L.content.media && !L.content.aiOnly && !L.content.idle;
    if (nothing) {
        *why = L"nothing to show: no media session and no AI status (whenIdle=hide). Play something in a player that supports system media controls";
        return false;
    }

    HMONITOR mon = MonitorFromWindow(tray, MONITOR_DEFAULTTONEAREST);
    if (ShouldHideForFullscreen(mon, s.hideInFullscreen, why)) return false;

    UINT dpi = GetDpiForWindow(tray);
    if (!dpi) dpi = 96;
    L.dpi = (int)dpi;
    auto D = [dpi](double v) { return (int)(v * dpi / 96.0 + 0.5); };
    int cw = cr.right - cr.left;
    L.w = D(s.width);
    L.h = cr.bottom - cr.top;
    if (L.h <= 0 || L.h > 1000 || L.w <= 0 || cw <= 0) { *why = L"bad taskbar height or panel width"; return false; }
    L.w = std::min(L.w, cw);
    int gap = MulDiv(s.xOffset, (int)dpi, 96);
    bool trayMode = s.posMode == PosMode::Tray || (s.posMode == PosMode::Widgets && g_env.alignLeft);
    POINT origin{0, 0};
    ClientToScreen(tray, &origin);  // tray notify area rect is in screen coordinates
    int x = gap;
    if (trayMode) {
        HWND notify = FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr);
        RECT nr{};
        if (notify && GetWindowRect(notify, &nr) && nr.right - nr.left > D(40) && nr.left > tr.left && nr.right <= tr.right)
            x = (nr.left - origin.x) - L.w - gap;
        else
            x = cw - L.w - gap - D(260);
    }
    L.cx = std::clamp(x, 0, std::max(0, cw - L.w));
    L.cy = 0;

    L.light = g_env.lightTaskbar;
    L.text = ResolveTextColor(s.textColorMode, L.light);
    // Default colors follow light/dark mode and need no shadow (looks native). A soft
    // shadow / glow only when the chosen color is close to the taskbar's brightness.
    if (IsLightColor(L.text) == L.light) L.shadow = IsLightColor(L.text) ? 0x80000000 : 0x70FFFFFF;

    L.pad = D(6);
    L.showArt = s.showArt && (L.content.media || L.content.aiOnly);
    L.showButtons = s.showButtons && L.content.media;
    L.art = std::min(L.h - D(10), D(40));
    L.cur = L.pad;
    if (L.showArt && L.art > 8) {
        L.artRc = {L.cur, (L.h - L.art) / 2, L.cur + L.art, (L.h - L.art) / 2 + L.art};
        L.cur += L.art + D(8);
    }
    L.btn = std::min(D(32), L.h - D(8));
    L.textRight = L.w - L.pad;
    if (L.showButtons) {
        int right = L.w - D(4);
        for (int i = 2; i >= 0; --i) {
            L.btnRc[i] = {right - L.btn, (L.h - L.btn) / 2, right, (L.h - L.btn) / 2 + L.btn};
            right -= L.btn;
        }
        L.textRight = L.btnRc[0].left - D(4);
    }
    L.bodyClickable = L.content.media && s.clickOpens;
    for (int i = 0; i < 3; ++i) panel.ui.hitRects[i] = L.btnRc[i];
    panel.ui.hitEnabled[HitPrev] = L.showButtons && L.content.canPrev;
    panel.ui.hitEnabled[HitPlay] = L.showButtons && L.content.canPlayPause;
    panel.ui.hitEnabled[HitNext] = L.showButtons && L.content.canNext;
    panel.ui.hitRects[HitBody] =
        L.bodyClickable ? RECT{D(2), D(2), L.showButtons ? L.btnRc[0].left : L.w - D(2), L.h - D(2)} : RECT{};
    panel.ui.hitEnabled[HitBody] = L.bodyClickable;
    if (panel.ui.hover != HitNone && !panel.ui.hitEnabled[panel.ui.hover]) panel.ui.hover = HitNone;

    if (L.showArt && L.content.thumb && (panel.ui.artVersion != L.content.thumbVersion || panel.ui.artPx != L.art)) {
        panel.ui.art = DecodeArt(*L.content.thumb, L.art, (Gdiplus::REAL)D(6));
        panel.ui.artVersion = L.content.thumbVersion;
        panel.ui.artPx = L.art;
    } else if (!L.content.thumb && panel.ui.art) {
        panel.ui.art.reset();
        panel.ui.artVersion = L.content.thumbVersion;
    }
    return true;
}

static Gdiplus::RectF ToRectF(const RECT& r) {
    return Gdiplus::RectF((Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top, (Gdiplus::REAL)(r.right - r.left),
                          (Gdiplus::REAL)(r.bottom - r.top));
}

static void PaintContent(Gdiplus::Graphics& g, Panel& panel, const Settings& s, const Layout& L) {
    auto D = [&](double v) { return (int)(v * L.dpi / 96.0 + 0.5); };
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);  // ClearType can't blend onto alpha
    g.Clear(Gdiplus::Color(0, 0, 0, 0));

    Gdiplus::REAL fw = (Gdiplus::REAL)L.w, fh = (Gdiplus::REAL)L.h;
    Gdiplus::RectF panelRc((Gdiplus::REAL)D(2), (Gdiplus::REAL)D(4), fw - D(4), fh - D(8));
    Gdiplus::REAL rad = (Gdiplus::REAL)D(8);

    PanelLook look = LookFor(s.panelBg, s.panelGlass, L.light);
    if (look.fill) FillRound(g, look.fill, panelRc, rad);
    if (look.edge) {
        Gdiplus::GraphicsPath p;
        RoundRectPath(p, panelRc.X, panelRc.Y, panelRc.Width, panelRc.Height, rad);
        Gdiplus::Pen pen(Gdiplus::Color(look.edge), 1.0f);
        g.DrawPath(&pen, &p);
    }

    // Alpha 1 (invisible) under clickable areas: fully transparent pixels of a layered
    // window are click-through, so without this the "none" preset could not be clicked.
    if (panel.ui.hitEnabled[HitBody]) FillRound(g, 0x01000000, ToRectF(panel.ui.hitRects[HitBody]), rad);
    for (int i = 0; i < 3; ++i)
        if (panel.ui.hitEnabled[i]) FillRound(g, 0x01000000, ToRectF(L.btnRc[i]), (Gdiplus::REAL)D(6));

    if (panel.ui.hover != HitNone) {
        bool bare = s.panelBg == PanelBg::None;  // only a faint highlight while hovered
        bool down = panel.ui.pressed == panel.ui.hover;
        BYTE a = bare ? (down ? 0x2A : 0x16) : (down ? 0x40 : 0x22);
        FillRound(g, WithAlpha(L.text, a), ToRectF(panel.ui.hitRects[panel.ui.hover]),
                  panel.ui.hover == HitBody ? rad : (Gdiplus::REAL)D(6));
    }

    if (L.showArt && L.artRc.right > L.artRc.left) {
        Gdiplus::RectF ar = ToRectF(L.artRc);
        if (panel.ui.art && L.content.media) {
            g.DrawImage(panel.ui.art.get(), ar.X, ar.Y, ar.Width, ar.Height);
        } else {
            FillRound(g, WithAlpha(L.text, 0x26), ar, (Gdiplus::REAL)D(6));
            Gdiplus::FontFamily sym(L"Segoe UI Symbol");
            const Gdiplus::FontFamily* sf = sym.IsAvailable() ? &sym : Gdiplus::FontFamily::GenericSansSerif();
            Gdiplus::Font f(sf, L.art * (L.content.aiOnly ? 0.34f : 0.5f), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::StringFormat cf;
            cf.SetAlignment(Gdiplus::StringAlignmentCenter);
            cf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            Gdiplus::SolidBrush b{Gdiplus::Color(WithAlpha(L.text, 0xC0))};
            g.DrawString(L.content.aiOnly ? L"AI" : L"\x266A", -1, &f, ar, &cf, &b);
        }
    }

    Gdiplus::FontFamily fam(s.font.c_str());
    const Gdiplus::FontFamily* useFam = fam.IsAvailable() ? &fam : Gdiplus::FontFamily::GenericSansSerif();
    int style = s.bold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular;
    bool two = !L.content.line2.empty();
    Gdiplus::REAL px1 = (Gdiplus::REAL)s.fontSize * (Gdiplus::REAL)L.dpi / 72.0f;
    if (two) px1 = std::min(px1, fh * 0.34f);
    else px1 = std::min(px1, fh * 0.5f);
    Gdiplus::REAL px2 = px1 * 0.82f;
    Gdiplus::Font font1(useFam, px1, style, Gdiplus::UnitPixel);
    Gdiplus::Font font2(useFam, px2, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::REAL tx = (Gdiplus::REAL)L.cur, tw = (Gdiplus::REAL)(L.textRight - L.cur);
    Gdiplus::REAL so = std::max<Gdiplus::REAL>(1.0f, (Gdiplus::REAL)L.dpi / 96.0f);
    if (tw > 8) {
        if (two) {
            Gdiplus::REAL h1 = px1 * 1.32f, h2 = px2 * 1.32f, top = (fh - h1 - h2) / 2;
            DrawLine(g, L.content.line1, font1, Gdiplus::RectF(tx, top, tw, h1), L.text, L.shadow, so);
            DrawLine(g, L.content.line2, font2, Gdiplus::RectF(tx, top + h1, tw, h2), WithAlpha(L.text, 0xB8), L.shadow, so);
        } else {
            DrawLine(g, L.content.line1, font1, Gdiplus::RectF(tx, 0, tw, fh), L.text, L.shadow, so);
        }
    }

    if (L.showButtons) {
        for (int i = 0; i < 3; ++i) {
            const RECT& r = L.btnRc[i];
            Gdiplus::REAL cx = (r.left + r.right) / 2.0f, cy = (r.top + r.bottom) / 2.0f;
            Gdiplus::REAL sz = (Gdiplus::REAL)D(i == HitPlay ? 13 : 11);
            DrawIcon(g, i, L.content.playing, cx, cy, sz, panel.ui.hitEnabled[i] ? L.text : WithAlpha(L.text, 0x55));
        }
    }
    g.Flush(Gdiplus::FlushIntentionSync);
}

// Screen position is left out on purpose: while the taskbar auto-hides the child slides
// with it and nothing needs redrawing.
static std::wstring MakeSig(const Layout& L, Panel& panel, const Settings& s) {
    return std::to_wstring(L.cx) + L"," + std::to_wstring(L.cy) + L"," + std::to_wstring(L.w) + L"," +
           std::to_wstring(L.h) + L"," + std::to_wstring(L.dpi) + L"," + std::to_wstring(L.text) + L"," +
           std::to_wstring(L.shadow) + L"," + (L.light ? L"L" : L"D") +
           std::to_wstring(panel.ui.hover) + std::to_wstring(panel.ui.pressed) + (L.content.playing ? L"P" : L"p") +
           (L.content.canPrev ? L"1" : L"0") + (L.content.canNext ? L"1" : L"0") +
           (L.content.canPlayPause ? L"1" : L"0") + std::to_wstring(L.content.thumbVersion) +
           (panel.ui.art ? L"A" : L"a") + (L.content.aiOnly ? L"I" : L"i") + (L.bodyClickable ? L"C" : L"c") +
           std::to_wstring((int)s.panelBg) + (s.panelGlass ? L"G" : L"g") + std::to_wstring(s.fontSize) + s.font +
           (s.bold ? L"B" : L"b") + L"\x1D" + LineKey(L.content.line1) + L"\x1D" + LineKey(L.content.line2);
}

// Keep the panel the top child of the taskbar (above the XAML island). Only acts when
// another child is above it; no hide / show, so nothing blinks.
static void EnsureTopChild(Panel& panel) {
    if (!panel.hwnd || !panel.tray) return;
    HWND top = GetWindow(panel.tray, GW_CHILD);
    if (!top || top == panel.hwnd) return;
    SetWindowPos(panel.hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    ++panel.ui.zFixes;
    if (panel.ui.zFixes <= 3 || panel.ui.zFixes % 100 == 0) {
        wchar_t cls[96]{};
        GetClassNameW(top, cls, 96);
        Wh_Log(L"[ui] panel %p raised back above taskbar child %p (%s), count %u", panel.hwnd, top, cls,
               panel.ui.zFixes);
    }
}

static void HidePanel(Panel& panel) {
    if (panel.hwnd && IsWindowVisible(panel.hwnd)) ShowWindow(panel.hwnd, SW_HIDE);
    panel.ui.visible = false;
    panel.ui.hideSince = 0;
    panel.ui.lastSig.clear();
    panel.ui.hover = panel.ui.pressed = HitNone;
}

// Hide reasons that must apply at once (the taskbar itself went away). Anything else
// (fullscreen check, nothing to show) has to persist before the panel hides, so a splash
// screen or a session switch while an app starts doesn't make it blink.
static bool IsImmediateHide(const wchar_t* why) {
    return wcsncmp(why, L"taskbar", 7) == 0 || wcsncmp(why, L"GetWindowRect", 13) == 0 || wcsncmp(why, L"bad", 3) == 0;
}

// Taskbar thread only (called from the panel's window procedure or creation).
static void RenderPanel(Panel& panel, bool force = false) {
    if (!panel.hwnd || !IsWindow(panel.hwnd)) return;
    Settings s = GetSettings();
    Layout L;
    const wchar_t* why = L"unknown";
    ULONGLONG now = GetTickCount64();
    if (!ComputeLayout(panel, s, L, force, &why)) {
        if (panel.ui.visible && !IsImmediateHide(why)) {
            if (!panel.ui.hideSince) panel.ui.hideSince = now;
            DWORD wait = wcsncmp(why, L"fullscreen", 10) == 0 ? 700 : 450;
            if (now - panel.ui.hideSince < wait) return;  // keep what is on screen until the reason persists
        }
        if (panel.lastWhy != why) {
            panel.lastWhy = why;
            Wh_Log(L"[ui] panel %p hidden: %s", panel.hwnd, why);
        }
        HidePanel(panel);
        return;
    }
    panel.ui.hideSince = 0;

    std::wstring sig = MakeSig(L, panel, s);
    if (!force && panel.ui.visible && sig == panel.ui.lastSig) {
        EnsureTopChild(panel);
        return;
    }

    // Per-pixel alpha frame, composed off-screen and handed to DWM in one call.
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = L.w;
    bi.bmiHeader.biHeight = -L.h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = mem ? CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    if (!bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        if (mem) DeleteDC(mem);
        ReleaseDC(nullptr, screen);
        Wh_Log(L"[ui] CreateDIBSection %dx%d failed", L.w, L.h);
        return;
    }
    HGDIOBJ old = SelectObject(mem, bmp);
    {
        Gdiplus::Bitmap canvas(L.w, L.h, L.w * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Gdiplus::Graphics g(&canvas);
        PaintContent(g, panel, s, L);
    }
    // pptDst stays null: for a child it would be ambiguous (screen vs parent client);
    // the position is set with SetWindowPos in parent client coordinates instead.
    POINT ptSrc{0, 0};
    SIZE sz{L.w, L.h};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(panel.hwnd, screen, nullptr, &sz, mem, &ptSrc, 0, &bf, ULW_ALPHA))
        Wh_Log(L"[ui] UpdateLayeredWindow failed %u", GetLastError());
    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);

    if (panel.ui.posX != L.cx || panel.ui.posY != L.cy) {
        SetWindowPos(panel.hwnd, nullptr, L.cx, L.cy, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        panel.ui.posX = L.cx;
        panel.ui.posY = L.cy;
    }
    panel.ui.lastSig = sig;
    if (!IsWindowVisible(panel.hwnd))
        SetWindowPos(panel.hwnd, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    panel.ui.visible = true;
    EnsureTopChild(panel);
    {
        wchar_t key[160];
        swprintf_s(key, L"shown at %d,%d %dx%d dpi=%d bg=%d glass=%d shadow=%d %s", L.cx, L.cy, L.w, L.h, L.dpi,
                   (int)s.panelBg, s.panelGlass ? 1 : 0, L.shadow ? 1 : 0,
                   L.content.media ? L"media" : L.content.aiOnly ? L"ai" : L"idle");
        if (panel.lastWhy != key) {
            panel.lastWhy = key;
            Wh_Log(L"[ui] panel %p %s visible=%d", panel.hwnd, key, IsWindowVisible(panel.hwnd) ? 1 : 0);
        }
    }
}

static int HitTestClient(Panel& panel, POINT pt) {
    for (int i = 0; i < 4; ++i)
        if (panel.ui.hitEnabled[i] && PtInRect(&panel.ui.hitRects[i], pt)) return i;
    return HitNone;
}

// Subclass on Shell_TrayWnd / Shell_SecondaryTrayWnd: whenever the taskbar moves, resizes,
// changes DPI / theme or gets a new child, ask the panel to re-check layout and z-order.
static LRESULT CALLBACK TraySubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    LRESULT r = DefSubclassProc(hwnd, msg, wp, lp);
    switch (msg) {
        case WM_WINDOWPOSCHANGED:
        case WM_SIZE:
        case WM_PARENTNOTIFY:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DISPLAYCHANGE:
        case WM_NCDESTROY: {
            std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
            if (Panel* p = PanelOfTray(hwnd)) {
                if (msg == WM_NCDESTROY) p->subclassed = false;  // the Windhawk wrapper removes it
                else if (p->hwnd && !g_stop) {
                    bool force = msg == WM_DPICHANGED || msg == WM_SETTINGCHANGE || msg == WM_THEMECHANGED ||
                                 msg == WM_DISPLAYCHANGE;
                    PostMessageW(p->hwnd, WM_ZEN_REFRESH, force ? 1 : 0, 0);
                }
            }
            break;
        }
    }
    return r;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
    Panel* panel = PanelOf(hwnd);
    switch (msg) {
        case WM_TIMER:
            if (wp == TIMER_TICK && panel && !g_stop) RenderPanel(*panel);
            return 0;
        case WM_ZEN_REFRESH:
            if (panel && !g_stop) {
                EnsureTopChild(*panel);
                RenderPanel(*panel, wp != 0);
            }
            return 0;
        case WM_PAINT: {  // layered (UpdateLayeredWindow) windows keep their own surface
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_DPICHANGED_AFTERPARENT:
        case WM_SETTINGCHANGE:
            if (panel) RenderPanel(*panel, true);
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE: {
            if (!panel) return 0;
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            if (!panel->ui.tracking) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                panel->ui.tracking = TrackMouseEvent(&tme) != FALSE;
            }
            int hit = HitTestClient(*panel, pt);
            if (hit != panel->ui.hover) {
                panel->ui.hover = hit;
                RenderPanel(*panel);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (!panel) return 0;
            panel->ui.tracking = false;
            if (panel->ui.hover != HitNone) {
                panel->ui.hover = HitNone;
                RenderPanel(*panel);
            }
            return 0;
        case WM_SETCURSOR:
            if (panel && LOWORD(lp) == HTCLIENT && panel->ui.hover != HitNone) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
            break;
        case WM_LBUTTONDOWN: {
            if (!panel) return 0;
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            panel->ui.pressed = HitTestClient(*panel, pt);
            if (panel->ui.pressed != HitNone) SetCapture(hwnd);
            RenderPanel(*panel);
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!panel) return 0;
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            int hit = HitTestClient(*panel, pt), was = panel->ui.pressed;
            panel->ui.pressed = HitNone;
            if (GetCapture() == hwnd) ReleaseCapture();
            if (hit != HitNone && hit == was) {
                if (hit == HitPrev) PostMediaCmd(MediaCmd::Prev);
                else if (hit == HitPlay) PostMediaCmd(MediaCmd::PlayPause);
                else if (hit == HitNext) PostMediaCmd(MediaCmd::Next);
                else if (hit == HitBody) PostMediaCmd(MediaCmd::OpenPlayer);
            }
            RenderPanel(*panel);
            return 0;
        }
        case WM_RBUTTONUP:
            return 0;  // don't let the taskbar's own menu pop up over the panel
        case WM_CAPTURECHANGED:
            if (panel && panel->ui.pressed != HitNone) {
                panel->ui.pressed = HitNone;
                RenderPanel(*panel);
            }
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_TICK);
            if (panel) {
                panel->hwnd = nullptr;
                panel->ui = UiState{};
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ── Runs on the taskbar thread (via RunFromWindowThread) ─────────────────────
struct CreateParam { HWND tray; bool ok; };
static void WINAPI CreatePanelOnTrayThread(void* pv) {
    auto* cp = (CreateParam*)pv;
    std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
    if (g_stop || !IsWindow(cp->tray)) return;
    if (Panel* existing = PanelOfTray(cp->tray)) {
        cp->ok = existing->hwnd != nullptr;
        return;
    }
    HWND topBefore = GetWindow(cp->tray, GW_CHILD);
    wchar_t topCls[96]{};
    if (topBefore) GetClassNameW(topBefore, topCls, 96);
    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY, kWndClass,
                                L"ZenDesktop Taskbar Music", WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1, cp->tray, nullptr,
                                g_uiInst, nullptr);
    if (!hwnd) {
        Wh_Log(L"[ui] CreateWindowEx(child of %p) failed %u", cp->tray, GetLastError());
        return;
    }
    auto panel = std::make_unique<Panel>();
    panel->hwnd = hwnd;
    panel->tray = cp->tray;
    panel->secondary = IsSecondaryTaskbar(cp->tray);
    panel->subclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(cp->tray, TraySubclassProc, 0) != FALSE;
    if (!panel->subclassed) Wh_Log(L"[ui] subclassing taskbar %p failed; z-order only checked on the 100 ms tick", cp->tray);
    g_panels.push_back(std::move(panel));
    Panel& p = *g_panels.back();
    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    SetTimer(hwnd, TIMER_TICK, 100, nullptr);
    RECT cr{};
    GetClientRect(cp->tray, &cr);
    Wh_Log(L"[ui] panel %p attached as child of %s %p (taskbar thread %u, client %dx%d, dpi %u, top child before: %s)",
           hwnd, p.secondary ? L"Shell_SecondaryTrayWnd" : L"Shell_TrayWnd", cp->tray, GetCurrentThreadId(),
           cr.right - cr.left, cr.bottom - cr.top, GetDpiForWindow(cp->tray), topBefore ? topCls : L"none");
    RenderPanel(p, true);
    cp->ok = true;
}

// pv = the tray whose panel to destroy.
static void WINAPI DestroyPanelOnTrayThread(void* pv) {
    HWND tray = (HWND)pv;
    std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
    for (auto it = g_panels.begin(); it != g_panels.end(); ++it) {
        Panel& p = **it;
        if (p.tray != tray) continue;
        if (p.subclassed && IsWindow(tray)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(tray, TraySubclassProc);
        p.subclassed = false;
        if (p.hwnd && IsWindow(p.hwnd)) {
            HWND h = p.hwnd;
            KillTimer(h, TIMER_TICK);
            DestroyWindow(h);
            Wh_Log(L"[ui] panel %p destroyed (taskbar %p)", h, tray);
        }
        p.hwnd = nullptr;
        g_panels.erase(it);
        return;
    }
}

static void WINAPI ForceRenderOnTrayThread(void* pv) {
    std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
    if (Panel* p = PanelOfTray((HWND)pv)) {
        if (p->hwnd) {
            p->lastWhy.clear();
            RenderPanel(*p, true);
        }
    }
}

// ── Our UI thread ────────────────────────────────────────────────────────────
static void SyncPanels(bool forceRender) {
    if (g_stop) return;
    Settings s = GetSettings();
    auto trays = FindTaskbars(s.multiMonitor);
    {
        static int lastCount = -1;
        static bool loggedForeign = false, loggedMissing = false;
        if ((int)trays.size() != lastCount) {
            lastCount = (int)trays.size();
            Wh_Log(L"[ui] taskbars found: %d (multiMonitor=%d)", lastCount, s.multiMonitor ? 1 : 0);
        }
        if (trays.empty() && !loggedForeign) {
            HWND any = FindWindowW(L"Shell_TrayWnd", nullptr);
            if (any && !OwnedByThisProcess(any)) {
                loggedForeign = true;
                Wh_Log(L"[ui] Shell_TrayWnd %p belongs to another process; this explorer.exe (pid %u) stays idle", any,
                       GetCurrentProcessId());
            } else if (!any && !loggedMissing) {
                loggedMissing = true;
                Wh_Log(L"[ui] Shell_TrayWnd not found yet, retrying every 2s");
            }
        }
    }
    auto wanted = [&](HWND t) { return std::find(trays.begin(), trays.end(), t) != trays.end(); };

    std::vector<HWND> have, drop;
    {
        std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
        for (auto it = g_panels.begin(); it != g_panels.end();) {
            Panel& p = **it;
            bool trayAlive = IsWindow(p.tray) != FALSE;
            bool childAlive = p.hwnd && IsWindow(p.hwnd);
            if (!trayAlive) {  // taskbar recreated: the child died with it, subclass auto-removed
                Wh_Log(L"[ui] taskbar %p is gone, panel will be re-attached", p.tray);
                it = g_panels.erase(it);
                continue;
            }
            if (!childAlive || !wanted(p.tray)) drop.push_back(p.tray);
            else have.push_back(p.tray);
            ++it;
        }
    }
    for (HWND t : drop) RunFromWindowThread(t, DestroyPanelOnTrayThread, t);
    for (HWND t : trays) {
        if (g_stop) return;
        if (std::find(have.begin(), have.end(), t) != have.end()) {
            if (forceRender) RunFromWindowThread(t, ForceRenderOnTrayThread, t);
            continue;
        }
        CreateParam cp{t, false};
        if (!RunFromWindowThread(t, CreatePanelOnTrayThread, &cp))
            Wh_Log(L"[ui] could not reach the thread of taskbar %p (SetWindowsHookEx failed %u)", t, GetLastError());
        else if (!cp.ok)
            Wh_Log(L"[ui] panel for taskbar %p not created", t);
    }
}

static void DestroyAllPanels() {
    std::vector<HWND> trays;
    {
        std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
        for (auto& p : g_panels)
            if (IsWindow(p->tray)) trays.push_back(p->tray);
    }
    for (HWND t : trays) RunFromWindowThread(t, DestroyPanelOnTrayThread, t);
    std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
    for (auto& p : g_panels)
        if (p->hwnd && IsWindow(p->hwnd)) Wh_Log(L"[ui] warning: panel %p could not be destroyed", p->hwnd);
    g_panels.clear();
}

static UINT g_taskbarCreatedMsg = 0;

// Hidden top-level window (message-only windows don't get the TaskbarCreated broadcast).
static LRESULT CALLBACK HelperProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        Wh_Log(L"[ui] TaskbarCreated received, re-attaching panels");
        PostThreadMessageW(GetCurrentThreadId(), WM_ZEN_SYNC, 0, 0);
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wp == TIMER_SYNC) SyncPanels(false);
            return 0;
        case WM_DISPLAYCHANGE:
            PostThreadMessageW(GetCurrentThreadId(), WM_ZEN_SYNC, 0, 0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void UiThread() {
    g_uiThreadId = GetCurrentThreadId();
    MSG m;
    PeekMessageW(&m, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    Gdiplus::GdiplusStartupInput gsi;
    ULONG_PTR gdipToken = 0;  // process-wide; the taskbar thread paints with it too
    if (Gdiplus::GdiplusStartup(&gdipToken, &gsi, nullptr) != Gdiplus::Ok) {
        Wh_Log(L"[ui] GdiplusStartup failed, panel disabled");
        return;
    }
    Wh_Log(L"[ui] thread started (pid %u)", GetCurrentProcessId());

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&WndProc, &g_uiInst);
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_uiInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWndClass;
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        Wh_Log(L"[ui] RegisterClass failed %u", GetLastError());
    WNDCLASSW hc{};
    hc.lpfnWndProc = HelperProc;
    hc.hInstance = g_uiInst;
    hc.lpszClassName = kHelperClass;
    if (!RegisterClassW(&hc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        Wh_Log(L"[ui] RegisterClass(helper) failed %u", GetLastError());

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    HWND helper = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kHelperClass, L"ZenLyricsHelper", WS_POPUP, 0, 0,
                                  0, 0, nullptr, nullptr, g_uiInst, nullptr);
    if (helper) SetTimer(helper, TIMER_SYNC, 2000, nullptr);
    else Wh_Log(L"[ui] helper window failed %u; taskbar will not be re-scanned", GetLastError());

    SyncPanels(false);

    while (!g_stop) {
        BOOL r = GetMessageW(&m, nullptr, 0, 0);
        if (r <= 0) break;
        if (m.message == WM_ZEN_SETTINGS) {
            SyncPanels(true);
            continue;
        }
        if (m.message == WM_ZEN_SYNC) {
            SyncPanels(false);
            continue;
        }
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    if (helper) {
        KillTimer(helper, TIMER_SYNC);
        DestroyWindow(helper);
    }
    DestroyAllPanels();  // on each taskbar's own thread; removes the taskbar subclass too
    UnregisterClassW(kWndClass, g_uiInst);
    UnregisterClassW(kHelperClass, g_uiInst);
    Gdiplus::GdiplusShutdown(gdipToken);
    g_uiInst = nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk entry points
// ─────────────────────────────────────────────────────────────────────────────
static std::thread g_mediaThread, g_lyricsThread, g_uiThread, g_aiThread;

BOOL Wh_ModInit() {
    Wh_Log(L"Init 0.4.0 (pid %u)", GetCurrentProcessId());
    LoadSettings();
    g_stop = false;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_cmdEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_cmdEvent) {
        if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
        if (g_cmdEvent) { CloseHandle(g_cmdEvent); g_cmdEvent = nullptr; }
        return FALSE;
    }
    g_lyricsThread = std::thread(LyricsThread);
    g_mediaThread = std::thread(MediaThread);
    g_aiThread = std::thread(AiThread);
    g_uiThread = std::thread(UiThread);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (DWORD tid = g_uiThreadId.load()) PostThreadMessageW(tid, WM_ZEN_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_stop = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    {
        std::lock_guard<std::mutex> lk(g_reqLock);  // avoid lost wakeup
    }
    g_reqCv.notify_all();
    // WM_QUIT works whether or not the window currently exists; the loop then destroys it on its own thread.
    if (DWORD tid = g_uiThreadId.load()) PostThreadMessageW(tid, WM_QUIT, 0, 0);
    // Joins can take up to ~3s per in-flight HTTP call / one GSMTC call.
    if (g_uiThread.joinable()) g_uiThread.join();
    if (g_aiThread.joinable()) g_aiThread.join();
    if (g_mediaThread.joinable()) g_mediaThread.join();
    if (g_lyricsThread.joinable()) g_lyricsThread.join();
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    if (g_cmdEvent) { CloseHandle(g_cmdEvent); g_cmdEvent = nullptr; }
    g_uiThreadId = 0;
}
