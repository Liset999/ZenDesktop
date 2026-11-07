"""Exercise the actual visual selection function against a clipped DWM tree."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


root = Path(__file__).resolve().parents[2]
source = (root / "optional/local@wobbly-windows.wh.cpp").read_text(encoding="utf-8")
start = source.index("static void* GetTopLevelVisualProxy(")
end = source.index("static void* GetTransitionVisualProxy(", start)
function = source[start:end]
fallback_start = source.index("if (!FindVisualProxyAccessPath(", source.index("static bool InitializeDwmHooks("))
fallback_end = source.index("g_transitionVisualProxyOffset =", fallback_start)
fallback = source[fallback_start:fallback_end]
assert "return false" not in fallback, "Unknown accessor layout must allow typed fallback initialization"
assert "g_visualProxyOffset = SIZE_MAX;" in fallback, "Fallback must discard unproven offsets"
compiler = os.environ.get("CXX") or shutil.which("clang++")
if not compiler:
    candidate = Path("E:/Tools/Windhawk/Compiler/bin/clang++.exe")
    if candidate.exists():
        compiler = str(candidate)
if not compiler:
    raise SystemExit("Set CXX to a C++ compiler")

harness = r'''
#include <cstddef>
#include <cstdio>
#include <cwchar>
#include <cstdint>
constexpr size_t SIZE_MAX_VALUE = static_cast<size_t>(-1);
struct Visual { void* tag; void* proxy; };
int outerProxy, borderProxy, canvasProxy, topVtable;
Visual outer{&topVtable, &outerProxy};
Visual border{nullptr, &borderProxy}, canvas{nullptr, &canvasProxy};
int g_topLevelWindowVtable;
size_t g_visualProxyOffset = offsetof(Visual, proxy);
size_t g_canvasVisualOwnerOffset = SIZE_MAX_VALUE;
bool IsDwmObjectPointerValid(void* object, int&) { return object == &outer; }
bool IsVisualProxyPointerValid(void* proxy) {
    return proxy == &outerProxy || proxy == &borderProxy || proxy == &canvasProxy;
}
void* ReadPointerMember(void* object, size_t offset) {
    if (!object || offset == SIZE_MAX_VALUE) return nullptr;
    return *reinterpret_cast<void**>(static_cast<char*>(object) + offset);
}
void* RootGetter(void*, int) { return &border; }
int canvasCalls = 0;
void* canvasResult = &canvasProxy;
void* CanvasGetter(void*) { ++canvasCalls; return canvasResult; }
auto g_topLevelWindowGetRootVisual = RootGetter;
auto g_getCanvasRootVisualProxy = CanvasGetter;
'''
tests = r'''
int main() {
    const wchar_t* name = nullptr;
    if (GetTopLevelVisualProxy(&outer, &name) != &outerProxy) {
        std::puts("FAIL: transform selected an inner, clipped visual"); return 1;
    }
    if (GetTopLevelVisualProxy(nullptr) != nullptr) return 2;
    if (GetTopLevelVisualProxy(&border) != nullptr) return 3;
    outer.proxy = nullptr;
    if (GetTopLevelVisualProxy(&outer) != nullptr) return 4;
    outer.proxy = &border;
    if (GetTopLevelVisualProxy(&outer) != nullptr) return 5;
    outer.proxy = &outerProxy;
    if (canvasCalls != 0) return 6;
    g_visualProxyOffset = SIZE_MAX_VALUE;
    if (GetTopLevelVisualProxy(&outer, &name) != &canvasProxy ||
        std::wcscmp(name, L"TypedCanvasAccessor") != 0 || canvasCalls != 1) {
        std::puts("FAIL: unavailable offset did not use typed compatibility accessor"); return 7;
    }
    canvasResult = &border;
    if (GetTopLevelVisualProxy(&outer, &name) != nullptr ||
        std::wcscmp(name, L"None") != 0) return 8;
    canvasResult = nullptr;
    if (GetTopLevelVisualProxy(&outer) != nullptr) return 9;
    g_getCanvasRootVisualProxy = nullptr;
    if (GetTopLevelVisualProxy(&outer) != nullptr) return 10;
    std::puts("PASS: outer root preferred; unknown offset uses typed fallback; invalid objects rejected");
}
'''
with tempfile.TemporaryDirectory(prefix="wobbly-root-") as tmp:
    cpp = Path(tmp) / "test.cpp"
    binary = Path(tmp) / ("test.exe" if os.name == "nt" else "test")
    cpp.write_text(harness + function + tests, encoding="utf-8")
    subprocess.run([compiler, "-std=c++17", str(cpp), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
