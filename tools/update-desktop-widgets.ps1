# Targeted source/metadata update. No compilation, activation or other mods.
param([string]$WindhawkData = "$env:ProgramData\Windhawk")
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
$source = Join-Path $repoRoot 'optional\local@zen-desktop-widgets.wh.cpp'
$destination = Join-Path $WindhawkData 'ModsSource\local@zen-desktop-widgets.wh.cpp'
$administrator = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $administrator) { throw '请以管理员身份运行此脚本。' }
if (-not (Test-Path -LiteralPath (Split-Path $destination -Parent))) { throw '未找到 Windhawk 源码目录，请指定 -WindhawkData。' }
$backup = Join-Path $repoRoot ('Backups\desktop-widgets-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $backup -Force | Out-Null
if (Test-Path -LiteralPath $destination) { Copy-Item -LiteralPath $destination -Destination $backup }
$modPath = 'SOFTWARE\Windhawk\Engine\Mods\local@zen-desktop-widgets'
$registryBase = [Microsoft.Win32.RegistryKey]::OpenBaseKey([Microsoft.Win32.RegistryHive]::LocalMachine, [Microsoft.Win32.RegistryView]::Registry64)
try {
    $existing = $registryBase.OpenSubKey($modPath)
    if ($existing) {
        $existing.Close()
        & reg.exe export "HKLM\$modPath" (Join-Path $backup 'settings.reg') /y | Out-Null
        if ($LASTEXITCODE -ne 0) { throw '备份注册表失败，未更新。' }
    }
    Copy-Item -LiteralPath $source -Destination $destination -Force
    if ((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $destination).Hash) { throw '源码校验失败。' }
    $key = $registryBase.CreateSubKey($modPath)
    try {
        # Retire the old DLL before extending its process list. It must not be
        # injected into Windhawk with the old Explorer-only implementation.
        $key.SetValue('LibraryFileName', '', [Microsoft.Win32.RegistryValueKind]::String)
        if ($key.GetValueNames() -notcontains 'Disabled') { $key.SetValue('Disabled', 1, [Microsoft.Win32.RegistryValueKind]::DWord) }
        $key.SetValue('Include', 'explorer.exe|windhawk.exe', [Microsoft.Win32.RegistryValueKind]::String)
        $key.SetValue('Architecture', 'x86|x86-64', [Microsoft.Win32.RegistryValueKind]::String)
        $key.SetValue('Version', '3.0.5', [Microsoft.Win32.RegistryValueKind]::String)
    } finally { $key.Close() }
    . (Join-Path $PSScriptRoot 'Initialize-DesktopWidgetSettings.ps1')
    $settings = $registryBase.CreateSubKey("$modPath\Settings")
    try { Initialize-DesktopWidgetSettings -Key $settings } finally { $settings.Close() }
    $writable = $registryBase.CreateSubKey('SOFTWARE\Windhawk\Engine\ModsWritable\local@zen-desktop-widgets')
    $writable.Close()
} finally { $registryBase.Close() }
Write-Host '桌面小组件 3.0.5 源码已更新。请在 Windhawk 编辑页自行编译；首次安装仍默认关闭。'
Write-Host "启用内置音频前，请关闭单独安装的 Tourne’Table，避免重复运行。"
Write-Host "备份位置：$backup"
