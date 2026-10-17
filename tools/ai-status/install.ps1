# Copies the ZenDesktop AI status scripts to %LOCALAPPDATA%\ZenDesktop\ai-status-tools\ and writes
# ready-to-paste config snippets with your real paths into ...\ai-status-tools\snippets\.
# It does NOT edit any agent's config file. ASCII-only (Windows PowerShell 5.1 reads BOM-less as ANSI).
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

$jsonPath = $dst.Replace('\', '\\')
$utf8 = New-Object Text.UTF8Encoding $false
$map = @(
    @('claude-code\settings-snippet.json', 'claude-code-settings.json'),
    @('codex\hooks.json', 'codex-hooks.json'),
    @('codex\config-snippet.toml', 'codex-config.toml'),
    @('opencode\opencode-snippet.json', 'opencode.json')
)
foreach ($m in $map) {
    $text = [IO.File]::ReadAllText((Join-Path $src $m[0]), [Text.Encoding]::UTF8)
    $text = $text.Replace('__TOOLS_DIR_RAW__', $dst).Replace('__TOOLS_DIR__', $jsonPath)
    [IO.File]::WriteAllText((Join-Path $snip $m[1]), $text, $utf8)
}
Copy-Item -LiteralPath (Join-Path $src 'AGENTS-snippet.md') -Destination $snip -Force

$mcp = 'powershell.exe -NoProfile -ExecutionPolicy Bypass -File "' + (Join-Path $dst 'zen-status-mcp.ps1') + '" -Agent claude-code'
Write-Host ''
Write-Host "Installed to: $dst"
Write-Host "Snippets:     $snip"
Write-Host ''
Write-Host 'Claude Code MCP (run once):'
Write-Host ('  claude mcp add --scope user zen-status -- ' + $mcp)
Write-Host ''
Write-Host 'Then paste the snippets (see tools\ai-status\README.md):'
Get-ChildItem -LiteralPath $snip | ForEach-Object { Write-Host ('  ' + $_.FullName) }
Write-Host ''
Write-Host 'opencode plugin: copy this file to %USERPROFILE%\.config\opencode\plugins\'
Write-Host ('  ' + (Join-Path $dst 'zen-status.js'))
Write-Host ''
Write-Host 'Quick test (should show on the taskbar within a second):'
Write-Host ('  powershell -NoProfile -ExecutionPolicy Bypass -File "' + (Join-Path $dst 'zen-ai-status.ps1') + '" -Agent test -State working -Message hello')
