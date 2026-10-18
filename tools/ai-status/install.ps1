# Copies the ZenDesktop AI status tools to %LOCALAPPDATA%\ZenDesktop\ai-status-tools\ and writes
# ready-to-paste config snippets with your real paths into ...\ai-status-tools\snippets\.
# It does NOT edit any agent's config file. ASCII-only (Windows PowerShell 5.1 reads BOM-less as ANSI).
#
# Hooks use zen-status.exe (starts in milliseconds) when it is available next to this script
# (zen-status.exe or zen-status\zen-status.exe, built by GitHub Actions), otherwise the PowerShell
# hook script. -PowerShellHooks forces the PowerShell version.
param([switch]$PowerShellHooks)
$ErrorActionPreference = 'Stop'
$src = $PSScriptRoot
$base = $env:LOCALAPPDATA
if (-not $base) { $base = Join-Path $env:USERPROFILE 'AppData\Local' }
$dst = Join-Path $base 'ZenDesktop\ai-status-tools'
$snip = Join-Path $dst 'snippets'
New-Item -ItemType Directory -Force -Path $dst, $snip | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $base 'ZenDesktop\ai-status') | Out-Null

foreach ($f in 'zen-ai-status-lib.ps1', 'zen-ai-status.ps1', 'agent-hook.ps1', 'codex-notify.ps1', 'zen-status-mcp.ps1') {
    Copy-Item -LiteralPath (Join-Path $src $f) -Destination $dst -Force
}
Copy-Item -LiteralPath (Join-Path $src 'opencode\zen-status.js') -Destination $dst -Force

# zen-status.exe (optional)
$exeDst = Join-Path $dst 'zen-status.exe'
$exeSrc = $null
foreach ($c in (Join-Path $src 'zen-status.exe'), (Join-Path $src 'zen-status\zen-status.exe')) {
    if (-not $exeSrc -and (Test-Path -LiteralPath $c)) { $exeSrc = $c }
}
if ($exeSrc) {
    try { Copy-Item -LiteralPath $exeSrc -Destination $exeDst -Force }
    catch { Write-Host "Could not copy zen-status.exe (in use by a running hook?): $($_.Exception.Message)" }
}
$useExe = (-not $PowerShellHooks) -and (Test-Path -LiteralPath $exeDst)

# Command-string form of the exe path (Codex / Antigravity run hook commands through a shell).
# Avoid quotes so it works in both cmd and PowerShell: use the 8.3 short path if the path has spaces.
$exeCmd = $exeDst
if ($exeCmd.Contains(' ')) {
    try { $exeCmd = (New-Object -ComObject Scripting.FileSystemObject).GetFile($exeDst).ShortPath } catch { }
    if (-not $exeCmd -or $exeCmd.Contains(' ')) { $exeCmd = '"' + $exeDst + '"' }
}

$jsonPath = $dst.Replace('\', '\\')
$jsonExe = $exeCmd.Replace('\', '\\').Replace('"', '\"')
$utf8 = New-Object Text.UTF8Encoding $false
if ($useExe) { $sfx = '' } else { $sfx = '-powershell' }
$map = @(
    @("claude-code\settings-snippet$sfx.json", 'claude-code-settings.json'),
    @("codex\hooks$sfx.json", 'codex-hooks.json'),
    @('codex\config-snippet.toml', 'codex-config.toml'),
    @('opencode\opencode-snippet.json', 'opencode.json'),
    @('antigravity\mcp_config-snippet.json', 'antigravity-mcp_config.json'),
    @("antigravity\hooks$sfx.json", 'antigravity-hooks.json')
)
foreach ($m in $map) {
    $text = [IO.File]::ReadAllText((Join-Path $src $m[0]), [Text.Encoding]::UTF8)
    $text = $text.Replace('__ZEN_EXE__', $jsonExe).Replace('__TOOLS_DIR_RAW__', $dst).Replace('__TOOLS_DIR__', $jsonPath)
    [IO.File]::WriteAllText((Join-Path $snip $m[1]), $text, $utf8)
}
Copy-Item -LiteralPath (Join-Path $src 'AGENTS-snippet.md') -Destination $snip -Force
Copy-Item -LiteralPath (Join-Path $src 'antigravity\GEMINI-snippet.md') -Destination $snip -Force

$mcp = 'powershell.exe -NoProfile -ExecutionPolicy Bypass -File "' + (Join-Path $dst 'zen-status-mcp.ps1') + '" -Agent claude-code'
$home2 = $env:USERPROFILE
Write-Host ''
Write-Host "Installed to: $dst"
Write-Host "Snippets:     $snip"
if ($useExe) { Write-Host 'Hooks use:    zen-status.exe (fast)' }
else { Write-Host 'Hooks use:    agent-hook.ps1 (PowerShell; put zen-status.exe next to install.ps1 for faster hooks)' }
Write-Host ''
Write-Host 'Claude Code MCP (run once):'
Write-Host ('  claude mcp add --scope user zen-status -- ' + $mcp)
Write-Host ''
Write-Host 'Antigravity (nothing was changed, merge these yourself):'
Write-Host ('  MCP:   "mcpServers" from snippets\antigravity-mcp_config.json  ->  ' + (Join-Path $home2 '.gemini\config\mcp_config.json'))
Write-Host '         (older versions: .gemini\antigravity\mcp_config.json; "View raw config" in Antigravity opens the right one)'
Write-Host ('  Hooks: snippets\antigravity-hooks.json  ->  ' + (Join-Path $home2 '.gemini\config\hooks.json'))
Write-Host ('  Rules: snippets\GEMINI-snippet.md  ->  append to ' + (Join-Path $home2 '.gemini\GEMINI.md'))
foreach ($t in (Join-Path $home2 '.gemini\config\mcp_config.json'), (Join-Path $home2 '.gemini\config\hooks.json')) {
    if (Test-Path -LiteralPath $t) { Write-Host ('  exists, merge by hand: ' + $t) }
}
Write-Host ''
Write-Host 'First set_status call: if your IDE/CLI asks for permission, choose "Always Allow",'
Write-Host 'otherwise every status update interrupts you.'
Write-Host ''
Write-Host 'Then paste the snippets (see tools\ai-status\README.md):'
Get-ChildItem -LiteralPath $snip | ForEach-Object { Write-Host ('  ' + $_.FullName) }
Write-Host ''
Write-Host 'opencode plugin: copy this file to %USERPROFILE%\.config\opencode\plugins\'
Write-Host ('  ' + (Join-Path $dst 'zen-status.js'))
Write-Host ''
Write-Host 'Quick test (should show on the taskbar within a second):'
if ($useExe) { Write-Host ('  & "' + $exeDst + '" test working hello') }
Write-Host ('  powershell -NoProfile -ExecutionPolicy Bypass -File "' + (Join-Path $dst 'zen-ai-status.ps1') + '" -Agent test -State working -Message hello')
