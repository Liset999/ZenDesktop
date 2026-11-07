# Repair animation source only; never compile, enable, or change settings.
param(
    [string]$SourcePath = 'C:\ProgramData\Windhawk\ModsSource\local@windows-animations.wh.cpp',
    [string]$DestinationPath = $SourcePath
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$diagnostics = Join-Path $root '.diagnostics'
New-Item -ItemType Directory -Path $diagnostics -Force | Out-Null
try {
    $source = [IO.File]::ReadAllText($SourcePath)
    $fixed = $source
    $metadataEnd = $fixed.IndexOf('// ==/WindhawkMod==')
    if ($metadataEnd -lt 0) { throw 'Missing Windhawk metadata block' }
    $comments = [regex]::Match($fixed, '(?m)^// Wuthering Waves[^\r\n]*\r?\n// \*-Win64-Shipping\.exe[^\r\n]*\r?\n// file name only;[^\r\n]*\r?\n')
    if ($comments.Success -and $comments.Index -lt $metadataEnd) {
        $fixed = $fixed.Remove($comments.Index, $comments.Length)
        $end = [regex]::new('(// ==/WindhawkMod==\r?\n)')
        $fixed = $end.Replace($fixed, ('$1' + "`r`n" + $comments.Value), 1)
    }
    # Embed the guard: Windhawk passes source on stdin, without ModsSource in -I.
    $guard = [IO.File]::ReadAllText((Join-Path $root 'optional\zen-game-guard.h'))
    $guard = [regex]::Replace($guard, '(?m)^#pragma once\s*\r?\n', '').TrimEnd()
    $block = "// BEGIN embedded zen-game-guard.h`n// Windhawk compiles stdin; keep this block synced with tools/sync_animation_guard.py.`n$guard`n// END embedded zen-game-guard.h"
    $pattern = [regex]::new('// BEGIN embedded zen-game-guard\.h.*?// END embedded zen-game-guard\.h|#include "zen-game-guard\.h"', [Text.RegularExpressions.RegexOptions]::Singleline)
    if ($pattern.Matches($fixed).Count -ne 1) { throw 'Expected exactly one game guard include/block; no changes made' }
    $fixed = $pattern.Replace($fixed, [Text.RegularExpressions.MatchEvaluator]{ param($m) $block })
    $header = [regex]::Match($fixed, '(?ms)^// ==WindhawkMod==\r?\n(.*?)^// ==/WindhawkMod==')
    if (-not $header.Success) { throw 'Invalid metadata block' }
    foreach ($line in ($header.Groups[1].Value -split '\r?\n')) {
        if ($line.Trim() -and $line -notmatch '^//\s+@[A-Za-z][\w-]*(?::[\w-]+)?\s+\S.*$') { throw "Invalid metadata line: $line" }
    }
    if ((Test-Path -LiteralPath $DestinationPath) -and [IO.File]::ReadAllText($DestinationPath) -ceq $fixed) {
        Write-Host 'Animation source already repaired.'
        exit 0
    }
    if (Test-Path -LiteralPath $DestinationPath) {
        $backup = Join-Path $diagnostics ('windows-animations-before-repair-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '.wh.cpp')
        Copy-Item -LiteralPath $DestinationPath -Destination $backup
        Write-Host "Backup: $backup"
    }
    [IO.File]::WriteAllText($DestinationPath, $fixed, [Text.UTF8Encoding]::new($false))
    if ([IO.File]::ReadAllText($DestinationPath) -cne $fixed) { throw 'Readback mismatch' }
    Write-Host "Repaired source: $DestinationPath"
} catch {
    Write-Host "Repair failed: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
