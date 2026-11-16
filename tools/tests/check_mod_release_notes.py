"""Validate that every packaged Mod records its current version in its own readme."""

from __future__ import annotations

import re
import sys
from pathlib import Path


DEFAULT_SOURCES = sorted(
    [*Path(".").glob("local@*.wh.cpp"), *Path("optional").glob("local@*.wh.cpp")]
)

VERSION_RE = re.compile(r"^//\s*@version\s+([^\s]+)\s*$", re.MULTILINE)
README_RE = re.compile(
    r"//\s*==WindhawkModReadme==\s*\n\s*/\*(.*?)\*/\s*"
    r"//\s*==/WindhawkModReadme==",
    re.DOTALL,
)


def validate_source(path: Path) -> list[str]:
    errors: list[str] = []
    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        return [f"{path}: cannot read source: {exc}"]

    version_match = VERSION_RE.search(text)
    if not version_match:
        errors.append(f"{path}: missing // @version metadata")
        return errors
    version = version_match.group(1)

    readme_match = README_RE.search(text)
    if not readme_match:
        errors.append(f"{path}: missing WindhawkModReadme block")
        return errors

    readme = readme_match.group(1)
    entry_re = re.compile(
        rf"(?m)^\s*[-*]\s+\*\*{re.escape(version)}\*\*\s*[：:]"
    )
    if not entry_re.search(readme):
        errors.append(
            f"{path}: @version {version} is missing from its WindhawkModReadme "
            "update log"
        )
    return errors


def main() -> int:
    sources = [Path(arg) for arg in sys.argv[1:]] or DEFAULT_SOURCES
    errors: list[str] = []
    for source in sources:
        errors.extend(validate_source(source))

    if errors:
        print("Mod release-note validation failed:")
        for error in errors:
            print(f"- {error}")
        return 1

    print(f"Mod release-note validation passed: {len(sources)} source(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
