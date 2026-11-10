# ZenDesktop v5.2.2

> **作者：Lanbo**  
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

Windows 11 桌面美化套件，通过 [Windhawk](https://windhawk.net/) 统一任务栏、开始菜单、通知栏、设置和文件管理器的玻璃风格。本版修复桌面小组件在部署后只登记为 32 位、导致 64 位资源管理器无法加载时钟、日历和资源占用的问题。

## 下载哪个

| **文件** | **适合谁** | **里面有什么** | **大概大小** |
| --- | --- | --- | --- |
| `ZenDesktop_OneKeyDeploy_v5.2.2.zip` | 可以使用联网安装器 | 全部 Mod、脚本和 Windhawk 联网安装器 | 约 13 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.2_Offline.zip` | 网络较差，需要离线安装 | 全部 Mod、脚本和 Windhawk 离线安装器 | 约 144 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.2_Online.zip` | 已安装 Windhawk | Mod 和脚本，不含 Windhawk 安装器 | 约 2.3 MB |

还没有安装 Windhawk：选择标准包或 Offline 包，先安装 Windhawk；已经安装：选择 Online 包，直接运行 `deploy.bat`。

## 其他文件

- `uninstall-ExplorerBlurMica.bat`：从 v4.1 升级时，用于清理旧版 ExplorerBlurMica。
- `zen-status.exe`：用于任务栏歌词面板联动 AI 助手状态，完整接入步骤见安装包内 `tools/ai-status/README.md`。

## 安装与升级

1. 完整解压下载的压缩包，阅读 `AAA使用前必看.txt`。
2. 未安装 Windhawk 的用户先安装 Windhawk；已安装的用户直接双击 `deploy.bat`，按提示授予管理员权限。
3. 打开 Windhawk →「已安装的 Mod」，按需启用 Mod；提示编译时完成编译。首次部署的 Mod 默认关闭。
4. 建议在 Windows「设置 → 个性化 → 颜色」中选择深色模式。完成所需模块的编译和启用后，请保存工作并重启电脑。
5. 从 v5.2.1 升级可直接部署。部署后确认「桌面小组件」架构为 `x86|x86-64`，然后重启资源管理器或电脑。

## 卸载

运行压缩包内的 `uninstall.bat`，按提示操作。Windhawk 本身和其他第三方 Mod 不会被卸载。

## 本版亮点

### 修复

- **桌面小组件恢复显示**。部署脚本现在保留 `x86` 与 `x86-64` 两个架构声明，64 位资源管理器可以加载已编译的小组件。
- **设置不丢失**。重复部署补齐缺失的小组件默认值，不覆盖已有时钟、日历、资源和音频设置。
- **无需重新编译**。本版不修改 Mod 源码，已编译且源码未变化的 DLL 会保留。

用户已通过重新部署验证桌面小组件恢复。其他设备仍建议在部署后确认架构并重启资源管理器。

## 支持作者

如果 ZenDesktop 对你有帮助，可以通过 [爱发电](https://ifdian.net/a/Lanboo) 支持作者 Lanbo。

![微信赞赏码](https://cdn.jsdelivr.net/gh/Liset999/ZenDesktop@main/docs/donate-wechat.png)
