# ZenDesktop AI 助手状态

任务栏音乐与歌词 mod（`zen-taskbar-lyrics`）会读取：

```
%LOCALAPPDATA%\ZenDesktop\ai-status\<助手名>.json
```

每个助手一个文件。JSON 字段：

| 字段 | 说明 |
|---|---|
| `agent` | 标识，例如 `claude-code`、`codex`、`opencode` |
| `name` | 显示名（可空，mod 会按 agent 猜） |
| `state` | `working` / `tool` / `waiting` / `done` / `error` / `idle`（idle 不显示） |
| `message` | 一句说明（建议不超过 20 个汉字） |
| `detail` | 短详情，例如工具名 |
| `project` | 项目文件夹名 |
| `updated` | Unix 秒（UTC） |

文件要「原子写入」（先写临时文件再替换），否则 mod 可能读到半截 JSON。

## 安装（推荐）

在 PowerShell 里，进入本目录后运行一次：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\install.ps1
```

它会：

1. 把脚本复制到 `%LOCALAPPDATA%\ZenDesktop\ai-status-tools\`
2. 把填好你本机路径的配置片段写到 `...\ai-status-tools\snippets\`
3. **不会**改任何助手的配置文件，片段要你自己粘贴

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

## 给没有钩子的工具

在 Windhawk 里打开「任务栏音乐与歌词」→「按进程名显示」，填进程名（逗号分隔），例如：

```text
gemini.exe, aider.exe
```

进程在运行时显示「运行中」。也可以在自己的脚本里直接调：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "$env:LOCALAPPDATA\ZenDesktop\ai-status-tools\zen-ai-status.ps1" -Agent mytool -State working -Message "在跑测试"
```

## 核对说明

| 适配器 | 状态 |
|---|---|
| Claude Code hooks + MCP | 已按官方 Hooks / MCP 文档核对；在本机用 PowerShell 7 跑过 hook 与 MCP `set_status` |
| Codex hooks + MCP + notify | 已按官方 Hooks / Config 文档核对；`notify` 参数形状按文档示例；hooks 需在 Codex 里 `/hooks` 信任后才跑 |
| opencode plugin + MCP | 已按官方 Plugins / MCP 文档核对；插件在 Node 里跑过写入 |
| 按进程名显示 | 尽力方案：mod 扫进程列表，约 3 秒刷新一次 |

PowerShell 脚本全部是 ASCII，避免 Windows PowerShell 5.1 把无 BOM 脚本当 ANSI 读坏。中文内容走 UTF-8 JSON。
