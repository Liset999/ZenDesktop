# Requires PowerShell 5+ STA. Launched by 打开配置工具.cmd
# Recreates the missing config-editor-wpf source: edit ExplorerBlurMica config.ini
# and register/unregister the BHO. Checks Maplespe CLSID {B44BD3C8-...}.

Add-Type -AssemblyName PresentationFramework, PresentationCore, WindowsBase, System.Windows.Forms

$ErrorActionPreference = 'Stop'
$script:Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$script:IniPath = Join-Path $script:Root 'config.ini'
$script:DllPath = Join-Path $script:Root 'ExplorerBlurMica.dll'
$script:RestartBat = Join-Path $script:Root 'restart_explorer.bat'
$script:Clsid = '{B44BD3C8-E597-4E08-AE43-246CE24698E7}'
$script:Dirty = $false

$script:Presets = [ordered]@{
    '默认'     = @{ effect = 1; clearAddress = $true; clearBarBg = $true; clearWinUIBg = $true; showLine = $true;  lr = 255; lg = 255; lb = 255; la = 147; dr = 30; dg = 30; db = 30; da = 185 }
    '轻透'     = @{ effect = 1; clearAddress = $true; clearBarBg = $true; clearWinUIBg = $true; showLine = $false; lr = 255; lg = 255; lb = 255; la = 70;  dr = 20; dg = 20; db = 20; da = 80 }
    '高可读性' = @{ effect = 1; clearAddress = $true; clearBarBg = $true; clearWinUIBg = $true; showLine = $true;  lr = 245; lg = 245; lb = 245; la = 210; dr = 18; dg = 18; db = 18; da = 220 }
    '深色清爽' = @{ effect = 1; clearAddress = $true; clearBarBg = $true; clearWinUIBg = $true; showLine = $true;  lr = 40;  lg = 40;  lb = 40;  la = 140; dr = 16; dg = 16; db = 16; da = 130 }
    '近全透'   = @{ effect = 3; clearAddress = $true; clearBarBg = $true; clearWinUIBg = $true; showLine = $false; lr = 255; lg = 255; lb = 255; la = 18;  dr = 8;  dg = 8;  db = 8;  da = 28 }
}

function Get-Ini {
    $cfg = @{
        effect = 1; clearAddress = $true; clearBarBg = $true; clearWinUIBg = $true; showLine = $true
        lr = 255; lg = 255; lb = 255; la = 147; dr = 30; dg = 30; db = 30; da = 185
    }
    if (-not (Test-Path $script:IniPath)) { return $cfg }
    $section = ''
    foreach ($line in Get-Content -LiteralPath $script:IniPath -Encoding UTF8) {
        $t = $line.Trim()
        if ($t -eq '' -or $t.StartsWith('#') -or $t.StartsWith(';')) { continue }
        if ($t -match '^\[(.+)\]$') { $section = $Matches[1].ToLower(); continue }
        if ($t -notmatch '^([^=]+)=(.*)$') { continue }
        $k = $Matches[1].Trim().ToLower()
        $v = $Matches[2].Trim()
        $bool = { param($x) $x -match '^(1|true|yes)$' }
        switch ($section) {
            'config' {
                switch ($k) {
                    'effect'        { $cfg.effect = [int]$v }
                    'clearaddress'  { $cfg.clearAddress = & $bool $v }
                    'clearbarbg'    { $cfg.clearBarBg = & $bool $v }
                    'clearwinuibg'  { $cfg.clearWinUIBg = & $bool $v }
                    'showline'      { $cfg.showLine = & $bool $v }
                }
            }
            'light' {
                switch ($k) { 'r' { $cfg.lr = [int]$v } 'g' { $cfg.lg = [int]$v } 'b' { $cfg.lb = [int]$v } 'a' { $cfg.la = [int]$v } }
            }
            'dark' {
                switch ($k) { 'r' { $cfg.dr = [int]$v } 'g' { $cfg.dg = [int]$v } 'b' { $cfg.db = [int]$v } 'a' { $cfg.da = [int]$v } }
            }
        }
    }
    return $cfg
}

function Write-Ini($cfg) {
    $tf = { param($b) if ($b) { 'true' } else { 'false' } }
    $text = @"
[config]
effect=$($cfg.effect)
clearAddress=$(& $tf $cfg.clearAddress)
clearBarBg=$(& $tf $cfg.clearBarBg)
clearWinUIBg=$(& $tf $cfg.clearWinUIBg)
showLine=$(& $tf $cfg.showLine)
[light]
r=$($cfg.lr)
g=$($cfg.lg)
b=$($cfg.lb)
a=$($cfg.la)
[dark]
r=$($cfg.dr)
g=$($cfg.dg)
b=$($cfg.db)
a=$($cfg.da)
"@
    $utf8 = New-Object System.Text.UTF8Encoding $false
    [IO.File]::WriteAllText($script:IniPath, $text.Trim() + "`r`n", $utf8)
}

function Test-Registered {
    $p = "HKLM:\SOFTWARE\Classes\CLSID\$($script:Clsid)\InProcServer32"
    if (-not (Test-Path $p)) { return $false }
    $val = (Get-ItemProperty $p).'(default)'
    return -not [string]::IsNullOrWhiteSpace($val)
}

function Read-Ui {
    return @{
        effect        = [int]$script:CbEffect.SelectedIndex
        clearAddress  = [bool]$script:ChkAddress.IsChecked
        clearBarBg    = [bool]$script:ChkBar.IsChecked
        clearWinUIBg  = [bool]$script:ChkWinUI.IsChecked
        showLine      = [bool]$script:ChkLine.IsChecked
        lr = [int]$script:SlLR.Value; lg = [int]$script:SlLG.Value; lb = [int]$script:SlLB.Value; la = [int]$script:SlLA.Value
        dr = [int]$script:SlDR.Value; dg = [int]$script:SlDG.Value; db = [int]$script:SlDB.Value; da = [int]$script:SlDA.Value
    }
}

function Write-Ui($cfg) {
    $script:Suppress = $true
    $script:CbEffect.SelectedIndex = [Math]::Max(0, [Math]::Min(3, [int]$cfg.effect))
    $script:ChkAddress.IsChecked = [bool]$cfg.clearAddress
    $script:ChkBar.IsChecked     = [bool]$cfg.clearBarBg
    $script:ChkWinUI.IsChecked   = [bool]$cfg.clearWinUIBg
    $script:ChkLine.IsChecked    = [bool]$cfg.showLine
    $script:SlLR.Value = $cfg.lr; $script:SlLG.Value = $cfg.lg; $script:SlLB.Value = $cfg.lb; $script:SlLA.Value = $cfg.la
    $script:SlDR.Value = $cfg.dr; $script:SlDG.Value = $cfg.dg; $script:SlDB.Value = $cfg.db; $script:SlDA.Value = $cfg.da
    $script:Suppress = $false
    Update-Preview
}

function Update-Preview {
    $toHex = { param($n) ('{0:X2}' -f [int]$n) }
    $script:LightSwatch.Background = [Windows.Media.BrushConverter]::new().ConvertFromString(('#{0}{1}{2}' -f (& $toHex $script:SlLR.Value), (& $toHex $script:SlLG.Value), (& $toHex $script:SlLB.Value)))
    $script:DarkSwatch.Background  = [Windows.Media.BrushConverter]::new().ConvertFromString(('#{0}{1}{2}' -f (& $toHex $script:SlDR.Value), (& $toHex $script:SlDG.Value), (& $toHex $script:SlDB.Value)))
    $script:LblLA.Text = [int]$script:SlLA.Value
    $script:LblDA.Text = [int]$script:SlDA.Value
}

function Refresh-Status($msg) {
    $dllOk = Test-Path $script:DllPath
    $regOk = Test-Registered
    $script:LblConfig.Text = "配置文件：$script:IniPath"
    if (Test-Path $script:RestartBat) {
        $script:LblRestart.Text = "已找到重启脚本：$script:RestartBat"
    } else {
        $script:LblRestart.Text = '未找到 restart_explorer.bat，需要手动重启资源管理器'
    }
    $script:LblDll.Text = "组件路径：$script:DllPath"
    if (-not $dllOk) {
        $script:LblComp.Text = '组件状态：缺少 DLL 组件文件'
        $script:LblComp.Foreground = '#B45309'
    } elseif ($regOk) {
        $script:LblComp.Text = '组件状态：已安装且注册（正在生效）'
        $script:LblComp.Foreground = '#047857'
    } else {
        $script:LblComp.Text = '组件状态：未注册安装（暂未生效）'
        $script:LblComp.Foreground = '#B91C1C'
    }
    if ($msg) {
        $script:LblStatus.Text = "状态：$msg"
        $script:LblStatus.Foreground = if ($msg -match '失败|找不到') { '#B91C1C' } else { '#047857' }
    }
}

function Mark-Dirty {
    if ($script:Suppress) { return }
    $script:Dirty = $true
    Update-Preview
}

function Invoke-ElevatedRegsvr32([string]$extraArgs) {
    if (-not (Test-Path $script:DllPath)) {
        [Windows.MessageBox]::Show('找不到 ExplorerBlurMica.dll', '配置工具') | Out-Null
        return $false
    }
    $arg = '/s "{0}"' -f $script:DllPath
    if ($extraArgs) { $arg = "$extraArgs $arg" }
    try {
        $p = Start-Process -FilePath "$env:SystemRoot\System32\regsvr32.exe" -ArgumentList $arg -Verb RunAs -Wait -PassThru
        return ($p.ExitCode -eq 0)
    } catch {
        return $false
    }
}

$xaml = @'
<Window xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        Title="资源管理器透明配置工具"
        Width="820" Height="920" MinWidth="720" MinHeight="700"
        WindowStartupLocation="CenterScreen"
        Background="#F3F6FA" FontFamily="Segoe UI, Microsoft YaHei UI">
  <Window.Resources>
    <Style TargetType="Button">
      <Setter Property="Padding" Value="14,8"/>
      <Setter Property="FontSize" Value="13"/>
      <Setter Property="Cursor" Value="Hand"/>
      <Setter Property="BorderThickness" Value="0"/>
    </Style>
  </Window.Resources>
  <ScrollViewer VerticalScrollBarVisibility="Auto">
    <StackPanel Margin="28,20,28,28">
      <TextBlock Text="资源管理器透明配置工具" FontSize="26" FontWeight="SemiBold" HorizontalAlignment="Center"/>
      <TextBlock Text="可视化参数调节。请先选择预设方案，如有需要再进行细节微调。"
                 FontSize="13" Foreground="#6B7280" HorizontalAlignment="Center" Margin="0,6,0,16"/>

      <Border Background="White" CornerRadius="10" Padding="18" Margin="0,0,0,12">
        <StackPanel>
          <TextBlock Text="运行状态与环境" FontSize="16" FontWeight="SemiBold" Margin="0,0,0,8"/>
          <TextBlock x:Name="LblConfig" FontSize="13" TextWrapping="Wrap"/>
          <TextBlock x:Name="LblRestart" FontSize="13" TextWrapping="Wrap" Margin="0,2,0,0"/>
          <TextBlock x:Name="LblStatus" FontSize="13" FontWeight="SemiBold" Margin="0,4,0,0" Foreground="#047857"/>
        </StackPanel>
      </Border>

      <Border Background="White" CornerRadius="10" Padding="18" Margin="0,0,0,12">
        <StackPanel>
          <TextBlock Text="美化组件管理 (ExplorerBlurMica)" FontSize="16" FontWeight="SemiBold"/>
          <TextBlock Text="将美化组件注册为系统 COM 组件，使资源管理器加载它以渲染透明磨砂效果。"
                     FontSize="12" Foreground="#6B7280" TextWrapping="Wrap" Margin="0,4,0,8"/>
          <TextBlock x:Name="LblComp" FontSize="13" FontWeight="SemiBold" Margin="0,0,0,2"/>
          <TextBlock x:Name="LblDll" FontSize="12" Foreground="#6B7280" TextWrapping="Wrap" Margin="0,0,0,12"/>
          <UniformGrid Columns="2">
            <Button x:Name="BtnReg" Content="注册安装美化组件" Background="#0F766E" Foreground="White" Margin="0,0,8,0"/>
            <Button x:Name="BtnUnreg" Content="注销卸载美化组件" Background="White" BorderBrush="#D1D5DB" BorderThickness="1" Margin="8,0,0,0"/>
          </UniformGrid>
        </StackPanel>
      </Border>

      <Border Background="White" CornerRadius="10" Padding="18" Margin="0,0,0,12">
        <StackPanel>
          <TextBlock Text="常用预设" FontSize="16" FontWeight="SemiBold"/>
          <TextBlock Text="先一键切换，再按需微调亮色/暗色参数。" FontSize="12" Foreground="#6B7280" Margin="0,4,0,10"/>
          <UniformGrid Columns="5" x:Name="PresetHost"/>
        </StackPanel>
      </Border>

      <Grid>
        <Grid.ColumnDefinitions>
          <ColumnDefinition Width="*"/>
          <ColumnDefinition Width="12"/>
          <ColumnDefinition Width="*"/>
        </Grid.ColumnDefinitions>
        <Border Grid.Column="0" Background="White" CornerRadius="10" Padding="18">
          <StackPanel>
            <TextBlock Text="效果类型" FontSize="16" FontWeight="SemiBold"/>
            <TextBlock Text="对应 config.ini 的 effect。保存后重新打开资源管理器窗口生效。"
                       FontSize="12" Foreground="#6B7280" TextWrapping="Wrap" Margin="0,4,0,10"/>
            <ComboBox x:Name="CbEffect" Height="32">
              <ComboBoxItem Content="0 Blur（仅旧版 Win11）"/>
              <ComboBoxItem Content="1 Acrylic"/>
              <ComboBoxItem Content="2 Mica（Win11）"/>
              <ComboBoxItem Content="3 Blur Clear"/>
            </ComboBox>
          </StackPanel>
        </Border>
        <Border Grid.Column="2" Background="White" CornerRadius="10" Padding="18">
          <StackPanel>
            <TextBlock Text="透明选项" FontSize="16" FontWeight="SemiBold"/>
            <TextBlock Text="选择要参与透明化覆盖的界面部分。" FontSize="12" Foreground="#6B7280" Margin="0,4,0,8"/>
            <CheckBox x:Name="ChkAddress" Content="地址栏透明" Margin="0,4"/>
            <CheckBox x:Name="ChkBar" Content="工具栏 / 滚动条透明" Margin="0,4"/>
            <CheckBox x:Name="ChkWinUI" Content="WinUI 区域透明" Margin="0,4"/>
            <CheckBox x:Name="ChkLine" Content="显示分隔线" Margin="0,4"/>
          </StackPanel>
        </Border>
      </Grid>

      <Grid Margin="0,12,0,0">
        <Grid.ColumnDefinitions>
          <ColumnDefinition Width="*"/>
          <ColumnDefinition Width="12"/>
          <ColumnDefinition Width="*"/>
        </Grid.ColumnDefinitions>
        <Border Grid.Column="0" Background="White" CornerRadius="10" Padding="18">
          <StackPanel>
            <DockPanel>
              <Border x:Name="LightSwatch" Width="22" Height="22" CornerRadius="4" DockPanel.Dock="Right"/>
              <TextBlock Text="亮色模式" FontSize="16" FontWeight="SemiBold"/>
            </DockPanel>
            <TextBlock Text="R / G / B / 不透明度 A" FontSize="12" Foreground="#6B7280" Margin="0,8,0,4"/>
            <Slider x:Name="SlLR" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            <Slider x:Name="SlLG" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            <Slider x:Name="SlLB" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            <DockPanel>
              <TextBlock x:Name="LblLA" DockPanel.Dock="Right" Width="36" TextAlignment="Right"/>
              <Slider x:Name="SlLA" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            </DockPanel>
          </StackPanel>
        </Border>
        <Border Grid.Column="2" Background="White" CornerRadius="10" Padding="18">
          <StackPanel>
            <DockPanel>
              <Border x:Name="DarkSwatch" Width="22" Height="22" CornerRadius="4" DockPanel.Dock="Right"/>
              <TextBlock Text="暗色模式" FontSize="16" FontWeight="SemiBold"/>
            </DockPanel>
            <TextBlock Text="R / G / B / 不透明度 A" FontSize="12" Foreground="#6B7280" Margin="0,8,0,4"/>
            <Slider x:Name="SlDR" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            <Slider x:Name="SlDG" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            <Slider x:Name="SlDB" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            <DockPanel>
              <TextBlock x:Name="LblDA" DockPanel.Dock="Right" Width="36" TextAlignment="Right"/>
              <Slider x:Name="SlDA" Minimum="0" Maximum="255" TickFrequency="1" IsSnapToTickEnabled="True"/>
            </DockPanel>
          </StackPanel>
        </Border>
      </Grid>

      <UniformGrid Columns="2" Margin="0,16,0,0">
        <Button x:Name="BtnSave" Content="保存配置" Background="#0F766E" Foreground="White" Margin="0,0,8,0" Height="40"/>
        <Button x:Name="BtnRestart" Content="保存并重启资源管理器" Background="White" BorderBrush="#D1D5DB" BorderThickness="1" Margin="8,0,0,0" Height="40"/>
      </UniformGrid>
    </StackPanel>
  </ScrollViewer>
</Window>
'@

$window = [Windows.Markup.XamlReader]::Parse($xaml)
$script:LblConfig  = $window.FindName('LblConfig')
$script:LblRestart = $window.FindName('LblRestart')
$script:LblStatus  = $window.FindName('LblStatus')
$script:LblComp    = $window.FindName('LblComp')
$script:LblDll     = $window.FindName('LblDll')
$script:CbEffect   = $window.FindName('CbEffect')
$script:ChkAddress = $window.FindName('ChkAddress')
$script:ChkBar     = $window.FindName('ChkBar')
$script:ChkWinUI   = $window.FindName('ChkWinUI')
$script:ChkLine    = $window.FindName('ChkLine')
$script:SlLR = $window.FindName('SlLR'); $script:SlLG = $window.FindName('SlLG'); $script:SlLB = $window.FindName('SlLB'); $script:SlLA = $window.FindName('SlLA')
$script:SlDR = $window.FindName('SlDR'); $script:SlDG = $window.FindName('SlDG'); $script:SlDB = $window.FindName('SlDB'); $script:SlDA = $window.FindName('SlDA')
$script:LblLA = $window.FindName('LblLA'); $script:LblDA = $window.FindName('LblDA')
$script:LightSwatch = $window.FindName('LightSwatch'); $script:DarkSwatch = $window.FindName('DarkSwatch')
$presetHost = $window.FindName('PresetHost')

foreach ($name in $script:Presets.Keys) {
    $b = New-Object Windows.Controls.Button
    $b.Content = $name
    $b.Margin = '0,0,8,8'
    $b.Background = [Windows.Media.Brushes]::White
    $b.BorderBrush = [Windows.Media.BrushConverter]::new().ConvertFromString('#D1D5DB')
    $b.BorderThickness = 1
    $b.Tag = $name
    $b.Add_Click({
        $n = [string]$this.Tag
        Write-Ui $script:Presets[$n]
        $script:Dirty = $true
        Refresh-Status "已套用预设「$n」，记得保存"
    })
    [void]$presetHost.Children.Add($b)
}

foreach ($ctrl in @($script:CbEffect, $script:ChkAddress, $script:ChkBar, $script:ChkWinUI, $script:ChkLine,
                    $script:SlLR, $script:SlLG, $script:SlLB, $script:SlLA,
                    $script:SlDR, $script:SlDG, $script:SlDB, $script:SlDA)) {
    if ($ctrl -is [Windows.Controls.Primitives.RangeBase]) {
        $ctrl.Add_ValueChanged({ Mark-Dirty })
    } elseif ($ctrl -is [Windows.Controls.ComboBox]) {
        $ctrl.Add_SelectionChanged({ Mark-Dirty })
    } else {
        $ctrl.Add_Click({ Mark-Dirty })
    }
}

$window.FindName('BtnReg').Add_Click({
    if (Invoke-ElevatedRegsvr32 '') {
        Start-Sleep -Milliseconds 400
        Refresh-Status '美化组件安装注册成功！'
    } else {
        Refresh-Status '注册失败（取消了 UAC 或 regsvr32 返回错误）'
    }
})
$window.FindName('BtnUnreg').Add_Click({
    if (Invoke-ElevatedRegsvr32 '/u') {
        Start-Sleep -Milliseconds 400
        Refresh-Status '已注销美化组件'
    } else {
        Refresh-Status '注销失败'
    }
})
$window.FindName('BtnSave').Add_Click({
    Write-Ini (Read-Ui)
    $script:Dirty = $false
    Refresh-Status '配置已保存。重新打开资源管理器窗口后生效。'
})
$window.FindName('BtnRestart').Add_Click({
    Write-Ini (Read-Ui)
    $script:Dirty = $false
    if (Test-Path $script:RestartBat) {
        Start-Process -FilePath $script:RestartBat
        Refresh-Status '已保存并重启资源管理器'
    } else {
        Refresh-Status '已保存。未找到 restart_explorer.bat，请手动重启 Explorer'
    }
})

$window.Add_Closing({
    param($s, $e)
    if (-not $script:Dirty) { return }
    $r = [Windows.MessageBox]::Show('有未保存的更改，退出前需要保存吗？', '配置工具', 'YesNoCancel', 'Question')
    if ($r -eq 'Yes') { Write-Ini (Read-Ui) }
    elseif ($r -eq 'Cancel') { $e.Cancel = $true }
})

Write-Ui (Get-Ini)
Refresh-Status '已加载配置文件。可直接选择预设方案。'
[void]$window.ShowDialog()
