## 任务栏状态汇报（zen-status）
本机任务栏会显示你的工作状态。请用 zen-status 这个 MCP 服务的 set_status 工具汇报：
- 开始处理一个新任务时：state 设为 "working"，message 用一句不超过 20 个字的中文说明在做什么，例如「重写任务栏 mod 的最大化层」。
- 需要我回答问题、确认或批准时：state 设为 "waiting"，message 写清楚在等什么。
- 任务完成时：state 设为 "done"，message 用一句话总结结果。
- 遇到无法继续的错误时：state 设为 "error"，message 写原因。
只在状态变化时调用，不要每一步都调用。message 里不要写密码、令牌、手机号等隐私信息。
