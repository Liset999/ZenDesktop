$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$source = Get-Content -LiteralPath (Join-Path $root 'deploy.ps1') -Raw -Encoding UTF8
$failures = @()
if ($source -match 'Stop-Process -Name windhawk -Force') {
    $failures += 'Deployment still force-terminates Windhawk.'
}
if ($source -notmatch 'ArgumentList @\("-exit", "-wait", "-timeout", "45000"\)') {
    $failures += 'Deployment does not wait for the supported engine shutdown.'
}
if ($source -notmatch 'if \(\$copyResult -ne "skipped" -or \(\$file.DirectoryName -eq \$optionalDir -and \$sharedHeadersChanged\)\)\s*\{\s*\$modKey.SetValue\("LibraryFileName"') {
    $failures += 'Unchanged sources lose their compiled library registration.'
}
if ($source -match 'Start-Service -Name Windhawk -ErrorAction SilentlyContinue') {
    $failures += 'Service startup failures are suppressed.'
}
if ($failures.Count) { throw ($failures -join "`n") }
$tokens = $null
$parseErrors = $null
[void][System.Management.Automation.Language.Parser]::ParseFile((Join-Path $root 'deploy.ps1'), [ref]$tokens, [ref]$parseErrors)
if ($parseErrors.Count) { throw ($parseErrors | Out-String) }
Write-Output 'PASS: deployment lifecycle checks and PowerShell syntax.'
