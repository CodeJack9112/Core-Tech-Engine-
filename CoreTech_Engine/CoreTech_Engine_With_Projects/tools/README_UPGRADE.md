# Upgrade Notes & Implementation Guide

This file gives practical steps and recommended patterns for upgrading Core Tech's engine.
It focuses on safe, incremental changes to reduce regressions and provide measurable improvements.

## 1) Minimal Thread Pool & Job System
- Goal: improve CPU utilization on multi-core machines.
- Suggested approach: create a simple thread-pool with a lock-free job queue (e.g., single-producer multiple-consumer ring buffer).
- Integration targets: physics step, audio mixing, and prepare-render tasks.
- Safety: keep the existing single-threaded flow intact and add flags to enable the job system.

## 2) Lock-Free Scheduler (prototype)
- Use per-worker single-producer single-consumer queues (SPSC) and work-stealing for starvation handling.
- Start with atomic<int> head/tail indices and a power-of-two sized circular buffer.

## 3) Visual Scripting Compiler Enhancements
- Implement topological sort (Kahn's algorithm) on graph nodes; validate for cycles and type mismatches.
- Emit a compact bytecode or index-based instruction stream for runtime execution with zero lookups.

## 4) Frame Graph
- Replace hardcoded render passes with a frame graph that tracks resources and pass dependencies.
- Useful for multi-backend abstraction and safe automatic synchronization.

## 5) Asset Pipeline & Hot Reload
- Create an external tool that watches asset folders and converts raw assets to runtime-friendly blobs.
- Implement per-asset versioning and checksum-based reload to minimize recomposition time.

## 6) CI / Tests
- Add CMake test targets and a GitHub Actions workflow to run build + unit tests on push/PR.
- Use Catch2 or GoogleTest for lightweight unit tests.

