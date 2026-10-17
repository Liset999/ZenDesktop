# Hook adapter for Claude Code hooks and Codex hooks. Both send one JSON object on stdin with
# hook_event_name, cwd and (for tool events) tool_name. Always exits 0 and prints nothing,
# so it can never block or change what the agent does. ASCII-only (see zen-ai-status-lib.ps1).
param([string]$Agent = 'claude-code', [string]$Name)

. (Join-Path $PSScriptRoot 'zen-ai-status-lib.ps1')

try {
    $stdin = [Console]::OpenStandardInput()
    $ms = New-Object IO.MemoryStream
    $stdin.CopyTo($ms)
    $raw = [Text.Encoding]::UTF8.GetString($ms.ToArray())
    $e = $raw | ConvertFrom-Json
} catch { exit 0 }
if (-not $e) { exit 0 }

$proj = ''
if ($e.cwd) { try { $proj = Split-Path -Leaf ([string]$e.cwd) } catch { $proj = '' } }
$tool = [string]$e.tool_name
if ($tool -like 'mcp__zen-status__*') { exit 0 }  # our own MCP call: let it set the state itself
$common = @{ Agent = $Agent; Project = $proj }
if ($Name) { $common.Name = $Name }

try {
    switch ([string]$e.hook_event_name) {
        'SessionStart'       { Write-ZenStatus @common -State idle -ClearMessage }
        'UserPromptSubmit'   { Write-ZenStatus @common -State working -ClearMessage }
        'PreToolUse'         { Write-ZenStatus @common -State tool -Detail $tool -Weak }
        'PostToolUse'        { Write-ZenStatus @common -State working -Weak }
        'PostToolUseFailure' { Write-ZenStatus @common -State working -Weak }
        'PermissionRequest'  { Write-ZenStatus @common -State waiting -Detail $tool }
        'Notification' {
            # permission_prompt / elicitation_dialog = really waiting for you; idle_prompt etc. are ignored
            $t = [string]$e.notification_type
            if ($t -eq 'permission_prompt' -or $t -like 'elicitation*' -or $t -eq 'agent_needs_input') {
                Write-ZenStatus @common -State waiting
            }
        }
        'Stop'               { Write-ZenStatus @common -State done }
        'StopFailure'        { Write-ZenStatus @common -State error -Detail ([string]$e.error_type) }
        'Interrupt'          { Write-ZenStatus @common -State idle }
        'SessionEnd'         { Write-ZenStatus @common -State idle }
        default { }
    }
} catch { }
exit 0
