"""Exercise the weather worker's actual scheduling expression with a fake clock."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--compiler-dir', required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = (root / 'local@zen-taskbar-lyrics.wh.cpp').read_text(encoding='utf-8-sig')
start = source.index('        const bool needWeather =')
end = source.index('        if (needWeather)', start)
schedule = source[start:end]
completion = source.index('lastWeather = GetTickCount64();', end)
assert completion > source.index('w = FetchWeatherAt(locCache);', end)
assert 'weatherFailed = w.empty();' in source[completion:completion + 120]
assert 'locCache.lat.empty()) {' in source[source.index('static void ExtrasThread()'):end + 500]
code = r'''
#include <cstdio>
#include <string>
using ULONGLONG = unsigned long long;
enum class IdleMode { Message, Weather };
struct Settings {
    bool xWeather = true;
    IdleMode idleMode = IdleMode::Message;
    int weatherRefreshMin = 30;
};
static bool Due(const Settings& s, ULONGLONG now, ULONGLONG lastWeather,
                bool weatherFailed, std::wstring wantedKey = L"auto",
                std::wstring lastWantedKey = L"auto") {
''' + schedule + r'''
    return needWeather;
}
int main() {
    Settings s;
    int failures = 0, checks = 0;
    auto check = [&](bool ok) { ++checks; if (!ok) ++failures; };
    check(Due(s, 1000, 0, false));
    for (int refresh : {5, 30, 1440}) {
        s.weatherRefreshMin = refresh;
        check(!Due(s, 60999, 1000, true));
        check(Due(s, 61000, 1000, true));
        check(Due(s, 62000, 1000, true));
        // A second failure restarts the minute, including request duration.
        check(!Due(s, 123999, 64000, true));
        check(Due(s, 124000, 64000, true));
        // Recovery returns to the configured interval.
        check(!Due(s, 184000, 124000, false));
        check(!Due(s, 124000 + refresh * 60000ULL - 1, 124000, false));
        check(Due(s, 124000 + refresh * 60000ULL, 124000, false));
    }
    check(Due(s, 1001, 1000, true, L"new city"));
    s.xWeather = false;
    check(!Due(s, 61000, 1000, true));
    s.idleMode = IdleMode::Weather;
    check(Due(s, 61000, 1000, true));
    std::printf("Weather retry: %d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
'''
with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / 'weather_retry.cpp'
    exe = Path(directory) / 'weather_retry.exe'
    cpp.write_text(code, encoding='utf-8')
    subprocess.run([str(args.compiler_dir / 'bin/clang++.exe'), '-std=c++23',
                    '-static', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
