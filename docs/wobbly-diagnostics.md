# 果冻窗口开启后无效果：排查记录

## 2026-11-07 本机可用、用户不可用的版本对比

用户确认本机果冻效果可用。本机只读报告显示 Windows 26H2 / 26300.9818，注册版本和已安装源码均为 0.115.7，模块已开启，64位DLL存在，进程包含列表明确包含 dwm.exe，Windhawk服务运行。受影响用户的版本和调试日志仍待收集，尚不能确认与下述0.115.6初始化失败属于同一原因。

诊断脚本保存为带BOM的UTF-8，兼容Windows PowerShell 5.1的中文源码读取。发布包包含该脚本与本文。先对比报告中的 WindowsBuild、RegisteredVersion、SourceVersion、DllPresent 和 DwmExplicitlyIncluded，再依据模块调试日志定位失败阶段。

### 受影响用户截图确认

用户提供的原始截图显示0.115.7已在dwm.exe中执行，依次输出 `Native maximize/restore hooks: request=available animation=generic-visual-sync`、`compositor primary vftable verified by PDB`、`outer visual proxy offset could not be proven`，随后输出 `Failed to initialize DWM hooks`。已确定退出点是 `FindVisualProxyAccessPath` 返回false。截图未包含Windows内部版本、udwm.dll版本或该函数的机器码，尚不能确定具体不匹配的指令。

当前解析器要求 `CTopLevelWindow::GetCanvasRootVisualProxy` 使用两次rax指针读取，允许直接返回或精确的 `test rax,rax; jnz +2; ret; int3` 空指针分支，并验证成员偏移范围与对齐。0.115.7仍在该用户环境失败，说明已有的特定分支兼容修复未覆盖此环境，需取得受影响系统的udwm.dll和Windows内部版本后分析。不得根据最终失败信息直接放宽验证或硬编码本机偏移。

### 用户DLL分析与0.115.8修复

随后收到用户Windows截图，26H2 / 26300.9711。所附uDWM.dll文件版本为10.0.26100.9711，匹配PDB标识为28CF5A3BD2BC549F2F4E4EFCB7FBF89C1。微软匹配符号定位 `CTopLevelWindow::GetCanvasRootVisualProxy` 至RVA 0x85d30，机器码如下。

```text
48 8b 81 f8 00 00 00  mov rax,[rcx+0xf8]
48 85 c0              test rax,rax
74 06                 jz null_return
48 8b 40 10           mov rax,[rax+0x10]
c3                    ret
cc                    int3
c3        null_return: ret
```

0.115.7只识别空指针提前返回的jnz布局，因此拒绝该函数。0.115.8新增对上述精确jz布局的识别，同时检查跳转位置、两个返回、填充字节、读取指令与偏移约束。使用用户机器码新增回归用例，修改前复现拒绝，修改后通过；错误跳转、缺少空指针返回、截断代码和缺少正常返回均拒绝。最外层视觉选择回归通过，Windhawk编译器完整DLL编译通过。仍待用户加载0.115.8后提供初始化日志及拖动效果验收。

## 历史排查

### 0.115.9 恢复兼容访问路径

偏移解析失败时继续初始化，清空未证明的偏移，记录 `using typed Canvas accessor fallback`。只在偏移不可用时，通过已经解析并验证函数地址的 `GetCanvasRootVisualProxy` 获取代理，并沿用精确代理类型校验。偏移已验证时仍使用最外层视觉节点；已知节点的代理无效则拒绝绑定。ABI日志显示 `visual=complete-root` 或 `visual=typed-canvas-fallback`，原有绑定日志记录 `TypedCanvasAccessor`。其他核心函数、对象与线程检查继续生效。

该兼容路径可能使窗口形变被固定外框裁切，已写入模块说明。新增回归用例在修改前复现未知偏移无法获取代理，修改后回退、无效返回、缺失访问函数和最外层优先策略均通过检查。初始化分支检查确认未知布局可继续初始化且丢弃未证明偏移。两种真实系统指令布局回归通过，Windhawk编译器完整DLL编译通过。用户实机效果仍待验收。

### 旧版回退与新增初始化门槛

核对仓库最早收录版本8a8f1dd的0.115源码，`FindVisualProxyAccessPath`失败仅记录 `visual accessor path unavailable; using typed accessor validation`，继续初始化。视觉选择可回退到PDB解析的 `GetCanvasRootVisualProxy`，并验证返回代理类型。当前0.115.6起为解决外框裁切改为最外层视觉节点，偏移验证失败直接返回false。因此本次用户无效果与本地新增的强制初始化条件直接相关。最早收录版不能等同于已核验的上游原版；本次读取上游源码正文因TLS错误失败，上游当前实现尚未完成对比。

2026-11-05：用户报告本机及多位用户开启后无效果，目前未取得受影响用户的运行日志，不能归为同一根因。

本机只读核验：Windows 26H2 / 26300.9818，果冻模块注册版本与已安装源码均为 0.115.6，Disabled=0，注册的64位DLL存在，Windhawk服务运行，进程包含列表有 dwm.exe。上述条件均不证明DWM中初始化成功。未改运行设置、未重新编译或重启进程。

## 收集报告

在64位PowerShell中运行（脚本只输出JSON）：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\tools\Get-WobblyDiagnostics.ps1
```

默认查标准安装路径 `%ProgramData%\Windhawk` 和 HKLM；便携安装需另行核对其 windhawk.ini，不应把未找到默认路径视为未安装。管理员运行可减少进程/服务的读取权限限制。报告不包含用户名或机器名。

同时提供 Windhawk 果冻模块的调试日志：打开模块日志功能，拖动普通窗口，复制输出。若开启日志前已经初始化，可能缺少启动日志；不能仅凭没有初始化行断定未注入。不要为收集日志强行结束 dwm.exe。

## 按日志区分问题

- 没有模块输出：先确认日志捕获方式与时间，再核对注入状态。
- `Failed to initialize DWM hooks`：查看其前的 `DWM compatibility` 或符号解析失败信息。
- `outer visual proxy offset could not be proven`：0.115.6 的最外层视觉路径验证未通过，模块明确返回初始化失败。不能删除校验强行运行。
- `Initialized successfully`：继续看 `BIND FAILED`、`BIND PIPELINE UNAVAILABLE`、场景线程/唤醒日志，不能再归因于没打开开关。
- `Refusing to load: repeated recent Dwminit warnings`：命中近期DWM警告保护，先调查事件，不能直接移除保护。

兼容性返回分支来自本地源码，并非本机已命中的错误。拿到运行日志后才能确定修复方向；不要把静态检查或编译通过当作实际果冻效果验收。

## 本机日志已确认的根因与修复

用户随后提供的日志确认0.115.6已经注入dwm.exe，在 `outer visual proxy offset could not be proven` 后初始化失败。
本机PDB定位的 `CTopLevelWindow::GetCanvasRootVisualProxy` 指令为：

```text
48 8b 81 f8 00 00 00   mov rax,[rcx+0xf8]
48 85 c0               test rax,rax
75 02                  jnz nonnull
c3                     ret
cc                     int3
48 8b 40 10   nonnull:  mov rax,[rax+0x10]
c3                     ret
```

原解析器线性扫描，在空指针分支的ret处停止，未读到代理偏移。0.115.7只接受完整的两次读取与返回，允许上述精确空指针分支，拒绝未知分支；偏移仍动态提取且检查范围与对齐，不移除初始化安全门。

`python tools/tests/check_wobbly_access_path.py` 在修改前复现失败，修改后通过，覆盖带空指针分支、直接读取、错误跳转目标及缺少返回的情况。完整源码经Windhawk编译器标准输入语法检查通过。以上不代替DWM实际加载和拖动验收，其他用户是否同一原因仍需日志。
