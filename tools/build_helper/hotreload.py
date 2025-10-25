#!/usr/bin/env python3
"""
Simple asset hot-reload watcher (prototype).
Watches tools/assets for file changes and writes a touch file tools/assets/.reload_trigger
The engine can detect this file change and reload assets.
Run this in background during development: python tools/build_helper/hotreload.py
"""
import time, os, sys
from pathlib import Path

watch_dir = Path("tools/assets")
trigger = watch_dir / ".reload_trigger"
if not watch_dir.exists():
    print("Assets folder not found:", watch_dir); sys.exit(1)
mtimes = {}
for p in watch_dir.rglob("*"):
    if p.is_file():
        mtimes[str(p)] = p.stat().st_mtime

print("Watching", watch_dir)
try:
    while True:
        changed = False
        for p in watch_dir.rglob("*"):
            if p.is_file():
                m = p.stat().st_mtime
                key = str(p)
                if key not in mtimes or mtimes[key] != m:
                    mtimes[key] = m
                    changed = True
        if changed:
            print("File change detected, touching reload trigger")
            with open(trigger, "w") as f: f.write("reload\n")
        time.sleep(1.0)
except KeyboardInterrupt:
    print("Watcher stopped")
