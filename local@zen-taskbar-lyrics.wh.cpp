// ==WindhawkMod==
// @id              zen-taskbar-lyrics
// @name            ZenDesktop: 任务栏音乐与歌词
// @name:zh-CN      ZenDesktop: 任务栏音乐与歌词
// @description     Compact media panel in the taskbar Widgets slot: album art, synced lyrics, playback buttons, and AI agent status.
// @description:zh-CN 在任务栏左下角（小组件的位置）放一个音乐小面板：专辑封面、同步歌词、上一首／播放暂停／下一首，还能显示本机 AI 编程助手的状态。
// @version         0.5.21
// @author          Lanbo
// @github          https://github.com/Liset999
// @donateUrl       https://ifdian.net/a/Lanboo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinhttp -lruntimeobject -lgdiplus -lole32 -loleaut32 -lshell32 -lshlwapi -luser32 -lgdi32 -ladvapi32 -ldwmapi -lcomctl32 -luuid -lpdh -liphlpapi -ld3d11 -ldxgi -ldcomp -ld3dcompiler_47
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

# ZenDesktop: 任务栏音乐与歌词

## 简介
把 Windows 11 的小组件关掉以后，任务栏左下角会空出一块。这个 mod 在那里放一个音乐小面板，同时可以显示本机 AI 编程助手（Claude Code、Codex、opencode、Antigravity、DeepSeek Harness 等）正在做什么。

开发者：Lanbo（ZenDesktop）。哔哩哔哩：https://space.bilibili.com/1280632999 ，抖音：https://v.douyin.com/Isl7QPpKZZ4/ ，爱发电：https://ifdian.net/a/Lanboo

## 功能
- 支持 Spotify、网易云音乐、QQ 音乐等播放器（接入系统媒体控制的都可以）。
- 专辑封面：来自系统媒体会话，播放器提供才有。
- 歌词：显示当前这一句（网易云、QQ 音乐、LRCLIB），第二行可显示翻译或「歌名 - 歌手」。查不到歌词时显示歌名和歌手。
- 播放控制：上一首、播放／暂停、下一首，鼠标移上去会高亮；播放器不支持的按钮会变灰。
- **歌词展开面板**（0.5.0）：单击封面或文字，从任务栏所在边弹出展开面板（带弹性动画；0.5.3 起自动适配顶／左／右任务栏）；再点一次、点面板外面，或鼠标离开几秒后自动收起。面板不抢焦点、没有输入框。各板块可在「常用开关」中单独开关。「颜色与外观 → 卡片材质」可选液态玻璃（默认磨砂玻璃，液态玻璃仅展开时运行）：
  - 播放控制：封面、歌名歌手、可点击跳转的进度条（播放器需支持）、上一首／播放暂停／下一首、「打开播放器」按钮。
  - AI 状态：和歌词条同一份数据（状态文件、钩子、MCP、进程名），逐个列出助手、状态、项目和说明。
  - 文件中转：把文件拖到收起的歌词条上（会高亮）即可暂存，只记录源路径；拖出到别的软件或文件夹时只允许复制，不剪切、不移动源文件，单击在资源管理器里定位，「清空」一键清除。可设保留天数（默认 3 天），过期或源文件已删的条目会自动移出列表（不删源文件）。列表只在内存里，重载 explorer／mod 会清空。
  - 天气：当前气温和天气（open-meteo.com，不需要密钥）。默认开启，自动定位（本机定位 → 可选按 IP 估计 → 手动城市后备），也可改成只用手动城市；后台线程获取并缓存，不卡任务栏。
  - 资源占用：CPU、内存、GPU、网络上下行。展开资源板块或空闲栏显示资源占用时采样。
- 想要旧行为（点击切到播放器）可在「点封面或文字」里选「打开播放器」，或选「不响应」。
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

建议先接钩子（第 3 步），钩子不用助手配合、也不会弹权限确认；MCP 用来补充「在做什么」的一句说明。DeepSeek Harness 例外，先接 MCP（原因见第 3 步）。钩子推荐用 `zen-status.exe`（几毫秒就跑完，Antigravity 这类同步钩子也不会变卡），包里的 `tools\ai-status\` 已经带了它，运行 `install.ps1` 时会自动用上；没有它时自动退回 PowerShell 脚本（每次约 0.3–0.5 秒）。

**注意权限弹窗：** Antigravity 等会拦截 MCP 工具的 IDE/CLI，第一次调用 `set_status` 时会弹出权限确认，请选「Always Allow／总是允许」，否则每次更新状态都会打断你。

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

Antigravity（`~/.gemini/config/mcp_config.json`，在 Antigravity 里点 … → MCP Servers → Manage MCP Servers → View raw config 打开的就是它；旧版本是 `~/.gemini/antigravity/mcp_config.json`）。已有文件的话，只把 `zen-status` 这一项合并进 `mcpServers`：

```
{
  "mcpServers": {
    "zen-status": {
      "command": "powershell.exe",
      "args": ["-NoProfile", "-ExecutionPolicy", "Bypass", "-File", "C:\\Users\\你的用户名\\AppData\\Local\\ZenDesktop\\ai-status-tools\\zen-status-mcp.ps1", "-Agent", "antigravity", "-Name", "Antigravity"]
    }
  }
}
```

DeepSeek Harness（`dsh`，配置在 `%USERPROFILE%\.dsh\`）：把下面这一项加到 `%USERPROFILE%\.dsh\cordis.patch.yml`。没有这个文件就直接新建；已经有了就把这一项追加到列表末尾。`install.ps1` 生成的 `deepseek-harness-cordis.patch.yml` 里路径已经填好。改完重启 dsh，再新开一个会话：

```
- insert:
    - id: mcp-zen-status
      name: '@deepseek-ai/dsh-mcp-client'
      config:
        serverName: zen-status
        transport: stdio
        command: powershell.exe
        args: ['-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', 'C:\Users\你的用户名\AppData\Local\ZenDesktop\ai-status-tools\zen-status-mcp.ps1', '-Agent', 'deepseek-harness', '-Name', 'DeepSeek Harness']
```

### 2. 给助手的说明（粘贴到 AGENTS.md、CLAUDE.md 或 GEMINI.md）
Codex、opencode 和 DeepSeek Harness 读 `AGENTS.md`（DeepSeek Harness 全局的放在 `~/.dsh/AGENTS.md`），Claude Code 读 `CLAUDE.md`（全局的放在 `~/.claude/CLAUDE.md`），Antigravity 读 `GEMINI.md`（全局的放在 `~/.gemini/GEMINI.md`）。

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
片段都在 `install.ps1` 生成的 `snippets\` 文件夹里，路径已经填好。
- Claude Code：把 `claude-code-settings.json` 里的 `hooks` 合并进 `~/.claude/settings.json`。
- Codex：把 `codex-hooks.json` 放到 `~/.codex/hooks.json`（已有的话合并），然后在 Codex 里用 `/hooks` 信任这些钩子；也可以只用 `config.toml` 里的 `notify`，它只在一轮回答结束时触发。
- Antigravity：把 `antigravity-hooks.json` 放到 `~/.gemini/config/hooks.json`（已有的话把 `zen-status` 这一项合并进去），可在 … → Customizations → Hooks 里查看和开关。
- opencode：把 `tools/ai-status/opencode/zen-status.js` 复制到 `~/.config/opencode/plugins/`。
- DeepSeek Harness：钩子是可选的，要先装实验插件 `@deepseek-ai/dsh-hooks-claude-code`，步骤见 `tools/ai-status/README.md`。dsh 在 Windows 上用沙箱跑钩子，默认权限下钩子很可能写不进状态文件，所以请以 MCP 为主。
- 其他没有钩子的工具：在设置「按进程名显示」里填进程名（默认已填 `Antigravity.exe, Antigravity IDE.exe, DeepSeek Harness.exe`），进程在运行时显示「运行中」；这个助手已经用钩子或 MCP 汇报时不会重复显示。

## 设置说明
设置分为「常用开关」「颜色与外观」「高级设置」。日常显示开关和配色在前；自定义布局、字体、天气定位、AI 接入和歌词源在高级子组中。玻璃颜色与浓度同时控制展开卡片和有底色的任务栏条；任务栏选「无底色」时保持透明。「透明（自定义玻璃）」和液态玻璃都可独立调整模糊。自定义玻璃只模糊背景，不带折射；普通磨砂仍使用系统亚克力。两种可调玻璃在打开卡片时取一帧背景，取景后恢复截图和录屏。再次打开会更新背景。
- 位置：小组件位置（推荐）、固定在最左边、固定在托盘左边。
- 位置偏移：横向任务栏左右移动，侧边任务栏上下移动。按 100% 缩放的像素，高 DPI 自动放大，范围 -500–2000。起始位置模式下正数向右／下，托盘模式下正数远离托盘。
- 空闲时显示：可选「未在播放」「显示天气」「显示资源占用」或隐藏面板。有 AI 状态时优先显示 AI。展开卡片的板块开关单独设置。
- 面板宽度：紧凑 260、标准 340、宽 440、超宽 560，或自定义 120–2000。
- 面板玻璃：默认开启，为展开卡片启用系统亚克力模糊，关闭后只保留半透明底色。任务栏条沿用任务栏本身的模糊。「液态玻璃背景」是独立的 GPU 材质开关。
- 多显示器：默认关闭（只主屏）。开启后每个任务栏各显示一个面板，媒体与 AI 状态共用。
- 第一行、第二行：歌词、翻译、歌名歌手的组合。
- 文字颜色、面板底色、字体、字号（10–16 磅）、粗体。面板底色默认「无底色」：完全透明，没有底色、边框和阴影，文字、封面和按钮直接画在任务栏上，看起来就是任务栏的一部分，只有鼠标移上去时有很淡的高亮。
- 歌词偏移：毫秒，正数提前，默认 200（抵消播放器报进度和刷新的延迟），范围 -60000–60000。
- 全屏和游戏时隐藏：默认关闭。开启后全屏游戏、视频、PPT 放映时隐藏（多显示器只藏全屏那块屏）。
- AI 状态：一行列出、轮流显示或关闭；「已完成」保留多久；按进程名显示（默认填了 Antigravity 和 DeepSeek Harness 桌面版，清空即可关闭）。

## 兼容性与已知问题
- 支持 Windows 11 任务栏。底部横向为默认；顶部／左侧／右侧（含 Windhawk 竖向任务栏、ExplorerPatcher 等把任务栏挪走的情况）会自动检测边并改布局，无需额外开关。默认只在主显示器显示；开启「多显示器」后，每个任务栏（含副屏 `Shell_SecondaryTrayWnd`）各有一个面板。竖向任务栏上具体子窗口结构因美化 mod 而异，托盘／开始按钮避让按常见布局估算。
- 小组件没关时会和小组件按钮重叠，请在「设置 → 个性化 → 任务栏」里关掉小组件。
- 任务栏图标靠左时，托盘的位置靠估算，如果和托盘重叠，调「水平偏移」。
- 面板是任务栏（`Shell_TrayWnd`／`Shell_SecondaryTrayWnd`）的子窗口，排在任务栏自己的 XAML 内容上面。子窗口用不了系统亚克力，所以面板自己不做模糊，透出来的是任务栏本身的材质；「淡淡一层」「卡片」是在上面叠一层半透明色。
- 文字颜色选得和任务栏深浅接近时（比如浅色模式选白色），会自动加一层很淡的阴影保证看得清；默认颜色不加。
- 面板不出来时：在 Windhawk 这个 mod 的「高级设置」里打开调试日志，再看日志输出里 `[ui]`、`[media]` 开头的行，会写明「没找到任务栏」「没有媒体会话」「判定为全屏」等原因；正常时会有一行 `attached as child of Shell_TrayWnd`。没在放歌且「没在放歌时」选了「隐藏面板」时不显示是正常的；默认是显示「未在播放」。
- 点「打开播放器」只能切换到有可见窗口的播放器；缩到托盘里的播放器切不出来。
- 网易云、QQ 音乐的歌词接口是非官方的，随时可能失效。
- 歌词缓存在 `%LOCALAPPDATA%\ZenDesktop\lyrics\`。所有联网和媒体控制都在后台线程，不会卡任务栏。媒体会话与 AI 状态在多显示器间共用一份。

## 更新日志
- **0.5.21**：修复精简设置布局下「单击操作」等常用项不生效：以前仅当旧键 positionMode 非空才加载整段设置，导致界面已选「打开播放器」仍按默认展开；现以 common／general 布局探测为准加载。
- **0.5.20**：多应用同时 Playing 时优先音乐播放器（Spotify／网易云／QQ 音乐等），B站／浏览器等低优先级源即使在播也不会长期抢走歌词；文件夹窗口 explorer 不再启动任何工作线程，设置变更不再刷屏。
- **0.5.19**：媒体会话改为粘滞选择（优先正在播放、短时不跟系统 current 抖动），避免 Spotify 与 B站／浏览器抢会话时歌词乱切；仅任务栏宿主 explorer 进程跑媒体／歌词／AI／天气线程，文件夹窗口进程不再重复搜词。
- **0.5.18**：天气定位或预报获取失败后，每隔 1 分钟自动重试，成功后恢复设置的刷新间隔；重试期间保留上次成功的天气。
- **0.5.17**：侧边任务栏避让开始按钮时排除整条高度的 Start／LaunchBand 宿主，避免位置偏移被推到底部；整条高度的托盘容器改用后备定位。补充侧边栏偏移方向说明。
- **0.5.13**：「透明」改为真正的自定义玻璃，可独立调整模糊度、颜色、浓度与颜色亮度。模糊为 0 时不做背景采集；有模糊时使用无折射的 GPU 背景。液态玻璃另加折射效果。保留颜色预设、设置布局和 120fps 动画。
- **0.5.12**：卡片材质「半透明」改名为「透明」；液态玻璃模糊度前置，可直接选强度；玻璃颜色补齐红／绿／蓝等直观预设，不用手写色码。自选颜色移至高级，支持新版 Windhawk 原生色盘；保留旧颜色和模糊值。
- **0.5.11**：展开／收起动画改用高精度等待计时器，以 120fps 为调度目标；使用高精度时间戳，并合并积压帧。动画结束立即停止计时。液态玻璃保留 20fps 刷新目标，优先保障卡片动画；实际画面帧率取决于显示器与系统负载。设置布局不变。
- **0.5.10**：展开／收起动画复用绘图表面和内容快照，避免逐帧排版和检查中转文件；弹簧按实际时间推进。液态玻璃收起改为后台停止捕获并休眠，界面线程不等待 GPU 清理。设置布局保持不变。
- **0.5.9**：展开卡片的进度条支持点击跳转，位置按点击比例计算；播放器不支持进度调整时保持只读。跳转在媒体线程执行，并检查曲目和播放器没有切换。
- **0.5.8**：设置精简为「常用开关」「颜色与外观」「高级设置」。常用显示开关集中在顶部；合并重复的玻璃开关为一个材质选择；字体、布局、定位与歌词源收进高级子组，缩短标签和说明。保留旧设置。
- **0.5.7**：文件中转拖入／拖出统一为复制语义，拖出不再允许移动或创建快捷方式；源文件保留。资源占用开关移至「常用：点击与布局」前部，保留旧开关值。
- **0.5.6**：修复歌词条与任务栏音频频谱反复争夺顶层导致的闪烁。层级维护跳过频谱、隐藏窗口和不相交窗口，仍在被任务栏内容实际遮挡时抬高。展开液态玻璃的折射限于圆角边缘，避免宽折射带穿过距离场法线切换线产生三角折痕。天气板块默认开启，使用自动定位。
- **0.5.5**：没有媒体会话时隐藏整个播放板块，释放缓存封面；不再显示上次歌曲的封面、歌手、进度和灰色按钮。有会话但暂停时仍可继续播放。
- **0.5.4**：AI 状态和空闲时也能点击展开；移除动画窗口的透明预留带，避免顶部双层玻璃；设置按位置、外观、歌词、AI、展开面板、玻璃材质分组；玻璃支持开始菜单同款配色、自定义 #RRGGBB、0–100 浓度、0–150 颜色亮度及液态玻璃 0–60 模糊。任务栏选无底色仍完全透明。
- **0.5.3**：自动检测任务栏所在边（底／顶／左／右）：主任务栏用 `SHAppBarMessage(ABM_GETTASKBARPOS)`，副屏或未更新 ABM 的美化 mod 用任务栏矩形相对显示器比较，竖向以高大于宽为准。竖向任务栏收起态改为贴合宽度的方块／竖条（封面＋播放状态；设置「竖向任务栏歌词」可选竖排滚动歌词，默认只显示封面），不超出任务栏宽度。迷你面板与歌词展开面板从对应边弹出（底向上、顶向下、左向右、右向左），弹性动画沿该轴，并夹在显示器工作区内；拖放命中与液态玻璃捕获矩形跟随新几何。底部横向行为与 0.5.2 一致。
- **0.5.2**：展开面板可选「液态玻璃」背景（默认关闭）：WGC 捕获面板后方区域 + D3D11 模糊／折射／色散／高光边；仅展开时采集和渲染，收起后停止捕获并休眠；失败回退到亚克力／半透明。截图／录屏时面板与玻璃层不参与捕获。
- **0.5.1**：天气支持自动定位（本机定位 → 可选 IP 估计 → 手动城市）；文件中转可按保留天数自动清理（默认 3 天，不删源文件；列表仍不持久化）。展开面板底色与迷你面板一致（跟随面板底色／玻璃设置，半透明；尝试 DWM 亚克力／圆角，失败则保留半透明衬底），文字颜色与阴影也跟迷你面板走。
- **0.5.0**：新增「歌词展开面板」：单击歌词条展开（弹性动画、点外面或闲置自动收起、不抢焦点），含播放控制、AI 状态详情、文件中转（拖到歌词条暂存，可再拖出）、天气（open-meteo）、资源占用（仅展开时采样）五个可单独开关的板块。「点封面或文字」新增「展开面板」并设为默认。
- **0.4.8**：性能：PID→进程名缓存、任务栏 HWND 短缓存、设置快照按代际复用。
- **0.4.7**：说明里写明支持 Spotify、网易云音乐、QQ 音乐等播放器。
- **0.4.6**：微信赞赏链接改为指向 `main` 分支。
- **0.4.5**：AI 状态的钩子程序 `zen-status.exe` 已随包附带，说明里不再要求去 GitHub Actions 下载。
- **0.4.4**：歌词偏移默认改为 +200 毫秒，抵消播放器报进度和面板刷新的延迟，歌词不再慢半拍。之前保存过设置的不会自动改，可以手动设成 200。换句改为按下一句的开始时间精确定时，不再最多晚 100 毫秒。修复播放器很久没更新进度时，刚打开面板歌词位置不对的问题。
- **0.4.3**：DeepSeek Harness 成为正式支持的助手：显示名「DeepSeek Harness」，「按进程名显示」默认加上桌面版的 `DeepSeek Harness.exe`。说明里补充它的 MCP（`~/.dsh/cordis.patch.yml`）、`AGENTS.md` 和可选钩子的接法。

## 致谢与许可
- 歌词源和控制面板的设计参考了 TaskbarLyrics（https://github.com/ANYNC/TaskbarLyrics ，MIT 许可，© 2026 anync），没有复制其代码。
- 歌词数据来自 LRCLIB、网易云音乐、QQ 音乐，版权归原权利人所有。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- common:
  - clickAction: expand
    $name: 单击操作
    $name:zh-CN: 单击操作
    $options:
    - expand: 展开面板
    - open: 打开播放器
    - none: 不响应
  - resources: true
    $name: 资源占用
    $name:zh-CN: 资源占用
  - weather: true
    $name: 天气
    $name:zh-CN: 天气
  - files: true
    $name: 文件中转
    $name:zh-CN: 文件中转
  - media: true
    $name: 音乐播放
    $name:zh-CN: 音乐播放
  - ai: true
    $name: AI 状态
    $name:zh-CN: AI 状态
  - sizePreset: standard
    $name: 任务栏宽度
    $name:zh-CN: 任务栏宽度
    $options:
    - compact: 紧凑（260）
    - standard: 标准（340）
    - wide: 宽（440）
    - extra: 超宽（560）
    - custom: 自定义（用下面的宽度）
  - whenIdle: show
    $name: 空闲时显示
    $name:zh-CN: 空闲时显示
    $options:
    - show: 未在播放（默认）
    - weather: 显示天气
    - resources: 显示资源占用
    - hide: 隐藏面板
  - hideInFullscreen: false
    $name: 全屏时隐藏
    $name:zh-CN: 全屏时隐藏
  $name: 常用开关
  $name:zh-CN: 常用开关
- appearance:
  - effect: acrylic
    $name: 卡片材质
    $name:zh-CN: 卡片材质
    $options:
    - plain: 透明（自定义玻璃）
    - acrylic: 磨砂玻璃
    - liquid: 液态玻璃
  - blur: 12
    $name: 模糊度
    $name:zh-CN: 模糊度
    $description: 自定义玻璃和液态玻璃可调；0 不模糊，数值越大越模糊。
    $description:zh-CN: 自定义玻璃和液态玻璃可调；0 不模糊，数值越大越模糊。
    $options:
    - 0: 清晰（0）
    - 3: 很轻（3）
    - 6: 轻微（6）
    - 12: 标准（12）
    - 20: 柔和（20）
    - 30: 较强（30）
    - 45: 强（45）
    - 60: 最强（60）
  - color: Default
    $name: 玻璃颜色
    $name:zh-CN: 玻璃颜色
    $options:
    - Default: 跟随系统深浅色
    - Accent: 系统强调色
    - GlassWhite: 透白
    - MistGrey: 雾灰
    - DeepBlack: 深黑
    - Red: 红色
    - Green: 绿色
    - Blue: 蓝色
    - Yellow: 黄色
    - Orange: 橙色
    - Purple: 紫色
    - Pink: 粉色
    - Cyan: 青色
    - OceanBlue: 海蓝
    - AuroraCyan: 极光青
    - RosePink: 玫瑰粉
    - BordeauxRed: 酒红
    - ForestGreen: 森林绿
    - RoyalPurple: 皇家紫
    - SunsetOrange: 落日橙
    - ChampagneGold: 香槟金
    - MorandiSage: 莫兰迪绿
    - Custom: 自选颜色（高级）
  - opacity: 40
    $name: 颜色浓度（%）
    $name:zh-CN: 颜色浓度（%）
    $description: 0 完全透明，100 完全不透明。任务栏无底色时仍透明。
    $description:zh-CN: 0 完全透明，100 完全不透明。任务栏无底色时仍透明。
  - luminosity: 100
    $name: 颜色亮度
    $name:zh-CN: 颜色亮度
    $description: 100 为原色，范围 0–150。
    $description:zh-CN: 100 为原色，范围 0–150。
  - textColorMode: default
    $name: 文字颜色
    $name:zh-CN: 文字颜色
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
  - fontSize: 12
    $name: 字号
    $name:zh-CN: 字号
    $options:
    - 10: 10 磅
    - 11: 11 磅
    - 12: 12 磅（推荐）
    - 13: 13 磅
    - 14: 14 磅
    - 16: 16 磅
  $name: 颜色与外观
  $name:zh-CN: 颜色与外观
- advanced:
  - layout:
    - positionMode: widgets
      $name: 任务栏位置
      $name:zh-CN: 任务栏位置
      $options:
      - widgets: 小组件位置（推荐）
      - left: 固定在任务栏最左边
      - auto_tray: 固定在托盘左边
    - xOffset: 12
      $name: 位置偏移（像素）
      $name:zh-CN: 位置偏移（像素）
      $description: 横向任务栏左右移动，侧边任务栏上下移动。起始位置模式下正数向右／下，托盘模式下正数远离托盘。高 DPI 自动缩放，范围 -500–2000。
      $description:zh-CN: 横向任务栏左右移动，侧边任务栏上下移动。起始位置模式下正数向右／下，托盘模式下正数远离托盘。高 DPI 自动缩放，范围 -500–2000。
    - width: 340
      $name: 自定义任务栏宽度
      $name:zh-CN: 自定义任务栏宽度
      $description: 任务栏宽度选「自定义」时生效，120–2000 像素。
      $description:zh-CN: 任务栏宽度选「自定义」时生效，120–2000 像素。
    - expandWidth: normal
      $name: 展开卡片宽度
      $name:zh-CN: 展开卡片宽度
      $options:
      - narrow: 窄（300）
      - normal: 标准（360）
      - wide: 宽（440）
    - autoCollapseSeconds: 8
      $name: 自动收起（秒）
      $name:zh-CN: 自动收起（秒）
    - multiMonitor: false
      $name: 所有显示器显示
      $name:zh-CN: 所有显示器显示
    - verticalLyric: cover
      $name: 竖向任务栏布局
      $name:zh-CN: 竖向任务栏布局
      $options:
      - cover: 只显示封面
      - scroll: 竖排滚动歌词
    $name: 布局
    $name:zh-CN: 布局
  - text:
    - fontPreset: msyh
      $name: 字体
      $name:zh-CN: 字体
      $options:
      - msyh: 微软雅黑 UI
      - segoe: Segoe UI
      - dengxian: 等线
      - custom: 自定义（下面填写）
    - customFont: ''
      $name: 自定义字体名
      $name:zh-CN: 自定义字体名
      $description: 字体选「自定义」时填写。
      $description:zh-CN: 字体选「自定义」时填写。
    - bold: false
      $name: 标题加粗
      $name:zh-CN: 标题加粗
    - showArt: true
      $name: 任务栏封面
      $name:zh-CN: 任务栏封面
    - showButtons: true
      $name: 任务栏播放按钮
      $name:zh-CN: 任务栏播放按钮
    - textMode: lyrics
      $name: 第一行
      $name:zh-CN: 第一行
      $options:
      - lyrics: 当前歌词（没有就显示歌名）
      - title: 只显示歌名
    - secondLine: auto
      $name: 第二行
      $name:zh-CN: 第二行
      $options:
      - auto: 有翻译显示翻译，没有显示歌名和歌手
      - translation: 只显示翻译
      - info: 歌名和歌手
      - none: 不显示，只要一行
    $name: 字体与内容
    $name:zh-CN: 字体与内容
  - glass:
    - panelBackground: none
      $name: 任务栏底色
      $name:zh-CN: 任务栏底色
      $options:
      - none: 无底色（最通透）
      - subtle: 淡淡一层
      - card: 卡片（最浓）
    - customColor: '#1A365D'
      #! $format: colorRgb
      $name: 自选颜色
      $name:zh-CN: 自选颜色
      $description: 颜色选「自选颜色」时使用；支持色盘的设置页可直接点选。
      $description:zh-CN: 颜色选「自选颜色」时使用；支持色盘的设置页可直接点选。
    $name: 玻璃细节
    $name:zh-CN: 玻璃细节
  - weather:
    - locationMode: auto
      $name: 定位方式
      $name:zh-CN: 定位方式
      $options:
      - auto: 自动定位
      - manual: 只用手动城市
    - city: ''
      $name: 手动城市
      $name:zh-CN: 手动城市
      $description: 手动定位时必填；自动定位失败时作为后备。
      $description:zh-CN: 手动定位时必填；自动定位失败时作为后备。
    - allowIp: true
      $name: 允许 IP 定位
      $name:zh-CN: 允许 IP 定位
      $description: 本机定位不可用时，使用 ipwho.is 估计城市。
      $description:zh-CN: 本机定位不可用时，使用 ipwho.is 估计城市。
    - refreshMinutes: 30
      $name: 刷新间隔（分钟）
      $name:zh-CN: 刷新间隔（分钟）
    $name: 天气定位
    $name:zh-CN: 天气定位
  - relay:
    - retentionDays: 3
      $name: 保留天数
      $name:zh-CN: 保留天数
      $description: 只清除列表记录，不删源文件；0 为不按天过期。
      $description:zh-CN: 只清除列表记录，不删源文件；0 为不按天过期。
    $name: 文件中转
    $name:zh-CN: 文件中转
  - ai:
    - mode: compact
      $name: 状态显示方式
      $name:zh-CN: 状态显示方式
      $options:
      - compact: 一行列出所有助手
      - rotate: 轮流显示（每 4 秒换一个，带说明）
      - false: 关闭
    - doneSeconds: '120'
      $name: 完成状态保留
      $name:zh-CN: 完成状态保留
      $options:
      - '30': 30 秒
      - '120': 2 分钟
      - '600': 10 分钟
      - '0': 一直显示
    - processes: Antigravity.exe, Antigravity IDE.exe, DeepSeek Harness.exe
      $name: 按进程名识别
      $name:zh-CN: 按进程名识别
      $description: 逗号分隔进程名；留空关闭。
      $description:zh-CN: 逗号分隔进程名；留空关闭。
    $name: AI 接入
    $name:zh-CN: AI 接入
  - lyrics:
    - offsetMs: 200
      $name: 同步偏移（毫秒）
      $name:zh-CN: 同步偏移（毫秒）
      $description: 正数提前，负数推后。
      $description:zh-CN: 正数提前，负数推后。
    - netease: true
      $name: 网易云
      $name:zh-CN: 网易云
    - qq: true
      $name: QQ 音乐
      $name:zh-CN: QQ 音乐
    - lrclib: true
      $name: LRCLIB
      $name:zh-CN: LRCLIB
    $name: 歌词来源
    $name:zh-CN: 歌词来源
  $name: 高级设置
  $name:zh-CN: 高级设置
  $description: 仅在需要精细调整时修改。
  $description:zh-CN: 仅在需要精细调整时修改。
*/
// ==/WindhawkModSettings==

#include <winsock2.h>  // before windows.h: netioapi (GetIfTable2) needs the v2 socket types
#include <windows.h>
#include <initguid.h>
#include <knownfolders.h>
#include <winhttp.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <tlhelp32.h>
#include <commctrl.h>
#include <ole2.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dcomp.h>
#include <dxgi1_2.h>
#include <inspectable.h>
#include <roapi.h>
#include <winstring.h>
#include <wrl/client.h>

#include <windhawk_utils.h>

#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Devices.Geolocation.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
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
// Settings. Defaults below mirror the settings block; they are used when the
// mod's settings were never written (e.g. installed by deploy.ps1 without
// opening the settings page). Compact layout is detected via common.* keys;
// do not require advanced.layout.positionMode — that key can stay unread while
// common.clickAction is already set, and gating on it skipped clickAction.
// ─────────────────────────────────────────────────────────────────────────────
enum class PosMode { Widgets, Left, Tray };
enum class SecondLine { Auto, Translation, Info, None };
enum class PanelBg { None, Subtle, Card };
enum class AiMode { Off, Compact, Rotate };
enum class IdleMode { Message, Weather, Resources, Hide };

struct Settings {
    PosMode posMode = PosMode::Widgets;
    int xOffset = 12;
    int width = 340;
    bool showArt = true;
    bool showButtons = true;
    bool titleOnly = false;
    SecondLine secondLine = SecondLine::Auto;
    IdleMode idleMode = IdleMode::Message;
    bool clickOpens = true;
    bool clickExpands = true;      // 歌词展开面板
    int expandWidth = 360;
    int expandAutoCollapseSec = 8;
    bool xMedia = true, xAi = true, xFiles = true, xWeather = true, xRes = true;
    bool weatherLocAuto = true;       // false = manual city only
    bool weatherAllowIp = true;       // IP estimate behind its own switch
    std::wstring weatherCity;
    int weatherRefreshMin = 30;
    int filesRetentionDays = 3;       // 0 = never expire by age
    bool expandCustomGlass = false;  // configurable blur without refraction
    bool expandLiquidGlass = false;   // optional WGC+D3D11 glass behind expand panel
    bool verticalScrollLyric = false; // vertical taskbar: cover-only (false) or 竖排滚动歌词
    std::wstring textColorMode = L"default";
    PanelBg panelBg = PanelBg::None;
    std::wstring font = L"Microsoft YaHei UI";
    int fontSize = 12;
    bool bold = false;
    int offsetMs = 200;  // compensates player progress-report + redraw latency (tested by Lanbo)
    bool hideInFullscreen = false;
    bool panelGlass = true;
    std::wstring glassColor, glassCustomColor;
    int glassOpacity = 40, glassLuminosity = 100, glassBlur = 12;
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
static std::atomic<uint64_t> g_settingsGen{1};
static thread_local uint64_t tls_settingsGen = 0;
static thread_local Settings tls_settings;

static int g_settingsLayout = 0;  // 0: flat, 1: 0.5.4 groups, 2: compact settings
static std::wstring SettingKey(PCWSTR name) {
    if (g_settingsLayout == 2) {
        if (wcscmp(name, L"clickAction") == 0) return L"common.clickAction";
        if (wcscmp(name, L"expandPanel.resources") == 0) return L"common.resources";
        if (wcscmp(name, L"expandPanel.weather") == 0) return L"common.weather";
        if (wcscmp(name, L"expandPanel.files") == 0) return L"common.files";
        if (wcscmp(name, L"expandPanel.media") == 0) return L"common.media";
        if (wcscmp(name, L"expandPanel.ai") == 0) return L"common.ai";
        if (wcscmp(name, L"sizePreset") == 0) return L"common.sizePreset";
        if (wcscmp(name, L"whenIdle") == 0) return L"common.whenIdle";
        if (wcscmp(name, L"hideInFullscreen") == 0) return L"common.hideInFullscreen";
        if (wcscmp(name, L"material.color") == 0) return L"appearance.color";
        if (wcscmp(name, L"material.customColor") == 0) return L"advanced.glass.customColor";
        if (wcscmp(name, L"material.opacity") == 0) return L"appearance.opacity";
        if (wcscmp(name, L"material.luminosity") == 0) return L"appearance.luminosity";
        if (wcscmp(name, L"textColorMode") == 0) return L"appearance.textColorMode";
        if (wcscmp(name, L"fontSize") == 0) return L"appearance.fontSize";
        if (wcscmp(name, L"positionMode") == 0) return L"advanced.layout.positionMode";
        if (wcscmp(name, L"xOffset") == 0) return L"advanced.layout.xOffset";
        if (wcscmp(name, L"width") == 0) return L"advanced.layout.width";
        if (wcscmp(name, L"expandPanel.width") == 0) return L"advanced.layout.expandWidth";
        if (wcscmp(name, L"expandPanel.autoCollapseSeconds") == 0) return L"advanced.layout.autoCollapseSeconds";
        if (wcscmp(name, L"multiMonitor") == 0) return L"advanced.layout.multiMonitor";
        if (wcscmp(name, L"verticalLyric") == 0) return L"advanced.layout.verticalLyric";
        if (wcscmp(name, L"fontPreset") == 0) return L"advanced.text.fontPreset";
        if (wcscmp(name, L"customFont") == 0) return L"advanced.text.customFont";
        if (wcscmp(name, L"bold") == 0) return L"advanced.text.bold";
        if (wcscmp(name, L"showArt") == 0) return L"advanced.text.showArt";
        if (wcscmp(name, L"showButtons") == 0) return L"advanced.text.showButtons";
        if (wcscmp(name, L"textMode") == 0) return L"advanced.text.textMode";
        if (wcscmp(name, L"secondLine") == 0) return L"advanced.text.secondLine";
        if (wcscmp(name, L"panelBackground") == 0) return L"advanced.glass.panelBackground";
        if (wcscmp(name, L"material.blur") == 0) return L"appearance.blur";
        if (wcscmp(name, L"expandPanel.weatherLocationMode") == 0) return L"advanced.weather.locationMode";
        if (wcscmp(name, L"expandPanel.weatherCity") == 0) return L"advanced.weather.city";
        if (wcscmp(name, L"expandPanel.weatherAllowIp") == 0) return L"advanced.weather.allowIp";
        if (wcscmp(name, L"expandPanel.weatherRefreshMinutes") == 0) return L"advanced.weather.refreshMinutes";
        if (wcscmp(name, L"expandPanel.filesRetentionDays") == 0) return L"advanced.relay.retentionDays";
        if (wcscmp(name, L"aiStatus") == 0) return L"advanced.ai.mode";
        if (wcscmp(name, L"aiDoneSeconds") == 0) return L"advanced.ai.doneSeconds";
        if (wcscmp(name, L"aiProcesses") == 0) return L"advanced.ai.processes";
        if (wcscmp(name, L"offsetMs") == 0) return L"advanced.lyrics.offsetMs";
        if (wcscmp(name, L"enableNetease") == 0) return L"advanced.lyrics.netease";
        if (wcscmp(name, L"enableQQ") == 0) return L"advanced.lyrics.qq";
        if (wcscmp(name, L"enableLrclib") == 0) return L"advanced.lyrics.lrclib";
    }
    if (g_settingsLayout == 1) {
        if (wcscmp(name, L"expandPanel.resources") == 0) return L"general.resources";
        if (wcscmp(name, L"positionMode") == 0) return L"general.positionMode";
        if (wcscmp(name, L"xOffset") == 0) return L"general.xOffset";
        if (wcscmp(name, L"sizePreset") == 0) return L"general.sizePreset";
        if (wcscmp(name, L"width") == 0) return L"general.width";
        if (wcscmp(name, L"whenIdle") == 0) return L"general.whenIdle";
        if (wcscmp(name, L"clickAction") == 0) return L"general.clickAction";
        if (wcscmp(name, L"multiMonitor") == 0) return L"general.multiMonitor";
        if (wcscmp(name, L"hideInFullscreen") == 0) return L"general.hideInFullscreen";
        if (wcscmp(name, L"verticalLyric") == 0) return L"general.verticalLyric";
        if (wcscmp(name, L"showArt") == 0) return L"appearance.showArt";
        if (wcscmp(name, L"showButtons") == 0) return L"appearance.showButtons";
        if (wcscmp(name, L"textColorMode") == 0) return L"appearance.textColorMode";
        if (wcscmp(name, L"fontPreset") == 0) return L"appearance.fontPreset";
        if (wcscmp(name, L"customFont") == 0) return L"appearance.customFont";
        if (wcscmp(name, L"fontSize") == 0) return L"appearance.fontSize";
        if (wcscmp(name, L"bold") == 0) return L"appearance.bold";
        if (wcscmp(name, L"panelBackground") == 0) return L"appearance.panelBackground";
        if (wcscmp(name, L"panelGlass") == 0) return L"appearance.panelGlass";
        if (wcscmp(name, L"textMode") == 0) return L"lyrics.textMode";
        if (wcscmp(name, L"secondLine") == 0) return L"lyrics.secondLine";
        if (wcscmp(name, L"offsetMs") == 0) return L"lyrics.offsetMs";
        if (wcscmp(name, L"enableNetease") == 0) return L"lyrics.enableNetease";
        if (wcscmp(name, L"enableQQ") == 0) return L"lyrics.enableQQ";
        if (wcscmp(name, L"enableLrclib") == 0) return L"lyrics.enableLrclib";
        if (wcscmp(name, L"aiStatus") == 0) return L"ai.aiStatus";
        if (wcscmp(name, L"aiDoneSeconds") == 0) return L"ai.aiDoneSeconds";
        if (wcscmp(name, L"aiProcesses") == 0) return L"ai.aiProcesses";
    }
    return name;
}
static int GetInt(PCWSTR name) { return Wh_GetIntSetting(SettingKey(name).c_str()); }

static std::wstring GetStr(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(SettingKey(name).c_str());
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

// True when Windhawk has a writable settings layout (grouped/compact) or the
// legacy flat positionMode key. Compact users often have common.clickAction set
// while advanced.layout.positionMode is still empty — that must still load.
static bool SettingsBodyWritten(int layout, const std::wstring& positionMode) {
    return layout > 0 || !positionMode.empty();
}

static void LoadSettings() {
    PCWSTR grouped = Wh_GetStringSetting(L"general.positionMode");
    g_settingsLayout = grouped && *grouped ? 1 : 0;
    Wh_FreeStringSetting(grouped);
    PCWSTR compact = Wh_GetStringSetting(L"common.clickAction");
    if (compact && *compact) g_settingsLayout = 2;
    Wh_FreeStringSetting(compact);
    Settings s;  // defaults
    s.glassColor = GetStr(L"material.color");
    s.glassCustomColor = GetStr(L"material.customColor");
    if (!s.glassColor.empty()) {
        s.glassOpacity = std::clamp(GetInt(L"material.opacity"), 0, 100);
        s.glassLuminosity = std::clamp(GetInt(L"material.luminosity"), 0, 150);
        s.glassBlur = std::clamp(GetInt(L"material.blur"), 0, 60);
    }
    std::wstring mode = GetStr(L"positionMode");
    if (!SettingsBodyWritten(g_settingsLayout, mode)) {
        Wh_Log(L"settings not written yet, using defaults");
    } else {
        if (!mode.empty()) {
            s.posMode = mode == L"left" ? PosMode::Left : mode == L"auto_tray" ? PosMode::Tray : PosMode::Widgets;
        }
        s.xOffset = std::clamp(GetInt(L"xOffset"), -500, 2000);
        std::wstring size = GetStr(L"sizePreset");
        if (size == L"compact") s.width = 260;
        else if (size == L"wide") s.width = 440;
        else if (size == L"extra") s.width = 560;
        else if (size == L"custom") { int w = GetInt(L"width"); s.width = w > 0 ? std::clamp(w, 120, 2000) : 340; }
        else s.width = 340;
        s.showArt = GetInt(L"showArt") != 0;
        s.showButtons = GetInt(L"showButtons") != 0;
        s.titleOnly = GetStr(L"textMode") == L"title";
        std::wstring sl = GetStr(L"secondLine");
        s.secondLine = sl == L"translation" ? SecondLine::Translation : sl == L"info" ? SecondLine::Info
                     : sl == L"none" ? SecondLine::None : SecondLine::Auto;
        {
            std::wstring wi = GetStr(L"whenIdle");
            if (wi == L"hide") s.idleMode = IdleMode::Hide;
            else if (wi == L"weather") s.idleMode = IdleMode::Weather;
            else if (wi == L"resources") s.idleMode = IdleMode::Resources;
        }
        {
            std::wstring ca = GetStr(L"clickAction");
            s.clickExpands = ca.empty() || ca == L"expand";
            s.clickOpens = ca != L"none";  // body is clickable for open and expand
        }
        {
            std::wstring w = GetStr(L"expandPanel.width");
            s.expandWidth = w == L"narrow" ? 300 : w == L"wide" ? 440 : 360;
            s.expandAutoCollapseSec = std::clamp(GetInt(L"expandPanel.autoCollapseSeconds"), 0, 600);
            s.xMedia = GetInt(L"expandPanel.media") != 0;
            s.xAi = GetInt(L"expandPanel.ai") != 0;
            s.xFiles = GetInt(L"expandPanel.files") != 0;
            s.xWeather = GetInt(L"expandPanel.weather") != 0;
            s.xRes = GetInt(L"expandPanel.resources") != 0;
            s.weatherLocAuto = GetStr(L"expandPanel.weatherLocationMode") != L"manual";
            s.weatherAllowIp = GetInt(L"expandPanel.weatherAllowIp") != 0;
            s.weatherCity = GetStr(L"expandPanel.weatherCity");
            int rm = GetInt(L"expandPanel.weatherRefreshMinutes");
            s.weatherRefreshMin = rm > 0 ? std::clamp(rm, 5, 1440) : 30;
            int fd = GetInt(L"expandPanel.filesRetentionDays");
            // 0 = never by age; negative / missing → default 3
            s.filesRetentionDays = fd < 0 ? 3 : std::clamp(fd, 0, 3650);
            s.expandLiquidGlass = GetInt(L"expandPanel.liquidGlass") != 0;
        }
        s.verticalScrollLyric = GetStr(L"verticalLyric") == L"scroll";
        s.textColorMode = GetStr(L"textColorMode");
        if (s.textColorMode.empty()) s.textColorMode = L"default";
        std::wstring bg = GetStr(L"panelBackground");
        s.panelBg = bg == L"subtle" ? PanelBg::Subtle : bg == L"card" ? PanelBg::Card : PanelBg::None;
        std::wstring preset = GetStr(L"fontPreset"), custom = GetStr(L"customFont");
        if (preset == L"segoe") s.font = L"Segoe UI";
        else if (preset == L"dengxian") s.font = L"DengXian";
        else if (preset == L"custom" && !custom.empty()) s.font = custom;
        int fs = GetInt(L"fontSize");
        s.fontSize = fs > 0 ? std::clamp(fs, 6, 48) : 12;
        s.bold = GetInt(L"bold") != 0;
        s.offsetMs = std::clamp(GetInt(L"offsetMs"), -60000, 60000);
        s.hideInFullscreen = GetInt(L"hideInFullscreen") != 0;
        s.panelGlass = GetInt(L"panelGlass") != 0;
        s.multiMonitor = GetInt(L"multiMonitor") != 0;
        s.enableNetease = GetInt(L"enableNetease") != 0;
        s.enableQQ = GetInt(L"enableQQ") != 0;
        s.enableLrclib = GetInt(L"enableLrclib") != 0;
        std::wstring ai = GetStr(L"aiStatus");
        s.aiMode = ai == L"off" ? AiMode::Off : ai == L"rotate" ? AiMode::Rotate : AiMode::Compact;
        std::wstring done = GetStr(L"aiDoneSeconds");
        s.aiDoneSeconds = done.empty() ? 120 : std::clamp(_wtoi(done.c_str()), 0, 86400);
        s.aiProcesses = GetStr(L"aiProcesses");
    }
    std::wstring effect = GetStr(L"appearance.effect");
    if (!effect.empty()) {
        s.panelGlass = effect != L"plain";
        s.expandCustomGlass = effect == L"plain";
        s.expandLiquidGlass = effect == L"liquid";
    }
    {
        std::lock_guard<std::mutex> lk(g_settingsLock);
        g_settings = std::move(s);
    }
    g_settingsGen.fetch_add(1, std::memory_order_release);
}

// Custom glass shares the start menu's independent blur/tint controls. A GDI
// popup cannot host its XAML WindhawkBlur brush, so use the existing captured
// backdrop renderer with refraction/highlights disabled. At blur=0 no capture is needed.
static bool WantsCapturedBackdrop(const Settings& s) {
    return s.expandLiquidGlass || (s.expandCustomGlass && s.glassBlur > 0);
}

static const Settings& GetSettings() {
    const uint64_t gen = g_settingsGen.load(std::memory_order_acquire);
    if (tls_settingsGen != gen) {
        std::lock_guard<std::mutex> lk(g_settingsLock);
        tls_settings = g_settings;
        tls_settingsGen = g_settingsGen.load(std::memory_order_relaxed);
    }
    return tls_settings;
}

static std::atomic<bool> g_stop{false};
static HANDLE g_stopEvent = nullptr;  // manual-reset; signalled on uninit, wakes sleeping threads
static HANDLE g_cmdEvent = nullptr;   // auto-reset; UI thread -> media thread "a command is queued"

static bool SleepOrStop(DWORD ms) {
    return WaitForSingleObject(g_stopEvent, ms) == WAIT_OBJECT_0;
}

// Windhawk injects into every explorer.exe (including separate folder windows).
// Only the process that owns Shell_TrayWnd should poll media / fetch lyrics.
static bool OwnedByThisProcess(HWND h) {
    DWORD pid = 0;
    return h && GetWindowThreadProcessId(h, &pid) && pid == GetCurrentProcessId();
}

// Probe primary taskbar ownership once. -1 = foreign folder-window explorer,
// 0 = tray not up yet, 1 = this process owns Shell_TrayWnd.
static int ProbeTaskbarHostRole() {
    struct Ctx {
        HWND ours = nullptr;
        HWND any = nullptr;
    } ctx;
    EnumWindows(
        [](HWND h, LPARAM lp) -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lp);
            wchar_t cls[64]{};
            if (!GetClassNameW(h, cls, 64) || _wcsicmp(cls, L"Shell_TrayWnd") != 0) return TRUE;
            if (!c->any) c->any = h;
            if (OwnedByThisProcess(h)) {
                c->ours = h;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
    if (ctx.ours) return 1;
    if (ctx.any) return -1;
    return 0;
}

// Returns true when this process owns the primary taskbar. Returns false (and
// the caller should exit its worker) when another explorer already owns it.
// If the tray is not up yet (shell still starting), waits until it appears or stop.
static bool WaitForTaskbarHostRole() {
    bool loggedForeign = false, loggedWaiting = false;
    while (!g_stop) {
        int role = ProbeTaskbarHostRole();
        if (role > 0) {
            if (loggedForeign || loggedWaiting) {
                Wh_Log(L"[host] pid %u owns Shell_TrayWnd; workers active", GetCurrentProcessId());
            }
            return true;
        }
        if (role < 0) {
            if (!loggedForeign) {
                loggedForeign = true;
                Wh_Log(L"[host] Shell_TrayWnd is foreign; pid %u skips media/lyrics/AI/weather workers",
                       GetCurrentProcessId());
            }
            return false;
        }
        if (!loggedWaiting) {
            loggedWaiting = true;
            Wh_Log(L"[host] Shell_TrayWnd not ready (pid %u); waiting", GetCurrentProcessId());
        }
        if (SleepOrStop(2000)) return false;
    }
    return false;
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
    bool canSeek = false;
    std::wstring trackKey;         // title \x1F artist
    std::shared_ptr<const Lyrics> lyrics;  // null = none/pending
    std::shared_ptr<const std::vector<BYTE>> thumb;  // encoded image bytes (PNG/JPEG), null = none
    unsigned thumbVersion = 0;     // bumped whenever thumb changes
    long long durationMs = 0;      // EndTime - StartTime, 0 = unknown
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
    if (!WaitForTaskbarHostRole()) return;
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
enum class MediaCmd : int { None = 0, Prev, PlayPause, Next, OpenPlayer, Seek };
struct PendingMediaCommand {
    MediaCmd kind = MediaCmd::None;
    double fraction = 0;
    std::wstring trackKey, sourceApp;
};
static std::mutex g_mediaCmdLock;
static PendingMediaCommand g_pendingCmd;

static void QueueMediaCommand(PendingMediaCommand command) {
    {
        std::lock_guard<std::mutex> lk(g_mediaCmdLock);
        g_pendingCmd = std::move(command);  // kind and payload travel together; latest wins
    }
    if (g_cmdEvent) SetEvent(g_cmdEvent);
}
static PendingMediaCommand TakeMediaCommand() {
    std::lock_guard<std::mutex> lk(g_mediaCmdLock);
    PendingMediaCommand command = std::move(g_pendingCmd);
    g_pendingCmd = {};
    return command;
}
static long long SeekPositionTicks(double fraction, long long start, long long end) {
    if (end <= start || !std::isfinite(fraction)) return start;
    return start + (long long)((long double)(end - start) * std::clamp(fraction, 0.0, 1.0));
}
static void PostMediaSeek(double fraction, const std::wstring& track, const std::wstring& app) {
    QueueMediaCommand({MediaCmd::Seek, std::clamp(fraction, 0.0, 1.0), track, app});
}

static void PostMediaCmd(MediaCmd c) {
    QueueMediaCommand({c});
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

static std::mutex g_pidExeMutex;
static std::unordered_map<DWORD, std::wstring> g_pidExeCache;
static ULONGLONG g_pidExeCacheTick = 0;

static std::wstring ExeNameOfPid(DWORD pid) {
    if (!pid) return {};
    const ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lk(g_pidExeMutex);
        if (now - g_pidExeCacheTick >= 30000) {
            g_pidExeCache.clear();
            g_pidExeCacheTick = now;
        } else {
            auto it = g_pidExeCache.find(pid);
            if (it != g_pidExeCache.end()) return it->second;
        }
    }
    std::wstring out;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return out;
    wchar_t pathBuf[MAX_PATH];
    DWORD n = MAX_PATH;
    if (QueryFullProcessImageNameW(h, 0, pathBuf, &n)) {
        const wchar_t* name = wcsrchr(pathBuf, L'\\');
        out = name ? name + 1 : pathBuf;
    }
    CloseHandle(h);
    if (!out.empty()) {
        std::lock_guard<std::mutex> lk(g_pidExeMutex);
        g_pidExeCache[pid] = out;
    }
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

// Pure sticky-session policy (no WinRT). Kept extractable for regression tests.
// Prefer dedicated music apps over video/browser SMTC stealers when several are
// Playing. Stick through rapid GetCurrentSession flips; only yield after holdMs
// when sticky stops Playing, or when a higher-priority Playing app appears.
struct MediaSessionCandidate {
    std::wstring app;
    bool playing = false;
    bool hasTitle = false;
};
struct StickySessionState {
    std::wstring stickyApp;
    std::wstring challengerApp;
    ULONGLONG challengerSince = 0;
};
static constexpr ULONGLONG kSessionStickHoldMs = 1500;

static int MediaAppPriority(const std::wstring& app) {
    std::wstring lower = app;
    for (auto& ch : lower) {
        if (ch >= L'A' && ch <= L'Z') ch = (wchar_t)(ch - L'A' + L'a');
    }
    auto hasL = [&](const wchar_t* token) { return lower.find(token) != std::wstring::npos; };
    if (hasL(L"spotify") || hasL(L"qqmusic") || hasL(L"cloudmusic") || hasL(L"netease") ||
        hasL(L"music.ui") || hasL(L"applemusic") || hasL(L"itunes") || hasL(L"foobar") ||
        hasL(L"dopamine") || hasL(L"aimp") || hasL(L"musicbee") || hasL(L"yesplaymusic") ||
        hasL(L"listen1") || hasL(L"lyricease") || hasL(L"groove") || hasL(L"mediaplayer"))
        return 100;
    if (hasL(L"bilibili")) return 10;
    if (hasL(L"chrome") || hasL(L"msedge") || hasL(L"firefox") || hasL(L"opera") ||
        hasL(L"brave") || hasL(L"vivaldi") || hasL(L"movies") || hasL(L"video.ui") ||
        hasL(L"wmplayer") || hasL(L"potplayer") || hasL(L"vlc") || hasL(L"mpv"))
        return 20;
    return 50;
}

static std::wstring PickStickyMediaApp(const std::vector<MediaSessionCandidate>& cands,
                                       const std::wstring& systemCurrentApp,
                                       StickySessionState& st,
                                       ULONGLONG now,
                                       ULONGLONG holdMs = kSessionStickHoldMs) {
    auto findApp = [&](const std::wstring& app) -> const MediaSessionCandidate* {
        if (app.empty()) return nullptr;
        for (const auto& c : cands) {
            if (c.app == app && c.hasTitle) return &c;
        }
        return nullptr;
    };
    auto scoreOf = [&](const MediaSessionCandidate& c) {
        int score = MediaAppPriority(c.app);
        if (c.playing) score += 1000;
        if (c.app == systemCurrentApp) score += 1;  // tie-break only
        return score;
    };
    const MediaSessionCandidate* best = nullptr;
    int bestScore = -1;
    for (const auto& c : cands) {
        if (!c.hasTitle) continue;
        int score = scoreOf(c);
        if (score > bestScore) {
            bestScore = score;
            best = &c;
        }
    }

    const MediaSessionCandidate* sticky = findApp(st.stickyApp);
    auto acceptChallenger = [&](const MediaSessionCandidate* challenger) -> std::wstring {
        if (!challenger) {
            st.challengerApp.clear();
            st.challengerSince = 0;
            return sticky ? sticky->app : std::wstring{};
        }
        if (st.challengerApp != challenger->app) {
            st.challengerApp = challenger->app;
            st.challengerSince = now;
        }
        if (now - st.challengerSince >= holdMs) {
            st.stickyApp = challenger->app;
            st.challengerApp.clear();
            st.challengerSince = 0;
            return challenger->app;
        }
        return sticky->app;
    };

    if (sticky && sticky->playing) {
        // Same/lower priority Playing apps (typical SMTC thrash) never displace us.
        // Higher-priority music can take over after holdMs even while sticky still plays.
        if (best && best->app != sticky->app && best->playing &&
            MediaAppPriority(best->app) > MediaAppPriority(sticky->app)) {
            return acceptChallenger(best);
        }
        st.challengerApp.clear();
        st.challengerSince = 0;
        return sticky->app;
    }
    if (sticky) {
        if (best && best->app != sticky->app && best->playing) return acceptChallenger(best);
        st.challengerApp.clear();
        st.challengerSince = 0;
        return sticky->app;
    }
    st.challengerApp.clear();
    st.challengerSince = 0;
    if (best) {
        st.stickyApp = best->app;
        return best->app;
    }
    st.stickyApp.clear();
    return {};
}

static GlobalSystemMediaTransportControlsSession FindSessionByApp(
    GlobalSystemMediaTransportControlsSessionManager const& mgr, const std::wstring& app) {
    if (!mgr || app.empty()) return nullptr;
    try {
        for (auto&& s : mgr.GetSessions()) {
            if (std::wstring{s.SourceAppUserModelId()} == app) return s;
        }
    } catch (...) {
    }
    return nullptr;
}

static void RunMediaCmd(GlobalSystemMediaTransportControlsSessionManager const& mgr, const PendingMediaCommand& command) {
    MediaCmd c = command.kind;
    if (!mgr) return;
    std::wstring prefer = command.sourceApp;
    if (prefer.empty()) {
        std::lock_guard<std::mutex> lk(g_stateLock);
        prefer = g_state.sourceApp;
    }
    auto session = FindSessionByApp(mgr, prefer);
    if (!session) session = mgr.GetCurrentSession();
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
        case MediaCmd::Seek: {
            // The UI can outlive a track/session change. Never seek a different player
            // or song using geometry from the card that the user actually clicked.
            if (std::wstring{session.SourceAppUserModelId()} != command.sourceApp) break;
            auto props = session.TryGetMediaPropertiesAsync().get();
            std::wstring key = std::wstring{props.Title()} + L"\x1F" + std::wstring{props.Artist()};
            if (key != command.trackKey || !session.GetPlaybackInfo().Controls().IsPlaybackPositionEnabled()) break;
            auto timeline = session.GetTimelineProperties();
            long long start = timeline.StartTime().count(), end = timeline.EndTime().count();
            if (end <= start) break;
            // GSMTC takes 100 ns ticks, including nonzero timeline origins.
            bool accepted = session.TryChangePlaybackPositionAsync(SeekPositionTicks(command.fraction, start, end)).get();
            if (!accepted) Wh_Log(L"[media] player rejected seek");
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
    if (!WaitForTaskbarHostRole()) return;
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
    StickySessionState sticky;
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
            if (PendingMediaCommand command = TakeMediaCommand(); command.kind != MediaCmd::None) {
                try { RunMediaCmd(mgr, command); } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"command %d failed 0x%08X", (int)command.kind, (unsigned)e.code());
                }
                if (command.kind == MediaCmd::Seek) lastReportedPos = -1;
                // Give the player a moment to apply it, so the next poll doesn't undo the optimistic icon.
                if (SleepOrStop(300)) break;
            }
            GlobalSystemMediaTransportControlsSession session{nullptr};
            std::wstring app, title, artist, album;
            if (mgr) {
                std::vector<MediaSessionCandidate> cands;
                std::wstring currentApp;
                try {
                    if (auto cur = mgr.GetCurrentSession()) currentApp = std::wstring{cur.SourceAppUserModelId()};
                    for (auto&& s : mgr.GetSessions()) {
                        MediaSessionCandidate c;
                        c.app = std::wstring{s.SourceAppUserModelId()};
                        try {
                            auto props = s.TryGetMediaPropertiesAsync().get();
                            c.hasTitle = !std::wstring{props.Title()}.empty();
                            auto st = s.GetPlaybackInfo().PlaybackStatus();
                            c.playing = st == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                        } catch (...) {
                        }
                        cands.push_back(std::move(c));
                    }
                } catch (...) {
                }
                ULONGLONG nowPick = GetTickCount64();
                std::wstring chosenApp = PickStickyMediaApp(cands, currentApp, sticky, nowPick);
                if (!chosenApp.empty()) session = FindSessionByApp(mgr, chosenApp);
            }
            if (!session) {
                if (!lastLoggedApp.empty() || !loggedNoSession) {
                    Wh_Log(L"[media] no media session (player not running or no system media controls)");
                    lastLoggedApp.clear();
                    loggedNoSession = true;
                }
                sticky.stickyApp.clear();
                sticky.challengerApp.clear();
                sticky.challengerSince = 0;
                markNoMedia();
            } else {
                auto props = session.TryGetMediaPropertiesAsync().get();
                auto tl = session.GetTimelineProperties();
                auto pb = session.GetPlaybackInfo();
                auto ctl = pb.Controls();
                title = std::wstring{props.Title()};
                artist = std::wstring{props.Artist()};
                album = std::wstring{props.AlbumTitle()};
                app = std::wstring{session.SourceAppUserModelId()};
                if (app != lastLoggedApp) {
                    Wh_Log(L"[media] session: %s, title empty=%d sticky=1", app.c_str(), title.empty() ? 1 : 0);
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
                    g_state.durationMs = durMs > 0 ? durMs : 0;
                    g_state.canPrev = ctl.IsPreviousEnabled();
                    g_state.canNext = ctl.IsNextEnabled();
                    g_state.canSeek = ctl.IsPlaybackPositionEnabled() && durMs > 0;
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
                        // LastUpdatedTime is when posMs was true. Players that report rarely can make
                        // it minutes old on the first poll; trust it as long as it stays inside the
                        // track (the old 60 s cap fell back to a stale position => lyrics lagged).
                        long long maxAge = durMs > 0 ? (long long)((double)(durMs - posMs) / rate) + 2000 : 600000;
                        if (ageMs < 0 || ageMs > maxAge || !playing) ageMs = 0;
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
    // Antigravity is Gemini-based; check it first so "antigravity" / "agy" never falls back to the raw key.
    if (k.find(L"antigravity") != std::wstring::npos || k == L"agy") return L"Antigravity";
    // DeepSeek Harness: status files use "deepseek-harness"; the Desktop app runs as "DeepSeek Harness.exe".
    if (k.find(L"deepseek") != std::wstring::npos || k == L"dsh") return L"DeepSeek Harness";
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
    if (!WaitForTaskbarHostRole()) return;
    std::wstring dir = AiStatusDir();
    std::vector<std::wstring> procs;
    ULONGLONG lastProcScan = 0;
    std::shared_ptr<const std::vector<AiEntry>> last;
    while (!g_stop) {
        Settings s = GetSettings();
        auto list = std::make_shared<std::vector<AiEntry>>();
        std::vector<std::wstring> reported;  // display names of every agent with a status file (even idle/stale)
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
                    reported.push_back(e.name);
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
                    // A hook / MCP already reports this agent (also when it said idle): don't add "Running".
                    bool covered = std::any_of(reported.begin(), reported.end(),
                                               [&](const std::wstring& n) { return _wcsicmp(n.c_str(), pretty.c_str()) == 0; });
                    if (covered) continue;
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
static constexpr UINT_PTR TIMER_LINE = 3;  // one-shot, fires right when the next lyric line starts

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

// Taskbar ownership check lives near SleepOrStop (also gates media/lyrics workers).
// Enumerate instead of FindWindow: status-bar tools (YASB etc.) also register Shell_TrayWnd
// (windhawk-mods issue #1704). Only the process that owns the taskbar shows the panel.

static bool IsSecondaryTaskbar(HWND h) {
    wchar_t cls[64]{};
    return h && GetClassNameW(h, cls, 64) && _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

static std::vector<HWND> g_cachedTaskbars;
static bool g_cachedTaskbarsMulti = false;
static ULONGLONG g_cachedTaskbarsTick = 0;

static std::vector<HWND> FindTaskbars(bool multi) {
    const ULONGLONG now = GetTickCount64();
    if (multi == g_cachedTaskbarsMulti && now - g_cachedTaskbarsTick < 800 && !g_cachedTaskbars.empty()) {
        bool allValid = true;
        for (HWND h : g_cachedTaskbars) {
            if (!h || !IsWindow(h)) { allValid = false; break; }
        }
        if (allValid) return g_cachedTaskbars;
    }
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
    g_cachedTaskbars = out;
    g_cachedTaskbarsMulti = multi;
    g_cachedTaskbarsTick = now;
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
    int nextLineInMs = -1;  // wall-clock ms until the next lyric line starts; -1 = none / not playing
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

static void SetIdleContent(Content& c, const Settings& s);

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
                if (g_state.playing) {
                    const auto& ls = g_state.lyrics->lines;
                    auto nx = std::upper_bound(ls.begin(), ls.end(), (int)pos,
                                               [](int v, const LyricLine& x) { return v < x.timeMs; });
                    if (nx != ls.end()) {
                        double ms = (double)(nx->timeMs - pos) / g_state.rate;
                        c.nextLineInMs = (int)std::min(ms + 1.0, 600000.0);
                    }
                }
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
    } else if (s.idleMode != IdleMode::Hide) {
        SetIdleContent(c, s);
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
    bool dropHover = false;              // 文件中转: a file drag is over the bar
};

struct Panel {
    HWND hwnd = nullptr;   // child of tray, owned by the taskbar thread
    HWND tray = nullptr;
    bool secondary = false;
    bool subclassed = false;
    bool oleInit = false, dropRegistered = false;
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


// Taskbar edge. Auto-detected per Shell_TrayWnd / Shell_SecondaryTrayWnd — no user setting.
enum class TaskbarEdge { Bottom = 0, Top, Left, Right };

static bool IsVerticalEdge(TaskbarEdge e) {
    return e == TaskbarEdge::Left || e == TaskbarEdge::Right;
}

// Primary: SHAppBarMessage(ABM_GETTASKBARPOS) when its rect still matches this tray.
// Secondary / mods that don't update ABM: compare tray rect to its monitor.
// Clearly taller-than-wide trays are treated as vertical even if ABM still says bottom.
static TaskbarEdge DetectTaskbarEdge(HWND tray) {
    RECT tr{};
    if (!tray || !GetWindowRect(tray, &tr)) return TaskbarEdge::Bottom;
    const int tw = tr.right - tr.left, th = tr.bottom - tr.top;
    const bool clearlyVertical = th > tw * 3 / 2;
    const bool clearlyHorizontal = tw > th * 3 / 2;

    auto fromMonitor = [&]() -> TaskbarEdge {
        HMONITOR mon = MonitorFromWindow(tray, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        if (!GetMonitorInfoW(mon, &mi))
            return clearlyVertical ? TaskbarEdge::Left : TaskbarEdge::Bottom;
        const RECT& m = mi.rcMonitor;
        const int dL = abs(tr.left - m.left), dR = abs(tr.right - m.right);
        const int dT = abs(tr.top - m.top), dB = abs(tr.bottom - m.bottom);
        if (clearlyVertical || (!clearlyHorizontal && th > tw))
            return dL <= dR ? TaskbarEdge::Left : TaskbarEdge::Right;
        return dT <= dB ? TaskbarEdge::Top : TaskbarEdge::Bottom;
    };

    if (!IsSecondaryTaskbar(tray)) {
        APPBARDATA abd{};
        abd.cbSize = sizeof(abd);
        abd.hWnd = tray;
        if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
            const RECT& ar = abd.rc;
            const int cxT = (tr.left + tr.right) / 2, cyT = (tr.top + tr.bottom) / 2;
            const int cxA = (ar.left + ar.right) / 2, cyA = (ar.top + ar.bottom) / 2;
            const bool matches = abs(cxA - cxT) <= std::max(tw, 96) && abs(cyA - cyT) <= std::max(th, 96);
            if (matches) {
                TaskbarEdge e = TaskbarEdge::Bottom;
                switch (abd.uEdge) {
                    case ABE_LEFT: e = TaskbarEdge::Left; break;
                    case ABE_TOP: e = TaskbarEdge::Top; break;
                    case ABE_RIGHT: e = TaskbarEdge::Right; break;
                    case ABE_BOTTOM: e = TaskbarEdge::Bottom; break;
                    default: break;
                }
                // Mods that rotate the bar but leave ABM on Bottom/Top: trust geometry.
                if (clearlyVertical && !IsVerticalEdge(e)) return fromMonitor();
                if (clearlyHorizontal && IsVerticalEdge(e)) return fromMonitor();
                return e;
            }
        }
    }
    return fromMonitor();
}

struct Layout {
    int cx = 0, cy = 0, w = 0, h = 0;  // client coordinates inside the taskbar
    int dpi = 96;
    int pad = 0, art = 0, btn = 0, cur = 0, textRight = 0;
    RECT artRc{};
    RECT btnRc[3]{};
    bool showArt = false, showButtons = false, bodyClickable = false;
    bool vertical = false;           // left/right taskbar
    bool vertScrollLyric = false;    // setting: 竖排滚动歌词
    TaskbarEdge edge = TaskbarEdge::Bottom;
    Gdiplus::ARGB text = 0xFFFFFFFF;
    Gdiplus::ARGB shadow = 0;
    bool light = false;
    Content content;
};

static Gdiplus::ARGB MaterialColor(const Settings& s, bool light) {
    DWORD rgb = light ? 0xF7F7F7 : 0x141414;
    const std::wstring& mode = s.glassColor;
    if (mode == L"Red") rgb = 0xEF4444;
    if (mode == L"Green") rgb = 0x22C55E;
    if (mode == L"Blue") rgb = 0x3B82F6;
    if (mode == L"Yellow") rgb = 0xFACC15;
    if (mode == L"Orange") rgb = 0xF97316;
    if (mode == L"Purple") rgb = 0xA855F7;
    if (mode == L"Pink") rgb = 0xEC4899;
    if (mode == L"Cyan") rgb = 0x06B6D4;
    if (mode == L"GlassWhite") rgb = 0xFFFFFF;
    if (mode == L"MistGrey") rgb = 0x2E2E2E;
    if (mode == L"DeepBlack") rgb = 0x0A0A0A;
    if (mode == L"OceanBlue") rgb = 0x1A365D;
    if (mode == L"AuroraCyan") rgb = 0x005C53;
    if (mode == L"RosePink") rgb = 0x5B2C30;
    if (mode == L"BordeauxRed") rgb = 0x4A0E17;
    if (mode == L"ForestGreen") rgb = 0x1A3A2A;
    if (mode == L"RoyalPurple") rgb = 0x3B1354;
    if (mode == L"SunsetOrange") rgb = 0x8C3D00;
    if (mode == L"ChampagneGold") rgb = 0x7A653F;
    if (mode == L"MorandiSage") rgb = 0x4E5E50;
    if (mode == L"Accent") {
        DWORD color = 0; BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) rgb = color & 0xFFFFFF;
    }
    if (mode == L"Custom" && s.glassCustomColor.size() == 7 && s.glassCustomColor[0] == L'#') {
        DWORD parsed = 0; bool valid = true;
        for (size_t i = 1; i < 7; ++i) {
            wchar_t c = s.glassCustomColor[i];
            int digit = c >= L'0' && c <= L'9' ? c - L'0' :
                        c >= L'a' && c <= L'f' ? c - L'a' + 10 :
                        c >= L'A' && c <= L'F' ? c - L'A' + 10 : -1;
            if (digit < 0) { valid = false; break; }
            parsed = (parsed << 4) | digit;
        }
        if (valid) rgb = parsed;
    }
    auto channel = [&](int shift) -> DWORD {
        return (DWORD)std::clamp((int)((rgb >> shift) & 255) * s.glassLuminosity / 100, 0, 255);
    };
    return ((DWORD)(s.glassOpacity * 255 / 100) << 24) | (channel(16) << 16) | (channel(8) << 8) | channel(0);
}

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
    int ch = cr.bottom - cr.top;
    L.edge = DetectTaskbarEdge(tray);
    L.vertical = IsVerticalEdge(L.edge);
    const bool idleInfo = L.content.idle && s.idleMode != IdleMode::Message;
    L.vertScrollLyric = L.vertical && (s.verticalScrollLyric || idleInfo);

    L.light = g_env.lightTaskbar;
    L.text = ResolveTextColor(s.textColorMode, L.light);
    // Default colors follow light/dark mode and need no shadow (looks native). A soft
    // shadow / glow only when the chosen color is close to the taskbar's brightness.
    if (IsLightColor(L.text) == L.light) L.shadow = IsLightColor(L.text) ? 0x80000000 : 0x70FFFFFF;

    int gap = MulDiv(s.xOffset, (int)dpi, 96);
    bool trayMode = s.posMode == PosMode::Tray || (s.posMode == PosMode::Widgets && g_env.alignLeft);
    POINT origin{0, 0};
    ClientToScreen(tray, &origin);  // tray notify area rect is in screen coordinates

    if (!L.vertical) {
        // Horizontal taskbar (bottom / top): keep 0.5.2 geometry.
        L.w = D(s.width);
        L.h = ch;
        if (L.h <= 0 || L.h > 1000 || L.w <= 0 || cw <= 0) { *why = L"bad taskbar height or panel width"; return false; }
        L.w = std::min(L.w, cw);
        int x = gap;
        if (trayMode) {
            HWND notify = FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr);
            RECT nr{};
            if (notify && GetWindowRect(notify, &nr) && nr.right - nr.left > D(40) && nr.left > tr.left &&
                nr.right <= tr.right)
                x = (nr.left - origin.x) - L.w - gap;
            else
                x = cw - L.w - gap - D(260);
        }
        L.cx = std::clamp(x, 0, std::max(0, cw - L.w));
        L.cy = 0;

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
        L.bodyClickable = s.clickExpands || (L.content.media && s.clickOpens);
        for (int i = 0; i < 3; ++i) panel.ui.hitRects[i] = L.btnRc[i];
        panel.ui.hitEnabled[HitPrev] = L.showButtons && L.content.canPrev;
        panel.ui.hitEnabled[HitPlay] = L.showButtons && L.content.canPlayPause;
        panel.ui.hitEnabled[HitNext] = L.showButtons && L.content.canNext;
        panel.ui.hitRects[HitBody] =
            L.bodyClickable ? RECT{D(2), D(2), L.showButtons ? L.btnRc[0].left : L.w - D(2), L.h - D(2)} : RECT{};
        panel.ui.hitEnabled[HitBody] = L.bodyClickable;
    } else {
        // Vertical taskbar (left / right): compact tile that never overflows the bar width.
        if (cw <= 0 || ch <= 0 || cw > 1000) { *why = L"bad vertical taskbar size"; return false; }
        L.w = cw;
        L.pad = D(4);
        L.showArt = true;  // cover tile is the point of the vertical layout
        L.showButtons = false;  // too narrow for prev/play/next
        const int artMax = std::max(D(22), cw - D(8));
        L.art = artMax;
        if (L.vertScrollLyric) {
            // Tall tile: art on top, vertical lyric below. Cap so we don't eat the whole bar.
            int want = L.art + D(12) + D(std::max(90, s.fontSize * 8));
            L.h = std::clamp(want, L.art + D(40), std::max(L.art + D(40), ch * 2 / 5));
        } else {
            L.h = std::min(ch, L.art + D(8));  // near-square cover tile
        }
        L.h = std::min(L.h, ch);
        int y = gap;
        if (trayMode) {
            HWND notify = FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr);
            RECT nr{};
            if (notify && GetWindowRect(notify, &nr) && nr.bottom - nr.top > D(16) && nr.top > tr.top &&
                nr.bottom <= tr.bottom + 2)
                y = (nr.top - origin.y) - L.h - gap;
            else
                y = ch - L.h - gap - D(80);
        }
        // Keep clear of a Start / search cluster commonly sitting at the top of a left bar.
        if (!trayMode) {
            HWND start = FindWindowExW(tray, nullptr, L"Start", nullptr);
            if (!start) start = FindWindowExW(tray, nullptr, L"LaunchBand", nullptr);
            RECT sr{};
            // Only reserve a compact top cluster. A full-height sidebar host can
            // use these class names too; its bottom would pin every offset to ch.
            if (start && GetWindowRect(start, &sr) && sr.top >= tr.top - 2 && sr.bottom > sr.top &&
                sr.bottom - origin.y <= ch / 2) {
                int below = (sr.bottom - origin.y) + gap;
                if (below > y) y = below;
            }
        }
        L.cx = 0;
        L.cy = std::clamp(y, 0, std::max(0, ch - L.h));
        int ax = (L.w - L.art) / 2;
        int ay = L.vertScrollLyric ? L.pad : (L.h - L.art) / 2;
        L.artRc = {ax, ay, ax + L.art, ay + L.art};
        L.cur = L.pad;
        L.textRight = L.w - L.pad;
        L.btn = 0;
        for (int i = 0; i < 3; ++i) L.btnRc[i] = {};
        L.bodyClickable = s.clickExpands || (L.content.media && s.clickOpens);
        for (int i = 0; i < 3; ++i) {
            panel.ui.hitRects[i] = {};
            panel.ui.hitEnabled[i] = false;
        }
        panel.ui.hitRects[HitBody] =
            L.bodyClickable ? RECT{D(1), D(1), L.w - D(1), L.h - D(1)} : RECT{};
        panel.ui.hitEnabled[HitBody] = L.bodyClickable;
    }
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
    Gdiplus::RectF panelRc = L.vertical
                                  ? Gdiplus::RectF((Gdiplus::REAL)D(1), (Gdiplus::REAL)D(2), fw - D(2), fh - D(4))
                                  : Gdiplus::RectF((Gdiplus::REAL)D(2), (Gdiplus::REAL)D(4), fw - D(4), fh - D(8));
    Gdiplus::REAL rad = (Gdiplus::REAL)D(L.vertical ? 6 : 8);

    PanelLook look = LookFor(s.panelBg, s.panelGlass, L.light);
    if (!s.glassColor.empty() && s.panelBg != PanelBg::None) look.fill = MaterialColor(s, L.light);
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

    if (panel.ui.dropHover) {
        FillRound(g, WithAlpha(L.text, 0x30), panelRc, rad);
        Gdiplus::GraphicsPath dp;
        RoundRectPath(dp, panelRc.X + 1, panelRc.Y + 1, panelRc.Width - 2, panelRc.Height - 2, rad);
        Gdiplus::Pen pen(Gdiplus::Color(WithAlpha(L.text, 0xC0)), (Gdiplus::REAL)D(1.5));
        g.DrawPath(&pen, &dp);
    }
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
        // Vertical cover tile: play/pause badge on the art so state is visible without buttons.
        if (L.vertical && L.content.media) {
            Gdiplus::REAL badge = std::max<Gdiplus::REAL>(10.0f, ar.Width * 0.28f);
            Gdiplus::REAL bx = ar.X + ar.Width - badge - D(2);
            Gdiplus::REAL by = ar.Y + ar.Height - badge - D(2);
            FillRound(g, WithAlpha(L.light ? 0xFF000000u : 0xFFFFFFFFu, 0x88),
                      Gdiplus::RectF(bx, by, badge, badge), badge * 0.35f);
            DrawIcon(g, HitPlay, L.content.playing, bx + badge / 2, by + badge / 2, badge * 0.42f, L.text);
        }
    }

    if (L.vertical) {
        if (L.vertScrollLyric) {
            Gdiplus::FontFamily fam(s.font.c_str());
            const Gdiplus::FontFamily* useFam = fam.IsAvailable() ? &fam : Gdiplus::FontFamily::GenericSansSerif();
            int style = s.bold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular;
            Gdiplus::REAL px = (Gdiplus::REAL)s.fontSize * (Gdiplus::REAL)L.dpi / 72.0f;
            px = std::min(px, (Gdiplus::REAL)std::max(10, L.w - D(6)));
            Gdiplus::Font font1(useFam, px, style, Gdiplus::UnitPixel);
            Gdiplus::REAL so = std::max<Gdiplus::REAL>(1.0f, (Gdiplus::REAL)L.dpi / 96.0f);
            Gdiplus::REAL top = (Gdiplus::REAL)(L.artRc.bottom + D(4));
            Gdiplus::REAL th = fh - top - (Gdiplus::REAL)D(4);
            Gdiplus::REAL tx = (Gdiplus::REAL)D(2);
            Gdiplus::REAL tw = fw - (Gdiplus::REAL)D(4);
            if (th > 8 && tw > 4 && !L.content.line1.empty()) {
                // Vertical writing (top → bottom). Current lyric advances as the song does.
                Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
                fmt.SetFormatFlags(Gdiplus::StringFormatFlagsDirectionVertical |
                                   Gdiplus::StringFormatFlagsNoWrap |
                                   Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
                fmt.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
                fmt.SetAlignment(Gdiplus::StringAlignmentNear);
                fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                std::wstring text;
                for (const Seg& sg : L.content.line1) text += sg.text;
                Gdiplus::RectF dst(tx, top, tw, th);
                if (L.shadow) {
                    Gdiplus::SolidBrush sh{Gdiplus::Color(L.shadow)};
                    Gdiplus::RectF ds(dst.X, dst.Y + so, dst.Width, dst.Height);
                    g.DrawString(text.c_str(), -1, &font1, ds, &fmt, &sh);
                }
                Gdiplus::SolidBrush b{Gdiplus::Color(L.text)};
                g.DrawString(text.c_str(), -1, &font1, dst, &fmt, &b);
            }
        }
        g.Flush(Gdiplus::FlushIntentionSync);
        return;
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
           std::to_wstring(g_settingsGen.load(std::memory_order_relaxed)) + L":" + std::to_wstring((int)s.panelBg) + (s.panelGlass ? L"G" : L"g") + std::to_wstring(s.fontSize) + s.font +
           (s.bold ? L"B" : L"b") + (panel.ui.dropHover ? L"H" : L"h") +
           (L.vertical ? L"V" : L"H") + std::to_wstring((int)L.edge) + (L.vertScrollLyric ? L"S" : L"s") +
           L"\x1D" + LineKey(L.content.line1) + L"\x1D" + LineKey(L.content.line2);
}

// Repair actual occlusion by taskbar content, rather than fighting every sibling
// for HWND_TOP. In particular the click-through audio visualizer raises itself on
// each render; moving above it every 100 ms creates a continuous z-order loop.
static void EnsureTopChild(Panel& panel) {
    if (!panel.hwnd || !panel.tray) return;
    RECT panelRc{};
    if (!GetWindowRect(panel.hwnd, &panelRc)) return;
    HWND covering = nullptr;
    for (HWND sibling = GetWindow(panel.tray, GW_CHILD);
         sibling && sibling != panel.hwnd; sibling = GetWindow(sibling, GW_HWNDNEXT)) {
        if (!IsWindowVisible(sibling)) continue;
        wchar_t cls[96]{};
        GetClassNameW(sibling, cls, 96);
        // Peer decoration, not the taskbar's opaque XAML surface. Its transparent
        // pixels must not cause us to reorder both layered windows indefinitely.
        if (wcsncmp(cls, L"ZenAudioBars_", 13) == 0) continue;
        RECT siblingRc{}, overlap{};
        if (GetWindowRect(sibling, &siblingRc) && IntersectRect(&overlap, &panelRc, &siblingRc)) {
            covering = sibling;
            break;
        }
    }
    if (!covering) return;
    SetWindowPos(panel.hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    ++panel.ui.zFixes;
    if (panel.ui.zFixes <= 3 || panel.ui.zFixes % 100 == 0) {
        wchar_t cls[96]{};
        GetClassNameW(covering, cls, 96);
        Wh_Log(L"[ui] panel %p raised above overlapping taskbar child %p (%s), count %u", panel.hwnd, covering, cls,
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

    // The 100 ms tick alone would show a new line up to ~100 ms late (more when Explorer's
    // thread is busy). When the next line starts before the tick after next, arm a one-shot
    // timer for that exact moment; re-arming on each tick keeps it aligned with the clock.
    if (L.content.nextLineInMs >= 0 && L.content.nextLineInMs <= 220)
        SetTimer(panel.hwnd, TIMER_LINE, (UINT)std::max(L.content.nextLineInMs, 10), nullptr);

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

// ─────────────────────────────────────────────────────────────────────────────
// 歌词展开面板 (0.5.0). Single click on the collapsed bar opens a popup above it with
// sections: playback, AI status (same data as the bar, listed in detail), 文件中转
// (paths dropped on the collapsed bar), 天气, 资源占用. It never takes focus
// (WS_EX_NOACTIVATE + MA_NOACTIVATE), has no text input, and runs on the taskbar
// thread like the bar. Network and counters run on g_extrasThread, never here.
// The section list is data-driven so later sections can be added the same way.
// ─────────────────────────────────────────────────────────────────────────────
static const wchar_t kExpandClass[] = L"ZenTaskbarLyricsExpand";
static constexpr UINT_PTR TIMER_ANIM = 11;  // fallback only
static constexpr UINT WM_ZEN_ANIMFRAME = WM_APP + 4;
static constexpr int kExpandAnimationFps = 120;
static constexpr UINT_PTR TIMER_WATCH = 12;
static std::atomic<bool> g_expandedAny{false};  // read by the extras thread (sampling gate)

// ── 文件中转: paths only, never copies; in-memory only (lost on explorer/mod reload)
struct RelayEntry {
    std::wstring path;
    long long addedUnix = 0;  // wall-clock seconds since 1970 (UnixNow)
};
static std::mutex g_filesLock;
static std::vector<RelayEntry> g_files;

static void PruneRelayFilesUnlocked(int retentionDays, long long nowUnix) {
    g_files.erase(std::remove_if(g_files.begin(), g_files.end(),
                                 [&](const RelayEntry& e) {
                                     if (retentionDays > 0 && e.addedUnix && nowUnix >= e.addedUnix &&
                                         (nowUnix - e.addedUnix) >= (long long)retentionDays * 86400LL)
                                         return true;
                                     return false;
                                 }),
                  g_files.end());
}

static void PruneRelayFiles() {
    // Slow/disconnected network paths must never hold the UI's list lock.
    const int days = GetSettings().filesRetentionDays;
    const ULONGLONG now = UnixNow();
    std::vector<RelayEntry> snapshot, missing;
    {
        std::lock_guard<std::mutex> lk(g_filesLock);
        snapshot = g_files;
    }
    for (const RelayEntry& e : snapshot)
        if (GetFileAttributesW(e.path.c_str()) == INVALID_FILE_ATTRIBUTES) missing.push_back(e);
    std::lock_guard<std::mutex> lk(g_filesLock);
    PruneRelayFilesUnlocked(days, now);
    g_files.erase(std::remove_if(g_files.begin(), g_files.end(), [&](const RelayEntry& e) {
        return std::any_of(missing.begin(), missing.end(), [&](const RelayEntry& gone) {
            return gone.path == e.path && gone.addedUnix == e.addedUnix;
        });
    }), g_files.end());
}

static void AddRelayFiles(const std::vector<std::wstring>& add) {
    const long long now = UnixNow();
    const int days = GetSettings().filesRetentionDays;
    std::lock_guard<std::mutex> lk(g_filesLock);
    for (auto& p : add) {
        if (p.empty()) continue;
        auto it = std::find_if(g_files.begin(), g_files.end(),
                               [&](const RelayEntry& e) { return e.path == p; });
        if (it != g_files.end()) continue;  // keep original add time
        g_files.insert(g_files.begin(), RelayEntry{p, now});
    }
    PruneRelayFilesUnlocked(days, now);
    if (g_files.size() > 30) g_files.resize(30);
}

static std::vector<std::wstring> GetRelayFiles() {
    // UI reads a memory snapshot; the extras worker validates source paths.
    std::lock_guard<std::mutex> lk(g_filesLock);
    std::vector<std::wstring> out;
    out.reserve(g_files.size());
    for (auto& e : g_files) out.push_back(e.path);
    return out;
}

static void ClearRelayFiles() {
    std::lock_guard<std::mutex> lk(g_filesLock);
    g_files.clear();
}

// ── 天气 (open-meteo, no key) and 资源占用 ──────────────────────────────────
struct Extras {
    std::wstring weather;  // "上海 23°C 多云", empty = none yet
    bool weatherErr = false;
    float cpu = 0, mem = 0, gpu = 0;
    bool gpuKnown = false, resKnown = false;
    double down = 0, up = 0;
};
static std::mutex g_extrasLock;
static Extras g_extras;

static const wchar_t* WmoText(int c) {
    if (c == 0) return L"晴";
    if (c <= 2) return L"多云";
    if (c == 3) return L"阴";
    if (c == 45 || c == 48) return L"雾";
    if (c >= 51 && c <= 57) return L"毛毛雨";
    if (c >= 61 && c <= 67) return L"雨";
    if (c >= 71 && c <= 77) return L"雪";
    if (c >= 80 && c <= 82) return L"阵雨";
    if (c >= 85 && c <= 86) return L"阵雪";
    if (c >= 95) return L"雷雨";
    return L"";
}

struct ResolvedLoc {
    std::string lat, lon;
    std::wstring name;
    const wchar_t* source = L"";  // winrt / ip / city
};

static std::wstring FmtCoord(double v) {
    wchar_t b[32];
    swprintf_s(b, L"%.4f", v);
    return b;
}

static std::wstring ReverseGeocodeName(const std::string& lat, const std::string& lon) {
    // Nominatim over HTTPS; User-Agent is required (kZenUA).
    JValue j;
    std::wstring path = L"/reverse?format=json&zoom=10&accept-language=zh&lat=" + FromUtf8(lat) +
                        L"&lon=" + FromUtf8(lon);
    if (!HttpGetJson(L"nominatim.openstreetmap.org", path, j)) return {};
    const JValue& addr = j["address"];
    for (const char* key : {"city", "town", "village", "municipality", "county", "state"}) {
        std::string s = addr[key].str();
        if (!s.empty()) return FromUtf8(s);
    }
    std::string dn = j["name"].str();
    return dn.empty() ? std::wstring{} : FromUtf8(dn);
}

// Never calls RequestAccessAsync (would prompt). GetGeopositionAsync fails quietly
// when location is off / denied. Runs on the extras worker thread only.
static bool TryWinrtLocation(ResolvedLoc& out) {
    try {
        using namespace winrt::Windows::Devices::Geolocation;
        Geolocator geo;
        geo.DesiredAccuracy(PositionAccuracy::Default);
        auto op = geo.GetGeopositionAsync();
        const ULONGLONG deadline = GetTickCount64() + 10000;
        while (op.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
            if (g_stop.load() || GetTickCount64() >= deadline) {
                op.Cancel();
                return false;
            }
            if (SleepOrStop(100)) {
                op.Cancel();
                return false;
            }
        }
        if (op.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) return false;
        auto pos = op.GetResults();
        auto coord = pos.Coordinate();
        double lat = coord.Latitude();
        double lon = coord.Longitude();
        if (!std::isfinite(lat) || !std::isfinite(lon)) return false;
        out.lat = ToUtf8(FmtCoord(lat));
        out.lon = ToUtf8(FmtCoord(lon));
        out.name = ReverseGeocodeName(out.lat, out.lon);
        if (out.name.empty()) out.name = L"当前位置";
        out.source = L"winrt";
        return true;
    } catch (...) {
        return false;
    }
}

static bool TryIpLocation(ResolvedLoc& out) {
    // ipwho.is: HTTPS, no API key. Sends the public IP to a third party.
    JValue j;
    if (!HttpGetJson(L"ipwho.is", L"/", j)) return false;
    if (j["success"].type == JValue::Bool && !j["success"].b) return false;
    out.lat = j["latitude"].str();
    out.lon = j["longitude"].str();
    if (out.lat.empty() || out.lon.empty()) return false;
    out.name = FromUtf8(j["city"].str());
    if (out.name.empty()) out.name = FromUtf8(j["region"].str());
    if (out.name.empty()) out.name = L"当前位置";
    out.source = L"ip";
    return true;
}

static bool GeocodeCity(const std::wstring& city, ResolvedLoc& out) {
    if (city.empty()) return false;
    JValue g;
    if (!HttpGetJson(L"geocoding-api.open-meteo.com",
                     L"/v1/search?count=1&language=zh&format=json&name=" + UrlEncode(city), g))
        return false;
    const JValue& r = g["results"].at(0);
    out.lat = r["latitude"].str();
    out.lon = r["longitude"].str();
    if (out.lat.empty() || out.lon.empty()) return false;
    std::string n = r["name"].str();
    out.name = n.empty() ? city : FromUtf8(n);
    out.source = L"city";
    return true;
}

static bool ResolveWeatherLocation(const Settings& s, ResolvedLoc& out) {
    if (!s.weatherLocAuto) return GeocodeCity(s.weatherCity, out);
    if (TryWinrtLocation(out)) return true;
    if (s.weatherAllowIp && TryIpLocation(out)) return true;
    if (!s.weatherCity.empty() && GeocodeCity(s.weatherCity, out)) return true;
    return false;
}

static std::wstring FetchWeatherAt(const ResolvedLoc& loc) {
    JValue w;
    if (!HttpGetJson(L"api.open-meteo.com",
                     L"/v1/forecast?current=temperature_2m,weather_code&latitude=" + FromUtf8(loc.lat) +
                         L"&longitude=" + FromUtf8(loc.lon),
                     w))
        return {};
    const JValue& cur = w["current"];
    std::string t = cur["temperature_2m"].str();
    if (t.empty()) return {};
    double tv = strtod(t.c_str(), nullptr);
    wchar_t buf[128];
    swprintf_s(buf, L"%s  %.0f°C  %s", loc.name.c_str(), tv, WmoText((int)cur["weather_code"].num()));
    return buf;
}

// Resource sampling, same approach as the desktop widgets mod (GetSystemTimes,
// GlobalMemoryStatusEx, PDH "GPU Engine" 3D utilization, GetIfTable2 deltas).
struct ResSampler {
    ULONGLONG prevIdle = 0, prevTotal = 0;
    PDH_HQUERY q = nullptr;
    PDH_HCOUNTER c = nullptr;
    bool gpuTried = false;
    ULONGLONG netIn = 0, netOut = 0, netTick = 0;
    bool netHave = false;
    static ULONGLONG Ft(const FILETIME& f) { return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime; }
    void Close() {
        if (q) PdhCloseQuery(q);
        q = nullptr;
        c = nullptr;
        gpuTried = false;
        netHave = false;
        prevTotal = 0;
    }
    void Sample(Extras& e) {
        FILETIME idle, kernel, user;
        if (GetSystemTimes(&idle, &kernel, &user)) {
            ULONGLONG i = Ft(idle), total = Ft(kernel) + Ft(user);
            if (prevTotal && total > prevTotal) {
                double busy = 1.0 - (double)(i - prevIdle) / (double)(total - prevTotal);
                e.cpu = (float)std::clamp(busy * 100.0, 0.0, 100.0);
                e.resKnown = true;
            }
            prevIdle = i;
            prevTotal = total;
        }
        MEMORYSTATUSEX ms{};
        ms.dwLength = sizeof(ms);
        if (GlobalMemoryStatusEx(&ms)) e.mem = (float)ms.dwMemoryLoad;
        if (!gpuTried) {
            gpuTried = true;
            if (PdhOpenQuery(nullptr, 0, &q) != ERROR_SUCCESS) q = nullptr;
            else if (PdhAddEnglishCounterW(q, L"\\GPU Engine(*)\\Utilization Percentage", 0, &c) != ERROR_SUCCESS) {
                PdhCloseQuery(q);
                q = nullptr;
            }
        }
        if (q && PdhCollectQueryData(q) == ERROR_SUCCESS) {
            DWORD bytes = 0, count = 0;
            if (PdhGetFormattedCounterArrayW(c, PDH_FMT_DOUBLE, &bytes, &count, nullptr) == (PDH_STATUS)PDH_MORE_DATA && bytes) {
                std::vector<unsigned char> buf(bytes);
                auto* items = (PDH_FMT_COUNTERVALUE_ITEM_W*)buf.data();
                if (PdhGetFormattedCounterArrayW(c, PDH_FMT_DOUBLE, &bytes, &count, items) == ERROR_SUCCESS) {
                    double best3d = -1, bestAny = -1;
                    for (DWORD k = 0; k < count; k++) {
                        if (items[k].FmtValue.CStatus != ERROR_SUCCESS) continue;
                        double v = items[k].FmtValue.doubleValue;
                        if (v > bestAny) bestAny = v;
                        if (items[k].szName && wcsstr(items[k].szName, L"engtype_3D") && v > best3d) best3d = v;
                    }
                    double pick = best3d >= 0 ? best3d : bestAny;
                    if (pick >= 0) { e.gpu = (float)std::min(pick, 100.0); e.gpuKnown = true; }
                }
            }
        }
        PMIB_IF_TABLE2 table = nullptr;
        if (GetIfTable2(&table) == NO_ERROR && table) {
            ULONGLONG inB = 0, outB = 0;
            for (ULONG k = 0; k < table->NumEntries; k++) {
                const MIB_IF_ROW2& row = table->Table[k];
                if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK || row.Type == IF_TYPE_TUNNEL) continue;
                if (!row.InterfaceAndOperStatusFlags.HardwareInterface || row.OperStatus != IfOperStatusUp) continue;
                inB += row.InOctets;
                outB += row.OutOctets;
            }
            ULONGLONG now = GetTickCount64();
            if (netHave && now > netTick) {
                double dt = (double)(now - netTick) / 1000.0;
                if (inB >= netIn) e.down = (double)(inB - netIn) / dt;
                if (outB >= netOut) e.up = (double)(outB - netOut) / dt;
            }
            netIn = inB;
            netOut = outB;
            netTick = now;
            netHave = true;
        }
        if (table) FreeMibTable(table);
    }
};

static void ExtrasThread() {
    if (!WaitForTaskbarHostRole()) return;
    ResSampler res;
    ResolvedLoc locCache;
    ULONGLONG lastLocAt = 0;       // tick when location was last resolved
    ULONGLONG lastWeather = 0;
    bool weatherFailed = false;
    std::wstring lastWantedKey;
    ULONGLONG lastPrune = 0;
    constexpr ULONGLONG kLocRefreshMs = 6ULL * 3600ULL * 1000ULL;  // 6h
    while (!g_stop) {
        Settings s = GetSettings();
        const bool expanded = g_expandedAny.load();
        bool idleResources = false;
        if (s.idleMode == IdleMode::Resources) {
            {
                std::lock_guard<std::mutex> lk(g_stateLock);
                idleResources = !g_state.hasSession;
            }
            auto ai = s.aiMode != AiMode::Off ? GetAi() : nullptr;
            idleResources = idleResources && (!ai || ai->empty());
        }
        if ((s.xRes && expanded) || idleResources) {
            Extras e;
            { std::lock_guard<std::mutex> lk(g_extrasLock); e = g_extras; }
            res.Sample(e);
            std::lock_guard<std::mutex> lk(g_extrasLock);
            g_extras.cpu = e.cpu; g_extras.mem = e.mem; g_extras.gpu = e.gpu; g_extras.gpuKnown = e.gpuKnown;
            g_extras.down = e.down; g_extras.up = e.up; g_extras.resKnown = e.resKnown;
        } else {
            res.Close();
            std::lock_guard<std::mutex> lk(g_extrasLock);
            g_extras.resKnown = false;
        }
        ULONGLONG now = GetTickCount64();
        // 文件中转: only the background worker checks filesystem paths.
        if (s.xFiles && (lastPrune == 0 || now - lastPrune >= (expanded ? 5000ULL : 60000ULL))) {
            lastPrune = now;
            PruneRelayFiles();
        }
        // 天气: resolve location (cached 6h) then forecast; no city required in auto mode.
        std::wstring wantedKey = std::wstring(s.weatherLocAuto ? L"auto|" : L"manual|") +
                                 (s.weatherAllowIp ? L"ip1|" : L"ip0|") + s.weatherCity;
        const bool needWeather = (s.xWeather || s.idleMode == IdleMode::Weather) &&
            (lastWeather == 0 || wantedKey != lastWantedKey ||
             now - lastWeather >= (weatherFailed ? 60000ULL : (ULONGLONG)s.weatherRefreshMin * 60000ULL));
        if (needWeather) {
            static std::wstring lastLocKey;
            if (wantedKey != lastLocKey || lastLocAt == 0 || now - lastLocAt >= kLocRefreshMs ||
                locCache.lat.empty()) {
                ResolvedLoc loc;
                if (ResolveWeatherLocation(s, loc)) {
                    locCache = loc;
                    Wh_Log(L"[weather] loc %s (%s) %S,%S", loc.name.c_str(), loc.source, loc.lat.c_str(),
                           loc.lon.c_str());
                } else {
                    locCache = {};
                    Wh_Log(L"[weather] location failed");
                }
                lastLocAt = now;
                lastLocKey = wantedKey;
            }
            lastWantedKey = wantedKey;
            std::wstring w;
            if (!locCache.lat.empty()) w = FetchWeatherAt(locCache);
            // Count from completion so a slow failure still waits a full minute.
            lastWeather = GetTickCount64();
            weatherFailed = w.empty();
            std::lock_guard<std::mutex> lk(g_extrasLock);
            if (!w.empty()) {
                g_extras.weather = w;
                g_extras.weatherErr = false;
            } else {
                g_extras.weatherErr = true;
            }
            Wh_Log(L"[weather] forecast -> %s", w.empty() ? L"(failed)" : w.c_str());
        }
        if (SleepOrStop(1000)) break;
    }
    res.Close();
}

static Extras GetExtras() {
    std::lock_guard<std::mutex> lk(g_extrasLock);
    return g_extras;
}

// ── Popup window state (taskbar thread only) ────────────────────────────────
enum ExpandHit { XNone = -1, XPrev, XPlay, XNext, XOpen, XClear, XFile, XSeek };
struct XHit { RECT rc; int id; int index; };

// Own one grow-only GDI surface for the lifetime of a popup. The backing bitmap
// reserves spring overshoot space; the native window still fits the visible card.
struct ExpandSurface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ original = nullptr;
    void* bits = nullptr;
    int width = 0, height = 0;
    ~ExpandSurface() {
        if (dc && original) SelectObject(dc, original);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
    }
    bool Ensure(HDC screen, int w, int h) {
        if (bitmap && w <= width && h <= height) return true;
        w = std::max(w, width); h = std::max(h, height);
        BITMAPINFO info{};
        info.bmiHeader = {sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB};
        void* newBits = nullptr;
        HBITMAP next = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &newBits, nullptr, 0);
        if (!next || !newBits) { if (next) DeleteObject(next); return false; }
        if (!dc) dc = CreateCompatibleDC(screen);
        if (!dc) { DeleteObject(next); return false; }
        HGDIOBJ prev = SelectObject(dc, next);
        if (!original) original = prev;
        if (bitmap) DeleteObject(bitmap);
        bitmap = next; bits = newBits; width = w; height = h;
        return true;
    }
};

// Closed-form damped spring: equal wall-clock time yields the same position,
// even when WM_TIMER delivery is delayed or a frame is skipped.
static void StepExpandSpring(double& position, double& velocity, double target, double elapsed) {
    if (elapsed <= 0 || !std::isfinite(elapsed)) return;
    const double damping = 8.0, frequency = std::sqrt(210.0 - damping * damping);
    double t = std::min(elapsed, 1.0), x = position - target;
    double wave = (velocity + damping * x) / frequency;
    double cs = std::cos(frequency * t), sn = std::sin(frequency * t), decay = std::exp(-damping * t);
    position = target + decay * (x * cs + wave * sn);
    velocity = decay * (-damping * (x * cs + wave * sn) + frequency * (-x * sn + wave * cs));
}

static double AnimationTimeSeconds() {
    static const double frequency = [] { LARGE_INTEGER f{}; QueryPerformanceFrequency(&f); return (double)f.QuadPart; }();
    LARGE_INTEGER now{}; QueryPerformanceCounter(&now);
    return (double)now.QuadPart / frequency;
}

// The worker only schedules frames; all painting stays on Explorer's window thread.
// A high-resolution waitable timer avoids USER_TIMER_MINIMUM and global timer changes.
struct ExpandAnimationClock {
    HANDLE stopEvent = nullptr, timer = nullptr;
    std::thread worker;
    std::atomic<bool> queued{false};
    HWND hwnd = nullptr;
    uintptr_t generation = 0;
    bool running = false;  // window-thread only
    ~ExpandAnimationClock() { Stop(); }
    void Stop() {
        running = false;
        if (stopEvent) SetEvent(stopEvent);
        if (worker.joinable()) worker.join();  // worker never waits on the UI thread
        if (timer) { CancelWaitableTimer(timer); CloseHandle(timer); timer = nullptr; }
        if (stopEvent) { CloseHandle(stopEvent); stopEvent = nullptr; }
        queued = false;
    }
    bool Start(HWND window) {
        Stop();
        static std::atomic<uintptr_t> serial{0};
        generation = ++serial; hwnd = window;
        stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        timer = CreateWaitableTimerExW(nullptr, nullptr, 0x00000002 /* HIGH_RESOLUTION */, TIMER_ALL_ACCESS);
        if (!timer) timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
        if (!timer || !stopEvent) { Stop(); return false; }
        running = true;
        try {
            worker = std::thread([this] {
                using Clock = std::chrono::steady_clock;
                const auto period = std::chrono::nanoseconds(1000000000LL / kExpandAnimationFps);
                auto deadline = Clock::now() + period;
                HANDLE waits[2] = {stopEvent, timer};
                for (;;) {
                    auto remaining = std::chrono::duration_cast<std::chrono::nanoseconds>(deadline - Clock::now()).count();
                    LARGE_INTEGER due{}; due.QuadPart = -std::max(1LL, (remaining + 99) / 100);
                    if (!SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE)) break;
                    if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) != WAIT_OBJECT_0 + 1) break;
                    if (!queued.exchange(true) && !PostMessageW(hwnd, WM_ZEN_ANIMFRAME, generation, 0)) queued = false;
                    deadline += period;
                    auto now = Clock::now();
                    if (deadline <= now) deadline = now + period;  // skip missed frames, never catch up in a burst
                }
            });
        } catch (...) { Stop(); return false; }
        return true;
    }
};

struct ExpandState {
    HWND hwnd = nullptr;
    HWND bar = nullptr;         // collapsed bar it belongs to
    double p = 0, v = 0;        // spring position (0 closed .. 1 open) and velocity
    double target = 0;
    int w = 0, h = 0;           // full size in px
    TaskbarEdge edge = TaskbarEdge::Bottom;
    int anchorX = 0, anchorY = 0;  // fixed edge of the popup against the bar (screen)
    ULONGLONG lastInteract = 0;
    double lastAnimationTime = 0;
    std::unique_ptr<ExpandAnimationClock> animationClock;
    bool lbuttonWasDown = false;
    int hover = XNone, pressed = XNone, pressedIndex = -1;
    POINT pressPt{};
    std::vector<XHit> hits;
    std::vector<XHit> contentHits;
    std::unique_ptr<ExpandSurface> surface;
    std::unique_ptr<Gdiplus::Bitmap> content;
    int contentDpi = 0;
    bool contentLight = false;
    uint64_t contentSettingsGen = 0;
    std::wstring seekTrack, seekApp;
    std::unique_ptr<Gdiplus::Bitmap> art;
    unsigned artVersion = ~0u;
};
static ExpandState g_x;  // one popup at a time (the bar that was clicked)

static std::wstring FmtRate(double bps) {
    wchar_t b[32];
    if (bps >= 1024.0 * 1024.0) swprintf_s(b, L"%.1f MB/s", bps / 1048576.0);
    else swprintf_s(b, L"%.0f KB/s", bps / 1024.0);
    return b;
}

static std::wstring WeatherText(const Extras& ex, const Settings& s) {
    if (!ex.weather.empty()) return ex.weather;
    if (!s.weatherLocAuto && s.weatherCity.empty()) return L"在设置里填写城市";
    return ex.weatherErr ? L"天气获取失败，1 分钟后重试" : L"正在获取天气…";
}

static std::wstring ResourceText(const Extras& ex) {
    if (!ex.resKnown) return L"正在采样资源占用…";
    wchar_t line[160], gpu[16] = L"—";
    if (ex.gpuKnown) swprintf_s(gpu, L"%.0f%%", ex.gpu);
    swprintf_s(line, L"CPU %.0f%%   内存 %.0f%%   GPU %s", ex.cpu, ex.mem, gpu);
    return line;
}

static std::wstring NetworkText(const Extras& ex) {
    return L"↓ " + FmtRate(ex.down) + L"    ↑ " + FmtRate(ex.up);
}

static void SetIdleContent(Content& c, const Settings& s) {
    c.idle = true;
    if (s.idleMode == IdleMode::Message) {
        c.line1 = TextLine(L"♪ 未在播放");
        return;
    }
    Extras ex = GetExtras();
    if (s.idleMode == IdleMode::Weather) {
        c.line1 = TextLine(WeatherText(ex, s));
    } else {
        c.line1 = TextLine(ResourceText(ex));
        if (ex.resKnown) c.line2 = TextLine(NetworkText(ex));
    }
}
static std::wstring FmtTime(long long ms) {
    if (ms < 0) ms = 0;
    wchar_t b[16];
    swprintf_s(b, L"%lld:%02lld", ms / 60000, (ms / 1000) % 60);
    return b;
}

// Builds the content and computes g_x.w / g_x.h; paints when g != nullptr.
// Text colors / shadow follow the mini panel (textColorMode + light/dark).
static void ExpandLayoutPaint(Gdiplus::Graphics* g, int dpi, bool light) {
    const Settings& s = GetSettings();
    auto D = [&](double v) { return (Gdiplus::REAL)(v * dpi / 96.0); };
    const Gdiplus::ARGB text = ResolveTextColor(s.textColorMode, light);
    Gdiplus::ARGB shadow = 0;
    if (IsLightColor(text) == light) shadow = IsLightColor(text) ? 0x80000000 : 0x70FFFFFF;
    const Gdiplus::ARGB dim = WithAlpha(text, 0xA0);
    const Gdiplus::ARGB hoverFill = light ? 0x18000000 : 0x24FFFFFF;
    const Gdiplus::REAL W = D(s.expandWidth), pad = D(14);
    Gdiplus::REAL y = pad;
    g_x.hits.clear();
    g_x.seekTrack.clear();
    g_x.seekApp.clear();
    Gdiplus::FontFamily fam(s.font.c_str());
    Gdiplus::Font fTitle(&fam, D(14), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font fBody(&fam, D(12.5), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font fSmall(&fam, D(11), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::StringFormat sf(Gdiplus::StringFormatFlagsNoWrap);
    sf.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    const Gdiplus::REAL shadowOff = D(1);
    auto Text = [&](const std::wstring& t, const Gdiplus::Font& f, Gdiplus::ARGB c, Gdiplus::RectF r) {
        if (!g || t.empty()) return;
        if (shadow) {
            Gdiplus::SolidBrush sh{Gdiplus::Color(WithAlpha(shadow, (BYTE)(((shadow >> 24) & 255) * ((c >> 24) & 255) / 255)))};
            Gdiplus::RectF ds(r.X, r.Y + shadowOff, r.Width, r.Height);
            g->DrawString(t.c_str(), (INT)t.size(), &f, ds, &sf, &sh);
        }
        Gdiplus::SolidBrush b{Gdiplus::Color(c)};
        g->DrawString(t.c_str(), (INT)t.size(), &f, r, &sf, &b);
    };
    auto Header = [&](const wchar_t* t) {
        Text(t, fSmall, dim, Gdiplus::RectF(pad, y, W - 2 * pad, D(18)));
        y += D(20);
    };
    auto AddHit = [&](Gdiplus::RectF r, int id, int index) {
        g_x.hits.push_back({RECT{(LONG)r.X, (LONG)r.Y, (LONG)(r.X + r.Width), (LONG)(r.Y + r.Height)}, id, index});
        if (g && g_x.hover == id) FillRound(*g, hoverFill, r, D(6));
    };
    bool first = true;
    auto Sep = [&] {
        if (!first) {
            if (g) FillRound(*g, WithAlpha(text, 0x20), Gdiplus::RectF(pad, y + D(4), W - 2 * pad, D(1)), 0);
            y += D(10);
        }
        first = false;
    };

    // (a) Playback exists only while there is a media session. Use the same
    // snapshot for visibility, layout and painting; no stale metadata in idle mode.
    PlaybackState st;
    if (s.xMedia) {
        std::lock_guard<std::mutex> lk(g_stateLock);
        st = g_state;
        st.lyrics = nullptr;
    }
    if (!st.hasSession || !st.thumb) {
        g_x.art.reset();
        g_x.artVersion = 0;
    }
    if (s.xMedia && st.hasSession) {
        Sep();
        const Gdiplus::REAL art = D(72);
        if (st.thumb && g && g_x.artVersion != st.thumbVersion) {
            g_x.art = DecodeArt(*st.thumb, (int)art, D(10));
            g_x.artVersion = st.thumbVersion;
        }
        if (g) {
            if (g_x.art) g->DrawImage(g_x.art.get(), pad, y, art, art);
            else FillRound(*g, WithAlpha(text, 0x18), Gdiplus::RectF(pad, y, art, art), D(10));
        }
        Gdiplus::REAL tx = pad + art + D(12), tw = W - tx - pad;
        Text(st.hasSession ? st.title : std::wstring(L"未在播放"), fTitle, text, Gdiplus::RectF(tx, y, tw, D(22)));
        Text(st.artist, fBody, dim, Gdiplus::RectF(tx, y + D(22), tw, D(18)));
        // progress
        long long pos = st.positionMs;
        if (st.playing && st.sampledAtTick)
            pos += (long long)((double)(GetTickCount64() - st.sampledAtTick) * st.rate);
        if (st.durationMs > 0) {
            pos = std::clamp(pos, 0LL, st.durationMs);
            Gdiplus::REAL by = y + D(46), bw = tw;
            if (st.canSeek) {
                AddHit(Gdiplus::RectF(tx, by - D(6), bw, D(16)), XSeek, 0);
                g_x.seekTrack = st.trackKey;
                g_x.seekApp = st.sourceApp;
            }
            if (g) {
                FillRound(*g, WithAlpha(text, 0x30), Gdiplus::RectF(tx, by, bw, D(4)), D(2));
                FillRound(*g, text, Gdiplus::RectF(tx, by, bw * (Gdiplus::REAL)pos / (Gdiplus::REAL)st.durationMs, D(4)), D(2));
            }
            Text(FmtTime(pos) + L" / " + FmtTime(st.durationMs), fSmall, dim, Gdiplus::RectF(tx, by + D(6), tw, D(16)));
        }
        y += art + D(8);
        // buttons
        const Gdiplus::REAL bs = D(34);
        Gdiplus::REAL bx = pad;
        int ids[3] = {XPrev, XPlay, XNext};
        bool en[3] = {st.canPrev, st.canPlayPause, st.canNext};
        for (int i = 0; i < 3; ++i) {
            Gdiplus::RectF r(bx, y, bs, bs);
            if (st.hasSession && en[i]) AddHit(r, ids[i], 0);
            if (g) DrawIcon(*g, i, st.playing, bx + bs / 2, y + bs / 2, D(14), st.hasSession && en[i] ? text : WithAlpha(text, 0x50));
            bx += bs + D(6);
        }
        if (st.hasSession) {
            Gdiplus::RectF r(W - pad - D(110), y + D(3), D(110), bs - D(6));
            if (g) FillRound(*g, WithAlpha(text, 0x18), r, D(8));
            AddHit(r, XOpen, 0);
            Gdiplus::StringFormat c(Gdiplus::StringFormatFlagsNoWrap);
            c.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            c.SetAlignment(Gdiplus::StringAlignmentCenter);
            if (g) {
                Gdiplus::SolidBrush b{Gdiplus::Color(text)};
                g->DrawString(L"打开播放器", -1, &fBody, r, &c, &b);
            }
        }
        y += bs + D(4);
    }

    // (b) AI状态: the same list the bar shows, in detail.
    if (s.xAi && s.aiMode != AiMode::Off) {
        auto ai = GetAi();
        if (ai && !ai->empty()) {
            Sep();
            Header(L"AI 状态");
            size_t n = std::min<size_t>(ai->size(), 6);
            for (size_t i = 0; i < n; ++i) {
                const AiEntry& e = (*ai)[i];
                if (g) {
                    Gdiplus::SolidBrush dot{Gdiplus::Color(AiStateColor(e.state))};
                    g->FillEllipse(&dot, pad, y + D(6), D(8), D(8));
                }
                std::wstring head = e.name + L"  " + AiStateLabel(e.state);
                if (!e.project.empty()) head += L"  · " + e.project;
                Text(head, fBody, text, Gdiplus::RectF(pad + D(14), y, W - 2 * pad - D(14), D(20)));
                y += D(20);
                std::wstring msg = e.message.empty() ? e.detail : e.message;
                if (!msg.empty()) {
                    Text(msg, fSmall, dim, Gdiplus::RectF(pad + D(14), y, W - 2 * pad - D(14), D(16)));
                    y += D(18);
                }
            }
        }
    }

    // (c) 文件中转
    if (s.xFiles) {
        auto files = GetRelayFiles();
        Sep();
        Header(L"文件中转");
        if (!files.empty()) {
            Gdiplus::RectF clr(W - pad - D(44), y - D(20), D(44), D(18));
            AddHit(clr, XClear, 0);
            Text(L"清空", fSmall, dim, clr);
        }
        if (files.empty()) {
            Text(L"把文件拖到任务栏的歌词条上，再从这里复制拖出", fSmall, dim, Gdiplus::RectF(pad, y, W - 2 * pad, D(18)));
            y += D(20);
        }
        for (size_t i = 0; i < std::min<size_t>(files.size(), 6); ++i) {
            const std::wstring& f = files[i];
            const wchar_t* name = wcsrchr(f.c_str(), L'\\');
            Gdiplus::RectF r(pad - D(4), y, W - 2 * pad + D(8), D(22));
            g_x.hits.push_back({RECT{(LONG)r.X, (LONG)r.Y, (LONG)(r.X + r.Width), (LONG)(r.Y + r.Height)}, XFile, (int)i});
            if (g && g_x.hover == XFile && g_x.pressedIndex == (int)i) FillRound(*g, hoverFill, r, D(6));
            Text(name ? name + 1 : f.c_str(), fBody, text, Gdiplus::RectF(pad, y, W - 2 * pad, D(22)));
            y += D(22);
        }
        if (files.size() > 6) {
            Text(L"…另有 " + std::to_wstring(files.size() - 6) + L" 个", fSmall, dim, Gdiplus::RectF(pad, y, W - 2 * pad, D(16)));
            y += D(18);
        }
    }

    // (d) 天气 and (e) 资源占用
    Extras ex = GetExtras();
    if (s.xWeather) {
        Sep();
        Header(L"天气");
        Text(WeatherText(ex, s), fBody, text, Gdiplus::RectF(pad, y, W - 2 * pad, D(20)));
        y += D(22);
    }
    if (s.xRes) {
        Sep();
        Header(L"资源占用");
        Text(ResourceText(ex), fBody, text, Gdiplus::RectF(pad, y, W - 2 * pad, D(20)));
        y += D(20);
        if (ex.resKnown) {
            Text(NetworkText(ex), fBody, dim, Gdiplus::RectF(pad, y, W - 2 * pad, D(20)));
            y += D(20);
        }
    }
    y += pad - D(4);
    g_x.w = (int)W;
    g_x.h = (int)y;
}

// ---------------------------------------------------------------------------
// Expand-panel liquid glass (0.5.2). Own code; approach matches
// optional/local@zen-liquid-glass.wh.cpp (WGC + D3D11 blur/refraction/chroma/rim,
// Briefly exclude the popup while taking a clean backdrop snapshot. Release
// capture and restore screenshot visibility before presenting the glass.
namespace xlg {

struct SizeInt32 { INT32 Width; INT32 Height; };

MIDL_INTERFACE("3628E81B-3CAC-4C60-B7F4-23CE0E0C3356")
ICaptureItemInterop : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateForWindow(HWND, REFIID, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateForMonitor(HMONITOR, REFIID, void**) = 0;
};
MIDL_INTERFACE("79C3F95B-31F7-4EC2-A464-632EF5D30760")
ICaptureItem : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Size(SizeInt32*) = 0;
};
MIDL_INTERFACE("814E42A9-F70F-4AD7-939B-FDDCC6EB880D")
ICaptureSession : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE StartCapture() = 0;
};
MIDL_INTERFACE("2C39AE40-7D2E-5044-804E-8B6799D4CF9E")
ICaptureSession2 : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_IsCursorCaptureEnabled(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsCursorCaptureEnabled(boolean) = 0;
};
MIDL_INTERFACE("F2CDD966-22AE-5EA1-9596-3A289344C3BE")
ICaptureSession3 : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_IsBorderRequired(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsBorderRequired(boolean) = 0;
};
MIDL_INTERFACE("FA50C623-38DA-4B32-ACF3-FA9734AD800E")
ICaptureFrame : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Surface(IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SystemRelativeTime(INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentSize(SizeInt32*) = 0;
};
MIDL_INTERFACE("24EB6D22-1975-422E-82E7-780DBD8DDF24")
ICaptureFramePool : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Recreate(IInspectable*, INT32, INT32, SizeInt32) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetNextFrame(ICaptureFrame**) = 0;
    virtual HRESULT STDMETHODCALLTYPE add_FrameArrived(IUnknown*, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE remove_FrameArrived(INT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCaptureSession(ICaptureItem*, ICaptureSession**) = 0;
};
MIDL_INTERFACE("589B103F-6BBC-5DF5-A991-02E28B3B66D5")
ICaptureFramePoolStatics2 : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE CreateFreeThreaded(IInspectable*, INT32, INT32, SizeInt32, ICaptureFramePool**) = 0;
};
MIDL_INTERFACE("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1")
IDxgiInterfaceAccess : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetInterface(REFIID, void**) = 0;
};
MIDL_INTERFACE("30D5A829-7FA4-4026-83BB-D75BAE4EA99E")
IClosable : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Close() = 0;
};

}  // namespace xlg

// MinGW emulates __uuidof with template specializations. MIDL_INTERFACE alone
// does not register UUIDs; WRL ComPtr::As also needs these global declarations.
__CRT_UUID_DECL(xlg::ICaptureItemInterop, 0x3628E81B, 0x3CAC, 0x4C60, 0xB7, 0xF4, 0x23, 0xCE, 0x0E, 0x0C, 0x33, 0x56)
__CRT_UUID_DECL(xlg::ICaptureItem, 0x79C3F95B, 0x31F7, 0x4EC2, 0xA4, 0x64, 0x63, 0x2E, 0xF5, 0xD3, 0x07, 0x60)
__CRT_UUID_DECL(xlg::ICaptureSession, 0x814E42A9, 0xF70F, 0x4AD7, 0x93, 0x9B, 0xFD, 0xDC, 0xC6, 0xEB, 0x88, 0x0D)
__CRT_UUID_DECL(xlg::ICaptureSession2, 0x2C39AE40, 0x7D2E, 0x5044, 0x80, 0x4E, 0x8B, 0x67, 0x99, 0xD4, 0xCF, 0x9E)
__CRT_UUID_DECL(xlg::ICaptureSession3, 0xF2CDD966, 0x22AE, 0x5EA1, 0x95, 0x96, 0x3A, 0x28, 0x93, 0x44, 0xC3, 0xBE)
__CRT_UUID_DECL(xlg::ICaptureFrame, 0xFA50C623, 0x38DA, 0x4B32, 0xAC, 0xF3, 0xFA, 0x97, 0x34, 0xAD, 0x80, 0x0E)
__CRT_UUID_DECL(xlg::ICaptureFramePool, 0x24EB6D22, 0x1975, 0x422E, 0x82, 0xE7, 0x78, 0x0D, 0xBD, 0x8D, 0xDF, 0x24)
__CRT_UUID_DECL(xlg::ICaptureFramePoolStatics2, 0x589B103F, 0x6BBC, 0x5DF5, 0xA9, 0x91, 0x02, 0xE2, 0x8B, 0x3B, 0x66, 0xD5)
__CRT_UUID_DECL(xlg::IDxgiInterfaceAccess, 0xA9B3D012, 0x3DF2, 0x4EE3, 0xB8, 0xD1, 0x86, 0x95, 0xF4, 0x57, 0xD3, 0xC1)
__CRT_UUID_DECL(xlg::IClosable, 0x30D5A829, 0x7FA4, 0x4026, 0x83, 0xBB, 0xD7, 0x5B, 0xAE, 0x4E, 0xA9, 0x9E)

namespace xlg {

constexpr INT32 kPixelFormatB8G8R8A8 = 87;
// The card spring runs on a 15 ms tick; a lower glass cap made the backdrop lag
// behind the card and read as a flicker on 120 Hz displays.
constexpr int kMaxFps = 120;
constexpr float kBlur = 12.f, kRefract = 18.f, kChroma = 0.15f, kRim = 0.60f, kTint = 0.08f;

const char kShader[] = R"(
cbuffer P : register(b0) {
    float2 size; float2 srcOrigin;
    float2 srcSize; float radius; float blur;
    float refract; float chroma; float rim; float tint;
    float2 cardMin; float2 cardSize;
};
Texture2D tex : register(t0);
SamplerState smp : register(s0);
struct V { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
V vs(uint id : SV_VertexID) {
    V o; float2 uv = float2((id << 1) & 2, id & 2);
    o.pos = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1); o.uv = uv; return o;
}
float sdRR(float2 p, float2 b, float r) {
    float2 q = abs(p) - b + r;
    return length(max(q, 0)) + min(max(q.x, q.y), 0) - r;
}
float3 blurAt(float2 px) {
    float3 acc = 0; float ws = 0;
    [unroll] for (int i = -2; i <= 2; i++)
    [unroll] for (int j = -2; j <= 2; j++) {
        float w = exp(-(i * i + j * j) / 4.5);
        float2 o = float2(i, j) * (blur * 0.5);
        acc += tex.SampleLevel(smp, (srcOrigin + px + o) / srcSize, 0).rgb * w;
        ws += w;
    }
    return acc / ws;
}
float4 ps(V i) : SV_Target {
    float2 px = i.uv * size;
    // The card occupies a sub-rect of a window that never moves while the
    // expand animation runs, so the backdrop stays pinned to the screen.
    float2 c = cardMin + cardSize * 0.5;
    float2 b = cardSize * 0.5;
    float2 p = px - c;
    float r = min(radius, min(b.x, b.y));
    float d = sdRR(p, b, r);
    float a = saturate(0.5 - d);
    if (a <= 0) return 0;
    float e = 0.5;
    float2 g = float2(sdRR(p + float2(e, 0), b, r) - sdRR(p - float2(e, 0), b, r),
                      sdRR(p + float2(0, e), b, r) - sdRR(p - float2(0, e), b, r));
    float2 n = g / max(length(g), 1e-4);
    // Keep bending inside the rounded bevel. The old 40%-of-card band reached
    // the SDF medial axes, where the nearest edge switches abruptly (top/side).
    // Refracting across that normal discontinuity produces triangular creases.
    float band = max(r, 1.0);
    float k = saturate(1.0 + d / band);
    float2 off = -n * (k * k * refract);
    float3 col;
    col.r = blurAt(px + off * (1.0 + chroma)).r;
    col.g = blurAt(px + off).g;
    col.b = blurAt(px + off * (1.0 - chroma)).b;
    col = lerp(col, float3(1, 1, 1), tint);
    float3 L = normalize(float3(-0.35, -0.85, 0.4));
    float spec = pow(saturate(dot(normalize(float3(n, 0.55)), L)), 10.0) * k * rim;
    col += spec;
    return float4(saturate(col) * a, a);
}
)";

struct alignas(16) ShaderParams {
    float size[2], srcOrigin[2];
    float srcSize[2], radius, blur;
    float refract, chroma, rim, tint;
    float cardMin[2], cardSize[2];
};

using Microsoft::WRL::ComPtr;

struct Glass {
    HWND hwnd = nullptr;
    HWND expand = nullptr;
    RECT target = {};          // whole window in screen space; fixed while open
    RECT monRect = {};
    HMONITOR monitor = nullptr;
    float cornerRadius = 14.f;
    float cardL = 0, cardT = 0, cardW = 0, cardH = 0;  // visible card inside the window
    ComPtr<ID3D11Device> dev;
    ComPtr<ID3D11DeviceContext> ctx;
    ComPtr<IDXGISwapChain1> swap;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<IDCompositionDevice> dcomp;
    ComPtr<IDCompositionTarget> dcompTarget;
    ComPtr<IDCompositionVisual> visual;
    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    ComPtr<ID3D11Buffer> cb;
    ComPtr<ID3D11SamplerState> sampler;
    ComPtr<ID3D11Texture2D> src;
    ComPtr<ID3D11ShaderResourceView> srv;
    D3D11_BOX srcBox = {};
    ComPtr<IInspectable> winrtDevice;
    ComPtr<ICaptureItem> item;
    ComPtr<ICaptureFramePool> pool;
    ComPtr<ICaptureSession> session;
    SizeInt32 poolSize = {};
    int width = 0, height = 0;
    bool visible = false;
    bool hasContent = false;
};

std::atomic<bool> g_stop{false};
std::atomic<bool> g_deviceLost{false};
std::atomic<bool> g_live{false};       // glass currently presenting
std::atomic<bool> g_hasFrame{false};   // a backdrop frame is cached for this monitor
std::atomic<bool> g_want{false};       // expand open + setting on
HANDLE g_thread = nullptr;
HANDLE g_wake = nullptr;
std::mutex g_geomLock;
RECT g_geomRect{};
RECT g_geomCard{};   // visible card, relative to the window
HMONITOR g_geomMon = nullptr;
float g_geomRadius = 14.f;
HWND g_expandHwnd = nullptr;
uint64_t g_openGeneration = 0;
DWORD g_expandPrevAffinity = WDA_NONE;
bool g_expandAffinitySet = false;

template <typename T>
HRESULT GetFactory(PCWSTR cls, REFIID iid, T** out) {
    HSTRING h = nullptr;
    HRESULT hr = WindowsCreateString(cls, (UINT32)wcslen(cls), &h);
    if (FAILED(hr)) return hr;
    hr = RoGetActivationFactory(h, iid, (void**)out);
    WindowsDeleteString(h);
    return hr;
}

bool CompileShaders(Glass& g) {
    ComPtr<ID3DBlob> vsb, psb, err;
    if (FAILED(D3DCompile(kShader, sizeof(kShader) - 1, "xlg", nullptr, nullptr,
                          "vs", "vs_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsb, &err)) ||
        FAILED(D3DCompile(kShader, sizeof(kShader) - 1, "xlg", nullptr, nullptr,
                          "ps", "ps_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psb, &err))) {
        Wh_Log(L"[xlg] shader compile failed: %S",
               err ? (const char*)err->GetBufferPointer() : "?");
        return false;
    }
    return SUCCEEDED(g.dev->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &g.vs)) &&
           SUCCEEDED(g.dev->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &g.ps));
}

bool CreateDevice(Glass& g) {
    D3D_FEATURE_LEVEL fl[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                 D3D11_CREATE_DEVICE_BGRA_SUPPORT, fl, (UINT)ARRAYSIZE(fl),
                                 D3D11_SDK_VERSION, &g.dev, nullptr, &g.ctx))) {
        return false;
    }
    if (!CompileShaders(g)) return false;
    D3D11_BUFFER_DESC bd = {sizeof(ShaderParams), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER};
    if (FAILED(g.dev->CreateBuffer(&bd, nullptr, &g.cb))) return false;
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    if (FAILED(g.dev->CreateSamplerState(&sd, &g.sampler))) return false;
    ComPtr<IDXGIDevice> dxgi;
    g.dev.As(&dxgi);
    if (FAILED(DCompositionCreateDevice(dxgi.Get(), IID_PPV_ARGS(&g.dcomp)))) return false;
    if (FAILED(g.dcomp->CreateTargetForHwnd(g.hwnd, TRUE, &g.dcompTarget))) return false;
    if (FAILED(g.dcomp->CreateVisual(&g.visual))) return false;
    using Create_t = HRESULT(WINAPI*)(IDXGIDevice*, IInspectable**);
    auto create = reinterpret_cast<Create_t>(
        GetProcAddress(GetModuleHandleW(L"d3d11.dll"), "CreateDirect3D11DeviceFromDXGIDevice"));
    return create && SUCCEEDED(create(dxgi.Get(), &g.winrtDevice));
}

bool EnsureSwapChain(Glass& g, int w, int h) {
    if (g.swap && w == g.width && h == g.height) return true;
    g.rtv.Reset();
    if (g.swap) {
        if (FAILED(g.swap->ResizeBuffers(2, (UINT)w, (UINT)h, DXGI_FORMAT_B8G8R8A8_UNORM, 0))) return false;
    } else {
        ComPtr<IDXGIDevice> dxgi;
        ComPtr<IDXGIAdapter> adapter;
        ComPtr<IDXGIFactory2> factory;
        g.dev.As(&dxgi);
        dxgi->GetAdapter(&adapter);
        adapter->GetParent(IID_PPV_ARGS(&factory));
        DXGI_SWAP_CHAIN_DESC1 d = {};
        d.Width = (UINT)w;
        d.Height = (UINT)h;
        d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        d.SampleDesc.Count = 1;
        d.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        d.BufferCount = 2;
        d.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        d.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        if (FAILED(factory->CreateSwapChainForComposition(g.dev.Get(), &d, nullptr, &g.swap))) return false;
        g.visual->SetContent(g.swap.Get());
        g.dcompTarget->SetRoot(g.visual.Get());
        g.dcomp->Commit();
    }
    ComPtr<ID3D11Texture2D> back;
    g.swap->GetBuffer(0, IID_PPV_ARGS(&back));
    if (FAILED(g.dev->CreateRenderTargetView(back.Get(), nullptr, &g.rtv))) return false;
    g.width = w;
    g.height = h;
    return true;
}

void StopCapture(Glass& g) {
    ComPtr<IClosable> c;
    if (g.session && SUCCEEDED(g.session.As(&c))) c->Close();
    c.Reset();
    if (g.pool && SUCCEEDED(g.pool.As(&c))) c->Close();
    g.session.Reset();
    g.pool.Reset();
    g.item.Reset();
    g.monitor = nullptr;
}

bool StartCapture(Glass& g, HMONITOR mon) {
    StopCapture(g);
    ComPtr<ICaptureItemInterop> interop;
    if (FAILED(GetFactory(L"Windows.Graphics.Capture.GraphicsCaptureItem",
                          __uuidof(ICaptureItemInterop), interop.GetAddressOf())) ||
        FAILED(interop->CreateForMonitor(mon, __uuidof(ICaptureItem), (void**)g.item.GetAddressOf()))) {
        Wh_Log(L"[xlg] CreateForMonitor failed");
        return false;
    }
    g.item->get_Size(&g.poolSize);
    ComPtr<ICaptureFramePoolStatics2> statics;
    if (FAILED(GetFactory(L"Windows.Graphics.Capture.Direct3D11CaptureFramePool",
                          __uuidof(ICaptureFramePoolStatics2), statics.GetAddressOf())) ||
        FAILED(statics->CreateFreeThreaded(g.winrtDevice.Get(), kPixelFormatB8G8R8A8, 2,
                                           g.poolSize, &g.pool)) ||
        FAILED(g.pool->CreateCaptureSession(g.item.Get(), &g.session))) {
        Wh_Log(L"[xlg] frame pool / session failed");
        StopCapture(g);
        return false;
    }
    ComPtr<ICaptureSession2> s2;
    if (SUCCEEDED(g.session.As(&s2))) s2->put_IsCursorCaptureEnabled(false);
    ComPtr<ICaptureSession3> s3;
    if (SUCCEEDED(g.session.As(&s3))) s3->put_IsBorderRequired(false);
    if (FAILED(g.session->StartCapture())) {
        StopCapture(g);
        return false;
    }
    g.monitor = mon;
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    g.monRect = mi.rcMonitor;
    return true;
}

bool PullFrame(Glass& g) {
    ComPtr<ICaptureFrame> frame;
    if (FAILED(g.pool->TryGetNextFrame(&frame)) || !frame) return false;
    SizeInt32 cs = {};
    frame->get_ContentSize(&cs);
    if (cs.Width != g.poolSize.Width || cs.Height != g.poolSize.Height) {
        g.poolSize = cs;
        g.pool->Recreate(g.winrtDevice.Get(), kPixelFormatB8G8R8A8, 2, cs);
        return false;
    }
    ComPtr<IInspectable> surface;
    ComPtr<IDxgiInterfaceAccess> access;
    ComPtr<ID3D11Texture2D> tex;
    if (FAILED(frame->get_Surface(&surface)) || FAILED(surface.As(&access)) ||
        FAILED(access->GetInterface(IID_PPV_ARGS(&tex)))) {
        return false;
    }
    // Cache the monitor frame once so the popup can grow or move over it.
    if (cs.Width <= 0 || cs.Height <= 0) return false;
    UINT w = (UINT)cs.Width, h = (UINT)cs.Height;
    D3D11_TEXTURE2D_DESC have = {};
    if (g.src) g.src->GetDesc(&have);
    if (!g.src || have.Width != w || have.Height != h) {
        g.src.Reset();
        g.srv.Reset();
        D3D11_TEXTURE2D_DESC d = {};
        d.Width = w;
        d.Height = h;
        d.MipLevels = d.ArraySize = 1;
        d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        d.SampleDesc.Count = 1;
        d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        if (FAILED(g.dev->CreateTexture2D(&d, nullptr, &g.src)) ||
            FAILED(g.dev->CreateShaderResourceView(g.src.Get(), nullptr, &g.srv))) {
            return false;
        }
    }
    g.srcBox = {0, 0, 0, w, h, 1};
    g.ctx->CopySubresourceRegion(g.src.Get(), 0, 0, 0, 0, tex.Get(), 0, &g.srcBox);
    g.hasContent = true;
    g_hasFrame = true;
    return true;
}

HRESULT Render(Glass& g) {
    ShaderParams p = {};
    p.size[0] = (float)g.width;
    p.size[1] = (float)g.height;
    p.srcOrigin[0] = (float)(g.target.left - g.monRect.left - (LONG)g.srcBox.left);
    p.srcOrigin[1] = (float)(g.target.top - g.monRect.top - (LONG)g.srcBox.top);
    p.srcSize[0] = (float)(g.srcBox.right - g.srcBox.left);
    p.srcSize[1] = (float)(g.srcBox.bottom - g.srcBox.top);
    p.cardMin[0] = g.cardL;
    p.cardMin[1] = g.cardT;
    p.cardSize[0] = g.cardW > 0 ? g.cardW : (float)g.width;
    p.cardSize[1] = g.cardH > 0 ? g.cardH : (float)g.height;
    p.radius = g.cornerRadius;
    const Settings& settings = GetSettings();
    p.blur = (float)settings.glassBlur;
    p.refract = settings.expandLiquidGlass ? kRefract : 0.f;
    p.chroma = settings.expandLiquidGlass ? kChroma : 0.f;
    p.rim = settings.expandLiquidGlass ? kRim : 0.f;
    p.tint = 0;  // color and brightness are applied once by the popup material
    g.ctx->UpdateSubresource(g.cb.Get(), 0, nullptr, &p, 0, 0);
    D3D11_VIEWPORT vp = {0, 0, (float)g.width, (float)g.height, 0, 1};
    float clear[4] = {0, 0, 0, 0};
    ID3D11RenderTargetView* rtv = g.rtv.Get();
    g.ctx->ClearRenderTargetView(rtv, clear);
    g.ctx->OMSetRenderTargets(1, &rtv, nullptr);
    g.ctx->RSSetViewports(1, &vp);
    g.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g.ctx->IASetInputLayout(nullptr);
    g.ctx->VSSetShader(g.vs.Get(), nullptr, 0);
    g.ctx->PSSetShader(g.ps.Get(), nullptr, 0);
    ID3D11Buffer* cb = g.cb.Get();
    g.ctx->PSSetConstantBuffers(0, 1, &cb);
    ID3D11ShaderResourceView* srv = g.srv.Get();
    g.ctx->PSSetShaderResources(0, 1, &srv);
    ID3D11SamplerState* s = g.sampler.Get();
    g.ctx->PSSetSamplers(0, 1, &s);
    g.ctx->Draw(3, 0);
    return g.swap->Present(0, 0);
}

bool ShouldPause() {
    QUERY_USER_NOTIFICATION_STATE st;
    if (SUCCEEDED(SHQueryUserNotificationState(&st)) &&
        (st == QUNS_RUNNING_D3D_FULL_SCREEN || st == QUNS_PRESENTATION_MODE)) {
        return true;
    }
    return false;
}

void SetVisible(Glass& g, bool v) {
    if (g.visible == v) return;
    g.visible = v;
    ShowWindow(g.hwnd, v ? SW_SHOWNOACTIVATE : SW_HIDE);
    g_live = v;
}

void SetExpandExclude(HWND expand, bool on) {
    if (on) {
        if (!expand || !IsWindow(expand) || g_expandAffinitySet) return;
        DWORD prev = WDA_NONE;
        GetWindowDisplayAffinity(expand, &prev);
        if (SetWindowDisplayAffinity(expand, WDA_EXCLUDEFROMCAPTURE)) {
            g_expandPrevAffinity = prev;
            g_expandAffinitySet = true;
        }
    } else if (g_expandAffinitySet) {
        if (expand && IsWindow(expand)) SetWindowDisplayAffinity(expand, g_expandPrevAffinity);
        g_expandAffinitySet = false;
    }
}

void FinishSnapshotCapture(Glass& g) {
    StopCapture(g);
    SetExpandExclude(g.expand, false);
    // The glass overlay must not feed its own output back into the capture.
    if (g.hwnd && IsWindow(g.hwnd)) SetWindowDisplayAffinity(g.hwnd, WDA_NONE);
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    if (m == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(h, m, w, l);
}

DWORD WINAPI Worker(LPVOID) {
    RoInitialize(RO_INIT_MULTITHREADED);
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"ZenExpandLiquidGlass";
    RegisterClassW(&wc);

    Glass g;
    g.hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
            WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    if (!g.hwnd || !CreateDevice(g)) {
        Wh_Log(L"[xlg] init failed — keeping acrylic / translucent fallback");
        g_deviceLost = true;
        g_want = false;
        if (g.hwnd) DestroyWindow(g.hwnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        g_live = false;
        RoUninitialize();
        return 0;
    }

    ULONGLONG lastPauseCheck = 0;
    bool paused = false;
    HMONITOR snapshotMonitor = nullptr;
    uint64_t lastSettingsGen = 0;
    ULONGLONG captureStarted = 0;
    ULONGLONG lastMoveAt = 0;
    bool captureFailed = false;
    auto nextFrame = std::chrono::steady_clock::now();
    while (!g_stop.load()) {
        auto waitNs = std::chrono::duration_cast<std::chrono::nanoseconds>(nextFrame - std::chrono::steady_clock::now()).count();
        const DWORD frameMs = g_want.load() ? (DWORD)std::max(0LL, (waitNs + 999999) / 1000000) : INFINITE;
        MsgWaitForMultipleObjects(1, &g_wake, FALSE, frameMs, QS_ALLINPUT);
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (g_stop.load()) break;
        if (!g_want.load()) {
            SetVisible(g, false);
            FinishSnapshotCapture(g);
            // Keep the cached monitor frame. Dropping it here forced a fresh
            // capture on every open, so the glass stayed hidden for the first
            // frames of the expand animation (a visible flash). The frame is
            // only thrown away when the monitor or the popup window changes.
            nextFrame = std::chrono::steady_clock::now();
            continue;  // sleep until reopen/unload; no capture, rendering or idle polling
        }

        auto frameNow = std::chrono::steady_clock::now();
        if (frameNow < nextFrame) continue;  // geometry wake-ups do not exceed the capture budget
        nextFrame = frameNow + std::chrono::nanoseconds(1000000000LL / kMaxFps);
        RECT rc;
        RECT card;
        HMONITOR mon;
        float radius;
        HWND expand;
        uint64_t generation;
        {
            std::lock_guard<std::mutex> lk(g_geomLock);
            rc = g_geomRect;
            card = g_geomCard;
            mon = g_geomMon;
            radius = g_geomRadius;
            expand = g_expandHwnd;
            generation = g_openGeneration;
        }
        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0 || !mon || !expand || !IsWindow(expand)) {
            SetVisible(g, false);
            FinishSnapshotCapture(g);
            continue;
        }
        if (snapshotMonitor != mon || g.expand != expand) {
            SetVisible(g, false);
            FinishSnapshotCapture(g);
            g.src.Reset(); g.srv.Reset(); g.hasContent = false;
            g_hasFrame = false;
            captureFailed = false;
            g.expand = expand;
            snapshotMonitor = mon;
        }

        ULONGLONG now = GetTickCount64();
        if (now - lastPauseCheck > 1000) {
            lastPauseCheck = now;
            paused = ShouldPause();
        }
        if (paused) {
            SetVisible(g, false);
            FinishSnapshotCapture(g);
            continue;
        }

        uint64_t settingsGen = g_settingsGen.load(std::memory_order_acquire);
        if (settingsGen != lastSettingsGen) captureFailed = false;

        // The backdrop has to follow the card while the expand animation runs.
        // A frame grabbed once and reused made the background slide together
        // with the glass, so keep pulling frames for as long as geometry moves.
        const bool moved = !EqualRect(&rc, &g.target);
        if (moved) lastMoveAt = now;

        if (!g.hasContent || (moved && !g.session && !captureFailed)) {
            if (captureFailed) continue;
            if (!g.session) {
                SetExpandExclude(expand, true);
                // Exclude the overlay itself: capturing while it is visible fed
                // the previous glass frame back in and smeared the backdrop.
                if (g.hwnd && IsWindow(g.hwnd))
                    SetWindowDisplayAffinity(g.hwnd, WDA_EXCLUDEFROMCAPTURE);
                if (!g_expandAffinitySet || !StartCapture(g, mon)) {
                    Wh_Log(L"[xlg] could not start a clean backdrop snapshot");
                    FinishSnapshotCapture(g);
                    captureFailed = true;
                    lastSettingsGen = settingsGen;
                    continue;
                }
                captureStarted = now;
            }
            if (!PullFrame(g)) {
                if (now - captureStarted >= 1000) {
                    Wh_Log(L"[xlg] backdrop snapshot timed out");
                    FinishSnapshotCapture(g);
                    captureFailed = true;
                    lastSettingsGen = settingsGen;
                }
                continue;
            }
            lastMoveAt = now;
        } else if (moved && g.session) {
            // Refresh in place; a missed frame simply keeps the previous one.
            PullFrame(g);
            lastMoveAt = now;
        }
        // Settled: release the capture so screenshots and recording see the card.
        if (g.hasContent && g.session && now - lastMoveAt > 300) {
            FinishSnapshotCapture(g);
            Wh_Log(L"[xlg] backdrop settled; screenshots enabled");
        }

        const bool rectChanged = !EqualRect(&rc, &g.target);
        const bool cardChanged = card.left != (LONG)g.cardL || card.top != (LONG)g.cardT ||
                                 (card.right - card.left) != (LONG)g.cardW ||
                                 (card.bottom - card.top) != (LONG)g.cardH;
        if (g.visible && !rectChanged && !cardChanged && radius == g.cornerRadius &&
            settingsGen == lastSettingsGen)
            continue;
        g.target = rc;
        g.cardL = (float)card.left;
        g.cardT = (float)card.top;
        g.cardW = (float)(card.right - card.left);
        g.cardH = (float)(card.bottom - card.top);
        g.cornerRadius = radius;
        lastSettingsGen = settingsGen;
        // The window only moves when the full card rect changes. While the
        // expand animation runs it stays put and the visible sub-rect grows, so
        // DWM never slides the previous frame along with the window.
        if (rectChanged) SetWindowPos(g.hwnd, expand, rc.left, rc.top, w, h, SWP_NOACTIVATE);
        if (!EnsureSwapChain(g, w, h)) {
            Wh_Log(L"[xlg] swap chain failed, disabling for this session");
            g_deviceLost = true;
            break;
        }
        HRESULT hr = Render(g);
        if (FAILED(hr)) {
            Wh_Log(L"[xlg] present failed (%08X), disabling for this session", (unsigned)hr);
            g_deviceLost = true;
            break;
        }
        bool stillCurrent = false;
        {
            std::lock_guard<std::mutex> lk(g_geomLock);
            stillCurrent = g_expandHwnd == expand && g_openGeneration == generation;
        }
        SetVisible(g, g_want.load() && stillCurrent && IsWindow(expand));
    }

    FinishSnapshotCapture(g);
    SetVisible(g, false);
    if (g.dcompTarget) g.dcompTarget->SetRoot(nullptr);
    if (g.dcomp) g.dcomp->Commit();
    HWND doomed = g.hwnd;
    g = Glass{};
    if (doomed) DestroyWindow(doomed);
    UnregisterClassW(wc.lpszClassName, GetModuleHandleW(nullptr));
    g_live = false;
    g_want = false;
    RoUninitialize();
    return 0;
}

void StartWorker() {
    if (g_thread && WaitForSingleObject(g_thread, 0) == WAIT_OBJECT_0) {
        CloseHandle(g_thread);
        if (g_wake) CloseHandle(g_wake);
        g_thread = nullptr;
        g_wake = nullptr;
    }
    if (g_thread || g_deviceLost.load()) return;
    g_stop = false;
    g_wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread) {
        if (g_wake) { CloseHandle(g_wake); g_wake = nullptr; }
        Wh_Log(L"[xlg] CreateThread failed");
    }
}

void StopWorker() {
    if (!g_thread) return;
    g_stop = true;
    g_want = false;
    if (g_wake) SetEvent(g_wake);
    WaitForSingleObject(g_thread, 5000);
    CloseHandle(g_thread);
    if (g_wake) CloseHandle(g_wake);
    g_thread = nullptr;
    g_wake = nullptr;
    g_live = false;
}

}  // namespace xlg

static bool XlgIsLive() {
    // A cached frame means the glass will be on screen from the first animation
    // frame, so the popup must not paint its acrylic fallback first and switch
    // it off again a frame later (that switch is visible as a flash).
    return xlg::g_live.load() || (xlg::g_want.load() && xlg::g_hasFrame.load());
}

static void XlgOnSettingsChanged() {
    // Saving settings is the user's explicit retry after a device-removed disable.
    xlg::g_deviceLost = false;
    if (!WantsCapturedBackdrop(GetSettings())) { xlg::g_want = false; xlg::StopWorker(); }
}

// Collapsing must not join a capture/GPU worker on Explorer's UI thread.
// Keep its device warm; the worker hides, releases capture frames, then sleeps.
static void XlgSuspend() {
    xlg::g_want = false;
    if (xlg::g_wake) SetEvent(xlg::g_wake);
}

static void XlgStop() {
    xlg::g_want = false;
    xlg::StopWorker();
    {
        std::lock_guard<std::mutex> lk(xlg::g_geomLock);
        xlg::g_expandHwnd = nullptr;
        xlg::g_geomMon = nullptr;
        xlg::g_geomRect = {};
    }
}

static void XlgStart(HWND expandHwnd) {
    if (!expandHwnd || !WantsCapturedBackdrop(GetSettings()) || xlg::g_deviceLost.load()) return;
    {
        std::lock_guard<std::mutex> lk(xlg::g_geomLock);
        xlg::g_expandHwnd = expandHwnd;
        ++xlg::g_openGeneration;
    }
    xlg::g_want = true;
    xlg::StartWorker();
    if (xlg::g_wake) SetEvent(xlg::g_wake);
}

static void XlgUpdate(const RECT& screenRc, const RECT& cardRc, HMONITOR mon, float cornerRadiusPx) {
    if (!xlg::g_want.load()) return;
    {
        std::lock_guard<std::mutex> lk(xlg::g_geomLock);
        xlg::g_geomRect = screenRc;
        xlg::g_geomCard = cardRc;
        xlg::g_geomMon = mon;
        xlg::g_geomRadius = cornerRadiusPx;
    }
    if (xlg::g_wake) SetEvent(xlg::g_wake);
}


// Try DWM system backdrop / accent acrylic behind the top-level expand popup.
// Works with UpdateLayeredWindow: DWM blurs whatever is behind, and our per-pixel
// alpha decides how much shows through. Failures leave the translucent LookFor fill.
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMSBT_TRANSIENTWINDOW
#define DWMSBT_TRANSIENTWINDOW 3
#endif

static bool ApplyExpandBackdrop(HWND hwnd, bool light) {
    if (!hwnd) return false;
    // Settings / GPU readiness can change while open. Avoid resetting native materials
    // on every 15 ms animation tick when neither has changed.
    static HWND lastHwnd = nullptr;
    static uint64_t lastGen = 0;
    static bool lastLiquid = false, lastLight = false;
    uint64_t gen = g_settingsGen.load(std::memory_order_acquire);
    bool liquid = WantsCapturedBackdrop(GetSettings()) && XlgIsLive();
    if (hwnd == lastHwnd && gen == lastGen && liquid == lastLiquid && light == lastLight) return true;
    lastHwnd = hwnd; lastGen = gen; lastLiquid = liquid; lastLight = light;
    bool any = false;
    int corner = DWMWCP_ROUND;
    if (SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner))))
        any = true;
    // 磨砂 has no captured backdrop of its own, so the system must draw the
    // acrylic; the glass modes keep DWMSBT_NONE and supply the material
    // themselves, so the two never stack.
    const bool wantsGlass = WantsCapturedBackdrop(GetSettings());
    int backdrop = wantsGlass ? 1 /* DWMSBT_NONE */ : DWMSBT_TRANSIENTWINDOW;
    if (SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop))))
        any = true;

    // Accent acrylic (same entry point shell surfaces use). Composes with ULW alpha.
    using SetWCA = BOOL(WINAPI*)(HWND, void*);
    static SetWCA setWca = []() -> SetWCA {
        HMODULE u = GetModuleHandleW(L"user32.dll");
        return u ? reinterpret_cast<SetWCA>(GetProcAddress(u, "SetWindowCompositionAttribute")) : nullptr;
    }();
    if (setWca) {
        struct AccentPolicy { int state; DWORD flags; DWORD gradientColor; DWORD anim; };
        struct WcaData { int attr; void* data; SIZE_T size; };
        AccentPolicy policy{};
        const Settings& s = GetSettings();
        // Glass modes draw the whole backdrop themselves; the popup must not add
        // a second material of its own, which would move with the window.
        policy.state = (WantsCapturedBackdrop(s) || !s.panelGlass) ? 0 : 4;
        // ABGR tint: light = soft white, dark = soft black; alpha = tint strength.
        // Tint is drawn once by GDI+ below; the native layer supplies blur only.
        policy.gradientColor = 0x01000000;
        WcaData data{19 /* WCA_ACCENT_POLICY */, &policy, sizeof(policy)};
        if (setWca(hwnd, &data)) any = true;
    }
    return any;
}


// Place the expand panel relative to the collapsed bar / taskbar edge, clamped to the
// monitor work area. anchorX/Y is the fixed edge of the popup (bottom for Bottom edge,
// top for Top, left for Left, right for Right).
static void ComputeExpandAnchor(HWND bar, int panelW, int panelH, TaskbarEdge& edge, int& anchorX,
                                int& anchorY) {
    RECT br{};
    GetWindowRect(bar, &br);
    HWND tray = GetAncestor(bar, GA_PARENT);
    RECT tr{};
    if (!tray || !GetWindowRect(tray, &tr)) tr = br;
    edge = DetectTaskbarEdge(tray && IsWindow(tray) ? tray : bar);
    const int dpi = (int)GetDpiForWindow(bar);
    HMONITOR mon = MonitorFromWindow(bar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    const RECT& work = mi.rcWork;
    const int gap = MulDiv(8, dpi ? dpi : 96, 96);
    if (IsVerticalEdge(edge)) {
        int y = std::clamp((int)br.top, (int)work.top + gap, (int)work.bottom - panelH - gap);
        if (edge == TaskbarEdge::Left) {
            anchorX = tr.right + gap;
            if (anchorX + panelW > work.right - gap) anchorX = std::max((int)work.left + gap, (int)work.right - panelW - gap);
        } else {
            anchorX = tr.left - gap;  // right edge of popup will sit here
            if (anchorX - panelW < work.left + gap) anchorX = std::min((int)work.right - gap, (int)work.left + panelW + gap);
        }
        anchorY = y;
    } else {
        int x = std::clamp((int)br.left, (int)work.left + gap, (int)work.right - panelW - gap);
        if (edge == TaskbarEdge::Bottom) {
            anchorY = tr.top - gap;  // bottom edge of popup
            if (anchorY - panelH < work.top + gap) anchorY = std::min((int)work.bottom - gap, (int)work.top + panelH + gap);
        } else {
            anchorY = tr.bottom + gap;  // top edge of popup
            if (anchorY + panelH > work.bottom - gap) anchorY = std::max((int)work.top + gap, (int)work.bottom - panelH - gap);
        }
        anchorX = x;
    }
}

static void ExpandRender(bool refreshContent = true) {
    if (!g_x.hwnd) return;
    const Settings& s = GetSettings();
    const int dpi = (int)GetDpiForWindow(g_x.bar ? g_x.bar : g_x.hwnd);
    RefreshEnv(false);
    const bool light = g_env.lightTaskbar;
    if (g_x.target > 0.5 && WantsCapturedBackdrop(s) && !xlg::g_want.load()) XlgStart(g_x.hwnd);
    ApplyExpandBackdrop(g_x.hwnd, light);
    uint64_t settingsGen = g_settingsGen.load(std::memory_order_acquire);
    if (refreshContent || !g_x.content || g_x.contentDpi != dpi || g_x.contentLight != light ||
        g_x.contentSettingsGen != settingsGen) {
        ExpandLayoutPaint(nullptr, dpi, light);
        auto content = std::make_unique<Gdiplus::Bitmap>(g_x.w, g_x.h, PixelFormat32bppPARGB);
        if (content->GetLastStatus() != Gdiplus::Ok) return;
        Gdiplus::Graphics cg(content.get());
        cg.Clear(Gdiplus::Color(0, 0, 0, 0));
        cg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        cg.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        ExpandLayoutPaint(&cg, dpi, light);
        g_x.content = std::move(content);
        g_x.contentHits = g_x.hits;  // untransformed, never accumulate animation offsets
        g_x.contentDpi = dpi; g_x.contentLight = light; g_x.contentSettingsGen = settingsGen;
    }
    if (g_x.bar) ComputeExpandAnchor(g_x.bar, g_x.w, g_x.h, g_x.edge, g_x.anchorX, g_x.anchorY);
    const double vis = std::clamp(g_x.p, 0.0, 1.08);
    const bool axisX = IsVerticalEdge(g_x.edge);  // left/right: spring along X
    const int fullW = axisX ? std::max(1, (int)(g_x.w * vis)) : g_x.w;
    const int fullH = axisX ? g_x.h : std::max(1, (int)(g_x.h * vis));
    const int shownW = axisX ? std::max(1, (int)(g_x.w * vis)) : g_x.w;
    const int shownH = axisX ? g_x.h : std::max(1, (int)(g_x.h * vis));
    HDC screen = GetDC(nullptr);
    if (!g_x.surface) g_x.surface = std::make_unique<ExpandSurface>();
    const int capacityW = std::max(fullW, (int)(g_x.w * 1.08));
    const int capacityH = std::max(fullH, (int)(g_x.h * 1.08));
    if (!g_x.surface->Ensure(screen, capacityW, capacityH)) { ReleaseDC(nullptr, screen); return; }
    HDC mem = g_x.surface->dc;
    void* bits = g_x.surface->bits;
    const Gdiplus::REAL rad = (Gdiplus::REAL)(14 * dpi / 96.0);
    {
        Gdiplus::REAL cardX = 0, cardY = 0;
        {
            Gdiplus::Bitmap canvas(fullW, fullH, g_x.surface->width * 4, PixelFormat32bppPARGB, (BYTE*)bits);
            Gdiplus::Graphics g(&canvas);
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
            g.Clear(Gdiplus::Color(0, 0, 0, 0));
            if (axisX) {
                cardX = (g_x.edge == TaskbarEdge::Right) ? (Gdiplus::REAL)(fullW - shownW) : 0;
                cardY = 0;
            } else if (g_x.edge == TaskbarEdge::Bottom) {
                cardX = 0;
                cardY = (Gdiplus::REAL)(fullH - shownH);  // grows upward; bottom edge fixed
            } else {
                cardX = 0;
                cardY = 0;  // Top: grows downward
            }
            Gdiplus::RectF card(cardX, cardY, (Gdiplus::REAL)shownW, (Gdiplus::REAL)shownH);
            // Same look as the mini panel. panelBackground=none still gets a subtle
            // tint so text stays readable on a fully transparent taskbar.
            PanelLook look = LookFor(s.panelBg, s.panelGlass, light);
            Gdiplus::ARGB fill = look.fill;
            if (!s.glassColor.empty()) fill = MaterialColor(s, light);
            else if (!fill) fill = light ? 0x66F7F7F7 : 0x66141414;
            // Alpha 1 keeps a zero-opacity card interactive in layered-window hit testing.
            if ((fill >> 24) == 0) fill = 0x01000000;
            FillRound(g, fill, card, rad);
            if (look.edge) {
                Gdiplus::GraphicsPath edge;
                RoundRectPath(edge, card.X, card.Y, card.Width, card.Height, rad);
                Gdiplus::Pen pen(Gdiplus::Color(look.edge), 1.0f);
                g.DrawPath(&pen, &edge);
            }
            Gdiplus::GraphicsPath clip;
            RoundRectPath(clip, card.X, card.Y, card.Width, card.Height, rad);
            g.SetClip(&clip);
            // Content slides with the card, slightly offset for the spring feel.
            Gdiplus::REAL ox = cardX + (Gdiplus::REAL)(shownW - g_x.w) * 0.5f;
            Gdiplus::REAL oy = cardY + (Gdiplus::REAL)(shownH - g_x.h) * 0.5f;
            g.DrawImage(g_x.content.get(), ox, oy, (Gdiplus::REAL)g_x.content->GetWidth(),
                        (Gdiplus::REAL)g_x.content->GetHeight());
        }
        POINT srcPt{0, 0};
        SIZE sz{fullW, fullH};
        POINT dst{};
        if (axisX) {
            if (g_x.edge == TaskbarEdge::Left) dst = {g_x.anchorX, g_x.anchorY};
            else dst = {g_x.anchorX - fullW, g_x.anchorY};  // Right: right edge fixed at anchorX
        } else if (g_x.edge == TaskbarEdge::Bottom) {
            dst = {g_x.anchorX, g_x.anchorY - fullH};
        } else {
            dst = {g_x.anchorX, g_x.anchorY};  // Top
        }
        BYTE alpha = (BYTE)(std::clamp(g_x.p * 1.4, 0.0, 1.0) * 255);
        BLENDFUNCTION bf{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_x.hwnd, screen, &dst, &sz, mem, &srcPt, 0, &bf, ULW_ALPHA);
        g_x.hits = g_x.contentHits;
        // Hit rects are in content coordinates; shift them to window coordinates.
        LONG dx = (LONG)(cardX + (shownW - g_x.w) * 0.5);
        LONG dy = (LONG)(cardY + (shownH - g_x.h) * 0.5);
        for (auto& h : g_x.hits) OffsetRect(&h.rc, dx, dy);
        if (WantsCapturedBackdrop(s)) {
            // fullW/fullH above track the animation and grow every frame, so the
            // window used to expand and crawl upwards with the card. Give the
            // glass window the final card rect instead: it stays put while the
            // visible sub-rect grows inside it.
            const int winW = std::max(1, g_x.w), winH = std::max(1, g_x.h);
            LONG wx = dst.x, wy = dst.y;
            if (axisX) {
                if (g_x.edge == TaskbarEdge::Right) wx = dst.x + fullW - winW;
            } else if (g_x.edge == TaskbarEdge::Bottom) {
                wy = dst.y + fullH - winH;
            }
            RECT fullRc{wx, wy, wx + winW, wy + winH};
            // cardX/cardY are relative to dst; convert to window-local coordinates.
            const LONG cardDx = cardX + (dst.x - wx);
            const LONG cardDy = cardY + (dst.y - wy);
            RECT cardRc{cardDx, cardDy, cardDx + shownW, cardDy + shownH};
            HMONITOR mon = MonitorFromRect(&fullRc, MONITOR_DEFAULTTONEAREST);
            XlgUpdate(fullRc, cardRc, mon, (float)rad);
        }
    }
    ReleaseDC(nullptr, screen);
}

static void StopExpandAnimation() {
    if (g_x.hwnd) KillTimer(g_x.hwnd, TIMER_ANIM);
    if (g_x.animationClock) g_x.animationClock->Stop();
}
static void StartExpandAnimation() {
    g_x.lastAnimationTime = AnimationTimeSeconds();
    if (!g_x.animationClock) g_x.animationClock = std::make_unique<ExpandAnimationClock>();
    if (!g_x.animationClock->Start(g_x.hwnd)) {
        Wh_Log(L"[expand] high-resolution clock unavailable, using legacy timer");
        SetTimer(g_x.hwnd, TIMER_ANIM, 15, nullptr);
    }
}

static void ExpandClose(bool immediate) {
    if (!g_x.hwnd) return;
    XlgSuspend();  // stop capture asynchronously; the spring never waits for GPU teardown
    g_x.target = 0;
    g_x.lastAnimationTime = AnimationTimeSeconds();
    if (immediate) {
        StopExpandAnimation();
        KillTimer(g_x.hwnd, TIMER_WATCH);
        DestroyWindow(g_x.hwnd);
        g_x = ExpandState{};
        g_expandedAny = false;
        return;
    }
    StartExpandAnimation();
}

static void ExpandOpen(HWND bar) {
    const Settings& s = GetSettings();
    if (g_x.hwnd && g_x.bar == bar && g_x.target > 0.5) {  // click again collapses
        ExpandClose(false);
        return;
    }
    if (g_x.hwnd && g_x.bar != bar) ExpandClose(true);
    HWND tray = GetAncestor(bar, GA_PARENT);
    const int dpi = (int)GetDpiForWindow(bar);
    ExpandLayoutPaint(nullptr, dpi, g_env.lightTaskbar);
    TaskbarEdge edge = TaskbarEdge::Bottom;
    int ax = 0, ay = 0;
    ComputeExpandAnchor(bar, g_x.w, g_x.h, edge, ax, ay);
    // Seed window position near the anchor (ExpandRender places the layered surface).
    int seedX = ax, seedY = ay;
    if (edge == TaskbarEdge::Bottom) seedY = ay - 1;
    else if (edge == TaskbarEdge::Right) seedX = ax - 1;
    if (!g_x.hwnd) {
        g_x.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, kExpandClass,
                                   L"ZenDesktop 歌词展开面板", WS_POPUP, seedX, seedY, 1, 1, tray, nullptr, g_uiInst,
                                   nullptr);
        if (!g_x.hwnd) {
            Wh_Log(L"[expand] CreateWindowEx failed %u", GetLastError());
            return;
        }
        ApplyExpandBackdrop(g_x.hwnd, g_env.lightTaskbar);
        g_x.p = g_x.v = 0;
    }
    g_x.bar = bar;
    g_x.edge = edge;
    g_x.anchorX = ax;
    g_x.anchorY = ay;
    g_x.target = 1;
    g_x.lastAnimationTime = AnimationTimeSeconds();
    g_x.lastInteract = GetTickCount64();
    g_x.lbuttonWasDown = true;  // the click that opened us must not count as "outside"
    g_expandedAny = true;
    if (WantsCapturedBackdrop(s)) XlgStart(g_x.hwnd);  // want=true before first ExpandRender geom push
    ExpandRender();
    ShowWindow(g_x.hwnd, SW_SHOWNOACTIVATE);
    StartExpandAnimation();
    SetTimer(g_x.hwnd, TIMER_WATCH, 50, nullptr);
}

// Starts a shell drag of one relayed file (CF_HDROP and shell formats via the
// item's own data object). DoDragDrop runs a modal loop on this thread, as any
// drag from the taskbar does.
struct ZenDropSource final : IDropSource {
    LONG ref = 1;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** pv) override {
        if (iid == IID_IUnknown || iid == IID_IDropSource) { *pv = this; AddRef(); return S_OK; }
        *pv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&ref); }
    ULONG STDMETHODCALLTYPE Release() override { LONG r = InterlockedDecrement(&ref); if (!r) delete this; return r; }
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL esc, DWORD keys) override {
        if (esc) return DRAGDROP_S_CANCEL;
        return (keys & MK_LBUTTON) ? S_OK : DRAGDROP_S_DROP;
    }
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override { return DRAGDROP_S_USEDEFAULTCURSORS; }
};

static void DragFileOut(HWND hwnd, const std::wstring& path) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) || !pidl) {
        Wh_Log(L"[relay] %s is gone", path.c_str());
        return;
    }
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    IDataObject* data = nullptr;
    if (SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child)) &&
        SUCCEEDED(parent->GetUIObjectOf(hwnd, 1, &child, IID_IDataObject, nullptr, (void**)&data))) {
        auto* src = new ZenDropSource();
        DWORD effect = 0;
        g_x.lastInteract = GetTickCount64();
        DoDragDrop(data, src, DROPEFFECT_COPY, &effect);
        src->Release();
        data->Release();
    }
    if (parent) parent->Release();
    CoTaskMemFree(pidl);
}

static int ExpandHitTest(POINT pt, int* index) {
    for (auto& h : g_x.hits)
        if (PtInRect(&h.rc, pt)) { if (index) *index = h.index; return h.id; }
    if (index) *index = -1;
    return XNone;
}

static void AdvanceExpandAnimation() {
    double now = AnimationTimeSeconds();
    double elapsed = g_x.lastAnimationTime ? now - g_x.lastAnimationTime : 1.0 / kExpandAnimationFps;
    g_x.lastAnimationTime = now;
    StepExpandSpring(g_x.p, g_x.v, g_x.target, elapsed);
    bool settled = fabs(g_x.p - g_x.target) < 0.002 && fabs(g_x.v) < 0.01;
    if (settled) { g_x.p = g_x.target; StopExpandAnimation(); }
    if (g_x.target == 0 && (settled || g_x.p < 0.02)) { ExpandClose(true); return; }
    ExpandRender(false);  // cached paint path, shared by 120 Hz clock and legacy fallback
}

static LRESULT CALLBACK ExpandProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_ZEN_ANIMFRAME:
            if (g_x.animationClock && g_x.animationClock->running && wp == g_x.animationClock->generation) {
                g_x.animationClock->queued = false;
                AdvanceExpandAnimation();
            }
            return 0;
        case WM_TIMER: {
            if (wp == TIMER_ANIM) { AdvanceExpandAnimation(); return 0; }
            if (wp == TIMER_WATCH) {
                const Settings& s = GetSettings();
                POINT pt;
                GetCursorPos(&pt);
                RECT wr{}, br{};
                GetWindowRect(hwnd, &wr);
                if (g_x.bar) GetWindowRect(g_x.bar, &br);
                bool over = PtInRect(&wr, pt) || PtInRect(&br, pt);
                bool down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000);
                if (over) g_x.lastInteract = GetTickCount64();
                if (down && !g_x.lbuttonWasDown && !over) { ExpandClose(false); return 0; }  // click outside
                g_x.lbuttonWasDown = down;
                if (s.expandAutoCollapseSec > 0 && g_x.target > 0.5 &&
                    GetTickCount64() - g_x.lastInteract > (ULONGLONG)s.expandAutoCollapseSec * 1000ULL) {
                    ExpandClose(false);
                    return 0;
                }
                static ULONGLONG lastRefresh = 0;  // progress / AI / resources refresh
                bool animating = fabs(g_x.p - g_x.target) >= 0.002 || fabs(g_x.v) >= 0.01;
                if (!animating && GetTickCount64() - lastRefresh >= 500) {
                    lastRefresh = GetTickCount64(); ExpandRender();
                }
                return 0;
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            g_x.lastInteract = GetTickCount64();
            if ((wp & MK_LBUTTON) && g_x.pressed == XFile && g_x.pressedIndex >= 0 &&
                (abs(pt.x - g_x.pressPt.x) > GetSystemMetrics(SM_CXDRAG) ||
                 abs(pt.y - g_x.pressPt.y) > GetSystemMetrics(SM_CYDRAG))) {
                auto files = GetRelayFiles();
                int idx = g_x.pressedIndex;
                g_x.pressed = XNone;
                if (GetCapture() == hwnd) ReleaseCapture();
                if (idx < (int)files.size()) DragFileOut(hwnd, files[idx]);
                return 0;
            }
            int idx = -1;
            int hit = ExpandHitTest(pt, &idx);
            if (hit != g_x.hover || (hit == XFile && idx != g_x.pressedIndex && !(wp & MK_LBUTTON))) {
                g_x.hover = hit;
                if (!(wp & MK_LBUTTON)) g_x.pressedIndex = idx;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                ExpandRender(fabs(g_x.p - g_x.target) < 0.002 && fabs(g_x.v) < 0.01);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_x.hover != XNone) {
                g_x.hover = XNone;
                ExpandRender(fabs(g_x.p - g_x.target) < 0.002 && fabs(g_x.v) < 0.01);
            }
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT && g_x.hover != XNone) { SetCursor(LoadCursorW(nullptr, IDC_HAND)); return TRUE; }
            break;
        case WM_LBUTTONDOWN: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            g_x.pressed = ExpandHitTest(pt, &g_x.pressedIndex);
            g_x.pressPt = pt;
            if (g_x.pressed != XNone) SetCapture(hwnd);
            return 0;
        }
        case WM_LBUTTONUP: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            int idx = -1;
            int hit = ExpandHitTest(pt, &idx), was = g_x.pressed;
            g_x.pressed = XNone;
            if (GetCapture() == hwnd) ReleaseCapture();
            if (hit != XNone && hit == was) {
                if (hit == XPrev) PostMediaCmd(MediaCmd::Prev);
                else if (hit == XPlay) PostMediaCmd(MediaCmd::PlayPause);
                else if (hit == XNext) PostMediaCmd(MediaCmd::Next);
                else if (hit == XOpen) { PostMediaCmd(MediaCmd::OpenPlayer); ExpandClose(false); return 0; }
                else if (hit == XSeek) {
                    for (const XHit& h : g_x.hits) {
                        if (h.id != XSeek || h.rc.right <= h.rc.left) continue;
                        double fraction = std::clamp((double)(pt.x - h.rc.left) / (h.rc.right - h.rc.left), 0.0, 1.0);
                        PostMediaSeek(fraction, g_x.seekTrack, g_x.seekApp);
                        break;
                    }
                }
                else if (hit == XClear) { ClearRelayFiles(); }
                else if (hit == XFile) {
                    auto files = GetRelayFiles();  // click opens the file's folder with it selected
                    if (idx >= 0 && idx < (int)files.size()) {
                        std::wstring args = L"/select,\"" + files[idx] + L"\"";
                        ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
                    }
                }
            }
            ExpandRender();
            return 0;
        }
        case WM_RBUTTONUP: return 0;
        case WM_DESTROY:
            StopExpandAnimation();
            KillTimer(hwnd, TIMER_WATCH);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ── OLE drop target on the collapsed bar (files dropped without expanding) ──
struct ZenDropTarget final : IDropTarget {
    LONG ref = 1;
    HWND bar;
    bool accept = false;
    explicit ZenDropTarget(HWND h) : bar(h) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** pv) override {
        if (iid == IID_IUnknown || iid == IID_IDropTarget) { *pv = this; AddRef(); return S_OK; }
        *pv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&ref); }
    ULONG STDMETHODCALLTYPE Release() override { LONG r = InterlockedDecrement(&ref); if (!r) delete this; return r; }
    void SetHighlight(bool on) {
        std::lock_guard<std::recursive_mutex> lk(g_panelsLock);
        if (Panel* p = PanelOf(bar)) {
            if (p->ui.dropHover != on) { p->ui.dropHover = on; RenderPanel(*p, true); }
        }
    }
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* d, DWORD, POINTL, DWORD* eff) override {
        FORMATETC fe{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        accept = GetSettings().xFiles && d && d->QueryGetData(&fe) == S_OK;
        *eff = accept && (*eff & DROPEFFECT_COPY) ? DROPEFFECT_COPY : DROPEFFECT_NONE;  // source stays untouched
        if (accept) SetHighlight(true);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL, DWORD* eff) override {
        *eff = accept && (*eff & DROPEFFECT_COPY) ? DROPEFFECT_COPY : DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() override { SetHighlight(false); return S_OK; }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject* d, DWORD, POINTL, DWORD* eff) override {
        SetHighlight(false);
        const bool canCopy = (*eff & DROPEFFECT_COPY) != 0;
        *eff = DROPEFFECT_NONE;
        if (!accept || !d || !canCopy) return S_OK;
        FORMATETC fe{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM sm{};
        if (d->GetData(&fe, &sm) != S_OK) return S_OK;
        std::vector<std::wstring> add;
        if (HDROP h = (HDROP)GlobalLock(sm.hGlobal)) {
            UINT n = DragQueryFileW(h, 0xFFFFFFFF, nullptr, 0);
            for (UINT i = 0; i < n && i < 30; ++i) {
                UINT len = DragQueryFileW(h, i, nullptr, 0);
                std::wstring p(len, L'\0');
                DragQueryFileW(h, i, p.data(), len + 1);
                add.push_back(std::move(p));
            }
            GlobalUnlock(sm.hGlobal);
        }
        ReleaseStgMedium(&sm);
        AddRelayFiles(add);
        Wh_Log(L"[relay] %u path(s) added", (unsigned)add.size());
        *eff = DROPEFFECT_COPY;
        if (g_x.hwnd) ExpandRender();
        return S_OK;
    }
};

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
            if (wp == TIMER_LINE) KillTimer(hwnd, TIMER_LINE);
            if ((wp == TIMER_TICK || wp == TIMER_LINE) && panel && !g_stop) RenderPanel(*panel);
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
                else if (hit == HitBody) {
                    if (GetSettings().clickExpands) ExpandOpen(hwnd);
                    else PostMediaCmd(MediaCmd::OpenPlayer);
                }
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
            KillTimer(hwnd, TIMER_LINE);
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
    // 文件中转: OLE drop target on the collapsed bar itself.
    HRESULT oh = OleInitialize(nullptr);  // taskbar thread is normally STA already (S_FALSE)
    p.oleInit = SUCCEEDED(oh);
    if (p.oleInit) {
        auto* dt = new ZenDropTarget(hwnd);
        HRESULT dr = RegisterDragDrop(hwnd, dt);
        dt->Release();  // OLE keeps its own reference while registered
        p.dropRegistered = SUCCEEDED(dr);
        if (!p.dropRegistered) Wh_Log(L"[relay] RegisterDragDrop failed 0x%08X", (unsigned)dr);
    } else {
        Wh_Log(L"[relay] OleInitialize failed 0x%08X", (unsigned)oh);
    }
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
        if (g_x.hwnd && g_x.bar == p.hwnd) ExpandClose(true);
        if (p.dropRegistered && p.hwnd) RevokeDragDrop(p.hwnd);
        p.dropRegistered = false;
        if (p.oleInit) OleUninitialize();
        p.oleInit = false;
        if (p.hwnd && IsWindow(p.hwnd)) {
            HWND h = p.hwnd;
            KillTimer(h, TIMER_TICK);
            KillTimer(h, TIMER_LINE);
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
    MSG m;
    PeekMessageW(&m, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_uiThreadId = GetCurrentThreadId();
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
    WNDCLASSW xc{};
    xc.lpfnWndProc = ExpandProc;
    xc.hInstance = g_uiInst;
    xc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    xc.lpszClassName = kExpandClass;
    if (!RegisterClassW(&xc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        Wh_Log(L"[ui] RegisterClass(expand) failed %u", GetLastError());
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
    UnregisterClassW(kExpandClass, g_uiInst);
    Gdiplus::GdiplusShutdown(gdipToken);
    g_uiInst = nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk entry points
// ─────────────────────────────────────────────────────────────────────────────
struct WorkerThreads {
    std::thread media, lyrics, ui, ai, extras;
};

// Process shutdown may bypass Wh_ModUninit; do not destruct joinable threads then.
static WorkerThreads* g_threads = nullptr;

static void StopWorkers() {
    g_stop = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    {
        std::lock_guard<std::mutex> lk(g_reqLock);  // avoid lost wakeup
    }
    g_reqCv.notify_all();
    if (DWORD tid = g_uiThreadId.load()) PostThreadMessageW(tid, WM_QUIT, 0, 0);
    if (g_threads) {
        if (g_threads->ui.joinable()) g_threads->ui.join();
        if (g_threads->ai.joinable()) g_threads->ai.join();
        if (g_threads->extras.joinable()) g_threads->extras.join();
        if (g_threads->media.joinable()) g_threads->media.join();
        if (g_threads->lyrics.joinable()) g_threads->lyrics.join();
        delete g_threads;
        g_threads = nullptr;
    }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    if (g_cmdEvent) { CloseHandle(g_cmdEvent); g_cmdEvent = nullptr; }
    g_uiThreadId = 0;
}

BOOL Wh_ModInit() {
    const DWORD pid = GetCurrentProcessId();
    // Folder-window explorer.exe processes are injected too; they must stay fully idle.
    if (ProbeTaskbarHostRole() < 0) {
        Wh_Log(L"Init 0.5.21 (pid %u) foreign explorer; idle", pid);
        return TRUE;
    }
    Wh_Log(L"Init 0.5.21 (pid %u)", pid);
    LoadSettings();
    g_stop = false;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_cmdEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_cmdEvent) {
        if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
        if (g_cmdEvent) { CloseHandle(g_cmdEvent); g_cmdEvent = nullptr; }
        return FALSE;
    }
    try {
        g_threads = new WorkerThreads;
        g_threads->lyrics = std::thread(LyricsThread);
        g_threads->media = std::thread(MediaThread);
        g_threads->ai = std::thread(AiThread);
        g_threads->extras = std::thread(ExtrasThread);
        g_threads->ui = std::thread(UiThread);
    } catch (...) {
        Wh_Log(L"Failed to start worker threads");
        StopWorkers();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (ProbeTaskbarHostRole() < 0) return;  // folder-window explorers: no workers, no log spam
    if (!g_threads) return;
    LoadSettings();
    XlgOnSettingsChanged();
    if (DWORD tid = g_uiThreadId.load()) PostThreadMessageW(tid, WM_ZEN_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    XlgStop();
    StopWorkers();
}
