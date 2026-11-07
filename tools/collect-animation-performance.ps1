param([int]$Seconds = 45)
$ErrorActionPreference = 'Stop'
if ($Seconds -lt 1 -or $Seconds -gt 120) { throw 'Seconds must be 1..120' }
$reader = 'E:\Tools\Windhawk\UI\resources\app\extensions\windhawk\files\DbgViewMini.exe'
$logDir = Join-Path (Split-Path -Parent $PSScriptRoot) '.diagnostics'
New-Item -ItemType Directory -Path $logDir -Force | Out-Null
$log = Join-Path $logDir ('animation-performance-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '.log')
$start = New-Object System.Diagnostics.ProcessStartInfo
$start.FileName = $reader
$start.Arguments = '--pattern "[WH] *" --no-buffering'
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
$start.RedirectStandardOutput = $true
$process = New-Object System.Diagnostics.Process
$process.StartInfo = $start
$writer = New-Object System.IO.StreamWriter($log, $false, ([Text.UTF8Encoding]::new($false)))
try {
    if (-not $process.Start()) { throw 'Log reader failed to start' }
    Write-Output "Recording animation diagnostics for $Seconds seconds; log: $log"
    $deadline = [DateTime]::UtcNow.AddSeconds($Seconds)
    $lineTask = $process.StandardOutput.ReadLineAsync()
    $count = 0
    while ([DateTime]::UtcNow -lt $deadline -and -not $process.HasExited) {
        if ($lineTask.Wait(100)) {
            $line = $lineTask.Result
            if ($null -eq $line) { break }
            if ($line -match 'windows-animations' -and $line -match 'Perf summary|Animation renderer=|Screen capture|capture failed') {
                $writer.WriteLine($line)
                $writer.Flush()
                Write-Output $line
                $count++
            }
            $lineTask = $process.StandardOutput.ReadLineAsync()
        }
    }
    Write-Output "Captured $count relevant log line(s)."
} finally {
    if (-not $process.HasExited) { $process.Kill() }
    $writer.Dispose()
    $process.Dispose()
}
