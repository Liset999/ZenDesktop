from pathlib import Path
import re
import sys

root = Path(__file__).resolve().parents[2]
paths = [Path(p) for p in sys.argv[1:]] or sorted(root.glob('*.wh.cpp')) + sorted((root / 'optional').glob('*.wh.cpp'))
errors = []
for path in paths:
    if path.read_bytes().startswith(b'\xef\xbb\xbf'):
        errors.append(f'{path}: UTF-8 BOM prevents Windhawk metadata recognition')
    source = path.read_text(encoding='utf-8-sig')
    match = re.search(r'^// ==WindhawkMod==\s*\n(.*?)^// ==/WindhawkMod==', source, re.M | re.S)
    if not match:
        errors.append(f'{path}: missing metadata block')
        continue
    first_line = source[:match.start(1)].count('\n') + 1
    for offset, line in enumerate(match.group(1).splitlines()):
        if line.strip() and not re.fullmatch(r'//\s+@[A-Za-z][\w-]*(?::[\w-]+)?\s+\S.*', line):
            errors.append(f'{path}:{first_line + offset}: invalid metadata line: {line}')
    id_match = re.search(r'^//\s+@id\s+([^\s]+)\s*$', match.group(1), re.M)
    if not id_match:
        errors.append(f'{path}: missing @id metadata')
    elif not re.fullmatch(r'[0-9a-z-]+', id_match.group(1)):
        errors.append(f'{path}: invalid @id {id_match.group(1)!r}; use only 0-9, a-z, and hyphen')
for error in errors:
    print(error)
if errors:
    sys.exit(1)
print(f'Metadata line validation passed: {len(paths)} file(s)')
