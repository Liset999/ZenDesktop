# 社区音频可视化替代核对

核对日期：2026-11-05。用户要求停止从零实现，优先使用 Windhawk 社区已有 Mod。

| 候选 | 官方集合源码核对版本 | 适用范围 |
| --- | --- | --- |
| [Tourne'Table](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/tourne-table-desktop-audio-visualizer.wh.cpp) | 2.1.1，MIT | 桌面、任务栏、独立面板；推荐优先验证 |
| [Desktop Audio Visualizer Plus](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/neiz-supersmile-audio-visualizer.wh.cpp) | 1.2.0，MIT | 桌面可视化，多屏、自动增益、平滑及可选媒体/歌词功能 |
| [Taskbar Fluent Media Player](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-fluent-media-player.wh.cpp) | 1.6.0 | 任务栏播放器，内置可启用的频谱 |

Tourne'Table 的官方源码 README 说明以独立 tool-mod 进程运行、需要 Windhawk 1.7.3+，提供 Position → Place In = Desktop / Taskbar / Panel，以及多种分析/绘制配置。本机 `E:\Tools\Windhawk\windhawk.exe` 产品与文件版本均实测为 1.7.3，满足其声明的版本门槛；这不代表本机运行兼容性已经验证。

已将三份上游源码及 README 下载到 `.diagnostics/community-audio-review/` 仅供核对；没有安装、编译、启用或替换现有音频模块。

建议后续路径：保留时钟、日历和资源小组件，用社区原版独立验证音频显示。优先 Tourne'Table；确认在本机桌面/任务栏真实可见后，才进行翻译、预设和发布集成。避免再次将模拟音频或单元测试当作最终显示验证。

## 后续进展

用户已手动安装并启用 Tourne’Table 2.1.1，确认原版独立面板和任务栏可见并随音乐跳动。随后明确要求整合进单一小组件 Mod。3.0 整合源码与开发/升级说明见 [desktop-widgets-3.0.md](desktop-widgets-3.0.md)；整合版仍需手动加载验收。
