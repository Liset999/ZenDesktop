// ==WindhawkMod==
// @id              zen-desktop-clock
// @name            ZenDesktop: 桌面时钟
// @name:zh-CN      ZenDesktop: 桌面时钟
// @description     Clock, calendar and resource widgets behind the desktop icons. Each can be shown, resized and dragged on its own.
// @description:zh-CN 在桌面图标后方显示时钟、日历和资源占用（CPU、内存、GPU、网速），三块可单独开关、单独拖动。
// @version         1.4.0
// @author          Lanbo, m417z
// @github          https://github.com/Liset999
// @donateUrl       https://ifdian.net/a/Lanboo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ld2d1 -ldwrite -ld3d11 -ldxgi -ldcomp -lshcore -luuid -lole32 -lgdi32 -ladvapi32 -lpdh -liphlpapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The desktop host lookup (WorkerW behind the icons), the DirectComposition
// swap chain set-up and the window life cycle are adapted from m417z's
// "Desktop Live Overlay" (GPL-3.0):
// https://github.com/m417z/my-windhawk-mods · https://windhawk.net/mods/desktop-live-overlay
// Clock layout, presets and effects (rotation, shadow, glow, outline, flip
// cards) are written for ZenDesktop by Lanbo.

// ==WindhawkModReadme==
/*
> **作者：Lanbo**
> [B站](https://space.bilibili.com/1280632999) · [抖音](https://v.douyin.com/Isl7QPpKZZ4/) · [爱发电](https://ifdian.net/a/Lanboo) · [微信赞赏](https://raw.githubusercontent.com/Liset999/ZenDesktop/main/docs/donate-wechat.png)

# ZenDesktop: 桌面时钟

在桌面图标**后方**放小组件，不挡图标、不抢焦点。关掉拖动后不吃鼠标。

- **三块，各自开关、各自大小、各自拖动**：时钟、日历（整月、紧凑、本周、今天、一行，可选农历）、资源占用（CPU、内存、GPU、网速可以单独关，每 2 秒刷新）。拖框中间移动，拖四个角连续改大小。
- **摆位置**：「拖动调整位置」和「重置拖动位置」在设置最上面。打开拖动后每块出现一个半透明框，拖好后关掉。重置打开一次就忘掉拖过的位置。
- **17 种样式**加自定义。颜色、24 小时、秒、日期、农历和九宫格都在「高级」里。

基于 m417z 的 Desktop Live Overlay（GPL-3.0）的桌面宿主与渲染框架。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dragMode: false
  $name: "Drag to position"
  $name:zh-CN: "拖动调整位置"
  $description: "Drag the box to move. Drag a corner to resize freely, then turn this off."
  $description:zh-CN: "拖框中间移动，拖四个角连续改大小。调好后把这项关掉。"
- resetPosition: false
  $name: "Reset dragged position"
  $name:zh-CN: "重置拖动位置"
  $description: "Turn on once to forget every dragged position, then turn it off again."
  $description:zh-CN: "打开一次即清除时钟、日历、资源占用记住的位置，之后请再关掉。"
- style: tilted
  $name: "Style"
  $name:zh-CN: "样式"
  $options:
  - tilted: "Tilted shadow"
  - minimal: "Minimal"
  - neon: "Neon glow"
  - flip: "Flip cards"
  - cover: "Magazine cover"
  - stacked: "Stacked"
  - hollow: "Hollow outline"
  - glass: "Glass card"
  - segment: "Retro 7-segment"
  - rainbow: "Rainbow gradient"
  - sunset: "Sunset gradient"
  - chinese: "Chinese numerals"
  - sentence: "Word clock"
  - analog: "Analog dial"
  - ring: "Progress ring"
  - accent: "Accent bar"
  - glitch: "Two-tone glitch"
  - custom: "Custom (Advanced)"
  $options:zh-CN:
  - tilted: "倾斜阴影"
  - minimal: "极简"
  - neon: "霓虹"
  - flip: "翻页"
  - cover: "大字封面"
  - stacked: "竖排"
  - hollow: "描边空心"
  - glass: "玻璃卡片"
  - segment: "复古数码管"
  - rainbow: "彩虹渐变"
  - sunset: "日落渐变"
  - chinese: "中文大写"
  - sentence: "文字时钟"
  - analog: "圆形表盘"
  - ring: "进度环"
  - accent: "左侧细线"
  - glitch: "双色错位"
  - custom: "自定义（用「高级」设置）"
- showClock: true
  $name: "Clock"
  $name:zh-CN: "时钟"
  $description: "Turn off to keep only the calendar or the resource widget."
  $description:zh-CN: "关掉后可以只留日历或资源占用。"
- size: 100
  $name: "Clock size (%)"
  $name:zh-CN: "时钟大小（%）"
  $description: "20-800. Or drag a corner while Drag to position is on."
  $description:zh-CN: "20-800。也可以在「拖动调整位置」时拖角落连续改。"
- calendar: none
  $name: "Calendar"
  $name:zh-CN: "日历"
  $options:
  - none: "Off"
  - on: "On"
  - right: "On (was: right of clock)"
  - below: "On (was: below clock)"
  $options:zh-CN:
  - none: "关闭"
  - on: "开启"
  - right: "开启（以前在右侧）"
  - below: "开启（以前在下方）"
  $description: "Its own widget. Drag to place it. Today is marked with the accent color."
  $description:zh-CN: "单独一块，用「拖动调整位置」摆放。今天用强调色标出。"
- calendarStyle: month
  $name: "Calendar style"
  $name:zh-CN: "日历样式"
  $options:
  - month: "Month"
  - compact: "Compact month"
  - week: "This week"
  - today: "Today"
  - strip: "One line"
  $options:zh-CN:
  - month: "整月"
  - compact: "紧凑整月"
  - week: "本周"
  - today: "今天"
  - strip: "一行日期"
  $description: "Compact hides lunar dates and uses smaller cells."
  $description:zh-CN: "紧凑整月不显示农历，格子更小。其余样式仍按「高级」里的农历开关。"
- calendarSize: 100
  $name: "Calendar size (%)"
  $name:zh-CN: "日历大小（%）"
  $description: "20-800. Or drag a corner."
  $description:zh-CN: "20-800。也可以拖角落连续改。"
- showResources: false
  $name: "Resource usage"
  $name:zh-CN: "资源占用"
  $description: "Its own widget, refreshed every 2 s. Pick the rows below. GPU shows -- if it cannot be read."
  $description:zh-CN: "单独一块，每 2 秒刷新。下面勾选要显示的项。读不到 GPU 时显示 --。"
- resCpu: true
  $name: "CPU"
  $name:zh-CN: "CPU"
- resMem: true
  $name: "Memory"
  $name:zh-CN: "内存"
- resGpu: true
  $name: "GPU"
  $name:zh-CN: "GPU"
- resNet: true
  $name: "Network speed"
  $name:zh-CN: "网速"
- resourcesSize: 100
  $name: "Usage size (%)"
  $name:zh-CN: "资源占用大小（%）"
  $description: "20-800. Or drag a corner."
  $description:zh-CN: "20-800。也可以拖角落连续改。"
- monitor: 1
  $name: "Monitor"
  $name:zh-CN: "显示器"
  $description: "1 = first, 2 = second… 0 = all."
  $description:zh-CN: "1 = 第一个，2 = 第二个……0 = 全部。"
- advanced:
  - anchor: center
    $name: "Position"
    $name:zh-CN: "位置"
    $options:
    - topLeft: "Top left"
    - top: "Top"
    - topRight: "Top right"
    - left: "Left"
    - center: "Center"
    - right: "Right"
    - bottomLeft: "Bottom left"
    - bottom: "Bottom"
    - bottomRight: "Bottom right"
    $options:zh-CN:
    - topLeft: "左上"
    - top: "上"
    - topRight: "右上"
    - left: "左"
    - center: "正中"
    - right: "右"
    - bottomLeft: "左下"
    - bottom: "下"
    - bottomRight: "右下"
  - offset:
    - x: 0
      $name: "Horizontal"
      $name:zh-CN: "水平（正数向右）"
    - y: -80
      $name: "Vertical"
      $name:zh-CN: "垂直（正数向下）"
    $name: "Fine offset (px at 100%)"
    $name:zh-CN: "微调偏移（像素）"
  - use24Hour: true
    $name: "24-hour time"
    $name:zh-CN: "24 小时制"
  - showSeconds: false
    $name: "Show seconds"
    $name:zh-CN: "显示秒"
  - dateFormat: cn
    $name: "Date"
    $name:zh-CN: "日期格式"
    $options:
    - cn: "10月28日 周三"
    - cnFull: "2026年10月28日 星期三"
    - iso: "2026-10-28"
    - mdy: "10/28/2026"
    - dmy: "28/10/2026"
    - none: "Hidden"
    $options:zh-CN:
    - cn: "10月28日 周三"
    - cnFull: "2026年10月28日 星期三"
    - iso: "2026-10-28"
    - mdy: "10/28/2026"
    - dmy: "28/10/2026"
    - none: "不显示"
  - lunar: true
    $name: "Lunar dates in calendar"
    $name:zh-CN: "日历显示农历"
  - color: default
    $name: "Text color"
    $name:zh-CN: "颜色"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "跟随样式"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - customColor: ""
    $name: "Custom text color code"
    $name:zh-CN: "自定义颜色代码"
    $description: "Only used when Custom is selected above. #RRGGBB or #AARRGGBB."
    $description:zh-CN: "只在上面选「自定义」时生效，格式 #RRGGBB 或 #AARRGGBB（如 #FF8800）。"
  - accentColor: default
    $name: "Accent color"
    $name:zh-CN: "强调色（进度环、竖线、秒针、日历今天）"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "跟随样式"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - customAccent: ""
    $name: "Custom accent color code"
    $name:zh-CN: "自定义强调色代码"
    $description: "Only used when Custom is selected above. #RRGGBB or #AARRGGBB."
    $description:zh-CN: "只在上面选「自定义」时生效，格式 #RRGGBB 或 #AARRGGBB（如 #FF8800）。"
  - layout: normal
    $name: "Layout"
    $name:zh-CN: "版式"
    $options:
    - normal: "Time + date"
    - stacked: "Hours over minutes"
    - cover: "Date above"
    - sentence: "Sentence"
    - chinese: "Chinese numerals"
    - analog: "Analog dial"
    - ring: "Progress ring"
    $options:zh-CN:
    - normal: "时间 + 日期"
    - stacked: "竖排"
    - cover: "日期在上"
    - sentence: "文字句子"
    - chinese: "中文大写"
    - analog: "圆形表盘"
    - ring: "进度环"
  - timeFont: "Segoe UI Variable Display"
    $name:zh-CN: "时间字体"
  - timeFontSize: 140
    $name:zh-CN: "时间字号"
  - timeFontWeight: 600
    $name:zh-CN: "时间字重（100–900）"
  - dateFont: "Segoe UI Variable Display"
    $name:zh-CN: "日期字体"
  - dateFontSize: 40
    $name:zh-CN: "日期字号"
  - dateFontWeight: 500
    $name:zh-CN: "日期字重"
  - textAlign: left
    $name:zh-CN: "行对齐"
    $options:
    - left: "Left"
    - center: "Center"
    - right: "Right"
    $options:zh-CN:
    - left: "左"
    - center: "中"
    - right: "右"
  - lineGap: 4
    $name:zh-CN: "行间距"
  - rotation: -8
    $name:zh-CN: "旋转角度（-45~45）"
  - shadow: true
    $name:zh-CN: "投影"
  - shadowColor: default
    $name:zh-CN: "投影颜色"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "默认"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - shadowColorCustom: ""
    $name:zh-CN: "投影颜色：自定义代码（选「自定义」时用，#RRGGBB / #AARRGGBB）"
  - shadowBlur: 8
    $name:zh-CN: "投影模糊"
  - shadowOffsetX: 6
    $name:zh-CN: "投影水平偏移"
  - shadowOffsetY: 8
    $name:zh-CN: "投影垂直偏移"
  - glow: false
    $name:zh-CN: "光晕"
  - glowColor: default
    $name:zh-CN: "光晕颜色"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "默认"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - glowColorCustom: ""
    $name:zh-CN: "光晕颜色：自定义代码（选「自定义」时用，#RRGGBB / #AARRGGBB）"
  - glowRadius: 12
    $name:zh-CN: "光晕半径"
  - outline: false
    $name:zh-CN: "描边"
  - outlineColor: default
    $name:zh-CN: "描边颜色"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "默认"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - outlineColorCustom: ""
    $name:zh-CN: "描边颜色：自定义代码（选「自定义」时用，#RRGGBB / #AARRGGBB）"
  - outlineWidth: 2
    $name:zh-CN: "描边宽度"
  - hollow: false
    $name:zh-CN: "空心字（只留描边）"
  - gradientStart: default
    $name:zh-CN: "渐变起始色（起止都选才启用）"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "默认"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - gradientStartCustom: ""
    $name:zh-CN: "渐变起始色（起止都选才启用）：自定义代码（选「自定义」时用，#RRGGBB / #AARRGGBB）"
  - gradientEnd: default
    $name:zh-CN: "渐变结束色"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "默认"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - gradientEndCustom: ""
    $name:zh-CN: "渐变结束色：自定义代码（选「自定义」时用，#RRGGBB / #AARRGGBB）"
  - flipCards: false
    $name:zh-CN: "翻页卡片底"
  - cardColor: default
    $name:zh-CN: "翻页卡片颜色"
    $options:
    - default: "Follow style"
    - white: "White"
    - black: "Black"
    - lightgray: "Light gray"
    - red: "Red"
    - orange: "Orange"
    - gold: "Gold"
    - green: "Green"
    - cyan: "Cyan"
    - blue: "Blue"
    - purple: "Purple"
    - pink: "Pink"
    - system: "System accent color"
    - custom: "Custom (fill in below)"
    $options:zh-CN:
    - default: "默认"
    - white: "白色"
    - black: "黑色"
    - lightgray: "浅灰"
    - red: "红"
    - orange: "橙"
    - gold: "金黄"
    - green: "绿"
    - cyan: "青"
    - blue: "蓝"
    - purple: "紫"
    - pink: "粉"
    - system: "跟随系统强调色"
    - custom: "自定义（填下面的代码）"
  - cardColorCustom: ""
    $name:zh-CN: "翻页卡片颜色：自定义代码（选「自定义」时用，#RRGGBB / #AARRGGBB）"
  - glassCard: false
    $name:zh-CN: "玻璃卡片底"
  - accentBar: false
    $name:zh-CN: "左侧竖线"
  - showWeekday: false
    $name:zh-CN: "日期加星期"
  $name: "Advanced"
  $name:zh-CN: "高级"
  $description: "Color, time format, lunar dates, anchor, and custom-style details."
  $description:zh-CN: "颜色、时间格式、农历、九宫格，以及自定义样式的细调。平时不用打开。"

*/
// ==/WindhawkModSettings==

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-W#warnings"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma clang diagnostic pop

#include <windhawk_utils.h>

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwrite.h>
#include <dxgi1_3.h>
#include <shellscalingapi.h>
#include <wrl/client.h>
#include <dwrite_1.h>

#include <atomic>
#include <cmath>
#include <string>
#include <vector>

#include <netioapi.h>
#include <iphlpapi.h>
#include <pdh.h>
#include <pdhmsg.h>

using Microsoft::WRL::ComPtr;

#define ZC_TIMER_REFRESH 1
#define ZC_TIMER_ZORDER 0x5A43
#define ZC_TIMER_MSG_DISPLAY 1
#define ZC_TIMER_MSG_RECREATE 2
#define ZC_WM_CLEANUP (WM_APP + 1)
#define ZC_WM_SETTINGS (WM_APP + 2)
#define ZC_OVERLAY_CLASS (L"ZenDesktopClock_" WH_MOD_ID)
#define ZC_MESSAGE_CLASS (L"ZenDesktopClockMsg_" WH_MOD_ID)

// D2D built-in effect CLSIDs (defined locally so no dxguid import is needed).
// CLSID_D2D1Shadow {C67EA361-1863-4E69-89DB-695D3E9A5B6B}
static const CLSID kZcClsidShadow = {
    0xc67ea361, 0x1863, 0x4e69, {0x89, 0xdb, 0x69, 0x5d, 0x3e, 0x9a, 0x5b, 0x6b}};
// CLSID_D2D1Morphology {EAE6C40D-626A-4C2D-BFCB-391001ABE202}
static const CLSID kZcClsidMorphology = {
    0xeae6c40d, 0x626a, 0x4c2d, {0xbf, 0xcb, 0x39, 0x10, 0x01, 0xab, 0xe2, 0x02}};
// Property indices (D2D1_SHADOW_PROP / D2D1_MORPHOLOGY_PROP).
enum : UINT32 {
    kZcShadowPropBlur = 0,
    kZcShadowPropColor = 1,
    kZcMorphPropMode = 0,
    kZcMorphPropWidth = 1,
    kZcMorphPropHeight = 2,
    kZcMorphModeDilate = 1,
};

enum class ZcDate { mdy, dmy, iso, cn, cnFull, none };
enum class ZcAlign { left, center, right };

enum class ZcLayoutKind { normal, stacked, cover, sentence, chinese, analog, ring };

struct ZcStyle {
    std::wstring timeFont, dateFont;
    int timeSize = 140, timeWeight = 600, dateSize = 40, dateWeight = 500;
    D2D1_COLOR_F text{1, 1, 1, 1};
    float rotation = 0;
    bool shadow = false;
    D2D1_COLOR_F shadowColor{0, 0, 0, 1};
    float shadowBlur = 8, shadowDx = 6, shadowDy = 8;
    bool glow = false;
    D2D1_COLOR_F glowColor{0, 0.9f, 1, 1};
    float glowRadius = 12;
    bool outline = false;
    D2D1_COLOR_F outlineColor{0, 0, 0, 1};
    float outlineWidth = 2;
    bool flip = false;
    D2D1_COLOR_F cardColor{0.125f, 0.125f, 0.14f, 0.88f};
    // 1.1.0 additions.
    ZcLayoutKind layout = ZcLayoutKind::normal;
    bool hollow = false;              // outline only, transparent fill
    std::vector<D2D1_COLOR_F> grad;   // >= 2 stops: gradient text fill
    bool glass = false;               // translucent rounded card behind block
    D2D1_COLOR_F glassColor{1, 1, 1, 0.16f};
    D2D1_COLOR_F glassBorder{1, 1, 1, 0.38f};
    bool ghost = false;               // dim "88:88" behind the digits
    D2D1_COLOR_F ghostColor{1, 1, 1, 0.08f};
    bool accentBar = false;           // vertical bar on the left
    D2D1_COLOR_F accent{1, 0.42f, 0.2f, 1};
    bool weekday = false;             // force weekday into the date line
    bool dup = false;                 // two-tone offset duplicates
    D2D1_COLOR_F dupA{0, 0.9f, 1, 0.85f}, dupB{1, 0.1f, 0.55f, 0.85f};
    float dupOffset = 5;
    bool dateAbove = false;
    float dateTracking = 0;           // extra letter spacing for the date
    bool upper = false;               // uppercase date text
    D2D1_COLOR_F track{1, 1, 1, 0.18f};  // ring track / dial face line
};

enum class ZcCalKind { month, compact, week, today, strip };

struct ZcSettings {
    bool showClock = true;
    bool use24 = true, seconds = false;
    ZcDate date = ZcDate::cn;
    ZcAlign align = ZcAlign::left;
    int anchor = 4;  // 0..8, used until a widget has been dragged
    int offsetX = 0, offsetY = -80, monitor = 1, lineGap = 4;
    int size = 100, calendarSize = 100, resourcesSize = 100;
    bool resources = false;
    bool resCpu = true, resMem = true, resGpu = true, resNet = true;
    bool calendar = false;
    bool lunar = true;
    ZcCalKind calKind = ZcCalKind::month;
    bool drag = false, reset = false;
    ZcStyle style;
} g_zc;

bool ZcResOn() {
    return g_zc.resources && (g_zc.resCpu || g_zc.resMem || g_zc.resGpu || g_zc.resNet);
}

// Default calendar spot before it is dragged: 1 = right of the clock, 2 = below.
int g_zcCalPlace = 1;

// Latest resource sample. GPU stays unknown until a PDH sample succeeds.
float g_zcCpu = 0, g_zcMem = 0, g_zcGpu = 0;
bool g_zcGpuKnown = false;
double g_zcDownBps = 0, g_zcUpBps = 0;
ULONGLONG g_zcPrevIdle = 0, g_zcPrevTotal = 0;
PDH_HQUERY g_zcGpuQuery = nullptr;
PDH_HCOUNTER g_zcGpuCounter = nullptr;
bool g_zcGpuOpen = false;
ULONGLONG g_zcNetIn = 0, g_zcNetOut = 0, g_zcNetTick = 0;
bool g_zcNetHave = false;

ULONGLONG ZcFt(const FILETIME& f) {
    return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime;
}

void ZcGpuClose() {
    if (g_zcGpuQuery) PdhCloseQuery(g_zcGpuQuery);
    g_zcGpuQuery = nullptr;
    g_zcGpuCounter = nullptr;
    g_zcGpuOpen = false;
}

// English counter path works on a Chinese Windows install.
bool ZcGpuEnsure() {
    if (g_zcGpuOpen) return g_zcGpuQuery != nullptr;
    g_zcGpuOpen = true;
    if (PdhOpenQuery(nullptr, 0, &g_zcGpuQuery) != ERROR_SUCCESS) {
        g_zcGpuQuery = nullptr;
        return false;
    }
    if (PdhAddEnglishCounterW(g_zcGpuQuery, L"\\GPU Engine(*)\\Utilization Percentage", 0,
                              &g_zcGpuCounter) != ERROR_SUCCESS) {
        ZcGpuClose();
        g_zcGpuOpen = true;  // don't retry every sample
        return false;
    }
    return true;
}

void ZcSampleGpu() {
    if (!ZcGpuEnsure()) return;
    if (PdhCollectQueryData(g_zcGpuQuery) != ERROR_SUCCESS) return;
    DWORD bytes = 0, count = 0;
    PDH_STATUS st = PdhGetFormattedCounterArrayW(
        g_zcGpuCounter, PDH_FMT_DOUBLE, &bytes, &count, nullptr);
    if (st != PDH_MORE_DATA || bytes == 0) return;
    std::vector<unsigned char> buf(bytes);
    auto* items = (PDH_FMT_COUNTERVALUE_ITEM_W*)buf.data();
    st = PdhGetFormattedCounterArrayW(g_zcGpuCounter, PDH_FMT_DOUBLE, &bytes, &count, items);
    if (st != ERROR_SUCCESS) return;
    double best3d = -1, bestAny = -1;
    for (DWORD i = 0; i < count; i++) {
        if (items[i].FmtValue.CStatus != ERROR_SUCCESS) continue;
        double v = items[i].FmtValue.doubleValue;
        if (v < 0) continue;
        if (v > bestAny) bestAny = v;
        if (items[i].szName && wcsstr(items[i].szName, L"engtype_3D") && v > best3d)
            best3d = v;
    }
    double pick = best3d >= 0 ? best3d : bestAny;
    if (pick < 0) return;
    if (pick > 100) pick = 100;
    g_zcGpu = (float)pick;
    g_zcGpuKnown = true;
}

void ZcSampleNet() {
    PMIB_IF_TABLE2 table = nullptr;
    if (GetIfTable2(&table) != NO_ERROR) {
        if (table) FreeMibTable(table);
        return;
    }
    ULONGLONG inB = 0, outB = 0;
    for (ULONG i = 0; i < table->NumEntries; i++) {
        const MIB_IF_ROW2& row = table->Table[i];
        if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK || row.Type == IF_TYPE_TUNNEL) continue;
        if (!row.InterfaceAndOperStatusFlags.HardwareInterface) continue;
        if (row.OperStatus != IfOperStatusUp) continue;
        inB += row.InOctets;
        outB += row.OutOctets;
    }
    FreeMibTable(table);
    ULONGLONG now = GetTickCount64();
    if (!g_zcNetHave) {
        g_zcNetIn = inB;
        g_zcNetOut = outB;
        g_zcNetTick = now;
        g_zcNetHave = true;
        return;
    }
    if (now <= g_zcNetTick) return;
    double dt = (double)(now - g_zcNetTick) / 1000.0;
    if (dt < 0.2) return;  // keep the baseline so the next delta stays meaningful
    if (inB >= g_zcNetIn) g_zcDownBps = (double)(inB - g_zcNetIn) / dt;
    if (outB >= g_zcNetOut) g_zcUpBps = (double)(outB - g_zcNetOut) / dt;
    g_zcNetIn = inB;
    g_zcNetOut = outB;
    g_zcNetTick = now;
}

void ZcSampleResources() {
    FILETIME idle, kernel, user;
    if (g_zc.resCpu && GetSystemTimes(&idle, &kernel, &user)) {
        ULONGLONG i = ZcFt(idle), total = ZcFt(kernel) + ZcFt(user);  // kernel includes idle
        if (g_zcPrevTotal && total > g_zcPrevTotal) {
            double busy = 1.0 - (double)(i - g_zcPrevIdle) / (double)(total - g_zcPrevTotal);
            g_zcCpu = (float)(busy < 0 ? 0 : busy > 1 ? 100 : busy * 100);
        }
        g_zcPrevIdle = i;
        g_zcPrevTotal = total;
    }
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    if (g_zc.resMem && GlobalMemoryStatusEx(&ms)) g_zcMem = (float)ms.dwMemoryLoad;
    if (g_zc.resGpu) ZcSampleGpu();
    if (g_zc.resNet) ZcSampleNet();
}

std::atomic<bool> g_zcUnloading{false};
std::atomic<bool> g_zcLazyInit{false};
std::atomic<bool> g_zcInitOk{false};
HWND g_zcOverlayWnd = nullptr;
HWND g_zcMessageWnd = nullptr;
UINT_PTR g_zcCreateTimer = 0;
UINT g_zcTaskbarCreatedMsg = 0;

[[clang::no_destroy]] ComPtr<ID3D11Device> g_zcD3d;
[[clang::no_destroy]] ComPtr<IDXGIDevice> g_zcDxgiDevice;
[[clang::no_destroy]] ComPtr<IDXGIFactory2> g_zcDxgiFactory;
[[clang::no_destroy]] ComPtr<ID2D1Factory1> g_zcD2dFactory;
[[clang::no_destroy]] ComPtr<ID2D1Device> g_zcD2dDevice;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_zcDwrite;
[[clang::no_destroy]] ComPtr<IDXGISwapChain1> g_zcSwapChain;
[[clang::no_destroy]] ComPtr<ID2D1DeviceContext> g_zcDc;
[[clang::no_destroy]] ComPtr<ID2D1Bitmap1> g_zcTarget;
[[clang::no_destroy]] ComPtr<IDCompositionDevice> g_zcCompDevice;
[[clang::no_destroy]] ComPtr<IDCompositionTarget> g_zcCompTarget;
[[clang::no_destroy]] ComPtr<IDCompositionVisual> g_zcCompVisual;

////////////////////////////////////////////////////////////////////////////////
// Utilities (RunFromWindowThread / GetCurrentModuleHandle / FolderView and
// WorkerW lookup adapted from m417z's Desktop Live Overlay, GPL-3.0)

using ZcThreadProc_t = void(WINAPI*)(void*);

bool ZcRunFromWindowThread(HWND hWnd, ZcThreadProc_t proc, void* procParam) {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param {
        ZcThreadProc_t proc;
        void* procParam;
    };
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == msg) {
                    Param* p = (Param*)cwp->lParam;
                    p->proc(p->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }
    Param p{proc, procParam};
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

HMODULE ZcModule() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&ZcModule, &module);
    return module;
}

bool ZcIsFolderView(HWND hWnd) {
    WCHAR buf[64];
    if (!GetClassName(hWnd, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"SysListView32")) {
        return false;
    }
    if (!GetWindowText(hWnd, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"FolderView")) {
        return false;
    }
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    if (!parent || !GetClassName(parent, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"SHELLDLL_DefView") || GetWindowTextLength(parent) > 0) {
        return false;
    }
    HWND parent2 = GetAncestor(parent, GA_PARENT);
    if (!parent2) {
        return false;
    }
    if ((!GetClassName(parent2, buf, ARRAYSIZE(buf)) ||
         _wcsicmp(buf, L"Progman")) &&
        parent2 != GetShellWindow()) {
        return false;
    }
    return true;
}

HWND ZcGetProgman() {
    HWND progman = FindWindow(L"Progman", nullptr);
    if (!progman) {
        return nullptr;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(progman, &pid);
    return pid == GetCurrentProcessId() ? progman : nullptr;
}

BOOL CALLBACK ZcFindWorkerProc(HWND hWnd, LPARAM lParam) {
    if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
        return TRUE;
    }
    HWND worker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
    if (worker) {
        *(HWND*)lParam = worker;
        return FALSE;
    }
    return TRUE;
}

// The WorkerW window that sits behind the desktop icons.
HWND ZcGetWorkerW() {
    HWND progman = ZcGetProgman();
    if (!progman) {
        return nullptr;
    }
    SendMessage(progman, 0x052C, 0xD, 0);
    SendMessage(progman, 0x052C, 0xD, 1);
    HWND worker = nullptr;
    EnumWindows(ZcFindWorkerProc, (LPARAM)&worker);
    if (!worker) {
        SendMessage(progman, 0x052C, 0, 0);
        EnumWindows(ZcFindWorkerProc, (LPARAM)&worker);
    }
    if (!worker) {
        // 24H2+: WorkerW is a child of Progman.
        worker = FindWindowEx(progman, nullptr, L"WorkerW", nullptr);
    }
    return worker ? worker : progman;
}

float ZcMonitorScale(HMONITOR monitor) {
    UINT dx = 96, dy = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dx, &dy))) {
        return dx / 96.0f;
    }
    return 1.0f;
}

BOOL CALLBACK ZcEnumMonitorProc(HMONITOR m, HDC, LPRECT, LPARAM lParam) {
    ((std::vector<HMONITOR>*)lParam)->push_back(m);
    return TRUE;
}

std::vector<HMONITOR> ZcTargetMonitors() {
    std::vector<HMONITOR> all;
    EnumDisplayMonitors(nullptr, nullptr, ZcEnumMonitorProc, (LPARAM)&all);
    if (g_zc.monitor <= 0) {
        return all;
    }
    if (g_zc.monitor <= (int)all.size()) {
        return {all[g_zc.monitor - 1]};
    }
    return {MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)};
}

bool ZcParseColor(PCWSTR s, D2D1_COLOR_F* out) {
    if (!s) {
        return false;
    }
    if (*s == L'#') {
        s++;
    }
    size_t len = wcslen(s);
    if (len != 6 && len != 8) {
        return false;
    }
    WCHAR* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (!end || *end) {
        return false;
    }
    unsigned a = len == 8 ? (v >> 24) & 0xFF : 0xFF;
    out->r = ((v >> 16) & 0xFF) / 255.0f;
    out->g = ((v >> 8) & 0xFF) / 255.0f;
    out->b = (v & 0xFF) / 255.0f;
    out->a = a / 255.0f;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

std::wstring ZcStr(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

D2D1_COLOR_F ZcColor(PCWSTR name, D2D1_COLOR_F def) {
    std::wstring s = ZcStr(name);
    D2D1_COLOR_F c = def;
    if (!ZcParseColor(s.c_str(), &c)) {
        c = def;
    }
    return c;
}

// System accent color: HKCU\Software\Microsoft\Windows\DWM\AccentColor (0xAABBGGRR).
bool ZcSystemAccent(D2D1_COLOR_F* out) {
    DWORD v = 0, cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &v, &cb) != ERROR_SUCCESS) {
        return false;
    }
    out->r = (v & 0xFF) / 255.0f;
    out->g = ((v >> 8) & 0xFF) / 255.0f;
    out->b = ((v >> 16) & 0xFF) / 255.0f;
    out->a = 1;
    return true;
}

// Dropdown color: preset name, "system", "custom" (+ custom field), or a legacy
// hex string (old versions stored the code directly). false = keep style default.
bool ZcApplyChoice(const std::wstring& v, const std::wstring& custom, D2D1_COLOR_F* out) {
    if (v.empty() || v == L"default") return false;
    if (v == L"custom") return ZcParseColor(custom.c_str(), out);
    if (v == L"system") return ZcSystemAccent(out);
    static const struct { const wchar_t* n; unsigned rgb; } k[] = {
        {L"white", 0xFFFFFF}, {L"black", 0x000000}, {L"lightgray", 0xC8C8C8},
        {L"red", 0xFF3B30},   {L"orange", 0xFF9500}, {L"gold", 0xFFCC00},
        {L"green", 0x34C759}, {L"cyan", 0x00E5FF},   {L"blue", 0x0A84FF},
        {L"purple", 0xAF52DE}, {L"pink", 0xFF5FA2},
    };
    for (auto& e : k) {
        if (v == e.n) {
            *out = {((e.rgb >> 16) & 0xFF) / 255.0f, ((e.rgb >> 8) & 0xFF) / 255.0f,
                    (e.rgb & 0xFF) / 255.0f, 1};
            return true;
        }
    }
    return ZcParseColor(v.c_str(), out);  // legacy hex
}

bool ZcChoiceColor(PCWSTR choiceKey, PCWSTR customKey, D2D1_COLOR_F* out) {
    return ZcApplyChoice(ZcStr(choiceKey), ZcStr(customKey), out);
}

std::wstring ZcStored(PCWSTR key) {
    WCHAR b[180];
    b[0] = 0;
    Wh_GetStringValue(key, b, ARRAYSIZE(b));
    return b;
}

std::wstring ZcPinnedStr(PCWSTR advKey, PCWSTR legacyKey, PCWSTR pinKey, PCWSTR seenKey) {
    std::wstring now = ZcStr(advKey);
    std::wstring seen = ZcStored(seenKey);
    if (seen.empty()) {
        std::wstring leg = ZcStr(legacyKey);
        Wh_SetStringValue(seenKey, now.c_str());
        if (!leg.empty() && leg != now) {
            Wh_SetStringValue(pinKey, leg.c_str());
            return leg;
        }
        return now;
    }
    if (now != seen) {
        Wh_SetStringValue(seenKey, now.c_str());
        Wh_SetStringValue(pinKey, L"");
        return now;
    }
    std::wstring pin = ZcStored(pinKey);
    return pin.empty() ? now : pin;
}

int ZcPinnedInt(PCWSTR advKey, PCWSTR legacyKey, PCWSTR pinKey, PCWSTR seenKey) {
    int now = Wh_GetIntSetting(advKey);
    std::wstring seen = ZcStored(seenKey);
    WCHAR buf[32];
    if (seen.empty()) {
        swprintf_s(buf, L"%d", now);
        Wh_SetStringValue(seenKey, buf);
        std::wstring legS = ZcStr(legacyKey);
        if (!legS.empty()) {
            int leg = _wtoi(legS.c_str());
            if (leg != now) {
                swprintf_s(buf, L"%d", leg);
                Wh_SetStringValue(pinKey, buf);
                return leg;
            }
        }
        return now;
    }
    if (now != _wtoi(seen.c_str())) {
        swprintf_s(buf, L"%d", now);
        Wh_SetStringValue(seenKey, buf);
        Wh_SetStringValue(pinKey, L"");
        return now;
    }
    std::wstring pin = ZcStored(pinKey);
    return pin.empty() ? now : _wtoi(pin.c_str());
}

int ZcClamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Corner-drag size overrides the percent box until that box is edited.
int ZcLivePct(PCWSTR settingKey, PCWSTR pinKey, PCWSTR seenKey) {
    int now = Wh_GetIntSetting(settingKey);
    if (now <= 0) now = 100;
    int seen = Wh_GetIntValue(seenKey, -1);
    if (seen < 0) {
        Wh_SetIntValue(seenKey, now);
        return ZcClamp(now, 20, 800);
    }
    if (now != seen) {
        Wh_SetIntValue(seenKey, now);
        Wh_SetIntValue(pinKey, 0);
        return ZcClamp(now, 20, 800);
    }
    int pin = Wh_GetIntValue(pinKey, 0);
    return ZcClamp(pin > 0 ? pin : now, 20, 800);
}

// Chinese font with fallbacks (first installed family wins).
std::wstring ZcPickFont(std::initializer_list<const wchar_t*> names) {
    ComPtr<IDWriteFontCollection> fc;
    if (g_zcDwrite && SUCCEEDED(g_zcDwrite->GetSystemFontCollection(&fc))) {
        for (auto n : names) {
            UINT32 idx;
            BOOL exists = FALSE;
            if (SUCCEEDED(fc->FindFamilyName(n, &idx, &exists)) && exists) {
                return n;
            }
        }
    }
    return *names.begin();
}

void ZcApplyPreset(const std::wstring& name, ZcStyle& s) {
    const wchar_t* kFont = L"Segoe UI Variable Display";
    s = ZcStyle{};
    s.timeFont = kFont;
    s.dateFont = kFont;
    auto softShadow = [&](float a, float blur, float dx, float dy) {
        s.shadow = true;
        s.shadowColor = {0, 0, 0, a};
        s.shadowBlur = blur;
        s.shadowDx = dx;
        s.shadowDy = dy;
    };
    if (name == L"tilted") {
        // White large text, tilted ~-8 degrees, dark soft shadow.
        s.timeSize = 150;
        s.timeWeight = 700;
        s.dateSize = 44;
        s.dateWeight = 600;
        s.rotation = -8;
        softShadow(0.75f, 10, 6, 10);
    } else if (name == L"minimal") {
        s.timeFont = L"Segoe UI Variable Display Light";
        s.timeSize = 120;
        s.timeWeight = 200;
        s.dateSize = 30;
        s.dateWeight = 400;
        s.text = {1, 1, 1, 0.92f};
        softShadow(0.35f, 4, 0, 2);
    } else if (name == L"neon") {
        s.timeFont = s.dateFont = L"Bahnschrift";
        s.timeSize = 130;
        s.timeWeight = 600;
        s.dateSize = 38;
        s.dateWeight = 500;
        s.text = {0.88f, 1, 1, 1};
        s.glow = true;
        s.glowColor = {0, 0.9f, 1, 1};
        s.glowRadius = 14;
    } else if (name == L"flip") {
        s.timeFont = L"Bahnschrift";
        s.timeSize = 120;
        s.timeWeight = 700;
        s.dateSize = 34;
        s.dateWeight = 600;
        s.flip = true;
        s.cardColor = {0.11f, 0.11f, 0.125f, 0.92f};
        softShadow(0.6f, 8, 0, 6);
    } else if (name == L"cover") {
        // Magazine cover: huge hairline time, tiny tracked date above.
        s.timeFont = L"Segoe UI Variable Display Light";
        s.timeSize = 220;
        s.timeWeight = 100;
        s.dateSize = 22;
        s.dateWeight = 600;
        s.dateAbove = true;
        s.dateTracking = 6;
        s.upper = true;
        s.weekday = true;
        softShadow(0.3f, 6, 0, 3);
    } else if (name == L"stacked") {
        s.timeFont = L"Bahnschrift";
        s.timeSize = 150;
        s.timeWeight = 700;
        s.dateSize = 30;
        s.dateWeight = 500;
        s.layout = ZcLayoutKind::stacked;
        softShadow(0.5f, 8, 0, 5);
    } else if (name == L"hollow") {
        s.timeFont = L"Segoe UI Variable Display";
        s.timeSize = 160;
        s.timeWeight = 800;
        s.dateSize = 40;
        s.dateWeight = 700;
        s.hollow = true;
        s.outline = true;
        s.outlineColor = {1, 1, 1, 1};
        s.outlineWidth = 2;
    } else if (name == L"glass") {
        s.timeSize = 120;
        s.timeWeight = 600;
        s.dateSize = 32;
        s.dateWeight = 500;
        s.glass = true;
        s.weekday = true;
        softShadow(0.25f, 6, 0, 3);
    } else if (name == L"segment") {
        // Retro 7-segment: condensed tabular digits over a dim 88:88 ghost.
        s.timeFont = s.dateFont = L"Bahnschrift Condensed";
        s.timeSize = 140;
        s.timeWeight = 400;
        s.dateSize = 32;
        s.dateWeight = 400;
        s.text = {1, 0.23f, 0.19f, 1};
        s.ghost = true;
        s.ghostColor = {1, 0.23f, 0.19f, 0.12f};
        s.glow = true;
        s.glowColor = {1, 0.1f, 0.05f, 0.9f};
        s.glowRadius = 10;
    } else if (name == L"rainbow" || name == L"sunset") {
        s.timeSize = 150;
        s.timeWeight = 800;
        s.dateSize = 40;
        s.dateWeight = 700;
        if (name == L"rainbow") {
            s.grad = {{1, 0.23f, 0.19f, 1}, {1, 0.8f, 0, 1}, {0.2f, 0.85f, 0.35f, 1},
                      {0, 0.6f, 1, 1},      {0.69f, 0.32f, 0.87f, 1}};
        } else {
            s.grad = {{1, 0.37f, 0.43f, 1}, {1, 0.6f, 0.2f, 1}, {1, 0.85f, 0.4f, 1}};
        }
        softShadow(0.4f, 8, 0, 5);
    } else if (name == L"chinese") {
        s.timeFont = s.dateFont = ZcPickFont({L"KaiTi", L"STKaiti", L"楷体",
                                              L"Microsoft YaHei"});
        s.timeSize = 96;
        s.timeWeight = 400;
        s.dateSize = 40;
        s.dateWeight = 400;
        s.layout = ZcLayoutKind::chinese;
        softShadow(0.55f, 6, 3, 4);
    } else if (name == L"sentence") {
        s.timeFont = s.dateFont = L"Microsoft YaHei UI Light";
        s.timeSize = 64;
        s.timeWeight = 300;
        s.dateSize = 26;
        s.dateWeight = 400;
        s.layout = ZcLayoutKind::sentence;
        s.weekday = true;
        softShadow(0.45f, 6, 0, 3);
    } else if (name == L"analog") {
        s.timeSize = 120;  // dial diameter ~ 2.2x
        s.timeWeight = 400;
        s.dateSize = 28;
        s.dateWeight = 500;
        s.layout = ZcLayoutKind::analog;
        softShadow(0.45f, 8, 0, 4);
    } else if (name == L"ring") {
        s.timeSize = 90;
        s.timeWeight = 600;
        s.dateSize = 26;
        s.dateWeight = 500;
        s.layout = ZcLayoutKind::ring;
        s.accent = {0.3f, 0.85f, 1, 1};
        softShadow(0.35f, 6, 0, 3);
    } else if (name == L"accent") {
        s.timeSize = 120;
        s.timeWeight = 300;
        s.dateSize = 30;
        s.dateWeight = 600;
        s.accentBar = true;
        s.weekday = true;
        softShadow(0.35f, 5, 0, 3);
    } else if (name == L"glitch") {
        s.timeFont = s.dateFont = L"Bahnschrift";
        s.timeSize = 140;
        s.timeWeight = 800;
        s.dateSize = 36;
        s.dateWeight = 700;
        s.dup = true;
        s.dupOffset = 5;
    }
}

void ZcLoadSettings() {
    ZcSettings z;
    z.showClock = Wh_GetIntSetting(L"showClock") != 0;
    z.use24 = ZcPinnedInt(L"advanced.use24Hour", L"use24Hour", L"pin24", L"seen24") != 0;
    z.seconds = ZcPinnedInt(L"advanced.showSeconds", L"showSeconds", L"pinSec", L"seenSec") != 0;

    std::wstring d = ZcPinnedStr(L"advanced.dateFormat", L"dateFormat", L"pinDate", L"seenDate");
    z.date = d == L"dmy"      ? ZcDate::dmy
             : d == L"iso"    ? ZcDate::iso
             : d == L"mdy"    ? ZcDate::mdy
             : d == L"cnFull" ? ZcDate::cnFull
             : d == L"none"   ? ZcDate::none
                              : ZcDate::cn;
    static const wchar_t* kAnchors[] = {L"topLeft", L"top",        L"topRight",
                                        L"left",    L"center",     L"right",
                                        L"bottomLeft", L"bottom", L"bottomRight"};
    std::wstring an = ZcPinnedStr(L"advanced.anchor", L"anchor", L"pinAnchor", L"seenAnchor");
    z.anchor = 4;
    for (int i = 0; i < 9; i++) {
        if (an == kAnchors[i]) z.anchor = i;
    }
    z.offsetX = ZcPinnedInt(L"advanced.offset.x", L"offset.x", L"pinOffX", L"seenOffX");
    z.offsetY = ZcPinnedInt(L"advanced.offset.y", L"offset.y", L"pinOffY", L"seenOffY");
    z.size = ZcLivePct(L"size", L"sizePin", L"sizeSeen");
    z.calendarSize = ZcLivePct(L"calendarSize", L"calSizePin", L"calSizeSeen");
    z.resourcesSize = ZcLivePct(L"resourcesSize", L"resSizePin", L"resSizeSeen");
    z.monitor = Wh_GetIntSetting(L"monitor");
    z.resources = Wh_GetIntSetting(L"showResources") != 0;
    z.resCpu = Wh_GetIntSetting(L"resCpu") != 0;
    z.resMem = Wh_GetIntSetting(L"resMem") != 0;
    z.resGpu = Wh_GetIntSetting(L"resGpu") != 0;
    z.resNet = Wh_GetIntSetting(L"resNet") != 0;
    // Missing keys come back as 0, which would hide every row after an upgrade.
    // 0 = not seen, 1 = still using that fallback, 2 = the saved settings are real.
    int raw = (z.resCpu ? 1 : 0) | (z.resMem ? 2 : 0) | (z.resGpu ? 4 : 0) |
              (z.resNet ? 8 : 0);
    int seen = Wh_GetIntValue(L"resPickSeen", 0);
    if (seen != 2 && raw == 0) {
        z.resCpu = z.resMem = z.resGpu = z.resNet = true;
        if (seen != 1) Wh_SetIntValue(L"resPickSeen", 1);
    } else if (seen != 2) {
        Wh_SetIntValue(L"resPickSeen", 2);
    }
    std::wstring cal = ZcStr(L"calendar");
    z.calendar = cal == L"on" || cal == L"right" || cal == L"below";
    if (cal == L"below") Wh_SetIntValue(L"calPlace", 2);
    else if (cal == L"right") Wh_SetIntValue(L"calPlace", 1);
    g_zcCalPlace = Wh_GetIntValue(L"calPlace", 1) == 2 ? 2 : 1;
    z.lunar = ZcPinnedInt(L"advanced.lunar", L"lunar", L"pinLunar", L"seenLunar") != 0;
    z.drag = Wh_GetIntSetting(L"dragMode") != 0;
    z.reset = Wh_GetIntSetting(L"resetPosition") != 0;
    std::wstring ck = ZcStr(L"calendarStyle");
    z.calKind = ck == L"compact" ? ZcCalKind::compact
             : ck == L"week"    ? ZcCalKind::week
             : ck == L"today"   ? ZcCalKind::today
             : ck == L"strip"   ? ZcCalKind::strip
                                  : ZcCalKind::month;
    if (z.reset && (Wh_GetIntValue(L"posSet", 0) || Wh_GetIntValue(L"calSet", 0) ||
                    Wh_GetIntValue(L"resSet", 0))) {
        Wh_SetIntValue(L"posSet", 0);
        Wh_SetIntValue(L"calSet", 0);
        Wh_SetIntValue(L"resSet", 0);
        Wh_Log(L"dragged positions cleared");
    }

    ZcStyle& s = z.style;
    std::wstring style = ZcStr(L"style");
    if (style != L"custom") {
        ZcApplyPreset(style.empty() ? L"tilted" : style, s);
    } else {
        auto A = [](const wchar_t* k) { return std::wstring(L"advanced.") + k; };
        auto I = [&](const wchar_t* k) { return Wh_GetIntSetting(A(k).c_str()); };
        auto S = [&](const wchar_t* k) { return ZcStr(A(k).c_str()); };
        auto C = [&](const wchar_t* k, D2D1_COLOR_F def) {
            D2D1_COLOR_F c = def;
            if (!ZcChoiceColor(A(k).c_str(), (A(k) + L"Custom").c_str(), &c)) c = def;
            return c;
        };
        s.timeFont = S(L"timeFont");
        s.dateFont = S(L"dateFont");
        if (s.timeFont.empty()) s.timeFont = L"Segoe UI Variable Display";
        if (s.dateFont.empty()) s.dateFont = s.timeFont;
        s.timeSize = I(L"timeFontSize");
        s.timeWeight = I(L"timeFontWeight");
        s.dateSize = I(L"dateFontSize");
        s.dateWeight = I(L"dateFontWeight");
        std::wstring a = S(L"textAlign");
        z.align = a == L"center" ? ZcAlign::center
                  : a == L"right" ? ZcAlign::right
                                  : ZcAlign::left;
        z.lineGap = ZcClamp(I(L"lineGap"), -200, 400);
        s.rotation = (float)ZcClamp(I(L"rotation"), -45, 45);
        s.shadow = I(L"shadow") != 0;
        s.shadowColor = C(L"shadowColor", {0, 0, 0, 0.7f});
        {
            // Named shadow colors are opaque; keep the old soft look.
            std::wstring sc = S(L"shadowColor");
            if (!sc.empty() && sc != L"default" && sc != L"custom" && sc[0] != L'#')
                s.shadowColor.a *= 0.7f;
        }
        s.shadowBlur = (float)ZcClamp(I(L"shadowBlur"), 0, 100);
        s.shadowDx = (float)I(L"shadowOffsetX");
        s.shadowDy = (float)I(L"shadowOffsetY");
        s.glow = I(L"glow") != 0;
        s.glowColor = C(L"glowColor", {0, 0.9f, 1, 1});
        s.glowRadius = (float)ZcClamp(I(L"glowRadius"), 1, 100);
        s.outline = I(L"outline") != 0;
        s.outlineColor = C(L"outlineColor", {0, 0, 0, 1});
        s.outlineWidth = (float)ZcClamp(I(L"outlineWidth"), 1, 20);
        s.hollow = I(L"hollow") != 0;
        D2D1_COLOR_F g0, g1;
        if (ZcChoiceColor(A(L"gradientStart").c_str(), A(L"gradientStartCustom").c_str(), &g0) &&
            ZcChoiceColor(A(L"gradientEnd").c_str(), A(L"gradientEndCustom").c_str(), &g1)) {
            s.grad = {g0, g1};
        }
        s.flip = I(L"flipCards") != 0;
        s.cardColor = C(L"cardColor", {0.125f, 0.125f, 0.14f, 0.88f});
        s.glass = I(L"glassCard") != 0;
        s.accentBar = I(L"accentBar") != 0;
        s.weekday = I(L"showWeekday") != 0;
        std::wstring lay = S(L"layout");
        s.layout = lay == L"stacked"    ? ZcLayoutKind::stacked
                   : lay == L"sentence" ? ZcLayoutKind::sentence
                   : lay == L"chinese"  ? ZcLayoutKind::chinese
                   : lay == L"analog"   ? ZcLayoutKind::analog
                   : lay == L"ring"     ? ZcLayoutKind::ring
                                        : ZcLayoutKind::normal;
        s.dateAbove = lay == L"cover";
    }
    D2D1_COLOR_F c;
    std::wstring color = ZcPinnedStr(L"advanced.color", L"color", L"pinColor", L"seenColor");
    std::wstring colorC =
        ZcPinnedStr(L"advanced.customColor", L"customColor", L"pinColorC", L"seenColorC");
    if (ZcApplyChoice(color, colorC, &c)) {
        s.text = c;
        s.grad.clear();
    }
    std::wstring accent =
        ZcPinnedStr(L"advanced.accentColor", L"accentColor", L"pinAccent", L"seenAccent");
    std::wstring accentC =
        ZcPinnedStr(L"advanced.customAccent", L"customAccent", L"pinAccentC", L"seenAccentC");
    if (ZcApplyChoice(accent, accentC, &c)) s.accent = c;
    s.timeSize = ZcClamp(s.timeSize ? s.timeSize : 140, 8, 600);
    s.dateSize = ZcClamp(s.dateSize ? s.dateSize : 40, 6, 300);
    s.timeWeight = ZcClamp(s.timeWeight ? s.timeWeight : 400, 100, 950);
    s.dateWeight = ZcClamp(s.dateWeight ? s.dateWeight : 400, 100, 950);

    g_zc = z;
    Wh_Log(L"settings: style=%s clock=%d size=%d cal=%d calSize=%d res=%d resSize=%d "
           L"lunar=%d seconds=%d monitor=%d anchor=%d offset=(%d,%d)",
           style.c_str(), (int)z.showClock, z.size, (int)z.calendar, z.calendarSize,
           (int)z.resources, z.resourcesSize, (int)z.lunar, (int)z.seconds, z.monitor,
           z.anchor, z.offsetX, z.offsetY);
}

void ZcFormat(const SYSTEMTIME& t, std::wstring& hh, std::wstring& mm,
              std::wstring& ss, std::wstring& suffix, std::wstring& date) {
    static const wchar_t* kWeekShort[] = {L"周日", L"周一", L"周二", L"周三",
                                          L"周四", L"周五", L"周六"};
    static const wchar_t* kWeekLong[] = {L"星期日", L"星期一", L"星期二",
                                         L"星期三", L"星期四", L"星期五",
                                         L"星期六"};
    WCHAR b[64];
    int hour = t.wHour;
    suffix.clear();
    if (!g_zc.use24) {
        suffix = hour < 12 ? L"AM" : L"PM";
        hour %= 12;
        if (hour == 0) hour = 12;
    }
    swprintf_s(b, g_zc.use24 ? L"%02d" : L"%d", hour);
    hh = b;
    swprintf_s(b, L"%02d", t.wMinute);
    mm = b;
    swprintf_s(b, L"%02d", t.wSecond);
    ss = g_zc.seconds ? b : L"";
    int wd = t.wDayOfWeek % 7;
    switch (g_zc.date) {
        case ZcDate::mdy:
            swprintf_s(b, L"%02d/%02d/%04d", t.wMonth, t.wDay, t.wYear);
            break;
        case ZcDate::dmy:
            swprintf_s(b, L"%02d/%02d/%04d", t.wDay, t.wMonth, t.wYear);
            break;
        case ZcDate::iso:
            swprintf_s(b, L"%04d-%02d-%02d", t.wYear, t.wMonth, t.wDay);
            break;
        case ZcDate::cn:
            swprintf_s(b, L"%d月%d日 %s", t.wMonth, t.wDay, kWeekShort[wd]);
            break;
        case ZcDate::cnFull:
            swprintf_s(b, L"%d年%d月%d日 %s", t.wYear, t.wMonth, t.wDay,
                       kWeekLong[wd]);
            break;
        case ZcDate::none:
            b[0] = 0;
            break;
    }
    date = b;
}

ComPtr<IDWriteTextLayout> ZcLayout(const std::wstring& text,
                                   const std::wstring& font,
                                   int weight,
                                   float size) {
    ComPtr<IDWriteTextFormat> fmt;
    if (FAILED(g_zcDwrite->CreateTextFormat(
            font.c_str(), nullptr, (DWRITE_FONT_WEIGHT)weight,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size,
            L"zh-CN", &fmt))) {
        // Font name invalid: fall back to Segoe UI.
        if (FAILED(g_zcDwrite->CreateTextFormat(
                L"Segoe UI", nullptr, (DWRITE_FONT_WEIGHT)weight,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size,
                L"zh-CN", &fmt))) {
            return nullptr;
        }
    }
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(g_zcDwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                            fmt.Get(), 100000.0f, 100000.0f,
                                            &layout))) {
        return nullptr;
    }
    return layout;
}

D2D1_SIZE_F ZcMeasure(IDWriteTextLayout* layout) {
    DWRITE_TEXT_METRICS m{};
    if (layout) layout->GetMetrics(&m);
    return D2D1::SizeF(m.widthIncludingTrailingWhitespace, m.height);
}

struct ZcPiece {
    ComPtr<IDWriteTextLayout> layout;
    D2D1_POINT_2F pos;
    D2D1_SIZE_F size;
    bool card;
    bool ghost = false;  // decorative "88:88" drawn only in the main pass
    bool dim = false;    // secondary text (weekend, lunar, headers)
};

// Everything needed to draw one clock block at the origin.
struct ZcFrame {
    std::vector<ZcPiece> pieces;
    D2D1_SIZE_F size{};
    float scale = 1;
    SYSTEMTIME t{};
    bool dial = false;  // analog face or progress ring
    D2D1_POINT_2F dialC{};
    float dialR = 0;
    bool hasBar = false;
    D2D1_RECT_F bar{};
    D2D1_RECT_F glassRect{};
    struct Mini { D2D1_POINT_2F c; float r, frac; };
    std::vector<Mini> minis;  // small resource rings
    bool calHi = false;       // calendar "today" highlight
    D2D1_RECT_F calHiRect{};
};

// Lunar years 1999..2099: {days from 1999-01-01 to 正月初一, leap month (0 none), bit i = month i (in order, leap included) has 30 days}.
static const struct { int day; unsigned char leap; unsigned short big; } kZcLunar[] = {
    {46,0,0x0749}, {400,0,0x0693}, {754,4,0x152b}, {1138,0,0x052b}, {1492,0,0x0a5b}, {1847,2,0x155a},
    {2231,0,0x056a}, {2585,7,0x1b55}, {2970,0,0x0ba4}, {3324,0,0x0b49}, {3678,5,0x1a93}, {4062,0,0x0a95},
    {4416,0,0x052d}, {4770,4,0x0aad}, {5154,0,0x0ab5}, {5509,9,0x15aa}, {5893,0,0x05d2}, {6247,0,0x0da5},
    {6602,6,0x1d4a}, {6986,0,0x0d4a}, {7340,0,0x0c95}, {7694,4,0x152e}, {8078,0,0x0556}, {8432,0,0x0ab5},
    {8787,2,0x15b2}, {9171,0,0x06d2}, {9525,6,0x0ea5}, {9909,0,0x0725}, {10263,0,0x064b}, {10617,5,0x0c97},
    {11001,0,0x0cab}, {11356,0,0x055a}, {11710,3,0x0ad6}, {12094,0,0x0b69}, {12449,11,0x1752}, {12833,0,0x0b52},
    {13187,0,0x0b25}, {13541,6,0x1a4b}, {13925,0,0x0a4b}, {14279,0,0x04ab}, {14633,5,0x055b}, {15017,0,0x05ad},
    {15372,0,0x0b6a}, {15727,2,0x1b52}, {16111,0,0x0d92}, {16465,7,0x1d25}, {16849,0,0x0d25}, {17203,0,0x0a55},
    {17557,5,0x14ad}, {17941,0,0x04b6}, {18295,0,0x05b5}, {18650,3,0x0daa}, {19034,0,0x0ec9}, {19389,8,0x1e92},
    {19773,0,0x0e92}, {20127,0,0x0d26}, {20481,6,0x0a56}, {20864,0,0x0a57}, {21219,0,0x0556}, {21573,4,0x06d5},
    {21957,0,0x0755}, {22312,0,0x0749}, {22666,3,0x0e93}, {23050,0,0x0693}, {23404,7,0x152b}, {23788,0,0x052b},
    {24142,0,0x0a5b}, {24497,5,0x155a}, {24881,0,0x056a}, {25235,0,0x0b65}, {25590,4,0x174a}, {25974,0,0x0b4a},
    {26328,8,0x1a95}, {26712,0,0x0a95}, {27066,0,0x052d}, {27420,6,0x0aad}, {27804,0,0x0ab5}, {28159,0,0x05aa},
    {28513,4,0x0ba5}, {28897,0,0x0da5}, {29252,0,0x0d4a}, {29606,3,0x1c95}, {29990,0,0x0c96}, {30344,7,0x194e},
    {30728,0,0x0556}, {31082,0,0x0ab5}, {31437,5,0x15b2}, {31821,0,0x06d2}, {32175,0,0x0ea5}, {32530,4,0x0e4a},
    {32913,0,0x068b}, {33267,8,0x0c97}, {33651,0,0x04ab}, {34005,0,0x055b}, {34360,6,0x0ad6}, {34744,0,0x0b6a},
    {35099,0,0x0752}, {35453,4,0x1725}, {35837,0,0x0b45}, {36191,0,0x0a8b}, {36545,2,0x149b},
};

int ZcDaysFromCivil(int y, int m, int d) {
    y -= m <= 2;
    int era = (y >= 0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

// Solar date -> lunar month/day. Returns false outside 1999-02..2099.
bool ZcLunarDate(int y, int m, int d, int* lm, int* ld, bool* leap) {
    int days = ZcDaysFromCivil(y, m, d) - ZcDaysFromCivil(1999, 1, 1);
    int n = (int)(sizeof(kZcLunar) / sizeof(kZcLunar[0]));
    int i = n - 1;
    while (i >= 0 && kZcLunar[i].day > days) i--;
    if (i < 0) return false;
    int off = days - kZcLunar[i].day;
    int lp = kZcLunar[i].leap, count = lp ? 13 : 12;
    for (int k = 0; k < count; k++) {
        int len = 29 + ((kZcLunar[i].big >> k) & 1);
        if (off < len) {
            *leap = lp && k == lp;
            *lm = lp && k >= lp ? k : k + 1;
            *ld = off + 1;
            return true;
        }
        off -= len;
    }
    return false;
}

std::wstring ZcCnNum(int n, bool day) {
    static const wchar_t* d[] = {L"〇", L"一", L"二", L"三", L"四",
                                 L"五", L"六", L"七", L"八", L"九"};
    if (n < 10) return d[n];
    if (n == 10) return L"十";
    if (n < 20) return std::wstring(L"十") + d[n % 10];
    if (day && n > 20 && n < 30) return std::wstring(L"廿") + d[n % 10];
    std::wstring r = std::wstring(d[n / 10]) + L"十";
    if (n % 10) r += d[n % 10];
    return r;
}

const wchar_t* ZcPeriod(int h) {
    return h < 5 ? L"凌晨" : h < 12 ? L"上午" : h < 13 ? L"中午"
           : h < 18 ? L"下午" : L"晚上";
}

void ZcBuildFrame(float scale, ZcFrame& f) {
    const ZcStyle& s = g_zc.style;
    f.scale = scale;
    GetLocalTime(&f.t);
    const SYSTEMTIME& t = f.t;
    std::wstring hh, mm, ss, suffix, date;
    ZcFormat(t, hh, mm, ss, suffix, date);
    static const wchar_t* kWeek[] = {L"星期日", L"星期一", L"星期二", L"星期三",
                                     L"星期四", L"星期五", L"星期六"};
    static const wchar_t* kWeekEn[] = {L"Sunday",   L"Monday", L"Tuesday",
                                       L"Wednesday", L"Thursday", L"Friday",
                                       L"Saturday"};
    int wd = t.wDayOfWeek % 7;
    if (s.layout == ZcLayoutKind::chinese && g_zc.date != ZcDate::none) {
        date = ZcCnNum(t.wMonth, false) + L"月" + ZcCnNum(t.wDay, true) + L" " +
               kWeek[wd];
    } else if (s.weekday && !date.empty() && date.find(L"周") == std::wstring::npos &&
               date.find(L"星期") == std::wstring::npos) {
        date = (s.upper ? std::wstring(kWeekEn[wd]) : std::wstring(kWeek[wd])) +
               L"  " + date;
    }
    if (s.upper) {
        for (auto& c : date) c = (wchar_t)towupper(c);
    }

    float timeSize = s.timeSize * scale;
    std::vector<ZcPiece> row;
    float x = 0, rowH = 0;
    auto make = [&](const std::wstring& txt, float size) {
        ZcPiece p{ZcLayout(txt, s.timeFont, s.timeWeight, size), {}, {}, false};
        if (p.layout) p.size = ZcMeasure(p.layout.Get());
        return p;
    };
    auto add = [&](const std::wstring& txt, float size, bool card) {
        if (txt.empty()) return;
        ZcPiece p = make(txt, size);
        if (!p.layout) return;
        p.card = card;
        float pad = card ? size * 0.12f : 0;
        p.pos = D2D1::Point2F(x + pad, 0);
        x += p.size.width + pad * 2;
        rowH = rowH > p.size.height ? rowH : p.size.height;
        row.push_back(p);
    };
    float cardGap = timeSize * 0.08f;
    std::wstring digits = hh + L":" + mm + (ss.empty() ? L"" : L":" + ss);
    switch (s.layout) {
        case ZcLayoutKind::analog: {
            float d = timeSize * 2.2f;
            f.dial = true;
            f.dialR = d / 2;
            f.dialC = D2D1::Point2F(d / 2, d / 2);
            x = d;
            rowH = d;
            break;
        }
        case ZcLayoutKind::stacked: {
            ZcPiece a = make(hh, timeSize), b = make(mm, timeSize);
            if (a.layout && b.layout) {
                float w = a.size.width > b.size.width ? a.size.width : b.size.width;
                float step = timeSize * 0.92f;
                a.pos = D2D1::Point2F((w - a.size.width) / 2, 0);
                b.pos = D2D1::Point2F((w - b.size.width) / 2, step);
                row.push_back(a);
                row.push_back(b);
                x = w;
                rowH = step + b.size.height;
                if (!ss.empty()) {
                    ZcPiece c = make(ss, timeSize * 0.3f);
                    if (c.layout) {
                        c.pos = D2D1::Point2F(w + timeSize * 0.06f,
                                              rowH - c.size.height - timeSize * 0.15f);
                        x = c.pos.x + c.size.width;
                        row.push_back(c);
                    }
                }
            }
            break;
        }
        case ZcLayoutKind::chinese: {
            std::wstring txt = ZcCnNum(t.wHour, false) + L"时" +
                               (t.wMinute ? ZcCnNum(t.wMinute, false) + L"分" : L"整");
            if (t.wMinute && t.wMinute < 10) txt.insert(txt.size() - 2, L"零");
            if (g_zc.seconds) txt += ZcCnNum(t.wSecond, false) + L"秒";
            add(txt, timeSize, false);
            break;
        }
        case ZcLayoutKind::sentence: {
            int h12 = t.wHour % 12;
            if (h12 == 0) h12 = 12;
            WCHAR b[96];
            if (t.wMinute == 0)
                swprintf_s(b, L"现在是 %s %d 点整", ZcPeriod(t.wHour), h12);
            else
                swprintf_s(b, L"现在是 %s %d 点 %d 分", ZcPeriod(t.wHour), h12,
                           t.wMinute);
            add(b, timeSize, false);
            break;
        }
        default:
            if (s.flip) {
                add(hh, timeSize, true);
                x += cardGap;
                add(mm, timeSize, true);
                if (!ss.empty()) {
                    x += cardGap;
                    add(ss, timeSize, true);
                }
            } else {
                if (s.ghost) {
                    std::wstring g = digits;
                    for (auto& c : g) if (c >= L'0' && c <= L'9') c = L'8';
                    ZcPiece gp = make(g, timeSize);
                    if (gp.layout) {
                        gp.ghost = true;
                        gp.pos = D2D1::Point2F(0, 0);
                        row.push_back(gp);
                    }
                }
                add(digits, timeSize, false);
            }
            if (!suffix.empty()) {
                x += timeSize * 0.08f;
                add(suffix, timeSize * 0.3f, false);
                row.back().pos.y = rowH - row.back().size.height - timeSize * 0.12f;
            }
            break;
    }
    if (s.layout == ZcLayoutKind::ring && !row.empty()) {
        // Center the text inside a circle; the ring shows minute progress.
        float r = std::sqrt(x * x + rowH * rowH) / 2 + timeSize * 0.18f;
        float ox = r - x / 2, oy = r - rowH / 2;
        for (auto& p : row) {
            p.pos.x += ox;
            p.pos.y += oy;
        }
        f.dial = true;
        f.dialR = r;
        f.dialC = D2D1::Point2F(r, r);
        x = rowH = 2 * r;
    }
    float rowW = x;

    ZcPiece datePiece{};
    float blockW = rowW, blockH = rowH, dateGap = g_zc.lineGap * scale;
    if (!date.empty()) {
        datePiece.layout = ZcLayout(date, s.dateFont, s.dateWeight, s.dateSize * scale);
        if (datePiece.layout && s.dateTracking > 0) {
            ComPtr<IDWriteTextLayout1> l1;
            if (SUCCEEDED(datePiece.layout.As(&l1))) {
                float tr = s.dateTracking * scale;
                l1->SetCharacterSpacing(0, tr, 0, {0, (UINT32)date.size()});
            }
        }
        if (datePiece.layout) {
            datePiece.size = ZcMeasure(datePiece.layout.Get());
            if (s.dup || f.dial) dateGap += s.dateSize * scale * 0.3f;
            blockW = blockW > datePiece.size.width ? blockW : datePiece.size.width;
            blockH = rowH + dateGap + datePiece.size.height;
        }
    }
    auto alignX = [&](float w) {
        ZcAlign a = f.dial ? ZcAlign::center : g_zc.align;
        switch (a) {
            case ZcAlign::center: return (blockW - w) / 2;
            case ZcAlign::right: return blockW - w;
            default: return 0.0f;
        }
    };
    float rowY = (datePiece.layout && s.dateAbove) ? datePiece.size.height + dateGap : 0;
    float rowOff = alignX(rowW);
    for (auto& p : row) {
        p.pos.x += rowOff;
        p.pos.y += rowY;
        f.pieces.push_back(p);
    }
    f.dialC.x += rowOff;
    f.dialC.y += rowY;
    if (datePiece.layout) {
        datePiece.pos = D2D1::Point2F(alignX(datePiece.size.width),
                                      s.dateAbove ? 0 : rowH + dateGap);
        f.pieces.push_back(datePiece);
    }
    auto shiftAll = [&](float dx, float dy) {
        for (auto& p : f.pieces) {
            p.pos.x += dx;
            p.pos.y += dy;
        }
        for (auto& m : f.minis) {
            m.c.x += dx;
            m.c.y += dy;
        }
        f.dialC.x += dx;
        f.dialC.y += dy;
        f.calHiRect.left += dx; f.calHiRect.right += dx;
        f.calHiRect.top += dy; f.calHiRect.bottom += dy;
        f.bar.left += dx; f.bar.right += dx; f.bar.top += dy; f.bar.bottom += dy;
    };
    if (s.accentBar) {
        float bw = (std::max)(3.0f, timeSize * 0.035f), gap = timeSize * 0.16f;
        shiftAll(bw + gap, 0);
        f.hasBar = true;
        f.bar = D2D1::RectF(0, blockH * 0.06f, bw, blockH * 0.96f);
        blockW += bw + gap;
    }
    if (s.glass) {
        float pad = timeSize * 0.22f;
        shiftAll(pad, pad);
        blockW += pad * 2;
        blockH += pad * 2;
        f.glassRect = D2D1::RectF(0, 0, blockW, blockH);
    }
    f.size = D2D1::SizeF(blockW, blockH);
}

std::wstring ZcLunarText(int y, int m, int d) {
    int lm, ld;
    bool leap = false;
    if (!ZcLunarDate(y, m, d, &lm, &ld, &leap) || lm < 1 || lm > 12) return L"";
    static const wchar_t* kLm[] = {L"\u6b63", L"\u4e8c", L"\u4e09", L"\u56db", L"\u4e94", L"\u516d",
                                   L"\u4e03", L"\u516b", L"\u4e5d", L"\u5341", L"\u51ac", L"\u814a"};
    if (ld == 1) return std::wstring(leap ? L"\u95f0" : L"") + kLm[lm - 1] + L"\u6708";
    if (ld <= 10) return std::wstring(L"\u521d") + ZcCnNum(ld, false);
    if (ld == 20) return L"\u4e8c\u5341";
    if (ld == 30) return L"\u4e09\u5341";
    return ZcCnNum(ld, true);
}

int ZcMonthDays(int y, int m) {
    static const int md[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int n = md[m - 1];
    if (m == 2 && ((y % 4 == 0 && y % 100) || y % 400 == 0)) n = 29;
    return n;
}

SYSTEMTIME ZcAddDays(SYSTEMTIME t, int delta) {
    int y = t.wYear, m = t.wMonth, d = (int)t.wDay + delta;
    while (d > ZcMonthDays(y, m)) {
        d -= ZcMonthDays(y, m);
        if (++m == 13) {
            m = 1;
            ++y;
        }
    }
    while (d < 1) {
        if (--m == 0) {
            m = 12;
            --y;
        }
        d += ZcMonthDays(y, m);
    }
    SYSTEMTIME o = t;
    o.wYear = (WORD)y;
    o.wMonth = (WORD)m;
    o.wDay = (WORD)d;
    int wd = (ZcDaysFromCivil(y, m, d) + 4) % 7;  // 1970-01-01 = Thursday
    if (wd < 0) wd += 7;
    o.wDayOfWeek = (WORD)wd;
    return o;
}

std::wstring ZcRateText(double bps) {
    WCHAR b[32];
    if (bps < 0) bps = 0;
    if (bps >= 1024.0 * 1024.0) swprintf_s(b, L"%.1f MB/s", bps / (1024.0 * 1024.0));
    else if (bps >= 1024.0) swprintf_s(b, L"%.0f KB/s", bps / 1024.0);
    else swprintf_s(b, L"%.0f B/s", bps);
    return b;
}

std::wstring ZcRateLine() {
    return std::wstring(L"\u2193 ") + ZcRateText(g_zcDownBps) + L"   \u2191 " + ZcRateText(g_zcUpBps);
}

void ZcBuildCalendar(float scale, ZcFrame& f) {
    const ZcStyle& s = g_zc.style;
    f = {};
    f.scale = scale;
    GetLocalTime(&f.t);
    const SYSTEMTIME& t = f.t;
    ZcCalKind kind = g_zc.calKind;
    bool lunarOn = g_zc.lunar && kind != ZcCalKind::compact;
    float cs = (std::max)(13.0f, s.dateSize * scale * (kind == ZcCalKind::compact ? 0.58f : 0.72f));
    static const wchar_t* kWeek[] = {L"\u5468\u65e5", L"\u5468\u4e00", L"\u5468\u4e8c", L"\u5468\u4e09",
                                     L"\u5468\u56db", L"\u5468\u4e94", L"\u5468\u516d"};

    auto addLine = [&](const std::wstring& txt, float size, int weight, bool dim) {
        ZcPiece p{};
        p.layout = ZcLayout(txt, s.dateFont, weight, size);
        if (!p.layout) return;
        p.size = ZcMeasure(p.layout.Get());
        p.dim = dim;
        float y = f.pieces.empty() ? 0 : f.pieces.back().pos.y + f.pieces.back().size.height + size * 0.12f;
        p.pos = D2D1::Point2F(0, y);
        f.pieces.push_back(p);
    };
    auto finishLines = [&]() {
        float w = 1, h = 1;
        for (auto& p : f.pieces) w = (std::max)(w, p.size.width);
        for (auto& p : f.pieces) p.pos.x = (w - p.size.width) / 2;
        if (!f.pieces.empty()) {
            auto& last = f.pieces.back();
            h = last.pos.y + last.size.height;
        }
        f.size = D2D1::SizeF(w, h);
    };

    if (kind == ZcCalKind::today || kind == ZcCalKind::strip) {
        WCHAR b[64];
        if (kind == ZcCalKind::today) {
            swprintf_s(b, L"%d", t.wDay);
            addLine(b, cs * 3.1f, 700, false);
            swprintf_s(b, L"%d\u6708  %s", t.wMonth, kWeek[t.wDayOfWeek % 7]);
            addLine(b, cs * 0.95f, (std::max)(s.dateWeight, 600), false);
        } else {
            swprintf_s(b, L"%d\u6708%d\u65e5 %s", t.wMonth, t.wDay, kWeek[t.wDayOfWeek % 7]);
            std::wstring line = b;
            if (lunarOn) {
                std::wstring lt = ZcLunarText(t.wYear, t.wMonth, t.wDay);
                if (!lt.empty()) line += L"  " + lt;
            }
            addLine(line, cs * 1.05f, s.dateWeight, false);
            lunarOn = false;
        }
        if (lunarOn) {
            std::wstring lt = ZcLunarText(t.wYear, t.wMonth, t.wDay);
            if (!lt.empty()) addLine(lt, cs * 0.72f, s.dateWeight, true);
        }
        finishLines();
        return;
    }

    bool compact = kind == ZcCalKind::compact;
    float cellW = cs * (lunarOn ? 2.5f : compact ? 1.42f : 1.9f);
    float cellH = cs * (lunarOn ? 2.1f : compact ? 1.18f : 1.5f);
    auto put = [&](const std::wstring& txt, float size, int weight, float cx, float y, bool dim) {
        ZcPiece p{};
        p.layout = ZcLayout(txt, s.dateFont, weight, size);
        if (!p.layout) return;
        p.size = ZcMeasure(p.layout.Get());
        p.pos = D2D1::Point2F(cx - p.size.width / 2, y);
        p.dim = dim;
        f.pieces.push_back(p);
    };
    WCHAR b[64];
    static const wchar_t* kHead[] = {L"\u4e00", L"\u4e8c", L"\u4e09", L"\u56db", L"\u4e94", L"\u516d", L"\u65e5"};
    if (kind == ZcCalKind::week) {
        SYSTEMTIME mon = ZcAddDays(t, t.wDayOfWeek == 0 ? -6 : 1 - (int)t.wDayOfWeek);
        SYSTEMTIME sun = ZcAddDays(mon, 6);
        if (mon.wYear == sun.wYear && mon.wMonth == sun.wMonth)
            swprintf_s(b, L"%d\u5e74%d\u6708", mon.wYear, mon.wMonth);
        else if (mon.wYear == sun.wYear)
            swprintf_s(b, L"%d\u5e74%d\u6708 / %d\u6708", mon.wYear, mon.wMonth, sun.wMonth);
        else
            swprintf_s(b, L"%d\u5e74%d\u6708 / %d\u5e74%d\u6708", mon.wYear, mon.wMonth, sun.wYear, sun.wMonth);
        put(b, cs * 1.05f, (std::max)(s.dateWeight, 600), cellW * 3.5f, 0, false);
        float gy = cs * 1.7f;
        for (int i = 0; i < 7; i++)
            put(kHead[i], cs * 0.8f, s.dateWeight, cellW * (i + 0.5f), gy, true);
        gy += compact ? cs * 1.05f : cs * 1.35f;
        for (int i = 0; i < 7; i++) {
            SYSTEMTIME day = ZcAddDays(mon, i);
            bool today = day.wYear == t.wYear && day.wMonth == t.wMonth && day.wDay == t.wDay;
            float cx = cellW * (i + 0.5f);
            if (today) {
                f.calHi = true;
                f.calHiRect = D2D1::RectF(cx - cellW * 0.46f, gy - cs * 0.15f,
                                          cx + cellW * 0.46f, gy + cellH - cs * 0.25f);
            }
            swprintf_s(b, L"%d", day.wDay);
            put(b, cs, today ? 700 : s.dateWeight, cx, gy, !today && (i >= 5 || day.wMonth != t.wMonth));
            if (lunarOn) {
                std::wstring lt = ZcLunarText(day.wYear, day.wMonth, day.wDay);
                if (!lt.empty())
                    put(lt, cs * 0.55f, s.dateWeight, cx, gy + cs * 1.25f, !today);
            }
        }
        f.size = D2D1::SizeF(cellW * 7, gy + cellH);
    } else {
        swprintf_s(b, L"%d\u5e74%d\u6708", t.wYear, t.wMonth);
        put(b, cs * (compact ? 1.0f : 1.15f), (std::max)(s.dateWeight, 600), cellW * 3.5f, 0, false);
        float gy = cs * (compact ? 1.55f : 2.0f);
        for (int i = 0; i < 7; i++)
            put(kHead[i], cs * 0.8f, s.dateWeight, cellW * (i + 0.5f), gy, true);
        gy += compact ? cs * 1.05f : cs * 1.4f;
        int days = ZcMonthDays(t.wYear, t.wMonth);
        int first = (ZcDaysFromCivil(t.wYear, t.wMonth, 1) + 3) % 7;  // 1970-01-01 = Thu
        if (first < 0) first += 7;
        int rows = (first + days + 6) / 7;
        for (int dday = 1; dday <= days; dday++) {
            int idx = first + dday - 1, col = idx % 7, row = idx / 7;
            float cx = cellW * (col + 0.5f), y = gy + row * cellH;
            if (dday == t.wDay) {
                f.calHi = true;
                f.calHiRect = D2D1::RectF(cx - cellW * 0.46f, y - cs * 0.15f,
                                          cx + cellW * 0.46f, y + cellH - cs * 0.25f);
            }
            swprintf_s(b, L"%d", dday);
            put(b, cs, dday == t.wDay ? 700 : s.dateWeight, cx, y, col >= 5 && dday != t.wDay);
            if (lunarOn) {
                std::wstring lt = ZcLunarText(t.wYear, t.wMonth, dday);
                if (!lt.empty())
                    put(lt, cs * 0.55f, s.dateWeight, cx, y + cs * 1.25f, dday != t.wDay);
            }
        }
        f.size = D2D1::SizeF(cellW * 7, gy + rows * cellH);
    }
    float minL = 0, maxR = f.size.width, maxB = f.size.height;
    for (auto& p : f.pieces) {
        minL = (std::min)(minL, p.pos.x);
        maxR = (std::max)(maxR, p.pos.x + p.size.width);
        maxB = (std::max)(maxB, p.pos.y + p.size.height);
    }
    if (minL < 0) {
        for (auto& p : f.pieces) p.pos.x -= minL;
        f.calHiRect.left -= minL;
        f.calHiRect.right -= minL;
        maxR -= minL;
    }
    f.size = D2D1::SizeF((std::max)(maxR, 1.0f), (std::max)(maxB, 1.0f));
}

void ZcBuildResources(float scale, ZcFrame& f) {
    const ZcStyle& s = g_zc.style;
    f = {};
    f.scale = scale;
    GetLocalTime(&f.t);
    float ds = (std::max)(12.0f, s.dateSize * scale * 0.72f);
    struct Item { float val; bool known; const wchar_t* name; };
    Item items[3];
    int n = 0;
    if (g_zc.resCpu) items[n++] = {g_zcCpu, true, L"CPU"};
    if (g_zc.resMem) items[n++] = {g_zcMem, true, L"\u5185\u5b58"};
    if (g_zc.resGpu) items[n++] = {g_zcGpu, g_zcGpuKnown, L"GPU"};
    bool net = g_zc.resNet;
    bool rings = n > 0 && (s.layout == ZcLayoutKind::ring || s.layout == ZcLayoutKind::analog);
    std::wstring rate = net ? ZcRateLine() : L"";
    auto addText = [&](const std::wstring& txt, float size, float y) -> float {
        if (txt.empty()) return y;
        ZcPiece p{};
        p.layout = ZcLayout(txt, s.dateFont, s.dateWeight, size);
        if (!p.layout) return y;
        p.size = ZcMeasure(p.layout.Get());
        p.pos = D2D1::Point2F(0, y);
        f.pieces.push_back(p);
        f.size.width = (std::max)(f.size.width, p.size.width);
        f.size.height = p.pos.y + p.size.height;
        return f.size.height + ds * 0.16f;
    };
    if (!rings) {
        std::wstring line;
        WCHAR b[48];
        for (int i = 0; i < n; i++) {
            if (items[i].known) swprintf_s(b, L"%s %.0f%%", items[i].name, (double)items[i].val);
            else swprintf_s(b, L"%s --", items[i].name);
            if (!line.empty()) line += L"   ";
            line += b;
        }
        float y = addText(line, ds, 0);
        addText(rate, ds * 0.86f, y);
        if (f.size.width < 1) f.size.width = 1;
        if (f.size.height < 1) f.size.height = 1;
        return;
    }
    float rr = ds * 1.15f, gap = ds * 0.55f, x = 0, blockH = 0;
    for (int i = 0; i < n; i++) {
        D2D1_POINT_2F c = D2D1::Point2F(x + rr, rr);
        float frac = items[i].known ? items[i].val / 100.0f : 0;
        if (frac < 0) frac = 0;
        if (frac > 1) frac = 1;
        f.minis.push_back({c, rr, frac});
        WCHAR vb[16];
        if (!items[i].known) swprintf_s(vb, L"--");
        else swprintf_s(vb, L"%.0f%%", (double)items[i].val);
        ZcPiece p{};
        p.layout = ZcLayout(vb, s.dateFont, s.dateWeight, ds * 0.55f);
        ZcPiece q{};
        q.layout = ZcLayout(items[i].name, s.dateFont, s.dateWeight, ds * 0.42f);
        float colW = rr * 2, bottom = rr * 2;
        if (p.layout) {
            p.size = ZcMeasure(p.layout.Get());
            p.pos = D2D1::Point2F(c.x - p.size.width / 2, c.y - p.size.height / 2);
            f.pieces.push_back(p);
            colW = (std::max)(colW, p.size.width);
        }
        if (q.layout) {
            q.size = ZcMeasure(q.layout.Get());
            q.pos = D2D1::Point2F(c.x - q.size.width / 2, c.y + rr + ds * 0.12f);
            f.pieces.push_back(q);
            colW = (std::max)(colW, q.size.width);
            bottom = q.pos.y + q.size.height;
        }
        blockH = (std::max)(blockH, bottom);
        x += colW + gap;
    }
    float blockW = (std::max)(x - gap, 1.0f);
    if (net) {
        ZcPiece sp{};
        sp.layout = ZcLayout(rate, s.dateFont, s.dateWeight, ds * 0.62f);
        if (sp.layout) {
            sp.size = ZcMeasure(sp.layout.Get());
            if (sp.size.width > blockW) {
                float shift = (sp.size.width - blockW) / 2;
                for (auto& m : f.minis) m.c.x += shift;
                for (auto& piece : f.pieces) piece.pos.x += shift;
                sp.pos = D2D1::Point2F(0, blockH + ds * 0.28f);
                blockW = sp.size.width;
            } else {
                sp.pos = D2D1::Point2F((blockW - sp.size.width) / 2, blockH + ds * 0.28f);
            }
            blockH = sp.pos.y + sp.size.height;
            f.pieces.push_back(sp);
        }
    }
    f.size = D2D1::SizeF((std::max)(blockW, 1.0f), (std::max)(blockH, 1.0f));
}

// Arc from 12 o'clock clockwise by `frac` of a full turn.
ComPtr<ID2D1PathGeometry> ZcArc(D2D1_POINT_2F c, float r, float frac) {
    ComPtr<ID2D1PathGeometry> g;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_zcD2dFactory->CreatePathGeometry(&g)) || FAILED(g->Open(&sink)))
        return nullptr;
    float a = frac * 6.2831853f;
    sink->BeginFigure(D2D1::Point2F(c.x, c.y - r), D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(c.x + r * std::sin(a), c.y - r * std::cos(a)),
        D2D1::SizeF(r, r), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
        frac > 0.5f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    return g;
}

// Records the block at the origin. main = full colors (gradient, ghost, cards,
// glass, accents); otherwise every shape is drawn in `color` as a mask.
ComPtr<ID2D1CommandList> ZcRecord(const ZcFrame& f, D2D1_COLOR_F color, bool main) {
    const ZcStyle& s = g_zc.style;
    float scale = f.scale;
    ComPtr<ID2D1CommandList> list;
    if (FAILED(g_zcDc->CreateCommandList(&list))) return nullptr;
    ComPtr<ID2D1Image> oldTarget;
    g_zcDc->GetTarget(&oldTarget);
    D2D1_MATRIX_3X2_F oldTransform;
    g_zcDc->GetTransform(&oldTransform);
    // Called inside the frame's BeginDraw/EndDraw: switching the target to a
    // command list records the drawing instead of rasterizing it.
    g_zcDc->SetTarget(list.Get());
    g_zcDc->SetTransform(D2D1::Matrix3x2F::Identity());
    ComPtr<ID2D1SolidColorBrush> brush, cardBrush, lineBrush, ghostBrush, accentBrush,
        trackBrush;
    g_zcDc->CreateSolidColorBrush(color, &brush);
    ComPtr<ID2D1SolidColorBrush> dimBrush;
    g_zcDc->CreateSolidColorBrush(
        D2D1::ColorF(s.text.r, s.text.g, s.text.b, s.text.a * 0.55f), &dimBrush);
    ComPtr<ID2D1Brush> fill = brush;
    if (main) {
        g_zcDc->CreateSolidColorBrush(s.ghostColor, &ghostBrush);
        g_zcDc->CreateSolidColorBrush(s.accent, &accentBrush);
        g_zcDc->CreateSolidColorBrush(s.track, &trackBrush);
        if (s.flip) {
            g_zcDc->CreateSolidColorBrush(s.cardColor, &cardBrush);
            g_zcDc->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.55f), &lineBrush);
        }
        if (s.grad.size() >= 2) {
            std::vector<D2D1_GRADIENT_STOP> stops;
            for (size_t i = 0; i < s.grad.size(); i++)
                stops.push_back({(float)i / (s.grad.size() - 1), s.grad[i]});
            ComPtr<ID2D1GradientStopCollection> sc;
            ComPtr<ID2D1LinearGradientBrush> lg;
            if (SUCCEEDED(g_zcDc->CreateGradientStopCollection(
                    stops.data(), (UINT32)stops.size(), &sc)) &&
                SUCCEEDED(g_zcDc->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, 0),
                        D2D1::Point2F(f.size.width, f.size.height * 0.6f)),
                    sc.Get(), &lg))) {
                fill = lg;
            }
        }
        if (s.glass) {
            ComPtr<ID2D1SolidColorBrush> gb, gl;
            g_zcDc->CreateSolidColorBrush(s.glassColor, &gb);
            g_zcDc->CreateSolidColorBrush(s.glassBorder, &gl);
            float rad = s.timeSize * scale * 0.18f;
            auto rr = D2D1::RoundedRect(f.glassRect, rad, rad);
            if (gb) g_zcDc->FillRoundedRectangle(rr, gb.Get());
            if (gl) {
                auto inner = rr;
                inner.rect.left += 0.5f; inner.rect.top += 0.5f;
                inner.rect.right -= 0.5f; inner.rect.bottom -= 0.5f;
                g_zcDc->DrawRoundedRectangle(inner, gl.Get(), 1.0f * scale);
            }
        }
    }
    ID2D1Brush* acc = main && accentBrush ? (ID2D1Brush*)accentBrush.Get() : brush.Get();
    if (f.hasBar && acc) {
        float w = f.bar.right - f.bar.left;
        g_zcDc->FillRoundedRectangle(D2D1::RoundedRect(f.bar, w / 2, w / 2), acc);
    }
    if (f.calHi && acc) {
        float rr = (f.calHiRect.bottom - f.calHiRect.top) * 0.22f;
        g_zcDc->FillRoundedRectangle(D2D1::RoundedRect(f.calHiRect, rr, rr), acc);
    }
    if (!f.minis.empty()) {
        ComPtr<ID2D1StrokeStyle> round;
        g_zcD2dFactory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND),
            nullptr, 0, &round);
        ID2D1Brush* trk = main && trackBrush ? (ID2D1Brush*)trackBrush.Get() : brush.Get();
        for (const auto& m : f.minis) {
            float sw = (std::max)(2.0f, m.r * 0.14f), r = m.r - sw / 2;
            g_zcDc->DrawEllipse(D2D1::Ellipse(m.c, r, r), trk, sw);
            if (m.frac > 0.999f) {
                g_zcDc->DrawEllipse(D2D1::Ellipse(m.c, r, r), acc, sw);
            } else if (m.frac > 0.005f) {
                if (auto g = ZcArc(m.c, r, m.frac))
                    g_zcDc->DrawGeometry(g.Get(), acc, sw, round.Get());
            }
        }
    }
    if (f.dial && brush) {
        D2D1_POINT_2F c = f.dialC;
        float r = f.dialR;
        ID2D1Brush* trk = main && trackBrush ? (ID2D1Brush*)trackBrush.Get() : brush.Get();
        if (s.layout == ZcLayoutKind::ring) {
            float sw = (std::max)(3.0f, r * 0.05f);
            g_zcDc->DrawEllipse(D2D1::Ellipse(c, r - sw / 2, r - sw / 2), trk, sw);
            float frac = (f.t.wMinute + (g_zc.seconds ? f.t.wSecond / 60.0f : 0)) / 60.0f;
            ComPtr<ID2D1StrokeStyle> round;
            g_zcD2dFactory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND),
                nullptr, 0, &round);
            if (frac > 0.999f) {
                g_zcDc->DrawEllipse(D2D1::Ellipse(c, r - sw / 2, r - sw / 2), acc, sw);
            } else if (frac > 0.001f) {
                if (auto g = ZcArc(c, r - sw / 2, frac))
                    g_zcDc->DrawGeometry(g.Get(), acc, sw, round.Get());
            }
        } else {
            // Analog face: rim, 60 ticks (12 long), hands.
            float rim = (std::max)(2.0f, r * 0.025f);
            g_zcDc->DrawEllipse(D2D1::Ellipse(c, r - rim / 2, r - rim / 2), fill.Get(), rim);
            for (int i = 0; i < 60; i++) {
                bool hour = i % 5 == 0;
                float a = i * 6.2831853f / 60, sn = std::sin(a), cs = std::cos(a);
                float r1 = r * 0.9f, r0 = r1 - r * (hour ? 0.12f : 0.04f);
                g_zcDc->DrawLine(D2D1::Point2F(c.x + r0 * sn, c.y - r0 * cs),
                                 D2D1::Point2F(c.x + r1 * sn, c.y - r1 * cs),
                                 hour ? fill.Get() : trk,
                                 hour ? r * 0.025f : r * 0.012f);
            }
            ComPtr<ID2D1StrokeStyle> round;
            g_zcD2dFactory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND),
                nullptr, 0, &round);
            float sec = g_zc.seconds ? f.t.wSecond : 0;
            float mins = f.t.wMinute + sec / 60.0f;
            float hrs = (f.t.wHour % 12) + mins / 60.0f;
            auto hand = [&](float turns, float len, float tail, float w, ID2D1Brush* b) {
                float a = turns * 6.2831853f, sn = std::sin(a), cs = std::cos(a);
                g_zcDc->DrawLine(D2D1::Point2F(c.x - tail * sn, c.y + tail * cs),
                                 D2D1::Point2F(c.x + len * sn, c.y - len * cs), b, w,
                                 round.Get());
            };
            hand(hrs / 12, r * 0.5f, r * 0.08f, r * 0.06f, fill.Get());
            hand(mins / 60, r * 0.74f, r * 0.08f, r * 0.04f, fill.Get());
            if (g_zc.seconds) hand(sec / 60, r * 0.8f, r * 0.15f, r * 0.015f, acc);
            g_zcDc->FillEllipse(D2D1::Ellipse(c, r * 0.04f, r * 0.04f),
                                g_zc.seconds ? acc : fill.Get());
        }
    }
    for (const auto& p : f.pieces) {
        if (p.ghost) {
            if (main && ghostBrush && p.layout)
                g_zcDc->DrawTextLayout(p.pos, p.layout.Get(), ghostBrush.Get());
            continue;
        }
        if (p.card && cardBrush) {
            float pad = p.size.height * 0.12f;
            D2D1_RECT_F r = D2D1::RectF(p.pos.x - pad, p.pos.y,
                                        p.pos.x + p.size.width + pad,
                                        p.pos.y + p.size.height);
            float radius = p.size.height * 0.08f;
            g_zcDc->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius),
                                         cardBrush.Get());
            float mid = (r.top + r.bottom) / 2;
            g_zcDc->DrawLine(D2D1::Point2F(r.left, mid), D2D1::Point2F(r.right, mid),
                             lineBrush.Get(), 2.0f * scale);
        }
        if (p.dim && main && dimBrush && p.layout) {
            g_zcDc->DrawTextLayout(p.pos, p.layout.Get(), dimBrush.Get());
        } else if (fill && p.layout) {
            g_zcDc->DrawTextLayout(p.pos, p.layout.Get(), fill.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }
    }
    g_zcDc->SetTarget(oldTarget.Get());
    g_zcDc->SetTransform(oldTransform);
    return SUCCEEDED(list->Close()) ? list : nullptr;
}

ComPtr<ID2D1Effect> ZcShadowEffect(ID2D1Image* input, D2D1_COLOR_F color,
                                   float blur) {
    ComPtr<ID2D1Effect> fx;
    if (FAILED(g_zcDc->CreateEffect(kZcClsidShadow, &fx))) return nullptr;
    fx->SetInput(0, input);
    // Standard deviation ~ radius / 2, the convention used by CSS blur.
    fx->SetValue(kZcShadowPropBlur, blur / 2.0f);
    fx->SetValue(kZcShadowPropColor,
                 D2D1::Vector4F(color.r, color.g, color.b, color.a));
    return fx;
}

bool g_zcDiagPending = true;  // log one diagnostic frame
// Dragged centers relative to the monitor work area, logical px.
// 0 clock, 1 calendar, 2 resources.
struct ZcWPos { bool set = false; int x = 0, y = 0; };
ZcWPos g_zcWPos[3];
bool g_zcPosLoaded = false;

struct ZcSlot {
    bool on = false;
    POINT center{};
    D2D1_SIZE_F size{};
    HWND wnd = nullptr;
};
ZcSlot g_zcSlot[3];
HWND g_zcDragMoving = nullptr;

void ZcLoadPos() {
    auto one = [](int i, const wchar_t* setK, const wchar_t* xK, const wchar_t* yK) {
        g_zcWPos[i].set = Wh_GetIntValue(setK, 0) != 0;
        g_zcWPos[i].x = Wh_GetIntValue(xK, 0);
        g_zcWPos[i].y = Wh_GetIntValue(yK, 0);
    };
    one(0, L"posSet", L"posX", L"posY");
    one(1, L"calSet", L"calX", L"calY");
    one(2, L"resSet", L"resX", L"resY");
    g_zcPosLoaded = true;
}

void ZcDrawFrame(const ZcFrame& f, D2D1_POINT_2F center, bool rotate) {
    const ZcStyle& s = g_zc.style;
    if (f.pieces.empty() && !f.dial && f.minis.empty()) return;

    float rot = rotate ? s.rotation : 0;
    D2D1_POINT_2F half = D2D1::Point2F(f.size.width / 2, f.size.height / 2);
    D2D1::Matrix3x2F place =
        D2D1::Matrix3x2F::Rotation(rot, half) *
        D2D1::Matrix3x2F::Translation(center.x - half.x, center.y - half.y);

    ComPtr<ID2D1CommandList> content = ZcRecord(f, s.text, true);
    if (!content) return;

    // Hollow text: the shadow/glow come from the solid mask, the fill is
    // punched out afterwards so only the outline remains.
    ComPtr<ID2D1CommandList> solid =
        s.hollow ? ZcRecord(f, D2D1::ColorF(1, 1, 1, 1), false) : content;

    if (s.shadow && s.shadowColor.a > 0 && solid) {
        if (auto fx = ZcShadowEffect(solid.Get(), s.shadowColor, s.shadowBlur * f.scale)) {
            // Offset in screen space so the light comes from the same side
            // regardless of rotation.
            g_zcDc->SetTransform(place * D2D1::Matrix3x2F::Translation(
                                             s.shadowDx * f.scale, s.shadowDy * f.scale));
            g_zcDc->DrawImage(fx.Get());
        }
    }
    if (s.dup) {
        float o = s.dupOffset * f.scale;
        if (auto a = ZcRecord(f, s.dupA, false)) {
            g_zcDc->SetTransform(D2D1::Matrix3x2F::Translation(-o, -o * 0.4f) * place);
            g_zcDc->DrawImage(a.Get());
        }
        if (auto b = ZcRecord(f, s.dupB, false)) {
            g_zcDc->SetTransform(D2D1::Matrix3x2F::Translation(o, o * 0.4f) * place);
            g_zcDc->DrawImage(b.Get());
        }
    }
    g_zcDc->SetTransform(place);
    if (s.glow) {
        ComPtr<ID2D1CommandList> mask = ZcRecord(f, s.glowColor, false);
        if (mask) {
            // Wide soft halo plus a tighter bright core.
            if (auto fx = ZcShadowEffect(mask.Get(), s.glowColor, s.glowRadius * f.scale))
                g_zcDc->DrawImage(fx.Get());
            if (auto fx = ZcShadowEffect(mask.Get(), s.glowColor,
                                         s.glowRadius * f.scale * 0.35f))
                g_zcDc->DrawImage(fx.Get());
        }
    }
    if (s.outline || s.hollow) {
        D2D1_COLOR_F oc = s.outline ? s.outlineColor : s.text;
        ComPtr<ID2D1CommandList> mask = ZcRecord(f, oc, false);
        ComPtr<ID2D1Effect> fx;
        if (mask && SUCCEEDED(g_zcDc->CreateEffect(kZcClsidMorphology, &fx))) {
            UINT32 w = (UINT32)ZcClamp((int)std::lround(s.outlineWidth * f.scale), 1, 50);
            fx->SetInput(0, mask.Get());
            fx->SetValue(kZcMorphPropMode, (UINT32)kZcMorphModeDilate);
            fx->SetValue(kZcMorphPropWidth, w * 2 + 1);
            fx->SetValue(kZcMorphPropHeight, w * 2 + 1);
            g_zcDc->DrawImage(fx.Get());
        }
    }
    if (s.hollow) {
        if (solid) {
            g_zcDc->DrawImage(solid.Get(), D2D1_INTERPOLATION_MODE_LINEAR,
                              D2D1_COMPOSITE_MODE_DESTINATION_OUT);
        }
    } else {
        g_zcDc->DrawImage(content.Get());
    }
    g_zcDc->SetTransform(D2D1::Matrix3x2F::Identity());
}

////////////////////////////////////////////////////////////////////////////////
// Rendering (swap chain + DirectComposition as in Desktop Live Overlay)

bool ZcInitDirectX() {
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                   D3D11_SDK_VERSION, &g_zcD3d, nullptr, nullptr);
    if (FAILED(hr)) {
        // No usable GPU (e.g. remote session): fall back to WARP.
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                               D3D11_SDK_VERSION, &g_zcD3d, nullptr, nullptr);
    }
    if (FAILED(hr) || FAILED(g_zcD3d.As(&g_zcDxgiDevice)) ||
        FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&g_zcDxgiFactory))) ||
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 IID_PPV_ARGS(&g_zcD2dFactory))) ||
        FAILED(g_zcD2dFactory->CreateDevice(g_zcDxgiDevice.Get(),
                                            &g_zcD2dDevice)) ||
        FAILED(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(g_zcDwrite.GetAddressOf())))) {
        Wh_Log(L"InitDirectX failed: 0x%08X", (unsigned)hr);
        return false;
    }
    return true;
}

void ZcUninitDirectX() {
    g_zcDwrite.Reset();
    g_zcD2dDevice.Reset();
    g_zcD2dFactory.Reset();
    g_zcDxgiFactory.Reset();
    g_zcDxgiDevice.Reset();
    g_zcD3d.Reset();
}

void ZcReleaseSwapChain() {
    g_zcCompVisual.Reset();
    g_zcCompTarget.Reset();
    g_zcCompDevice.Reset();
    if (g_zcDc) g_zcDc->SetTarget(nullptr);
    g_zcTarget.Reset();
    g_zcDc.Reset();
    g_zcSwapChain.Reset();
}

bool ZcBindTarget() {
    ComPtr<IDXGISurface2> surface;
    if (FAILED(g_zcSwapChain->GetBuffer(0, IID_PPV_ARGS(&surface)))) return false;
    D2D1_BITMAP_PROPERTIES1 props = {};
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    g_zcTarget.Reset();
    if (FAILED(g_zcDc->CreateBitmapFromDxgiSurface(surface.Get(), props,
                                                   &g_zcTarget))) {
        return false;
    }
    g_zcDc->SetTarget(g_zcTarget.Get());
    // Work in physical pixels; DPI scaling is applied per monitor.
    g_zcDc->SetDpi(96.0f, 96.0f);
    return true;
}

bool ZcCreateSwapChain(UINT width, UINT height) {
    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = width ? width : 1;
    scd.Height = height ? height : 1;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    HRESULT hr = g_zcDxgiFactory->CreateSwapChainForComposition(
        g_zcDxgiDevice.Get(), &scd, nullptr, &g_zcSwapChain);
    if (FAILED(hr) ||
        FAILED(hr = g_zcD2dDevice->CreateDeviceContext(
                   D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_zcDc)) ||
        !ZcBindTarget() ||
        FAILED(hr = DCompositionCreateDevice(g_zcDxgiDevice.Get(),
                                             IID_PPV_ARGS(&g_zcCompDevice))) ||
        FAILED(hr = g_zcCompDevice->CreateTargetForHwnd(g_zcOverlayWnd, TRUE,
                                                        &g_zcCompTarget)) ||
        FAILED(hr = g_zcCompDevice->CreateVisual(&g_zcCompVisual)) ||
        FAILED(hr = g_zcCompVisual->SetContent(g_zcSwapChain.Get())) ||
        FAILED(hr = g_zcCompTarget->SetRoot(g_zcCompVisual.Get())) ||
        FAILED(hr = g_zcCompDevice->Commit())) {
        Wh_Log(L"CreateSwapChain failed: 0x%08X", (unsigned)hr);
        return false;
    }
    return true;
}

bool ZcResizeSwapChain(UINT width, UINT height) {
    if (!g_zcSwapChain || !g_zcDc) return false;
    g_zcDc->SetTarget(nullptr);
    g_zcTarget.Reset();
    if (FAILED(g_zcSwapChain->ResizeBuffers(0, width ? width : 1,
                                            height ? height : 1,
                                            DXGI_FORMAT_UNKNOWN, 0))) {
        return false;
    }
    return ZcBindTarget();
}

POINT g_zcLastCenter{};      // screen coords of the first drawn block center
HMONITOR g_zcLastMon = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Drag handles (only while drag-to-position is on). One window per visible widget.

#define ZC_DRAG_CLASS (L"ZenDesktopClockDrag_" WH_MOD_ID)
bool g_zcDragClass = false;
DWORD g_zcLastDragRender = 0;
void ZcRender();
void ZcSyncDragWindow();

static const wchar_t* kZcSetKey[] = {L"posSet", L"calSet", L"resSet"};
static const wchar_t* kZcXKey[] = {L"posX", L"calX", L"resX"};
static const wchar_t* kZcYKey[] = {L"posY", L"calY", L"resY"};
static const wchar_t* kZcDragLabel[] = {
    L"时钟\r\n拖中间移动\r\n拖角落改大小",
    L"日历\r\n拖中间移动\r\n拖角落改大小",
    L"资源占用\r\n拖中间移动\r\n拖角落改大小",
};

RECT ZcDragRect(POINT c, D2D1_SIZE_F s) {
    float w = (std::max)(s.width, 180.0f) + 36;
    float h = (std::max)(s.height, 72.0f) + 36;
    return {c.x - (LONG)(w / 2), c.y - (LONG)(h / 2), c.x + (LONG)(w / 2),
            c.y + (LONG)(h / 2)};
}

void ZcStoreCenter(int idx, POINT c) {
    if (idx < 0 || idx > 2) return;
    HMONITOR m = g_zcLastMon ? g_zcLastMon : MonitorFromPoint(c, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(m, &mi)) return;
    float scale = ZcMonitorScale(m);
    if (scale < 0.01f) scale = 1;
    LONG cx = (std::min)((std::max)(c.x, mi.rcWork.left), mi.rcWork.right);
    LONG cy = (std::min)((std::max)(c.y, mi.rcWork.top), mi.rcWork.bottom);
    g_zcWPos[idx].set = true;
    g_zcWPos[idx].x = (int)std::lround((cx - mi.rcWork.left) / scale);
    g_zcWPos[idx].y = (int)std::lround((cy - mi.rcWork.top) / scale);
    g_zcPosLoaded = true;
}

void ZcPosFromWindow(HWND hWnd) {
    int idx = (int)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (idx < 0 || idx > 2) return;
    RECT wr;
    GetWindowRect(hWnd, &wr);
    ZcStoreCenter(idx, POINT{(wr.left + wr.right) / 2, (wr.top + wr.bottom) / 2});
}

// 0 top-left, 1 top-right, 2 bottom-left, 3 bottom-right, -1 elsewhere.
int ZcHitCorner(HWND hWnd, POINT screen) {
    RECT rc;
    GetWindowRect(hWnd, &rc);
    const int m = 28;
    bool L = screen.x < rc.left + m;
    bool R = screen.x >= rc.right - m;
    bool T = screen.y < rc.top + m;
    bool B = screen.y >= rc.bottom - m;
    if (T && L) return 0;
    if (T && R) return 1;
    if (B && L) return 2;
    if (B && R) return 3;
    return -1;
}

struct ZcCornerDrag {
    bool on = false;
    int idx = 0;
    POINT anchor{};
    float startDist = 1;
    int startPct = 100;
    float startW = 1, startH = 1;
    int signX = 1, signY = 1;
};
ZcCornerDrag g_zcCorner;

static const wchar_t* kZcSizeKey[] = {L"size", L"calendarSize", L"resourcesSize"};
static const wchar_t* kZcSizePin[] = {L"sizePin", L"calSizePin", L"resSizePin"};
static const wchar_t* kZcSizeSeen[] = {L"sizeSeen", L"calSizeSeen", L"resSizeSeen"};

int ZcPctOf(int idx) {
    if (idx == 0) return g_zc.size;
    if (idx == 1) return g_zc.calendarSize;
    return g_zc.resourcesSize;
}

void ZcSetPct(int idx, int pct) {
    pct = ZcClamp(pct, 20, 800);
    if (idx == 0) g_zc.size = pct;
    else if (idx == 1) g_zc.calendarSize = pct;
    else g_zc.resourcesSize = pct;
}

void ZcSaveLiveSize(int idx) {
    if (idx < 0 || idx > 2) return;
    Wh_SetIntValue(kZcSizePin[idx], ZcPctOf(idx));
    int setting = Wh_GetIntSetting(kZcSizeKey[idx]);
    if (setting <= 0) setting = 100;
    Wh_SetIntValue(kZcSizeSeen[idx], setting);
    Wh_SetIntValue(kZcXKey[idx], g_zcWPos[idx].x);
    Wh_SetIntValue(kZcYKey[idx], g_zcWPos[idx].y);
    Wh_SetIntValue(kZcSetKey[idx], 1);
}

void ZcBeginCorner(HWND hWnd, int corner, POINT cursor) {
    int idx = (int)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (idx < 0 || idx > 2 || !g_zcSlot[idx].on) return;
    const ZcSlot& slot = g_zcSlot[idx];
    float hw = slot.size.width / 2, hh = slot.size.height / 2;
    if (hw < 1) hw = 1;
    if (hh < 1) hh = 1;
    float ax, ay;
    int sx, sy;
    if (corner == 0) { ax = slot.center.x + hw; ay = slot.center.y + hh; sx = -1; sy = -1; }
    else if (corner == 1) { ax = slot.center.x - hw; ay = slot.center.y + hh; sx = 1; sy = -1; }
    else if (corner == 2) { ax = slot.center.x + hw; ay = slot.center.y - hh; sx = -1; sy = 1; }
    else { ax = slot.center.x - hw; ay = slot.center.y - hh; sx = 1; sy = 1; }
    float dx = cursor.x - ax, dy = cursor.y - ay;
    float dist = std::hypot(dx, dy);
    if (dist < 8) dist = 8;
    g_zcCorner = {};
    g_zcCorner.on = true;
    g_zcCorner.idx = idx;
    g_zcCorner.anchor = {(LONG)std::lround(ax), (LONG)std::lround(ay)};
    g_zcCorner.startDist = dist;
    g_zcCorner.startPct = ZcPctOf(idx);
    g_zcCorner.startW = slot.size.width;
    g_zcCorner.startH = slot.size.height;
    g_zcCorner.signX = sx;
    g_zcCorner.signY = sy;
    SetCapture(hWnd);
}

void ZcCornerMove(POINT cursor) {
    ZcCornerDrag& d = g_zcCorner;
    if (!d.on) return;
    float dist = std::hypot((float)(cursor.x - d.anchor.x), (float)(cursor.y - d.anchor.y));
    float factor = dist / d.startDist;
    int pct = ZcClamp((int)std::lround(d.startPct * factor), 20, 800);
    ZcSetPct(d.idx, pct);
    float ratio = d.startPct ? (float)pct / (float)d.startPct : 1;
    POINT c{d.anchor.x + (LONG)std::lround(d.signX * d.startW * ratio / 2),
            d.anchor.y + (LONG)std::lround(d.signY * d.startH * ratio / 2)};
    ZcStoreCenter(d.idx, c);
    DWORD now = GetTickCount();
    if (now - g_zcLastDragRender > 33) {
        g_zcLastDragRender = now;
        ZcRender();
        ZcSyncDragWindow();
    }
}

void ZcEndCorner() {
    if (!g_zcCorner.on) return;
    int idx = g_zcCorner.idx;
    g_zcCorner.on = false;
    if (GetCapture()) ReleaseCapture();
    ZcRender();
    ZcSyncDragWindow();
    ZcSaveLiveSize(idx);
    Wh_Log(L"corner size saved [%d]: %d%%", idx, ZcPctOf(idx));
}

LRESULT CALLBACK ZcDragProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    int idx = (int)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (idx < 0 || idx > 2) idx = 0;
    switch (msg) {
        case WM_NCHITTEST: {
            POINT pt{(SHORT)LOWORD(lp), (SHORT)HIWORD(lp)};
            if (ZcHitCorner(hWnd, pt) >= 0) return HTCLIENT;
            return HTCAPTION;
        }
        case WM_SETCURSOR: {
            POINT pt;
            GetCursorPos(&pt);
            int c = ZcHitCorner(hWnd, pt);
            LPCWSTR cur = IDC_SIZEALL;
            if (c == 0 || c == 3) cur = IDC_SIZENWSE;
            else if (c > 0) cur = IDC_SIZENESW;
            SetCursor(LoadCursor(nullptr, cur));
            return TRUE;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_LBUTTONDOWN: {
            POINT pt{(SHORT)LOWORD(lp), (SHORT)HIWORD(lp)};
            ClientToScreen(hWnd, &pt);
            int c = ZcHitCorner(hWnd, pt);
            if (c >= 0) ZcBeginCorner(hWnd, c, pt);
            return 0;
        }
        case WM_MOUSEMOVE:
            if (g_zcCorner.on && g_zcCorner.idx == idx) {
                POINT pt;
                GetCursorPos(&pt);
                ZcCornerMove(pt);
            }
            return 0;
        case WM_LBUTTONUP:
        case WM_CAPTURECHANGED:
            if (g_zcCorner.on && g_zcCorner.idx == idx) ZcEndCorner();
            return 0;
        case WM_MOVING: {
            g_zcDragMoving = hWnd;
            ZcPosFromWindow(hWnd);
            DWORD now = GetTickCount();
            if (now - g_zcLastDragRender > 33) {
                g_zcLastDragRender = now;
                ZcRender();
            }
            break;
        }
        case WM_EXITSIZEMOVE:
            g_zcDragMoving = nullptr;
            ZcPosFromWindow(hWnd);
            Wh_SetIntValue(kZcXKey[idx], g_zcWPos[idx].x);
            Wh_SetIntValue(kZcYKey[idx], g_zcWPos[idx].y);
            Wh_SetIntValue(kZcSetKey[idx], 1);
            Wh_Log(L"dragged position saved [%d]: (%d,%d) logical px", idx, g_zcWPos[idx].x,
                   g_zcWPos[idx].y);
            ZcRender();
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hWnd, &ps);
            RECT rc;
            GetClientRect(hWnd, &rc);
            HBRUSH bg = CreateSolidBrush(RGB(20, 20, 24));
            FillRect(dc, &rc, bg);
            DeleteObject(bg);
            HBRUSH fr = CreateSolidBrush(RGB(0, 170, 255));
            for (int i = 0; i < 3; i++) {
                RECT r = rc;
                InflateRect(&r, -i, -i);
                FrameRect(dc, &r, fr);
            }
            DeleteObject(fr);
            HBRUSH grip = CreateSolidBrush(RGB(255, 255, 255));
            for (int i = 0; i < 4; i++) {
                int x = (i & 1) ? rc.right - 16 : rc.left + 6;
                int y = (i & 2) ? rc.bottom - 16 : rc.top + 6;
                RECT g{x, y, x + 10, y + 10};
                FillRect(dc, &g, grip);
            }
            DeleteObject(grip);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(255, 255, 255));
            HFONT font = CreateFont(-18, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                                    CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
            HGDIOBJ old = SelectObject(dc, font);
            RECT tr = rc;
            InflateRect(&tr, -18, -18);
            DrawText(dc, kZcDragLabel[idx], -1, &tr, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
            SelectObject(dc, old);
            DeleteObject(font);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            if (g_zcDragMoving == hWnd) g_zcDragMoving = nullptr;
            if (g_zcCorner.on && g_zcCorner.idx == idx) g_zcCorner.on = false;
            for (auto& s : g_zcSlot) {
                if (s.wnd == hWnd) s.wnd = nullptr;
            }
            return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

void ZcDestroyDragWindow() {
    g_zcDragMoving = nullptr;
    for (auto& s : g_zcSlot) {
        if (s.wnd) DestroyWindow(s.wnd);
        s.wnd = nullptr;
    }
}

void ZcSyncDragWindow() {
    bool want = g_zc.drag && g_zcOverlayWnd && !g_zcUnloading;
    if (!want) {
        ZcDestroyDragWindow();
        return;
    }
    if (!g_zcDragClass) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = ZcDragProc;
        wc.hInstance = ZcModule();
        wc.hCursor = LoadCursor(nullptr, IDC_SIZEALL);
        wc.lpszClassName = ZC_DRAG_CLASS;
        g_zcDragClass = RegisterClass(&wc) != 0;
        if (!g_zcDragClass) return;
    }
    for (int i = 0; i < 3; i++) {
        if (!g_zcSlot[i].on) {
            if (g_zcSlot[i].wnd) {
                DestroyWindow(g_zcSlot[i].wnd);
                g_zcSlot[i].wnd = nullptr;
            }
            continue;
        }
        RECT r = ZcDragRect(g_zcSlot[i].center, g_zcSlot[i].size);
        int w = r.right - r.left, h = r.bottom - r.top;
        if (!g_zcSlot[i].wnd) {
            g_zcSlot[i].wnd = CreateWindowEx(
                WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE, ZC_DRAG_CLASS, L"",
                WS_POPUP, r.left, r.top, w, h, nullptr, nullptr, ZcModule(), nullptr);
            if (!g_zcSlot[i].wnd) {
                Wh_Log(L"drag window %d failed: %u", i, GetLastError());
                continue;
            }
            SetWindowLongPtr(g_zcSlot[i].wnd, GWLP_USERDATA, i);
            SetLayeredWindowAttributes(g_zcSlot[i].wnd, 0, 110, LWA_ALPHA);
            SetWindowPos(g_zcSlot[i].wnd, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            Wh_Log(L"drag window %d shown", i);
        } else if (g_zcSlot[i].wnd != g_zcDragMoving) {
            SetWindowPos(g_zcSlot[i].wnd, HWND_TOP, r.left, r.top, w, h,
                         SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}


// Wallpaper Engine (WPEDesktopDX11Window) and similar tools add children to
// the same WorkerW and can cover the overlay. Keep the overlay on top of its
// siblings. Returns true if other siblings exist (so we keep checking).
bool ZcEnsureTop() {
    if (!g_zcOverlayWnd) return false;
    HWND host = GetParent(g_zcOverlayWnd);
    if (!host) return false;
    HWND first = GetWindow(host, GW_CHILD);
    if (first && first != g_zcOverlayWnd) {
        SetWindowPos(g_zcOverlayWnd, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        Wh_Log(L"raised overlay above %p", first);
    }
    return GetWindow(g_zcOverlayWnd, GW_HWNDNEXT) != nullptr ||
           GetWindow(host, GW_CHILD) != g_zcOverlayWnd;
}

bool g_zcHostDiagDone = false;  // z-order dump only once per overlay

// ---- Diagnostics (logged once per overlay / on demand) ----

std::wstring ZcClassOf(HWND h) {
    WCHAR b[128] = L"";
    if (h) GetClassName(h, b, ARRAYSIZE(b));
    return b;
}

void ZcLogHostDiag() {
    if (!g_zcOverlayWnd) return;
    HWND host = GetParent(g_zcOverlayWnd);
    HWND hp = host ? GetParent(host) : nullptr;
    RECT hr{}, orc{};
    if (host) GetWindowRect(host, &hr);
    GetWindowRect(g_zcOverlayWnd, &orc);
    Wh_Log(L"diag host=%p class=%s parent=%p class=%s visible=%d rect=(%ld,%ld,%ld,%ld)",
           host, ZcClassOf(host).c_str(), hp, ZcClassOf(hp).c_str(),
           host ? (int)IsWindowVisible(host) : -1, hr.left, hr.top, hr.right, hr.bottom);
    Wh_Log(L"diag overlay=%p visible=%d rect=(%ld,%ld,%ld,%ld) exstyle=0x%08lX",
           g_zcOverlayWnd, (int)IsWindowVisible(g_zcOverlayWnd), orc.left, orc.top,
           orc.right, orc.bottom, (unsigned long)GetWindowLongPtr(g_zcOverlayWnd, GWL_EXSTYLE));
    Wh_Log(L"diag virtual screen=(%d,%d %dx%d)", GetSystemMetrics(SM_XVIRTUALSCREEN),
           GetSystemMetrics(SM_YVIRTUALSCREEN), GetSystemMetrics(SM_CXVIRTUALSCREEN),
           GetSystemMetrics(SM_CYVIRTUALSCREEN));
    // Z-order of the host's parent children (top first).
    HWND top = hp ? hp : host;
    int i = 0;
    for (HWND c = top ? GetWindow(top, GW_CHILD) : nullptr; c && i < 12;
         c = GetWindow(c, GW_HWNDNEXT), i++) {
        RECT r{};
        GetWindowRect(c, &r);
        Wh_Log(L"diag z[%d] %p class=%s visible=%d rect=(%ld,%ld,%ld,%ld)%s", i, c,
               ZcClassOf(c).c_str(), (int)IsWindowVisible(c), r.left, r.top, r.right,
               r.bottom, c == host ? L" <= host" : L"");
    }
    if (host) {
        i = 0;
        for (HWND c = GetWindow(host, GW_CHILD); c && i < 8;
             c = GetWindow(c, GW_HWNDNEXT), i++) {
            Wh_Log(L"diag host child[%d] %p class=%s%s", i, c, ZcClassOf(c).c_str(),
                   c == g_zcOverlayWnd ? L" <= overlay" : L"");
        }
    }
}

void ZcRender() {
    if (g_zcUnloading || !g_zcDc || !g_zcSwapChain || !g_zcOverlayWnd) return;

    HWND host = GetParent(g_zcOverlayWnd);
    RECT hostRc{};
    if (host) GetWindowRect(host, &hostRc);

    if (ZcResOn()) ZcSampleResources();
    g_zcDc->BeginDraw();
    g_zcDc->Clear(D2D1::ColorF(0, 0, 0, 0));
    for (auto& slot : g_zcSlot) slot.on = false;
    bool first = true;
    for (HMONITOR m : ZcTargetMonitors()) {
        MONITORINFO mi{sizeof(mi)};
        if (!GetMonitorInfo(m, &mi)) continue;
        float scale = ZcMonitorScale(m);
        RECT r = mi.rcWork;
        OffsetRect(&r, -hostRc.left, -hostRc.top);
        int col = g_zc.anchor % 3, row = g_zc.anchor / 3;
        float fx[] = {0.22f, 0.5f, 0.78f};
        float fy[] = {0.2f, 0.5f, 0.8f};
        D2D1_POINT_2F anchor = D2D1::Point2F(
            r.left + (r.right - r.left) * fx[col] + g_zc.offsetX * scale,
            r.top + (r.bottom - r.top) * fy[row] + g_zc.offsetY * scale);
        if (!g_zcPosLoaded) ZcLoadPos();
        auto clampC = [&](D2D1_POINT_2F c) {
            c.x = (std::min)((std::max)(c.x, (float)r.left + 20), (float)r.right - 20);
            c.y = (std::min)((std::max)(c.y, (float)r.top + 20), (float)r.bottom - 20);
            return c;
        };
        auto fromPos = [&](const ZcWPos& p, D2D1_POINT_2F fallback) {
            if (!p.set) return fallback;
            return D2D1::Point2F(r.left + p.x * scale, r.top + p.y * scale);
        };
        ZcFrame clockF, calF, resF;
        if (g_zc.showClock) ZcBuildFrame(scale * g_zc.size / 100.0f, clockF);
        if (g_zc.calendar) ZcBuildCalendar(scale * g_zc.calendarSize / 100.0f, calF);
        if (ZcResOn()) ZcBuildResources(scale * g_zc.resourcesSize / 100.0f, resF);
        float gap = 28.0f * scale;
        D2D1_POINT_2F clockC = clampC(fromPos(g_zcWPos[0], anchor));
        D2D1_POINT_2F calFallback = anchor;
        if (g_zc.showClock) {
            if (g_zcCalPlace == 2) {
                calFallback = D2D1::Point2F(
                    clockC.x, clockC.y + clockF.size.height / 2 + calF.size.height / 2 + gap);
            } else {
                calFallback = D2D1::Point2F(
                    clockC.x + clockF.size.width / 2 + calF.size.width / 2 + gap, clockC.y);
            }
        }
        D2D1_POINT_2F calC = clampC(fromPos(g_zcWPos[1], calFallback));
        D2D1_POINT_2F resBase = g_zc.showClock ? clockC : anchor;
        float resAbove = g_zc.showClock ? clockF.size.height / 2 : 0;
        if (g_zc.calendar && !g_zcWPos[1].set && (g_zcCalPlace == 2 || !g_zc.showClock)) {
            resBase = calC;
            resAbove = calF.size.height / 2;
        }
        D2D1_POINT_2F resFallback =
            D2D1::Point2F(resBase.x, resBase.y + resAbove + resF.size.height / 2 + gap);
        D2D1_POINT_2F resC = clampC(fromPos(g_zcWPos[2], resFallback));
        if (ZcResOn()) ZcDrawFrame(resF, resC, false);
        if (g_zc.calendar) ZcDrawFrame(calF, calC, false);
        if (g_zc.showClock) ZcDrawFrame(clockF, clockC, true);
        if (g_zcDiagPending) {
            Wh_Log(L"diag monitor %p scale=%.2f clock=(%.0f,%.0f) cal=(%.0f,%.0f) res=(%.0f,%.0f)",
                   m, scale, clockC.x, clockC.y, calC.x, calC.y, resC.x, resC.y);
        }
        if (first) {
            first = false;
            auto screenOf = [&](D2D1_POINT_2F c) {
                return POINT{(LONG)std::lround(c.x + hostRc.left),
                             (LONG)std::lround(c.y + hostRc.top)};
            };
            g_zcSlot[0].on = g_zc.showClock;
            g_zcSlot[0].center = screenOf(clockC);
            g_zcSlot[0].size = clockF.size;
            g_zcSlot[1].on = g_zc.calendar;
            g_zcSlot[1].center = screenOf(calC);
            g_zcSlot[1].size = calF.size;
            g_zcSlot[2].on = ZcResOn();
            g_zcSlot[2].center = screenOf(resC);
            g_zcSlot[2].size = resF.size;
            g_zcLastCenter = g_zcSlot[0].on ? g_zcSlot[0].center
                             : g_zcSlot[1].on ? g_zcSlot[1].center
                                              : g_zcSlot[2].center;
            g_zcLastMon = m;
        }

    }
    HRESULT hr = g_zcDc->EndDraw();
    if (FAILED(hr) || g_zcDiagPending) {
        Wh_Log(L"diag EndDraw hr=0x%08X", (unsigned)hr);
    }
    if (hr == (HRESULT)D2DERR_RECREATE_TARGET) {
        Wh_Log(L"Device lost, recreating");
        RECT rc;
        GetClientRect(g_zcOverlayWnd, &rc);
        ZcReleaseSwapChain();
        ZcCreateSwapChain(rc.right, rc.bottom);
        return;
    }
    HRESULT phr = g_zcSwapChain->Present(0, 0);
    if (FAILED(phr) || g_zcDiagPending) {
        DXGI_SWAP_CHAIN_DESC1 d{};
        g_zcSwapChain->GetDesc1(&d);
        Wh_Log(L"diag Present hr=0x%08X swapchain=%ux%u", (unsigned)phr, d.Width, d.Height);
    }
    if (g_zcDiagPending) {
        g_zcDiagPending = false;
        if (!g_zcHostDiagDone) {
            g_zcHostDiagDone = true;
            ZcLogHostDiag();
        }
    }
}

// Wake up right after the next minute (or second) boundary only.
void ZcScheduleNext() {
    if (!g_zcOverlayWnd) return;
    SYSTEMTIME t;
    GetLocalTime(&t);
    UINT ms = g_zc.seconds ? 1000 - t.wMilliseconds
                           : (60 - t.wSecond) * 1000 - t.wMilliseconds;
    // Resource usage refreshes every ~2 s, only while it is shown.
    if (ZcResOn() && ms > 2000) ms = 2000 - t.wMilliseconds % 1000;
    // While other windows share the WorkerW (e.g. Wallpaper Engine), re-check
    // only the z-order every 2 s (no redraw).
    if (ZcEnsureTop()) SetTimer(g_zcOverlayWnd, ZC_TIMER_ZORDER, 2000, nullptr);
    SetTimer(g_zcOverlayWnd, ZC_TIMER_REFRESH, ms + 20, nullptr);
}

void ZcFitToHost() {
    if (!g_zcOverlayWnd) return;
    HWND host = GetParent(g_zcOverlayWnd);
    RECT rc;
    if (!host || !GetWindowRect(host, &rc)) return;
    SetWindowPos(g_zcOverlayWnd, nullptr, 0, 0, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
    ZcRender();
}

////////////////////////////////////////////////////////////////////////////////
// Windows

void ZcCreateOverlay();

LRESULT CALLBACK ZcOverlayProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER:
            if (wp == ZC_TIMER_ZORDER) {
                if (g_zcUnloading || !ZcEnsureTop()) KillTimer(hWnd, ZC_TIMER_ZORDER);
                return 0;
            }
            if (wp == ZC_TIMER_REFRESH && !g_zcUnloading) {
                ZcRender();
                ZcScheduleNext();
                return 0;
            }
            break;
        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* pos = (const WINDOWPOS*)lp;
            if (!(pos->flags & SWP_NOSIZE) && !g_zcUnloading) {
                ZcResizeSwapChain(pos->cx, pos->cy);
                ZcRender();
            }
            break;
        }
        case WM_DESTROY:
            KillTimer(hWnd, ZC_TIMER_REFRESH);
            KillTimer(hWnd, ZC_TIMER_ZORDER);
            ZcDestroyDragWindow();
            ZcReleaseSwapChain();
            g_zcOverlayWnd = nullptr;
            if (!g_zcUnloading && g_zcMessageWnd) {
                SetTimer(g_zcMessageWnd, ZC_TIMER_MSG_RECREATE, 500, nullptr);
            }
            return 0;
        case ZC_WM_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
        case ZC_WM_SETTINGS:
            ZcLoadSettings();
            ZcLoadPos();
            g_zcDiagPending = true;
            ZcRender();
            ZcScheduleNext();
            ZcSyncDragWindow();
            return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

LRESULT CALLBACK ZcMessageProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_zcTaskbarCreatedMsg && g_zcTaskbarCreatedMsg) {
        // Shell restarted inside this process: rebind to the new WorkerW.
        Wh_Log(L"TaskbarCreated");
        if (!g_zcUnloading) SetTimer(hWnd, ZC_TIMER_MSG_RECREATE, 1000, nullptr);
        return 0;
    }
    switch (msg) {
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
            if (!g_zcUnloading) SetTimer(hWnd, ZC_TIMER_MSG_DISPLAY, 300, nullptr);
            return 0;
        case WM_TIMECHANGE:
            if (g_zcOverlayWnd && !g_zcUnloading) {
                ZcRender();
                ZcScheduleNext();
            }
            return 0;
        case WM_TIMER:
            KillTimer(hWnd, wp);
            if (g_zcUnloading) return 0;
            if (wp == ZC_TIMER_MSG_DISPLAY) {
                ZcFitToHost();
            } else if (wp == ZC_TIMER_MSG_RECREATE) {
                HWND worker = ZcGetWorkerW();
                if (g_zcOverlayWnd && worker &&
                    GetParent(g_zcOverlayWnd) != worker) {
                    DestroyWindow(g_zcOverlayWnd);  // re-arms this timer
                } else if (!g_zcOverlayWnd) {
                    ZcCreateOverlay();
                }
            }
            return 0;
        case ZC_WM_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            g_zcMessageWnd = nullptr;
            return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

bool g_zcOverlayClass = false, g_zcMessageClass = false;

bool ZcRegister(bool& done, PCWSTR name, WNDPROC proc) {
    if (done) return true;
    WNDCLASS wc = {};
    wc.lpfnWndProc = proc;
    wc.hInstance = ZcModule();
    wc.lpszClassName = name;
    done = RegisterClass(&wc) != 0;
    if (!done) Wh_Log(L"RegisterClass failed: %u", GetLastError());
    return done;
}

void ZcCreateOverlay() {
    if (g_zcOverlayWnd || g_zcUnloading) return;
    if (!g_zcLazyInit.exchange(true)) {
        g_zcInitOk = ZcInitDirectX();
    }
    if (!g_zcInitOk) return;
    HWND worker = ZcGetWorkerW();
    if (!worker) {
        Wh_Log(L"WorkerW not found");
        return;
    }
    if (!ZcRegister(g_zcOverlayClass, ZC_OVERLAY_CLASS, ZcOverlayProc)) return;
    RECT rc;
    GetWindowRect(worker, &rc);
    g_zcOverlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        ZC_OVERLAY_CLASS, nullptr, WS_CHILD | WS_VISIBLE | WS_DISABLED, 0, 0,
        rc.right - rc.left, rc.bottom - rc.top, worker, nullptr, ZcModule(),
        nullptr);
    if (!g_zcOverlayWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }
    Wh_Log(L"overlay created on %p (%ldx%ld)", worker, rc.right - rc.left,
           rc.bottom - rc.top);
    g_zcDiagPending = true;
    g_zcHostDiagDone = false;
    ZcEnsureTop();
    if (ZcCreateSwapChain(rc.right - rc.left, rc.bottom - rc.top)) {
        ZcRender();
        ZcScheduleNext();
        ZcSyncDragWindow();
    } else {
        ZcReleaseSwapChain();
    }
}

void ZcCreateMessageWindow() {
    if (g_zcMessageWnd) return;
    if (!ZcRegister(g_zcMessageClass, ZC_MESSAGE_CLASS, ZcMessageProc)) return;
    // Hidden top-level window: receives WM_DISPLAYCHANGE / TaskbarCreated.
    g_zcMessageWnd = CreateWindowEx(0, ZC_MESSAGE_CLASS, nullptr, 0, 0, 0, 0, 0,
                                    nullptr, nullptr, ZcModule(), nullptr);
}

void WINAPI ZcCreateAll(void*) {
    ZcCreateOverlay();
    ZcCreateMessageWindow();
}

////////////////////////////////////////////////////////////////////////////////
// Hook: the desktop FolderView appears (first start / explorer restart)

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t ZcCreateWindowExW_Original;

HWND WINAPI ZcCreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name,
                                   DWORD style, int x, int y, int w, int h,
                                   HWND parent, HMENU menu, HINSTANCE inst,
                                   LPVOID param) {
    HWND hWnd = ZcCreateWindowExW_Original(exStyle, cls, name, style, x, y, w,
                                           h, parent, menu, inst, param);
    if (!hWnd || g_zcUnloading || !ZcIsFolderView(hWnd)) return hWnd;
    Wh_Log(L"FolderView created");
    g_zcCreateTimer = SetTimer(nullptr, g_zcCreateTimer, 1000,
                               [](HWND, UINT, UINT_PTR id, DWORD) {
                                   KillTimer(nullptr, id);
                                   g_zcCreateTimer = 0;
                                   ZcCreateAll(nullptr);
                               });
    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Mod life cycle

BOOL Wh_ModInit() {
    Wh_Log(L">");
    ZcLoadSettings();
    g_zcTaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)ZcCreateWindowExW_Hook,
                       (void**)&ZcCreateWindowExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (HWND progman = ZcGetProgman()) {
        ZcRunFromWindowThread(progman, ZcCreateAll, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_zcUnloading = true;
    if (HWND progman = ZcGetProgman()) {
        ZcRunFromWindowThread(
            progman,
            [](void*) {
                if (g_zcCreateTimer) {
                    KillTimer(nullptr, g_zcCreateTimer);
                    g_zcCreateTimer = 0;
                }
            },
            nullptr);
    }
    if (g_zcOverlayWnd) SendMessage(g_zcOverlayWnd, ZC_WM_CLEANUP, 0, 0);
    if (g_zcMessageWnd) SendMessage(g_zcMessageWnd, ZC_WM_CLEANUP, 0, 0);
    if (g_zcOverlayClass) UnregisterClass(ZC_OVERLAY_CLASS, ZcModule());
    if (g_zcMessageClass) UnregisterClass(ZC_MESSAGE_CLASS, ZcModule());
    if (g_zcDragClass) UnregisterClass(ZC_DRAG_CLASS, ZcModule());
    ZcGpuClose();
    ZcUninitDirectX();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (g_zcOverlayWnd) {
        SendMessage(g_zcOverlayWnd, ZC_WM_SETTINGS, 0, 0);
    } else {
        ZcLoadSettings();
    }
}
