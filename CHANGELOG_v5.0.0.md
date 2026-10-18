# ZenDesktop v5.0.0

> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

相对 v4.1 的主要更新。

## 新增
- **文件管理器美化**（`zen-explorer-styler` 1.6.5）：替代 ExplorerBlurMica，色调、不透明度、文字颜色都能在 Windhawk 里调，适配浅色模式；右键菜单玻璃效果（测试中）。
- **任务栏音乐与歌词**（0.4.6）：占用小组件位置，显示封面、滚动歌词和播放控制，支持 Spotify 等接入系统媒体控制的播放器；歌词默认偏移 +200 毫秒，可在设置里调。
- **AI 工作状态**：歌词面板可显示本机 AI 编程助手（Claude Code、Codex、opencode、Antigravity、DeepSeek Harness 等）的状态。运行 `tools/ai-status/install.ps1` 即可配置，已附带编译好的 `zen-status.exe`。
- **可选 Mod**：果冻窗口（0.115.4）、窗口动画（1.5.2，含 macOS 神奇效果和多种新的打开/关闭样式）。部署时可选择安装，**默认关闭**。
- **一键卸载**：`uninstall.bat`，删除前会列出内容让你确认，不卸载 Windhawk 本身。

## 改进
- **任务栏美化** 4.2.7：重写最大化层逻辑；设置按区分组，新增「统一设置」（默认打开）；去掉玻璃描边，圆角与系统一致。
- **开始菜单美化** 4.1.9：修复浅色模式白雾和液态玻璃下应用名称不显示。
- **通知栏美化** 4.2.1：浅色模式加白底、文字加深，新增「对比度增强」选项。
- **设置美化** 4.2.2：失焦时保持透明，新增浅色模式白底选项。
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
2. 按提示选择是否安装可选 Mod。
3. 打开 Windhawk，新装的 Mod 默认关闭，按需开启。
4. 重启电脑。

## 从 v4.1 升级
先双击 `tools/uninstall-ExplorerBlurMica.bat` 卸载旧版 ExplorerBlurMica，再运行 `deploy.bat`。原有设置会保留。

## 卸载
双击 `uninstall.bat`。

## 支持作者
[爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)
