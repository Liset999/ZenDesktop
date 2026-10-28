# ZenDesktop v5.0.0

> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

相对 v4.1 的主要更新。

## 本分支修复（v-next）
- **性能（全 Mod）**：热路径缓存 / TaskbarCreated 防抖 / 减少无效重绘；详见 [docs/perf.md](docs/perf.md)。版本点号见各 `.wh.cpp` 更新日志。
- **任务栏美化** 4.2.8：资源管理器自重启后自动补挂任务栏 / 托盘 / 音量浮窗（TaskbarCreated + 开机重试）；开了「统一设置」不必再手动重置。
- **设置美化** 4.2.5：跳过「个性化 → 背景」壁纸面板样式并收窄全局透明规则，减轻 Win11 26H2 进背景页闪退。
- **设置美化** 4.2.4：修复控制面板白底卡片盖住白色文字。
- **任务栏美化** 4.3.0：新增「任务栏尺寸」——可调任务栏高度与图标大小（默认 0 = 系统默认，不改动），移植自 m417z「Taskbar height and icon size」(GPL-3.0)。
- **文件管理器美化** 1.7.4：修复开窗闪一下；与上游 Explorer Styler 冲突时少弹窗（请关闭上游 mod）；1.7.3 选中改为真正半透明深色玻璃（无浅色描边）；控制面板顶部黑条改透明；1.7.2 控制面板暗链对比度与地址栏白条；1.7.1 起 GetTargetWnds 短缓存；1.7.0 起减轻首次打开黑闪、子菜单玻璃对齐、仅支持 Win11 现代右键菜单。
- **桌面时钟** 1.2.0（新可选 Mod，默认关）：在桌面图标后方显示花样时钟与日期，17 种预设（倾斜阴影 / 极简 / 霓虹 / 翻页 / 大字封面 / 竖排 / 描边空心 / 玻璃卡片 / 复古数码管 / 彩虹渐变 / 日落渐变 / 中文大写 / 文字时钟 / 圆形表盘 / 进度环 / 左侧细线 / 双色错位）+ 自定义；可选 CPU/内存占用（2 秒刷新）与月历（今天高亮、内置农历）；设置精简为 13 项 + 折叠的「高级」；支持旋转、模糊投影、光晕、描边、多显示器与 DPI；框架改编自 m417z Desktop Live Overlay (GPL-3.0)。
- **窗口动画** 1.6.3：移除「零雾开窗」；1.6.1/1.6.2 性能与编译修复保留；1.5.3 开机预热 Direct2D 保留。

## 新增
- **文件管理器美化**（`zen-explorer-styler` 1.6.6）：替代 ExplorerBlurMica，色调、不透明度、文字颜色都能在 Windhawk 里调，适配浅色模式；右键菜单玻璃效果（测试中）。
- **任务栏音乐与歌词**（0.4.7）：占用小组件位置，显示封面、滚动歌词和播放控制，支持 Spotify、网易云音乐、QQ 音乐等播放器；歌词默认偏移 +200 毫秒，可在设置里调。
- **AI 工作状态**：歌词面板可显示本机 AI 编程助手（Claude Code、Codex、opencode、Antigravity、DeepSeek Harness 等）的状态。运行 `tools/ai-status/install.ps1` 即可配置，已附带编译好的 `zen-status.exe`。
- **可选 Mod**：果冻窗口（0.115.4）、窗口动画（1.5.2，含 macOS 神奇效果和多种新的打开/关闭样式）。和其他 mod 一起装进 Windhawk，**默认关闭**，想用时在 Windhawk 里打开。
- **一键卸载**：`uninstall.bat`，删除前会列出内容让你确认，不卸载 Windhawk 本身。

## 改进
- **任务栏美化** 4.2.7：重写最大化层逻辑；设置按区分组，新增「统一设置」（默认打开）；去掉玻璃描边，圆角与系统一致。
- **开始菜单美化** 4.1.10：修复浅色模式白雾和液态玻璃下应用名称不显示；修复从任务栏打开的搜索页不透明（搜索页打开时自动重新挂接，外框保持系统圆角）。
- **通知栏美化** 4.2.2：修复横幅通知（Toast 弹窗）材质未统一的问题，全面适配所有内置主题与浅色高对比度；浅色模式加白底、文字加深，新增「对比度增强」选项。
- **设置美化** 4.2.3：失焦时保持透明，新增浅色模式白底选项（默认 20%）。
- 液态玻璃样式全部去掉额外描边。
- 部署脚本：中英双语提示，没装 Windhawk 时自动打开包内安装器，运行完窗口不会一闪就关；只补缺失的默认值，不覆盖你已有的设置和开关。
- 新增 GPL-3.0 许可证，反馈地址统一改到本仓库 Issues。

## 下载哪个
| 文件 | 适合 |
|------|------|
| `ZenDesktop_OneKeyDeploy_v5.0.0.zip` | 大多数人，内含 Windhawk 联网安装器 |
| `ZenDesktop_OneKeyDeploy_v5.0.0_Offline.zip` | 网络差，内含 Windhawk 离线安装器 |
| `ZenDesktop_OneKeyDeploy_v5.0.0_Online.zip` | 已经装好 Windhawk，只含 Mod 和脚本 |

## 安装
1. 解压后双击 `deploy.bat`，在权限提示里点「是」。
2. 打开 Windhawk，新装的 Mod（包括两个可选 Mod）默认关闭，按需开启。
3. 全部调好后重启一次电脑。

## 从 v4.1 升级
先双击 `tools/uninstall-ExplorerBlurMica.bat` 卸载旧版 ExplorerBlurMica，再运行 `deploy.bat`。原有设置会保留。

## 卸载
双击 `uninstall.bat`。

## 支持作者
[爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)


### 桌面时钟 1.2.1
- 加诊断日志：宿主窗口类名/父窗口/位置、各显示器工作区、绘制中心、交换链尺寸、EndDraw/Present 返回值、Z 序兄弟窗口
- 偏移量最多只能把时钟中心推到显示器边缘内 20px，不会再整块推出屏幕
- 「桌面不显示」排查中（真机未验证）

### 桌面时钟 1.2.2
- 修复：装了 Wallpaper Engine 时时钟被壁纸盖住、桌面不显示。原因是 WE 的 WPEDesktopDX11Window 与时钟在同一个 WorkerW 下且排在上面；现在创建时把时钟提到最上层，有其他同级窗口时每 2 秒检查一次
- Z 序诊断日志只在创建时输出一次
