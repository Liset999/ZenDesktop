/*
 * zen-status.exe - tiny, CRT-free writer for the ZenDesktop taskbar AI status
 * (%LOCALAPPDATA%\ZenDesktop\ai-status\<agent>.json, read by the zen-taskbar-lyrics Windhawk mod).
 * Same JSON schema and merge rules as zen-ai-status-lib.ps1, but starts in a few milliseconds,
 * so it can be used from synchronous hooks without slowing the agent down.
 *
 * Usage:
 *   zen-status <agent> <state> [message] [--name X] [--detail X] [--project X] [--weak] [--clear]
 *       state: working | tool | waiting | done | error | idle   (idle = hidden)
 *   zen-status hook <agent> [--event NAME] [--name X]
 *       Reads one hook JSON object from stdin (Claude Code, Codex, Antigravity) and maps the event to
 *       a state. Without --event the event is taken from "hook_event_name" (Claude Code / Codex).
 *       With --event (Antigravity, whose payload has no event name) a valid JSON answer is printed:
 *       PreToolUse -> {"decision":"allow"}, Stop -> {"decision":"stop"}, others -> {}.
 *       Never blocks or changes what the agent does; always exits 0.
 *
 * Build (no CRT, kernel32 + shell32 only):
 *   x86_64-w64-mingw32-gcc -Os -s -nostdlib -ffreestanding -fno-builtin -fno-stack-protector
 *       -mno-stack-arg-probe -fno-asynchronous-unwind-tables -e zs_entry -Wl,--subsystem,console
 *       zen-status.c -lkernel32 -lshell32 -o zen-status.exe
 *   (see build.sh / build.cmd)
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

/* ---- freestanding helpers (the compiler may emit calls to these) ---- */
void* memset(void* d, int c, size_t n) { unsigned char* p = (unsigned char*)d; while (n--) *p++ = (unsigned char)c; return d; }
void* memcpy(void* d, const void* s, size_t n) { unsigned char* a = (unsigned char*)d; const unsigned char* b = (const unsigned char*)s; while (n--) *a++ = *b++; return d; }
void* memmove(void* d, const void* s, size_t n) {
    unsigned char* a = (unsigned char*)d; const unsigned char* b = (const unsigned char*)s;
    if (a < b) { while (n--) *a++ = *b++; } else { a += n; b += n; while (n--) *--a = *--b; }
    return d;
}

static HANDLE g_heap;
static void* zalloc(size_t n) { return HeapAlloc(g_heap, HEAP_ZERO_MEMORY, n ? n : 1); }
static void zfree(void* p) { if (p) HeapFree(g_heap, 0, p); }
static size_t slen(const char* s) { size_t n = 0; if (s) while (s[n]) n++; return n; }
static int seq(const char* a, const char* b) {
    if (!a || !b) return 0;
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}
static int sstarts(const char* s, const char* p) { if (!s) return 0; while (*p) if (*s++ != *p++) return 0; return 1; }
static int scontains(const char* s, const char* p) {
    if (!s) return 0;
    for (; *s; s++) if (sstarts(s, p)) return 1;
    return 0;
}
static int memcmp_eq(const char* a, const char* b, size_t n) { while (n--) if (*a++ != *b++) return 0; return 1; }
static char* sdup(const char* s, size_t n) { char* r = (char*)zalloc(n + 1); if (s) memcpy(r, s, n); r[n] = 0; return r; }

/* ---- growable byte buffer ---- */
typedef struct { char* p; size_t n, cap; } Buf;
static void bput(Buf* b, const char* s, size_t n) {
    if (b->n + n + 1 > b->cap) {
        size_t c = b->cap ? b->cap * 2 : 256;
        while (c < b->n + n + 1) c *= 2;
        char* q = (char*)zalloc(c);
        if (b->p) { memcpy(q, b->p, b->n); zfree(b->p); }
        b->p = q; b->cap = c;
    }
    memcpy(b->p + b->n, s, n); b->n += n; b->p[b->n] = 0;
}
static void bputs(Buf* b, const char* s) { bput(b, s, slen(s)); }
static void bputu64(Buf* b, unsigned long long v) {
    char t[24]; int i = 23; t[i] = 0;
    do { t[--i] = (char)('0' + v % 10); v /= 10; } while (v);
    bputs(b, t + i);
}
static void bputjstr(Buf* b, const char* s) {
    static const char hex[] = "0123456789abcdef";
    bput(b, "\"", 1);
    for (; s && *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c == '"') bput(b, "\\\"", 2);
        else if (c == '\\') bput(b, "\\\\", 2);
        else if (c == '\n') bput(b, "\\n", 2);
        else if (c == '\r') bput(b, "\\r", 2);
        else if (c == '\t') bput(b, "\\t", 2);
        else if (c < 0x20) { char e[6] = {'\\', 'u', '0', '0', hex[c >> 4], hex[c & 15]}; bput(b, e, 6); }
        else bput(b, (const char*)&c, 1);
    }
    bput(b, "\"", 1);
}

/* ---- UTF-8 / UTF-16 ---- */
static char* to_utf8(const WCHAR* w) {
    if (!w) return sdup("", 0);
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
    char* r = (char*)zalloc(n > 0 ? (size_t)n : 1);
    if (n > 0) WideCharToMultiByte(CP_UTF8, 0, w, -1, r, n, NULL, NULL);
    return r;
}
/* Cut to at most max UTF-16 units (same limit the PowerShell writer uses), on a code point boundary. */
static void utf8_truncate(char* s, int max) {
    int units = 0; size_t i = 0;
    while (s[i]) {
        unsigned char c = (unsigned char)s[i];
        size_t len = c < 0x80 ? 1 : (c >> 5) == 6 ? 2 : (c >> 4) == 14 ? 3 : (c >> 3) == 30 ? 4 : 1;
        int u = len == 4 ? 2 : 1;
        if (units + u > max) { s[i] = 0; return; }
        units += u; i += len;
        for (size_t k = 1; k < len; k++) if (!s[i - len + k]) return;
    }
}

/* ---- minimal JSON reader: find values by path in one object ---- */
typedef struct { const char* p; const char* e; } JCur;
static void jws(JCur* c) { while (c->p < c->e && (*c->p == ' ' || *c->p == '\t' || *c->p == '\r' || *c->p == '\n')) c->p++; }
static int hexv(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}
static void put_cp(Buf* b, unsigned cp) {
    char t[4];
    if (cp < 0x80) { t[0] = (char)cp; bput(b, t, 1); }
    else if (cp < 0x800) { t[0] = (char)(0xC0 | (cp >> 6)); t[1] = (char)(0x80 | (cp & 63)); bput(b, t, 2); }
    else if (cp < 0x10000) { t[0] = (char)(0xE0 | (cp >> 12)); t[1] = (char)(0x80 | ((cp >> 6) & 63)); t[2] = (char)(0x80 | (cp & 63)); bput(b, t, 3); }
    else { t[0] = (char)(0xF0 | (cp >> 18)); t[1] = (char)(0x80 | ((cp >> 12) & 63)); t[2] = (char)(0x80 | ((cp >> 6) & 63)); t[3] = (char)(0x80 | (cp & 63)); bput(b, t, 4); }
}
static int read_hex4(JCur* c, unsigned* v) {
    if (c->e - c->p < 4) return 0;
    unsigned r = 0;
    for (int i = 0; i < 4; i++) { int h = hexv(c->p[i]); if (h < 0) return 0; r = r * 16 + (unsigned)h; }
    c->p += 4; *v = r; return 1;
}
/* Parses a string at c->p (must be '"'). out may be NULL to just skip. */
static int jstr(JCur* c, Buf* out) {
    if (c->p >= c->e || *c->p != '"') return 0;
    c->p++;
    while (c->p < c->e) {
        char ch = *c->p++;
        if (ch == '"') return 1;
        if (ch != '\\') { if (out) bput(out, &ch, 1); continue; }
        if (c->p >= c->e) return 0;
        ch = *c->p++;
        switch (ch) {
            case 'n': if (out) bput(out, "\n", 1); break;
            case 't': if (out) bput(out, "\t", 1); break;
            case 'r': if (out) bput(out, "\r", 1); break;
            case 'b': if (out) bput(out, "\b", 1); break;
            case 'f': if (out) bput(out, "\f", 1); break;
            case 'u': {
                unsigned v;
                if (!read_hex4(c, &v)) return 0;
                if (v >= 0xD800 && v < 0xDC00 && c->e - c->p >= 6 && c->p[0] == '\\' && c->p[1] == 'u') {
                    JCur t = { c->p + 2, c->e }; unsigned lo;
                    if (read_hex4(&t, &lo) && lo >= 0xDC00 && lo < 0xE000) { c->p = t.p; v = 0x10000 + ((v - 0xD800) << 10) + (lo - 0xDC00); }
                }
                if (out) put_cp(out, v);
                break;
            }
            default: if (out) bput(out, &ch, 1); break;
        }
    }
    return 0;
}
static int jskip(JCur* c, int depth) {
    jws(c);
    if (c->p >= c->e || depth > 64) return 0;
    char ch = *c->p;
    if (ch == '"') return jstr(c, NULL);
    if (ch == '{' || ch == '[') {
        char close = ch == '{' ? '}' : ']';
        c->p++; jws(c);
        if (c->p < c->e && *c->p == close) { c->p++; return 1; }
        for (;;) {
            if (ch == '{') { jws(c); if (!jstr(c, NULL)) return 0; jws(c); if (c->p >= c->e || *c->p != ':') return 0; c->p++; }
            if (!jskip(c, depth + 1)) return 0;
            jws(c);
            if (c->p >= c->e) return 0;
            if (*c->p == ',') { c->p++; continue; }
            if (*c->p == close) { c->p++; return 1; }
            return 0;
        }
    }
    while (c->p < c->e && *c->p != ',' && *c->p != '}' && *c->p != ']' && *c->p != ' ' && *c->p != '\r' && *c->p != '\n' && *c->p != '\t') c->p++;
    return 1;
}
/* Look up a value: path is like "toolCall.name" or "workspacePaths.0". On success *v points at the value. */
static int jfind(const char* json, size_t n, const char* path, JCur* v) {
    JCur c = { json, json + n };
    const char* seg = path;
    for (;;) {
        const char* dot = seg; while (*dot && *dot != '.') dot++;
        size_t sl = (size_t)(dot - seg);
        jws(&c);
        if (c.p >= c.e) return 0;
        int found = 0;
        if (*c.p == '{') {
            c.p++;
            for (;;) {
                jws(&c);
                if (c.p < c.e && *c.p == '}') return 0;
                Buf key = {0};
                if (!jstr(&c, &key)) { zfree(key.p); return 0; }
                jws(&c);
                if (c.p >= c.e || *c.p != ':') { zfree(key.p); return 0; }
                c.p++; jws(&c);
                int match = key.n == sl && (sl == 0 || memcmp_eq(key.p, seg, sl));
                zfree(key.p);
                if (match) { found = 1; break; }
                if (!jskip(&c, 0)) return 0;
                jws(&c);
                if (c.p < c.e && *c.p == ',') { c.p++; continue; }
                return 0;
            }
        } else if (*c.p == '[') {
            int idx = 0;
            for (size_t i = 0; i < sl; i++) { if (seg[i] < '0' || seg[i] > '9') return 0; idx = idx * 10 + (seg[i] - '0'); }
            c.p++;
            for (int i = 0;; i++) {
                jws(&c);
                if (c.p < c.e && *c.p == ']') return 0;
                if (i == idx) { found = 1; break; }
                if (!jskip(&c, 0)) return 0;
                jws(&c);
                if (c.p < c.e && *c.p == ',') { c.p++; continue; }
                return 0;
            }
        }
        if (!found) return 0;
        if (!*dot) { *v = c; return 1; }
        seg = dot + 1;
    }
}
/* String value at path (unescaped, UTF-8), or NULL if missing / not a string. Caller frees. */
static char* jget_str(const char* json, size_t n, const char* path) {
    JCur v;
    if (!json || !jfind(json, n, path, &v) || v.p >= v.e || *v.p != '"') return NULL;
    Buf b = {0};
    if (!jstr(&v, &b)) { zfree(b.p); return NULL; }
    if (!b.p) return sdup("", 0);
    return b.p;
}
/* Integer (or -1), and boolean (1/0, -1 if missing). */
static long long jget_int(const char* json, size_t n, const char* path) {
    JCur v;
    if (!json || !jfind(json, n, path, &v)) return -1;
    int neg = 0; long long r = 0, any = 0;
    if (v.p < v.e && *v.p == '-') { neg = 1; v.p++; }
    while (v.p < v.e && *v.p >= '0' && *v.p <= '9') { r = r * 10 + (*v.p++ - '0'); any = 1; }
    if (!any) return -1;
    return neg ? -r : r;
}
static int jget_bool(const char* json, size_t n, const char* path) {
    JCur v;
    if (!json || !jfind(json, n, path, &v) || v.p >= v.e) return -1;
    if (*v.p == 't') return 1;
    if (*v.p == 'f') return 0;
    return -1;
}

/* ---- files ---- */
static char* read_file(const WCHAR* path, size_t* n) {
    *n = 0;
    HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
    if (f == INVALID_HANDLE_VALUE) return NULL;
    LARGE_INTEGER sz;
    char* r = NULL;
    if (GetFileSizeEx(f, &sz) && sz.QuadPart > 0 && sz.QuadPart <= 64 * 1024) {
        r = (char*)zalloc((size_t)sz.QuadPart + 1);
        DWORD got = 0;
        if (!ReadFile(f, r, (DWORD)sz.QuadPart, &got, NULL)) got = 0;
        *n = got;
        if (got >= 3 && (unsigned char)r[0] == 0xEF && (unsigned char)r[1] == 0xBB && (unsigned char)r[2] == 0xBF) { memmove(r, r + 3, got - 3); *n = got - 3; r[*n] = 0; }
    }
    CloseHandle(f);
    return r;
}
static char* read_stdin(size_t* n) {
    Buf b = {0};
    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    *n = 0;
    if (in == NULL || in == INVALID_HANDLE_VALUE) return NULL;
    if (GetFileType(in) == FILE_TYPE_CHAR) return NULL;  /* interactive console: nothing piped */
    char* chunk = (char*)zalloc(4096);
    for (;;) {
        DWORD got = 0;
        if (!ReadFile(in, chunk, 4096, &got, NULL) || got == 0) break;
        bput(&b, chunk, got);
        if (b.n > 4 * 1024 * 1024) break;
    }
    zfree(chunk);
    *n = b.n;
    return b.p;
}
static void wcat(WCHAR* d, const WCHAR* s) { while (*d) d++; while ((*d++ = *s++)) {} }

static unsigned long long unix_now(void) {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    unsigned long long t = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (t - 116444736000000000ULL) / 10000000ULL;
}

/* ---- the writer (mirrors Write-ZenStatus in zen-ai-status-lib.ps1) ---- */
typedef struct {
    const char* agent;    /* required */
    const char* state;    /* required */
    const char* message;  /* NULL = keep the old one (sticky, set by MCP) */
    const char* detail;   /* NULL = "" */
    const char* project;  /* NULL = keep old */
    const char* name;     /* NULL/"" = keep old */
    int weak;             /* async tool hook: don't overwrite a done/error/idle from the last 5 s */
    int clear;            /* clear the message */
    int clearIfFinished;  /* clear the message only if the previous state was done/error/idle (new turn) */
} Status;

static int valid_state(const char* s) {
    return seq(s, "working") || seq(s, "tool") || seq(s, "waiting") || seq(s, "done") || seq(s, "error") || seq(s, "idle");
}
static int finished_state(const char* s) { return seq(s, "done") || seq(s, "error") || seq(s, "idle"); }

static int write_status(const Status* st) {
    WCHAR* dir = (WCHAR*)zalloc(sizeof(WCHAR) * 2048);
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", dir, 1500);
    if (n == 0 || n >= 1500) {
        n = GetEnvironmentVariableW(L"USERPROFILE", dir, 1500);
        if (n == 0 || n >= 1500) { zfree(dir); return 0; }
        wcat(dir, L"\\AppData\\Local");
    }
    wcat(dir, L"\\ZenDesktop");
    CreateDirectoryW(dir, NULL);
    wcat(dir, L"\\ai-status");
    CreateDirectoryW(dir, NULL);

    /* file name: agent with anything outside [A-Za-z0-9_.-] replaced by '_' */
    WCHAR* path = (WCHAR*)zalloc(sizeof(WCHAR) * 2400);
    wcat(path, dir); wcat(path, L"\\");
    {
        WCHAR* d = path; while (*d) d++;
        int k = 0;
        for (const char* a = st->agent; *a && k < 200; a++, k++) {
            char c = *a;
            int ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '-';
            *d++ = ok ? (WCHAR)c : L'_';
        }
        *d = 0;
        wcat(path, L".json");
    }

    size_t on = 0;
    char* old = read_file(path, &on);
    char* oldState = old ? jget_str(old, on, "state") : NULL;
    unsigned long long now = unix_now();
    int result = 1;

    if (st->weak && oldState && finished_state(oldState)) {
        long long upd = jget_int(old, on, "updated");
        long long age = (long long)now - upd;
        if (upd > 0 && age >= 0 && age <= 5) goto done;
    }

    char* msg;
    if (st->message) msg = sdup(st->message, slen(st->message));
    else if (st->clear || !old || (st->clearIfFinished && (!oldState || finished_state(oldState)))) msg = sdup("", 0);
    else { msg = jget_str(old, on, "message"); if (!msg) msg = sdup("", 0); }
    char* proj;
    if (st->project) proj = sdup(st->project, slen(st->project));
    else { proj = old ? jget_str(old, on, "project") : NULL; if (!proj) proj = sdup("", 0); }
    char* name;
    if (st->name && *st->name) name = sdup(st->name, slen(st->name));
    else { name = old ? jget_str(old, on, "name") : NULL; if (!name) name = sdup("", 0); }
    char* detail = sdup(st->detail ? st->detail : "", slen(st->detail));
    utf8_truncate(msg, 120);
    utf8_truncate(detail, 60);

    Buf j = {0};
    bputs(&j, "{\"agent\":"); bputjstr(&j, st->agent);
    bputs(&j, ",\"name\":"); bputjstr(&j, name);
    bputs(&j, ",\"state\":"); bputjstr(&j, st->state);
    bputs(&j, ",\"message\":"); bputjstr(&j, msg);
    bputs(&j, ",\"detail\":"); bputjstr(&j, detail);
    bputs(&j, ",\"project\":"); bputjstr(&j, proj);
    bputs(&j, ",\"updated\":"); bputu64(&j, now);
    bputs(&j, "}");

    /* atomic: write <path>.<pid>.tmp, then MoveFileEx(REPLACE_EXISTING) */
    WCHAR* tmp = (WCHAR*)zalloc(sizeof(WCHAR) * 2500);
    wcat(tmp, path);
    {
        WCHAR num[16]; int i = 15; num[i] = 0;
        DWORD pid = GetCurrentProcessId();
        do { num[--i] = (WCHAR)(L'0' + pid % 10); pid /= 10; } while (pid);
        wcat(tmp, L"."); wcat(tmp, num + i); wcat(tmp, L".tmp");
    }
    result = 0;
    HANDLE f = CreateFileW(tmp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f != INVALID_HANDLE_VALUE) {
        DWORD w = 0;
        BOOL ok = WriteFile(f, j.p, (DWORD)j.n, &w, NULL) && w == j.n;
        CloseHandle(f);
        if (ok) {
            for (int i = 0; i < 5 && !result; i++) {
                if (MoveFileExW(tmp, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) result = 1;
                else Sleep(40);
            }
            if (!result && CopyFileW(tmp, path, FALSE)) result = 1;  /* last resort, not atomic */
        }
        if (!result) DeleteFileW(tmp);
    }
    zfree(tmp); zfree(j.p); zfree(msg); zfree(proj); zfree(name); zfree(detail);
done:
    zfree(oldState); zfree(old); zfree(path); zfree(dir);
    return result;
}

/* ---- hook mode ---- */
static char* leaf(const char* p) {
    if (!p) return sdup("", 0);
    size_t n = slen(p);
    while (n > 0 && (p[n - 1] == '\\' || p[n - 1] == '/')) n--;
    size_t a = n;
    while (a > 0 && p[a - 1] != '\\' && p[a - 1] != '/') a--;
    return sdup(p + a, n - a);
}
static int own_tool(const char* t) {
    return t && (sstarts(t, "mcp__zen-status__") || scontains(t, "zen-status") || scontains(t, "zen_status") || seq(t, "set_status"));
}
static void write_out(const char* s) {
    HANDLE o = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD w;
    if (o && o != INVALID_HANDLE_VALUE) WriteFile(o, s, (DWORD)slen(s), &w, NULL);
}

static void run_hook(const char* agent, const char* eventArg, const char* name) {
    size_t n = 0;
    char* in = read_stdin(&n);
    char* ev = eventArg ? sdup(eventArg, slen(eventArg)) : jget_str(in, n, "hook_event_name");
    if (!ev) goto out;

    char* cwd = jget_str(in, n, "cwd");
    if (!cwd) cwd = jget_str(in, n, "workspacePaths.0");  /* Antigravity */
    char* proj = leaf(cwd);
    char* tool = jget_str(in, n, "tool_name");
    if (!tool) tool = jget_str(in, n, "toolCall.name");   /* Antigravity */

    Status s = {0};
    s.agent = agent; s.name = name; s.project = *proj ? proj : NULL;  /* no cwd in this event: keep the old one */
    int write = 1;
    if (own_tool(tool)) write = 0;  /* our own MCP call sets the state itself */
    else if (seq(ev, "SessionStart")) { s.state = "idle"; s.clear = 1; }
    else if (seq(ev, "UserPromptSubmit")) { s.state = "working"; s.clear = 1; }
    else if (seq(ev, "PreInvocation")) { s.state = "working"; s.clearIfFinished = 1; }
    else if (seq(ev, "PreToolUse")) { s.state = "tool"; s.detail = tool; s.weak = 1; }
    else if (seq(ev, "PostToolUse") || seq(ev, "PostToolUseFailure")) { s.state = "working"; s.weak = 1; }
    else if (seq(ev, "PermissionRequest")) { s.state = "waiting"; s.detail = tool; }
    else if (seq(ev, "Notification")) {
        char* t = jget_str(in, n, "notification_type");
        if (seq(t, "permission_prompt") || sstarts(t, "elicitation") || seq(t, "agent_needs_input")) s.state = "waiting";
        else write = 0;
        zfree(t);
    }
    else if (seq(ev, "Stop")) {
        char* reason = jget_str(in, n, "terminationReason");  /* Antigravity */
        char* err = jget_str(in, n, "error");
        if (seq(reason, "error")) { s.state = "error"; s.detail = err; }
        else if (jget_bool(in, n, "fullyIdle") == 0) s.state = "working";  /* background tasks still running */
        else s.state = "done";
        if (write) write_status(&s);
        write = 0;
        zfree(reason); zfree(err);
    }
    else if (seq(ev, "StopFailure")) {
        char* et = jget_str(in, n, "error_type");
        s.state = "error"; s.detail = et;
        write_status(&s);
        write = 0;
        zfree(et);
    }
    else if (seq(ev, "Interrupt") || seq(ev, "SessionEnd")) s.state = "idle";
    else write = 0;  /* PostInvocation and anything unknown */
    if (write) write_status(&s);
    zfree(cwd); zfree(proj); zfree(tool);
out:
    /* Antigravity reads a JSON answer from every hook; Claude Code / Codex get nothing. */
    if (eventArg) {
        if (seq(eventArg, "PreToolUse")) write_out("{\"decision\":\"allow\"}\n");
        else if (seq(eventArg, "Stop")) write_out("{\"decision\":\"stop\"}\n");
        else write_out("{}\n");
    }
    zfree(ev); zfree(in);
}

static void usage(void) {
    HANDLE e = GetStdHandle(STD_ERROR_HANDLE);
    const char* u =
        "zen-status - ZenDesktop taskbar AI status\n"
        "  zen-status <agent> <state> [message] [--name X] [--detail X] [--project X] [--weak] [--clear]\n"
        "      state: working | tool | waiting | done | error | idle\n"
        "  zen-status hook <agent> [--event NAME] [--name X]   (hook JSON on stdin)\n";
    DWORD w;
    if (e && e != INVALID_HANDLE_VALUE) WriteFile(e, u, (DWORD)slen(u), &w, NULL);
}

int zs_main(void) {
    g_heap = GetProcessHeap();
    int argc = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wargv) return 0;
    char** argv = (char**)zalloc(sizeof(char*) * (size_t)(argc + 1));
    for (int i = 0; i < argc; i++) argv[i] = to_utf8(wargv[i]);
    LocalFree(wargv);

    /* split positionals and --options */
    const char* pos[4] = {0}; int np = 0;
    const char *optName = NULL, *optDetail = NULL, *optProject = NULL, *optEvent = NULL;
    int weak = 0, clear = 0;
    for (int i = 1; i < argc; i++) {
        const char* a = argv[i];
        if (seq(a, "--name") && i + 1 < argc) optName = argv[++i];
        else if (seq(a, "--detail") && i + 1 < argc) optDetail = argv[++i];
        else if (seq(a, "--project") && i + 1 < argc) optProject = argv[++i];
        else if (seq(a, "--event") && i + 1 < argc) optEvent = argv[++i];
        else if (seq(a, "--weak")) weak = 1;
        else if (seq(a, "--clear")) clear = 1;
        else if (seq(a, "--help") || seq(a, "-h") || seq(a, "/?")) { usage(); return 0; }
        else if (np < 4) pos[np++] = a;
    }

    if (np >= 2 && seq(pos[0], "hook")) {
        run_hook(pos[1], optEvent, optName);
        return 0;  /* hooks never fail */
    }
    if (np < 2 || !valid_state(pos[1])) { usage(); return 2; }
    Status s = {0};
    s.agent = pos[0]; s.state = pos[1];
    s.message = np >= 3 ? pos[2] : NULL;
    s.detail = optDetail; s.project = optProject; s.name = optName;
    s.weak = weak; s.clear = clear;
    return write_status(&s) ? 0 : 1;
}

void zs_entry(void) { ExitProcess((UINT)zs_main()); }
