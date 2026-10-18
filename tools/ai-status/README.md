# ZenDesktop AI 助手状态

任务栏音乐与歌词 mod（`zen-taskbar-lyrics`）会读取：

```
%LOCALAPPDATA%\ZenDesktop\ai-status\<助手名>.json
```

每个助手一个文件。JSON 字段：

| 字段 | 说明 |
|---|---|
| `agent` | 标识，例如 `claude-code`、`codex`、`opencode`、`antigravity`、`deepseek-harness` |
| `name` | 显示名（可空，mod 会按 agent 猜） |
| `state` | `working` / `tool` / `waiting` / `done` / `error` / `idle`（idle 不显示） |
| `message` | 一句说明（建议不超过 20 个汉字） |
| `detail` | 短详情，例如工具名 |
| `project` | 项目文件夹名 |
| `updated` | Unix 秒（UTC） |

文件要「原子写入」（先写临时文件再替换），否则 mod 可能读到半截 JSON。

## 先看这两条

- **钩子优先，MCP 补充。** 助手支持钩子（Claude Code、Codex、Antigravity、opencode 插件）就先接钩子：不用助手配合，也不会弹权限确认，自动报「工作中／调用工具／等你／已完成」。MCP 的 `set_status` 用来补一句「在做什么」，可以不接。DeepSeek Harness 例外：它的钩子要另装实验插件，还受沙箱限制，所以先接 MCP。
- **MCP 权限弹窗请选「总是允许」。** Antigravity 等会拦截 MCP 工具的 IDE/CLI，第一次调用 `set_status` 时会弹窗要你确认。请选「Always Allow／总是允许」，否则每次更新状态都会打断你。

## 钩子程序 zen-status.exe（推荐）

钩子每次触发都要启动一个进程。PowerShell 冷启动要 0.3–0.5 秒，Antigravity 这类同步钩子（工具调用要等钩子跑完）会明显变卡。`zen-status.exe` 是十几 KB 的小程序，不依赖 C 运行库，几毫秒就跑完，写的 JSON 和合并规则跟 PowerShell 脚本完全一样。

- 下载：仓库 Actions 里的「Build zen-status.exe」，最新一次运行的 `zen-status` 产物（发版时也会附在 Release 里）。
- 放到本目录（`tools\ai-status\zen-status.exe` 或 `tools\ai-status\zen-status\zen-status.exe`），再运行 `install.ps1`。它会复制过去，生成的钩子片段就会用它；没有它时自动用 PowerShell 脚本。想强制用 PowerShell，加 `-PowerShellHooks`。
- 自己编译：`tools\ai-status\zen-status\build.sh`（mingw-w64 gcc 或 llvm-mingw）。

用法：

```text
zen-status <agent> <state> [message] [--name X] [--detail X] [--project X] [--weak] [--clear]
zen-status hook <agent> [--event 事件名] [--name X]      # 钩子：从 stdin 读 JSON
```

`hook` 模式不带 `--event` 时从 `hook_event_name` 读事件（Claude Code、Codex）；带 `--event` 时（Antigravity 的输入里没有事件名）会按 Antigravity 的要求输出 JSON：`PreToolUse` 输出 `{"decision":"allow"}`（交给默认权限规则，不会替你批准），`Stop` 输出 `{"decision":"stop"}`，其他输出 `{}`。永远返回 0，不会挡住助手。

## 安装（推荐）

在 PowerShell 里，进入本目录后运行一次：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\install.ps1
```

它会：

1. 把脚本（和 `zen-status.exe`，如果有）复制到 `%LOCALAPPDATA%\ZenDesktop\ai-status-tools\`
2. 把填好你本机路径的配置片段写到 `...\ai-status-tools\snippets\`
3. **不会**改任何助手的配置文件，片段要你自己粘贴或合并；目标文件已存在时会提示你手动合并

测一下（任务栏应在约 1 秒内出现）：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "$env:LOCALAPPDATA\ZenDesktop\ai-status-tools\zen-ai-status.ps1" -Agent test -State working -Message hello
```

## 接法概览

| 助手 | 钩子（自动改状态） | MCP `set_status`（助手自己汇报说明） | 文档来源 |
|---|---|---|---|
| Claude Code | `~/.claude/settings.json` 的 `hooks` | `claude mcp add ...` | [Hooks](https://docs.anthropic.com/en/docs/claude-code/hooks)（已核对） |
| Codex | `~/.codex/hooks.json`，或 `notify` | `~/.codex/config.toml` 的 `[mcp_servers]` | [Hooks](https://developers.openai.com/codex/hooks)、[Config](https://developers.openai.com/codex/config-advanced)（已核对） |
| opencode | `~/.config/opencode/plugins/zen-status.js` | `opencode.json` 的 `mcp` | [Plugins](https://opencode.ai/docs/plugins/)、[MCP](https://opencode.ai/docs/mcp-servers/)（已核对） |
| Antigravity | `~/.gemini/config/hooks.json` | `~/.gemini/config/mcp_config.json` 的 `mcpServers` | [Hooks](https://antigravity.google/docs/hooks/)、[MCP](https://antigravity.google/docs/mcp/)、[Rules](https://antigravity.google/docs/rules/)（已核对） |
| DeepSeek Harness（dsh） | 可选：实验性的 Claude Code 钩子桥（见下文的限制） | `~/.dsh/cordis.patch.yml` 加一条 `@deepseek-ai/dsh-mcp-client` | [仓库](https://github.com/deepseek-ai/deepseek-harness)里的 MCP 客户端、钩子桥、AGENTS.md 文档（按源码核对，未在真机测） |
| 其他 | 无 | 无 | 在 mod 设置「按进程名显示」里填进程名（尽力方案） |

钩子只改 `state` / `detail`，不会清掉 MCP 写的 `message`。两样一起用最清楚：钩子管「忙不忙」，MCP 管「在做什么」。

## Claude Code

1. MCP（命令行跑一次）：

```text
claude mcp add --scope user zen-status -- powershell.exe -NoProfile -ExecutionPolicy Bypass -File C:\Users\你的用户名\AppData\Local\ZenDesktop\ai-status-tools\zen-status-mcp.ps1 -Agent claude-code
```

（`install.ps1` 会打印填好路径的这一行。）

2. 钩子：把 `snippets\claude-code-settings.json` 里的 `hooks` 合并进 `~/.claude/settings.json`（用户级），或项目的 `.claude/settings.json`。

3. 说明：把 `snippets\AGENTS-snippet.md` 粘进 `~/.claude/CLAUDE.md` 或项目的 `CLAUDE.md`。

用到的事件：`SessionStart`、`UserPromptSubmit`、`PreToolUse`、`PostToolUse`、`PostToolUseFailure`、`PermissionRequest`、`Notification`、`Stop`、`StopFailure`、`SessionEnd`。命令用 exec 形式（`command` + `args`），避免路径空格问题；工具类事件设了 `async: true`，不挡工具执行。

## Codex

1. MCP：把 `snippets\codex-config.toml` 里 `[mcp_servers.zen-status]` 那段粘进 `~/.codex/config.toml`。

2. 钩子（推荐）：把 `snippets\codex-hooks.json` 放到 `~/.codex/hooks.json`（已有的话合并 `hooks`）。启动 Codex 后用 `/hooks` 审查并信任这些钩子。

3. 或者只用 `notify`：取消 `codex-config.toml` 里 `notify = [...]` 那行的注释。注意 `notify` 全局只能有一个，而且目前只在 `agent-turn-complete` 时触发，只能报「已完成」，不能报「工作中」。

4. 说明：把 `AGENTS-snippet.md` 粘进用户或项目的 `AGENTS.md`。

## opencode

1. MCP：把 `snippets\opencode.json` 里的 `mcp.zen-status` 合并进 `~/.config/opencode/opencode.json`。

2. 插件：把 `zen-status.js` 复制到 `~/.config/opencode/plugins/`（或项目的 `.opencode/plugins/`）。

3. 说明：把 `AGENTS-snippet.md` 粘进 `AGENTS.md`。

插件监听：`session.status`、`session.idle`、`session.error`、`permission.asked` / `permission.updated` / `permission.replied`，以及 `tool.execute.before` / `tool.execute.after`。

## Antigravity

Antigravity（Gemini）的 App、IDE 和 CLI 都读 `~/.gemini/config/` 下的全局配置。

1. 钩子（推荐）：把 `snippets\antigravity-hooks.json` 放到 `~/.gemini/config/hooks.json`。已经有这个文件的话，把里面的 `"zen-status": {...}` 这一项合并进去（文件顶层是「钩子名 → 事件」）。可以在 … → Customizations → Hooks 里查看和开关；CLI 里用 `/hooks`。

   用到的事件：`SessionStart`（清空）、`PreInvocation`（工作中；上一轮已完成时清掉旧说明）、`PreToolUse`（调用工具，显示工具名）、`PostToolUse`（工作中）、`Stop`（已完成；`terminationReason` 为 `error` 时报错误；还有后台任务时保持工作中）。Antigravity 的钩子是同步的，所以强烈建议用 `zen-status.exe`。

   **钩子命令里的路径不能加引号。** Antigravity 2.24.0（Windows）按空格拆分钩子命令，但不会去掉引号，于是 `-File "C:\...\agent-hook.ps1"` 传给 PowerShell 的路径还带着引号，PowerShell 报「-File 后跟无效路径」（退出码 0xfffd0000）。Antigravity 接着把这段中文报错（GBK 编码）塞进 UTF-8 字段，报 `invalid UTF-8`，整个任务直接被终止，看起来像「超时」或 Agent execution terminated。`install.ps1` 生成的 `antigravity-hooks.json` 已经是不带引号的路径；路径有空格时自动换成 8.3 短路径（如 `C:\Users\ABCDEF~1\...`）。如果路径有空格、这个盘又关了 8.3 短名，安装时会打警告，这时请把 `zen-status.exe` 放到没有空格的目录再生成。自己手写 hooks.json 时也别加引号。`agent-hook.ps1` 和 `zen-status.exe` 本身不会往 stderr 写东西，永远返回 0。

2. MCP：把 `snippets\antigravity-mcp_config.json` 里 `mcpServers` 下的 `zen-status` 合并进 `~/.gemini/config/mcp_config.json`（项目级是 `.agents/mcp_config.json`）。在 Antigravity 里点 … → MCP Servers → Manage MCP Servers → View raw config，打开的就是你这个版本实际用的文件。早期版本用的是 `~/.gemini/antigravity/mcp_config.json`，[GitHub MCP Server 的 Antigravity 安装说明](https://github.com/github/github-mcp-server/blob/main/docs/installation-guides/install-antigravity.md) 里还是这个路径；以 View raw config 为准。第一次调用 `set_status` 弹权限确认时选「Always Allow」。

3. 说明：把 `snippets\GEMINI-snippet.md` 追加到 `~/.gemini/GEMINI.md`（全局规则，也可以用 `~/.gemini/AGENTS.md`），或项目里的 `GEMINI.md` / `AGENTS.md`。

注意 `~/.gemini/GEMINI.md` 和 Gemini CLI 共用，写进去的规则两边都会读到。

## DeepSeek Harness（dsh）

[DeepSeek Harness](https://github.com/deepseek-ai/deepseek-harness)（命令 `dsh`）是 DeepSeek 开源的编程智能体。用 `npx @deepseek-ai/dsh web` 启动网页界面，也有桌面版。配置放在 `$DSH_HOME`，默认是 `%USERPROFILE%\.dsh\`。它跟其他助手不一样，**MCP 是主要接法**，钩子只是可选项。

1. MCP（推荐）：把 `snippets\deepseek-harness-cordis.patch.yml` 里的 `- insert:` 那一项加到 `%USERPROFILE%\.dsh\cordis.patch.yml`。没有这个文件就直接复制过去；已经有了就把这一项追加到列表末尾，别整份覆盖。这个文件对所有 profile 都生效（网页版、桌面版、headless）。MCP 客户端插件 `@deepseek-ai/dsh-mcp-client` 是 dsh 自带的，不用另装。改完重启 dsh（或者等它自动重载配置），然后**新开一个会话**，就能看到 `zen-status` 的 `set_status` 工具。第一次调用如果弹出批准，选允许。

   片段是这样的（路径用单引号，反斜杠不用转义）：

   ```yaml
   - insert:
       - id: mcp-zen-status
         name: '@deepseek-ai/dsh-mcp-client'
         config:
           serverName: zen-status
           transport: stdio
           command: powershell.exe
           args: ['-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', 'C:\Users\你的用户名\AppData\Local\ZenDesktop\ai-status-tools\zen-status-mcp.ps1', '-Agent', 'deepseek-harness', '-Name', 'DeepSeek Harness']
   ```

2. 说明：把 `snippets\AGENTS-snippet.md` 追加到 `%USERPROFILE%\.dsh\AGENTS.md`（全局，每个会话都会读），或者项目里的 `AGENTS.md`。dsh 也会读 `~/.agents/AGENTS.md` 和项目里的 `CLAUDE.md`。

3. 钩子（可选，实验性）：dsh 有一个实验性插件 `@deepseek-ai/dsh-hooks-claude-code`，可以执行 Claude Code 格式的 `hooks.json`。它默认没装，也没启用。
   - 安装：`dsh plugin --profile web add @deepseek-ai/dsh-hooks-claude-code @deepseek-ai/dsh-hook-protocol`
   - 再把 `snippets\deepseek-harness-hooks-cordis.patch.yml` 里那一项加到**这个 profile 自己的** `%USERPROFILE%\.dsh\profiles\web\cordis.patch.yml`。别放进全局的 `cordis.patch.yml`，否则没装这个插件的 profile（比如桌面版）会加载失败。
   - 它指向 `snippets\deepseek-harness-hooks.json`，用到 `SessionStart`、`UserPromptSubmit`、`PreToolUse`、`PostToolUse`、`Stop` 五个事件。这个插件只认 `command` 字符串，不认 `args`。
   - 在 Windows 上，dsh 用 `pwsh -Command <命令>` 跑钩子命令，所以命令要写成 PowerShell 语法：`& 'C:\...\zen-status.exe' hook deepseek-harness`。`install.ps1` 生成的就是这个形式，路径里有空格也没问题，不用 8.3 短路径。
   - **限制：** dsh 在 Windows 上用沙箱跑钩子。默认的 `workspace-write` 模式下，子进程只能写工作区和一个私有临时目录，钩子很可能写不进 `%LOCALAPPDATA%\ZenDesktop\ai-status\`，任务栏就不会变。这是读源码得出的结论，没在真机上测过。所以 dsh 请以 MCP 为主。

4. 按进程名显示：桌面版的进程名是 `DeepSeek Harness.exe`，0.4.3 起默认填了。网页版和命令行跑在 `node.exe` 里，没法按进程名区分。

## 给没有钩子的工具

在 Windhawk 里打开「任务栏音乐与歌词」→「按进程名显示」，填进程名（逗号分隔），例如：

```text
Antigravity.exe, gemini.exe, aider.exe
```

进程在运行时显示「运行中」。0.4.1 起默认填了 `Antigravity.exe, Antigravity IDE.exe`，0.4.3 起还加了 `DeepSeek Harness.exe`（DeepSeek Harness 桌面版）；这个助手已经有状态文件（接了钩子或 MCP，哪怕当前是空闲）时不会再重复显示「运行中」。也可以在自己的脚本里直接调：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "$env:LOCALAPPDATA\ZenDesktop\ai-status-tools\zen-ai-status.ps1" -Agent mytool -State working -Message "在跑测试"
```

## 核对说明

| 适配器 | 状态 |
|---|---|
| Claude Code hooks + MCP | 已按官方 Hooks / MCP 文档核对；在本机用 PowerShell 7 跑过 hook 与 MCP `set_status` |
| Codex hooks + MCP + notify | 已按官方 Hooks / Config 文档核对；`notify` 参数形状按文档示例；hooks 需在 Codex 里 `/hooks` 信任后才跑 |
| opencode plugin + MCP | 已按官方 Plugins / MCP 文档核对；插件在 Node 里跑过写入 |
| Antigravity hooks + MCP + GEMINI.md | 已按官方 Hooks / MCP / Rules 文档核对（2026-10）；真机 Antigravity 2.24.0 上发现并修了路径引号问题，PowerShell 钩子 5 个事件实测返回 0 |
| DeepSeek Harness MCP + AGENTS.md | 按 deepseek-harness 仓库（2026-10-17 提交 687f282）的 `dsh-mcp-client`、`agent-instructions` 文档和源码核对，没在真机测过 |
| DeepSeek Harness 钩子 | 按 `dsh-hooks-claude-code`、`dsh-hook-protocol`、`dsh-pwsh-local` 源码核对：Claude Code 格式、命令经 `pwsh -Command` 执行、stdin 传 JSON。在 Linux 的 pwsh 7.4 上确认过 `& '路径'` 这种写法能解析，子进程也能收到 stdin。默认沙箱下大概率写不进状态文件，没在真机测过 |
| zen-status.exe | 用 llvm-mingw 和 mingw-w64 gcc 交叉编译通过，只导入 kernel32 / shell32；在 Wine 里跑过命令行和三种钩子输入 |
| 按进程名显示 | 尽力方案：mod 扫进程列表，约 3 秒刷新一次 |

PowerShell 脚本全部是 ASCII，避免 Windows PowerShell 5.1 把无 BOM 脚本当 ANSI 读坏。中文内容走 UTF-8 JSON。
