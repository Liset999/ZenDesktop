// ==WindhawkMod==
// @id              zen-liquid-glass
// @name            ZenDesktop: Liquid glass taskbar (prototype)
// @name:zh-CN      ZenDesktop: 液态玻璃任务栏（实验）
// @description     Experimental: draws a refracting, blurred glass layer behind a transparent taskbar using Windows.Graphics.Capture and a D3D11 shader.
// @description:zh-CN 实验功能：用屏幕捕获 + D3D11 着色器在透明任务栏下面画一层带折射、模糊、色散和高光边的液态玻璃。
// @version         0.1.0
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ld3d11 -ldxgi -ldcomp -ld3dcompiler_47 -lruntimeobject -lole32 -luuid -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZenDesktop: 液态玻璃任务栏（实验）

**原型，默认关闭。** 打开「启用」后，在任务栏下面放一层自己画的玻璃：
捕获任务栏后面的屏幕内容 → 模糊 → 圆角边缘折射（越靠边弯得越厉害）→ 轻微色散 → 高光边。

## 使用前提
- 请先在「ZenDesktop: 任务栏美化」里选「透明」主题，否则系统任务栏背景会挡住这层玻璃。
- 本 mod 运行时会把任务栏和玻璃层自身设为「不参与屏幕捕获」（避免把自己捕获进去形成反馈）。
  副作用：**截图、录屏、直播时任务栏不会出现**。关掉本 mod 即恢复。

## 省电与安全
- 帧率上限默认 20 FPS；屏幕内容没变化时不渲染。
- 全屏游戏 / 演示模式 / 任务栏隐藏时暂停并隐藏玻璃层。
- 显卡驱动重置（设备丢失）后本次会话自动停用，不会反复重建（重启资源管理器或重新保存设置可再试）。
- 不注入 dwm，不 hook 系统函数，只在 explorer 里开一个不抢焦点、鼠标穿透的窗口。

## 更新日志
- **0.1.0**：首个原型。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: false
  $name: Enable
  $name:zh-CN: 启用
  $description: Off by default. Needs a transparent taskbar theme.
  $description:zh-CN: 默认关闭。需要任务栏美化选「透明」主题。
- maxFps: 20
  $name: FPS cap
  $name:zh-CN: 帧率上限
  $description: 5-60. Lower uses less GPU.
  $description:zh-CN: 5～60，越低越省电。
- blur: 12
  $name: Blur radius (px)
  $name:zh-CN: 模糊半径（像素）
- refraction: 18
  $name: Edge refraction (px)
  $name:zh-CN: 边缘折射强度（像素）
- chroma: 15
  $name: Chromatic aberration (%)
  $name:zh-CN: 色散（%）
- rim: 60
  $name: Rim highlight (%)
  $name:zh-CN: 高光边（%）
- tint: 8
  $name: White tint (%)
  $name:zh-CN: 白色衬底（%）
- cornerRadius: 12
  $name: Corner radius (px)
  $name:zh-CN: 圆角半径（像素）
*/
// ==/WindhawkModSettings==

// Own implementation. Approach inspired by public descriptions of capture-based
// glass effects; no third-party or decompiled code is used here.

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

#include <algorithm>
#include <atomic>

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------------------
// Minimal ABI declarations for Windows.Graphics.Capture (the mingw headers are
// incomplete). Vtable order follows the public Windows SDK metadata.
namespace zlg {

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
    virtual HRESULT STDMETHODCALLTYPE CreateFreeThreaded(IInspectable*, INT32, INT32, SizeInt32, ICaptureFramePool**) = 0;
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

}  // namespace zlg

#define ZLG_IID(T) __uuidof(zlg::T)

// ---------------------------------------------------------------------------
// Settings

struct Settings {
    bool enabled;
    int maxFps;
    float blur, refraction, chroma, rim, tint, cornerRadius;
};
Settings g_settings;

void LoadSettings() {
    auto clampi = [](int v, int lo, int hi) { return std::min(hi, std::max(lo, v)); };
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;
    g_settings.maxFps = clampi(Wh_GetIntSetting(L"maxFps"), 5, 60);
    g_settings.blur = (float)clampi(Wh_GetIntSetting(L"blur"), 0, 40);
    g_settings.refraction = (float)clampi(Wh_GetIntSetting(L"refraction"), 0, 60);
    g_settings.chroma = clampi(Wh_GetIntSetting(L"chroma"), 0, 100) / 100.0f;
    g_settings.rim = clampi(Wh_GetIntSetting(L"rim"), 0, 100) / 100.0f;
    g_settings.tint = clampi(Wh_GetIntSetting(L"tint"), 0, 60) / 100.0f;
    g_settings.cornerRadius = (float)clampi(Wh_GetIntSetting(L"cornerRadius"), 0, 64);
}

// ---------------------------------------------------------------------------
// Shader. Rounded-rect SDF drives everything: inside a band near the edge the
// sample point is pushed along the edge normal (refraction), R and B use a
// slightly different push (chromatic aberration), and a light-facing rim adds
// a specular highlight. The blur is a fixed 5x5 gaussian spread by `blur`.

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

// ---------------------------------------------------------------------------
// Renderer state (owned by the worker thread).

struct Glass {
    HWND hwnd = nullptr;
    HWND tray = nullptr;
    RECT trayRect = {};
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
    ComPtr<zlg::ICaptureItem> item;
    ComPtr<zlg::ICaptureFramePool> pool;
    ComPtr<zlg::ICaptureSession> session;
    zlg::SizeInt32 poolSize = {};
    int width = 0, height = 0;
    bool visible = false;
    bool hasContent = false;
};

std::atomic<bool> g_stop{false};
std::atomic<bool> g_deviceLost{false};
HANDLE g_thread = nullptr;
HANDLE g_wakeEvent = nullptr;
DWORD g_trayPrevAffinity = WDA_NONE;
HWND g_trayAffinityWnd = nullptr;

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
    if (FAILED(D3DCompile(kShader, sizeof(kShader) - 1, "zlg", nullptr, nullptr,
                          "vs", "vs_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsb, &err)) ||
        FAILED(D3DCompile(kShader, sizeof(kShader) - 1, "zlg", nullptr, nullptr,
                          "ps", "ps_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psb, &err))) {
        Wh_Log(L"[glass] shader compile failed: %S",
               err ? (const char*)err->GetBufferPointer() : "?");
        return false;
    }
    return SUCCEEDED(g.dev->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &g.vs)) &&
           SUCCEEDED(g.dev->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &g.ps));
}

bool CreateDevice(Glass& g) {
    D3D_FEATURE_LEVEL fl[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                 D3D11_CREATE_DEVICE_BGRA_SUPPORT, fl, ARRAYSIZE(fl),
                                 D3D11_SDK_VERSION, &g.dev, nullptr, &g.ctx))) {
        return false;
    }
    if (!CompileShaders(g)) return false;

    D3D11_BUFFER_DESC bd = {sizeof(ShaderParams), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER};
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

    // WinRT IDirect3DDevice for the capture frame pool.
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
        if (FAILED(factory->CreateSwapChainForComposition(g.dev.Get(), &d, nullptr, &g.swap))) return false;
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
    ComPtr<zlg::IClosable> c;
    if (g.session && SUCCEEDED(g.session.As(&c))) c->Close();
    c.Reset();
    if (g.pool && SUCCEEDED(g.pool.As(&c))) c->Close();
    g.session.Reset();
    g.pool.Reset();
    g.item.Reset();
    g.monitor = nullptr;
}

bool StartCapture(Glass& g, HMONITOR mon) {
    StopCapture(g);
    ComPtr<zlg::ICaptureItemInterop> interop;
    if (FAILED(GetFactory(L"Windows.Graphics.Capture.GraphicsCaptureItem",
                          ZLG_IID(ICaptureItemInterop), interop.GetAddressOf())) ||
        FAILED(interop->CreateForMonitor(mon, ZLG_IID(ICaptureItem), (void**)g.item.GetAddressOf()))) {
        Wh_Log(L"[glass] CreateForMonitor failed");
        return false;
    }
    g.item->get_Size(&g.poolSize);
    ComPtr<zlg::ICaptureFramePoolStatics2> statics;
    if (FAILED(GetFactory(L"Windows.Graphics.Capture.Direct3D11CaptureFramePool",
                          ZLG_IID(ICaptureFramePoolStatics2), statics.GetAddressOf())) ||
        FAILED(statics->CreateFreeThreaded(g.winrtDevice.Get(), zlg::kPixelFormatB8G8R8A8, 2,
                                           g.poolSize, &g.pool)) ||
        FAILED(g.pool->CreateCaptureSession(g.item.Get(), &g.session))) {
        Wh_Log(L"[glass] frame pool / session failed");
        StopCapture(g);
        return false;
    }
    ComPtr<zlg::ICaptureSession2> s2;
    if (SUCCEEDED(g.session.As(&s2))) s2->put_IsCursorCaptureEnabled(false);
    ComPtr<zlg::ICaptureSession3> s3;  // Windows 11; silently ignored if denied.
    if (SUCCEEDED(g.session.As(&s3))) s3->put_IsBorderRequired(false);
    if (FAILED(g.session->StartCapture())) {
        StopCapture(g);
        return false;
    }
    g.monitor = mon;
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    g.monRect = mi.rcMonitor;
    return true;
}

// Copies the area behind the taskbar (plus a margin for blur/refraction) out
// of the latest frame. Returns false when no new frame arrived (idle).
bool PullFrame(Glass& g) {
    ComPtr<zlg::ICaptureFrame> frame;
    if (FAILED(g.pool->TryGetNextFrame(&frame)) || !frame) return false;
    zlg::SizeInt32 cs = {};
    frame->get_ContentSize(&cs);
    if (cs.Width != g.poolSize.Width || cs.Height != g.poolSize.Height) {
        g.poolSize = cs;
        g.pool->Recreate(g.winrtDevice.Get(), zlg::kPixelFormatB8G8R8A8, 2, cs);
        return false;
    }
    ComPtr<IInspectable> surface;
    ComPtr<zlg::IDxgiInterfaceAccess> access;
    ComPtr<ID3D11Texture2D> tex;
    if (FAILED(frame->get_Surface(&surface)) || FAILED(surface.As(&access)) ||
        FAILED(access->GetInterface(IID_PPV_ARGS(&tex)))) {
        return false;
    }

    const int margin = (int)(g_settings.blur + g_settings.refraction) + 4;
    LONG l = std::max<LONG>(0, g.trayRect.left - g.monRect.left - margin);
    LONG t = std::max<LONG>(0, g.trayRect.top - g.monRect.top - margin);
    LONG r = std::min<LONG>(cs.Width, g.trayRect.right - g.monRect.left + margin);
    LONG b = std::min<LONG>(cs.Height, g.trayRect.bottom - g.monRect.top + margin);
    if (r <= l || b <= t) return false;
    UINT w = r - l, h = b - t;

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
    // Our (0,0) inside the copied source region.
    p.srcOrigin[0] = (float)(g.trayRect.left - g.monRect.left - (LONG)g.srcBox.left);
    p.srcOrigin[1] = (float)(g.trayRect.top - g.monRect.top - (LONG)g.srcBox.top);
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

bool ShouldPause(HWND tray) {
    if (!tray || !IsWindowVisible(tray)) return true;
    QUERY_USER_NOTIFICATION_STATE st;
    if (SUCCEEDED(SHQueryUserNotificationState(&st)) &&
        (st == QUNS_BUSY || st == QUNS_RUNNING_D3D_FULL_SCREEN ||
         st == QUNS_PRESENTATION_MODE)) {
        return true;
    }
    return false;
}

void SetVisible(Glass& g, bool v) {
    if (g.visible == v) return;
    g.visible = v;
    ShowWindow(g.hwnd, v ? SW_SHOWNOACTIVATE : SW_HIDE);
}

void ExcludeTrayFromCapture(HWND tray) {
    if (tray == g_trayAffinityWnd) return;
    if (g_trayAffinityWnd && IsWindow(g_trayAffinityWnd)) {
        SetWindowDisplayAffinity(g_trayAffinityWnd, g_trayPrevAffinity);
    }
    g_trayAffinityWnd = nullptr;
    if (!tray) return;
    DWORD prev = WDA_NONE;
    GetWindowDisplayAffinity(tray, &prev);
    if (SetWindowDisplayAffinity(tray, WDA_EXCLUDEFROMCAPTURE)) {
        g_trayPrevAffinity = prev;
        g_trayAffinityWnd = tray;
    } else {
        Wh_Log(L"[glass] could not exclude taskbar from capture (%u)", GetLastError());
    }
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    if (m == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(h, m, w, l);
}

DWORD WINAPI Worker(LPVOID) {
    RoInitialize(RO_INIT_MULTITHREADED);
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"ZenLiquidGlass";
    RegisterClassW(&wc);

    Glass g;
    g.hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
            WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    if (!g.hwnd || !SetWindowDisplayAffinity(g.hwnd, WDA_EXCLUDEFROMCAPTURE) || !CreateDevice(g)) {
        Wh_Log(L"[glass] init failed, staying off");
        if (g.hwnd) DestroyWindow(g.hwnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        RoUninitialize();
        return 0;
    }

    ULONGLONG lastPauseCheck = 0;
    bool paused = true;
    while (!g_stop) {
        const DWORD frameMs = 1000 / g_settings.maxFps;
        DWORD wr = MsgWaitForMultipleObjects(1, &g_wakeEvent, FALSE, frameMs, QS_ALLINPUT);
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (g_stop) break;
        (void)wr;

        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        ULONGLONG now = GetTickCount64();
        if (tray != g.tray || now - lastPauseCheck > 1000) {
            lastPauseCheck = now;
            paused = ShouldPause(tray);
        }
        if (tray != g.tray) {
            g.tray = tray;
            ExcludeTrayFromCapture(tray);
        }
        if (paused) {
            SetVisible(g, false);
            if (g.session) StopCapture(g);  // Releases GPU work while paused.
            continue;
        }

        RECT rc;
        GetWindowRect(tray, &rc);
        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) continue;
        bool moved = !EqualRect(&rc, &g.trayRect);
        g.trayRect = rc;
        HMONITOR mon = MonitorFromWindow(tray, MONITOR_DEFAULTTONEAREST);
        if ((mon != g.monitor || !g.session) && !StartCapture(g, mon)) {
            SetVisible(g, false);
            continue;
        }
        if (moved || !g.visible) {
            // Directly below the taskbar in z-order, both topmost.
            SetWindowPos(g.hwnd, tray, rc.left, rc.top, w, h, SWP_NOACTIVATE);
        }
        if (!EnsureSwapChain(g, w, h)) {
            g_deviceLost = true;
            break;
        }
        // Idle: Graphics.Capture only delivers a frame when the screen changed.
        if (!PullFrame(g) && !moved) continue;
        if (!g.hasContent) continue;

        HRESULT hr = Render(g);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
            Wh_Log(L"[glass] device removed (%08X), disabling for this session",
                   g.dev->GetDeviceRemovedReason());
            g_deviceLost = true;
            break;
        }
        SetVisible(g, true);
    }

    StopCapture(g);
    ExcludeTrayFromCapture(nullptr);
    if (g.target) g.target->SetRoot(nullptr);
    if (g.dcomp) g.dcomp->Commit();
    g = Glass{.hwnd = g.hwnd};
    DestroyWindow(g.hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    RoUninitialize();
    return 0;
}

void Start() {
    if (g_thread || !g_settings.enabled || g_deviceLost) return;
    g_stop = false;
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
}

void Stop() {
    if (!g_thread) return;
    g_stop = true;
    SetEvent(g_wakeEvent);
    WaitForSingleObject(g_thread, 5000);
    CloseHandle(g_thread);
    CloseHandle(g_wakeEvent);
    g_thread = nullptr;
    g_wakeEvent = nullptr;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Start();
    return TRUE;
}

void Wh_ModUninit() { Stop(); }

void Wh_ModSettingsChanged() {
    Stop();
    LoadSettings();
    g_deviceLost = false;  // Saving settings is the user's explicit retry.
    Start();
}
