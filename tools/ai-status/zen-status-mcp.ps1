# Minimal stdio MCP server "zen-status" with one tool, set_status. JSON-RPC 2.0, one message per line.
# Lets the agent itself report what it is doing; the ZenDesktop taskbar mod shows it.
# Usage: powershell.exe -NoProfile -ExecutionPolicy Bypass -File zen-status-mcp.ps1 -Agent claude-code
# ASCII-only (see zen-ai-status-lib.ps1); Chinese text arrives as UTF-8 JSON and is written as UTF-8.
param([string]$Agent = 'agent', [string]$Name)

. (Join-Path $PSScriptRoot 'zen-ai-status-lib.ps1')

$utf8 = New-Object Text.UTF8Encoding $false
$in = New-Object IO.StreamReader([Console]::OpenStandardInput(), $utf8)
$out = New-Object IO.StreamWriter([Console]::OpenStandardOutput(), $utf8)
$out.AutoFlush = $true
$out.NewLine = "`n"

function Send($obj) { $out.WriteLine(($obj | ConvertTo-Json -Compress -Depth 20)) }
function Reply($id, $result) { Send ([ordered]@{ jsonrpc = '2.0'; id = $id; result = $result }) }
function ReplyError($id, [int]$code, [string]$msg) { Send ([ordered]@{ jsonrpc = '2.0'; id = $id; error = @{ code = $code; message = $msg } }) }

$tool = [ordered]@{
    name        = 'set_status'
    description = 'Show your current work status on the user''s Windows taskbar. Call it when you start a task (working), need the user (waiting), finish (done) or are stuck (error). Only on changes, not every step.'
    inputSchema = [ordered]@{
        type       = 'object'
        properties = [ordered]@{
            state   = [ordered]@{ type = 'string'; enum = @('working', 'waiting', 'done', 'error', 'idle'); description = 'working | waiting (needs the user) | done | error | idle (hide)' }
            message = [ordered]@{ type = 'string'; description = 'One short sentence (max ~20 Chinese characters) about the task. No secrets or personal data.' }
        }
        required   = @('state')
    }
}

while ($null -ne ($line = $in.ReadLine())) {
    if (-not $line.Trim()) { continue }
    try { $req = $line | ConvertFrom-Json } catch { continue }
    $hasId = $req.PSObject.Properties.Name -contains 'id'
    $id = $req.id
    try {
        switch ([string]$req.method) {
            'initialize' {
                $pv = [string]$req.params.protocolVersion
                if (-not $pv) { $pv = '2025-06-18' }
                Reply $id ([ordered]@{
                    protocolVersion = $pv
                    capabilities    = @{ tools = @{} }
                    serverInfo      = @{ name = 'zen-status'; version = '0.2.0' }
                })
            }
            'ping' { Reply $id @{} }
            'tools/list' { Reply $id @{ tools = @($tool) } }
            'tools/call' {
                if ([string]$req.params.name -ne 'set_status') { ReplyError $id -32602 'unknown tool'; break }
                $a = $req.params.arguments
                $state = [string]$a.state
                if (@('working', 'waiting', 'done', 'error', 'idle') -notcontains $state) {
                    Reply $id @{ content = @(@{ type = 'text'; text = 'state must be working, waiting, done, error or idle' }); isError = $true }
                    break
                }
                $p = @{ Agent = $Agent; State = $state; Message = [string]$a.message }
                if ($Name) { $p.Name = $Name }
                Write-ZenStatus @p
                Reply $id @{ content = @(@{ type = 'text'; text = 'ok' }) }
            }
            default { if ($hasId -and $null -ne $id) { ReplyError $id -32601 'method not found' } }
        }
    } catch {
        if ($hasId -and $null -ne $id) { ReplyError $id -32603 'internal error' }
    }
}
