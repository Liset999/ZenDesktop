> 历史记录：本页描述 2.0 的自制音频实验与模拟预览；3.0 已移除该引擎，改为整合 Tourne’Table。当前说明见 [桌面小组件 3.0](desktop-widgets-3.0.md)。

# 音频可视化参考与本地实现

2026-11-05：根据用户要求研究 GitHub 开源项目，调整任务栏频谱外观与响应。

- [vis_taskbar](https://github.com/vakuras/vis_taskbar)：阅读 `src/spectrum.rs`、`src/renderer.rs` 与 `src/audio.rs`。参考频谱聚合、频率补偿、渐变以及独立的回落与峰值效果。其实现使用 OpenGL/Direct2D 和独立窗口，本项目保持 Windhawk 内的透明分层子窗口。
- [Monstercat Visualizer](https://github.com/marcopixel/monstercat-visualizer)：阅读 `visualizer.ini` 中的 FFT overlap、attack/decay 和平滑 band 配置。参考将上升响应与回落速度分离的设计。
- [TaskbarTunes](https://github.com/ClaudioPenta/TaskbarTunes)：参考 README 的对数频带与自动增益架构说明；未直接移植其 NAudio/WPF 实现。

本地代码独立实现，不引入上述项目的二进制、资源或新运行时依赖。自动增益有噪声门槛和最小归一化参考值；按时间计算攻击/衰减。镜像模式将完整频段映射到左右两半，不再丢弃高频。

任务栏采用窄圆角柱、竖向轻渐变、透出底色的透明度与上下留白。桌面支持所选屏幕的可用区域；选择全部屏幕时，每块屏幕分别计算音频条位置。

`audio-spectrum-preview.gif` / `.png` 是使用实际 DSP 与绘制函数输出的**模拟音频预览**，不是用户桌面截图。

验证：

```powershell
python tools/tests/check_widgets_audio.py --compiler-dir 'E:\Tools\Windhawk\Compiler'
python tools/tests/check_widgets_layout.py --compiler-dir 'E:\Tools\Windhawk\Compiler'
```

布局测试嵌入 Windows 8/10 兼容清单，创建真实 `WS_EX_LAYERED | WS_CHILD` 窗口，在非零父窗口坐标下验证 60 帧位置不漂移、实际 presentation 成功及尺寸变化跟随。几何测试覆盖主屏、错位双屏、负坐标和顶部位置。测试成功不等于 Explorer/Wallpaper Engine 中的最终视觉效果已经确认。
