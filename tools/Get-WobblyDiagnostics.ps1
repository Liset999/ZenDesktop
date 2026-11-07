# Read-only support report. Does not enable logging, load DLLs or restart DWM.
# Run in 64-bit PowerShell; outputs JSON without usernames or machine names.
[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
$base = 'HKLM:\SOFTWARE\Windhawk\Engine'
$os = Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion'
$settings = Get-ItemProperty "$base\Settings" -ErrorAction SilentlyContinue
$mod = Get-ItemProperty "$base\Mods\local@wobbly-windows" -ErrorAction SilentlyContinue
$include = @(([string]$settings.Include) -split '\|' | ForEach-Object { $_.Trim() })
$appData = Join-Path $env:ProgramData 'Windhawk'
$library = [string]$mod.LibraryFileName
$dllExists = $false
if ($library -and [IO.Path]::GetFileName($library) -eq $library) {
    $dllExists = Test-Path -LiteralPath (Join-Path $appData "Engine\Mods\64\$library")
}
$source = Join-Path $appData 'ModsSource\local@wobbly-windows.wh.cpp'
$sourceVersion = $null
if (Test-Path -LiteralPath $source) {
    $match = [regex]::Match([IO.File]::ReadAllText($source), '(?m)^//\s+@version\s+(\S+)')
    if ($match.Success) { $sourceVersion = $match.Groups[1].Value }
}
$service = Get-Service Windhawk -ErrorAction SilentlyContinue
$processes = @(Get-Process windhawk -ErrorAction SilentlyContinue)
$warnings = [Collections.Generic.List[string]]::new()
if (-not [Environment]::Is64BitProcess) { $warnings.Add('请用64位PowerShell运行，32位注册表视图可能不完整。') }
if (-not $mod) { $warnings.Add('未找到果冻模块注册信息。') }
elseif ($mod.Disabled -ne 0) { $warnings.Add('果冻模块尚未开启。') }
if ($include -notcontains 'dwm.exe') { $warnings.Add('包含列表未明确列出dwm.exe；如有通配符规则，请人工核对。') }
if (-not $dllExists) { $warnings.Add('未找到已注册的64位DLL；编译或安装状态需要检查。') }
if ($sourceVersion -and $mod.Version -and $sourceVersion -ne $mod.Version) {
    $warnings.Add('源码版本与注册版本不同，请核对编译状态。')
}
if (-not $service -or $service.Status -ne 'Running') { $warnings.Add('Windhawk服务未运行或当前权限无法读取服务状态。') }
if ($processes.Count -eq 0) { $warnings.Add('未发现Windhawk进程，或当前环境无法枚举进程。') }
[ordered]@{
    ReportVersion = 1
    WindowsVersion = [string]$os.DisplayVersion
    WindowsBuild = "$($os.CurrentBuild).$($os.UBR)"
    Is64BitPowerShell = [Environment]::Is64BitProcess
    ModRegistered = [bool]$mod
    ModEnabled = if ($mod) { $mod.Disabled -eq 0 } else { $null }
    RegisteredVersion = $mod.Version
    SourceVersion = $sourceVersion
    DllPresent = $dllExists
    DwmExplicitlyIncluded = $include -contains 'dwm.exe'
    DebugLoggingEnabled = if ($mod) { [bool]$mod.LoggingEnabled } else { $null }
    WindhawkService = if ($service) { [string]$service.Status } else { 'Unknown' }
    WindhawkProcessCount = $processes.Count
    Warnings = @($warnings.ToArray())
    RuntimeStatus = '未验证：开关和DLL存在不能证明已注入或初始化成功。请附果冻模块调试日志。'
} | ConvertTo-Json -Depth 3
