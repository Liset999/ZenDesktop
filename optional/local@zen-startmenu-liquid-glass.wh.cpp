// ==WindhawkMod==
// @id              zen-startmenu-liquid-glass
// @name            ZenDesktop: Liquid glass Start menu (prototype)
// @name:zh-CN      ZenDesktop: 液态玻璃开始菜单（实验）
// @description     Experimental: draws a refracting, blurred glass layer behind the Win11 Start menu using Windows.Graphics.Capture and a D3D11 shader. Clears the Start acrylic backdrop via XAML diagnostics when possible.
// @description:zh-CN 实验功能：用屏幕捕获 + D3D11 着色器在开始菜单下面画一层带折射、模糊、色散和高光边的液态玻璃；并尽量把开始菜单自带亚克力背景清成透明。
// @version         0.1.0
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @architecture    x86-64
// @compilerOptions -ld3d11 -ldxgi -ldcomp -ld3dcompiler_47 -lruntimeobject -lole32 -loleaut32 -luuid -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZenDesktop: 液态玻璃开始菜单（实验）

**原型，默认关闭。** 打开「总开关」后：

1. 开始菜单（`StartMenuExperienceHost.exe`）打开时，捕获它后面的桌面 → 模糊 → 圆角边缘折射 → 色散 → 高光边，画在开始菜单下面一层。
2. 尽量通过 XAML 诊断把 `Border#AcrylicBorder` / `Border#AcrylicOverlay` 的背景设成透明（带限速防回退），让玻璃透出来。
3. 菜单收起就停捕获；进程一加载就预热捕获，减轻第一帧闪一下。

## 使用前提与副作用
- 建议先把「ZenDesktop: 开始菜单美化」的背景调成尽量透明，或暂时关掉它的亚克力主题，避免两套背景抢。
- 本 mod 会把开始菜单窗口和玻璃层设为「不参与屏幕捕获」。**截图、录屏、直播时开始菜单不会出现。**
- 「作用于搜索页」默认关。打开后才处理 `SearchHost.exe`。

## 省电与安全
- 帧率上限默认 30；全屏游戏 / 演示模式暂停；显卡设备丢失后本次会话自动停用。
- 任何初始化或渲染失败都保持开始菜单原样（fail-safe）。

## 已知风险（未在各版本实机核验）
- `AcrylicBorder` / `AcrylicOverlay` 等元素名随 Windows 大版本可能变化。
- 与开始菜单美化同时开时，样式可能来回抢；限速防回退会尽量压住。
- 第一帧仍可能偶发闪一下（预热失败时）。

## 更新日志
- **0.1.0**：首个原型。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: false
  $name: Master switch
  $name:zh-CN: 总开关
  $description: Off by default. Experimental.
  $description:zh-CN: 默认关闭。实验功能。
- applyToSearch: false
  $name: Also apply to Search host
  $name:zh-CN: 作用于搜索页
  $description: Off by default. When on, also runs inside SearchHost.exe.
  $description:zh-CN: 默认关闭。打开后才会处理 SearchHost.exe。
- maxFps: 30
  $name: FPS cap
  $name:zh-CN: 帧率上限
  $description: 5-60. Lower uses less GPU.
  $description:zh-CN: 5～60，越低越省电。
- blur: 14
  $name: Blur radius (px)
  $name:zh-CN: 模糊
- refraction: 20
  $name: Edge refraction (px)
  $name:zh-CN: 折射
- chroma: 15
  $name: Chromatic aberration (%)
  $name:zh-CN: 色散
- rim: 55
  $name: Rim highlight (%)
  $name:zh-CN: 边缘高光
- tint: 6
  $name: White tint (%)
  $name:zh-CN: 白色衬底
- cornerRadius: 24
  $name: Corner radius (px)
  $name:zh-CN: 圆角
*/
// ==/WindhawkModSettings==

// Own implementation. Capture + D3D11 glass approach matches
// optional/local@zen-liquid-glass.wh.cpp. No third-party or decompiled code.

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <windhawk_api.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dcomp.h>
#include <dxgi1_2.h>
#include <inspectable.h>
#include <roapi.h>
#include <shellapi.h>
#include <winstring.h>
#include <wrl/client.h>
#include <xamlom.h>
#include <ocidl.h>
#include <oleauto.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <mutex>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace zsmlg {

struct SizeInt32 {
    INT32 Width;
    INT32 Height;
};

MIDL_INTERFACE("3628E81B-3CAC-4C60-B7F4-23CE0E0C3356")
ICaptureItemInterop : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateForWindow(HWND, REFIID, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateForMonitor(HMONITOR, REFIID, void**) = 0;
};

MIDL_INTERFACE("79C3F95B-31F7-4EC2-A464-632EF5D30760")
ICaptureItem : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Size(SizeInt32*) = 0;
};

MIDL_INTERFACE("814E42A9-F70F-4AD7-939B-FDDCC6EB880D")
ICaptureSession : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE StartCapture() = 0;
};

MIDL_INTERFACE("2C39AE40-7D2E-5044-804E-8B6799D4CF9E")
ICaptureSession2 : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_IsCursorCaptureEnabled(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsCursorCaptureEnabled(boolean) = 0;
};

MIDL_INTERFACE("F2CDD966-22AE-5EA1-9596-3A289344C3BE")
ICaptureSession3 : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_IsBorderRequired(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsBorderRequired(boolean) = 0;
};

MIDL_INTERFACE("FA50C623-38DA-4B32-ACF3-FA9734AD800E")
ICaptureFrame : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Surface(IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SystemRelativeTime(INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentSize(SizeInt32*) = 0;
};

MIDL_INTERFACE("24EB6D22-1975-422E-82E7-780DBD8DDF24")
ICaptureFramePool : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Recreate(IInspectable*, INT32, INT32, SizeInt32) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetNextFrame(ICaptureFrame**) = 0;
    virtual HRESULT STDMETHODCALLTYPE add_FrameArrived(IUnknown*, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE remove_FrameArrived(INT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCaptureSession(ICaptureItem*, ICaptureSession**) = 0;
};

MIDL_INTERFACE("589B103F-6BBC-5DF5-A991-02E28B3B66D5")
ICaptureFramePoolStatics2 : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE CreateFreeThreaded(IInspectable*, INT32, INT32, SizeInt32,
                                                         ICaptureFramePool**) = 0;
};

MIDL_INTERFACE("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1")
IDxgiInterfaceAccess : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetInterface(REFIID, void**) = 0;
};

MIDL_INTERFACE("30D5A829-7FA4-4026-83BB-D75BAE4EA99E")
IClosable : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Close() = 0;
};

constexpr INT32 kPixelFormatB8G8R8A8 = 87;

}  // namespace zsmlg

#define ZS_IID(T) __uuidof(zsmlg::T)

// ---------------------------------------------------------------------------
// Settings

struct Settings {
    bool enabled = false;
    bool applyToSearch = false;
    int maxFps = 30;
    float blur = 14, refraction = 20, chroma = 0.15f, rim = 0.55f, tint = 0.06f;
    float cornerRadius = 24;
};
Settings g_settings;

bool IsSearchHostProcess() {
    WCHAR path[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return false;
    const WCHAR* base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;
    return _wcsicmp(base, L"SearchHost.exe") == 0;
}

bool ShouldRunInThisProcess() {
    if (!g_settings.enabled) return false;
    if (IsSearchHostProcess() && !g_settings.applyToSearch) return false;
    return true;
}

void LoadSettings() {
    auto clampi = [](int v, int lo, int hi) { return std::min(hi, std::max(lo, v)); };
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;
    g_settings.applyToSearch = Wh_GetIntSetting(L"applyToSearch") != 0;
    g_settings.maxFps = clampi(Wh_GetIntSetting(L"maxFps"), 5, 60);
    g_settings.blur = (float)clampi(Wh_GetIntSetting(L"blur"), 0, 40);
    g_settings.refraction = (float)clampi(Wh_GetIntSetting(L"refraction"), 0, 60);
    g_settings.chroma = clampi(Wh_GetIntSetting(L"chroma"), 0, 100) / 100.0f;
    g_settings.rim = clampi(Wh_GetIntSetting(L"rim"), 0, 100) / 100.0f;
    g_settings.tint = clampi(Wh_GetIntSetting(L"tint"), 0, 60) / 100.0f;
    g_settings.cornerRadius = (float)clampi(Wh_GetIntSetting(L"cornerRadius"), 0, 64);
}

// ---------------------------------------------------------------------------
// Shader (same approach as taskbar liquid glass).

const char kShader[] = R"(
cbuffer P : register(b0) {
    float2 size; float2 srcOrigin;
    float2 srcSize; float radius; float blur;
    float refract; float chroma; float rim; float tint;
};
Texture2D tex : register(t0);
SamplerState smp : register(s0);
struct V { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
V vs(uint id : SV_VertexID) {
    V o; float2 uv = float2((id << 1) & 2, id & 2);
    o.pos = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1); o.uv = uv; return o;
}
float sdRR(float2 p, float2 b, float r) {
    float2 q = abs(p) - b + r;
    return length(max(q, 0)) + min(max(q.x, q.y), 0) - r;
}
float3 blurAt(float2 px) {
    float3 acc = 0; float ws = 0;
    [unroll] for (int i = -2; i <= 2; i++)
    [unroll] for (int j = -2; j <= 2; j++) {
        float w = exp(-(i * i + j * j) / 4.5);
        float2 o = float2(i, j) * (blur * 0.5);
        acc += tex.SampleLevel(smp, (srcOrigin + px + o) / srcSize, 0).rgb * w;
        ws += w;
    }
    return acc / ws;
}
float4 ps(V i) : SV_Target {
    float2 px = i.uv * size;
    float2 c = size * 0.5;
    float2 p = px - c;
    float r = min(radius, min(c.x, c.y));
    float d = sdRR(p, c, r);
    float a = saturate(0.5 - d);
    if (a <= 0) return 0;
    float e = 0.5;
    float2 g = float2(sdRR(p + float2(e, 0), c, r) - sdRR(p - float2(e, 0), c, r),
                      sdRR(p + float2(0, e), c, r) - sdRR(p - float2(0, e), c, r));
    float2 n = g / max(length(g), 1e-4);
    float band = max(min(size.x, size.y) * 0.4, 4.0);
    float k = saturate(1.0 + d / band);
    float2 off = -n * (k * k * refract);
    float3 col;
    col.r = blurAt(px + off * (1.0 + chroma)).r;
    col.g = blurAt(px + off).g;
    col.b = blurAt(px + off * (1.0 - chroma)).b;
    col = lerp(col, float3(1, 1, 1), tint);
    float3 L = normalize(float3(-0.35, -0.85, 0.4));
    float spec = pow(saturate(dot(normalize(float3(n, 0.55)), L)), 10.0) * k * rim;
    col += spec;
    return float4(saturate(col) * a, a);
}
)";

struct alignas(16) ShaderParams {
    float size[2], srcOrigin[2];
    float srcSize[2], radius, blur;
    float refract, chroma, rim, tint;
};

struct Glass {
    HWND hwnd = nullptr;
    HWND core = nullptr;
    RECT coreRect = {};
    RECT monRect = {};
    HMONITOR monitor = nullptr;
    ComPtr<ID3D11Device> dev;
    ComPtr<ID3D11DeviceContext> ctx;
    ComPtr<IDXGISwapChain1> swap;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<IDCompositionDevice> dcomp;
    ComPtr<IDCompositionTarget> target;
    ComPtr<IDCompositionVisual> visual;
    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    ComPtr<ID3D11Buffer> cb;
    ComPtr<ID3D11SamplerState> sampler;
    ComPtr<ID3D11Texture2D> src;
    ComPtr<ID3D11ShaderResourceView> srv;
    D3D11_BOX srcBox = {};
    ComPtr<IInspectable> winrtDevice;
    ComPtr<zsmlg::ICaptureItem> item;
    ComPtr<zsmlg::ICaptureFramePool> pool;
    ComPtr<zsmlg::ICaptureSession> session;
    zsmlg::SizeInt32 poolSize = {};
    int width = 0, height = 0;
    bool visible = false;
    bool hasContent = false;
    bool captureWarm = false;
};

std::atomic<bool> g_stop{false};
std::atomic<bool> g_deviceLost{false};
HANDLE g_thread = nullptr;
HANDLE g_wakeEvent = nullptr;
DWORD g_corePrevAffinity = WDA_NONE;
HWND g_coreAffinityWnd = nullptr;

template <typename T>
HRESULT GetFactory(PCWSTR cls, REFIID iid, T** out) {
    HSTRING h = nullptr;
    HRESULT hr = WindowsCreateString(cls, (UINT32)wcslen(cls), &h);
    if (FAILED(hr)) return hr;
    hr = RoGetActivationFactory(h, iid, (void**)out);
    WindowsDeleteString(h);
    return hr;
}

bool CompileShaders(Glass& g) {
    ComPtr<ID3DBlob> vsb, psb, err;
    if (FAILED(D3DCompile(kShader, sizeof(kShader) - 1, "zsmlg", nullptr, nullptr, "vs", "vs_4_0",
                          D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsb, &err)) ||
        FAILED(D3DCompile(kShader, sizeof(kShader) - 1, "zsmlg", nullptr, nullptr, "ps", "ps_4_0",
                          D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psb, &err))) {
        Wh_Log(L"[smlg] shader compile failed: %S",
               err ? (const char*)err->GetBufferPointer() : "?");
        return false;
    }
    return SUCCEEDED(g.dev->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(),
                                               nullptr, &g.vs)) &&
           SUCCEEDED(g.dev->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(),
                                              nullptr, &g.ps));
}

bool CreateDevice(Glass& g) {
    D3D_FEATURE_LEVEL fl[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
                              D3D_FEATURE_LEVEL_10_0};
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                 D3D11_CREATE_DEVICE_BGRA_SUPPORT, fl, ARRAYSIZE(fl),
                                 D3D11_SDK_VERSION, &g.dev, nullptr, &g.ctx))) {
        return false;
    }
    if (!CompileShaders(g)) return false;

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(ShaderParams);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(g.dev->CreateBuffer(&bd, nullptr, &g.cb))) return false;
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    if (FAILED(g.dev->CreateSamplerState(&sd, &g.sampler))) return false;

    ComPtr<IDXGIDevice> dxgi;
    g.dev.As(&dxgi);
    if (FAILED(DCompositionCreateDevice(dxgi.Get(), IID_PPV_ARGS(&g.dcomp)))) return false;
    if (FAILED(g.dcomp->CreateTargetForHwnd(g.hwnd, TRUE, &g.target))) return false;
    if (FAILED(g.dcomp->CreateVisual(&g.visual))) return false;

    using Create_t = HRESULT(WINAPI*)(IDXGIDevice*, IInspectable**);
    auto create = (Create_t)GetProcAddress(GetModuleHandleW(L"d3d11.dll"),
                                           "CreateDirect3D11DeviceFromDXGIDevice");
    return create && SUCCEEDED(create(dxgi.Get(), &g.winrtDevice));
}

bool EnsureSwapChain(Glass& g, int w, int h) {
    if (g.swap && w == g.width && h == g.height) return true;
    g.rtv.Reset();
    if (g.swap) {
        if (FAILED(g.swap->ResizeBuffers(2, w, h, DXGI_FORMAT_B8G8R8A8_UNORM, 0))) return false;
    } else {
        ComPtr<IDXGIDevice> dxgi;
        ComPtr<IDXGIAdapter> adapter;
        ComPtr<IDXGIFactory2> factory;
        g.dev.As(&dxgi);
        dxgi->GetAdapter(&adapter);
        adapter->GetParent(IID_PPV_ARGS(&factory));
        DXGI_SWAP_CHAIN_DESC1 d = {};
        d.Width = w;
        d.Height = h;
        d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        d.SampleDesc.Count = 1;
        d.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        d.BufferCount = 2;
        d.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        d.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        if (FAILED(factory->CreateSwapChainForComposition(g.dev.Get(), &d, nullptr, &g.swap)))
            return false;
        g.visual->SetContent(g.swap.Get());
        g.target->SetRoot(g.visual.Get());
        g.dcomp->Commit();
    }
    ComPtr<ID3D11Texture2D> back;
    g.swap->GetBuffer(0, IID_PPV_ARGS(&back));
    if (FAILED(g.dev->CreateRenderTargetView(back.Get(), nullptr, &g.rtv))) return false;
    g.width = w;
    g.height = h;
    return true;
}

void StopCapture(Glass& g) {
    ComPtr<zsmlg::IClosable> c;
    if (g.session && SUCCEEDED(g.session.As(&c))) c->Close();
    c.Reset();
    if (g.pool && SUCCEEDED(g.pool.As(&c))) c->Close();
    g.session.Reset();
    g.pool.Reset();
    g.item.Reset();
    g.monitor = nullptr;
    g.captureWarm = false;
}

bool StartCapture(Glass& g, HMONITOR mon) {
    StopCapture(g);
    ComPtr<zsmlg::ICaptureItemInterop> interop;
    if (FAILED(GetFactory(L"Windows.Graphics.Capture.GraphicsCaptureItem", ZS_IID(ICaptureItemInterop),
                          interop.GetAddressOf())) ||
        FAILED(interop->CreateForMonitor(mon, ZS_IID(ICaptureItem), (void**)g.item.GetAddressOf()))) {
        Wh_Log(L"[smlg] CreateForMonitor failed");
        return false;
    }
    g.item->get_Size(&g.poolSize);
    ComPtr<zsmlg::ICaptureFramePoolStatics2> statics;
    if (FAILED(GetFactory(L"Windows.Graphics.Capture.Direct3D11CaptureFramePool",
                          ZS_IID(ICaptureFramePoolStatics2), statics.GetAddressOf())) ||
        FAILED(statics->CreateFreeThreaded(g.winrtDevice.Get(), zsmlg::kPixelFormatB8G8R8A8, 2,
                                           g.poolSize, &g.pool)) ||
        FAILED(g.pool->CreateCaptureSession(g.item.Get(), &g.session))) {
        Wh_Log(L"[smlg] frame pool / session failed");
        StopCapture(g);
        return false;
    }
    ComPtr<zsmlg::ICaptureSession2> s2;
    if (SUCCEEDED(g.session.As(&s2))) s2->put_IsCursorCaptureEnabled(false);
    ComPtr<zsmlg::ICaptureSession3> s3;
    if (SUCCEEDED(g.session.As(&s3))) s3->put_IsBorderRequired(false);
    if (FAILED(g.session->StartCapture())) {
        StopCapture(g);
        return false;
    }
    g.monitor = mon;
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    g.monRect = mi.rcMonitor;
    g.captureWarm = true;
    return true;
}

bool PullFrame(Glass& g) {
    ComPtr<zsmlg::ICaptureFrame> frame;
    if (FAILED(g.pool->TryGetNextFrame(&frame)) || !frame) return false;
    zsmlg::SizeInt32 cs = {};
    frame->get_ContentSize(&cs);
    if (cs.Width != g.poolSize.Width || cs.Height != g.poolSize.Height) {
        g.poolSize = cs;
        g.pool->Recreate(g.winrtDevice.Get(), zsmlg::kPixelFormatB8G8R8A8, 2, cs);
        return false;
    }
    ComPtr<IInspectable> surface;
    ComPtr<zsmlg::IDxgiInterfaceAccess> access;
    ComPtr<ID3D11Texture2D> tex;
    if (FAILED(frame->get_Surface(&surface)) || FAILED(surface.As(&access)) ||
        FAILED(access->GetInterface(IID_PPV_ARGS(&tex)))) {
        return false;
    }

    const int margin = (int)(g_settings.blur + g_settings.refraction) + 4;
    LONG l = std::max<LONG>(0, g.coreRect.left - g.monRect.left - margin);
    LONG t = std::max<LONG>(0, g.coreRect.top - g.monRect.top - margin);
    LONG r = std::min<LONG>(cs.Width, g.coreRect.right - g.monRect.left + margin);
    LONG b = std::min<LONG>(cs.Height, g.coreRect.bottom - g.monRect.top + margin);
    if (r <= l || b <= t) return false;
    UINT w = (UINT)(r - l), h = (UINT)(b - t);

    D3D11_TEXTURE2D_DESC have = {};
    if (g.src) g.src->GetDesc(&have);
    if (!g.src || have.Width != w || have.Height != h) {
        g.src.Reset();
        g.srv.Reset();
        D3D11_TEXTURE2D_DESC d = {};
        d.Width = w;
        d.Height = h;
        d.MipLevels = d.ArraySize = 1;
        d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        d.SampleDesc.Count = 1;
        d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        if (FAILED(g.dev->CreateTexture2D(&d, nullptr, &g.src)) ||
            FAILED(g.dev->CreateShaderResourceView(g.src.Get(), nullptr, &g.srv))) {
            return false;
        }
    }
    g.srcBox = {(UINT)l, (UINT)t, 0, (UINT)r, (UINT)b, 1};
    g.ctx->CopySubresourceRegion(g.src.Get(), 0, 0, 0, 0, tex.Get(), 0, &g.srcBox);
    g.hasContent = true;
    return true;
}

HRESULT Render(Glass& g) {
    ShaderParams p = {};
    p.size[0] = (float)g.width;
    p.size[1] = (float)g.height;
    p.srcOrigin[0] = (float)(g.coreRect.left - g.monRect.left - (LONG)g.srcBox.left);
    p.srcOrigin[1] = (float)(g.coreRect.top - g.monRect.top - (LONG)g.srcBox.top);
    p.srcSize[0] = (float)(g.srcBox.right - g.srcBox.left);
    p.srcSize[1] = (float)(g.srcBox.bottom - g.srcBox.top);
    p.radius = g_settings.cornerRadius;
    p.blur = g_settings.blur;
    p.refract = g_settings.refraction;
    p.chroma = g_settings.chroma;
    p.rim = g_settings.rim;
    p.tint = g_settings.tint;
    g.ctx->UpdateSubresource(g.cb.Get(), 0, nullptr, &p, 0, 0);

    D3D11_VIEWPORT vp = {0, 0, (float)g.width, (float)g.height, 0, 1};
    float clear[4] = {0, 0, 0, 0};
    ID3D11RenderTargetView* rtv = g.rtv.Get();
    g.ctx->ClearRenderTargetView(rtv, clear);
    g.ctx->OMSetRenderTargets(1, &rtv, nullptr);
    g.ctx->RSSetViewports(1, &vp);
    g.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g.ctx->IASetInputLayout(nullptr);
    g.ctx->VSSetShader(g.vs.Get(), nullptr, 0);
    g.ctx->PSSetShader(g.ps.Get(), nullptr, 0);
    ID3D11Buffer* cb = g.cb.Get();
    g.ctx->PSSetConstantBuffers(0, 1, &cb);
    ID3D11ShaderResourceView* srv = g.srv.Get();
    g.ctx->PSSetShaderResources(0, 1, &srv);
    ID3D11SamplerState* s = g.sampler.Get();
    g.ctx->PSSetSamplers(0, 1, &s);
    g.ctx->Draw(3, 0);
    return g.swap->Present(0, 0);
}

bool ShouldPause() {
    QUERY_USER_NOTIFICATION_STATE st;
    if (SUCCEEDED(SHQueryUserNotificationState(&st)) &&
        (st == QUNS_BUSY || st == QUNS_RUNNING_D3D_FULL_SCREEN || st == QUNS_PRESENTATION_MODE)) {
        return true;
    }
    return false;
}

void SetVisible(Glass& g, bool v) {
    if (g.visible == v) return;
    g.visible = v;
    ShowWindow(g.hwnd, v ? SW_SHOWNOACTIVATE : SW_HIDE);
}

void ExcludeCoreFromCapture(HWND core) {
    if (core == g_coreAffinityWnd) return;
    if (g_coreAffinityWnd && IsWindow(g_coreAffinityWnd)) {
        SetWindowDisplayAffinity(g_coreAffinityWnd, g_corePrevAffinity);
    }
    g_coreAffinityWnd = nullptr;
    if (!core) return;
    DWORD prev = WDA_NONE;
    GetWindowDisplayAffinity(core, &prev);
    if (SetWindowDisplayAffinity(core, WDA_EXCLUDEFROMCAPTURE)) {
        g_corePrevAffinity = prev;
        g_coreAffinityWnd = core;
    } else {
        Wh_Log(L"[smlg] could not exclude Start from capture (%u)", GetLastError());
    }
}

HWND FindCoreWindow() {
    struct Param {
        HWND found;
        DWORD pid;
    } p{nullptr, GetCurrentProcessId()};
    EnumWindows(
        [](HWND h, LPARAM lp) -> BOOL {
            auto* p = (Param*)lp;
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid != p->pid) return TRUE;
            WCHAR cls[64] = {};
            if (GetClassNameW(h, cls, ARRAYSIZE(cls)) &&
                _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                p->found = h;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&p);
    return p.found;
}

bool CoreLooksOpen(HWND core, RECT* outRc) {
    if (!core || !IsWindow(core) || !IsWindowVisible(core)) return false;
    RECT rc = {};
    if (!GetWindowRect(core, &rc)) return false;
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    // Closed / dormant Start host often sits at 0-size or off-screen tiny.
    if (w < 200 || h < 200) return false;
    if (outRc) *outRc = rc;
    return true;
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    if (m == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(h, m, w, l);
}

// ---------------------------------------------------------------------------
// Minimal classic-COM XAML diagnostics TAP: clear AcrylicBorder/Overlay.
// Rate-limited re-apply acts as anti-revert (no C++/WinRT DependencyObject
// callbacks; keeps mingw syntax-checkable and fail-safe).

// Own CLSID — must not collide with startmenu-acrylic's WindhawkTAP.
static constexpr CLSID CLSID_ZenSmLgTAP = {
    0x6b2e9c41, 0x8f3a, 0x4d71, {0x9c, 0x55, 0xa1, 0xb2, 0xc3, 0xd4, 0xe5, 0xf6}};

static constexpr int kAntiRevertMaxPerWindow = 30;
static constexpr ULONGLONG kAntiRevertWindowMs = 2000;

struct ClearedElement {
    InstanceHandle handle = 0;
    unsigned int bgIndex = UINT_MAX;
};

struct XamlClearState {
    std::mutex mu;
    ComPtr<IXamlDiagnostics> diag;
    ComPtr<IVisualTreeService> vts;
    std::vector<ClearedElement> cleared;
    ULONGLONG windowStart = 0;
    int appliesInWindow = 0;
    bool active = false;
};

XamlClearState g_xaml;
std::atomic<bool> g_tapReady{false};

bool AntiRevertAllow() {
    ULONGLONG now = GetTickCount64();
    std::lock_guard<std::mutex> lock(g_xaml.mu);
    if (now - g_xaml.windowStart > kAntiRevertWindowMs) {
        g_xaml.windowStart = now;
        g_xaml.appliesInWindow = 0;
    }
    if (g_xaml.appliesInWindow >= kAntiRevertMaxPerWindow) return false;
    g_xaml.appliesInWindow++;
    return true;
}

bool FindBackgroundIndex(IVisualTreeService* vts, InstanceHandle handle, unsigned int* outIndex) {
    unsigned int srcCount = 0, propCount = 0;
    PropertyChainSource* sources = nullptr;
    PropertyChainValue* values = nullptr;
    if (FAILED(vts->GetPropertyValuesChain(handle, &srcCount, &sources, &propCount, &values)) ||
        !values) {
        return false;
    }
    bool found = false;
    for (unsigned int i = 0; i < propCount; i++) {
        if (values[i].PropertyName && _wcsicmp(values[i].PropertyName, L"Background") == 0) {
            *outIndex = values[i].Index;
            found = true;
            break;
        }
    }
    // CoTaskMemFree each BSTR? GetPropertyValuesChain allocates; free arrays.
    if (sources) {
        for (unsigned int i = 0; i < srcCount; i++) {
            SysFreeString(sources[i].TargetType);
            SysFreeString(sources[i].Name);
            SysFreeString(sources[i].SrcInfo.FileName);
            SysFreeString(sources[i].SrcInfo.Hash);
        }
        CoTaskMemFree(sources);
    }
    if (values) {
        for (unsigned int i = 0; i < propCount; i++) {
            SysFreeString(values[i].Type);
            SysFreeString(values[i].DeclaringType);
            SysFreeString(values[i].ValueType);
            SysFreeString(values[i].ItemType);
            SysFreeString(values[i].Value);
            SysFreeString(values[i].PropertyName);
        }
        CoTaskMemFree(values);
    }
    return found;
}

bool ClearElementBackground(InstanceHandle handle) {
    ComPtr<IVisualTreeService> vts;
    {
        std::lock_guard<std::mutex> lock(g_xaml.mu);
        vts = g_xaml.vts;
    }
    if (!vts || handle == 0) return false;
    if (!AntiRevertAllow()) return false;

    unsigned int bgIndex = UINT_MAX;
    if (!FindBackgroundIndex(vts.Get(), handle, &bgIndex)) return false;

    BSTR typeName = SysAllocString(L"Windows.UI.Xaml.Media.SolidColorBrush");
    BSTR value = SysAllocString(L"Transparent");
    InstanceHandle brush = 0;
    HRESULT hr = vts->CreateInstance(typeName, value, &brush);
    SysFreeString(typeName);
    SysFreeString(value);
    if (FAILED(hr) || brush == 0) return false;

    hr = vts->SetProperty(handle, brush, bgIndex);
    if (FAILED(hr)) return false;

    std::lock_guard<std::mutex> lock(g_xaml.mu);
    bool known = false;
    for (auto& e : g_xaml.cleared) {
        if (e.handle == handle) {
            e.bgIndex = bgIndex;
            known = true;
            break;
        }
    }
    if (!known) g_xaml.cleared.push_back(ClearedElement{handle, bgIndex});
    return true;
}

bool NameIsAcrylicBackdrop(BSTR name) {
    if (!name) return false;
    return _wcsicmp(name, L"AcrylicBorder") == 0 || _wcsicmp(name, L"AcrylicOverlay") == 0 ||
           _wcsicmp(name, L"AppBorder") == 0 || _wcsicmp(name, L"AccentAppBorder") == 0;
}

void ReapplyCleared() {
    std::vector<ClearedElement> snap;
    {
        std::lock_guard<std::mutex> lock(g_xaml.mu);
        snap = g_xaml.cleared;
    }
    for (auto& e : snap) ClearElementBackground(e.handle);
}

extern "C" __declspec(dllexport) HRESULT STDMETHODCALLTYPE DllCanUnloadNow(void);
extern "C" __declspec(dllexport) HRESULT STDMETHODCALLTYPE DllGetClassObject(REFCLSID, REFIID, LPVOID*);

// Classic COM watcher (no C++/WinRT).
struct VisualTreeWatcher : public IVisualTreeServiceCallback2 {
    LONG refCount = 1;
    virtual ~VisualTreeWatcher() = default;
    ComPtr<IXamlDiagnostics> diag;
    ComPtr<IVisualTreeService> vts;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IVisualTreeServiceCallback ||
            riid == IID_IVisualTreeServiceCallback2) {
            *ppv = static_cast<IVisualTreeServiceCallback2*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG n = InterlockedDecrement(&refCount);
        if (n == 0) delete this;
        return (ULONG)n;
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation, VisualElement element,
                                                 VisualMutationType mutation) override {
        if (mutation != Add) {
            if (mutation == Remove) {
                std::lock_guard<std::mutex> lock(g_xaml.mu);
                g_xaml.cleared.erase(
                    std::remove_if(g_xaml.cleared.begin(), g_xaml.cleared.end(),
                                   [&](const ClearedElement& e) { return e.handle == element.Handle; }),
                    g_xaml.cleared.end());
            }
            return S_OK;
        }
        if (NameIsAcrylicBackdrop(element.Name)) {
            ClearElementBackground(element.Handle);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState,
                                                    LPCWSTR) override {
        return S_OK;
    }
};

VisualTreeWatcher* g_watcher = nullptr;

struct ZenSmLgTAP : public IObjectWithSite {
    LONG refCount = 1;
    virtual ~ZenSmLgTAP() = default;
    ComPtr<IUnknown> site;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IObjectWithSite) {
            *ppv = static_cast<IObjectWithSite*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG n = InterlockedDecrement(&refCount);
        if (n == 0) delete this;
        return (ULONG)n;
    }

    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override {
        if (g_watcher) {
            if (g_watcher->vts) g_watcher->vts->UnadviseVisualTreeChange(g_watcher);
            g_watcher->Release();
            g_watcher = nullptr;
        }
        {
            std::lock_guard<std::mutex> lock(g_xaml.mu);
            g_xaml.diag.Reset();
            g_xaml.vts.Reset();
            g_xaml.cleared.clear();
            g_xaml.active = false;
        }
        g_tapReady = false;
        site = pUnkSite;
        if (!site) return S_OK;

        // Balance the LoadLibrary done by InitializeXamlDiagnosticsEx.
        HMODULE self = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)&DllCanUnloadNow, &self);
        if (self) FreeLibrary(self);

        ComPtr<IXamlDiagnostics> diag;
        if (FAILED(site.As(&diag)) || !diag) return S_OK;
        ComPtr<IVisualTreeService> vts;
        if (FAILED(diag.As(&vts)) || !vts) return S_OK;

        auto* watcher = new (std::nothrow) VisualTreeWatcher();
        if (!watcher) return S_OK;
        watcher->diag = diag;
        watcher->vts = vts;
        if (FAILED(vts->AdviseVisualTreeChange(watcher))) {
            watcher->Release();
            return S_OK;
        }
        g_watcher = watcher;
        {
            std::lock_guard<std::mutex> lock(g_xaml.mu);
            g_xaml.diag = diag;
            g_xaml.vts = vts;
            g_xaml.active = true;
        }
        g_tapReady = true;
        Wh_Log(L"[smlg] XAML TAP ready");
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) override {
        if (!site) return E_FAIL;
        return site.CopyTo(riid, ppvSite);
    }
};

struct ZenSmLgFactory : public IClassFactory {
    LONG refCount = 1;
    virtual ~ZenSmLgFactory() = default;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG n = InterlockedDecrement(&refCount);
        if (n == 0) delete this;
        return (ULONG)n;
    }
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override {
        if (outer) return CLASS_E_NOAGGREGATION;
        auto* tap = new (std::nothrow) ZenSmLgTAP();
        if (!tap) return E_OUTOFMEMORY;
        HRESULT hr = tap->QueryInterface(riid, ppv);
        tap->Release();
        return hr;
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) override { return S_OK; }
};

extern "C" __declspec(dllexport) HRESULT STDMETHODCALLTYPE DllGetClassObject(REFCLSID rclsid,
                                                                             REFIID riid,
                                                                             LPVOID* ppv) {
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    if (rclsid != CLSID_ZenSmLgTAP) return CLASS_E_CLASSNOTAVAILABLE;
    auto* f = new (std::nothrow) ZenSmLgFactory();
    if (!f) return E_OUTOFMEMORY;
    HRESULT hr = f->QueryInterface(riid, ppv);
    f->Release();
    return hr;
}

extern "C" __declspec(dllexport) HRESULT STDMETHODCALLTYPE DllCanUnloadNow(void) {
    return S_FALSE;  // Keep loaded while TAP may be referenced.
}

HMODULE GetSelfModule() {
    HMODULE m = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&DllCanUnloadNow, &m);
    return m;
}

HRESULT InjectXamlTap() {
    HMODULE module = GetSelfModule();
    if (!module) return E_FAIL;
    WCHAR location[MAX_PATH];
    DWORD n = GetModuleFileNameW(module, location, ARRAYSIZE(location));
    if (n == 0 || n >= ARRAYSIZE(location)) return E_FAIL;

    HMODULE wux = LoadLibraryExW(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!wux) return HRESULT_FROM_WIN32(GetLastError());
    auto ixde = (decltype(&InitializeXamlDiagnosticsEx))GetProcAddress(wux, "InitializeXamlDiagnosticsEx");
    if (!ixde) return HRESULT_FROM_WIN32(GetLastError());

    HRESULT hr = E_FAIL;
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[256];
        wsprintfW(connectionName, L"VisualDiagConnection%d", i + 1);
        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_ZenSmLgTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) break;
    }
    return hr;
}

void TryInjectTapOnce() {
    static std::atomic<bool> attempted{false};
    bool expected = false;
    if (!attempted.compare_exchange_strong(expected, true)) return;
    HRESULT hr = InjectXamlTap();
    if (FAILED(hr)) {
        Wh_Log(L"[smlg] XAML TAP inject failed (%08X) — glass still runs; acrylic may block it",
               (unsigned)hr);
        // Fail-safe: leave Start menu styling alone.
    }
}

// ---------------------------------------------------------------------------
// Worker

DWORD WINAPI Worker(LPVOID) {
    RoInitialize(RO_INIT_MULTITHREADED);
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"ZenStartMenuLiquidGlass";
    RegisterClassW(&wc);

    Glass g;
    g.hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
            WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    if (!g.hwnd || !SetWindowDisplayAffinity(g.hwnd, WDA_EXCLUDEFROMCAPTURE) || !CreateDevice(g)) {
        Wh_Log(L"[smlg] init failed, staying off (Start menu untouched)");
        if (g.hwnd) DestroyWindow(g.hwnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        RoUninitialize();
        return 0;
    }

    // Pre-warm capture on the primary monitor so the first Start open has frames.
    HMONITOR warmMon = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (!StartCapture(g, warmMon)) {
        Wh_Log(L"[smlg] pre-warm capture failed; will retry when Start opens");
    }

    TryInjectTapOnce();

    ULONGLONG lastPauseCheck = 0;
    ULONGLONG lastReapply = 0;
    bool paused = false;
    while (!g_stop) {
        const DWORD frameMs = 1000 / (DWORD)g_settings.maxFps;
        MsgWaitForMultipleObjects(1, &g_wakeEvent, FALSE, frameMs, QS_ALLINPUT);
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (g_stop) break;

        ULONGLONG now = GetTickCount64();
        if (now - lastPauseCheck > 1000) {
            lastPauseCheck = now;
            paused = ShouldPause();
        }

        HWND core = FindCoreWindow();
        RECT rc = {};
        bool open = !paused && CoreLooksOpen(core, &rc);

        if (core != g.core) {
            g.core = core;
            ExcludeCoreFromCapture(open ? core : nullptr);
        } else if (open) {
            ExcludeCoreFromCapture(core);
        }

        if (!open) {
            SetVisible(g, false);
            // Keep capture warm while process lives (Start opens/closes often).
            continue;
        }

        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        bool moved = !EqualRect(&rc, &g.coreRect);
        g.coreRect = rc;
        HMONITOR mon = MonitorFromWindow(core, MONITOR_DEFAULTTONEAREST);
        if ((mon != g.monitor || !g.session) && !StartCapture(g, mon)) {
            SetVisible(g, false);
            continue;
        }
        if (moved || !g.visible) {
            // Place glass directly under the Start CoreWindow in z-order.
            SetWindowPos(g.hwnd, core, rc.left, rc.top, w, h, SWP_NOACTIVATE);
        }
        if (!EnsureSwapChain(g, w, h)) {
            g_deviceLost = true;
            break;
        }

        if (now - lastReapply > 400) {
            lastReapply = now;
            ReapplyCleared();
        }

        if (!PullFrame(g) && !moved) continue;
        if (!g.hasContent) continue;

        HRESULT hr = Render(g);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
            Wh_Log(L"[smlg] device removed (%08X), disabling for this session",
                   g.dev->GetDeviceRemovedReason());
            g_deviceLost = true;
            break;
        }
        SetVisible(g, true);
    }

    StopCapture(g);
    ExcludeCoreFromCapture(nullptr);
    if (g.target) g.target->SetRoot(nullptr);
    if (g.dcomp) g.dcomp->Commit();
    HWND dead = g.hwnd;
    g = Glass{};
    if (dead) DestroyWindow(dead);
    UnregisterClassW(wc.lpszClassName, GetModuleHandleW(nullptr));
    RoUninitialize();
    return 0;
}

void StartWorker() {
    if (g_thread || !ShouldRunInThisProcess() || g_deviceLost) return;
    g_stop = false;
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
}

void StopWorker() {
    if (!g_thread) return;
    g_stop = true;
    SetEvent(g_wakeEvent);
    WaitForSingleObject(g_thread, 5000);
    CloseHandle(g_thread);
    CloseHandle(g_wakeEvent);
    g_thread = nullptr;
    g_wakeEvent = nullptr;

    if (g_watcher) {
        if (g_watcher->vts) g_watcher->vts->UnadviseVisualTreeChange(g_watcher);
        g_watcher->Release();
        g_watcher = nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(g_xaml.mu);
        g_xaml.diag.Reset();
        g_xaml.vts.Reset();
        g_xaml.cleared.clear();
        g_xaml.active = false;
    }
    g_tapReady = false;
}

BOOL Wh_ModInit() {
    LoadSettings();
    StartWorker();
    return TRUE;
}

void Wh_ModUninit() { StopWorker(); }

void Wh_ModSettingsChanged() {
    StopWorker();
    LoadSettings();
    g_deviceLost = false;
    StartWorker();
}
