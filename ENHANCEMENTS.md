# Core Tech — Suggested Enhancements & Small Additions
Generated: 2025-10-23T20:43:49.895881Z

## Overview
I added a small set of non-invasive improvements and documentation files to the repo so you can build on them quickly. These are intentionally low-risk (no changes to existing build scripts) and intended as a starting point for stronger upgrades.

### What I added to this package
1. `ENHANCEMENTS.md` — this file (detailed list of suggested upgrades and how to approach them).
2. `tools/quick_profiler/ProfilerOverlay.cpp` — a lightweight ImGui-based profiler overlay stub that hooks into the existing `DebugUI`/ImGui workflow. It's a safe, compile-able starting point (no build integration changes).
3. `tools/README_UPGRADE.md` — guidance for implementing larger features (lock-free scheduler, graph compiler improvements, multi-threading, asset pipeline, etc.).
4. `tools/ci_build_notes.txt` — notes and a CMake snippet to add a CI-friendly target.

> NOTE: I did not change existing source files or CMake configuration to avoid breaking builds. The profiler file is a new, optional source file you can include into your build to test the overlay quickly.

## High-level roadmap (short-term -> long-term)
Short term (weeks):
- Add a minimal thread pool & job system; integrate with physics and renderer tasks.
- Add unit/integration tests and CI presets (GitHub Actions/CMake).
- Add ImGui-based scene hierarchy and inspector enhancements (serialization UI).
- Implement a lock-free scheduler prototype (use atomic operations and ring buffers).

Medium term (1-3 months):
- Replace fixed-function style render passes with a frame graph for dependency-driven rendering.
- Add Vulkan backend (for Linux/modern features) and shader compiler pipeline (glslc/FXC/SPIRV).
- Implement a robust asset pipeline with hot-reload (textures, shaders, scripts).

Long term (3+ months):
- Create a production-grade visual scripting compiler (topological sort, validation, offline bytecode generation).
- Add network replication layer and authoritative server model.
- Full profiling tools + capture + playback, and editor UX polish.

---
For implementation help, see `tools/README_UPGRADE.md` in this archive.
\n## Visual Scripting Enhancement Added\n- Added EnhancedVisualScripting subsystem under src/visualscripting.\n- Provides topological sort, validation, simple bytecode, interpreter, and ImGui compile UI.\n