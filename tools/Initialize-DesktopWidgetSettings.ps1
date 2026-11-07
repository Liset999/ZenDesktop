# Settings defaults are generated from the mod's WindhawkModSettings block.
# Accept an open key so deploy and targeted repairs share exactly one migration.
function Initialize-DesktopWidgetSettings {
    param(
        [Parameter(Mandatory)] $Key,
        [string] $DefaultsPath = (Join-Path $PSScriptRoot 'desktop-widget-defaults.json')
    )
    # Preserve old clock advanced values when moving their UI group to the end.
    foreach ($name in @($Key.GetValueNames())) {
        if ($name.StartsWith('clock.advanced.')) {
            $suffix = $name.Substring('clock.advanced.'.Length)
            $destination = if ($suffix -eq 'use24Hour') { 'clock.use24Hour' } else { 'clockAdvanced.' + $suffix }
            if ($Key.GetValueNames() -notcontains $destination) {
                $Key.SetValue($destination, $Key.GetValue($name), $Key.GetValueKind($name))
            }
        }
    }
    # 3.0: the community engine has a different settings model. Only migrate
    # into missing destinations; leave old values available for rollback.
    $audioNames = @($Key.GetValueNames())
    $placement = if ($audioNames -contains 'audio.placement') { [string]$Key.GetValue('audio.placement') } else { 'taskbar' }
    if ($placement -eq 'both') {
        # Upstream has one host per audio process. Prefer the verified taskbar
        # preset; the user can choose panel/desktop in the same settings page.
        $Key.SetValue('audio.placement', 'taskbar', [Microsoft.Win32.RegistryValueKind]::String)
        $placement = 'taskbar'
    }
    $migration = @{}
    if ($audioNames -contains 'audio.barCount') { $migration['audio.bars'] = [int]$Key.GetValue('audio.barCount') }
    if ($audioNames -contains 'monitor') { $migration['audio.monitor'] = [int]$Key.GetValue('monitor') }
    if (($placement -ne 'taskbar') -and ($audioNames -contains 'audio.height')) { $migration['audio.barHeight'] = [string]$Key.GetValue('audio.height') }
    if ($audioNames -contains 'audio.sensitivity') {
        $migration['audio.inputSensitivity'] = [string]([Math]::Min(300, [Math]::Max(0, 3 * [int]$Key.GetValue('audio.sensitivity'))))
    }
    foreach ($pair in @(@('audio.color', 'audio.primaryColor'), @('audio.color2', 'audio.secondaryColor'))) {
        if ($audioNames -contains $pair[0]) {
            $color = [string]$Key.GetValue($pair[0])
            if ($color -match '^#[0-9a-fA-F]{6}$') { $color = '#FF' + $color.Substring(1) }
            if ($color -match '^#[0-9a-fA-F]{8}$') { $migration[$pair[1]] = $color }
        }
    }
    foreach ($name in $migration.Keys) {
        if ($Key.GetValueNames() -notcontains $name) {
            $kind = if ($migration[$name] -is [string]) { [Microsoft.Win32.RegistryValueKind]::String } else { [Microsoft.Win32.RegistryValueKind]::DWord }
            $Key.SetValue($name, $migration[$name], $kind)
        }
    }
    # 3.0.2: group audio appearance/details without losing saved choices.
    foreach ($group in @('appearance', 'advanced')) {
        $fields = if ($group -eq 'appearance') { @('bars','barWidth','barGap','primaryColor','secondaryColor') } else { @('monitor','horizontal','vertical','inputSensitivity','autoGain','smoothing','fps') }
        foreach ($field in $fields) {
            $oldName = 'audio.' + $field
            $newName = 'audio.' + $group + '.' + $field
            if (($Key.GetValueNames() -contains $oldName) -and ($Key.GetValueNames() -notcontains $newName)) {
                $Key.SetValue($newName, $Key.GetValue($oldName), $Key.GetValueKind($oldName))
            }
        }
    }
    # Missing values read as zero/empty through the Windhawk API. Seed every
    # declared default, while respecting explicit zero values and user choices.
    $defaults = Get-Content -LiteralPath $DefaultsPath -Raw -Encoding UTF8 | ConvertFrom-Json
    foreach ($property in $defaults.PSObject.Properties) {
        if ($Key.GetValueNames() -notcontains $property.Name) {
            $kind = if ($property.Value -is [string]) { [Microsoft.Win32.RegistryValueKind]::String } else { [Microsoft.Win32.RegistryValueKind]::DWord }
            $Key.SetValue($property.Name, $property.Value, $kind)
        }
    }
}
