# ZenDesktop 项目开发与部署规范

## 核心设计铁律（MUST ALWAYS FOLLOW）

1. **Deploy 职责边界**：
   - `deploy.bat` / `deploy.ps1` **只负责把 Mod 源码和元数据装到 Windhawk 里面**。
   - **绝对不负责编译，绝对不负责开启！**
   - 所有部署的 Mod（核心 Mod 与可选 Mod）在初次部署时，默认状态必须全部为 **关闭状态（Disabled = 1）**。
   - 严禁通过脚本在后台强行开启任何 Mod（除非命令行参数显式指定）。
   - 严禁通过脚本触发自动编译或后台强行编译。
   - 必须将选择权、开启权、编译权完全交由用户在 Windhawk UI 界面中按需自行开启与确认编译。
   - 若用户此前已在 Windhawk 中手动调整过开关状态，重新部署时必须保留用户的现有状态。

2. **全中文本地化规范**：
   - 所有引入或更新的 Mod 必须包含完整的中文本地化元数据：
     - `// @name:zh-CN`
     - `// @description:zh-CN`
     - 完整的 `// ==WindhawkModSettings==` 中文翻译（`$name:zh-CN`, `$description:zh-CN`）。
   - 禁止在 UI 或控制台中向普通用户展示未翻译的英文设置项。

3. **第三方组件独立性（如 ExplorerBlurMica）**：
   - `deploy.ps1` 不得强行覆盖或静默篡改用户已有的文件管理器透明组件（ExplorerBlurMica）的系统注册与配置。
   - 注意 XAML 诊断接口：多个工具（Windhawk 的 XAML Styler 类 Mod、ExplorerBlurMica 的 `clearWinUIBg` 等）都会通过 XAML 诊断接口修改 WinUI 层，同时使用时可能互相干扰，Mod 里有对应的冲突处理设置。文件管理器命令栏黑杠的成因尚未确认，不要把它简单归因于此。
