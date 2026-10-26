// ==WindhawkMod==
// @id              zen-desktop-widgets
// @name            ZenDesktop: 桌面小组件
// @name:zh-CN      ZenDesktop: 桌面小组件
// @description     Optional macOS-inspired glass desktop widgets: size presets, caption/value hierarchy, soft corners, grid snap. Built-in templates + describe layout. Default off.
// @description:zh-CN 可选 macOS 风格玻璃桌面小组件：小/中/大尺寸、标题/数值层级、大圆角、网格吸附。内置模板 + 描述编排。默认关闭。
// @version         0.2.2
// @donateUrl       https://ifdian.net/a/Lanboo
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -lole32 -ldwmapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

# ZenDesktop: 桌面小组件

在桌面上放置可拖动的玻璃风卡片。**v0.2.2** 布局与排版向 **macOS Desktop / Notification Center Widgets** 靠拢：大圆角、统一内边距、小标题 + 大数值层级、小/中/大尺寸档、可选网格吸附。仍保留 v0.2 的内置模板与「超级背屏」式本地描述编排。不是 Rainmeter；不调用云端大模型。

## 内置模板

| 卡片 | 说明 |
| --- | --- |
| 时钟 | 大号时间 + 日期星期；可选秒、12/24 小时制 |
| 状态 | 自定义短文案 |
| 天气（演示） | 城市名 + 静态演示气温，标注「演示」；真天气可后续接本地 API |
| 日历 | 今日日号 + 星期 + 年月（轻量日程感） |
| 系统一览 | 电池电量（WinAPI）；无电池时显示交流供电 |
| 倒计时 | 目标日期剩余天数 |
| 便签 | 多行备忘风格短文 |

## macOS 风格布局（v0.2.1）

- **尺寸档**：小 / 中 / 大（近似 macOS small / medium / large 比例）
- **层级**：顶部小标题（caption）+ 中部大数值（value）+ 可选脚注
- **字体**：Segoe UI Variable Display / Text（接近 SF Pro 的 Windows 系统替代）
- **圆角**：Win11 `DWMWA_WINDOW_CORNER_PREFERENCE` 大圆角；旧系统用圆角区域兜底
- **玻璃**：Acrylic；跟随系统浅色/深色应用主题微调 tint 与字色
- **吸附**：可选释放时对齐到网格，默认位置按右缘整齐堆叠

## 卡片编排（超级背屏风格 MVP）

- **预设套装**：一键启用常见组合（日常 / 专注 / 全部关闭仅手调）。
- **描述想要的卡片**：填写中文或英文关键词，规则引擎启用对应模板。
- **编排模式**：追加或替换。
- 真 LLM / 本地 API 见 `docs/desktop-widgets.md`；本进程内不做网络推理。

## 其它

- 左键拖动；位置写入 `HKCU\Software\ZenDesktop\DesktopWidgets\`。
- 可选 Mod，部署默认关闭。玻璃依赖系统「透明度效果」。

## 更新日志

- **0.2.2**：空白灰块修复：WM_PAINT 始终校验、字体回退、无内容时占位/隐藏、玻璃下保证绘字；创建后重绑 USERDATA。
- **0.2.1**：macOS 风格布局（尺寸档、caption/value、大圆角、浅/深色、网格吸附）；文档更新。
- **0.2.0**：内置时钟/状态/天气演示/日历/电量/倒计时/便签；12/24h；预设套装 + 描述→模板编排。
- **0.1.1**：性能：未变不重绘、USERDATA、GDI 复用。
- **0.1.0**：MVP：时钟 + 状态卡。

## 致谢与许可

- ZenDesktop 原创 optional Mod，GPL-3.0。
- 开发者：Lanbo（[哔哩哔哩](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo)）
- 「超级背屏」仅为交互灵感来源，与小米无关、无商标授权。macOS Widgets 仅为视觉参考。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- presetPack: "manual"
  $name: "Preset pack"
  $name:zh-CN: "卡片预设套装"
  $description: "Quick layout. Manual keeps the toggles below. AI describe can still refine."
  $description:zh-CN: "一键布局。选「手动」则以下方各卡开关为准；「描述想要的卡片」仍可再编排。"
  $options:
    - manual: "手动（按下方开关）"
    - daily: "日常：时钟 + 天气 + 日历"
    - focus: "专注：时钟 + 倒计时 + 便签"
    - system: "系统：时钟 + 电量 + 状态"
    - all: "全部内置卡"
- aiDescribe: ""
  $name: "Describe the card you want"
  $name:zh-CN: "描述想要的卡片"
  $description: "Keywords map to built-in templates (clock/weather/calendar/battery/countdown/sticky/status). Empty = no AI layout. No cloud LLM in-process."
  $description:zh-CN: "用关键词映射内置模板（时钟/天气/日历/电量/倒计时/便签/状态）。留空不做描述编排。进程内不调用云端大模型。"
- aiLayoutMode: "append"
  $name: "AI layout mode"
  $name:zh-CN: "描述编排模式"
  $description: "How describe text applies when non-empty."
  $description:zh-CN: "描述非空时如何应用到开关。"
  $options:
    - append: "追加：打开匹配项，保留原开关"
    - replace: "替换：只开匹配项"
- widgetSize: "medium"
  $name: "Widget size"
  $name:zh-CN: "卡片尺寸"
  $description: "macOS-inspired small / medium / large presets."
  $description:zh-CN: "参考 macOS 小组件的小 / 中 / 大尺寸档。"
  $options:
    - small: "小（近似 square small）"
    - medium: "中（宽卡，默认）"
    - large: "大（接近正方形大卡）"
- snapToGrid: true
  $name: "Snap to grid when dropping"
  $name:zh-CN: "拖放后对齐网格"
  $description: "Snap position to an 16px grid on mouse up. Default: on."
  $description:zh-CN: "松开鼠标时按 16 像素网格对齐。默认开启。"
- showClock: true
  $name: "Show clock"
  $name:zh-CN: "显示时钟"
  $description: "Clock card. Default: on."
  $description:zh-CN: "时钟卡片。默认开启。"
- showStatus: true
  $name: "Show status card"
  $name:zh-CN: "显示状态卡"
  $description: "Custom status text card. Default: on."
  $description:zh-CN: "自定义状态卡片。默认开启。"
- showWeather: false
  $name: "Show weather (demo)"
  $name:zh-CN: "显示天气（演示）"
  $description: "Demo weather card with city name. Default: off."
  $description:zh-CN: "带城市名的演示天气卡。默认关闭。"
- showCalendar: false
  $name: "Show calendar"
  $name:zh-CN: "显示日历"
  $description: "Today date + weekday card. Default: off."
  $description:zh-CN: "今日日号与星期卡片。默认关闭。"
- showSystem: false
  $name: "Show system glance"
  $name:zh-CN: "显示系统一览"
  $description: "Battery percent via WinAPI. Default: off."
  $description:zh-CN: "WinAPI 电池电量一览。默认关闭。"
- showCountdown: false
  $name: "Show countdown"
  $name:zh-CN: "显示倒计时"
  $description: "Days until target date. Default: off."
  $description:zh-CN: "距目标日期剩余天数。默认关闭。"
- showSticky: false
  $name: "Show sticky note"
  $name:zh-CN: "显示便签"
  $description: "Sticky-note style text card. Default: off."
  $description:zh-CN: "便签风格短文卡片。默认关闭。"
- fontSize: 0
  $name: "Value font size (0 = auto)"
  $name:zh-CN: "主数值字号（0=随尺寸档）"
  $description: "0 follows size preset; otherwise 16–72 overrides the hero value size."
  $description:zh-CN: "0 表示随小/中/大档自动；否则 16–72 覆盖主数值字号。"
- clockShowSeconds: false
  $name: "Clock shows seconds"
  $name:zh-CN: "时钟显示秒"
  $description: "Include seconds. Default: off."
  $description:zh-CN: "时钟是否显示秒。默认关闭。"
- clock12Hour: false
  $name: "Clock 12-hour"
  $name:zh-CN: "时钟 12 小时制"
  $description: "12-hour clock with AM/PM. Default: 24-hour."
  $description:zh-CN: "12 小时制并显示上午/下午。默认 24 小时制。"
- statusText: ""
  $name: "Status text"
  $name:zh-CN: "状态卡文案"
  $description: "Custom status line. Empty uses built-in tip."
  $description:zh-CN: "状态卡自定义文案。留空则用内置提示。"
- weatherCity: "上海"
  $name: "Weather city label"
  $name:zh-CN: "天气城市名"
  $description: "City label on the demo weather card."
  $description:zh-CN: "演示天气卡上显示的城市名。"
- weatherDemoText: "晴 26°C"
  $name: "Weather demo line"
  $name:zh-CN: "天气演示文案"
  $description: "Static demo condition line (not live data)."
  $description:zh-CN: "静态演示天气文案（非实时数据）。"
- countdownLabel: "目标"
  $name: "Countdown label"
  $name:zh-CN: "倒计时标题"
  $description: "Title above the day count."
  $description:zh-CN: "剩余天数上方的标题。"
- countdownDate: "2027-01-01"
  $name: "Countdown target date"
  $name:zh-CN: "倒计时目标日期"
  $description: "YYYY-MM-DD. Invalid date shows a tip."
  $description:zh-CN: "格式 YYYY-MM-DD。无效则显示提示。"
- stickyText: "写点什么…"
  $name: "Sticky note text"
  $name:zh-CN: "便签内容"
  $description: "Sticky card body."
  $description:zh-CN: "便签卡片正文。"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <string>
#include <vector>
#include <cstring>

#ifndef WCA_ACCENT_POLICY
#define WCA_ACCENT_POLICY 19
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWCP_ROUND
#define DWMWCP_DONOTROUND 1
#define DWMWCP_ROUND 2
#define DWMWCP_ROUNDSMALL 3
#endif

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
};

struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    int GradientColor;  // ABGR
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    int Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
static SetWindowCompositionAttribute_t g_pSetWindowCompositionAttribute = nullptr;

static constexpr wchar_t kRegPath[] = L"Software\\ZenDesktop\\DesktopWidgets";
static constexpr wchar_t kClassName[] = L"ZenDesktop.DesktopWidget";
static constexpr UINT_PTR kTimerRefresh = 1;
static constexpr int kSnapGrid = 16;
static constexpr int kStackGap = 14;

enum class WidgetKind {
    Clock = 0,
    Status,
    Weather,
    Calendar,
    System,
    Countdown,
    Sticky,
    Count
};

enum class WidgetSize {
    Small = 0,
    Medium,
    Large
};

struct Settings {
    bool showClock = true;
    bool showStatus = true;
    bool showWeather = false;
    bool showCalendar = false;
    bool showSystem = false;
    bool showCountdown = false;
    bool showSticky = false;
    int fontSize = 0;  // 0 = auto from size preset
    bool clockShowSeconds = false;
    bool clock12Hour = false;
    bool snapToGrid = true;
    WidgetSize widgetSize = WidgetSize::Medium;
    std::wstring statusText;
    std::wstring weatherCity;
    std::wstring weatherDemoText;
    std::wstring countdownLabel;
    std::wstring countdownDate;
    std::wstring stickyText;
    std::wstring presetPack;
    std::wstring aiDescribe;
    std::wstring aiLayoutMode;
} g_settings;

struct WidgetText {
    wchar_t caption[64]{};
    wchar_t value[96]{};
    wchar_t footer[96]{};
};

struct Widget {
    WidgetKind kind = WidgetKind::Clock;
    HWND hwnd = nullptr;
    int x = 80;
    int y = 80;
    int width = 280;
    int height = 110;
    bool dragging = false;
    POINT dragStart{};
    POINT winStart{};
    wchar_t lastCap[64]{};
    wchar_t lastVal[96]{};
    wchar_t lastFoot[96]{};
    UINT timerMs = 0;
};

static std::vector<Widget> g_widgets;
static ATOM g_atom = 0;
static HFONT g_fontValue = nullptr;
static HFONT g_fontCaption = nullptr;
static HFONT g_fontBody = nullptr;
static HBRUSH g_brushCardBg = nullptr;
static bool g_classRegistered = false;
static bool g_darkMode = true;
static COLORREF g_colorValue = RGB(0xF5, 0xF5, 0xF7);
static COLORREF g_colorCaption = RGB(0xAE, 0xAE, 0xB2);
static COLORREF g_colorBody = RGB(0xE5, 0xE5, 0xEA);
static COLORREF g_colorCardSolid = RGB(0x1C, 0x1C, 0x1E);

static constexpr wchar_t kDefaultStatusText[] = L"ZenDesktop 小组件";
static constexpr wchar_t kWeek[][4] = {L"日", L"一", L"二", L"三", L"四", L"五", L"六"};
static constexpr wchar_t kWeekLong[][8] = {L"星期日", L"星期一", L"星期二", L"星期三",
                                           L"星期四", L"星期五", L"星期六"};

static void LoadSettings();
static void ApplyPresetAndAi();
static void RebuildFonts();
static void RefreshThemeColors();
static void ApplyGlass(HWND hwnd);
static void ApplyRoundedCorners(HWND hwnd, int width, int height);
static void EnsureWidgets();
static void DestroyAllWidgets();
static void PersistPosition(const Widget& w);
static void LoadPosition(Widget& w);
static LRESULT CALLBACK WidgetWndProc(HWND, UINT, WPARAM, LPARAM);
static void PaintWidget(HWND hwnd, Widget* w);
static Widget* FindWidget(HWND hwnd);
static void FormatWidgetText(WidgetKind kind, WidgetText* out);
static bool ParseYmd(const std::wstring& s, SYSTEMTIME* out);
static int DaysUntil(const SYSTEMTIME& target);
static void ReadBattery(WidgetText* out);
static void SizeForKind(WidgetKind kind, int* outW, int* outH);
static int PadForSize();
static int ValuePxForSize();
static int CaptionPxForSize();
static int BodyPxForSize();
static int CornerRadiusPx();
static int SnapCoord(int v);

static int Clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static std::wstring ReadStr(PCWSTR key) {
    std::wstring out;
    if (PCWSTR raw = Wh_GetStringSetting(key)) {
        out = raw;
        Wh_FreeStringSetting(raw);
    }
    return out;
}

static bool ContainsI(const std::wstring& hay, const wchar_t* needle) {
    if (!needle || !*needle) return false;
    std::wstring h = hay;
    std::wstring n = needle;
    for (auto& c : h) {
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
    }
    for (auto& c : n) {
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
    }
    return h.find(n) != std::wstring::npos;
}

static bool* ShowFlagFor(WidgetKind k) {
    switch (k) {
        case WidgetKind::Clock:
            return &g_settings.showClock;
        case WidgetKind::Status:
            return &g_settings.showStatus;
        case WidgetKind::Weather:
            return &g_settings.showWeather;
        case WidgetKind::Calendar:
            return &g_settings.showCalendar;
        case WidgetKind::System:
            return &g_settings.showSystem;
        case WidgetKind::Countdown:
            return &g_settings.showCountdown;
        case WidgetKind::Sticky:
            return &g_settings.showSticky;
        default:
            return nullptr;
    }
}

static void SetAllShows(bool v) {
    g_settings.showClock = v;
    g_settings.showStatus = v;
    g_settings.showWeather = v;
    g_settings.showCalendar = v;
    g_settings.showSystem = v;
    g_settings.showCountdown = v;
    g_settings.showSticky = v;
}

static WidgetSize ParseWidgetSize(const std::wstring& s) {
    if (s == L"small") return WidgetSize::Small;
    if (s == L"large") return WidgetSize::Large;
    return WidgetSize::Medium;
}

static int PadForSize() {
    switch (g_settings.widgetSize) {
        case WidgetSize::Small:
            return 16;
        case WidgetSize::Large:
            return 24;
        default:
            return 20;
    }
}

static int ValuePxForSize() {
    if (g_settings.fontSize > 0) {
        return Clamp(g_settings.fontSize, 16, 72);
    }
    switch (g_settings.widgetSize) {
        case WidgetSize::Small:
            return 28;
        case WidgetSize::Large:
            return 52;
        default:
            return 36;
    }
}

static int CaptionPxForSize() {
    switch (g_settings.widgetSize) {
        case WidgetSize::Small:
            return 11;
        case WidgetSize::Large:
            return 13;
        default:
            return 12;
    }
}

static int BodyPxForSize() {
    switch (g_settings.widgetSize) {
        case WidgetSize::Small:
            return 13;
        case WidgetSize::Large:
            return 16;
        default:
            return 14;
    }
}

static int CornerRadiusPx() {
    switch (g_settings.widgetSize) {
        case WidgetSize::Small:
            return 22;
        case WidgetSize::Large:
            return 32;
        default:
            return 26;
    }
}

static int SnapCoord(int v) {
    if (!g_settings.snapToGrid) return v;
    if (v >= 0) {
        return ((v + kSnapGrid / 2) / kSnapGrid) * kSnapGrid;
    }
    return -(((-v) + kSnapGrid / 2) / kSnapGrid) * kSnapGrid;
}

static void SizeForKind(WidgetKind kind, int* outW, int* outH) {
    // Approximate macOS Desktop widget footprints (scaled for free desktop cards).
    int w = 340;
    int h = 168;
    switch (g_settings.widgetSize) {
        case WidgetSize::Small:
            w = 168;
            h = 168;
            break;
        case WidgetSize::Large:
            w = 340;
            h = 340;
            break;
        default:
            w = 340;
            h = 168;
            break;
    }

    // Kind tweaks: clock/status prefer wide-short in small; sticky uses full box.
    if (g_settings.widgetSize == WidgetSize::Small) {
        if (kind == WidgetKind::Clock) {
            w = 200;
            h = 110;
        } else if (kind == WidgetKind::Status || kind == WidgetKind::Sticky) {
            w = 200;
            h = 110;
        }
    } else if (g_settings.widgetSize == WidgetSize::Medium) {
        if (kind == WidgetKind::Calendar || kind == WidgetKind::Countdown) {
            // Slightly squarer medium for number-forward cards.
            w = 200;
            h = 200;
        }
    } else {  // large
        if (kind == WidgetKind::Clock) {
            w = 340;
            h = 200;
        } else if (kind == WidgetKind::Status) {
            w = 340;
            h = 168;
        }
    }

    *outW = w;
    *outH = h;
}

static void ApplyPresetAndAi() {
    const std::wstring& pack = g_settings.presetPack;
    if (pack == L"daily") {
        SetAllShows(false);
        g_settings.showClock = true;
        g_settings.showWeather = true;
        g_settings.showCalendar = true;
    } else if (pack == L"focus") {
        SetAllShows(false);
        g_settings.showClock = true;
        g_settings.showCountdown = true;
        g_settings.showSticky = true;
    } else if (pack == L"system") {
        SetAllShows(false);
        g_settings.showClock = true;
        g_settings.showSystem = true;
        g_settings.showStatus = true;
    } else if (pack == L"all") {
        SetAllShows(true);
    }

    if (g_settings.aiDescribe.empty()) {
        return;
    }

    const bool replace = g_settings.aiLayoutMode == L"replace";
    bool hitClock = false, hitStatus = false, hitWeather = false, hitCal = false;
    bool hitSys = false, hitCd = false, hitSticky = false;

    const std::wstring& d = g_settings.aiDescribe;
    if (ContainsI(d, L"时钟") || ContainsI(d, L"时间") || ContainsI(d, L"clock") ||
        ContainsI(d, L"time")) {
        hitClock = true;
    }
    if (ContainsI(d, L"天气") || ContainsI(d, L"气温") || ContainsI(d, L"weather") ||
        ContainsI(d, L"温度")) {
        hitWeather = true;
    }
    if (ContainsI(d, L"日历") || ContainsI(d, L"日程") || ContainsI(d, L"今天") ||
        ContainsI(d, L"calendar") || ContainsI(d, L"agenda") || ContainsI(d, L"日期")) {
        hitCal = true;
    }
    if (ContainsI(d, L"电量") || ContainsI(d, L"电池") || ContainsI(d, L"系统") ||
        ContainsI(d, L"battery") || ContainsI(d, L"system") || ContainsI(d, L"电源")) {
        hitSys = true;
    }
    if (ContainsI(d, L"倒计时") || ContainsI(d, L"倒数") || ContainsI(d, L"countdown") ||
        ContainsI(d, L"还有几天") || ContainsI(d, L"考试") || ContainsI(d, L"假期")) {
        hitCd = true;
    }
    if (ContainsI(d, L"便签") || ContainsI(d, L"备忘") || ContainsI(d, L"sticky") ||
        ContainsI(d, L"note") || ContainsI(d, L"记事")) {
        hitSticky = true;
    }
    if (ContainsI(d, L"状态") || ContainsI(d, L"自定义") || ContainsI(d, L"status") ||
        ContainsI(d, L"文案")) {
        hitStatus = true;
    }

    const bool any = hitClock || hitStatus || hitWeather || hitCal || hitSys || hitCd || hitSticky;
    if (!any) {
        Wh_Log(L"[ZenWidgets] AI describe had no keyword match: %s", d.c_str());
        return;
    }

    if (replace) {
        SetAllShows(false);
    }
    if (hitClock) g_settings.showClock = true;
    if (hitStatus) g_settings.showStatus = true;
    if (hitWeather) g_settings.showWeather = true;
    if (hitCal) g_settings.showCalendar = true;
    if (hitSys) g_settings.showSystem = true;
    if (hitCd) g_settings.showCountdown = true;
    if (hitSticky) g_settings.showSticky = true;
}

static void LoadSettings() {
    g_settings.showClock = Wh_GetIntSetting(L"showClock") != 0;
    g_settings.showStatus = Wh_GetIntSetting(L"showStatus") != 0;
    g_settings.showWeather = Wh_GetIntSetting(L"showWeather") != 0;
    g_settings.showCalendar = Wh_GetIntSetting(L"showCalendar") != 0;
    g_settings.showSystem = Wh_GetIntSetting(L"showSystem") != 0;
    g_settings.showCountdown = Wh_GetIntSetting(L"showCountdown") != 0;
    g_settings.showSticky = Wh_GetIntSetting(L"showSticky") != 0;
    g_settings.fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_settings.fontSize != 0) {
        g_settings.fontSize = Clamp(g_settings.fontSize, 16, 72);
    }
    g_settings.clockShowSeconds = Wh_GetIntSetting(L"clockShowSeconds") != 0;
    g_settings.clock12Hour = Wh_GetIntSetting(L"clock12Hour") != 0;
    g_settings.snapToGrid = Wh_GetIntSetting(L"snapToGrid") != 0;
    g_settings.widgetSize = ParseWidgetSize(ReadStr(L"widgetSize"));

    g_settings.statusText = ReadStr(L"statusText");
    g_settings.weatherCity = ReadStr(L"weatherCity");
    if (g_settings.weatherCity.empty()) g_settings.weatherCity = L"上海";
    g_settings.weatherDemoText = ReadStr(L"weatherDemoText");
    if (g_settings.weatherDemoText.empty()) g_settings.weatherDemoText = L"晴 26°C";
    g_settings.countdownLabel = ReadStr(L"countdownLabel");
    if (g_settings.countdownLabel.empty()) g_settings.countdownLabel = L"目标";
    g_settings.countdownDate = ReadStr(L"countdownDate");
    if (g_settings.countdownDate.empty()) g_settings.countdownDate = L"2027-01-01";
    g_settings.stickyText = ReadStr(L"stickyText");
    if (g_settings.stickyText.empty()) g_settings.stickyText = L"写点什么…";
    g_settings.presetPack = ReadStr(L"presetPack");
    if (g_settings.presetPack.empty()) g_settings.presetPack = L"manual";
    g_settings.aiDescribe = ReadStr(L"aiDescribe");
    g_settings.aiLayoutMode = ReadStr(L"aiLayoutMode");
    if (g_settings.aiLayoutMode.empty()) g_settings.aiLayoutMode = L"append";

    ApplyPresetAndAi();
}

static bool ParseYmd(const std::wstring& s, SYSTEMTIME* out) {
    if (!out || s.size() < 8) return false;
    unsigned y = 0, m = 0, d = 0;
    if (swscanf_s(s.c_str(), L"%u-%u-%u", &y, &m, &d) != 3) return false;
    if (y < 1970 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) return false;
    ZeroMemory(out, sizeof(*out));
    out->wYear = static_cast<WORD>(y);
    out->wMonth = static_cast<WORD>(m);
    out->wDay = static_cast<WORD>(d);
    return true;
}

static int DaysUntil(const SYSTEMTIME& target) {
    SYSTEMTIME now{};
    GetLocalTime(&now);
    now.wHour = now.wMinute = now.wSecond = now.wMilliseconds = 0;
    SYSTEMTIME t = target;
    t.wHour = t.wMinute = t.wSecond = t.wMilliseconds = 0;
    FILETIME ftNow{}, ftT{};
    if (!SystemTimeToFileTime(&now, &ftNow) || !SystemTimeToFileTime(&t, &ftT)) {
        return 0;
    }
    ULARGE_INTEGER a{}, b{};
    a.LowPart = ftNow.dwLowDateTime;
    a.HighPart = ftNow.dwHighDateTime;
    b.LowPart = ftT.dwLowDateTime;
    b.HighPart = ftT.dwHighDateTime;
    const LONGLONG diff = static_cast<LONGLONG>(b.QuadPart) - static_cast<LONGLONG>(a.QuadPart);
    return static_cast<int>(diff / (10000000LL * 60 * 60 * 24));
}

static bool IsAppsUseDarkTheme() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
                      KEY_READ, &key) != ERROR_SUCCESS) {
        return true;
    }
    DWORD val = 0;
    DWORD size = sizeof(val);
    DWORD type = 0;
    const LONG rc =
        RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, &type, reinterpret_cast<LPBYTE>(&val),
                         &size);
    RegCloseKey(key);
    if (rc == ERROR_SUCCESS && type == REG_DWORD) {
        return val == 0;
    }
    return true;
}

static void RefreshThemeColors() {
    g_darkMode = IsAppsUseDarkTheme();
    if (g_darkMode) {
        g_colorValue = RGB(0xF5, 0xF5, 0xF7);
        g_colorCaption = RGB(0xAE, 0xAE, 0xB2);
        g_colorBody = RGB(0xE5, 0xE5, 0xEA);
        g_colorCardSolid = RGB(0x1C, 0x1C, 0x1E);
    } else {
        g_colorValue = RGB(0x1C, 0x1C, 0x1E);
        g_colorCaption = RGB(0x6C, 0x6C, 0x70);
        g_colorBody = RGB(0x3A, 0x3A, 0x3C);
        g_colorCardSolid = RGB(0xF2, 0xF2, 0xF7);
    }
    if (g_brushCardBg) {
        DeleteObject(g_brushCardBg);
        g_brushCardBg = nullptr;
    }
    g_brushCardBg = CreateSolidBrush(g_colorCardSolid);
}

static void ReadBattery(WidgetText* out) {
    SYSTEM_POWER_STATUS sps{};
    if (!GetSystemPowerStatus(&sps)) {
        wcscpy_s(out->caption, L"电源");
        wcscpy_s(out->value, L"—");
        wcscpy_s(out->footer, L"无法读取");
        return;
    }
    if (sps.BatteryFlag == 128 || sps.BatteryLifePercent == 255) {
        wcscpy_s(out->caption, L"电源");
        wcscpy_s(out->value, L"AC");
        wcscpy_s(out->footer, L"交流供电");
        return;
    }
    const int pct = static_cast<int>(sps.BatteryLifePercent);
    const bool ac = sps.ACLineStatus == 1;
    wcscpy_s(out->caption, L"电量");
    swprintf_s(out->value, L"%d%%", pct);
    if (ac) {
        wcscpy_s(out->footer, L"充电中");
    } else if (sps.BatteryLifeTime != 0xFFFFFFFF) {
        const DWORD mins = sps.BatteryLifeTime / 60;
        swprintf_s(out->footer, L"约 %u 时 %u 分", mins / 60, mins % 60);
    } else {
        wcscpy_s(out->footer, L"使用电池");
    }
}

static void FormatWidgetText(WidgetKind kind, WidgetText* out) {
    out->caption[0] = 0;
    out->value[0] = 0;
    out->footer[0] = 0;
    switch (kind) {
        case WidgetKind::Clock: {
            SYSTEMTIME st{};
            GetLocalTime(&st);
            wcscpy_s(out->caption, L"时钟");
            if (g_settings.clock12Hour) {
                unsigned h = st.wHour % 12;
                if (h == 0) h = 12;
                const wchar_t* ap = st.wHour < 12 ? L"上午" : L"下午";
                if (g_settings.clockShowSeconds) {
                    swprintf_s(out->value, L"%u:%02u:%02u", h, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(out->value, L"%u:%02u", h, st.wMinute);
                }
                swprintf_s(out->footer, L"%s · %04u-%02u-%02u %s", ap, st.wYear, st.wMonth, st.wDay,
                           kWeekLong[st.wDayOfWeek % 7]);
            } else if (g_settings.clockShowSeconds) {
                swprintf_s(out->value, L"%02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);
                swprintf_s(out->footer, L"%04u-%02u-%02u  %s", st.wYear, st.wMonth, st.wDay,
                           kWeekLong[st.wDayOfWeek % 7]);
            } else {
                swprintf_s(out->value, L"%02u:%02u", st.wHour, st.wMinute);
                swprintf_s(out->footer, L"%04u-%02u-%02u  %s", st.wYear, st.wMonth, st.wDay,
                           kWeekLong[st.wDayOfWeek % 7]);
            }
            break;
        }
        case WidgetKind::Status: {
            wcscpy_s(out->caption, L"状态");
            const wchar_t* t = g_settings.statusText.empty() ? kDefaultStatusText
                                                            : g_settings.statusText.c_str();
            wcsncpy_s(out->value, t, _TRUNCATE);
            break;
        }
        case WidgetKind::Weather:
            wcsncpy_s(out->caption, g_settings.weatherCity.c_str(), _TRUNCATE);
            wcsncpy_s(out->value, g_settings.weatherDemoText.c_str(), _TRUNCATE);
            wcscpy_s(out->footer, L"演示数据");
            break;
        case WidgetKind::Calendar: {
            SYSTEMTIME st{};
            GetLocalTime(&st);
            swprintf_s(out->caption, L"%04u年%u月", st.wYear, st.wMonth);
            swprintf_s(out->value, L"%u", st.wDay);
            swprintf_s(out->footer, L"%s", kWeekLong[st.wDayOfWeek % 7]);
            break;
        }
        case WidgetKind::System:
            ReadBattery(out);
            break;
        case WidgetKind::Countdown: {
            SYSTEMTIME target{};
            wcsncpy_s(out->caption, g_settings.countdownLabel.c_str(), _TRUNCATE);
            if (!ParseYmd(g_settings.countdownDate, &target)) {
                wcscpy_s(out->value, L"—");
                wcscpy_s(out->footer, L"日期无效");
            } else {
                const int days = DaysUntil(target);
                if (days > 0) {
                    swprintf_s(out->value, L"%d", days);
                    wcscpy_s(out->footer, L"天剩余");
                } else if (days == 0) {
                    wcscpy_s(out->value, L"0");
                    wcscpy_s(out->footer, L"就是今天");
                } else {
                    swprintf_s(out->value, L"%d", -days);
                    wcscpy_s(out->footer, L"天已过");
                }
            }
            break;
        }
        case WidgetKind::Sticky:
            wcscpy_s(out->caption, L"便签");
            wcsncpy_s(out->value, g_settings.stickyText.c_str(), _TRUNCATE);
            break;
        default:
            break;
    }
}

static HFONT CreateUiFont(int px, int weight, bool weightFaceDisplay) {
    if (px < 8) px = 8;
    const bool display = weightFaceDisplay;
    const wchar_t* faces[] = {
        display ? L"Segoe UI Variable Display" : L"Segoe UI Variable Text",
        L"Segoe UI",
        L"Microsoft YaHei UI",
        L"Microsoft YaHei",
    };
    for (const wchar_t* face : faces) {
        HFONT f = CreateFontW(-px, 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_SWISS, face);
        if (!f) continue;
        HDC hdc = GetDC(nullptr);
        if (!hdc) return f;
        HGDIOBJ old = SelectObject(hdc, f);
        wchar_t got[64]{};
        GetTextFaceW(hdc, ARRAYSIZE(got), got);
        SelectObject(hdc, old);
        ReleaseDC(nullptr, hdc);
        if (got[0] && (wcsstr(got, L"Segoe") || wcsstr(got, L"YaHei") ||
                       wcsstr(got, L"Microsoft"))) {
            return f;
        }
        DeleteObject(f);
    }
    return static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
}

static HFONT FontOrStock(HFONT f) {
    return f ? f : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
}

static void SafeDeleteFont(HFONT* f) {
    if (!f || !*f) return;
    const HFONT stock = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    if (*f != stock) DeleteObject(*f);
    *f = nullptr;
}

static void RebuildFonts() {
    SafeDeleteFont(&g_fontValue);
    SafeDeleteFont(&g_fontCaption);
    SafeDeleteFont(&g_fontBody);
    g_fontValue = CreateUiFont(ValuePxForSize(), FW_SEMIBOLD, true);
    g_fontCaption = CreateUiFont(CaptionPxForSize(), FW_SEMIBOLD, false);
    g_fontBody = CreateUiFont(BodyPxForSize(), FW_NORMAL, false);
}

static void ApplyRoundedCorners(HWND hwnd, int width, int height) {
    // Prefer Win11 system rounded corners (works well with Acrylic).
    int pref = DWMWCP_ROUND;
    if (SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref)))) {
        return;
    }
    // Fallback: region round-rect (softer large radius).
    const int r = CornerRadiusPx() * 2;
    HRGN rgn = CreateRoundRectRgn(0, 0, width + 1, height + 1, r, r);
    if (rgn) {
        SetWindowRgn(hwnd, rgn, TRUE);
    }
}

static void ApplyGlass(HWND hwnd) {
    if (!g_pSetWindowCompositionAttribute) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            g_pSetWindowCompositionAttribute =
                reinterpret_cast<SetWindowCompositionAttribute_t>(
                    GetProcAddress(user32, "SetWindowCompositionAttribute"));
        }
    }

    // Subtle glass tint: dark ≈ macOS dark widget; light ≈ frosted light card.
    const int tint = g_darkMode ? static_cast<int>(0x992C2C2E) : static_cast<int>(0xB3F5F5F7);

    if (g_pSetWindowCompositionAttribute) {
        ACCENT_POLICY policy{};
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        policy.GradientColor = tint;
        WINDOWCOMPOSITIONATTRIBDATA data{};
        data.Attribute = WCA_ACCENT_POLICY;
        data.Data = &policy;
        data.SizeOfData = sizeof(policy);
        if (g_pSetWindowCompositionAttribute(hwnd, &data)) {
            return;
        }
        policy.AccentState = ACCENT_ENABLE_BLURBEHIND;
        g_pSetWindowCompositionAttribute(hwnd, &data);
        return;
    }

    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, GetWindowLongPtrW(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, g_darkMode ? 210 : 230, LWA_ALPHA);
}

static const wchar_t* RegNameX(WidgetKind k) {
    switch (k) {
        case WidgetKind::Clock:
            return L"ClockX";
        case WidgetKind::Status:
            return L"StatusX";
        case WidgetKind::Weather:
            return L"WeatherX";
        case WidgetKind::Calendar:
            return L"CalendarX";
        case WidgetKind::System:
            return L"SystemX";
        case WidgetKind::Countdown:
            return L"CountdownX";
        case WidgetKind::Sticky:
            return L"StickyX";
        default:
            return L"WidgetX";
    }
}

static const wchar_t* RegNameY(WidgetKind k) {
    switch (k) {
        case WidgetKind::Clock:
            return L"ClockY";
        case WidgetKind::Status:
            return L"StatusY";
        case WidgetKind::Weather:
            return L"WeatherY";
        case WidgetKind::Calendar:
            return L"CalendarY";
        case WidgetKind::System:
            return L"SystemY";
        case WidgetKind::Countdown:
            return L"CountdownY";
        case WidgetKind::Sticky:
            return L"StickyY";
        default:
            return L"WidgetY";
    }
}

static void PersistPosition(const Widget& w) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRegPath, 0, nullptr, 0, KEY_WRITE, nullptr, &key,
                        nullptr) != ERROR_SUCCESS) {
        return;
    }
    DWORD x = static_cast<DWORD>(w.x);
    DWORD y = static_cast<DWORD>(w.y);
    RegSetValueExW(key, RegNameX(w.kind), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&x),
                   sizeof(x));
    RegSetValueExW(key, RegNameY(w.kind), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&y),
                   sizeof(y));
    RegCloseKey(key);
}

static void LoadPosition(Widget& w) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegPath, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return;
    }
    DWORD type = 0;
    DWORD x = 0, y = 0;
    DWORD size = sizeof(DWORD);
    if (RegQueryValueExW(key, RegNameX(w.kind), nullptr, &type, reinterpret_cast<LPBYTE>(&x),
                         &size) == ERROR_SUCCESS &&
        type == REG_DWORD) {
        w.x = static_cast<int>(x);
    }
    size = sizeof(DWORD);
    if (RegQueryValueExW(key, RegNameY(w.kind), nullptr, &type, reinterpret_cast<LPBYTE>(&y),
                         &size) == ERROR_SUCCESS &&
        type == REG_DWORD) {
        w.y = static_cast<int>(y);
    }
    RegCloseKey(key);
}

static Widget* FindWidget(HWND hwnd) {
    if (!hwnd) return nullptr;
    if (auto* w = reinterpret_cast<Widget*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA))) {
        return w;
    }
    for (auto& w : g_widgets) {
        if (w.hwnd == hwnd) return &w;
    }
    return nullptr;
}

static void DestroyCardGdi() {
    if (g_brushCardBg) {
        DeleteObject(g_brushCardBg);
        g_brushCardBg = nullptr;
    }
}

static void RebindWidgetUserData() {
    for (auto& w : g_widgets) {
        if (w.hwnd && IsWindow(w.hwnd)) {
            SetWindowLongPtrW(w.hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&w));
        }
    }
}

static bool IsBodyCard(WidgetKind kind) {
    return kind == WidgetKind::Status || kind == WidgetKind::Sticky;
}

static bool WidgetTextHasContent(const WidgetText& tx) {
    return tx.caption[0] || tx.value[0] || tx.footer[0];
}

static void PaintWidgetContent(HDC hdc, Widget* w, const RECT& rc) {
    if (!g_brushCardBg) {
        g_brushCardBg = CreateSolidBrush(g_colorCardSolid);
    }
    FillRect(hdc, &rc, g_brushCardBg);

    SetBkMode(hdc, TRANSPARENT);

    WidgetText tx{};
    FormatWidgetText(w->kind, &tx);
    if (!WidgetTextHasContent(tx)) {
        wcscpy_s(tx.caption, L"小组件");
        wcscpy_s(tx.value, L"—");
        wcscpy_s(tx.footer, L"无内容");
    }

    const int pad = PadForSize();
    RECT content = rc;
    InflateRect(&content, -pad, -pad);

    HFONT fontCap = FontOrStock(g_fontCaption);
    HFONT fontVal = FontOrStock(g_fontValue);
    HFONT fontBody = FontOrStock(g_fontBody);

    if (IsBodyCard(w->kind)) {
        RECT rCap = content;
        HFONT old = static_cast<HFONT>(SelectObject(hdc, fontCap));
        SetTextColor(hdc, g_colorCaption);
        DrawTextW(hdc, tx.caption, -1, &rCap, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

        SIZE capSz{};
        GetTextExtentPoint32W(hdc, tx.caption, static_cast<int>(wcslen(tx.caption)), &capSz);

        RECT rBody = content;
        rBody.top += capSz.cy + 8;
        SelectObject(hdc, fontBody);
        SetTextColor(hdc, g_colorBody);
        DrawTextW(hdc, tx.value, -1, &rBody, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(hdc, old);
    } else {
        int y = content.top;
        HFONT old = static_cast<HFONT>(SelectObject(hdc, fontCap));

        if (tx.caption[0]) {
            RECT rCap = content;
            rCap.top = y;
            SetTextColor(hdc, g_colorCaption);
            DrawTextW(hdc, tx.caption, -1, &rCap, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
            SIZE capSz{};
            GetTextExtentPoint32W(hdc, tx.caption, static_cast<int>(wcslen(tx.caption)), &capSz);
            y += capSz.cy + 6;
        }

        if (tx.value[0]) {
            RECT rVal = content;
            rVal.top = y;
            SelectObject(hdc, fontVal);
            SetTextColor(hdc, g_colorValue);
            const UINT valFlags =
                (g_settings.widgetSize == WidgetSize::Large &&
                 (w->kind == WidgetKind::Calendar || w->kind == WidgetKind::Countdown))
                    ? (DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX)
                    : (DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
            if (valFlags & DT_VCENTER) {
                rVal.bottom = content.bottom - (tx.footer[0] ? BodyPxForSize() + 10 : 0);
                DrawTextW(hdc, tx.value, -1, &rVal, valFlags);
                SIZE valSz{};
                GetTextExtentPoint32W(hdc, tx.value, static_cast<int>(wcslen(tx.value)), &valSz);
                y = (rVal.top + rVal.bottom + valSz.cy) / 2 + 8;
            } else {
                DrawTextW(hdc, tx.value, -1, &rVal, valFlags);
                SIZE valSz{};
                GetTextExtentPoint32W(hdc, tx.value, static_cast<int>(wcslen(tx.value)), &valSz);
                y += valSz.cy + 6;
            }
        }

        if (tx.footer[0]) {
            RECT rFoot = content;
            if (g_settings.widgetSize == WidgetSize::Large &&
                (w->kind == WidgetKind::Calendar || w->kind == WidgetKind::Countdown)) {
                rFoot.top = content.bottom - BodyPxForSize() - 4;
            } else {
                rFoot.top = y;
            }
            SelectObject(hdc, fontBody);
            SetTextColor(hdc, g_colorCaption);
            const UINT footFlags =
                (g_settings.widgetSize == WidgetSize::Large &&
                 (w->kind == WidgetKind::Calendar || w->kind == WidgetKind::Countdown))
                    ? (DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOPREFIX)
                    : (DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
            DrawTextW(hdc, tx.footer, -1, &rFoot, footFlags);
        }

        SelectObject(hdc, old);
    }
}

static void PaintWidget(HWND hwnd, Widget* w) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hwnd, &ps);
    if (!hdc) return;

    RECT rc{};
    GetClientRect(hwnd, &rc);
    if (!g_brushCardBg) {
        g_brushCardBg = CreateSolidBrush(g_colorCardSolid);
    }
    if (w) {
        PaintWidgetContent(hdc, w, rc);
    } else {
        FillRect(hdc, &rc, g_brushCardBg);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, g_colorValue);
        HFONT old = static_cast<HFONT>(SelectObject(hdc, FontOrStock(g_fontBody)));
        DrawTextW(hdc, L"ZenDesktop", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(hdc, old);
    }

    EndPaint(hwnd, &ps);
}

static UINT TimerIntervalFor(WidgetKind kind) {
    switch (kind) {
        case WidgetKind::Clock:
            return g_settings.clockShowSeconds ? 1000 : 15000;
        case WidgetKind::Calendar:
            return 60000;
        case WidgetKind::System:
            return 30000;
        case WidgetKind::Countdown:
            return 60000;
        case WidgetKind::Weather:
            return 0;
        default:
            return 0;
    }
}

static LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Widget* w = FindWidget(hwnd);

    switch (msg) {
        case WM_NCHITTEST:
            return HTCLIENT;

        case WM_LBUTTONDOWN:
            if (w) {
                SetCapture(hwnd);
                w->dragging = true;
                GetCursorPos(&w->dragStart);
                w->winStart = {w->x, w->y};
            }
            return 0;

        case WM_MOUSEMOVE:
            if (w && w->dragging && (wParam & MK_LBUTTON)) {
                POINT pt{};
                GetCursorPos(&pt);
                w->x = w->winStart.x + (pt.x - w->dragStart.x);
                w->y = w->winStart.y + (pt.y - w->dragStart.y);
                SetWindowPos(hwnd, nullptr, w->x, w->y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;

        case WM_LBUTTONUP:
            if (w && w->dragging) {
                w->dragging = false;
                ReleaseCapture();
                w->x = SnapCoord(w->x);
                w->y = SnapCoord(w->y);
                SetWindowPos(hwnd, nullptr, w->x, w->y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                PersistPosition(*w);
            }
            return 0;

        case WM_TIMER:
            if (wParam == kTimerRefresh && w) {
                WidgetText tx{};
                FormatWidgetText(w->kind, &tx);
                if (wcscmp(tx.caption, w->lastCap) != 0 || wcscmp(tx.value, w->lastVal) != 0 ||
                    wcscmp(tx.footer, w->lastFoot) != 0) {
                    wcscpy_s(w->lastCap, tx.caption);
                    wcscpy_s(w->lastVal, tx.value);
                    wcscpy_s(w->lastFoot, tx.footer);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            PaintWidget(hwnd, w);
            return 0;

        case WM_DESTROY:
            if (w) {
                KillTimer(hwnd, kTimerRefresh);
                w->hwnd = nullptr;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool EnsureClass() {
    if (g_classRegistered) return true;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WidgetWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;
    wc.hbrBackground = nullptr;  // WM_PAINT always fills; avoid empty black plates
    g_atom = RegisterClassExW(&wc);
    if (!g_atom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"[ZenWidgets] RegisterClassEx failed: %lu", GetLastError());
        return false;
    }
    g_classRegistered = true;
    return true;
}

static HWND CreateWidgetWindow(Widget& w) {
    if (!EnsureClass()) return nullptr;

    int width = 240;
    int height = 100;
    SizeForKind(w.kind, &width, &height);
    w.width = width;
    w.height = height;

    HWND hwnd =
        CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kClassName, L"ZenDesktop Widget",
                        WS_POPUP, w.x, w.y, w.width, w.height, nullptr, nullptr,
                        GetModuleHandleW(nullptr), nullptr);
    if (!hwnd) {
        Wh_Log(L"[ZenWidgets] CreateWindowEx failed: %lu", GetLastError());
        return nullptr;
    }

    w.hwnd = hwnd;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&w));
    ApplyRoundedCorners(hwnd, w.width, w.height);
    ApplyGlass(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    WidgetText tx{};
    FormatWidgetText(w.kind, &tx);
    wcscpy_s(w.lastCap, tx.caption);
    wcscpy_s(w.lastVal, tx.value);
    wcscpy_s(w.lastFoot, tx.footer);
    w.timerMs = TimerIntervalFor(w.kind);
    if (w.timerMs) {
        SetTimer(hwnd, kTimerRefresh, w.timerMs, nullptr);
    }
    InvalidateRect(hwnd, nullptr, FALSE);
    return hwnd;
}

static void DestroyAllWidgets() {
    for (auto& w : g_widgets) {
        if (w.hwnd && IsWindow(w.hwnd)) {
            DestroyWindow(w.hwnd);
        }
        w.hwnd = nullptr;
    }
    g_widgets.clear();
}

static void AddWidgetStacked(WidgetKind kind, int* colX, int* nextY) {
    bool* flag = ShowFlagFor(kind);
    if (!flag || !*flag) return;

    int w = 0, h = 0;
    SizeForKind(kind, &w, &h);

    Widget widget{};
    widget.kind = kind;
    widget.x = *colX;
    widget.y = *nextY;
    LoadPosition(widget);
    // First-time defaults stay stacked; after drag HKCU overrides.
    g_widgets.push_back(widget);
    if (!CreateWidgetWindow(g_widgets.back())) {
        g_widgets.pop_back();
        return;
    }
    // Advance stack using default column geometry (ignore dragged override for layout cursor).
    *nextY += h + kStackGap;
}

static void EnsureWidgets() {
    DestroyAllWidgets();
    RefreshThemeColors();
    RebuildFonts();
    g_widgets.reserve(static_cast<size_t>(WidgetKind::Count));

    const int screenW = GetSystemMetrics(SM_CXSCREEN);
    int maxW = 168;
    for (int i = 0; i < static_cast<int>(WidgetKind::Count); ++i) {
        int ww = 0, hh = 0;
        SizeForKind(static_cast<WidgetKind>(i), &ww, &hh);
        if (ww > maxW) maxW = ww;
    }
    int colX = screenW - maxW - 28;
    if (colX < 24) colX = 24;
    colX = SnapCoord(colX);
    int nextY = SnapCoord(40);

    // Stack defaults down the right edge (macOS-like tidy column); HKCU overrides after drag.
    AddWidgetStacked(WidgetKind::Clock, &colX, &nextY);
    AddWidgetStacked(WidgetKind::Calendar, &colX, &nextY);
    AddWidgetStacked(WidgetKind::Weather, &colX, &nextY);
    AddWidgetStacked(WidgetKind::System, &colX, &nextY);
    AddWidgetStacked(WidgetKind::Countdown, &colX, &nextY);
    AddWidgetStacked(WidgetKind::Status, &colX, &nextY);
    AddWidgetStacked(WidgetKind::Sticky, &colX, &nextY);

    RebindWidgetUserData();
    for (auto& w : g_widgets) {
        if (!w.hwnd || !IsWindow(w.hwnd)) continue;
        WidgetText tx{};
        FormatWidgetText(w.kind, &tx);
        if (!WidgetTextHasContent(tx)) {
            ShowWindow(w.hwnd, SW_HIDE);
            Wh_Log(L"[ZenWidgets] Hid empty widget kind=%d", static_cast<int>(w.kind));
            continue;
        }
        ApplyGlass(w.hwnd);
        InvalidateRect(w.hwnd, nullptr, FALSE);
        UpdateWindow(w.hwnd);
    }

    Wh_Log(L"[ZenWidgets] Active widgets: %zu (size=%d dark=%d snap=%d pack=%s)", g_widgets.size(),
           static_cast<int>(g_settings.widgetSize), g_darkMode ? 1 : 0,
           g_settings.snapToGrid ? 1 : 0, g_settings.presetPack.c_str());
}

BOOL Wh_ModInit() {
    Wh_Log(L"[ZenWidgets] === Wh_ModInit v0.2.2 ===");
    LoadSettings();
    EnsureWidgets();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[ZenWidgets] === Wh_ModUninit ===");
    DestroyAllWidgets();
    DestroyCardGdi();
    SafeDeleteFont(&g_fontValue);
    SafeDeleteFont(&g_fontCaption);
    SafeDeleteFont(&g_fontBody);
    if (g_classRegistered) {
        UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
        g_classRegistered = false;
        g_atom = 0;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[ZenWidgets] === Settings changed ===");
    LoadSettings();
    EnsureWidgets();
}
