# ZenDesktop AI status, command-line form (for scripts and tools without hooks):
#   powershell -NoProfile -ExecutionPolicy Bypass -File zen-ai-status.ps1 -Agent mytool -State working -Message "..."
# States: working, tool, waiting, done, error, idle (idle = hidden). ASCII-only (PowerShell 5.1 reads BOM-less as ANSI).
param(
    [Parameter(Mandatory = $true)][string]$Agent,
    [Parameter(Mandatory = $true)][string]$State,
    [string]$Message,
    [string]$Detail = '',
    [string]$Project,
    [string]$Name
)
. (Join-Path $PSScriptRoot 'zen-ai-status-lib.ps1')
$p = @{ Agent = $Agent; State = $State; Detail = $Detail }
if ($PSBoundParameters.ContainsKey('Message')) { $p.Message = $Message }
if ($PSBoundParameters.ContainsKey('Project')) { $p.Project = $Project }
if ($Name) { $p.Name = $Name }
Write-ZenStatus @p
