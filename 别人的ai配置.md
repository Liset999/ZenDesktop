# 别人的 AI 配置

这份文件用于把其他 AI、脚本或 OpenAI 兼容客户端接到 AinaibaHub 中转站，并调用 `gpt-image-2` 生成图片。

## 核心配置

| 配置项 | 值 |
| --- | --- |
| Base URL | `https://api-xai.ainaibahub.com/v1` |
| API Key 环境变量 | `XAI_API_KEY` |
| 文本/调度模型 | `gpt-5.5` |
| 图片模型 | `gpt-image-2` |
| 推荐图片接口 | `/v1/images/generations` |
| Responses 图片工具 | `image_generation` |

不要把 API Key 写进前端代码、公开仓库、聊天记录或 Markdown 文件。让程序从环境变量读取：

```powershell
[Environment]::SetEnvironmentVariable("XAI_API_KEY", "你的API_KEY", "User")
```

设置后重启终端或应用，让环境变量生效。

## 给其他 AI 的最短配置

如果对方支持 OpenAI 兼容接口，按下面配置：

```yaml
base_url: https://api-xai.ainaibahub.com/v1
api_key_env: XAI_API_KEY
chat_model: gpt-5.5
image_model: gpt-image-2
image_endpoint: /v1/images/generations
```

生成图片时优先使用 Images API：

```json
{
  "model": "gpt-image-2",
  "prompt": "Create a cinematic film still: a lone person holding a black umbrella in a rain-soaked neon city street at night, wet asphalt reflections, dramatic backlight, shallow depth of field. No words, no logos, no watermark.",
  "size": "1024x1024",
  "quality": "high",
  "output_format": "png",
  "response_format": "b64_json"
}
```

## Images API 示例

请求地址：

```text
POST https://api-xai.ainaibahub.com/v1/images/generations
```

请求体：

```json
{
  "model": "gpt-image-2",
  "prompt": "Create a cinematic film still: a rain-soaked city street at night, wet asphalt reflections, warm streetlight and teal neon contrast, realistic movie frame composition. No words, no logos, no watermark.",
  "size": "1024x1024",
  "quality": "high",
  "output_format": "png",
  "response_format": "b64_json"
}
```

典型返回中，图片在：

```text
data[0].b64_json
```

客户端需要把这个 base64 解码成 PNG 文件。

## PowerShell 保存图片示例

```powershell
$ErrorActionPreference = "Stop"

$key = $env:XAI_API_KEY
if ([string]::IsNullOrWhiteSpace($key)) {
  throw "XAI_API_KEY is missing"
}

$body = @{
  model = "gpt-image-2"
  prompt = "Create a cinematic film still: a lone person holding a black umbrella in a rain-soaked neon city street at night, wet asphalt reflections, dramatic backlight, shallow depth of field, anamorphic lens flare. No words, no logos, no watermark."
  size = "1024x1024"
  quality = "high"
  output_format = "png"
  response_format = "b64_json"
}

$response = Invoke-RestMethod `
  -Uri "https://api-xai.ainaibahub.com/v1/images/generations" `
  -Method Post `
  -Headers @{ Authorization = "Bearer $key" } `
  -ContentType "application/json" `
  -Body ($body | ConvertTo-Json -Depth 8) `
  -TimeoutSec 240

$b64 = $response.data[0].b64_json
if ([string]::IsNullOrWhiteSpace($b64)) {
  throw "No image data returned"
}

[IO.File]::WriteAllBytes("gpt-image-2-output.png", [Convert]::FromBase64String($b64))
```

## Responses API 示例

如果希望 `gpt-5.5` 先理解需求，再通过图片工具调用 `gpt-image-2`，使用 Responses API。

注意：这里顶层 `model` 是 `gpt-5.5`，图片模型放在 `tools` 数组里。

```json
{
  "model": "gpt-5.5",
  "input": "Create a cinematic rain-night city image. No words, no logos, no watermark.",
  "tools": [
    {
      "type": "image_generation",
      "model": "gpt-image-2",
      "size": "1024x1024",
      "quality": "high",
      "output_format": "png"
    }
  ],
  "stream": true
}
```

请求地址：

```text
POST https://api-xai.ainaibahub.com/v1/responses
```

流式返回时，最终图片通常出现在 `response.output_item.done` 事件中，类型为 `image_generation_call`，结果字段是 base64 图片。

## 推荐提示词规范

图片提示词建议明确写：

- 画面主体
- 场景和时间
- 光线和镜头语言
- 风格或用途
- 不要文字、不要 logo、不要水印

示例：

```text
Create a cinematic film still: a lone person holding a black umbrella in a rain-soaked neon city street at night, wet asphalt reflections, dramatic backlight, shallow depth of field, anamorphic lens flare, moody teal and warm amber color contrast, realistic texture, high-end movie frame composition. No words, no logos, no watermark.
```

## 常用尺寸

| 用途 | 尺寸 |
| --- | --- |
| 方图测试 | `1024x1024` |
| 横向封面 | `1792x1024` 或 `1920x1080` |
| 竖向海报 | `1024x1792` |
| 4K 横图 | `3840x2160` |
| 4K 竖图 | `2160x3840` |

不要使用 `4096x4096`。如果接口限制最大边长，优先控制在 `3840` 以内。

## 已验证结果

2026-06-04，本机已用以下配置生成 PNG 成功：

- Endpoint: `https://api-xai.ainaibahub.com/v1/images/generations`
- Model: `gpt-image-2`
- Size: `1024x1024`
- Quality: `high`
- Output format: `png`
- Response format: `b64_json`
