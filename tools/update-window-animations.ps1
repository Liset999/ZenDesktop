# Stage updated sources for compilation in the Windhawk UI.
param(
    [string]$ModsSource = 'C:\ProgramData\Windhawk\ModsSource',
    [switch]$WithWidgetCaptureRegion
)
$ErrorActionPreference = 'Stop'
try {
    $root = Split-Path -Parent $PSScriptRoot
    $files = @('local@windows-animations.wh.cpp')
    if ($WithWidgetCaptureRegion) { $files += 'local@zen-desktop-widgets.wh.cpp' }
    foreach ($file in $files) {
        if (-not (Test-Path -LiteralPath (Join-Path $ModsSource $file) -PathType Leaf)) {
            throw "Installed source $file not found. Pass the correct -ModsSource directory."
        }
        if (-not (Test-Path -LiteralPath (Join-Path $root "optional\$file") -PathType Leaf)) {
            throw "Workspace source $file not found."
        }
    }
    $diagnostics = Join-Path $root '.diagnostics'
    $backup = Join-Path $diagnostics ('animation-sources-before-update-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
    New-Item -ItemType Directory -Path $backup -Force | Out-Null
    foreach ($file in $files) {
        Copy-Item -LiteralPath (Join-Path $ModsSource $file) -Destination $backup
    }
    Write-Host "Original sources backed up at $backup"
    foreach ($file in $files) {
        $source = Join-Path $root "optional\$file"
        $destination = Join-Path $ModsSource $file
        Copy-Item -LiteralPath $source -Destination $destination -Force
        if ((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $destination).Hash) {
            throw "Source verification failed for $file. Backup at $backup."
        }
        Write-Host "Updated $file"
    }
    Write-Host 'Return to Windhawk and compile each updated mod.'
} catch {
    Write-Host "Update failed: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
