"""Windhawk 2.0 accepts $options only on string settings.

Integer defaults with $options make the whole settings schema fail to parse, so
the settings page shows 「当前模组无设置」. Option keys are stored as text too.

Saved choices from older versions are registry DWORDs. New choices are decimal
strings. Readers accept a complete decimal string first, then the legacy
integer, so an explicit 0 stays 0 and a missing value stays 0.
"""

from pathlib import Path
import re
import sys

import yaml

root = Path(__file__).resolve().parents[2]
INT32_MIN = -2147483648
INT32_MAX = 2147483647

BLOCK_RE = re.compile(
    r"// ==WindhawkModSettings==\s*/\*\s*(.*?)\s*\*/\s*// ==/WindhawkModSettings==",
    re.S,
)


def read_preset(stored):
    """Mirror of ZenReadPresetInt / ReadPresetInt.

    `stored` is the registry value: an int (old DWORD), a str (new text), or
    None when the value is missing. A non-decimal string falls through to the
    legacy integer read, which is 0 when that read also misses.
    """
    if isinstance(stored, str) and stored != "":
        if re.fullmatch(r"-?\d+", stored):
            value = int(stored)
            if INT32_MIN <= value <= INT32_MAX:
                return value
    if isinstance(stored, int):
        return stored
    return 0


def _walk_item(item, path, hits):
    keys = [k for k in item if not str(k).startswith("$") and k != "#!"]
    if len(keys) != 1:
        return
    key = keys[0]
    value = item[key]
    here = path + [str(key)]
    options = [(name, item[name]) for name in item if str(name).startswith("$options")]
    if isinstance(value, list) and value and all(isinstance(child, dict) for child in value):
        for child in value:
            if not isinstance(child, dict):
                continue
            child_keys = [k for k in child if not str(k).startswith("$") and k != "#!"]
            if len(child_keys) == 1:
                _walk_item(child, here, hits)
            else:
                for child_key in child_keys:
                    sub = {child_key: child[child_key]}
                    sub.update({k: child[k] for k in child if str(k).startswith("$")})
                    _walk_item(sub, here, hits)
        return
    if not options:
        return
    bad_value = not (
        isinstance(value, str)
        or (isinstance(value, list) and value and all(isinstance(part, str) for part in value))
    )
    bad_keys = []
    for name, option_list in options:
        if not isinstance(option_list, list):
            bad_keys.append(f"{name} is not a list")
            continue
        for option in option_list:
            if isinstance(option, dict) and len(option) == 1:
                option_key = next(iter(option))
                if not isinstance(option_key, str):
                    bad_keys.append(f"{name} key {option_key!r} is {type(option_key).__name__}")
    if bad_value or bad_keys:
        hits.append(( ".".join(here), type(value).__name__, value, bad_keys))


def schema_errors(path: Path):
    text = path.read_text(encoding="utf-8-sig")
    blocks = BLOCK_RE.findall(text)
    if not blocks:
        return []
    errors = []
    for block in blocks:
        data = yaml.safe_load(block)
        hits = []
        if isinstance(data, list):
            for item in data:
                if isinstance(item, dict):
                    _walk_item(item, [], hits)
        for setting, kind, value, bad_keys in hits:
            detail = f"value {value!r} ({kind})" if not bad_keys else ", ".join(bad_keys)
            errors.append(f"{path.relative_to(root)}: {setting} cannot use $options: {detail}")
    return errors


def reader_errors():
    """The six mods that used to store these presets as integers must still
    accept that DWORD, not only the new decimal string."""
    expected = {
        "local@zen-taskbar-acrylic.wh.cpp": "ZenReadStoredInt",
        "local@zen-taskbar-lyrics.wh.cpp": "ReadPresetInt",
        "local@zen-startmenu-acrylic.wh.cpp": "ZenReadPresetInt",
        "local@zen-notificationcenter-acrylic.wh.cpp": "ZenReadPresetInt",
        "local@windows-11-settings-styler-fork.wh.cpp": "ZenReadPresetInt",
        "local@zen-ime-caret-hint.wh.cpp": "ReadPresetInt",
    }
    errors = []
    for name, helper in expected.items():
        text = (root / name).read_text(encoding="utf-8-sig")
        match = re.search(
            rf"{helper}\s*\([^)]*\)\s*\{{(.*?)\n\}}",
            text,
            re.S,
        )
        if not match:
            errors.append(f"{name}: missing {helper}")
            continue
        body = match.group(1)
        if "wcstol" not in body or "10" not in body:
            errors.append(f"{name}: {helper} does not parse a decimal string")
        if "Wh_GetIntSetting" not in body and "GetInt(" not in body:
            errors.append(f"{name}: {helper} does not fall back to the legacy integer")
    return errors


def contract_errors():
    cases = [
        (30, 30, "old DWORD"),
        ("30", 30, "new string"),
        ("0", 0, "explicit zero string"),
        (0, 0, "explicit zero DWORD"),
        (None, 0, "missing"),
        ("", 0, "empty string"),
        ("12", 12, "font size string"),
        (24, 24, "maximized blur DWORD"),
        ("72", 72, "maximized opacity string"),
    ]
    errors = []
    for stored, want, label in cases:
        got = read_preset(stored)
        if got != want:
            errors.append(f"read_preset({stored!r}) == {got}, want {want} ({label})")
    return errors


def main():
    errors = []
    paths = sorted(root.rglob("*.wh.cpp"))
    for path in paths:
        errors.extend(schema_errors(path))
    errors.extend(reader_errors())
    errors.extend(contract_errors())
    if errors:
        print("\n".join(errors))
        return 1
    print(f"Windhawk 2.0 $options schema passed: {len(paths)} file(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
