# ============================================================
#  ZenDesktop Direct Injection Deploy Engine
#  Refined v4.1.0 - Full Chinese Localization & Dynamic Engine
# ============================================================
$ErrorActionPreference = 'Stop'

# Helper function to print colorful status
function Write-Status ($text, $color = "Cyan") {
    Write-Host "[ZenDeploy] $text" -ForegroundColor $color
}

Write-Status "================================================" "Green"
Write-Status "   ZenDesktop Direct Injection Engine v4.1.0" "Green"
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
    $proc = Get-Process windhawk -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($proc) {
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

$files = Get-ChildItem -Path $PSScriptRoot -Filter "local@*.wh.cpp"
if ($files.Count -eq 0) {
    Write-Error "No local@*.wh.cpp mod files found in script directory! Checked: $PSScriptRoot"
    Exit 1
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
    
    # Populate the registry with metadata
    Set-ItemProperty -Path $regPath -Name "Disabled" -Value 0 -Type DWord
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

Write-Status "================================================" "Green"
Write-Status "   [SUCCESS] ZenDesktop v4.1.0 deployed successfully!" "Green"
Write-Status "================================================" "Green"
Write-Host ""
Write-Host "Next steps:"
Write-Host "  1. Open Windhawk and compile/save mods if prompted."
Write-Host "  2. Verify all 6 ZenDesktop mods appear in 'Installed Mods'."
Write-Host "  3. Enjoy your fully localized transparent Windows 11 desktop!"
Write-Host ""
