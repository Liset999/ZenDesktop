import os
import shutil
import subprocess
import sys

# -------------------------------------------------------------------
# ZenDesktop Dual-Repository Synchronization Skill
# -------------------------------------------------------------------
# This script automates the synchronization of Mod source files from 
# the local development repository (ZenDesktop_OneKeyDeploy) to the 
# official Windhawk community repository fork (windhawk-mods).
# -------------------------------------------------------------------

SOURCE_DIR = r"D:\Repository\ZenDesktop_OneKeyDeploy"
TARGET_DIR = r"D:\Repository\windhawk-mods\mods"
TARGET_REPO = r"D:\Repository\windhawk-mods"

# Mapping of local files to community repository file names
# (The 'local@' prefix is stripped for the official community repo)
# Only the 4 core ZenDesktop mods are synced to windhawk-mods.
SYNC_MAP = {
    "local@zen-taskbar-acrylic.wh.cpp":             "zen-taskbar-acrylic.wh.cpp",
    "local@zen-notificationcenter-acrylic.wh.cpp":  "zen-notificationcenter-acrylic.wh.cpp",
    "local@zen-startmenu-acrylic.wh.cpp":           "zen-startmenu-acrylic.wh.cpp",
    "local@zen-desktop-toggle-icons.wh.cpp":        "zen-desktop-toggle-icons.wh.cpp",
}

def run_git_command(command, cwd):
    print(f"Running: {' '.join(command)}")
    try:
        result = subprocess.run(command, cwd=cwd, check=True, text=True, capture_output=True)
        if result.stdout.strip():
            print(result.stdout.strip())
        return True
    except subprocess.CalledProcessError as e:
        print(f"Git command failed: {e}")
        if e.stdout: print(e.stdout.strip())
        if e.stderr: print(e.stderr.strip())
        return False

def main():
    print("======================================================")
    print("  ZenDesktop -> Windhawk Mods Sync Skill")
    print("======================================================")

    if not os.path.exists(TARGET_DIR):
        print(f"[ERROR] Target directory not found: {TARGET_DIR}")
        print("Please ensure the windhawk-mods repository is cloned at the specified path.")
        sys.exit(1)

    synced_files = []

    # 1. Copy files
    for local_file, target_file in SYNC_MAP.items():
        src_path = os.path.join(SOURCE_DIR, local_file)
        dst_path = os.path.join(TARGET_DIR, target_file)

        if not os.path.exists(src_path):
            print(f"[WARN] Source file not found, skipping: {local_file}")
            continue
        
        # Only copy if the target exists (to avoid pushing unofficial mods to the community repo by mistake)
        if not os.path.exists(dst_path):
            print(f"[SKIP] Target file does not exist in community repo, skipping: {target_file}")
            continue

        try:
            # Compare contents to see if sync is actually needed
            with open(src_path, 'r', encoding='utf-8') as sf, open(dst_path, 'r', encoding='utf-8') as df:
                if sf.read() == df.read():
                    print(f"[SKIP] No changes detected for: {target_file}")
                    continue

            shutil.copy2(src_path, dst_path)
            print(f"[SYNC] Copied {local_file} -> {target_file}")
            synced_files.append(target_file)
        except Exception as e:
            print(f"[ERROR] Failed to copy {local_file}: {e}")

    if not synced_files:
        print("\nAll files are up to date. No git commit needed.")
        return

    # 2. Git Commit
    print("\n[GIT] Preparing to commit changes to windhawk-mods repository...")
    
    # Add files
    for f in synced_files:
        run_git_command(["git", "add", f"mods/{f}"], cwd=TARGET_REPO)
    
    # Check if there are changes to commit
    status_result = subprocess.run(["git", "status", "--porcelain"], cwd=TARGET_REPO, capture_output=True, text=True)
    if not status_result.stdout.strip():
        print("[GIT] Working tree clean, nothing to commit.")
        return

    # Commit
    commit_msg = f"update ZenDesktop mods ({', '.join([f.split('.')[0] for f in synced_files])})"
    success = run_git_command(["git", "commit", "-m", commit_msg], cwd=TARGET_REPO)

    if success:
        print("\n======================================================")
        print("[SUCCESS] Files synchronized and committed locally!")
        print("Next steps:")
        print(f"  1. cd {TARGET_REPO}")
        print("  2. git push")
        print("  3. Create a Pull Request on GitHub")
        print("======================================================")

if __name__ == "__main__":
    main()
