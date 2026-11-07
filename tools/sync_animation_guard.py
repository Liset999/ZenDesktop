from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
header = root / 'optional/zen-game-guard.h'
source = root / 'optional/local@windows-animations.wh.cpp'
body = re.sub(r'^#pragma once\s*\n', '', header.read_text(encoding='utf-8-sig'), flags=re.M).rstrip()
block = '// BEGIN embedded zen-game-guard.h\n// Windhawk compiles stdin; keep this block synced with tools/sync_animation_guard.py.\n' + body + '\n// END embedded zen-game-guard.h'
text = source.read_text(encoding='utf-8-sig')
pattern = r'// BEGIN embedded zen-game-guard\.h.*?// END embedded zen-game-guard\.h|#include "zen-game-guard\.h"'
updated, count = re.subn(pattern, lambda _: block, text, flags=re.S)
if count != 1:
    raise SystemExit(f'Expected one guard include/block, got {count}; no changes made')
if updated != text:
    source.write_text(updated, encoding='utf-8')
print('Animation game guard synchronized')
