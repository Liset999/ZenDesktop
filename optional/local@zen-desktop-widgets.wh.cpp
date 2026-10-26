// ==WindhawkMod==
// @id              zen-desktop-widgets
// @name            ZenDesktop: 桌面小组件
// @name:zh-CN      ZenDesktop: 桌面小组件
// @description     Optional glass desktop widgets: clock, status, weather demo, calendar, battery, countdown, sticky. Describe a card to enable templates. Default off.
// @description:zh-CN 可选玻璃桌面小组件：时钟、状态、天气演示、日历、电量、倒计时、便签。可用中文描述自动编排内置模板。默认关闭。
// @version         0.2.0
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

在桌面上放置可拖动的玻璃风卡片。**v0.2** 内置多块模板，并提供类似小米「超级背屏 / AI 生成应用卡」思路的**本地规则编排**（用一句话描述想要的卡片 → 映射到内置模板）。不是 Rainmeter；不调用云端大模型。

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

## 卡片编排（超级背屏风格 MVP）

- **预设套装**：一键启用常见组合（日常 / 专注 / 全部关闭仅手调）。
- **描述想要的卡片**：填写中文或英文关键词（如「时钟和天气」「倒计时考试」），规则引擎启用对应模板。
- **编排模式**：追加（保留原开关并打开匹配项）或替换（只开匹配项）。
- 真 LLM / 本地 API 生成布局见仓库 `docs/desktop-widgets.md` 路线图；本进程内不做网络推理。

## 其它

- 左键拖动；位置写入 `HKCU\Software\ZenDesktop\DesktopWidgets\`。
- 可选 Mod，部署默认关闭。玻璃依赖系统「透明度效果」。

## 更新日志

- **0.2.0**：内置时钟/状态/天气演示/日历/电量/倒计时/便签；12/24h；预设套装 + 描述→模板编排（超级背屏风格 MVP）；文档更新。
- **0.1.1**：性能：未变不重绘、USERDATA、GDI 复用。
- **0.1.0**：MVP：时钟 + 状态卡。

## 致谢与许可

- ZenDesktop 原创 optional Mod，GPL-3.0。
- 开发者：Lanbo（[哔哩哔哩](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo)）
- 「超级背屏」仅为交互灵感来源，与小米无关、无商标授权。
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
- fontSize: 28
  $name: "Font size (px)"
  $name:zh-CN: "主文字大小（像素）"
  $description: "Primary text size, 16–64. Default: 28."
  $description:zh-CN: "主文字大小，范围 16–64 像素。默认 28。"
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
static constexpr int kPad = 18;

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

struct Settings {
    bool showClock = true;
    bool showStatus = true;
    bool showWeather = false;
    bool showCalendar = false;
    bool showSystem = false;
    bool showCountdown = false;
    bool showSticky = false;
    int fontSize = 28;
    bool clockShowSeconds = false;
    bool clock12Hour = false;
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
    wchar_t lastLine1[96]{};
    wchar_t lastLine2[160]{};
    UINT timerMs = 0;
};

static std::vector<Widget> g_widgets;
static ATOM g_atom = 0;
static HFONT g_fontLarge = nullptr;
static HFONT g_fontSmall = nullptr;
static HBRUSH g_brushCardBg = nullptr;
static bool g_classRegistered = false;

static constexpr wchar_t kDefaultStatusText[] = L"ZenDesktop 小组件";
static constexpr wchar_t kWeek[][4] = {L"日", L"一", L"二", L"三", L"四", L"五", L"六"};
static constexpr wchar_t kWeekLong[][8] = {L"星期日", L"星期一", L"星期二", L"星期三",
                                           L"星期四", L"星期五", L"星期六"};

static void LoadSettings();
static void ApplyPresetAndAi();
static void RebuildFonts();
static void ApplyGlass(HWND hwnd);
static void EnsureWidgets();
static void DestroyAllWidgets();
static void PersistPosition(const Widget& w);
static void LoadPosition(Widget& w);
static LRESULT CALLBACK WidgetWndProc(HWND, UINT, WPARAM, LPARAM);
static void PaintWidget(HWND hwnd, Widget* w);
static Widget* FindWidget(HWND hwnd);
static void FormatClockLines(wchar_t* line1, size_t cch1, wchar_t* line2, size_t cch2);
static void FormatWidgetLines(WidgetKind kind, wchar_t* line1, size_t cch1, wchar_t* line2,
                              size_t cch2);
static bool ParseYmd(const std::wstring& s, SYSTEMTIME* out);
static int DaysUntil(const SYSTEMTIME& target);
static void ReadBattery(wchar_t* line1, size_t cch1, wchar_t* line2, size_t cch2);

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
    // manual: keep toggles from settings

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
    g_settings.fontSize = Clamp(Wh_GetIntSetting(L"fontSize"), 16, 64);
    g_settings.clockShowSeconds = Wh_GetIntSetting(L"clockShowSeconds") != 0;
    g_settings.clock12Hour = Wh_GetIntSetting(L"clock12Hour") != 0;

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

static void ReadBattery(wchar_t* line1, size_t cch1, wchar_t* line2, size_t cch2) {
    SYSTEM_POWER_STATUS sps{};
    if (!GetSystemPowerStatus(&sps)) {
        swprintf_s(line1, cch1, L"电源");
        swprintf_s(line2, cch2, L"无法读取");
        return;
    }
    if (sps.BatteryFlag == 128 || sps.BatteryLifePercent == 255) {
        swprintf_s(line1, cch1, L"交流供电");
        swprintf_s(line2, cch2, L"未检测到电池");
        return;
    }
    const int pct = static_cast<int>(sps.BatteryLifePercent);
    const bool ac = sps.ACLineStatus == 1;
    swprintf_s(line1, cch1, L"电量 %d%%", pct);
    if (ac) {
        swprintf_s(line2, cch2, L"充电中 / 已接电源");
    } else if (sps.BatteryLifeTime != 0xFFFFFFFF) {
        const DWORD mins = sps.BatteryLifeTime / 60;
        swprintf_s(line2, cch2, L"约 %u 小时 %u 分", mins / 60, mins % 60);
    } else {
        swprintf_s(line2, cch2, L"使用电池");
    }
}

static void FormatClockLines(wchar_t* line1, size_t cch1, wchar_t* line2, size_t cch2) {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    if (g_settings.clock12Hour) {
        unsigned h = st.wHour % 12;
        if (h == 0) h = 12;
        const wchar_t* ap = st.wHour < 12 ? L"上午" : L"下午";
        if (g_settings.clockShowSeconds) {
            swprintf_s(line1, cch1, L"%s %u:%02u:%02u", ap, h, st.wMinute, st.wSecond);
        } else {
            swprintf_s(line1, cch1, L"%s %u:%02u", ap, h, st.wMinute);
        }
    } else if (g_settings.clockShowSeconds) {
        swprintf_s(line1, cch1, L"%02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);
    } else {
        swprintf_s(line1, cch1, L"%02u:%02u", st.wHour, st.wMinute);
    }
    swprintf_s(line2, cch2, L"%04u-%02u-%02u  %s", st.wYear, st.wMonth, st.wDay,
               kWeekLong[st.wDayOfWeek % 7]);
}

static void FormatWidgetLines(WidgetKind kind, wchar_t* line1, size_t cch1, wchar_t* line2,
                              size_t cch2) {
    line1[0] = 0;
    line2[0] = 0;
    switch (kind) {
        case WidgetKind::Clock:
            FormatClockLines(line1, cch1, line2, cch2);
            break;
        case WidgetKind::Status: {
            const wchar_t* t = g_settings.statusText.empty() ? kDefaultStatusText
                                                            : g_settings.statusText.c_str();
            wcsncpy_s(line1, cch1, t, _TRUNCATE);
            break;
        }
        case WidgetKind::Weather:
            swprintf_s(line1, cch1, L"%s", g_settings.weatherCity.c_str());
            swprintf_s(line2, cch2, L"%s · 演示", g_settings.weatherDemoText.c_str());
            break;
        case WidgetKind::Calendar: {
            SYSTEMTIME st{};
            GetLocalTime(&st);
            swprintf_s(line1, cch1, L"%u", st.wDay);
            swprintf_s(line2, cch2, L"%04u年%u月 · 周%s", st.wYear, st.wMonth,
                       kWeek[st.wDayOfWeek % 7]);
            break;
        }
        case WidgetKind::System:
            ReadBattery(line1, cch1, line2, cch2);
            break;
        case WidgetKind::Countdown: {
            SYSTEMTIME target{};
            if (!ParseYmd(g_settings.countdownDate, &target)) {
                swprintf_s(line1, cch1, L"%s", g_settings.countdownLabel.c_str());
                swprintf_s(line2, cch2, L"日期无效");
            } else {
                const int days = DaysUntil(target);
                swprintf_s(line1, cch1, L"%s", g_settings.countdownLabel.c_str());
                if (days > 0) {
                    swprintf_s(line2, cch2, L"还有 %d 天", days);
                } else if (days == 0) {
                    swprintf_s(line2, cch2, L"就是今天");
                } else {
                    swprintf_s(line2, cch2, L"已过 %d 天", -days);
                }
            }
            break;
        }
        case WidgetKind::Sticky:
            wcsncpy_s(line1, cch1, g_settings.stickyText.c_str(), _TRUNCATE);
            break;
        default:
            break;
    }
}

static void RebuildFonts() {
    if (g_fontLarge) {
        DeleteObject(g_fontLarge);
        g_fontLarge = nullptr;
    }
    if (g_fontSmall) {
        DeleteObject(g_fontSmall);
        g_fontSmall = nullptr;
    }
    g_fontLarge = CreateFontW(
        -g_settings.fontSize, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI Variable Display");
    if (!g_fontLarge) {
        g_fontLarge = CreateFontW(
            -g_settings.fontSize, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS,
            L"Segoe UI");
    }
    int smallSize = Clamp(g_settings.fontSize * 45 / 100, 12, 28);
    g_fontSmall = CreateFontW(-smallSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
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

    const int tint = static_cast<int>(0x99232323);

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
    SetLayeredWindowAttributes(hwnd, 0, 200, LWA_ALPHA);
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

static void EnsureCardGdi() {
    if (!g_brushCardBg) {
        g_brushCardBg = CreateSolidBrush(RGB(0x23, 0x23, 0x23));
    }
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

static void MeasureTwoLine(HDC hdc, const wchar_t* line1, const wchar_t* line2, bool largeFirst,
                           int* outW, int* outH) {
    SIZE s1{}, s2{};
    HFONT old = static_cast<HFONT>(SelectObject(hdc, largeFirst ? g_fontLarge : g_fontSmall));
    GetTextExtentPoint32W(hdc, line1, static_cast<int>(wcslen(line1)), &s1);
    SelectObject(hdc, g_fontSmall);
    if (line2 && line2[0]) {
        GetTextExtentPoint32W(hdc, line2, static_cast<int>(wcslen(line2)), &s2);
    }
    SelectObject(hdc, old);
    *outW = (s1.cx > s2.cx ? s1.cx : s2.cx) + kPad * 2;
    *outH = s1.cy + (line2 && line2[0] ? s2.cy + 8 : 0) + kPad * 2;
    *outW = Clamp(*outW, 160, 520);
    *outH = Clamp(*outH, 56, 260);
}

static void MeasureWidget(HDC hdc, WidgetKind kind, int* outW, int* outH) {
    wchar_t l1[96]{}, l2[160]{};
    FormatWidgetLines(kind, l1, 96, l2, 160);
    const bool largeFirst =
        kind == WidgetKind::Clock || kind == WidgetKind::Calendar || kind == WidgetKind::Countdown;
    if (kind == WidgetKind::Status || kind == WidgetKind::Sticky) {
        SIZE s{};
        HFONT old = static_cast<HFONT>(SelectObject(hdc, g_fontSmall));
        GetTextExtentPoint32W(hdc, l1, static_cast<int>(wcslen(l1)), &s);
        SelectObject(hdc, old);
        *outW = Clamp(s.cx + kPad * 2, 160, 520);
        *outH = Clamp(s.cy + kPad * 2 + 12, 56, 200);
        return;
    }
    MeasureTwoLine(hdc, l1, l2, largeFirst, outW, outH);
}

static void PaintWidget(HWND hwnd, Widget* w) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc{};
    GetClientRect(hwnd, &rc);

    EnsureCardGdi();
    FillRect(hdc, &rc, g_brushCardBg);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0xF2, 0xF2, 0xF2));

    wchar_t line1[96]{}, line2[160]{};
    FormatWidgetLines(w->kind, line1, 96, line2, 160);

    const bool wordWrap = w->kind == WidgetKind::Status || w->kind == WidgetKind::Sticky;
    const bool largeFirst =
        w->kind == WidgetKind::Clock || w->kind == WidgetKind::Calendar ||
        w->kind == WidgetKind::Countdown;

    if (wordWrap) {
        RECT r = rc;
        InflateRect(&r, -kPad, -kPad);
        HFONT old = static_cast<HFONT>(SelectObject(hdc, g_fontSmall));
        DrawTextW(hdc, line1, -1, &r, DT_LEFT | DT_VCENTER | DT_WORDBREAK);
        SelectObject(hdc, old);
    } else {
        RECT r1 = rc;
        r1.top += kPad;
        r1.left += kPad;
        r1.right -= kPad;
        HFONT old = static_cast<HFONT>(SelectObject(hdc, largeFirst ? g_fontLarge : g_fontSmall));
        DrawTextW(hdc, line1, -1, &r1, DT_LEFT | DT_TOP | DT_SINGLELINE);

        SIZE s1{};
        GetTextExtentPoint32W(hdc, line1, static_cast<int>(wcslen(line1)), &s1);
        if (line2[0]) {
            RECT r2 = rc;
            r2.top = r1.top + s1.cy + 6;
            r2.left += kPad;
            r2.right -= kPad;
            SelectObject(hdc, g_fontSmall);
            SetTextColor(hdc, RGB(0xC8, 0xC8, 0xC8));
            DrawTextW(hdc, line2, -1, &r2, DT_LEFT | DT_TOP | DT_SINGLELINE);
        }
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
            return 0;  // static demo
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
                PersistPosition(*w);
            }
            return 0;

        case WM_TIMER:
            if (wParam == kTimerRefresh && w) {
                wchar_t line1[96]{}, line2[160]{};
                FormatWidgetLines(w->kind, line1, 96, line2, 160);
                if (wcscmp(line1, w->lastLine1) != 0 || wcscmp(line2, w->lastLine2) != 0) {
                    wcscpy_s(w->lastLine1, line1);
                    wcscpy_s(w->lastLine2, line2);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            if (w) PaintWidget(hwnd, w);
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
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
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

    HDC screen = GetDC(nullptr);
    int width = 240;
    int height = 100;
    MeasureWidget(screen, w.kind, &width, &height);
    ReleaseDC(nullptr, screen);
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
    ApplyGlass(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    FormatWidgetLines(w.kind, w.lastLine1, 96, w.lastLine2, 160);
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

static void AddWidget(WidgetKind kind, int defX, int defY) {
    bool* flag = ShowFlagFor(kind);
    if (!flag || !*flag) return;

    Widget w{};
    w.kind = kind;
    w.x = defX;
    w.y = defY;
    LoadPosition(w);
    g_widgets.push_back(w);
    if (!CreateWidgetWindow(g_widgets.back())) {
        g_widgets.pop_back();
    }
}

static void EnsureWidgets() {
    DestroyAllWidgets();
    RebuildFonts();
    g_widgets.reserve(static_cast<size_t>(WidgetKind::Count));

    const int right = GetSystemMetrics(SM_CXSCREEN);
    const int col = right - 340;
    // Stack defaults down the right edge; HKCU overrides after first drag.
    AddWidget(WidgetKind::Clock, col, 40);
    AddWidget(WidgetKind::Calendar, col, 160);
    AddWidget(WidgetKind::Weather, col, 280);
    AddWidget(WidgetKind::System, col, 400);
    AddWidget(WidgetKind::Countdown, col, 520);
    AddWidget(WidgetKind::Status, col, 640);
    AddWidget(WidgetKind::Sticky, col, 740);

    RebindWidgetUserData();
    Wh_Log(L"[ZenWidgets] Active widgets: %zu (pack=%s describeEmpty=%d)", g_widgets.size(),
           g_settings.presetPack.c_str(), g_settings.aiDescribe.empty() ? 1 : 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L"[ZenWidgets] === Wh_ModInit v0.2.0 ===");
    LoadSettings();
    EnsureWidgets();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[ZenWidgets] === Wh_ModUninit ===");
    DestroyAllWidgets();
    DestroyCardGdi();
    if (g_fontLarge) {
        DeleteObject(g_fontLarge);
        g_fontLarge = nullptr;
    }
    if (g_fontSmall) {
        DeleteObject(g_fontSmall);
        g_fontSmall = nullptr;
    }
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
