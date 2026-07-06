import os

files = [
    "local@zen-notificationcenter-acrylic.wh.cpp",
    "notification_center_full.wh.cpp",
    "notification_center_orig.wh.cpp"
]

for filename in files:
    if os.path.exists(filename):
        with open(filename, "r", encoding="utf-8") as f:
            lines = f.readlines()
        for idx, line in enumerate(lines):
            if "runFromWindowThread" in line:
                print(f"{filename}:{idx+1}: {line.strip()}")
