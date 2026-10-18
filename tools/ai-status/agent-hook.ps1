# Hook adapter for Claude Code, Codex and Antigravity hooks. All send one JSON object on stdin.
# Claude Code / Codex: the event is in hook_event_name; prints nothing.
# Antigravity: the payload has no event name, so pass -Event; a valid JSON answer is printed
# (PreToolUse -> {"decision":"allow"}, Stop -> {"decision":"stop"}, others -> {}).
# Always exits 0, so it can never block or change what the agent does.
# zen-status.exe does the same thing in a few milliseconds; this script is the fallback.
# ASCII-only (see zen-ai-status-lib.ps1).
param([string]$Agent = 'claude-code', [string]$Name, [string]$Event)

. (Join-Path $PSScriptRoot 'zen-ai-status-lib.ps1')

function Answer {
    if (-not $Event) { return }
    if ($Event -eq 'PreToolUse') { [Console]::Out.Write('{"decision":"allow"}') }
    elseif ($Event -eq 'Stop') { [Console]::Out.Write('{"decision":"stop"}') }
    else { [Console]::Out.Write('{}') }
}

try {
    $stdin = [Console]::OpenStandardInput()
    $ms = New-Object IO.MemoryStream
    $stdin.CopyTo($ms)
    $raw = [Text.Encoding]::UTF8.GetString($ms.ToArray())
    $e = $raw | ConvertFrom-Json
} catch { $e = $null }
if (-not $e) { Answer; exit 0 }

$ev = $Event
if (-not $ev) { $ev = [string]$e.hook_event_name }
$dir = [string]$e.cwd
if (-not $dir -and $e.workspacePaths) { $dir = [string]@($e.workspacePaths)[0] }  # Antigravity
$proj = ''
if ($dir) { try { $proj = Split-Path -Leaf $dir } catch { $proj = '' } }
$tool = [string]$e.tool_name
if (-not $tool -and $e.toolCall) { $tool = [string]$e.toolCall.name }  # Antigravity
if ($tool -like 'mcp__zen-status__*' -or $tool -like '*zen-status*' -or $tool -like '*zen_status*' -or $tool -eq 'set_status') {
    Answer; exit 0  # our own MCP call: let it set the state itself
}
$common = @{ Agent = $Agent }
if ($proj) { $common.Project = $proj }  # no cwd in this event: keep the old one
if ($Name) { $common.Name = $Name }

try {
    switch ($ev) {
        'SessionStart'       { Write-ZenStatus @common -State idle -ClearMessage }
        'UserPromptSubmit'   { Write-ZenStatus @common -State working -ClearMessage }
        'PreInvocation'      { Write-ZenStatus @common -State working -ClearIfFinished }
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
        'Stop' {
            if ([string]$e.terminationReason -eq 'error') { Write-ZenStatus @common -State error -Detail ([string]$e.error) }
            elseif ($e.PSObject.Properties.Name -contains 'fullyIdle' -and -not $e.fullyIdle) { Write-ZenStatus @common -State working }
            else { Write-ZenStatus @common -State done }
        }
        'StopFailure'        { Write-ZenStatus @common -State error -Detail ([string]$e.error_type) }
        'Interrupt'          { Write-ZenStatus @common -State idle }
        'SessionEnd'         { Write-ZenStatus @common -State idle }
        default { }
    }
} catch { }
Answer
exit 0
