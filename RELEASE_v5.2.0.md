# ZenDesktop v5.2.0

> **作者：Lanbo**  
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

Windows 11 桌面美化套件，通过 [Windhawk](https://windhawk.net/) 统一任务栏、开始菜单、通知栏、设置和文件管理器的玻璃风格。本版相对 v5.1.0，新增输入法提示、音乐展开面板和桌面音频可视化，并改进窗口动画与文件管理器美化。再次部署会保留已有配置与 Mod 开关状态。

## 下载哪个

| **文件** | **适合谁** | **里面有什么** | **大概大小** |
| --- | --- | --- | --- |
| `ZenDesktop_OneKeyDeploy_v5.2.0.zip` | 可以使用联网安装器 | 全部 Mod、脚本和 Windhawk 联网安装器 | 约 13 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.0_Offline.zip` | 网络较差，需要离线安装 | 全部 Mod、脚本和 Windhawk 离线安装器 | 约 144 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.0_Online.zip` | 已安装 Windhawk | Mod 和脚本，不含 Windhawk 安装器 | 约 2.2 MB |

还没有安装 Windhawk：选择标准包或 Offline 包，先安装 Windhawk；已经安装：选择 Online 包，直接运行 `deploy.bat`。

## 其他文件

- `uninstall-ExplorerBlurMica.bat`：从 v4.1 升级时，用于清理旧版 ExplorerBlurMica。
- `zen-status.exe`：用于任务栏歌词面板联动 AI 助手状态，完整接入步骤见安装包内 `tools/ai-status/README.md`。

## 安装与升级

1. 完整解压下载的压缩包，阅读 `AAA使用前必看.txt`。
2. 未安装 Windhawk 的用户先安装 Windhawk；已安装的用户直接双击 `deploy.bat`，按提示授予管理员权限。
3. 打开 Windhawk →「已安装的 Mod」，按需启用 Mod；提示编译时完成编译。首次部署的 Mod 默认关闭。
4. 建议在 Windows「设置 → 个性化 → 颜色」中选择深色模式。全部调好后建议重启电脑。
5. 从 v5.1.0 升级可直接部署；从 v4.1 升级请先运行 `tools/uninstall-ExplorerBlurMica.bat`。

## 卸载

运行压缩包内的 `uninstall.bat`，按提示操作。Windhawk 本身和其他第三方 Mod 不会被卸载。

## 本版亮点

### 新增

- **光标输入法提示 0.2.5**：切换中英文或大写状态时显示提示气泡，支持自定义玻璃外观。
- **任务栏音乐与歌词 0.5.18**：点击歌词条展开播放控制、AI 状态、天气、资源占用和文件中转面板；支持顶部、侧边任务栏及多显示器。网易云音乐识别不到时，请在网易云设置中开启 SMTC。
- **桌面小组件 3.0.5**：时钟、日历和资源占用之外，整合音频可视化，音频条可放在任务栏或桌面。此前单独安装过 Tourne’Table 的用户请先关闭它。

### 改进与修复

- **任务栏美化 4.4.4**：改进资源管理器重启后的样式恢复，完善竖向任务栏和背景设置。
- **文件管理器美化 1.7.10**：改进透明背景与文字颜色，修复整窗玻璃下文字可能消失或发白的问题。
- **窗口动画 1.6.17**：针对文件管理器和「设置」动画中背景变纯色的问题，使用合成截图或已有快照；截图不可用时回退到系统原生过渡。玻璃材质效果仍需在实际设备上验证。
- **果冻窗口 0.115.9**：改进不同系统版本上的初始化。开启后需重启电脑。

文件中转只暂存源文件路径，拖出为复制；清空或过期不会删除源文件，资源管理器或 Mod 重载后列表会清空。

## 支持作者

如果 ZenDesktop 对你有帮助，可以通过 [爱发电](https://ifdian.net/a/Lanboo) 支持作者 Lanbo。

![微信赞赏码](https://cdn.jsdelivr.net/gh/Liset999/ZenDesktop@main/docs/donate-wechat.png)
