import re

with open(r"C:\Users\ROG\.gemini\antigravity\brain\0c8055cd-4b4e-4cf2-af22-43ad7032a66f\.system_generated\steps\169\content.md", "r", encoding="utf-8") as f:
    content = f.read()

matches = re.findall(r'\"bodyText\":\"(.*?)\"', content)
for i, m in enumerate(matches):
    print(f"COMMENT {i}: {m}")
