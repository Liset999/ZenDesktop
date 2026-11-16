# ZenDesktop v5.2.6

> **作者：Lanbo**\
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

Windows 11 桌面美化套件，通过 [Windhawk](https://windhawk.net/) 统一任务栏、开始菜单、通知栏、设置和文件管理器的玻璃风格。本版相对 v5.2.5 拆出独立的任务栏高度与图标大小 Mod，调整 Windhawk 2.0 设置兼容性，并修复控制面板顶部白条与部署退出超时处理。再次部署会保留已有配置与 Mod 开关状态。

## 下载哪个

| **文件** | **适合谁** | **里面有什么** | **大概大小** |
| --- | --- | --- | --- |
| `ZenDesktop_OneKeyDeploy_v5.2.6.zip` | 可以使用联网安装器 | 全部 Mod、脚本和 Windhawk 联网安装器 | 约 13 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.6_Offline.zip` | 网络较差，需要离线安装 | 全部 Mod、脚本和 Windhawk 离线安装器 | 约 144 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.6_Online.zip` | 已安装 Windhawk | Mod 和脚本，不含 Windhawk 安装器 | 约 2.3 MB |

还没有安装 Windhawk 的用户选择标准包或 Offline 包，先安装 Windhawk。已经安装的用户选择 Online 包，直接运行 `deploy.bat`。

## 其他文件

- `uninstall-ExplorerBlurMica.bat` 用于从 v4.1 升级时清理旧版 ExplorerBlurMica，位于安装包的 `tools` 目录。
- `zen-status.exe` 用于任务栏歌词面板联动 AI 助手状态，完整接入步骤见安装包内 `tools/ai-status/README.md`。

## 安装与升级

1. 完整解压下载的压缩包，阅读 `AAA使用前必看.txt`。
2. 未安装 Windhawk 的用户先安装 Windhawk。已安装的用户直接双击 `deploy.bat`，按提示授予管理员权限。
3. 打开 Windhawk →「已安装的 Mod」，按需启用 Mod，提示编译时完成编译。首次部署的 Mod 默认关闭。新增或源码变化的 Mod 需要手动保存并编译，源码相同且已有编译结果的 Mod 可继续使用原 DLL。
4. 建议在 Windows「设置 → 个性化 → 颜色」中选择深色模式。完成所需模块的编译和启用后，请保存工作并重启电脑，使模块随应用启动生效。
5. 从 v5.2.5 升级可直接部署，已有设置与开关状态会保留，全新安装采用作者默认参数。从 v4.1 升级请先运行 `tools/uninstall-ExplorerBlurMica.bat`。
6. 任务栏高度、图标大小和按钮宽度现在在独立的「任务栏高度与图标大小」Mod 中设置。首次默认高度 52、图标大小 32、按钮宽度 44，升级后请核对所需尺寸。社区原版与此 Mod 选择一个启用。

## 卸载

运行压缩包内的 `uninstall.bat`，按提示操作。卸载范围为 ZenDesktop 部署的组件。

## 本版亮点

### 改进与修复

- **任务栏高度拆为独立 Mod**。新增「任务栏高度与图标大小」1.3.11，采用 m417z 社区实现并完成中文设置翻译。
- **调整 Windhawk 2.0 设置兼容性**。设置美化、输入法提示、通知栏、开始菜单、任务栏和歌词的整数预设改用数字输入，修正数字与布尔选项键，使用兼容标记保留 Windhawk 1.7.3 的整数配置读取方式。
- **修复控制面板顶部白条**。文件管理器美化按系统位图内容匹配并清除玻璃背景上的顶部残留。
- **改进任务栏启动恢复**。XAML/TAP 初始化失败时取消重启 Explorer 提示，快速重试后进入低频恢复，卸载时停止恢复。
- **修复部署退出超时处理**。Windhawk 退出返回 `0x800705B4` 时检查服务状态，确认服务已停止后继续部署。
- **补齐 Mod 简介更新日志**。每个变更版本增加对应日志，打包入口增加自动检查。

| Mod | 5.2.5 | 5.2.6 |
| --- | --- | --- |
| 设置美化 | 4.2.6 | 4.2.7 |
| 文件管理器美化 | 1.7.10 | 1.7.11 |
| 光标输入法提示 | 0.2.7 | 0.2.8 |
| 通知栏美化 | 4.2.3 | 4.2.4 |
| 开始菜单美化 | 4.1.11 | 4.1.13 |
| 任务栏美化 | 4.4.4 | 4.4.6 |
| 任务栏音乐与歌词 | 0.5.21 | 0.5.23 |

发布检查覆盖 Mod 元数据、简介版本日志、PowerShell 5.1 语法、部署生命周期规则、任务栏重试回归和控制面板位图绘制回归。Windhawk 2.0 远程编译及整机冷启动效果仍需对应环境实测。

## 支持作者

如果 ZenDesktop 对你有帮助，可以通过 [爱发电](https://ifdian.net/a/Lanboo) 支持作者 Lanbo。

![微信赞赏码](https://cdn.jsdelivr.net/gh/Liset999/ZenDesktop@main/docs/donate-wechat.png)
