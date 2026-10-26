' ZenDesktop 提权助手
' 用法: cscript //nologo elevate-run.vbs <batPath> <workingDir> [extraArgs]
Option Explicit
Dim sh, bat, cwd, extra, args
If WScript.Arguments.Count < 2 Then
  WScript.Echo "用法: elevate-run.vbs <bat路径> <工作目录> [额外参数]"
  WScript.Quit 1
End If
bat = WScript.Arguments(0)
cwd = WScript.Arguments(1)
extra = ""
If WScript.Arguments.Count >= 3 Then
  extra = Trim(WScript.Arguments(2))
End If
If Right(cwd, 1) <> "\" Then cwd = cwd & "\"
' WorkingDirectory is set via ShellExecute 3rd arg; bat also cds to %~dp0.
args = "/c """ & bat & """"
If Len(extra) > 0 Then args = args & " " & extra
Set sh = CreateObject("Shell.Application")
On Error Resume Next
sh.ShellExecute "cmd.exe", args, cwd, "runas", 1
If Err.Number <> 0 Then
  WScript.Echo "ShellExecute 失败: " & Err.Number & " " & Err.Description
  WScript.Quit 1
End If
WScript.Quit 0
