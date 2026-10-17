# Codex `notify` adapter. Codex runs the configured program with ONE extra argument: a JSON string
# whose "type" is currently only "agent-turn-complete". Use this if you don't want Codex hooks;
# it can only report "done", not "working". ASCII-only (see zen-ai-status-lib.ps1).
param([Parameter(ValueFromRemainingArguments = $true)][string[]]$Rest)
. (Join-Path $PSScriptRoot 'zen-ai-status-lib.ps1')
try {
    $n = ($Rest -join ' ') | ConvertFrom-Json
    if ([string]$n.type -eq 'agent-turn-complete') {
        $proj = ''
        if ($n.cwd) { $proj = Split-Path -Leaf ([string]$n.cwd) }
        Write-ZenStatus -Agent codex -State done -Project $proj
    }
} catch { }
exit 0
