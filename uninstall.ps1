# ============================================================
#  ZenDesktop Uninstaller
#  Removes the ZenDesktop mods that deploy.ps1 installed into Windhawk.
#  Windhawk itself is never uninstalled.
#
#  uninstall.ps1               # asks before removing anything
#  uninstall.ps1 -Yes          # no prompts; keeps %LOCALAPPDATA%\ZenDesktop
#  uninstall.ps1 -Yes -RemoveData   # no prompts; also deletes %LOCALAPPDATA%\ZenDesktop
# ============================================================
param(
    [switch]$Yes,
    [switch]$RemoveData
)

function Write-Status ($text, $color = "Cyan") {
    Write-Host "[ZenUninstall] $text" -ForegroundColor $color
}

function Ask-YesNo ($question) {
    if ($Yes) { return $true }
    $answer = Read-Host "$question [Y/N]"
    return ($answer -match '^\s*[yY]')
}

# 1. Administrator check (HKLM and ProgramData need it)
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Error "需要管理员权限：请运行 uninstall.bat，在权限提示里点「是」。 (Administrator privileges are required. Run uninstall.bat and accept the UAC prompt.)"
    Exit 1
}

# 2. The mods deploy.ps1 installs: every local@*.wh.cpp in this folder and in
#    optional\, plus the fixed list below in case uninstall.ps1 is run from
#    another folder. local@macos-minimize-animation was installed by
#    ZenDesktop v4.1 and is merged into Windows Animations since v4.2.
$modIds = New-Object System.Collections.Generic.List[string]
foreach ($id in @(
    "local@zen-taskbar-acrylic",
    "local@zen-notificationcenter-acrylic",
    "local@zen-startmenu-acrylic",
    "local@zen-desktop-toggle-icons",
    "local@zen-explorer-styler",
    "local@zen-taskbar-lyrics",
    "local@windows-11-settings-styler-fork",
    "local@wobbly-windows",
    "local@windows-animations",
    "local@zen-desktop-widgets",
    "local@zen-dock",
    "local@macos-minimize-animation"
)) { $modIds.Add($id) }
foreach ($dir in @($PSScriptRoot, (Join-Path $PSScriptRoot "optional"))) {
    if (Test-Path -LiteralPath $dir) {
        foreach ($f in @(Get-ChildItem -LiteralPath $dir -Filter "local@*.wh.cpp" -ErrorAction SilentlyContinue)) {
            $id = $f.BaseName -replace '\.wh$', ''
            if (-not $modIds.Contains($id)) { $modIds.Add($id) }
        }
    }
}

# 3. Find Windhawk and its data folder (same logic as deploy.ps1)
function Find-WindhawkDir {
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
    $proc = Get-Process windhawk -ErrorAction SilentlyContinue | Where-Object { $_.Path } | Select-Object -First 1
    if ($proc -and $proc.Path) { return (Split-Path $proc.Path) }
    $fallbacks = @()
    if ($env:ProgramFiles) { $fallbacks += (Join-Path $env:ProgramFiles "Windhawk") }
    $fallbacks += @("C:\Program Files\Windhawk", (Join-Path $PSScriptRoot "Windhawk"), $PSScriptRoot)
    foreach ($fallback in $fallbacks) {
        if (Test-Path -LiteralPath (Join-Path $fallback "windhawk.exe")) { return $fallback }
    }
    return $null
}

$windhawkDir = Find-WindhawkDir
$windhawkData = if ($env:ProgramData) { Join-Path $env:ProgramData "Windhawk" } else { "C:\ProgramData\Windhawk" }
if ($windhawkDir) {
    $windhawkDir = $windhawkDir.TrimEnd('\')
    $iniPath = Join-Path $windhawkDir "windhawk.ini"
    if (Test-Path -LiteralPath $iniPath) {
        $ini = Get-Content -LiteralPath $iniPath
        $isPortable = $false
        $foundAppDataPath = $null
        foreach ($line in $ini) {
            if ($line -match '^\s*Portable\s*=\s*(\d+)') { if ($Matches[1] -eq "1") { $isPortable = $true } }
            if ($line -match '^\s*AppDataPath\s*=\s*(.+)') { $foundAppDataPath = $Matches[1].Trim() }
        }
        if ($foundAppDataPath) {
            $windhawkData = [System.Environment]::ExpandEnvironmentVariables($foundAppDataPath)
            if (-not [System.IO.Path]::IsPathRooted($windhawkData)) {
                $windhawkData = [System.IO.Path]::GetFullPath((Join-Path $windhawkDir $windhawkData))
            }
        } elseif ($isPortable) {
            $windhawkData = Join-Path $windhawkDir "AppData"
        }
    } elseif (Test-Path -LiteralPath "$windhawkDir\AppData\ModsSource") {
        $windhawkData = "$windhawkDir\AppData"
    }
} else {
    Write-Status "未找到 Windhawk 安装目录，按默认数据目录清理。 (Windhawk installation not found; using the default data folder.)" "Yellow"
}
$modsSource = Join-Path $windhawkData "ModsSource"
$compiledRoot = Join-Path $windhawkData "Engine\Mods"
$zenData = Join-Path $env:LOCALAPPDATA "ZenDesktop"
$zenTaskName = "StartupCheck"
$zenTaskPath = "\ZenDesktop\"

# 4. Show what will be removed and confirm
$installed = @($modIds | Where-Object {
    (Test-Path "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$_") -or (Test-Path -LiteralPath (Join-Path $modsSource "$_.wh.cpp"))
})
$task = Get-ScheduledTask -TaskName $zenTaskName -TaskPath $zenTaskPath -ErrorAction SilentlyContinue

Write-Status "================================================" "Green"
Write-Status "   ZenDesktop 卸载 (Uninstall)" "Green"
Write-Status "================================================" "Green"
Write-Host "Windhawk 数据目录 (data folder): $windhawkData"
Write-Host ""
if ($installed.Count -eq 0 -and -not $task) {
    Write-Status "Windhawk 里没有找到 ZenDesktop 的 mod，也没有开机自检任务。 (No ZenDesktop mods or startup task found.)" "Yellow"
} else {
    Write-Host "将要移除 (Will remove):"
    foreach ($id in $installed) { Write-Host "  - $id" }
    if ($task) { Write-Host "  - 开机自检计划任务 (startup self-check task) $zenTaskPath$zenTaskName" }
    Write-Host ""
    Write-Host "包括 mod 源码、编译好的 DLL、Windhawk 里的 mod 记录和设置。Windhawk 本身不会被卸载。"
    Write-Host "(Mod sources, compiled DLLs, Windhawk mod entries and their settings. Windhawk itself stays installed.)"
    Write-Host ""
    if (-not (Ask-YesNo "确定移除吗？ (Remove them?)")) {
        Write-Status "已取消，没有做任何改动。 (Cancelled, nothing was changed.)" "Yellow"
        Exit 0
    }

    # 5. Stop Windhawk so the mods are unloaded and their files can be deleted
    Write-Status "正在停止 Windhawk... (Stopping Windhawk...)"
    Stop-Process -Name windhawk -Force -ErrorAction SilentlyContinue
    Stop-Service -Name Windhawk -Force -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 2

    $locked = 0
    foreach ($id in $installed) {
        Write-Status "移除 (Removing) $id"
        $src = Join-Path $modsSource "$id.wh.cpp"
        if (Test-Path -LiteralPath $src) { Remove-Item -LiteralPath $src -Force -ErrorAction SilentlyContinue }
        if (Test-Path -LiteralPath $compiledRoot) {
            foreach ($dll in @(Get-ChildItem -LiteralPath $compiledRoot -Recurse -File -Filter "$($id)_*.dll" -ErrorAction SilentlyContinue)) {
                try { Remove-Item -LiteralPath $dll.FullName -Force -ErrorAction Stop }
                catch { $locked++; Write-Status "  文件被占用，重启电脑后可手动删除 (in use, delete after a reboot): $($dll.FullName)" "Yellow" }
            }
        }
        foreach ($key in @("HKLM:\SOFTWARE\Windhawk\Engine\Mods\$id", "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$id")) {
            if (Test-Path $key) { Remove-Item -Path $key -Recurse -Force -ErrorAction SilentlyContinue }
        }
    }

    # Windhawk rebuilds its installed-mods list from this cache file (deploy.ps1 clears it too)
    $cacheFile = Join-Path $windhawkData "userprofile.json"
    if (Test-Path -LiteralPath $cacheFile) { Remove-Item -LiteralPath $cacheFile -Force -ErrorAction SilentlyContinue }

    # 6. Startup self-check task (deploy.ps1 -StartupCheck check|restart)
    if ($task) {
        Unregister-ScheduledTask -TaskName $zenTaskName -TaskPath $zenTaskPath -Confirm:$false -ErrorAction SilentlyContinue
        Write-Status "已移除开机自检任务。 (Startup self-check task removed.)" "Green"
    }

    Write-Status "正在重新启动 Windhawk... (Starting Windhawk again...)"
    Start-Service -Name Windhawk -ErrorAction SilentlyContinue
    if ($locked -gt 0) {
        Write-Status "有 $locked 个文件正被占用，重启电脑后再运行一次 uninstall.bat 即可删掉。 ($locked file(s) in use; run uninstall.bat again after a reboot.)" "Yellow"
    }
}

# 7. Optional: ZenDesktop's own data folder (AI status files and tools, startup-check log)
if (Test-Path -LiteralPath $zenData) {
    $removeZenData = $RemoveData
    if (-not $Yes) {
        $removeZenData = Ask-YesNo "同时删除 $zenData 吗？里面是 AI 状态文件和开机自检日志。 (Also delete $zenData? It holds AI status files and the startup-check log.)"
    }
    if ($removeZenData) {
        Remove-Item -LiteralPath $zenData -Recurse -Force -ErrorAction SilentlyContinue
        Write-Status "已删除 (Deleted) $zenData" "Green"
    } else {
        Write-Status "保留 (Kept) $zenData" "Gray"
    }
}

Write-Host ""
Write-Host "说明 (Notes):"
Write-Host "  1. deploy 往 Windhawk「设置 → 高级设置」的进程包含列表加了 dwm.exe，排除列表加了一些游戏和反作弊进程。"
Write-Host "     这些没有被改动，想还原请在那里手动删除。"
Write-Host "     (deploy added dwm.exe to Windhawk's process include list and some games / anti-cheat processes to the"
Write-Host "     exclude list. They were left as they are; remove them in Windhawk > Settings > Advanced if you want.)"
Write-Host "  2. 接过 AI 状态的，记得把当时粘贴到各 AI 助手配置里的 zen-status 片段删掉。"
Write-Host "     (If you connected AI status, remove the zen-status snippets you pasted into your assistants' configs.)"
Write-Host ""

if (-not $Yes) {
    if (Ask-YesNo "现在重启资源管理器，让任务栏和开始菜单恢复系统样式吗？ (Restart Explorer now to restore the default taskbar and Start menu?)") {
        Stop-Process -Name explorer -Force -ErrorAction SilentlyContinue
        Stop-Process -Name StartMenuExperienceHost -Force -ErrorAction SilentlyContinue
        Stop-Process -Name SearchHost -Force -ErrorAction SilentlyContinue
        Start-Sleep -Seconds 2
        if (-not (Get-Process explorer -ErrorAction SilentlyContinue)) { Start-Process explorer.exe }
    } else {
        Write-Host "稍后运行 restart_explorer.bat 或重启电脑即可。 (Run restart_explorer.bat or reboot later.)"
    }
}

Write-Status "================================================" "Green"
Write-Status "   ZenDesktop 卸载完成 (Uninstall finished)" "Green"
Write-Status "================================================" "Green"
