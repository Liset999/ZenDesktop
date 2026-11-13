// ==WindhawkMod==
// @id              zen-ime-caret-hint
// @name            ZenDesktop: 光标输入法提示
// @name:zh-CN      ZenDesktop: 光标输入法提示
// @description     Doubao/macOS-like Chinese/English and Caps Lock capsule beside the text caret (pointer fallback), with Custom Glass.
// @description:zh-CN 像 Mac 豆包输入法一样：切换中英／大写时在文字光标旁短暂弹出提示胶囊（取不到光标则贴鼠标），并支持自定义玻璃。
// @version         0.2.9
// @author          Lanbo
// @github          https://github.com/Liset999
// @donateUrl       https://ifdian.net/a/Lanboo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -limm32 -lshcore -lkernel32 -ldwmapi -loleacc -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// Original ZenDesktop code. No third-party IME overlay sources copied.
// Design informed by ImTip (https://github.com/aardio/ImTip, MIT) ideas only:
// prefer a real text caret, fall back to the pointer for apps that expose none.
//
// API notes (Win10/11):
// - One Explorer-hosted instance owns the overlay. MSAA / UIA run in a
//   job-controlled query process with a hard deadline. Input hooks are isolated.
// - Caret: GetGUIThreadInfo → MSAA OBJID_CARET → thin UIA TextPattern caret →
//   mouse pointer.
// - IME: IMM conversion/open on the focus thread's default IME window; bare
//   Shift tap seeds/flips state when IMM is unreadable (QQ etc.).
// - Caps: GetKeyState(VK_CAPITAL).
// - Glass: screen-back box blur + tint (small click-through layered windows
//   cannot host reliable DWM acrylic). Same preset vocabulary as taskbar.

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

# ZenDesktop: 光标输入法提示

像 **Mac 豆包输入法** 一样：按 Shift 切换中／英，或切换大写锁定时，在**文字光标旁**短暂弹出玻璃胶囊；拿不到光标时退到鼠标旁。点击输入框时显示当前中英或大写状态；确认输入框但取不到准确光标时贴鼠标。

## 功能

- 输入法中英切换、大写锁定开／关 → 约 0.8 秒的「中／英」「大写」（或 En／A）提示。关闭大写锁定后显示当前中／英状态。
- 位置自动：优先文字光标，否则鼠标指针（类 macOS／豆包）。
- 中英与大写共用同一套自定义玻璃（色调／模糊／不透明度／亮度）。
- 点击穿透、不抢焦点；多显示器与 DPI 感知。
- 只注入 Explorer（`@include explorer.exe`），由它统一查询前台窗口。

## 设置说明

- **启用气泡**、**显示时长**、输入法／大写触发开关、中英提示是否附带大写。
- **自定义玻璃**：与其他 ZenDesktop mod 一致的预设。
- **优先用鼠标旁的窗口类名**（可选）：逗号分隔的普通类名；匹配时跳过光标查询，直接贴鼠标。
- **高级偏移**：相对光标的像素偏移（再乘 DPI）。

## 兼容性与已知问题

- Steam 客户端及 steamwebhelper 窗口暂停提示，避免跨进程输入法／无障碍查询引发卡顿。
- 小尺寸点击穿透分层窗无法稳定挂 DWM 亚克力；模糊为截屏盒模糊近似。
- Chromium／Electron／WinUI 等：保留 UIA 输入框自动提醒。查询在独立进程执行，超过 600ms 回收；确认输入框但无准确光标时贴鼠标。
- 以管理员身份运行的程序受 UIPI 限制时，读不到光标会退到鼠标旁。
- 部分聊天软件（QQ 等）不暴露 IMM 状态：光靠 Shift 轻点仍可翻转提示。

## 更新日志

- **0.2.9**：补上解析下拉数值用的 `<cwchar>`，修复 Windhawk 编译失败。选项和以前保存的数值不变。
- **0.2.8**：自定义玻璃的模糊、不透明度和亮度下拉改为字符串选项，Windhawk 2.0 设置页可以正常显示。以前保存的数值仍按原来的档位生效。
- **0.2.7**：保留输入框自动提醒，将无障碍查询隔离到限时进程，键鼠钩子使用独立线程；修复失败重试降频、旧定位结果、卸载等待及输入框左边缘误定位。
- **0.2.6**：Steam 窗口跳过输入法、MSAA 和 UIA 查询，并取消待显示提示。
- **0.2.4**：关闭大写锁定后显示当前中／英状态，开启时显示大写。
- **0.2.3**：气泡按实际尺寸居中显示在文字光标正上方，间距 6 像素；屏幕顶部空间不足时显示在下方。
- **0.2.2**：定位完成后一次显示，避免鼠标到输入框跳位；点击或聚焦输入框显示当前中英／大写状态。
- **0.2.1**：低级键盘钩子捕获 Shift 轻点，排除组合键；输入法旧状态兜底并等待真实状态校准；切换输入焦点时重置 IME 缓存。
- **0.2.0**：按 Mac 豆包体验重勾。默认只在中英／大写切换时提示，去掉「焦点进入时显示」。锚点改为单一自动路径（光标 → 鼠标）。删除形态 B 占位、三档锚点设置、ImTip 风格的 @／# 类名前缀。精简 UIA 与 pending 状态机；光标探测改为 GetGUIThreadInfo → MSAA → 薄 UIA → 指针。
- **0.1.6**：Explorer 单实例、Shift 轻点兜底、编辑框类名、焦点辅助事件等（已由 0.2.3 收敛）。
- **0.1.5**：应用未提供 MSAA 光标时尝试 UIA 文本范围。
- **0.1.1**：自定义玻璃；Caps Lock 提示；屏幕背板模糊近似。
- **0.1.0**：形态 A 气泡；IMM／布局中英检测。

## 致谢与许可

- 开发者：Lanbo（[哔哩哔哩](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo)）
- 本文件为 ZenDesktop 原创实现；「光标优先 + 指针兜底」思路参考了 MIT 协议的 [ImTip](https://github.com/aardio/ImTip)，未复制其代码。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- bubbleEnabled: true
  $name: "Enable caret bubble"
  $name:zh-CN: "启用气泡提示"
  $description: "Show a short capsule beside the caret/pointer. Default on."
  $description:zh-CN: "在光标或鼠标旁短暂显示提示胶囊。默认开启。"
- durationMs: 800
  $name: "Duration (ms)"
  $name:zh-CN: "显示时长（毫秒）"
  $description: "How long the bubble stays, including fade. Range 200–3000. Default 800."
  $description:zh-CN: "气泡停留时长（含淡出），范围 200–3000。默认 800。"
- showOnImeToggle: true
  $name: "Show when IME toggles"
  $name:zh-CN: "输入法切换时显示"
  $description: "Show when Chinese/English mode or keyboard layout changes. Default on."
  $description:zh-CN: "中英模式或键盘布局变化时显示。默认开启。"
- showOnCapsLock: true
  $name: "Show when Caps Lock toggles"
  $name:zh-CN: "大写锁定切换时显示"
  $description: "Show when Caps Lock turns on or off. Default on."
  $description:zh-CN: "按下 Caps Lock 开／关时显示。默认开启。"
- englishLabel: zh
  $name: "English label"
  $name:zh-CN: "英文标签"
  $options:
  - zh: "英"
  - en: "En"
  $options:zh-CN:
  - zh: "英"
  - en: "En"
  $description: "Label for English IME mode. Default 英."
  $description:zh-CN: "英文输入模式显示的文字。默认为「英」。"
- capsLabel: zh
  $name: "Caps Lock label"
  $name:zh-CN: "大写标签"
  $options:
  - zh: "大写"
  - en: "A"
  $options:zh-CN:
  - zh: "大写"
  - en: "A"
  $description: "Label when Caps Lock turns on. Turning it off shows the current Chinese/English mode."
  $description:zh-CN: "开启大写锁定时显示「大写」或「A」；关闭后显示当前「中／英」。"
- combineCapsWithIme: true
  $name: "Append caps when showing IME"
  $name:zh-CN: "中英提示附带大写"
  $description: "When Caps Lock is on, IME bubbles show e.g. 中 · 大写. Default on."
  $description:zh-CN: "大写开启时，中英气泡显示如「中 · 大写」。默认开启。"
- editorClasses: "ChatWnd,WeChatMainWndForPC,EXCEL6,ConsoleWindowClass,AVL_AVView"
  $name: "Prefer pointer for window classes"
  $name:zh-CN: "优先用鼠标旁的窗口类名"
  $description: "Comma-separated plain class names. Matching focus windows skip caret lookup and place the bubble next to the mouse (QQ/WeChat/Excel/console defaults)."
  $description:zh-CN: "逗号分隔的普通窗口类名。匹配时跳过光标查询，直接贴在鼠标旁。默认含 QQ、微信、Excel、终端等。"
- glass:
    - bgColorMode: Default
      $name: "Background color"
      $name:zh-CN: "背景颜色"
      $description: "Base tint of the bubble glass. Default uses mist grey."
      $description:zh-CN: "气泡玻璃的基础色调。选「主题默认」时使用雾灰。"
      $options:
        - Default: "主题默认"
        - Accent: "系统强调色"
        - GlassWhite: "透白"
        - MistGrey: "雾灰"
        - DeepBlack: "深黑"
        - OceanBlue: "海蓝"
        - AuroraCyan: "极光青"
        - RosePink: "玫瑰粉"
        - BordeauxRed: "酒红"
        - ForestGreen: "森林绿"
        - RoyalPurple: "皇家紫"
        - SunsetOrange: "落日橙"
        - ChampagneGold: "香槟金"
        - MorandiSage: "莫兰迪绿"
    - blurPreset: "30"
      $name: "Blur"
      $name:zh-CN: "模糊"
      $description: "Background blur strength, 0–60. Larger is blurrier."
      $description:zh-CN: "背景模糊强度，范围 0–60，数值越大越模糊。"
      $options:
        - "0": "0：不模糊"
        - "10": "10：轻微"
        - "20": "20：中等"
        - "30": "30：较强"
        - "45": "45：强"
        - "60": "60：最强"
    - opacityPreset: "50"
      $name: "Opacity"
      $name:zh-CN: "不透明度"
      $description: "Fill strength of the background color, 0–100%. Default 50%."
      $description:zh-CN: "背景颜色的填充浓度，范围 0–100%。默认 50%。"
      $options:
        - "0": "0%：完全透明"
        - "10": "10%：很薄"
        - "25": "25%：较薄"
        - "50": "50%：适中"
        - "75": "75%：较厚"
        - "90": "90%：很厚"
        - "100": "100%：完全不透明"
    - luminosityPreset: "20"
      $name: "Luminosity"
      $name:zh-CN: "亮度"
      $description: "Luminosity layer strength, 0–150. Default 20."
      $description:zh-CN: "背景亮度层的强度，范围 0–150。默认 20。"
      $options:
        - "0": "0：最低"
        - "20": "20：低"
        - "25": "25：较低"
        - "50": "50：中"
        - "75": "75：较高"
        - "100": "100：标准"
        - "125": "125：高"
        - "150": "150：最高"
  $name: "Custom glass"
  $name:zh-CN: "自定义玻璃"
  $description: "Shared glass for 中/英/Caps bubbles. Same presets as other ZenDesktop mods."
  $description:zh-CN: "中／英／大写气泡共用的玻璃。选项与其他 ZenDesktop mod 一致。"
- anchorOffsetX: 0
  $name: "Advanced: X offset (px)"
  $name:zh-CN: "高级：水平偏移（像素）"
  $description: "Horizontal adjustment from centered-above-caret position. Default 0."
  $description:zh-CN: "相对光标正上方居中位置的水平微调。默认 0，正值向右。"
- anchorOffsetY: 0
  $name: "Advanced: Y offset (px)"
  $name:zh-CN: "高级：垂直偏移（像素）"
  $description: "Vertical adjustment above caret, with a 6px gap. Default 0."
  $description:zh-CN: "默认在光标上方留 6 像素间距。微调默认 0，负值上移。"
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#include <imm.h>
#include <oleacc.h>
#include <uiautomationclient.h>
#include <wrl/client.h>
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cstring>
#include <cwchar>  // wcstol for decimal preset strings
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#ifndef GUI_CARETBLINKING
#define GUI_CARETBLINKING 0x00000001
#endif

namespace {

constexpr UINT_PTR kPollTimerId = 0x1CE1;
constexpr UINT kPollIntervalMs = 50;
constexpr UINT_PTR kFadeTimerId = 0x1CE2;
constexpr UINT kFadeIntervalMs = 16;
constexpr ULONGLONG kLangRefreshMs = 200;
constexpr ULONGLONG kLangFailRetryMs = 1000;
constexpr ULONGLONG kCaretExpiryMs = 600;
constexpr UINT kKeyboardMessage = WM_APP + 1;
constexpr UINT kMouseMessage = WM_APP + 2;
constexpr ULONGLONG kDuplicateShowMs = 700;
constexpr ULONGLONG kShiftTapMaxMs = 1200;
constexpr ULONGLONG kShiftSettleMs = 80;
HHOOK g_keyboardHook = nullptr;
HHOOK g_mouseHook = nullptr;
ULONGLONG g_clickReadyAt = 0;
bool g_clickPending = false;
bool g_focusHintPending = false;
constexpr wchar_t kOverlayClass[] = L"ZenImeCaretHintOverlay";
constexpr wchar_t kHostClass[] = L"ZenImeCaretHintHost";
constexpr wchar_t kDefaultEditorClasses[] =
    L"ChatWnd,WeChatMainWndForPC,EXCEL6,ConsoleWindowClass,AVL_AVView";

enum class HintLang { English, Chinese };

struct GlassSettings {
    std::wstring bgColorMode = L"Default";
    int blurPreset = 30;
    int opacityPreset = 50;
    int luminosityPreset = 20;
};

struct Settings {
    bool bubbleEnabled = true;
    bool showOnImeToggle = true;
    bool showOnCapsLock = true;
    bool combineCapsWithIme = true;
    bool englishUseEn = false;
    bool capsUseEn = false;
    int durationMs = 800;
    int anchorOffsetX = 0;
    int anchorOffsetY = 0;
    std::wstring editorClasses;
    GlassSettings glass;
};

Settings g_settings;
HWND g_hostWnd = nullptr;
HWND g_overlayWnd = nullptr;
HINSTANCE g_hInstance = nullptr;
HANDLE g_hostThread = nullptr;
HANDLE g_inputThread = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_singleInstance = nullptr;
std::atomic<bool> g_settingsChanged{false};

struct CaretPacket {
    DWORD size = sizeof(CaretPacket);
    HWND focus = nullptr;
    bool requireEditor = false;
    bool editor = false;
    bool hasRect = false;
    RECT rect{};
};
struct PendingShow {
    HWND focus = nullptr;
    HWND foreground = nullptr;
    ULONGLONG generation = 0;
    ULONGLONG inputGeneration = 0;
    std::wstring text;
    ULONGLONG startedAt = 0;
    bool requireEditor = false;
};

HANDLE g_caretProcess = nullptr;
HANDLE g_caretJob = nullptr;
HANDLE g_caretMapping = nullptr;
CaretPacket* g_caretPacket = nullptr;
ULONGLONG g_nextCaretRequestId = 0;
ULONGLONG g_focusGeneration = 0;
std::atomic<ULONGLONG> g_inputGeneration{0};
std::optional<PendingShow> g_pendingShow;

// Only the host thread owns the query handles. Closing the job ends even a
// provider stuck in a synchronous COM call, without unloading executing code.
void CancelCaretQuery() {
    if (g_caretJob) {
        CloseHandle(g_caretJob);
        g_caretJob = nullptr;
    }
    if (g_caretProcess) {
        CloseHandle(g_caretProcess);
        g_caretProcess = nullptr;
    }
    if (g_caretPacket) {
        UnmapViewOfFile(g_caretPacket);
        g_caretPacket = nullptr;
    }
    if (g_caretMapping) {
        CloseHandle(g_caretMapping);
        g_caretMapping = nullptr;
    }
    g_pendingShow.reset();
}

std::atomic<ULONGLONG> g_logicalFocusTick{0};
ULONGLONG g_lastLogicalFocusTick = 0;

HWND g_cachedForeground = nullptr;
HWND g_cachedFocus = nullptr;
HWND g_cachedImeOwner = nullptr;
bool g_langQueryFailing = false;
bool g_shiftDown = false;
ULONGLONG g_shiftDownAt = 0;
bool g_shiftUsedInChord = false;
HWND g_shiftForeground = nullptr;
unsigned g_pendingShiftTaps = 0;
ULONGLONG g_shiftReadyAt = 0;
HWND g_pendingShiftForeground = nullptr;
std::optional<HintLang> g_shiftBaseline;
std::optional<HintLang> g_staleImeLang;

struct ShownBubble {
    std::wstring text;
    RECT caret{};
    ULONGLONG at = 0;
};
ShownBubble g_lastShown;

struct BackdropCache {
    bool valid = false;
    int width = 0;
    int height = 0;
    int originX = 0;
    int originY = 0;
    std::wstring text;
    std::vector<BYTE> pixels;
};
BackdropCache g_backdrop;
int g_lastPaintedAlpha = -1;

bool g_bubbleVisible = false;
ULONGLONG g_bubbleShownAt = 0;
std::wstring g_bubbleText;
int g_bubbleDpi = 96;
RECT g_bubbleCaret{};

HWND g_lastFocus = nullptr;
HWND g_lastForeground = nullptr;
bool g_hasLastLang = false;
HintLang g_lastLang = HintLang::English;
ULONGLONG g_lastLangQueryTick = 0;
bool g_hasLastCaps = false;
bool g_lastCaps = false;

#define ZIH_LOG(fmt, ...) Wh_Log(L"[zen-ime-caret-hint] " fmt, ##__VA_ARGS__)

int ClampInt(int v, int lo, int hi) {
    return (std::max)(lo, (std::min)(hi, v));
}

// Dropdown presets are strings so Windhawk 2.0 accepts $options. Older saves
// stored the same choice as a DWORD. A complete decimal string is that choice;
// otherwise the legacy integer is used (0 when the value is missing).
int ReadPresetInt(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    int parsed = 0;
    bool fromText = false;
    if (value && *value) {
        wchar_t* end = nullptr;
        long number = wcstol(value, &end, 10);
        if (end != value && *end == L'\0') {
            parsed = static_cast<int>(number);
            fromText = true;
        }
    }
    Wh_FreeStringSetting(value);
    return fromText ? parsed : Wh_GetIntSetting(name);
}

void LoadSettings() {
    g_settings.bubbleEnabled = Wh_GetIntSetting(L"bubbleEnabled") != 0;
    g_settings.showOnImeToggle = Wh_GetIntSetting(L"showOnImeToggle") != 0;
    g_settings.showOnCapsLock = Wh_GetIntSetting(L"showOnCapsLock") != 0;
    g_settings.combineCapsWithIme = Wh_GetIntSetting(L"combineCapsWithIme") != 0;
    g_settings.durationMs = ClampInt(Wh_GetIntSetting(L"durationMs"), 200, 3000);
    g_settings.anchorOffsetX = Wh_GetIntSetting(L"anchorOffsetX");
    g_settings.anchorOffsetY = Wh_GetIntSetting(L"anchorOffsetY");

    g_settings.englishUseEn = false;
    if (PCWSTR label = Wh_GetStringSetting(L"englishLabel")) {
        g_settings.englishUseEn = _wcsicmp(label, L"en") == 0;
        Wh_FreeStringSetting(label);
    }
    g_settings.capsUseEn = false;
    if (PCWSTR caps = Wh_GetStringSetting(L"capsLabel")) {
        g_settings.capsUseEn = _wcsicmp(caps, L"en") == 0;
        Wh_FreeStringSetting(caps);
    }

    g_settings.editorClasses.clear();
    if (PCWSTR classes = Wh_GetStringSetting(L"editorClasses")) {
        g_settings.editorClasses = classes;
        Wh_FreeStringSetting(classes);
    }
    if (g_settings.editorClasses.empty()) {
        g_settings.editorClasses = kDefaultEditorClasses;
    }

    g_settings.glass.bgColorMode = L"Default";
    if (PCWSTR color = Wh_GetStringSetting(L"glass.bgColorMode")) {
        if (*color) {
            g_settings.glass.bgColorMode = color;
        }
        Wh_FreeStringSetting(color);
    }
    g_settings.glass.blurPreset =
        ClampInt(ReadPresetInt(L"glass.blurPreset"), 0, 60);
    g_settings.glass.opacityPreset =
        ClampInt(ReadPresetInt(L"glass.opacityPreset"), 0, 100);
    g_settings.glass.luminosityPreset =
        ClampInt(ReadPresetInt(L"glass.luminosityPreset"), 0, 150);
}

int GetWindowDpiSafe(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static auto pGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    if (pGetDpiForWindow && hwnd && IsWindow(hwnd)) {
        UINT dpi = pGetDpiForWindow(hwnd);
        if (dpi > 0) {
            return static_cast<int>(dpi);
        }
    }
    HDC hdc = GetDC(nullptr);
    int dpi = hdc ? GetDeviceCaps(hdc, LOGPIXELSX) : 96;
    if (hdc) {
        ReleaseDC(nullptr, hdc);
    }
    return dpi > 0 ? dpi : 96;
}

int ScaleForDpi(int px, int dpi) {
    return MulDiv(px, dpi, 96);
}

int GetDpiForPointSafe(POINT pt) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
        GetProcAddress(GetModuleHandleW(L"shcore.dll"), "GetDpiForMonitor"));
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (pGetDpiForMonitor && monitor) {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(pGetDpiForMonitor(monitor, 0, &dpiX, &dpiY)) && dpiX) {
            return static_cast<int>(dpiX);
        }
    }
    HWND under = WindowFromPoint(pt);
    return GetWindowDpiSafe(under ? under : GetForegroundWindow());
}

bool ContainsNoCase(const wchar_t* haystack, const std::wstring& needle) {
    if (!haystack || needle.empty()) {
        return false;
    }
    const size_t length = wcslen(haystack);
    if (needle.size() > length) {
        return false;
    }
    for (size_t i = 0; i + needle.size() <= length; ++i) {
        if (_wcsnicmp(haystack + i, needle.c_str(), needle.size()) == 0) {
            return true;
        }
    }
    return false;
}

bool PreferPointerForClass(const wchar_t* className) {
    const std::wstring& list = g_settings.editorClasses;
    if (!className || list.empty()) {
        return false;
    }
    size_t start = 0;
    while (start <= list.size()) {
        const size_t comma = list.find_first_of(L",;", start);
        std::wstring token = list.substr(
            start, comma == std::wstring::npos ? std::wstring::npos : comma - start);
        const size_t first = token.find_first_not_of(L" \t@#");
        const size_t last = token.find_last_not_of(L" \t");
        if (first != std::wstring::npos) {
            token = token.substr(first, last - first + 1);
            if (!token.empty() && ContainsNoCase(className, token)) {
                return true;
            }
        }
        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }
    return false;
}

bool FocusPrefersPointer(HWND focus) {
    wchar_t name[64]{};
    for (HWND window = focus; window; window = GetParent(window)) {
        if (!GetClassNameW(window, name, ARRAYSIZE(name))) {
            break;
        }
        if (PreferPointerForClass(name)) {
            return true;
        }
        if (GetAncestor(window, GA_ROOT) == window) {
            break;
        }
    }
    return false;
}

bool IsSteamWindow(HWND hwnd) {
    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId) || !processId) {
        return false;
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }
    wchar_t path[MAX_PATH];
    DWORD length = ARRAYSIZE(path);
    const BOOL queried = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    if (!queried) {
        return false;
    }
    const wchar_t* separator = wcsrchr(path, L'\\');
    const wchar_t* name = separator ? separator + 1 : path;
    return _wcsicmp(name, L"steam.exe") == 0 ||
           _wcsicmp(name, L"steamwebhelper.exe") == 0;
}

bool QueryAccessibleCaret(HWND hwnd, RECT* rect) {
    if (IsSteamWindow(hwnd)) {
        return false;
    }
    IAccessible* caret = nullptr;
    if (FAILED(AccessibleObjectFromWindow(hwnd, OBJID_CARET, IID_IAccessible,
                                          reinterpret_cast<void**>(&caret))) ||
        !caret) {
        return false;
    }
    VARIANT self{};
    self.vt = VT_I4;
    self.lVal = CHILDID_SELF;
    LONG x = 0, y = 0, width = 0, height = 0;
    HRESULT result = caret->accLocation(&x, &y, &width, &height, self);
    caret->Release();
    if (FAILED(result) || height <= 0 || width < 0) {
        return false;
    }
    *rect = {x, y, x + (std::max)(width, 1L), y + height};
    return true;
}

BOOL CALLBACK FindChromiumCaret(HWND hwnd, LPARAM param) {
    wchar_t className[64]{};
    if (IsWindowVisible(hwnd) &&
        GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        wcscmp(className, L"Chrome_RenderWidgetHostHWND") == 0 &&
        QueryAccessibleCaret(hwnd, reinterpret_cast<RECT*>(param))) {
        return FALSE;
    }
    return TRUE;
}

bool QueryCaretScreenRect(HWND focus, const GUITHREADINFO& gui, RECT* outRect,
                          const wchar_t** methodOut) {
    if (gui.hwndCaret && gui.rcCaret.bottom > gui.rcCaret.top &&
        gui.rcCaret.right >= gui.rcCaret.left) {
        RECT rc = gui.rcCaret;
        if (rc.right == rc.left) {
            ++rc.right;
        }
        MapWindowPoints(gui.hwndCaret, nullptr, reinterpret_cast<POINT*>(&rc), 2);
        *outRect = rc;
        *methodOut = L"GetGUIThreadInfo.rcCaret";
        return true;
    }
    if (QueryAccessibleCaret(focus, outRect)) {
        *methodOut = L"MSAA caret";
        return true;
    }
    SetRectEmpty(outRect);
    HWND root = GetAncestor(focus, GA_ROOT);
    if (root) {
        EnumChildWindows(root, FindChromiumCaret, reinterpret_cast<LPARAM>(outRect));
    }
    *methodOut = L"Chromium MSAA caret";
    return !IsRectEmpty(outRect);
}

using Microsoft::WRL::ComPtr;

bool ReadUiaCaretBounds(IUIAutomationTextRange* range, bool trailingEdge, RECT* rect) {
    SAFEARRAY* bounds = nullptr;
    const HRESULT hr = range->GetBoundingRectangles(&bounds);
    const std::unique_ptr<SAFEARRAY, decltype(&SafeArrayDestroy)> owner(bounds,
                                                                      SafeArrayDestroy);
    VARTYPE type = VT_EMPTY;
    if (FAILED(hr) || !bounds || SafeArrayGetDim(bounds) != 1 ||
        bounds->rgsabound[0].cElements != 4 ||
        FAILED(SafeArrayGetVartype(bounds, &type)) || type != VT_R8) {
        return false;
    }
    void* data = nullptr;
    if (FAILED(SafeArrayAccessData(bounds, &data))) {
        return false;
    }
    const auto* values = static_cast<const double*>(data);
    const double x = values[0] + (trailingEdge ? values[2] : 0);
    const double y = values[1];
    const double bottom = y + values[3];
    SafeArrayUnaccessData(bounds);
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(bottom) ||
        x < LONG_MIN || x >= LONG_MAX || y < LONG_MIN || bottom > LONG_MAX ||
        bottom - y < 1) {
        return false;
    }
    const LONG left = static_cast<LONG>(std::floor(x));
    *rect = {left, static_cast<LONG>(std::floor(y)), left + 1,
             static_cast<LONG>(std::ceil(bottom))};
    return true;
}

ComPtr<IUIAutomationTextRange> GetUiaCaretRange(IUIAutomationElement* element) {
    ComPtr<IUIAutomationTextRange> range;
    ComPtr<IUIAutomationTextPattern2> text2;
    BOOL active = FALSE;
    if (SUCCEEDED(element->GetCurrentPatternAs(UIA_TextPattern2Id,
                                               IID_PPV_ARGS(&text2))) &&
        text2 && SUCCEEDED(text2->GetCaretRange(&active, &range)) && active && range) {
        return range;
    }
    ComPtr<IUIAutomationTextPattern> text;
    ComPtr<IUIAutomationTextRangeArray> selection;
    int count = 0;
    int comparison = 0;
    if (FAILED(element->GetCurrentPatternAs(UIA_TextPatternId, IID_PPV_ARGS(&text))) ||
        !text || FAILED(text->GetSelection(&selection)) || !selection ||
        FAILED(selection->get_Length(&count)) || count != 1 ||
        FAILED(selection->GetElement(0, range.ReleaseAndGetAddressOf())) || !range ||
        FAILED(range->CompareEndpoints(TextPatternRangeEndpoint_Start, range.Get(),
                                       TextPatternRangeEndpoint_End, &comparison)) ||
        comparison != 0) {
        return {};
    }
    return range;
}

bool TryCaretOnElement(IUIAutomationElement* element, RECT* rect) {
    if (!element) {
        return false;
    }
    ComPtr<IUIAutomationTextRange> range = GetUiaCaretRange(element);
    if (!range) {
        return false;
    }
    if (ReadUiaCaretBounds(range.Get(), false, rect)) {
        return true;
    }
    for (bool previous : {false, true}) {
        ComPtr<IUIAutomationTextRange> adjacent;
        int moved = 0;
        const auto endpoint = previous ? TextPatternRangeEndpoint_Start
                                       : TextPatternRangeEndpoint_End;
        if (SUCCEEDED(range->Clone(&adjacent)) && adjacent &&
            SUCCEEDED(adjacent->MoveEndpointByUnit(endpoint, TextUnit_Character,
                                                   previous ? -1 : 1, &moved)) &&
            moved != 0 && ReadUiaCaretBounds(adjacent.Get(), previous, rect)) {
            return true;
        }
    }
    return false;
}

std::optional<RECT> QueryUiaCaret(IUIAutomation* automation, HWND focus,
                                  bool requireEditor, bool* editorOut) {
    // Queued work can outlive a foreground change to Steam.
    if (IsSteamWindow(focus) || IsSteamWindow(GetForegroundWindow())) {
        return std::nullopt;
    }
    GUITHREADINFO gui{sizeof(gui)};
    if (!GetGUIThreadInfo(0, &gui)) {
        return std::nullopt;
    }
    if (gui.hwndFocus && gui.hwndFocus != focus) {
        return std::nullopt;
    }

    RECT rect{};
    ComPtr<IUIAutomationElement> focused;
    if (SUCCEEDED(automation->GetFocusedElement(&focused)) && focused) {
        CONTROLTYPEID type = 0;
        focused->get_CurrentControlType(&type);
        const bool editor = type == UIA_EditControlTypeId ||
                            static_cast<bool>(GetUiaCaretRange(focused.Get()));
        *editorOut = editor;
        if (requireEditor && !editor) {
            return std::nullopt;
        }
        if (TryCaretOnElement(focused.Get(), &rect)) {
            return rect;
        }
    } else if (requireEditor) {
        return std::nullopt;
    }
    HWND candidates[]{focus, GetAncestor(focus, GA_ROOT)};
    for (HWND hwnd : candidates) {
        ComPtr<IUIAutomationElement> root;
        if (hwnd && SUCCEEDED(automation->ElementFromHandle(hwnd, &root)) && root &&
            TryCaretOnElement(root.Get(), &rect)) {
            return rect;
        }
    }
    return std::nullopt;
}

bool RequestUiaCaret(HWND focus, const std::wstring& text, bool requireEditor = false) {
    CancelCaretQuery();
    if (IsSteamWindow(focus) || IsSteamWindow(GetForegroundWindow())) {
        return false;
    }
    wchar_t modulePath[32768]{};
    wchar_t systemPath[MAX_PATH]{};
    if (!GetModuleFileNameW(g_hInstance, modulePath, ARRAYSIZE(modulePath)) ||
        !GetSystemDirectoryW(systemPath, ARRAYSIZE(systemPath))) {
        return false;
    }
    const std::wstring mappingName = L"Local\\ZenCaretQuery-" +
        std::to_wstring(GetCurrentProcessId()) + L"-" +
        std::to_wstring(++g_nextCaretRequestId);
    g_caretMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
                                       0, sizeof(CaretPacket), mappingName.c_str());
    if (!g_caretMapping) {
        return false;
    }
    g_caretPacket = static_cast<CaretPacket*>(MapViewOfFile(
        g_caretMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CaretPacket)));
    if (!g_caretPacket) {
        CancelCaretQuery();
        return false;
    }
    *g_caretPacket = CaretPacket{};
    g_caretPacket->focus = focus;
    g_caretPacket->requireEditor = requireEditor;
    g_caretJob = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!g_caretJob || !SetInformationJobObject(g_caretJob,
            JobObjectExtendedLimitInformation, &limits, sizeof(limits))) {
        CancelCaretQuery();
        return false;
    }
    const std::wstring executable = std::wstring(systemPath) + L"\\rundll32.exe";
    std::wstring command = L"\"" + executable + L"\" \"" + modulePath +
        L"\",ZenCaretQuery " + mappingName;
    // The mod imports windhawk.dll from the engine directory. A plain
    // rundll32 process does not inherit Explorer's loaded DLL search state.
    wchar_t enginePath[32768]{};
    HMODULE engine = GetModuleHandleW(L"windhawk.dll");
    if (engine && !GetModuleFileNameW(engine, enginePath, ARRAYSIZE(enginePath))) {
        CancelCaretQuery();
        return false;
    }
    std::vector<wchar_t> environment;
    LPWCH inherited = GetEnvironmentStringsW();
    if (!inherited) {
        CancelCaretQuery();
        return false;
    }
    for (const wchar_t* entry = inherited; *entry; entry += wcslen(entry) + 1) {
        if (_wcsnicmp(entry, L"PATH=", 5) != 0) {
            environment.insert(environment.end(), entry, entry + wcslen(entry) + 1);
        }
    }
    FreeEnvironmentStringsW(inherited);
    const DWORD pathLength = GetEnvironmentVariableW(L"PATH", nullptr, 0);
    std::vector<wchar_t> inheritedPath(pathLength + 1);
    GetEnvironmentVariableW(L"PATH", inheritedPath.data(), inheritedPath.size());
    std::wstring searchPath = L"PATH=";
    wchar_t* separator = wcsrchr(enginePath, L'\\');
    if (separator) {
        *separator = 0;
        searchPath += std::wstring(enginePath) + L";";
    }
    searchPath += inheritedPath.data();
    environment.insert(environment.end(), searchPath.begin(), searchPath.end());
    environment.push_back(0);
    environment.push_back(0);
    STARTUPINFOW startup{sizeof(startup)};
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(executable.c_str(), command.data(), nullptr, nullptr, FALSE,
                        CREATE_SUSPENDED | CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT,
                        environment.data(), nullptr,
                        &startup, &process)) {
        CancelCaretQuery();
        return false;
    }
    g_caretProcess = process.hProcess;
    const bool assigned = AssignProcessToJobObject(g_caretJob, process.hProcess) != FALSE;
    const bool resumed = assigned && ResumeThread(process.hThread) != static_cast<DWORD>(-1);
    if (!resumed) {
        // The process is still suspended and has never run provider code.
        TerminateProcess(process.hProcess, ERROR_CANCELLED);
    }
    CloseHandle(process.hThread);
    if (!resumed) {
        CancelCaretQuery();
        return false;
    }
    g_pendingShow = PendingShow{focus, GetForegroundWindow(), g_focusGeneration,
                                g_inputGeneration.load(), text, GetTickCount64(), requireEditor};
    return true;
}

bool IsChineseLayoutForWindow(HWND hwnd) {
    DWORD tid = hwnd ? GetWindowThreadProcessId(hwnd, nullptr) : 0;
    if (!tid) {
        return false;
    }
    HKL layout = GetKeyboardLayout(tid);
    return PRIMARYLANGID(LOWORD(reinterpret_cast<ULONG_PTR>(layout))) == LANG_CHINESE;
}

std::optional<HintLang> DetectLangHint(HWND focus, const wchar_t** methodOut) {
    if (!IsChineseLayoutForWindow(focus)) {
        *methodOut = L"non-zh-layout";
        return HintLang::English;
    }

    constexpr WPARAM kGetConversionMode = 0x0001;  // IMC_GETCONVERSIONMODE
    constexpr WPARAM kGetOpenStatus = 0x0005;      // IMC_GETOPENSTATUS
    constexpr UINT kImeTimeoutMs = 10;
    const ULONGLONG deadline = GetTickCount64() + 40;
    constexpr UINT kSendFlags = SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT;
    const HWND candidates[]{focus, GetAncestor(focus, GA_ROOT), GetForegroundWindow(),
                            g_cachedImeOwner};
    HWND seen[4]{};
    int seenCount = 0;
    for (HWND hwnd : candidates) {
        if (GetTickCount64() >= deadline) {
            break;
        }
        HWND ime = hwnd ? ImmGetDefaultIMEWnd(hwnd) : nullptr;
        if (!ime) {
            continue;
        }
        bool duplicate = false;
        for (int i = 0; i < seenCount; ++i) {
            duplicate = duplicate || seen[i] == ime;
        }
        if (duplicate) {
            continue;
        }
        if (seenCount < ARRAYSIZE(seen)) {
            seen[seenCount++] = ime;
        }
        DWORD_PTR open = 0;
        if (!SendMessageTimeoutW(ime, WM_IME_CONTROL, kGetOpenStatus, 0, kSendFlags,
                                 kImeTimeoutMs, &open)) {
            continue;
        }
        g_cachedImeOwner = hwnd;
        if (!open) {
            *methodOut = L"IME closed";
            return HintLang::English;
        }
        DWORD_PTR conversion = 0;
        if (GetTickCount64() >= deadline) {
            break;
        }
        if (!SendMessageTimeoutW(ime, WM_IME_CONTROL, kGetConversionMode, 0, kSendFlags,
                                 kImeTimeoutMs, &conversion)) {
            continue;
        }
        HKL layout = GetKeyboardLayout(GetWindowThreadProcessId(hwnd, nullptr));
        if (layout && PRIMARYLANGID(LOWORD(reinterpret_cast<ULONG_PTR>(layout))) ==
                          LANG_ENGLISH) {
            *methodOut = L"IME open but en-US layout";
            return HintLang::English;
        }
        if (conversion & IME_CMODE_NOCONVERSION) {
            *methodOut = L"IME NOCONVERSION";
            return HintLang::English;
        }
        *methodOut = L"IME conversion mode";
        return (conversion & (IME_CMODE_NATIVE | IME_CMODE_KATAKANA))
                   ? HintLang::Chinese
                   : HintLang::English;
    }
    return std::nullopt;
}

bool IsCapsOn() {
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

const wchar_t* LangLabel(HintLang lang) {
    if (lang == HintLang::Chinese) {
        return L"中";
    }
    return g_settings.englishUseEn ? L"En" : L"英";
}

const wchar_t* CapsLabel(bool on) {
    if (g_settings.capsUseEn) {
        return on ? L"A" : L"a";
    }
    return on ? L"大写" : L"小写";
}

std::wstring BuildImeBubbleText(HintLang lang, bool capsOn) {
    std::wstring text = LangLabel(lang);
    if (g_settings.combineCapsWithIme && capsOn) {
        text += L" · ";
        text += CapsLabel(true);
    }
    return text;
}

struct Rgb {
    BYTE r;
    BYTE g;
    BYTE b;
};

Rgb ResolveGlassRgb() {
    const std::wstring& m = g_settings.glass.bgColorMode;
    if (_wcsicmp(m.c_str(), L"Accent") == 0) {
        DWORD colorization = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
            return {static_cast<BYTE>((colorization >> 16) & 0xFF),
                    static_cast<BYTE>((colorization >> 8) & 0xFF),
                    static_cast<BYTE>(colorization & 0xFF)};
        }
        return {0, 120, 215};
    }
    static const struct { const wchar_t* name; Rgb rgb; } kPresets[] = {
        {L"GlassWhite", {255, 255, 255}}, {L"DeepBlack", {10, 10, 10}},
        {L"OceanBlue", {26, 54, 93}},     {L"AuroraCyan", {0, 92, 83}},
        {L"RosePink", {91, 44, 48}},      {L"BordeauxRed", {74, 14, 23}},
        {L"ForestGreen", {26, 58, 42}},   {L"RoyalPurple", {59, 19, 84}},
        {L"SunsetOrange", {140, 61, 0}},  {L"ChampagneGold", {122, 101, 63}},
        {L"MorandiSage", {78, 94, 80}},   {L"MistGrey", {46, 46, 46}},
    };
    for (const auto& p : kPresets) {
        if (_wcsicmp(m.c_str(), p.name) == 0) {
            return p.rgb;
        }
    }
    return {46, 46, 46};  // Default
}

Rgb ApplyLuminosity(Rgb base, int luminosity) {
    int t = ClampInt(luminosity, 0, 150);
    auto blend = [t](int c) {
        return static_cast<BYTE>((c * (150 - t) + 255 * t) / 150);
    };
    return {blend(base.r), blend(base.g), blend(base.b)};
}

bool IsLightGlass(Rgb c) {
    int y = (c.r * 2126 + c.g * 7152 + c.b * 722) / 10000;
    return y >= 160;
}

void HideBubble();
void PumpFade();

void BoxBlurInPlace(std::vector<BYTE>& src, int width, int height, int radius) {
    if (radius <= 0 || width <= 0 || height <= 0) {
        return;
    }
    radius = ClampInt(radius, 1, 12);
    std::vector<BYTE> tmp(src.size());
    auto sample = [&](const std::vector<BYTE>& buf, int x, int y, int c) -> int {
        return buf[(static_cast<size_t>(ClampInt(y, 0, height - 1)) * width +
                    ClampInt(x, 0, width - 1)) *
                       4 +
                   c];
    };
    auto pass = [&](const std::vector<BYTE>& in, std::vector<BYTE>& out, bool horiz) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int b = 0, g = 0, r = 0, a = 0, n = 0;
                for (int k = -radius; k <= radius; ++k) {
                    const int sx = horiz ? x + k : x;
                    const int sy = horiz ? y : y + k;
                    b += sample(in, sx, sy, 0);
                    g += sample(in, sx, sy, 1);
                    r += sample(in, sx, sy, 2);
                    a += sample(in, sx, sy, 3);
                    ++n;
                }
                size_t i = (static_cast<size_t>(y) * width + x) * 4;
                out[i + 0] = static_cast<BYTE>(b / n);
                out[i + 1] = static_cast<BYTE>(g / n);
                out[i + 2] = static_cast<BYTE>(r / n);
                out[i + 3] = static_cast<BYTE>(a / n);
            }
        }
    };
    pass(src, tmp, true);
    pass(tmp, src, false);
}

bool CaptureScreenRect(int x, int y, int width, int height, std::vector<BYTE>& outBgra) {
    outBgra.assign(static_cast<size_t>(width) * height * 4, 0);
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return false;
    }
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(mem, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        DeleteDC(mem);
        ReleaseDC(nullptr, screen);
        return false;
    }
    HGDIOBJ old = SelectObject(mem, dib);
    BitBlt(mem, 0, 0, width, height, screen, x, y, SRCCOPY | CAPTUREBLT);
    memcpy(outBgra.data(), bits, outBgra.size());
    SelectObject(mem, old);
    DeleteObject(dib);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return true;
}

void PremultiplyBitmap(void* bits, int width, int height, BYTE alphaScale) {
    auto* p = static_cast<BYTE*>(bits);
    for (int i = 0; i < width * height; ++i) {
        BYTE b = p[0], g = p[1], r = p[2], a = p[3];
        if (a == 0 && (r | g | b)) {
            a = 255;
        }
        a = static_cast<BYTE>((static_cast<int>(a) * alphaScale) / 255);
        p[0] = static_cast<BYTE>((b * a) / 255);
        p[1] = static_cast<BYTE>((g * a) / 255);
        p[2] = static_cast<BYTE>((r * a) / 255);
        p[3] = a;
        p += 4;
    }
}

float RoundMask(int x, int y, int width, int height, int radius) {
    int rr = radius;
    auto corner = [&](int dx, int dy) {
        return (dx * dx + dy * dy) <= (rr * rr);
    };
    if (x < rr && y < rr) {
        return corner(rr - x - 1, rr - y - 1) ? 1.f : 0.f;
    }
    if (x >= width - rr && y < rr) {
        return corner(x - (width - rr), rr - y - 1) ? 1.f : 0.f;
    }
    if (x < rr && y >= height - rr) {
        return corner(rr - x - 1, y - (height - rr)) ? 1.f : 0.f;
    }
    if (x >= width - rr && y >= height - rr) {
        return corner(x - (width - rr), y - (height - rr)) ? 1.f : 0.f;
    }
    return 1.f;
}

POINT BubblePosition(const RECT& caret, int dpi, int width, int height) {
    const bool hasCaret = caret.bottom > caret.top;
    int x = hasCaret ? caret.left + (caret.right - caret.left) / 2 - width / 2
                     : caret.right;
    int y = hasCaret ? caret.top - height - ScaleForDpi(6, dpi) : caret.top;
    x += ScaleForDpi(g_settings.anchorOffsetX, dpi);
    y += ScaleForDpi(g_settings.anchorOffsetY, dpi);
    HMONITOR monitor = MonitorFromRect(&caret, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (GetMonitorInfoW(monitor, &info)) {
        if (hasCaret && y < info.rcWork.top) {
            y = caret.bottom + ScaleForDpi(6, dpi);
        }
        x = (std::max<LONG>)(info.rcWork.left,
                        (std::min<LONG>)(x, info.rcWork.right - width));
        y = (std::max<LONG>)(info.rcWork.top,
                        (std::min<LONG>)(y, info.rcWork.bottom - height));
    }
    return POINT{x, y};
}

bool PaintOverlay(const std::wstring& text, int dpi, BYTE alphaScale, const RECT& caret) {
    if (!g_overlayWnd || !IsWindow(g_overlayWnd) || text.empty()) {
        return false;
    }

    const int padX = ScaleForDpi(10, dpi);
    const int padY = ScaleForDpi(5, dpi);
    const int radius = ScaleForDpi(10, dpi);
    const int fontPx = ScaleForDpi(12, dpi);

    HDC screenDc = GetDC(nullptr);
    HDC memDc = CreateCompatibleDC(screenDc);

    HFONT font = CreateFontW(-fontPx, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT oldFont = static_cast<HFONT>(SelectObject(memDc, font));
    SIZE textSize{};
    GetTextExtentPoint32W(memDc, text.c_str(), static_cast<int>(text.size()), &textSize);
    int width = (std::max)(static_cast<int>(textSize.cx) + padX * 2, ScaleForDpi(28, dpi));
    int height = (std::max)(static_cast<int>(textSize.cy) + padY * 2, ScaleForDpi(22, dpi));

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        SelectObject(memDc, oldFont);
        DeleteObject(font);
        DeleteDC(memDc);
        ReleaseDC(nullptr, screenDc);
        return false;
    }
    HGDIOBJ oldBmp = SelectObject(memDc, dib);
    ZeroMemory(bits, static_cast<size_t>(width) * height * 4);

    // Use the measured capsule size for both capture and final placement.
    const POINT origin = BubblePosition(caret, dpi, width, height);
    const int ox = origin.x;
    const int oy = origin.y;

    const int blurRadius = g_settings.glass.blurPreset / 8;
    const bool cacheHit = g_backdrop.valid && g_backdrop.width == width &&
                          g_backdrop.height == height && g_backdrop.originX == ox &&
                          g_backdrop.originY == oy && g_backdrop.text == text;
    if (!cacheHit) {
        g_backdrop.pixels.clear();
        g_backdrop.valid = CaptureScreenRect(ox, oy, width, height, g_backdrop.pixels);
        if (g_backdrop.valid && blurRadius > 0) {
            BoxBlurInPlace(g_backdrop.pixels, width, height, blurRadius);
            if (g_settings.glass.blurPreset >= 45) {
                BoxBlurInPlace(g_backdrop.pixels, width, height,
                               (std::max)(1, blurRadius / 2));
            }
        }
        g_backdrop.width = width;
        g_backdrop.height = height;
        g_backdrop.originX = ox;
        g_backdrop.originY = oy;
        g_backdrop.text = text;
    }
    const bool haveBack = g_backdrop.valid;
    const std::vector<BYTE>& back = g_backdrop.pixels;

    Rgb tint = ApplyLuminosity(ResolveGlassRgb(), g_settings.glass.luminosityPreset);
    const int opacity = g_settings.glass.opacityPreset;
    const bool light = IsLightGlass(tint);
    const BYTE textR = light ? 20 : 255;
    const BYTE textG = light ? 20 : 255;
    const BYTE textB = light ? 20 : 255;

    auto* dst = static_cast<BYTE*>(bits);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float mask = RoundMask(x, y, width, height, radius);
            BYTE* p = dst + (static_cast<size_t>(y) * width + x) * 4;
            if (mask <= 0.f) {
                p[0] = p[1] = p[2] = p[3] = 0;
                continue;
            }
            int br = tint.r, bg = tint.g, bb = tint.b;
            if (haveBack) {
                size_t bi = (static_cast<size_t>(y) * width + x) * 4;
                int sr = back[bi + 2], sg = back[bi + 1], sb = back[bi + 0];
                br = (sr * (100 - opacity) + tint.r * opacity) / 100;
                bg = (sg * (100 - opacity) + tint.g * opacity) / 100;
                bb = (sb * (100 - opacity) + tint.b * opacity) / 100;
            }
            BYTE a = static_cast<BYTE>(ClampInt(
                haveBack ? (180 + opacity / 2) : (40 + opacity * 2), 30, 240));
            p[0] = static_cast<BYTE>(bb);
            p[1] = static_cast<BYTE>(bg);
            p[2] = static_cast<BYTE>(br);
            p[3] = a;
        }
    }

    HDC textDc = CreateCompatibleDC(screenDc);
    void* tbits = nullptr;
    HBITMAP tdib = CreateDIBSection(textDc, &bmi, DIB_RGB_COLORS, &tbits, nullptr, 0);
    HGDIOBJ oldTBmp = SelectObject(textDc, tdib);
    SelectObject(textDc, font);
    RECT tr{0, 0, width, height};
    HBRUSH clear = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(textDc, &tr, clear);
    DeleteObject(clear);
    SetBkMode(textDc, TRANSPARENT);
    SetTextColor(textDc, RGB(textR, textG, textB));
    DrawTextW(textDc, text.c_str(), -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    auto* src = static_cast<BYTE*>(tbits);
    dst = static_cast<BYTE*>(bits);
    for (int i = 0; i < width * height; ++i) {
        int lum = (src[0] + src[1] + src[2]) / 3;
        if (lum > 8 && dst[3] > 0) {
            if (light) {
                int inv = 255 - lum;
                if (inv > 8) {
                    dst[0] = static_cast<BYTE>((textB * inv + dst[0] * (255 - inv)) / 255);
                    dst[1] = static_cast<BYTE>((textG * inv + dst[1] * (255 - inv)) / 255);
                    dst[2] = static_cast<BYTE>((textR * inv + dst[2] * (255 - inv)) / 255);
                }
            } else if (lum > 16) {
                dst[0] = static_cast<BYTE>((textB * lum + dst[0] * (255 - lum)) / 255);
                dst[1] = static_cast<BYTE>((textG * lum + dst[1] * (255 - lum)) / 255);
                dst[2] = static_cast<BYTE>((textR * lum + dst[2] * (255 - lum)) / 255);
            }
        }
        dst += 4;
        src += 4;
    }

    SelectObject(textDc, oldTBmp);
    DeleteObject(tdib);
    DeleteDC(textDc);

    PremultiplyBitmap(bits, width, height, alphaScale);

    SIZE size{width, height};
    POINT srcPt{0, 0};
    POINT pt{ox, oy};
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(g_overlayWnd, screenDc, &pt, &size, memDc, &srcPt, 0, &blend,
                        ULW_ALPHA);
    g_lastPaintedAlpha = static_cast<int>(alphaScale);

    SelectObject(memDc, oldBmp);
    SelectObject(memDc, oldFont);
    DeleteObject(dib);
    DeleteObject(font);
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
    return true;
}

void EnsureOverlayWindow() {
    if (g_overlayWnd && IsWindow(g_overlayWnd)) {
        return;
    }
    g_overlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kOverlayClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_hInstance,
        nullptr);
    if (!g_overlayWnd) {
        ZIH_LOG(L"CreateWindowEx overlay failed: %lu", GetLastError());
    }
}

void ShowBubbleAt(const std::wstring& text, const RECT& caret, int dpi) {
    if (!g_settings.bubbleEnabled || text.empty()) {
        return;
    }
    const ULONGLONG nowTick = GetTickCount64();
    const LONG deltaX = caret.left - g_lastShown.caret.left;
    const LONG deltaY = caret.top - g_lastShown.caret.top;
    const bool sameCaret = deltaX >= -2 && deltaX <= 2 && deltaY >= -2 && deltaY <= 2;
    if (g_lastShown.at != 0 && nowTick - g_lastShown.at <= kDuplicateShowMs &&
        g_lastShown.text == text && sameCaret) {
        return;
    }
    g_lastShown.text = text;
    g_lastShown.caret = caret;
    g_lastShown.at = nowTick;

    EnsureOverlayWindow();
    if (!g_overlayWnd) {
        return;
    }
    g_bubbleText = text;
    g_bubbleDpi = dpi;
    g_bubbleCaret = caret;
    g_bubbleShownAt = GetTickCount64();
    g_bubbleVisible = true;

    ShowWindow(g_overlayWnd, SW_HIDE);
    g_backdrop.valid = false;
    g_lastPaintedAlpha = -1;
    PaintOverlay(text, dpi, 255, caret);
    SetWindowPos(g_overlayWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetTimer(g_hostWnd, kFadeTimerId, kFadeIntervalMs, nullptr);
}

void ShowPointerBubble(const std::wstring& text) {
    POINT cursor{};
    if (!GetCursorPos(&cursor)) {
        return;
    }
    const int dpi = GetDpiForPointSafe(cursor);
    const int baseX = ScaleForDpi(10, dpi);
    const int baseY = ScaleForDpi(18, dpi);
    const RECT anchor{cursor.x + baseX, cursor.y + baseY, cursor.x + baseX,
                      cursor.y + baseY};
    ShowBubbleAt(text, anchor, dpi);
}

void HideBubble() {
    g_bubbleVisible = false;
    g_backdrop.valid = false;
    g_lastPaintedAlpha = -1;
    if (g_hostWnd) {
        KillTimer(g_hostWnd, kFadeTimerId);
    }
    if (g_overlayWnd && IsWindow(g_overlayWnd)) {
        ShowWindow(g_overlayWnd, SW_HIDE);
    }
}

void PumpFade() {
    if (!g_bubbleVisible) {
        return;
    }
    const ULONGLONG now = GetTickCount64();
    const int duration = g_settings.durationMs;
    const int fadeMs = (std::min)(220, duration / 3);
    const ULONGLONG elapsed = now - g_bubbleShownAt;
    if (static_cast<int>(elapsed) >= duration) {
        HideBubble();
        return;
    }
    BYTE alpha = 255;
    const int remaining = duration - static_cast<int>(elapsed);
    if (remaining < fadeMs && fadeMs > 0) {
        alpha = static_cast<BYTE>((remaining * 255) / fadeMs);
    }
    if (static_cast<int>(alpha) == g_lastPaintedAlpha) {
        return;
    }
    PaintOverlay(g_bubbleText, g_bubbleDpi, alpha, g_bubbleCaret);
}

bool FocusStillCurrent(HWND focus) {
    if (!focus) {
        return false;
    }
    GUITHREADINFO gui{sizeof(gui)};
    if (!GetGUIThreadInfo(0, &gui)) {
        return false;
    }
    HWND current = gui.hwndFocus ? gui.hwndFocus : GetForegroundWindow();
    return current == focus;
}

void ShowHint(const std::wstring& text, HWND focus, const GUITHREADINFO& gui,
              const wchar_t* langMethod) {
    if (text.empty()) {
        return;
    }
    if (FocusPrefersPointer(focus)) {
        ZIH_LOG(L"show \"%s\" via=%s anchor=pointer(class)", text.c_str(), langMethod);
        ShowPointerBubble(text);
        return;
    }

    if (gui.hwndCaret && gui.rcCaret.bottom > gui.rcCaret.top &&
        FocusStillCurrent(focus)) {
        RECT caret = gui.rcCaret;
        if (caret.right == caret.left) {
            ++caret.right;
        }
        MapWindowPoints(gui.hwndCaret, nullptr, reinterpret_cast<POINT*>(&caret), 2);
        ShowBubbleAt(text, caret, GetWindowDpiSafe(focus));
        return;
    }

    // Resolve the anchor before showing. One bubble uses one position.
    HideBubble();
    if (!RequestUiaCaret(focus, text)) {
        ShowPointerBubble(text);
    }
}

void CompletePendingShow(HWND focus) {
    if (!g_pendingShow) {
        return;
    }
    const PendingShow pending = *g_pendingShow;
    if (pending.focus != focus || pending.foreground != GetForegroundWindow() ||
        pending.generation != g_focusGeneration ||
        pending.inputGeneration != g_inputGeneration.load() || !FocusStillCurrent(focus)) {
        CancelCaretQuery();
        return;
    }
    const bool complete = WaitForSingleObject(g_caretProcess, 0) == WAIT_OBJECT_0;
    const bool expired = GetTickCount64() - pending.startedAt >= kCaretExpiryMs;
    if (!complete && !expired) {
        return;
    }
    CaretPacket result{};
    DWORD exitCode = ERROR_CANCELLED;
    if (complete && GetExitCodeProcess(g_caretProcess, &exitCode) && exitCode == 0) {
        result = *g_caretPacket;
    }
    CancelCaretQuery();
    if (result.hasRect) {
        ShowBubbleAt(pending.text, result.rect, GetWindowDpiSafe(focus));
    } else if (!pending.requireEditor || result.editor) {
        ShowPointerBubble(pending.text);
    }
}

void HandleMouse(WPARAM message) {
    if (message == WM_LBUTTONUP) {
        g_clickPending = true;
        g_clickReadyAt = GetTickCount64() + 80;
    }
}

bool IsShiftKey(DWORD key) {
    return key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT;
}

void HandleShiftTap(HWND foreground) {
    if (!foreground || foreground != GetForegroundWindow()) {
        return;
    }
    if (!g_pendingShiftTaps || foreground != g_pendingShiftForeground) {
        g_pendingShiftTaps = 0;
        g_shiftBaseline = g_hasLastLang ? std::optional<HintLang>(g_lastLang) : std::nullopt;
    }
    ++g_pendingShiftTaps;
    g_pendingShiftForeground = foreground;
    g_shiftReadyAt = GetTickCount64() + kShiftSettleMs;
}

LRESULT CALLBACK MouseProc(int code, WPARAM message, LPARAM data) {
    if (code == HC_ACTION && message == WM_LBUTTONDOWN) {
        ++g_inputGeneration;
    }
    if (code == HC_ACTION && message == WM_LBUTTONUP) {
        PostMessageW(g_hostWnd, kMouseMessage, message, 0);
    }
    return CallNextHookEx(nullptr, code, message, data);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM message, LPARAM data) {
    if (code == HC_ACTION) {
        const auto& key = *reinterpret_cast<const KBDLLHOOKSTRUCT*>(data);
        const bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
        const bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
        if (down) {
            ++g_inputGeneration;
        }
        // Shift/chord state belongs exclusively to this input thread.
        if (IsShiftKey(key.vkCode)) {
            if (down && !g_shiftDown) {
                g_shiftDown = true;
                g_shiftDownAt = GetTickCount64();
                g_shiftForeground = GetForegroundWindow();
                g_shiftUsedInChord =
                    (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                    (GetAsyncKeyState(VK_MENU) & 0x8000) ||
                    (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
                    (GetAsyncKeyState(VK_RWIN) & 0x8000);
            } else if (up && g_shiftDown) {
                HWND foreground = GetForegroundWindow();
                if (!g_shiftUsedInChord &&
                    GetTickCount64() - g_shiftDownAt <= kShiftTapMaxMs &&
                    foreground && foreground == g_shiftForeground) {
                    PostMessageW(g_hostWnd, kKeyboardMessage,
                                 reinterpret_cast<WPARAM>(foreground), 0);
                }
                g_shiftDown = false;
            } else if (down) {
                g_shiftUsedInChord = true;
            }
        } else if (down && g_shiftDown) {
            g_shiftUsedInChord = true;
        }
    }
    return CallNextHookEx(nullptr, code, message, data);
}

DWORD WINAPI InputThread(void*) {
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, g_hInstance, 0);
    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, g_hInstance, 0);
    if (!g_mouseHook || !g_keyboardHook) {
        ZIH_LOG(L"Input hook initialization failed: %lu", GetLastError());
    }
    while (MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT,
                                       MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0 + 1) {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            DispatchMessageW(&message);
        }
    }
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

std::optional<HintLang> ResolveShiftLang(std::optional<HintLang> raw,
                                        unsigned taps,
                                        std::optional<HintLang> baseline,
                                        const wchar_t** method) {
    if (taps) {
        // IMM may report success while returning an unchanging TSF proxy value.
        // Allow real changes to win, otherwise keep a prediction until IMM catches up.
        const HintLang before = baseline.value_or(HintLang::Chinese);
        const HintLang expected = (taps % 2)
                                      ? (before == HintLang::Chinese ? HintLang::English
                                                                    : HintLang::Chinese)
                                      : before;
        if (raw && baseline && *raw != *baseline && !g_staleImeLang) {
            return raw;
        }
        g_staleImeLang = raw;
        if (raw && *raw == expected) {
            g_staleImeLang.reset();
        }
        *method = L"shift-tap";
        return expected;
    }
    if (g_staleImeLang) {
        if (!raw || *raw == *g_staleImeLang) {
            *method = L"shift-predicted";
            return g_lastLang;
        }
        g_staleImeLang.reset();
    }
    return raw;
}

void PollOnce() {
    if (g_settingsChanged.exchange(false)) {
        LoadSettings();
        CancelCaretQuery();
        HideBubble();
        g_hasLastLang = false;
        g_hasLastCaps = false;
        g_staleImeLang.reset();
        g_pendingShiftTaps = 0;
        g_langQueryFailing = false;
        g_lastLangQueryTick = 0;
    }
    if (!g_settings.bubbleEnabled) {
        CancelCaretQuery();
        if (g_bubbleVisible) {
            HideBubble();
        }
        return;
    }

    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == g_overlayWnd || foreground == g_hostWnd) {
        CancelCaretQuery();
        g_lastFocus = nullptr;
        g_hasLastLang = false;
        g_hasLastCaps = false;
        if (g_bubbleVisible) {
            HideBubble();
        }
        return;
    }

    // Avoid querying Steam's
    // IME or accessibility providers, and discard work from the previous app.
    if (IsSteamWindow(foreground)) {
        CancelCaretQuery();
        g_focusHintPending = false;
        g_clickPending = false;
        g_pendingShiftTaps = 0;
        g_cachedForeground = nullptr;
        g_cachedFocus = nullptr;
        g_cachedImeOwner = nullptr;
        g_lastFocus = nullptr;
        g_hasLastLang = false;
        g_hasLastCaps = false;
        if (g_bubbleVisible) {
            HideBubble();
        }
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG logicalTick = g_logicalFocusTick.load();
    const bool logicalEventNew = (logicalTick != g_lastLogicalFocusTick);
    g_lastLogicalFocusTick = logicalTick;
    if (logicalEventNew || foreground != g_cachedForeground) {
        ++g_focusGeneration;
        CancelCaretQuery();
        g_focusHintPending = true;
    }
    if (g_clickPending) {
        // Cancel work for the previous control even when both share one HWND.
        ++g_focusGeneration;
        CancelCaretQuery();
        if (now < g_clickReadyAt) {
            return;
        }
        g_clickPending = false;
        g_focusHintPending = true;
    }

    GUITHREADINFO gui{sizeof(gui)};
    const bool needsFocusQuery = foreground != g_cachedForeground || logicalEventNew ||
                                 g_focusHintPending || g_bubbleVisible || g_pendingShow.has_value() ||
                                 !g_cachedFocus;
    if (needsFocusQuery) {
        if (!GetGUIThreadInfo(0, &gui)) {
            CancelCaretQuery();
            g_cachedForeground = nullptr;
            g_cachedFocus = nullptr;
            if (g_bubbleVisible) {
                HideBubble();
            }
            return;
        }
        g_cachedForeground = foreground;
        g_cachedFocus = gui.hwndFocus ? gui.hwndFocus : foreground;
    } else {
        gui.hwndFocus = g_cachedFocus;
    }

    HWND focus = g_cachedFocus ? g_cachedFocus : foreground;
    if (focus == g_overlayWnd || focus == g_hostWnd) {
        CancelCaretQuery();
        g_lastFocus = nullptr;
        g_hasLastLang = false;
        g_hasLastCaps = false;
        if (g_bubbleVisible) {
            HideBubble();
        }
        return;
    }

    if (focus != g_lastFocus || foreground != g_lastForeground) {
        g_cachedImeOwner = nullptr;
        g_staleImeLang.reset();
        g_hasLastLang = false;
        g_lastLangQueryTick = 0;
        g_langQueryFailing = false;
        if (foreground != g_pendingShiftForeground) {
            g_pendingShiftTaps = 0;
        }
    }
    g_lastForeground = foreground;
    g_lastFocus = focus;
    if (g_pendingShiftTaps && now < g_shiftReadyAt) {
        return;
    }
    const unsigned shiftTaps = std::exchange(g_pendingShiftTaps, 0u);
    const bool shiftTap = shiftTaps != 0;

    const ULONGLONG langRetryMs = g_langQueryFailing ? kLangFailRetryMs : kLangRefreshMs;
    const wchar_t* langMethod = L"cached";
    std::optional<HintLang> lang;
    const bool queryDue = !g_lastLangQueryTick ||
                          now - g_lastLangQueryTick >= langRetryMs;
    if (shiftTap || queryDue) {
        langMethod = L"";
        lang = DetectLangHint(focus, &langMethod);
        g_langQueryFailing = !lang;
        // Record attempts too, including the very first failed query.
        g_lastLangQueryTick = GetTickCount64();
    } else if (g_hasLastLang) {
        lang = g_staleImeLang ? g_staleImeLang : std::optional<HintLang>(g_lastLang);
    }

    if (IsChineseLayoutForWindow(focus)) {
        lang = ResolveShiftLang(lang, shiftTaps, g_shiftBaseline, &langMethod);
        if (shiftTap && lang && !g_hasLastLang) {
            g_lastLang = g_shiftBaseline.value_or(HintLang::Chinese);
            g_hasLastLang = true;
        }
    } else {
        g_staleImeLang.reset();
    }

    const bool capsOn = IsCapsOn();
    const bool langChanged = lang && g_hasLastLang && *lang != g_lastLang;
    const bool capsChanged = g_hasLastCaps && (capsOn != g_lastCaps);

    bool shouldShow = false;
    std::wstring text;
    if (g_settings.showOnCapsLock && capsChanged) {
        shouldShow = true;
        text = capsOn ? CapsLabel(true)
                      : BuildImeBubbleText(lang.value_or(g_lastLang), false);
    }
    if (g_settings.showOnImeToggle && langChanged) {
        shouldShow = true;
        text = BuildImeBubbleText(*lang, capsOn);
    }

    if (g_focusHintPending && !shouldShow) {
        // UIA confirms an editable control before a click/focus hint can appear.
        const std::wstring current = capsOn ? CapsLabel(true)
                                           : BuildImeBubbleText(lang.value_or(g_lastLang), false);
        HideBubble();
        RequestUiaCaret(focus, current, true);
    }
    g_focusHintPending = false;

    if (shouldShow) {
        CancelCaretQuery();
        if (!needsFocusQuery) {
            GUITHREADINFO fresh{sizeof(fresh)};
            if (GetGUIThreadInfo(0, &fresh)) {
                gui = fresh;
                if (fresh.hwndFocus) {
                    g_cachedFocus = fresh.hwndFocus;
                    focus = fresh.hwndFocus;
                }
            }
        }
        ShowHint(text, focus, gui, langMethod);
    }
    CompletePendingShow(focus);

    g_lastForeground = foreground;
    g_lastFocus = focus;
    if (lang) {
        g_lastLang = *lang;
        g_hasLastLang = true;
    }
    g_lastCaps = capsOn;
    g_hasLastCaps = true;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static const UINT dismissForCapture =
        RegisterWindowMessageW(L"ZenDesktop.DismissCaretHintForCapture.v1");
    if (dismissForCapture && msg == dismissForCapture) {
        CancelCaretQuery();
        g_focusHintPending = false;
        HideBubble();
        return TRUE;
    }
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case kKeyboardMessage:
            HandleShiftTap(reinterpret_cast<HWND>(wParam));
            return 0;
        case kMouseMessage:
            HandleMouse(wParam);
            return 0;
        case WM_TIMER:
            if (wParam == kPollTimerId) {
                PollOnce();
                return 0;
            }
            if (wParam == kFadeTimerId) {
                PumpFade();
                return 0;
            }
            break;
        case WM_DESTROY:
            KillTimer(hwnd, kPollTimerId);
            KillTimer(hwnd, kFadeTimerId);
            break;
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool RegisterClasses() {
    WNDCLASSEXW oc{sizeof(oc)};
    oc.lpfnWndProc = OverlayWndProc;
    oc.hInstance = g_hInstance;
    oc.lpszClassName = kOverlayClass;
    if (!RegisterClassExW(&oc)) {
        ZIH_LOG(L"RegisterClass overlay failed: %lu", GetLastError());
        return false;
    }
    WNDCLASSEXW hc{sizeof(hc)};
    hc.lpfnWndProc = HostWndProc;
    hc.hInstance = g_hInstance;
    hc.lpszClassName = kHostClass;
    if (!RegisterClassExW(&hc)) {
        ZIH_LOG(L"RegisterClass host failed: %lu", GetLastError());
        UnregisterClassW(kOverlayClass, g_hInstance);
        return false;
    }
    return true;
}

void UpdatePollTimer() {
    if (GetForegroundWindow()) {
        if (!SetTimer(g_hostWnd, kPollTimerId, kPollIntervalMs, nullptr)) {
            ZIH_LOG(L"Set poll timer failed: %lu", GetLastError());
        }
    } else {
        KillTimer(g_hostWnd, kPollTimerId);
    }
    PollOnce();
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG,
                           DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        UpdatePollTimer();
        return;
    }
    if (event == EVENT_OBJECT_FOCUS && hwnd &&
        (idObject == OBJID_CLIENT || idObject == OBJID_WINDOW)) {
        HWND foreground = GetForegroundWindow();
        if (foreground && GetAncestor(hwnd, GA_ROOT) == GetAncestor(foreground, GA_ROOT)) {
            g_logicalFocusTick.fetch_add(1);
        }
    }
}

DWORD WINAPI HostThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    LoadSettings();
    if (!RegisterClasses()) {
        return 0;
    }
    HRESULT com = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    g_hostWnd = CreateWindowExW(0, kHostClass, L"", WS_POPUP, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, g_hInstance, nullptr);
    if (g_hostWnd) {
        g_inputThread = CreateThread(nullptr, 0, InputThread, nullptr, 0, nullptr);
        if (!g_inputThread) {
            ZIH_LOG(L"Input thread creation failed: %lu", GetLastError());
        }
        HWINEVENTHOOK foregroundHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        HWINEVENTHOOK focusHook = SetWinEventHook(
            EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS, nullptr,
            WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (foregroundHook) {
            UpdatePollTimer();
        } else if (!SetTimer(g_hostWnd, kPollTimerId, kPollIntervalMs, nullptr)) {
            ZIH_LOG(L"Set fallback poll timer failed: %lu", GetLastError());
        }
        ZIH_LOG(L"host ready pid=%lu tid=%lu", GetCurrentProcessId(),
                GetCurrentThreadId());

        for (;;) {
            const DWORD result = MsgWaitForMultipleObjectsEx(
                1, &g_stopEvent, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (result != WAIT_OBJECT_0 + 1) {
                break;
            }
            MSG msg{};
            if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    break;
                }
                DispatchMessageW(&msg);
            }
        }

        if (foregroundHook) {
            UnhookWinEvent(foregroundHook);
        }
        if (focusHook) {
            UnhookWinEvent(focusHook);
        }
        CancelCaretQuery();
        if (g_inputThread) {
            WaitForSingleObject(g_inputThread, INFINITE);
            CloseHandle(g_inputThread);
            g_inputThread = nullptr;
        }
        if (g_overlayWnd) {
            DestroyWindow(g_overlayWnd);
            g_overlayWnd = nullptr;
        }
        DestroyWindow(g_hostWnd);
        g_hostWnd = nullptr;
    } else {
        ZIH_LOG(L"Create host failed: %lu", GetLastError());
    }
    UnregisterClassW(kHostClass, g_hInstance);
    UnregisterClassW(kOverlayClass, g_hInstance);
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

// rundll32 entry point. The parent owns the job and enforces the deadline.
extern "C" __declspec(dllexport) void CALLBACK ZenCaretQuery(
    HWND, HINSTANCE, LPSTR command, int) {
    wchar_t mappingName[160]{};
    if (!MultiByteToWideChar(CP_ACP, 0, command, -1, mappingName, ARRAYSIZE(mappingName))) {
        return;
    }
    HANDLE mapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, mappingName);
    if (!mapping) {
        return;
    }
    auto* packet = static_cast<CaretPacket*>(MapViewOfFile(
        mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CaretPacket)));
    if (packet && packet->size == sizeof(CaretPacket) &&
        !IsSteamWindow(packet->focus) && !IsSteamWindow(GetForegroundWindow())) {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        const HRESULT com = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (SUCCEEDED(com)) {
            ComPtr<IUIAutomation2> automation;
            if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation8), nullptr,
                    CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&automation)))) {
                automation->put_ConnectionTimeout(100);
                automation->put_TransactionTimeout(100);
                auto rect = QueryUiaCaret(automation.Get(), packet->focus,
                                          packet->requireEditor, &packet->editor);
                if (rect) {
                    packet->rect = *rect;
                    packet->hasRect = true;
                }
            }
            if (!packet->hasRect && (!packet->requireEditor || packet->editor)) {
                GUITHREADINFO gui{sizeof(gui)};
                const wchar_t* method = L"";
                if (GetGUIThreadInfo(0, &gui) &&
                    QueryCaretScreenRect(packet->focus, gui, &packet->rect, &method)) {
                    packet->hasRect = true;
                }
            }
            automation.Reset();
            CoUninitialize();
        }
    }
    if (packet) {
        UnmapViewOfFile(packet);
    }
    CloseHandle(mapping);
}

BOOL Wh_ModInit() {
    USEROBJECTFLAGS stationFlags{};
    if (!GetUserObjectInformationW(GetProcessWindowStation(), UOI_FLAGS, &stationFlags,
                                   sizeof(stationFlags), nullptr) ||
        !(stationFlags.dwFlags & WSF_VISIBLE)) {
        return FALSE;
    }
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&HostThread), &g_hInstance)) {
        return FALSE;
    }
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    return g_stopEvent != nullptr;
}

void Wh_ModAfterInit() {
    LoadSettings();
    ZIH_LOG(L"zen-ime-caret-hint 0.2.9 up: ime=%d caps=%d classes=\"%s\"",
            g_settings.showOnImeToggle ? 1 : 0, g_settings.showOnCapsLock ? 1 : 0,
            g_settings.editorClasses.c_str());

    g_singleInstance = CreateMutexW(nullptr, FALSE, L"Local\\ZenImeCaretHintHost");
    if (g_singleInstance && GetLastError() == ERROR_ALREADY_EXISTS) {
        ZIH_LOG(L"another explorer instance is hosting; standing by (pid=%lu)",
                GetCurrentProcessId());
        CloseHandle(g_singleInstance);
        g_singleInstance = nullptr;
        return;
    }

    g_hostThread = CreateThread(nullptr, 0, HostThread, nullptr, 0, nullptr);
    if (!g_hostThread) {
        ZIH_LOG(L"Create host thread failed: %lu", GetLastError());
    }
}

void Wh_ModUninit() {
    SetEvent(g_stopEvent);
    if (g_hostThread) {
        WaitForSingleObject(g_hostThread, INFINITE);
        CloseHandle(g_hostThread);
        g_hostThread = nullptr;
    }
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
    if (g_singleInstance) {
        CloseHandle(g_singleInstance);
        g_singleInstance = nullptr;
    }
    ZIH_LOG(L"uninit");
}

void Wh_ModSettingsChanged() {
    g_settingsChanged.store(true);
}
