# ============================================================
# ZenDesktop Windhawk Mod Dev Helper
# Injects one mod source and prints the state needed for manual
# Windhawk UI compile/save validation. It never compiles DLLs.
# ============================================================

[CmdletBinding()]
param(
    [string]$ModId = "local@zen-taskbar-acrylic",
    [switch]$StatusOnly,
    [switch]$RestartExplorer,
    [switch]$NoElevate
)

$ErrorActionPreference = "Stop"

function Write-DevStatus {
    param(
        [string]$Text,
        [ConsoleColor]$Color = "Cyan"
    )

    Write-Host "[ZenDev] $Text" -ForegroundColor $Color
}

function Test-IsAdministrator {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = [Security.Principal.WindowsPrincipal]::new($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Invoke-SelfElevated {
    $argList = @(
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        "`"$PSCommandPath`"",
        "-ModId",
        "`"$ModId`""
    )

    if ($StatusOnly) {
        $argList += "-StatusOnly"
    }

    if ($RestartExplorer) {
        $argList += "-RestartExplorer"
    }

    Write-DevStatus "Requesting administrator privileges..." "Yellow"
    Start-Process -FilePath powershell.exe -ArgumentList $argList -Verb RunAs -WindowStyle Hidden -Wait
}

function Resolve-WindhawkPaths {
    $windhawkDir = $null
    $regPaths = @(
        "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk",
        "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk"
    )

    foreach ($regPath in $regPaths) {
        if (Test-Path $regPath) {
            $windhawkDir = Get-ItemPropertyValue -Path $regPath -Name "InstallLocation" -ErrorAction SilentlyContinue
            if ($windhawkDir) {
                break
            }
        }
    }

    if (-not $windhawkDir) {
        $proc = Get-Process windhawk -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($proc) {
            $windhawkDir = Split-Path $proc.Path
        }
    }

    foreach ($fallback in @("C:\Program Files\Windhawk", "$PSScriptRoot\Windhawk", $PSScriptRoot)) {
        if (-not $windhawkDir -and (Test-Path (Join-Path $fallback "windhawk.exe"))) {
            $windhawkDir = $fallback
        }
    }

    if (-not $windhawkDir) {
        throw "Windhawk executable could not be resolved."
    }

    $windhawkDir = $windhawkDir.TrimEnd("\")
    $windhawkData = "C:\ProgramData\Windhawk"
    $iniPath = Join-Path $windhawkDir "windhawk.ini"

    if (Test-Path $iniPath) {
        $ini = Get-Content -Path $iniPath
        $isPortable = $false
        $foundAppDataPath = $null

        foreach ($line in $ini) {
            if ($line -match "^\s*Portable\s*=\s*(\d+)") {
                $isPortable = $Matches[1] -eq "1"
            } elseif ($line -match "^\s*AppDataPath\s*=\s*(.+)") {
                $foundAppDataPath = $Matches[1].Trim()
            }
        }

        if ($foundAppDataPath) {
            $windhawkData = [Environment]::ExpandEnvironmentVariables($foundAppDataPath)
            if (-not [IO.Path]::IsPathRooted($windhawkData)) {
                $windhawkData = [IO.Path]::GetFullPath((Join-Path $windhawkDir $windhawkData))
            }
        } elseif ($isPortable) {
            $windhawkData = Join-Path $windhawkDir "AppData"
        }
    } elseif (Test-Path (Join-Path $windhawkDir "AppData\ModsSource")) {
        $windhawkData = Join-Path $windhawkDir "AppData"
    }

    return [PSCustomObject]@{
        Home = $windhawkDir
        Data = $windhawkData
        ModsSource = Join-Path $windhawkData "ModsSource"
    }
}

function Get-ModSourcePath {
    param([string]$Id)

    $fileName = "$Id.wh.cpp"
    $path = Join-Path $PSScriptRoot $fileName
    if (-not (Test-Path $path)) {
        throw "Source file not found: $path"
    }

    return $path
}

function Get-ModMetadata {
    param([string]$SourcePath)

    $content = Get-Content -Path $SourcePath -Raw
    $version = "3.4.0"
    $arch = "x86-64"

    if ($content -match "// @version\s+(.+)") {
        $version = $Matches[1].Trim()
    }

    if ($content -match "// @architecture\s+(.+)") {
        $arch = $Matches[1].Trim()
    }

    $includes = @()
    foreach ($m in [regex]::Matches($content, "// @include\s+(.+)")) {
        $includes += $m.Groups[1].Value.Trim()
    }

    if ($includes.Count -eq 0) {
        $includes = @("explorer.exe")
    }

    return [PSCustomObject]@{
        Version = $version
        Architecture = $arch
        Include = ($includes -join "|")
    }
}

function Set-RegistryValue {
    param(
        [string]$Path,
        [string]$Name,
        [object]$Value,
        [Microsoft.Win32.RegistryValueKind]$Type
    )

    New-ItemProperty -Path $Path -Name $Name -Value $Value -PropertyType $Type -Force | Out-Null
}

function Set-ModRegistryForPlatformCompile {
    param(
        [string]$Id,
        [object]$Metadata
    )

    $regPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$Id"
    if (-not (Test-Path $regPath)) {
        New-Item -Path $regPath -Force | Out-Null
    }

    Set-RegistryValue -Path $regPath -Name "Disabled" -Value 0 -Type DWord
    Set-RegistryValue -Path $regPath -Name "LoggingEnabled" -Value 0 -Type DWord
    Set-RegistryValue -Path $regPath -Name "Include" -Value $Metadata.Include -Type String
    Set-RegistryValue -Path $regPath -Name "Exclude" -Value "" -Type String
    Set-RegistryValue -Path $regPath -Name "Architecture" -Value $Metadata.Architecture -Type String
    Set-RegistryValue -Path $regPath -Name "Version" -Value $Metadata.Version -Type String
    Set-RegistryValue -Path $regPath -Name "LibraryFileName" -Value "" -Type String

    $writablePath = "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$Id"
    if (-not (Test-Path $writablePath)) {
        New-Item -Path $writablePath -Force | Out-Null
    }

    if ($Id -eq "local@zen-taskbar-acrylic") {
        $settingsPath = Join-Path $regPath "Settings"
        if (-not (Test-Path $settingsPath)) {
            New-Item -Path $settingsPath -Force | Out-Null
        }

        $currentTheme = (Get-ItemProperty -Path $settingsPath -Name "theme" -ErrorAction SilentlyContinue).theme
        if ([string]::IsNullOrWhiteSpace($currentTheme)) {
            Write-DevStatus "Repairing empty taskbar theme -> TranslucentTaskbar" "Yellow"
            Set-RegistryValue -Path $settingsPath -Name "theme" -Value "TranslucentTaskbar" -Type String
        }
    }

    if ($Id -eq "local@zen-fileexplorer-acrylic") {
        $settingsPath = Join-Path $regPath "Settings"
        if (-not (Test-Path $settingsPath)) {
            New-Item -Path $settingsPath -Force | Out-Null
        }

        $settings = Get-ItemProperty -Path $settingsPath -ErrorAction SilentlyContinue
        $settingNames = @($settings.PSObject.Properties.Name)

        if (-not ($settingNames -contains "bgColorMode")) {
            $legacy = if ($settingNames -contains "zenGlassColorMode") { $settings.zenGlassColorMode } else { "Default" }
            if ([string]::IsNullOrWhiteSpace($legacy)) { $legacy = "Default" }
            Set-RegistryValue -Path $settingsPath -Name "bgColorMode" -Value $legacy -Type String
        }
        if (-not ($settingNames -contains "blurPreset")) {
            $legacy = if ($settingNames -contains "zenGlassBlur") { $settings.zenGlassBlur } else { 30 }
            Set-RegistryValue -Path $settingsPath -Name "blurPreset" -Value ([int]$legacy) -Type DWord
        }
        if (-not ($settingNames -contains "opacityPreset")) {
            $legacy = if ($settingNames -contains "zenGlassOpacity") { $settings.zenGlassOpacity } else { 0 }
            Set-RegistryValue -Path $settingsPath -Name "opacityPreset" -Value ([int]$legacy) -Type DWord
        }
        if (-not ($settingNames -contains "luminosityPreset")) {
            $legacy = if ($settingNames -contains "zenGlassLuminosity") { $settings.zenGlassLuminosity } else { 100 }
            Set-RegistryValue -Path $settingsPath -Name "luminosityPreset" -Value ([int]$legacy) -Type DWord
        }
        if (-not ($settingNames -contains "cornerRadiusPreset")) {
            $legacy = if ($settingNames -contains "zenGlassCornerRadius") { $settings.zenGlassCornerRadius } else { 20 }
            Set-RegistryValue -Path $settingsPath -Name "cornerRadiusPreset" -Value ([int]$legacy) -Type DWord
        }

        $currentTheme = (Get-ItemProperty -Path $settingsPath -Name "theme" -ErrorAction SilentlyContinue).theme
        if ([string]::IsNullOrWhiteSpace($currentTheme) -or
            $currentTheme -eq "Translucent Explorer11" -or
            $currentTheme -eq "Mica" -or
            $currentTheme -eq "MicaAlt") {
            Write-DevStatus "Defaulting File Explorer -> CustomLiquidGlass full-window glass" "Yellow"
            Set-RegistryValue -Path $settingsPath -Name "theme" -Value "CustomLiquidGlass" -Type String
            Set-RegistryValue -Path $settingsPath -Name "zenGlassCoverage" -Value "auto" -Type String
            Set-RegistryValue -Path $settingsPath -Name "bgColorMode" -Value "Default" -Type String
            Set-RegistryValue -Path $settingsPath -Name "blurPreset" -Value 30 -Type DWord
            Set-RegistryValue -Path $settingsPath -Name "opacityPreset" -Value 0 -Type DWord
            Set-RegistryValue -Path $settingsPath -Name "luminosityPreset" -Value 100 -Type DWord
            Set-RegistryValue -Path $settingsPath -Name "cornerRadiusPreset" -Value 20 -Type DWord
            Set-RegistryValue -Path $settingsPath -Name "backgroundTranslucentEffect" -Value "acrylic" -Type String
            Set-RegistryValue -Path $settingsPath -Name "backgroundTranslucentEffectRegion" -Value "entireWindow" -Type String
        }
    }
}

function Show-ModStatus {
    param(
        [string]$Id,
        [object]$Paths
    )

    $sourcePath = Get-ModSourcePath -Id $Id
    $targetPath = Join-Path $Paths.ModsSource ([IO.Path]::GetFileName($sourcePath))
    $regPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$Id"
    $settingsPath = Join-Path $regPath "Settings"

    Write-DevStatus "Windhawk Home: $($Paths.Home)" "DarkGreen"
    Write-DevStatus "Windhawk Data: $($Paths.Data)" "DarkGreen"
    Write-DevStatus "Source: $sourcePath" "Gray"
    Write-DevStatus "ModsSource: $targetPath" "Gray"

    if (Test-Path $targetPath) {
        $srcHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $sourcePath).Hash
        $dstHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $targetPath).Hash
        Write-DevStatus "Source SHA256: $srcHash" "Gray"
        Write-DevStatus "Target SHA256: $dstHash" "Gray"
        if ($srcHash -eq $dstHash) {
            Write-DevStatus "Hash check: OK" "Green"
        } else {
            Write-DevStatus "Hash check: MISMATCH" "Yellow"
        }
    } else {
        Write-DevStatus "Target source missing in ModsSource" "Yellow"
    }

    if (Test-Path $regPath) {
        $modReg = Get-ItemProperty -Path $regPath
        Write-DevStatus "Registry LibraryFileName: '$($modReg.LibraryFileName)'" "Gray"
        Write-DevStatus "Registry Version: $($modReg.Version)" "Gray"
        Write-DevStatus "Registry Include: $($modReg.Include)" "Gray"
    } else {
        Write-DevStatus "Registry mod key missing: $regPath" "Yellow"
    }

    if (Test-Path $settingsPath) {
        $settings = Get-ItemProperty -Path $settingsPath
        $settingNames = @(
            "theme",
            "zenGlassCoverage",
            "bgColorMode",
            "blurPreset",
            "opacityPreset",
            "luminosityPreset",
            "cornerRadiusPreset",
            "backgroundTranslucentEffect",
            "backgroundTranslucentEffectRegion",
            "maximizedTaskbarLayer",
            "maximizedLayerOpacity",
            "maximizedLayerBlur"
        )

        foreach ($name in $settingNames) {
            if ($settings.PSObject.Properties.Name -contains $name) {
                Write-DevStatus "Setting $name = '$($settings.$name)'" "Gray"
            }
        }
    }

    try {
        $loaded = Get-Process explorer -ErrorAction Stop |
            ForEach-Object { $_.Modules } |
            Where-Object { $_.ModuleName -like "$Id*" } |
            Select-Object ModuleName, FileName

        if ($loaded) {
            Write-DevStatus "Explorer loaded module(s):" "Green"
            $loaded | Format-List
        } else {
            Write-DevStatus "Explorer loaded module: none" "Yellow"
        }
    } catch {
        Write-DevStatus "Explorer module scan failed: $($_.Exception.Message)" "Yellow"
    }
}

function Restart-WindowsExplorer {
    Write-DevStatus "Restarting Explorer..." "Yellow"
    Stop-Process -Name explorer -Force -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 1
    Start-Process explorer.exe
    Start-Sleep -Seconds 2
}

Write-DevStatus "ZenDesktop Windhawk Mod Dev Helper"
Write-DevStatus "Mode: $(if ($StatusOnly) { 'status only' } else { 'inject source, platform compile' })"

if (-not $StatusOnly -and -not $NoElevate -and -not (Test-IsAdministrator)) {
    Invoke-SelfElevated
    exit 0
}

$paths = Resolve-WindhawkPaths
$sourcePath = Get-ModSourcePath -Id $ModId
$metadata = Get-ModMetadata -SourcePath $sourcePath

if (-not $StatusOnly) {
    if (-not (Test-Path $paths.ModsSource)) {
        New-Item -ItemType Directory -Path $paths.ModsSource -Force | Out-Null
    }

    $targetPath = Join-Path $paths.ModsSource ([IO.Path]::GetFileName($sourcePath))
    Write-DevStatus "Injecting $ModId -> $targetPath"
    Copy-Item -LiteralPath $sourcePath -Destination $targetPath -Force
    Set-ModRegistryForPlatformCompile -Id $ModId -Metadata $metadata
}

if ($RestartExplorer) {
    Restart-WindowsExplorer
}

Show-ModStatus -Id $ModId -Paths $paths

Write-Host ""
Write-DevStatus "Next: open Windhawk, save/compile this mod from the Source Code page." "Green"
if (-not $RestartExplorer) {
    Write-DevStatus "After compile, restart Explorer if the visual state does not refresh." "Green"
}
