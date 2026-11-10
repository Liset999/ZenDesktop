. "$PSScriptRoot\..\Initialize-DesktopWidgetSettings.ps1"
Add-Type @'
using System;
using System.Collections.Generic;
using Microsoft.Win32;
public class WidgetMemoryKey {
    Dictionary<string, object> values = new Dictionary<string, object>();
    Dictionary<string, RegistryValueKind> kinds = new Dictionary<string, RegistryValueKind>();
    public string[] GetValueNames() { var a = new string[values.Count]; values.Keys.CopyTo(a,0); return a; }
    public object GetValue(string n) { return values[n]; }
    public RegistryValueKind GetValueKind(string n) { return kinds[n]; }
    public void SetValue(string n, object v, RegistryValueKind k) { values[n]=v; kinds[n]=k; }
}
'@
$old = New-Object WidgetMemoryKey
$old.SetValue('audio.enabled', 1, [Microsoft.Win32.RegistryValueKind]::DWord)
$old.SetValue('audio.placement', 'both', [Microsoft.Win32.RegistryValueKind]::String)
$old.SetValue('clock.advanced.use24Hour', 0, [Microsoft.Win32.RegistryValueKind]::DWord)
$old.SetValue('clock.advanced.glow', 0, [Microsoft.Win32.RegistryValueKind]::DWord)
Initialize-DesktopWidgetSettings -Key $old
if ($old.GetValue('audio.appearance.bars') -ne 25 -or $old.GetValue('audio.barHeight') -ne '33') { throw 'Missing audio defaults' }
if ($old.GetValue('audio.enabled') -ne 1 -or $old.GetValue('audio.placement') -ne 'taskbar') { throw 'User settings overwritten' }
if ($old.GetValue('clock.use24Hour') -ne 0 -or $old.GetValue('clockAdvanced.glow') -ne 0) { throw 'Migration lost zero values' }
$old.SetValue('audio.appearance.bars', 0, [Microsoft.Win32.RegistryValueKind]::DWord)
$old.SetValue('clockAdvanced.glow', 1, [Microsoft.Win32.RegistryValueKind]::DWord)
Initialize-DesktopWidgetSettings -Key $old
if ($old.GetValue('audio.appearance.bars') -ne 0 -or $old.GetValue('clockAdvanced.glow') -ne 1) { throw 'Repeated migration overwrote user choices' }
$fresh = New-Object WidgetMemoryKey
Initialize-DesktopWidgetSettings -Key $fresh
if ($fresh.GetValue('audio.enabled') -ne 1 -or $fresh.GetValue('clock.use24Hour') -ne 1) { throw 'Fresh defaults differ' }
Write-Output 'PASS: missing defaults, existing choices, legacy migration, explicit zero, repeat migration, fresh defaults'

$legacy = New-Object WidgetMemoryKey
$legacy.SetValue('audio.placement', 'desktop', [Microsoft.Win32.RegistryValueKind]::String)
$legacy.SetValue('audio.height', 140, [Microsoft.Win32.RegistryValueKind]::DWord)
$legacy.SetValue('audio.barCount', 64, [Microsoft.Win32.RegistryValueKind]::DWord)
$legacy.SetValue('audio.color', '#123456', [Microsoft.Win32.RegistryValueKind]::String)
$legacy.SetValue('audio.sensitivity', 50, [Microsoft.Win32.RegistryValueKind]::DWord)
Initialize-DesktopWidgetSettings -Key $legacy
if ($legacy.GetValue('audio.barHeight') -ne '140' -or $legacy.GetValue('audio.appearance.primaryColor') -ne '#FF123456' -or $legacy.GetValue('audio.appearance.bars') -ne 64 -or $legacy.GetValue('audio.advanced.inputSensitivity') -ne '150') { throw 'Legacy audio migration failed' }
if ($legacy.GetValueKind('audio.barHeight') -ne [Microsoft.Win32.RegistryValueKind]::String) { throw 'Numeric string type lost' }
Write-Output 'PASS: audio engine upgrade, placement fallback, color/size/sensitivity conversion'

$v301 = New-Object WidgetMemoryKey
$v301.SetValue('audio.bars', 32, [Microsoft.Win32.RegistryValueKind]::DWord)
$v301.SetValue('audio.primaryColor', '#FF112233', [Microsoft.Win32.RegistryValueKind]::String)
$v301.SetValue('audio.autoGain', 0, [Microsoft.Win32.RegistryValueKind]::DWord)
$v301.SetValue('audio.horizontal', '63.5', [Microsoft.Win32.RegistryValueKind]::String)
Initialize-DesktopWidgetSettings -Key $v301
if ($v301.GetValue('audio.appearance.bars') -ne 32 -or $v301.GetValue('audio.appearance.primaryColor') -ne '#FF112233' -or $v301.GetValue('audio.advanced.autoGain') -ne 0 -or $v301.GetValue('audio.advanced.horizontal') -ne '63.5') { throw 'Grouped audio settings lost values' }
$v301.SetValue('audio.appearance.bars', 16, [Microsoft.Win32.RegistryValueKind]::DWord)
Initialize-DesktopWidgetSettings -Key $v301
if ($v301.GetValue('audio.appearance.bars') -ne 16) { throw 'Old audio value overwrote grouped choice' }
Write-Output 'PASS: 3.0.1 grouped migration preserves colors, counts, positions, explicit zero and later edits'
