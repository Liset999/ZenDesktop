# ============================================================
#  ZenDesktop Direct Injection Deploy Engine
#  Refined v4.2.0 - Full Chinese Localization & Dynamic Engine
# ============================================================
# v4.2-draft: optional mods can be preselected for unattended runs, e.g.
#   deploy.ps1 -Optional "wobbly-windows,windows-animations"
#   deploy.ps1 -Optional none
# Optional startup self-check (scheduled task at logon, off by default):
#   deploy.ps1 -StartupCheck check    # restart explorer once only if taskbar missing/hung/crashed
#   deploy.ps1 -StartupCheck restart  # always restart explorer once per boot (manual workaround, automated)
#   deploy.ps1 -StartupCheck remove   # uninstall the task
param(
    [string]$Optional = "",
    [ValidateSet("", "off", "check", "restart", "remove")]
    [string]$StartupCheck = ""
)
$ErrorActionPreference = 'Stop'

# Helper function to print colorful status
function Write-Status ($text, $color = "Cyan") {
    Write-Host "[ZenDeploy] $text" -ForegroundColor $color
}

Write-Status "================================================" "Green"
Write-Status "   ZenDesktop Direct Injection Engine v4.2.0" "Green"
Write-Status "================================================" "Green"

# 1. Enforce Administrator Privileges
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Error "This script must be executed with Administrator privileges. Please run deploy.bat as Administrator."
    Exit 1
}

# 2. Detect Windhawk Paths
Write-Status "Detecting Windhawk installation..."
$windhawkDir = $null

# Registry check
$regPaths = @(
    "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk",
    "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk"
)
foreach ($regPath in $regPaths) {
    if (Test-Path $regPath) {
        $windhawkDir = Get-ItemPropertyValue -Path $regPath -Name "InstallLocation" -ErrorAction SilentlyContinue
        if ($windhawkDir) { break }
    }
}

# Active process fallback
if (-not $windhawkDir) {
    $proc = Get-Process windhawk -ErrorAction SilentlyContinue | Where-Object { $_.Path } | Select-Object -First 1
    if ($proc -and $proc.Path) {
        $windhawkDir = Split-Path $proc.Path
    }
}

# Standard paths fallbacks
$fallbacks = @(
    "C:\Program Files\Windhawk",
    "$PSScriptRoot\Windhawk",
    $PSScriptRoot
)
foreach ($fallback in $fallbacks) {
    if (-not $windhawkDir -and (Test-Path (Join-Path $fallback "windhawk.exe"))) {
        $windhawkDir = $fallback
    }
}

if (-not $windhawkDir) {
    Write-Error "Windhawk executable could not be resolved! Please ensure Windhawk is installed."
    Exit 1
}

# Normalize path (remove trailing backslash)
$windhawkDir = $windhawkDir.TrimEnd('\')
Write-Status "Windhawk Home: $windhawkDir" "DarkGreen"

# Determine data root by parsing windhawk.ini if it exists (Standard vs Portable)
$windhawkData = "C:\ProgramData\Windhawk" # Standard default fallback
$iniPath = Join-Path $windhawkDir "windhawk.ini"
if (Test-Path $iniPath) {
    Write-Status "Parsing windhawk.ini for active storage configuration..." "Gray"
    $ini = Get-Content -Path $iniPath
    
    # Parse Portable setting
    $isPortable = $false
    foreach ($line in $ini) {
        if ($line -match '^\s*Portable\s*=\s*(\d+)') {
            if ($Matches[1] -eq "1") { $isPortable = $true }
        }
    }
    
    # Parse AppDataPath setting
    $foundAppDataPath = $null
    foreach ($line in $ini) {
        if ($line -match '^\s*AppDataPath\s*=\s*(.+)') {
            $foundAppDataPath = $Matches[1].Trim()
        }
    }
    
    if ($foundAppDataPath) {
        $windhawkData = [System.Environment]::ExpandEnvironmentVariables($foundAppDataPath)
        if (-not [System.IO.Path]::IsPathRooted($windhawkData)) {
            $windhawkData = [System.IO.Path]::GetFullPath((Join-Path $windhawkDir $windhawkData))
        }
    } elseif ($isPortable) {
        $windhawkData = Join-Path $windhawkDir "AppData"
    }
} else {
    if (Test-Path "$windhawkDir\AppData\ModsSource") {
        $windhawkData = "$windhawkDir\AppData"
    }
}

Write-Status "Windhawk Data Folder: $windhawkData" "DarkGreen"

# 3. Stop Windhawk Service and UI
# Global Process Exclusion for Games & Anti-Cheat (ACE, Vanguard, EAC, etc.)
$globalExcludeList = @(
    "LeagueClient.exe", "LeagueClientUx.exe", "LeagueClientUxRender.exe", "League of Legends.exe",
    "RiotClientServices.exe", "RiotClientUx.exe", "RiotClientCrashHandler.exe",
    "wegame.exe", "WeGameLauncher.exe", "tgp_daemon.exe", "SGuard64.exe", "SGuard32.exe",
    "SGuardUpdate.exe", "TenSafe.exe", "TenSafe_1.exe", "CrossProxy.exe", "Client.exe",
    "vgc.exe", "vgtray.exe", "VALORANT.exe", "VALORANT-Win64-Shipping.exe",
    "EasyAntiCheat.exe", "EasyAntiCheat_EOS.exe", "BEService.exe", "BEService_x64.exe",
    "YuanShen.exe", "GenshinImpact.exe", "StarRail.exe", "ZenlessZoneZero.exe", "HYP.exe",
    "HoYoPlay.exe", "steam.exe", "steamwebhelper.exe", "EpicGamesLauncher.exe"
) -join "|"

$engineSettingsPath = "HKLM:\SOFTWARE\Windhawk\Engine\Settings"
if (-not (Test-Path $engineSettingsPath)) {
    New-Item -Path $engineSettingsPath -Force | Out-Null
}
Set-ItemProperty -Path $engineSettingsPath -Name "Exclude" -Value $globalExcludeList -Type String
Set-ItemProperty -Path $engineSettingsPath -Name "InjectIntoGames" -Value 0 -Type DWord

Write-Status "Stopping Windhawk Service & UI..."
Stop-Process -Name windhawk -Force -ErrorAction SilentlyContinue
Stop-Service -Name Windhawk -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1
Write-Status "Windhawk successfully stopped." "Green"

# 4. Copy and Inject Active Mods
Write-Status "Scanning and deploying active mod files..."
$modsSource = Join-Path $windhawkData "ModsSource"
if (-not (Test-Path $modsSource)) {
    New-Item -ItemType Directory -Path $modsSource -Force | Out-Null
}

# Clean retired legacy mods from Windhawk to prevent conflicts
$retiredMods = @("local@taskbar-background-helper", "local@start-menu-size")
foreach ($rMod in $retiredMods) {
    $legacyFile = Join-Path $modsSource "$rMod.wh.cpp"
    if (Test-Path $legacyFile) {
        Write-Status "Removing retired mod source: $rMod" "Yellow"
        Remove-Item -Path $legacyFile -Force -ErrorAction SilentlyContinue
    }
    $legacyReg = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$rMod"
    if (Test-Path $legacyReg) {
        Remove-Item -Path $legacyReg -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# Clean old mod files in portable leftovers if operating in standard mode
if ($windhawkData -ne "$windhawkDir\AppData" -and (Test-Path "$windhawkDir\AppData\ModsSource")) {
    Write-Status "Cleaning leftover ModsSource in portable folder..." "Yellow"
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@zen-*" -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@macos-*" -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@windows-11-settings-*" -Force -ErrorAction SilentlyContinue
}

$files = @(Get-ChildItem -Path $PSScriptRoot -Filter "local@*.wh.cpp")
if ($files.Count -eq 0) {
    Write-Error "No local@*.wh.cpp mod files found in script directory! Checked: $PSScriptRoot"
    Exit 1
}

# v4.2: optional mods live in .\optional and are deployed to Windhawk automatically.
# Core mods are enabled (Disabled=0) by default.
# Optional mods (wobbly-windows, windows-animations) are disabled (Disabled=1) by default,
# allowing users to toggle them ON in the Windhawk UI at any time without command-line prompts.
# Preselection via -Optional parameter (e.g. -Optional "wobbly-windows") remains supported for unattended runs.
$optionalDir = Join-Path $PSScriptRoot "optional"
$optionalModIds = @("local@wobbly-windows", "local@windows-animations")
$preselected = @()
if ($Optional) {
    $preselected = $Optional.Split(",") | ForEach-Object { $_.Trim() } | Where-Object { $_ -and $_ -ne "none" } | ForEach-Object {
        if ($_ -like "local@*") { $_ } else { "local@$_" }
    }
}
$dwmModSelected = $false
if (Test-Path $optionalDir) {
    $optFiles = @(Get-ChildItem -Path $optionalDir -Filter "local@*.wh.cpp")
    foreach ($optFile in $optFiles) {
        $files += $optFile
    }
}

foreach ($file in $files) {
    # Dynamically derive Mod ID from filename
    # Example: 'local@zen-taskbar-acrylic.wh.cpp' -> 'local@zen-taskbar-acrylic'
    $id = $file.BaseName -replace '\.wh$', ''
    Write-Status "Deploying Mod: $id ($($file.Name))..." "Cyan"
    
    # Copy source code to Windhawk
    Copy-Item -Path $file.FullName -Destination (Join-Path $modsSource $file.Name) -Force
    
    # Read and parse metadata directly from the source code headers
    $content = Get-Content -Path $file.FullName -Raw
    
    # @version
    $version = "4.1.0"
    if ($content -match '// @version\s+(.+)') {
        $version = $Matches[1].Trim()
    }
    
    # @architecture
    $arch = "x86-64"
    if ($content -match '// @architecture\s+(.+)') {
        $arch = $Matches[1].Trim()
    }
    
    # @include
    $includes = @()
    $matchesInclude = [regex]::Matches($content, '// @include\s+(.+)')
    foreach ($m in $matchesInclude) {
        $includes += $m.Groups[1].Value.Trim()
    }
    $includeStr = $includes -join "|"
    if (-not $includeStr) {
        $includeStr = "explorer.exe"
    }

    # @exclude
    $excludes = @()
    $matchesExclude = [regex]::Matches($content, '// @exclude\s+(.+)')
    foreach ($m in $matchesExclude) {
        $excludes += $m.Groups[1].Value.Trim()
    }
    $excludeStr = $excludes -join "|"
    
    Write-Status "         [Metadata] Version=$version | Arch=$arch | Include=$includeStr" "Gray"
    
    # Registry path under Windhawk Engine
    $regPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$id"
    if (-not (Test-Path $regPath)) {
        New-Item -Path $regPath -Force | Out-Null
    }

    # Determine Disabled state:
    # 1. Optional mods default to Disabled=1 (off) so user can toggle them in Windhawk UI.
    # 2. If -Optional parameter was passed, obey it.
    # 3. Otherwise preserve user's existing toggle state if mod was already registered.
    $isOptional = $optionalModIds -contains $id
    $currentDisabled = $null
    try {
        $prop = Get-ItemProperty -Path $regPath -Name "Disabled" -ErrorAction SilentlyContinue
        if ($null -ne $prop) { $currentDisabled = $prop.Disabled }
    } catch {
        $currentDisabled = $null
    }

    $targetDisabled = 0
    if ($isOptional) {
        if ($Optional) {
            $targetDisabled = if ($preselected -contains $id) { 0 } else { 1 }
        } elseif ($null -ne $currentDisabled) {
            $targetDisabled = $currentDisabled
        } else {
            $targetDisabled = 1  # Default OFF for optional mods on first install
        }
        if ($id -eq "local@wobbly-windows" -and $targetDisabled -eq 0) {
            $dwmModSelected = $true
        }
    } else {
        if ($null -ne $currentDisabled) {
            $targetDisabled = $currentDisabled
        } else {
            $targetDisabled = 0
        }
    }
    
    # Populate the registry with metadata
    Set-ItemProperty -Path $regPath -Name "Disabled" -Value $targetDisabled -Type DWord
    Set-ItemProperty -Path $regPath -Name "LoggingEnabled" -Value 0 -Type DWord
    Set-ItemProperty -Path $regPath -Name "Include" -Value $includeStr -Type String
    Set-ItemProperty -Path $regPath -Name "Exclude" -Value $excludeStr -Type String
    Set-ItemProperty -Path $regPath -Name "Architecture" -Value $arch -Type String
    Set-ItemProperty -Path $regPath -Name "Version" -Value $version -Type String
    Set-ItemProperty -Path $regPath -Name "LibraryFileName" -Value "" -Type String

    # Custom initial settings initialization per mod
    if ($id -eq "local@zen-taskbar-acrylic") {
        $settingsPath = Join-Path $regPath "Settings"
        if (-not (Test-Path $settingsPath)) {
            New-Item -Path $settingsPath -Force | Out-Null
        }

        $currentTheme = $null
        try {
            $currentTheme = (Get-ItemProperty -Path $settingsPath -Name "theme" -ErrorAction SilentlyContinue).theme
        } catch {
            $currentTheme = $null
        }

        if ([string]::IsNullOrWhiteSpace($currentTheme)) {
            Write-Status "         [Settings] Initializing taskbar theme -> TranslucentTaskbar" "Yellow"
            Set-ItemProperty -Path $settingsPath -Name "theme" -Value "TranslucentTaskbar" -Type String
        }

        $maxLayerVal = $null
        try {
            $maxLayerVal = (Get-ItemProperty -Path $settingsPath -Name "maximizedTaskbarLayer" -ErrorAction SilentlyContinue).maximizedTaskbarLayer
        } catch {
            $maxLayerVal = $null
        }
        if ($null -eq $maxLayerVal) {
            Set-ItemProperty -Path $settingsPath -Name "maximizedTaskbarLayer" -Value 0 -Type DWord
        }

        # 4.1.2: never leave XAML diagnostics on "alert" - the MessageBox blocks the
        # first File Explorer window thread when ExplorerBlurMica 2.x asks for it.
        # Keep an explicit "block" (user picked taskbar over Explorer blur).
        $xdh = $null
        try {
            $xdh = (Get-ItemProperty -Path $settingsPath -Name "xamlDiagnosticsHandling" -ErrorAction SilentlyContinue).xamlDiagnosticsHandling
        } catch {
            $xdh = $null
        }
        if ([string]::IsNullOrWhiteSpace($xdh) -or $xdh -eq "alert") {
            Write-Status "         [Settings] xamlDiagnosticsHandling -> allow (was: '$xdh')" "Yellow"
            Set-ItemProperty -Path $settingsPath -Name "xamlDiagnosticsHandling" -Value "allow" -Type String
        }
    }

    if ($id -eq "local@macos-minimize-animation") {
        $settingsPath = Join-Path $regPath "Settings"
        if (-not (Test-Path $settingsPath)) {
            New-Item -Path $settingsPath -Force | Out-Null
        }

        $defaults = @{
            "duration_ms" = 450
            "open_animation" = 1
            "launch_animation" = 0
            "multi_monitor" = 0
            "unhide_taskbar" = 1
            "unhide_duration_ms" = 450
            "tile_count" = 35
        }

        foreach ($key in $defaults.Keys) {
            $val = $null
            try {
                $val = (Get-ItemProperty -Path $settingsPath -Name $key -ErrorAction SilentlyContinue).$key
            } catch {
                $val = $null
            }
            if ($val -eq $null) {
                Write-Status "         [Settings] Initializing macOS Genie $key -> $($defaults[$key])" "Yellow"
                Set-ItemProperty -Path $settingsPath -Name $key -Value $defaults[$key] -Type DWord
            }
        }

        $engineVal = $null
        try {
            $engineVal = (Get-ItemProperty -Path $settingsPath -Name "engine" -ErrorAction SilentlyContinue).engine
        } catch {
            $engineVal = $null
        }
        if ([string]::IsNullOrWhiteSpace($engineVal)) {
            Set-ItemProperty -Path $settingsPath -Name "engine" -Value "modern" -Type String
        }
    }

    if ($id -eq "local@zen-startmenu-acrylic") {
        $settingsPath = Join-Path $regPath "Settings"
        if (-not (Test-Path $settingsPath)) {
            New-Item -Path $settingsPath -Force | Out-Null
        }

        $defaults = @{
            "width" = 0
            "height" = 0
        }

        foreach ($key in $defaults.Keys) {
            $val = $null
            try {
                $val = (Get-ItemProperty -Path $settingsPath -Name $key -ErrorAction SilentlyContinue).$key
            } catch {
                $val = $null
            }
            if ($val -eq $null) {
                Write-Status "         [Settings] Initializing StartMenu Acrylic $key -> $($defaults[$key])" "Yellow"
                Set-ItemProperty -Path $settingsPath -Name $key -Value $defaults[$key] -Type DWord
            }
        }
    }

    if ($id -eq "local@windows-11-settings-styler-fork") {
        $settingsPath = Join-Path $regPath "Settings"
        if (-not (Test-Path $settingsPath)) {
            New-Item -Path $settingsPath -Force | Out-Null
        }

        $stringDefaults = @{
            "theme" = "TranslucentSettings"
            "bgColorMode" = "Default"
            "textColorMode" = "default"
        }
        foreach ($key in $stringDefaults.Keys) {
            $val = $null
            try {
                $val = (Get-ItemProperty -Path $settingsPath -Name $key -ErrorAction SilentlyContinue).$key
            } catch {
                $val = $null
            }
            if ([string]::IsNullOrWhiteSpace($val)) {
                Write-Status "         [Settings] Initializing Settings Acrylic $key -> $($stringDefaults[$key])" "Yellow"
                Set-ItemProperty -Path $settingsPath -Name $key -Value $stringDefaults[$key] -Type String
            }
        }

        $intDefaults = @{
            "blurPreset" = 0
            "opacityPreset" = 0
            "luminosityPreset" = 0
        }
        foreach ($key in $intDefaults.Keys) {
            $val = $null
            try {
                $prop = Get-ItemProperty -Path $settingsPath -Name $key -ErrorAction SilentlyContinue
                if ($null -ne $prop) { $val = $prop.$key }
            } catch {
                $val = $null
            }
            if ($null -eq $val) {
                Write-Status "         [Settings] Initializing Settings Acrylic $key -> $($intDefaults[$key])" "Yellow"
                Set-ItemProperty -Path $settingsPath -Name $key -Value $intDefaults[$key] -Type DWord
            }
        }
    }

    if ($id -eq "local@windows-animations") {
        # v4.2-draft [UNTESTED ON WINDOWS]: ZenDesktop default = close effect only.
        # Nested Windhawk settings are stored as "group.key" values; booleans as DWORD 0/1.
        $settingsPath = Join-Path $regPath "Settings"
        if (-not (Test-Path $settingsPath)) {
            New-Item -Path $settingsPath -Force | Out-Null
        }
        $waDefaults = [ordered]@{
            "close.close_animation"               = 1
            "minimize_restore.minimize_animation" = 0
            "minimize_restore.restore_animation"  = 0
            "minimize_restore.launch_animation"   = 0
            "window_switch.switch_animation"      = 0
        }
        foreach ($key in $waDefaults.Keys) {
            $val = $null
            try {
                $prop = Get-ItemProperty -Path $settingsPath -Name $key -ErrorAction SilentlyContinue
                if ($null -ne $prop) { $val = $prop.$key }
            } catch {
                $val = $null
            }
            if ($null -eq $val) {
                Write-Status "         [Settings] Initializing Windows Animations $key -> $($waDefaults[$key])" "Yellow"
                Set-ItemProperty -Path $settingsPath -Name $key -Value $waDefaults[$key] -Type DWord
            }
        }

        # If Lanbo's macOS minimize animation is present and enabled, it owns minimize/restore.
        $macRegPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\local@macos-minimize-animation"
        $macSrcPath = Join-Path $modsSource "local@macos-minimize-animation.wh.cpp"
        $macActive = $false
        if (Test-Path $macRegPath) {
            $macDisabled = $null
            try {
                $macProp = Get-ItemProperty -Path $macRegPath -Name "Disabled" -ErrorAction SilentlyContinue
                if ($null -ne $macProp) { $macDisabled = $macProp.Disabled }
            } catch {
                $macDisabled = $null
            }
            if ($macDisabled -ne 1) { $macActive = $true }
        } elseif (Test-Path $macSrcPath) {
            $macActive = $true
        }
        if ($macActive) {
            Write-Status "         [Settings] macos-minimize-animation detected -> forcing Windows Animations minimize/restore OFF" "Yellow"
            foreach ($key in @("minimize_restore.minimize_animation", "minimize_restore.restore_animation", "minimize_restore.launch_animation")) {
                Set-ItemProperty -Path $settingsPath -Name $key -Value 0 -Type DWord
            }
        }
    }

    # Initialize ModsWritable runtime key
    $regWritablePath = "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$id"
    if (-not (Test-Path $regWritablePath)) {
        New-Item -Path $regWritablePath -Force | Out-Null
    }
}
Write-Status "Active mods successfully registered into Windhawk Engine." "Green"

# 5. UI Cache Regeneration (Refreshes installed list and Chinese metadata instantly)
$cacheFile = Join-Path $windhawkData "userprofile.json"
if (Test-Path $cacheFile) {
    Write-Status "Clearing UI Cache (userprofile.json) to force metadata scan..." "Yellow"
    Remove-Item -Path $cacheFile -Force
}

# 6. Restart Windhawk Service and UI
Write-Status "Starting Windhawk Service..."
Start-Service -Name Windhawk -ErrorAction SilentlyContinue

$exePath = Join-Path $windhawkDir "windhawk.exe"
if (Test-Path $exePath) {
    Write-Status "Launching Windhawk UI..."
    Start-Process -FilePath $exePath
}
Write-Status "Windhawk restarted successfully." "Green"

# 7. v4.2-draft [UNTESTED ON WINDOWS]: re-register ExplorerBlurMica to fix the black
# title/tab bar after Explorer restarts (Maplespe/ExplorerBlurMica#138).
# register.cmd itself runs regsvr32 and restarts explorer.exe.
# Window is kept visible on purpose (notes 2.4: never hide output, errors must be seen).
$blurRegister = Join-Path $PSScriptRoot "ExplorerBlurMica\register.cmd"
if (Test-Path $blurRegister) {
    Write-Status "Re-registering ExplorerBlurMica (black top bar fix)..." "Yellow"
    try {
        $p = Start-Process -FilePath "cmd.exe" -ArgumentList "/c `"`"$blurRegister`"`"" -WorkingDirectory (Split-Path $blurRegister) -Wait -PassThru -WindowStyle Normal
        Write-Status "ExplorerBlurMica re-registered (exit code $($p.ExitCode))." "Green"
    } catch {
        Write-Status "ExplorerBlurMica re-register failed: $($_.Exception.Message). Run ExplorerBlurMica\register.cmd manually." "Red"
    }
    # Safety net: make sure explorer is running again after register.cmd restarted it
    Start-Sleep -Seconds 2
    if (-not (Get-Process -Name explorer -ErrorAction SilentlyContinue)) {
        Start-Process explorer.exe
    }
} else {
    Write-Status "ExplorerBlurMica\register.cmd not found, skipping re-register." "Gray"
}

# 8. Optional startup self-check (scheduled task, current user, at logon).
$zenTaskName = "StartupCheck"
$zenTaskPath = "\ZenDesktop\"
if ($StartupCheck -eq "remove" -or $StartupCheck -eq "off") {
    if (Get-ScheduledTask -TaskName $zenTaskName -TaskPath $zenTaskPath -ErrorAction SilentlyContinue) {
        Unregister-ScheduledTask -TaskName $zenTaskName -TaskPath $zenTaskPath -Confirm:$false
        Write-Status "Startup self-check task removed." "Green"
    }
} elseif ($StartupCheck -eq "check" -or $StartupCheck -eq "restart") {
    $checkSrc = Join-Path $PSScriptRoot "tools\ZenDesktop-StartupCheck.ps1"
    if (Test-Path $checkSrc) {
        try {
            $checkDir = Join-Path $env:LOCALAPPDATA "ZenDesktop"
            if (-not (Test-Path $checkDir)) { New-Item -ItemType Directory -Path $checkDir -Force | Out-Null }
            $checkDst = Join-Path $checkDir "ZenDesktop-StartupCheck.ps1"
            Copy-Item -Path $checkSrc -Destination $checkDst -Force
            $user = [System.Security.Principal.WindowsIdentity]::GetCurrent().Name
            $action = New-ScheduledTaskAction -Execute "powershell.exe" -Argument "-NoProfile -WindowStyle Hidden -ExecutionPolicy Bypass -File `"$checkDst`" -Mode $StartupCheck -DelaySec 15"
            $trigger = New-ScheduledTaskTrigger -AtLogOn -User $user
            $principal = New-ScheduledTaskPrincipal -UserId $user -LogonType Interactive -RunLevel Limited
            $settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit (New-TimeSpan -Minutes 2)
            Register-ScheduledTask -TaskName $zenTaskName -TaskPath $zenTaskPath -Action $action -Trigger $trigger -Principal $principal -Settings $settings -Force | Out-Null
            Write-Status "Startup self-check installed (mode=$StartupCheck). Log: $checkDir\startup-check.log" "Green"
            Write-Status "Remove: deploy.ps1 -StartupCheck remove  (or Task Scheduler > ZenDesktop > StartupCheck)" "Gray"
        } catch {
            Write-Status "Startup self-check install failed: $($_.Exception.Message)" "Red"
        }
    } else {
        Write-Status "tools\ZenDesktop-StartupCheck.ps1 not found, skipping startup self-check." "Gray"
    }
}

if ($dwmModSelected) {
    Write-Status "果冻窗口 (Wobbly Windows) 已启用，需要注入 dwm.exe（Windhawk 默认跳过系统核心进程）。" "Yellow"
    Write-Status "请在 Windhawk > 设置 > 高级设置 的「进程包含列表」中添加 dwm.exe 并重启整台电脑。" "Yellow"
}

Write-Status "================================================" "Green"
Write-Status "   [SUCCESS] ZenDesktop v4.2.0 一键部署完成！" "Green"
Write-Status "================================================" "Green"
Write-Host ""
Write-Host "后续说明与使用建议："
Write-Host "  1. 打开 Windhawk，6 个核心美化模块已全部部署并就绪（提示时点击保存/编译）。"
Write-Host "  2. 果冻窗口与关窗动画已加入 Mod 列表（默认关闭），您可随时在 Windhawk 界面中点击右下角开关开启！"
Write-Host "  3. 若开启「果冻窗口」，请在 Windhawk「设置 > 高级设置 > 进程包含列表」中添加 dwm.exe。"
Write-Host "  4. 建议重启整台电脑，使所有美化与透明效果完整生效。"
Write-Host ""
