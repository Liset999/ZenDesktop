# Windhawk Mod Development Rules

## 1. Win32 Callback Calling Conventions
When writing or modifying Windhawk C++ mods, NEVER pass inline lambdas directly to Win32 callback APIs (e.g., `EnumWindows`, `EnumThreadWindows`, `SetWindowsHookEx`).
- **Constraint**: Always define callbacks as static functions decorated with `CALLBACK` or `WINAPI` (which resolves to `__stdcall`).
- **Context passing**: Use parameter structs (`LPARAM`) to pass local context variables to the static callback function.

## 2. MinGW Header & SDK Compatibility
Do not rely on `__cplusplus` version checks to determine if modern Win32 SDK declarations (like `CO_MTA_USAGE_COOKIE`) exist in the environment.
- **Constraint**: Check `#if defined(__MINGW32__)` to inject missing SDK types, handles, and function prototypes when compiling under Windhawk's GCC/Clang tooling.
