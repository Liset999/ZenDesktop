import shutil
import sys

src = r"d:\Repository\ZenDesktop_OneKeyDeploy\local@zen-notificationcenter-acrylic.wh.cpp"
dest = r"C:\ProgramData\Windhawk\ModsSource\local@zen-notificationcenter-acrylic.wh.cpp"
log_path = r"d:\Repository\ZenDesktop_OneKeyDeploy\overwrite_status.log"

try:
    shutil.copy2(src, dest)
    with open(log_path, "w", encoding="utf-8") as f:
        f.write("SUCCESS: Overwrote file successfully!\n")
    print("Success")
except Exception as e:
    with open(log_path, "w", encoding="utf-8") as f:
        f.write(f"ERROR: {str(e)}\n")
    print(f"Error: {e}")
