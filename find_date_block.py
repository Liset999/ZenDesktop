with open("local@zen-taskbar-acrylic.wh.cpp", "r", encoding="utf-8") as f:
    content = f.read()

lines = content.splitlines()

for idx, line in enumerate(lines):
    if "DateInnerTextBlock" in line:
        print(f"Line {idx}: {line}")
        # print 5 lines before and after
        start = max(0, idx - 5)
        end = min(len(lines), idx + 5)
        for i in range(start, end):
            print(f"  {i}: {lines[i]}")
        print("-" * 50)
