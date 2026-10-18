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

# Settings rule: deploy only seeds a value that does not exist yet (fresh
# install). An existing value, including an empty string or 0 chosen by the
# user, is never overwritten.
function Set-RegValueIfMissing ($path, $name, $value, $type, $label = "") {
    if (-not (Test-Path $path)) {
        New-Item -Path $path -Force | Out-Null
    }
    $prop = Get-ItemProperty -Path $path -Name $name -ErrorAction SilentlyContinue
    if ($null -eq $prop) {
        if ($label) {
            Write-Status "         [Settings] Seeding $label $name -> $value" "Yellow"
        }
        Set-ItemProperty -Path $path -Name $name -Value $value -Type $type
    }
}

# Adds entries to a "|"-separated Windhawk process list, keeping the user's
# entries and their order.
function Merge-ProcessList ($current, [string[]]$entries) {
    $list = New-Object System.Collections.Generic.List[string]
    if ($current) {
        foreach ($item in ($current -split '\|')) {
            $item = $item.Trim()
            if ($item -and -not ($list -contains $item)) { $list.Add($item) }
        }
    }
    foreach ($item in $entries) {
        if (-not ($list -contains $item)) { $list.Add($item) }
    }
    return ($list -join "|")
}

Write-Status "================================================" "Green"
Write-Status "   ZenDesktop Direct Injection Engine v4.2.0" "Green"
Write-Status "================================================" "Green"

# 1. Enforce Administrator Privileges
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Error "需要管理员权限：请右键 deploy.bat，选择「以管理员身份运行」。 (This script must be executed with Administrator privileges. Please run deploy.bat as Administrator.)"
    Exit 1
}

# 2. Detect Windhawk Paths
function Find-WindhawkDir {
    # Registry check
    $regPaths = @(
        "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk",
        "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk"
    )
    foreach ($regPath in $regPaths) {
        if (Test-Path $regPath) {
            $dir = Get-ItemPropertyValue -Path $regPath -Name "InstallLocation" -ErrorAction SilentlyContinue
            if ($dir -and (Test-Path -LiteralPath (Join-Path $dir "windhawk.exe"))) { return $dir }
        }
    }
    # Active process fallback
    $proc = Get-Process windhawk -ErrorAction SilentlyContinue | Where-Object { $_.Path } | Select-Object -First 1
    if ($proc -and $proc.Path) { return (Split-Path $proc.Path) }
    # Standard paths fallbacks
    $fallbacks = @()
    if ($env:ProgramFiles) { $fallbacks += (Join-Path $env:ProgramFiles "Windhawk") }
    $fallbacks += @("C:\Program Files\Windhawk", (Join-Path $PSScriptRoot "Windhawk"), $PSScriptRoot)
    foreach ($fallback in $fallbacks) {
        if (Test-Path -LiteralPath (Join-Path $fallback "windhawk.exe")) { return $fallback }
    }
    return $null
}

Write-Status "Detecting Windhawk installation..."
$windhawkDir = Find-WindhawkDir

# Not installed: run the Windhawk installer shipped in the package (if any), then detect again.
if (-not $windhawkDir) {
    $setup = @(Get-ChildItem -LiteralPath $PSScriptRoot -Filter "windhawk_setup*.exe" -ErrorAction SilentlyContinue) | Select-Object -First 1
    if ($setup) {
        Write-Status "未检测到 Windhawk，正在打开包内的安装程序 $($setup.Name)，请按提示装完后关闭安装窗口..." "Yellow"
        Write-Status "Windhawk not found, launching the bundled installer $($setup.Name). Finish the setup to continue..." "Yellow"
        try { Start-Process -FilePath $setup.FullName -Wait } catch { Write-Status "无法启动安装程序: $($_.Exception.Message)" "Red" }
        Start-Sleep -Seconds 2
        $windhawkDir = Find-WindhawkDir
    }
}

if (-not $windhawkDir) {
    Write-Error "未找到 Windhawk：请先从 https://windhawk.net 下载安装 Windhawk（或使用带 windhawk_setup.exe 的完整包），装好后再运行 deploy.bat。 (Windhawk could not be found. Install it from https://windhawk.net, then run deploy.bat again.)"
    Exit 1
}

# Normalize path (remove trailing backslash)
$windhawkDir = $windhawkDir.TrimEnd('\')
Write-Status "Windhawk Home: $windhawkDir" "DarkGreen"

# Determine data root by parsing windhawk.ini if it exists (Standard vs Portable)
$windhawkData = if ($env:ProgramData) { Join-Path $env:ProgramData "Windhawk" } else { "C:\ProgramData\Windhawk" } # Standard default fallback
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
)

# Engine process lists (HKLM\SOFTWARE\Windhawk\Engine\Settings, the same values
# as Windhawk > Settings > Advanced). Our entries are merged into the user's
# lists instead of replacing them. A process matching Include is injected even
# if it matches Exclude or the built-in critical process list, which is how
# dwm.exe is made available to Wobbly Windows (the mod itself still has to be
# enabled by the user).
$globalIncludeList = @("dwm.exe")

$engineSettingsPath = "HKLM:\SOFTWARE\Windhawk\Engine\Settings"
if (-not (Test-Path $engineSettingsPath)) {
    New-Item -Path $engineSettingsPath -Force | Out-Null
}
$currentExclude = (Get-ItemProperty -Path $engineSettingsPath -Name "Exclude" -ErrorAction SilentlyContinue).Exclude
Set-ItemProperty -Path $engineSettingsPath -Name "Exclude" -Value (Merge-ProcessList $currentExclude $globalExcludeList) -Type String
$currentInclude = (Get-ItemProperty -Path $engineSettingsPath -Name "Include" -ErrorAction SilentlyContinue).Include
Set-ItemProperty -Path $engineSettingsPath -Name "Include" -Value (Merge-ProcessList $currentInclude $globalIncludeList) -Type String
# Windhawk's own default is 0 (don't inject into games); only seeded.
Set-RegValueIfMissing $engineSettingsPath "InjectIntoGames" 0 DWord

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

$files = @(Get-ChildItem -LiteralPath $PSScriptRoot -Filter "local@*.wh.cpp")
if ($files.Count -eq 0) {
    Write-Error "部署目录里没有找到 local@*.wh.cpp：请先把压缩包完整解压，再运行解压目录里的 deploy.bat。 (No local@*.wh.cpp mod files found in: $PSScriptRoot. Extract the whole zip first.)"
    Exit 1
}

# v4.2: deploy 仅负责将所有 Mod 源码及元数据装入 Windhawk，不负责开启，不负责编译。
# 所有 Mod（包括核心 Mod 与 optional Mod）在首次部署时默认全部处于关闭状态（Disabled=1）。
# 用户在 Windhawk UI 中自行按需开启开关并在提示时编译。
# 支持在无人值守脚本中通过 -Optional 参数预选开启指定模块。

$optionalDir = Join-Path $PSScriptRoot "optional"
$optionalModIds = @("local@wobbly-windows", "local@windows-animations")
$preselected = @()
if ($Optional) {
    $preselected = $Optional.Split(",") | ForEach-Object { $_.Trim() } | Where-Object { $_ -and $_ -ne "none" } | ForEach-Object {
        if ($_ -like "local@*") { $_ } else { "local@$_" }
    }
}
$dwmModSelected = $false
if (Test-Path -LiteralPath $optionalDir) {
    $optFiles = @(Get-ChildItem -LiteralPath $optionalDir -Filter "local@*.wh.cpp")
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
    Copy-Item -LiteralPath $file.FullName -Destination (Join-Path $modsSource $file.Name) -Force
    
    # Read and parse metadata directly from the source code headers
    $content = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8
    
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
    # 核心原则：deploy 不负责编译，也不负责开启！只负责把 mod 装到 Windhawk 里面。
    # 所有 Mod 默认均为关闭状态（Disabled = 1），绝不自动开启、绝不执行后台编译。
    # 用户在 Windhawk UI 中自行按需开启开关并在提示时编译。
    # 若用户此前已在 Windhawk 中手动调整过开关状态，则予以保留。
    $currentDisabled = $null
    try {
        $prop = Get-ItemProperty -Path $regPath -Name "Disabled" -ErrorAction SilentlyContinue
        if ($null -ne $prop) { $currentDisabled = $prop.Disabled }
    } catch {
        $currentDisabled = $null
    }

    # -Optional only turns ON the listed mods; every other mod keeps the
    # user's existing toggle (or stays off on a fresh install).
    $targetDisabled = 1
    if ($Optional -and ($preselected -contains $id)) {
        $targetDisabled = 0
    } elseif ($null -ne $currentDisabled) {
        $targetDisabled = $currentDisabled
    } else {
        $targetDisabled = 1  # 默认关闭：所有 Mod 仅装入 Windhawk，不主动开启、不主动编译
    }
    if ($id -eq "local@wobbly-windows" -and $targetDisabled -eq 0) {
        $dwmModSelected = $true
    }

    
    # Populate the registry with metadata
    Set-ItemProperty -Path $regPath -Name "Disabled" -Value $targetDisabled -Type DWord
    Set-RegValueIfMissing $regPath "LoggingEnabled" 0 DWord
    Set-ItemProperty -Path $regPath -Name "Include" -Value $includeStr -Type String
    Set-ItemProperty -Path $regPath -Name "Exclude" -Value $excludeStr -Type String
    Set-ItemProperty -Path $regPath -Name "Architecture" -Value $arch -Type String
    Set-ItemProperty -Path $regPath -Name "Version" -Value $version -Type String
    Set-ItemProperty -Path $regPath -Name "LibraryFileName" -Value "" -Type String

    # Default settings for a fresh install: only seeded when missing, never
    # overwriting the user's settings. Registry-deployed mods get no defaults
    # from their settings block (the engine reads the registry only and returns
    # 0 / "" for a missing value).
    $settingsPath = Join-Path $regPath "Settings"

    if ($id -eq "local@zen-explorer-styler") {
        $label = "File Explorer"
        Set-RegValueIfMissing $settingsPath "theme" "ZenTranslucent" String $label
        Set-RegValueIfMissing $settingsPath "tintPreset" "followSystem" String $label
        Set-RegValueIfMissing $settingsPath "tintColor" "#232323" String $label
        Set-RegValueIfMissing $settingsPath "tintOpacity" 23 DWord $label
        Set-RegValueIfMissing $settingsPath "fieldOpacity" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "textColorMode" "default" String $label
        Set-RegValueIfMissing $settingsPath "contextMenuEnable" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "contextMenuBlur" 30 DWord $label
    }

    if ($id -eq "local@zen-taskbar-acrylic") {
        $label = "Taskbar"
        # 4.2.6 grouped the settings. Copy values saved under the old flat keys
        # to the grouped keys that are still missing; never overwrite.
        $taskbarKeyMap = [ordered]@{
            "theme" = "look.theme"; "bgColorMode" = "look.bgColorMode"; "textColorMode" = "look.textColorMode"
            "blurPreset" = "taskbar.blurPreset"; "opacityPreset" = "taskbar.opacityPreset"; "luminosityPreset" = "taskbar.luminosityPreset"
            "trayBlurPreset" = "tray.blurPreset"; "trayOpacityPreset" = "tray.opacityPreset"; "trayLuminosityPreset" = "tray.luminosityPreset"
            "volumeBlurPreset" = "volume.blurPreset"; "volumeOpacityPreset" = "volume.opacityPreset"; "volumeLuminosityPreset" = "volume.luminosityPreset"
            "maximizedTaskbarLayer" = "maximized.enabled"; "maximizedLayerBlur" = "maximized.blur"
            "maximizedLayerOpacity" = "maximized.opacity"; "maximizedLayerLuminosity" = "maximized.luminosity"
        }
        if (Test-Path $settingsPath) {
            $existing = Get-ItemProperty -Path $settingsPath -ErrorAction SilentlyContinue
            $hadLegacy = $existing -and ($null -ne $existing.PSObject.Properties["bgColorMode"])
            foreach ($oldKey in $taskbarKeyMap.Keys) {
                $prop = if ($existing) { $existing.PSObject.Properties[$oldKey] } else { $null }
                if ($null -ne $prop) {
                    $kind = (Get-Item $settingsPath).GetValueKind($oldKey)
                    $type = if ($kind -eq "DWord") { "DWord" } else { "String" }
                    Set-RegValueIfMissing $settingsPath $taskbarKeyMap[$oldKey] $prop.Value $type $label
                }
            }
            # Old saves had no unify option; keep their separate flyout values.
            if ($hadLegacy) {
                Set-RegValueIfMissing $settingsPath "unifyFlyouts" 0 DWord $label
            }
        }
        Set-RegValueIfMissing $settingsPath "look.theme" "TranslucentTaskbar" String $label
        Set-RegValueIfMissing $settingsPath "maximized.enabled" 0 DWord $label
    }

    if ($id -eq "local@macos-minimize-animation") {
        $label = "macOS Genie"
        $macDefaults = [ordered]@{
            "duration_ms" = 450
            "open_animation" = 1
            "launch_animation" = 0
            "multi_monitor" = 0
            "unhide_taskbar" = 1
            "unhide_duration_ms" = 450
            "tile_count" = 35
        }
        foreach ($key in $macDefaults.Keys) {
            Set-RegValueIfMissing $settingsPath $key $macDefaults[$key] DWord $label
        }
        Set-RegValueIfMissing $settingsPath "engine" "modern" String $label
    }

    if ($id -eq "local@zen-startmenu-acrylic") {
        $label = "StartMenu Acrylic"
        Set-RegValueIfMissing $settingsPath "width" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "height" 0 DWord $label
    }

    if ($id -eq "local@windows-11-settings-styler-fork") {
        $label = "Settings Acrylic"
        Set-RegValueIfMissing $settingsPath "theme" "TranslucentSettings" String $label
        Set-RegValueIfMissing $settingsPath "bgColorMode" "Default" String $label
        Set-RegValueIfMissing $settingsPath "textColorMode" "default" String $label
        Set-RegValueIfMissing $settingsPath "blurPreset" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "opacityPreset" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "luminosityPreset" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "lightModeBase" 45 DWord $label
    }

    if ($id -eq "local@zen-notificationcenter-acrylic") {
        $label = "NotificationCenter Acrylic"
        Set-RegValueIfMissing $settingsPath "lightModeContrast" "standard" String $label
    }


    if ($id -eq "local@zen-taskbar-lyrics") {
        $label = "Taskbar Lyrics"
        Set-RegValueIfMissing $settingsPath "positionMode" "widgets" String $label
        Set-RegValueIfMissing $settingsPath "xOffset" 12 DWord $label
        Set-RegValueIfMissing $settingsPath "sizePreset" "standard" String $label
        Set-RegValueIfMissing $settingsPath "width" 340 DWord $label
        Set-RegValueIfMissing $settingsPath "showArt" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "showButtons" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "textMode" "lyrics" String $label
        Set-RegValueIfMissing $settingsPath "secondLine" "auto" String $label
        Set-RegValueIfMissing $settingsPath "whenIdle" "show" String $label
        Set-RegValueIfMissing $settingsPath "clickAction" "open" String $label
        Set-RegValueIfMissing $settingsPath "textColorMode" "default" String $label
        Set-RegValueIfMissing $settingsPath "panelBackground" "none" String $label
        Set-RegValueIfMissing $settingsPath "fontPreset" "msyh" String $label
        Set-RegValueIfMissing $settingsPath "fontSize" 12 DWord $label
        Set-RegValueIfMissing $settingsPath "bold" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "offsetMs" 200 DWord $label
        Set-RegValueIfMissing $settingsPath "hideInFullscreen" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "panelGlass" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "multiMonitor" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "enableNetease" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "enableQQ" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "enableLrclib" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "aiStatus" "compact" String $label
        Set-RegValueIfMissing $settingsPath "aiDoneSeconds" "120" String $label
    }

    if ($id -eq "local@windows-animations") {
        # Matches the mod's settings block (1.4.0: macOS genie merged in, so
        # minimize/restore default on with the macos_genie style). Nested
        # Windhawk settings are stored as "group.key" values; booleans as
        # DWORD 0/1. Only missing values are seeded; existing settings stay.
        # The old macOS Minimize Animation mod is superseded: if the user also
        # enables it, both animate (deploy never enables or disables mods).
        $label = "Windows Animations"
        $waDefaults = [ordered]@{
            "close.close_animation"                 = 1
            "close.hide_as_close"                   = 1
            "close.random_effect"                   = 0
            "close.duration_ms"                     = 630
            "close.shatter_block_size"              = 5
            "close.gpu_acceleration"                = 1
            "minimize_restore.minimize_animation"   = 1
            "minimize_restore.restore_animation"    = 1
            "minimize_restore.launch_animation"     = 0
            "minimize_restore.random_effect"        = 0
            "minimize_restore.duration_ms"          = 360
            "minimize_restore.genie_tile_count"     = 35
            "minimize_restore.unhide_taskbar"       = 1
            "minimize_restore.unhide_duration_ms"   = 450
            "minimize_restore.show_desktop_top_window_only" = 0
            "minimize_restore.gpu_acceleration"     = 1
            "window_switch.switch_animation"        = 0
            "window_switch.duration_ms"             = 200
            "diagnostics.performance_telemetry"     = 0
        }
        foreach ($key in $waDefaults.Keys) {
            Set-RegValueIfMissing $settingsPath $key $waDefaults[$key] DWord $label
        }
        Set-RegValueIfMissing $settingsPath "close.effect_style" "thanos" String $label
        Set-RegValueIfMissing $settingsPath "minimize_restore.effect_style" "macos_genie" String $label
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

# 7. Optional startup self-check (scheduled task, current user, at logon).
$zenTaskName = "StartupCheck"
$zenTaskPath = "\ZenDesktop\"
if ($StartupCheck -eq "remove" -or $StartupCheck -eq "off") {
    if (Get-ScheduledTask -TaskName $zenTaskName -TaskPath $zenTaskPath -ErrorAction SilentlyContinue) {
        Unregister-ScheduledTask -TaskName $zenTaskName -TaskPath $zenTaskPath -Confirm:$false
        Write-Status "Startup self-check task removed." "Green"
    }
} elseif ($StartupCheck -eq "check" -or $StartupCheck -eq "restart") {
    $checkSrc = Join-Path $PSScriptRoot "tools\ZenDesktop-StartupCheck.ps1"
    if (Test-Path -LiteralPath $checkSrc) {
        try {
            $checkDir = Join-Path $env:LOCALAPPDATA "ZenDesktop"
            if (-not (Test-Path $checkDir)) { New-Item -ItemType Directory -Path $checkDir -Force | Out-Null }
            $checkDst = Join-Path $checkDir "ZenDesktop-StartupCheck.ps1"
            Copy-Item -LiteralPath $checkSrc -Destination $checkDst -Force
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
    Write-Status "deploy 已把 dwm.exe 加入 Windhawk「进程包含列表」，请重启整台电脑后生效。" "Yellow"
}

Write-Status "================================================" "Green"
Write-Status "   [SUCCESS] ZenDesktop v4.2.0 一键部署完成！" "Green"
Write-Status "================================================" "Green"
Write-Host ""
Write-Host "后续说明与使用建议："
Write-Host "  1. deploy 仅负责将美化模块装入 Windhawk，默认不自动开启任何 Mod，也不在后台执行编译。"
Write-Host "  2. 请在 Windhawk 界面中按需开启您想要使用的模块开关，在弹出的窗口中确认保存与编译。"
Write-Host "  3. deploy 已把 dwm.exe 加入 Windhawk「进程包含列表」；若开启「果冻窗口」，请重启整机后生效。"
Write-Host "     deploy 只会补上缺失的默认设置，不会覆盖您已有的 Mod 设置。"
Write-Host "  4. 建议在配置完成后重启整台电脑，使所有美化与透明效果达到最佳状态。"
Write-Host ""
