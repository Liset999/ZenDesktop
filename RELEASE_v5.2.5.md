# ZenDesktop v5.2.5

> **作者：Lanbo**  
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

Windows 11 桌面美化套件，通过 [Windhawk](https://windhawk.net/) 统一任务栏、开始菜单、通知栏、设置和文件管理器的玻璃风格。本版相对 v5.2.1 有两项变化：修复部署后桌面小组件不显示的问题，并为全新安装加入作者当前使用的 Mod 默认设置。再次部署会保留已有配置与 Mod 开关状态。

## 下载哪个

| **文件** | **适合谁** | **里面有什么** | **大概大小** |
| --- | --- | --- | --- |
| `ZenDesktop_OneKeyDeploy_v5.2.5.zip` | 可以使用联网安装器 | 全部 Mod、脚本和 Windhawk 联网安装器 | 约 13 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.5_Offline.zip` | 网络较差，需要离线安装 | 全部 Mod、脚本和 Windhawk 离线安装器 | 约 144 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.5_Online.zip` | 已安装 Windhawk | Mod 和脚本，不含 Windhawk 安装器 | 约 2.3 MB |

还没有安装 Windhawk：选择标准包或 Offline 包，先安装 Windhawk；已经安装：选择 Online 包，直接运行 `deploy.bat`。

## 其他文件

- `uninstall-ExplorerBlurMica.bat`：从 v4.1 升级时，用于清理旧版 ExplorerBlurMica。
- `zen-status.exe`：用于任务栏歌词面板联动 AI 助手状态，完整接入步骤见安装包内 `tools/ai-status/README.md`。

## 安装与升级

1. 完整解压下载的压缩包，阅读 `AAA使用前必看.txt`。
2. 未安装 Windhawk 的用户先安装 Windhawk；已安装的用户直接双击 `deploy.bat`，按提示授予管理员权限。
3. 打开 Windhawk →「已安装的 Mod」，按需启用 Mod；提示编译时完成编译。首次部署的 Mod 默认关闭。
4. 建议在 Windows「设置 → 个性化 → 颜色」中选择深色模式。完成所需模块的编译和启用后，请保存工作并重启电脑，使模块随应用启动生效。
5. 从 v5.2.1 或 v5.2.0 升级可直接部署。已有设置不会被覆盖；全新安装才会使用本版新增的默认设置。从 v4.1 升级请先运行 `tools/uninstall-ExplorerBlurMica.bat`。

## 卸载

运行压缩包内的 `uninstall.bat`，按提示操作。Windhawk 本身和其他第三方 Mod 不会被卸载。

## 本版亮点

### 改进与修复

- **修复桌面小组件消失**。部署脚本此前只读取第一行架构声明，把同时支持 32 位和 64 位的桌面小组件写成了 `x86`。64 位资源管理器因此无法加载它，时钟、日历、资源占用和音频可视化都不显示。现已收集全部架构声明并正确登记为 `x86|x86-64`，已获用户重新部署后的实机确认。
- **新增作者当前配置作为默认值**。全新安装时，任务栏、歌词、窗口动画、开始菜单、通知栏、文件管理器、果冻窗口和桌面小组件会使用作者当前的参数。桌面小组件默认开启时钟与任务栏音频，日历和资源占用默认关闭。
- **不覆盖已有用户设置**。升级或重复部署只补充缺失的设置项，已经保存的开关、数值和样式全部保留。本版不修改 Mod 源码，已编译且源码未变化的 DLL 也会保留，无需重新编译。

## 支持作者

如果 ZenDesktop 对你有帮助，可以通过 [爱发电](https://ifdian.net/a/Lanboo) 支持作者 Lanbo。

![微信赞赏码](https://cdn.jsdelivr.net/gh/Liset999/ZenDesktop@main/docs/donate-wechat.png)
