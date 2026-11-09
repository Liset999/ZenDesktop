# ZenDesktop v5.2.1

> **作者：Lanbo**  
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

Windows 11 桌面美化套件，通过 [Windhawk](https://windhawk.net/) 统一任务栏、开始菜单、通知栏、设置和文件管理器的玻璃风格。本版相对 v5.2.0，重点修复光标输入法提示导致的 Steam 卡顿，改进输入框定位、查询隔离和重复部署流程。再次部署会保留已有配置与 Mod 开关状态。

## 下载哪个

| **文件** | **适合谁** | **里面有什么** | **大概大小** |
| --- | --- | --- | --- |
| `ZenDesktop_OneKeyDeploy_v5.2.1.zip` | 可以使用联网安装器 | 全部 Mod、脚本和 Windhawk 联网安装器 | 约 13 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.1_Offline.zip` | 网络较差，需要离线安装 | 全部 Mod、脚本和 Windhawk 离线安装器 | 约 144 MB |
| `ZenDesktop_OneKeyDeploy_v5.2.1_Online.zip` | 已安装 Windhawk | Mod 和脚本，不含 Windhawk 安装器 | 约 2.3 MB |

还没有安装 Windhawk：选择标准包或 Offline 包，先安装 Windhawk；已经安装：选择 Online 包，直接运行 `deploy.bat`。

## 其他文件

- `uninstall-ExplorerBlurMica.bat`：从 v4.1 升级时，用于清理旧版 ExplorerBlurMica。
- `zen-status.exe`：用于任务栏歌词面板联动 AI 助手状态，完整接入步骤见安装包内 `tools/ai-status/README.md`。

## 安装与升级

1. 完整解压下载的压缩包，阅读 `AAA使用前必看.txt`。
2. 未安装 Windhawk 的用户先安装 Windhawk；已安装的用户直接双击 `deploy.bat`，按提示授予管理员权限。
3. 打开 Windhawk →「已安装的 Mod」，按需启用 Mod；提示编译时完成编译。首次部署的 Mod 默认关闭。
4. 建议在 Windows「设置 → 个性化 → 颜色」中选择深色模式。完成所需模块的编译和启用后，请保存工作并重启电脑，使模块随应用启动生效。
5. 从 v5.2.0 或 v5.1.0 升级可直接部署；升级后请确认「光标输入法提示」版本为 0.2.7 并保存编译；从 v4.1 升级请先运行 `tools/uninstall-ExplorerBlurMica.bat`。

## 卸载

运行压缩包内的 `uninstall.bat`，按提示操作。Windhawk 本身和其他第三方 Mod 不会被卸载。

## 本版亮点

### 改进与修复

- **光标输入法提示 0.2.7**。修复 Steam 卡顿，Steam 客户端与 steamwebhelper 窗口暂停提示并跳过输入法、无障碍查询，已获用户实机恢复确认。
- **查询隔离与输入响应**。保留点击或聚焦输入框的自动提醒，MSAA/UIA 查询改在独立进程执行，超过 600 毫秒回收；键鼠钩子使用独立线程，失败查询降低重试频率。
- **光标定位与重载**。切换输入框或发生新输入时丢弃旧结果，修复输入框左边缘误定位和卸载等待问题。确认输入框但无法取得准确光标时，气泡跟随鼠标。修正 UTF-8 BOM 导致 Windhawk 无法识别元数据的问题。
- **部署流程**。等待 Windhawk 正常退出和引擎卸载，退出失败或超时会停止部署；源码未变化时保留已编译 DLL 注册信息，源码或共享头文件变化后仍需编译；检查服务启动结果。
- **窗口动画生效说明**。部分已运行应用存在热注入限制，完成编译、启用后请重启电脑。游戏、反作弊及明确排除的程序继续跳过。本版沿用官方 Windhawk，已有进程热注入限制仍存在。
- **搜索界面恢复说明**。Windhawk 重启后，已有搜索进程可能未加载美化。部署脚本会重启搜索宿主；仍遇到搜索页样式失效时，可重启电脑恢复。本轮已确认重启搜索宿主后恢复。

光标生命周期、Steam 匹配、动画截图协作、正式 Mod 元数据与源码语法回归通过。Steam 恢复流畅已获用户确认，完整部署、重启后效果及其他输入交互仍需在各设备验证。

文件中转只暂存源文件路径，拖出为复制；清空或过期不会删除源文件，资源管理器或 Mod 重载后列表会清空。

## 支持作者

如果 ZenDesktop 对你有帮助，可以通过 [爱发电](https://ifdian.net/a/Lanboo) 支持作者 Lanbo。

![微信赞赏码](https://cdn.jsdelivr.net/gh/Liset999/ZenDesktop@main/docs/donate-wechat.png)

