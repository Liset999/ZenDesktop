# ============================================================
#  ZenDesktop startup self-check (optional, off by default)
#  PowerShell 5.1 compatible. Installed by: deploy.ps1 -StartupCheck check|restart
#  Remove with:  deploy.ps1 -StartupCheck remove
#            or: schtasks /Delete /TN "ZenDesktop\StartupCheck" /F
#
#  Runs once per logon (scheduled task, ~15s after logon). It restarts
#  explorer.exe AT MOST ONCE PER BOOT, and only when:
#    -Mode check   : explorer is missing, the taskbar window is missing/hung,
#                    or explorer.exe crashed since boot (Application log, ID 1000).
#    -Mode restart : always (automates the manual "restart explorer once after
#                    boot" workaround; use only if 'check' does not catch it).
#  Log: %LOCALAPPDATA%\ZenDesktop\startup-check.log (last ~200 lines kept)
# ============================================================
param(
    [ValidateSet("check", "restart")]
    [string]$Mode = "check",
    [int]$DelaySec = 15
)
$ErrorActionPreference = 'Continue'

$dir = Join-Path $env:LOCALAPPDATA "ZenDesktop"
if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }
$log = Join-Path $dir "startup-check.log"
$marker = Join-Path $dir "startup-check.lastboot"

function Write-Log([string]$msg) {
    $line = "{0}  {1}" -f (Get-Date -Format "yyyy-MM-dd HH:mm:ss"), $msg
    Add-Content -Path $log -Value $line -Encoding UTF8
}

try {
    $lines = Get-Content -Path $log -ErrorAction SilentlyContinue
    if ($lines -and $lines.Count -gt 200) {
        $lines | Select-Object -Last 200 | Set-Content -Path $log -Encoding UTF8
    }
} catch { }

Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public static class ZenWin {
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    public static extern IntPtr FindWindow(string cls, string title);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    public static extern IntPtr FindWindowEx(IntPtr parent, IntPtr after, string cls, string title);
    [DllImport("user32.dll")]
    public static extern bool IsHungAppWindow(IntPtr hWnd);
    [DllImport("user32.dll")]
    public static extern IntPtr SendMessageTimeout(IntPtr hWnd, uint msg, IntPtr w, IntPtr l, uint flags, uint timeout, out IntPtr result);
}
"@ -ErrorAction SilentlyContinue

Start-Sleep -Seconds $DelaySec

$os = Get-CimInstance -ClassName Win32_OperatingSystem
$bootTime = $os.LastBootUpTime
$bootKey = $bootTime.ToString("yyyyMMddHHmmss")
Write-Log "---- start (mode=$Mode, boot=$bootKey) ----"

if ((Test-Path $marker) -and ((Get-Content $marker -ErrorAction SilentlyContinue) -eq $bootKey)) {
    Write-Log "Already restarted explorer once this boot. Nothing to do."
    exit 0
}

$reasons = @()

$explorer = Get-Process -Name explorer -ErrorAction SilentlyContinue
if (-not $explorer) { $reasons += "explorer.exe not running" }

$tray = [ZenWin]::FindWindow("Shell_TrayWnd", $null)
if ($tray -eq [IntPtr]::Zero) {
    $reasons += "Shell_TrayWnd missing"
} else {
    $res = [IntPtr]::Zero
    # SMTO_ABORTIFHUNG = 0x2, WM_NULL = 0
    $ok = [ZenWin]::SendMessageTimeout($tray, 0, [IntPtr]::Zero, [IntPtr]::Zero, 2, 3000, [ref]$res)
    if ($ok -eq [IntPtr]::Zero -or [ZenWin]::IsHungAppWindow($tray)) { $reasons += "taskbar not responding" }
    $notify = [ZenWin]::FindWindowEx($tray, [IntPtr]::Zero, "TrayNotifyWnd", $null)
    Write-Log ("Shell_TrayWnd=0x{0:X} TrayNotifyWnd=0x{1:X}" -f $tray.ToInt64(), $notify.ToInt64())
}

try {
    $crashes = Get-WinEvent -FilterHashtable @{ LogName = 'Application'; Id = 1000; StartTime = $bootTime } -ErrorAction SilentlyContinue |
        Where-Object { $_.Message -match 'explorer\.exe' }
    foreach ($c in $crashes) {
        $first = ($c.Message -split "`n" | Select-Object -First 3) -join ' | '
        Write-Log ("explorer crash since boot @ {0}: {1}" -f $c.TimeCreated, $first.Trim())
    }
    if ($crashes) { $reasons += "explorer.exe crashed $(@($crashes).Count)x since boot" }
} catch { }

$blurDll = Get-Process -Name explorer -ErrorAction SilentlyContinue | ForEach-Object {
    try { $_.Modules | Where-Object { $_.ModuleName -ieq 'ExplorerBlurMica.dll' } } catch { }
}
Write-Log ("ExplorerBlurMica loaded in explorer: {0}" -f [bool]$blurDll)

if ($Mode -eq "restart") { $reasons += "mode=restart (unconditional)" }

if ($reasons.Count -eq 0) {
    Write-Log "Healthy. No restart."
    exit 0
}

Write-Log ("Restarting explorer once: " + ($reasons -join '; '))
Set-Content -Path $marker -Value $bootKey -Encoding ASCII
Stop-Process -Name explorer -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 3
if (-not (Get-Process -Name explorer -ErrorAction SilentlyContinue)) {
    Start-Process explorer.exe
}
Write-Log "Explorer restarted."
