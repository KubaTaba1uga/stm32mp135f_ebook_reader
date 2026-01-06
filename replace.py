#!/usr/bin/env python3
import os

def replace_in_file(path: str, old: str, new: str) -> bool:
    try:
        with open(path, "r", encoding="utf-8") as f:
            data = f.read()
    except (UnicodeDecodeError, PermissionError, IsADirectoryError):
        return False  # skip non-text/unreadable

    if old not in data:
        return False

    data = data.replace(old, new)

    with open(path, "w", encoding="utf-8") as f:
        f.write(data)

    return True

def replace_in_dir(root_dir: str, old: str, new: str):
    changed = 0
    for dirpath, _, filenames in os.walk(root_dir):
        for name in filenames:
            path = os.path.join(dirpath, name)
            if replace_in_file(path, old, new):
                changed += 1
                print(f"modified: {path}")
    print(f"\nDone. Modified {changed} file(s).")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <dir> <old_text> <new_text>")
        sys.exit(1)

    replace_in_dir(sys.argv[1], sys.argv[2], sys.argv[3])
