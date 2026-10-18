# ZenDesktop AI status writer library (functions only, safe to dot-source).
# Writes %LOCALAPPDATA%\ZenDesktop\ai-status\<agent>.json atomically (temp file + replace), read by the
# "ZenDesktop: Taskbar Music & Lyrics" Windhawk mod. States: working, tool, waiting, done, error, idle (idle = hidden).
# ASCII-only on purpose: Windows PowerShell 5.1 reads BOM-less scripts as ANSI.

function Write-ZenStatus {
    param(
        [Parameter(Mandatory = $true)][string]$Agent,
        [Parameter(Mandatory = $true)][ValidateSet('working', 'tool', 'waiting', 'done', 'error', 'idle')][string]$State,
        [string]$Message,
        [string]$Detail = '',
        [string]$Project,
        [string]$Name,
        [switch]$ClearMessage,
        # New turn (Antigravity PreInvocation): clear the message only if the last state was done/error/idle.
        [switch]$ClearIfFinished,
        # Weak = from an async tool hook. It may finish after the turn's Stop hook, so it must not
        # overwrite a done/error/idle written in the last few seconds.
        [switch]$Weak
    )
    $base = $env:LOCALAPPDATA
    if (-not $base) { $base = Join-Path $env:USERPROFILE 'AppData\Local' }
    $dir = Join-Path $base 'ZenDesktop\ai-status'
    if (-not (Test-Path -LiteralPath $dir)) { New-Item -ItemType Directory -Force -Path $dir | Out-Null }
    $safe = ($Agent -replace '[^A-Za-z0-9_.-]', '_')
    $path = Join-Path $dir ($safe + '.json')

    $old = $null
    if (Test-Path -LiteralPath $path) {
        try { $old = [IO.File]::ReadAllText($path, [Text.Encoding]::UTF8) | ConvertFrom-Json } catch { $old = $null }
    }
    if ($Weak -and $old -and @('done', 'error', 'idle') -contains [string]$old.state) {
        $age = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds() - [long]$old.updated
        if ($age -ge 0 -and $age -le 5) { return }
    }
    # message (from the agent via MCP) is sticky until a new prompt clears it; hooks only touch state/detail.
    if ($PSBoundParameters.ContainsKey('Message')) { $msg = $Message }
    elseif ($ClearMessage -or -not $old) { $msg = '' }
    elseif ($ClearIfFinished -and @('done', 'error', 'idle') -contains [string]$old.state) { $msg = '' }
    else { $msg = [string]$old.message }
    if ($PSBoundParameters.ContainsKey('Project')) { $proj = $Project }
    elseif ($old) { $proj = [string]$old.project } else { $proj = '' }
    if (-not $Name -and $old) { $Name = [string]$old.name }
    if ($msg.Length -gt 120) { $msg = $msg.Substring(0, 120) }
    if ($Detail.Length -gt 60) { $Detail = $Detail.Substring(0, 60) }

    $obj = [ordered]@{
        agent   = $Agent
        name    = $Name
        state   = $State
        message = $msg
        detail  = $Detail
        project = $proj
        updated = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds()
    }
    $json = $obj | ConvertTo-Json -Compress
    $tmp = $path + '.' + $PID + '.tmp'
    $utf8 = New-Object Text.UTF8Encoding $false
    [IO.File]::WriteAllText($tmp, $json, $utf8)
    # $null would be converted to "" for a .NET string parameter, so pass a real null backup path.
    $noBackup = [System.Management.Automation.Language.NullString]::Value
    for ($i = 0; $i -lt 5; $i++) {
        try {
            if ([IO.File]::Exists($path)) { [IO.File]::Replace($tmp, $path, $noBackup) } else { [IO.File]::Move($tmp, $path) }
            return
        } catch { Start-Sleep -Milliseconds 40 }
    }
    # Last resort (not atomic): overwrite in place.
    try { [IO.File]::Copy($tmp, $path, $true) } catch { }
    Remove-Item -LiteralPath $tmp -ErrorAction SilentlyContinue
}

