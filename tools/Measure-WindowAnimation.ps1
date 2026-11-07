# Temporarily collect animation diagnostics. No source, library, or enable-state changes.
param([ValidateRange(1,120)][int]$Seconds = 45)
$ErrorActionPreference = 'Stop'
$admin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $admin) { throw 'Run this diagnostic script in Administrator PowerShell; no settings were changed.' }
$hive = [Microsoft.Win32.RegistryKey]::OpenBaseKey([Microsoft.Win32.RegistryHive]::LocalMachine, [Microsoft.Win32.RegistryView]::Registry64)
$path = 'SOFTWARE\Windhawk\Engine\Mods\local@windows-animations'
$mod = $hive.OpenSubKey($path, $true)
$settings = $hive.OpenSubKey(($path + '\Settings'), $true)
if (-not $mod -or -not $settings) { throw 'Installed animation mod/settings were not found.' }
function Save-Value($key, $name) {
    $exists = @($key.GetValueNames()) -contains $name
    return @{ Exists=$exists; Value=if($exists){$key.GetValue($name)}else{$null}; Kind=if($exists){$key.GetValueKind($name)}else{[Microsoft.Win32.RegistryValueKind]::DWord} }
}
function Restore-Value($key, $name, $saved) {
    if ($saved.Exists) { $key.SetValue($name, $saved.Value, $saved.Kind) }
    else { $key.DeleteValue($name, $false) }
}
function Notify-SettingsChanged {
    $previous = [long]$mod.GetValue('SettingsChangeTime',0)
    $stamp = [Math]::Max([DateTimeOffset]::UtcNow.ToUnixTimeSeconds(), $previous+1)
    $mod.SetValue('SettingsChangeTime', [uint32]$stamp, [Microsoft.Win32.RegistryValueKind]::DWord)
    $mod.Flush()
}
$logging = Save-Value $mod 'LoggingEnabled'
$telemetry = Save-Value $settings 'diagnostics.performance_telemetry'
try {
    $mod.SetValue('LoggingEnabled', 1, [Microsoft.Win32.RegistryValueKind]::DWord)
    $settings.SetValue('diagnostics.performance_telemetry', 1, [Microsoft.Win32.RegistryValueKind]::DWord)
    $settings.Flush()
    Notify-SettingsChanged
    Write-Host "Recording for $Seconds seconds. Minimize and restore File Explorer and Settings twice."
    & (Join-Path $PSScriptRoot 'collect-animation-performance.ps1') -Seconds $Seconds
} finally {
    Restore-Value $settings 'diagnostics.performance_telemetry' $telemetry
    $settings.Flush()
    Restore-Value $mod 'LoggingEnabled' $logging
    Notify-SettingsChanged
    $settings.Dispose(); $mod.Dispose(); $hive.Dispose()
    Write-Host 'Original diagnostic switches restored. No animation source or DLL changed.'
}
