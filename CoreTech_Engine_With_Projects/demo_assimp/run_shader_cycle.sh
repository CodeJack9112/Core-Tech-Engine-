#!/usr/bin/env bash
set -e
mkdir -p screenshots
BUILD_DIR="../build"
if [ ! -d "$BUILD_DIR" ]; then
  echo "Build directory not found. Configure and build first (use CMakePresets.json or build_deps.sh)."
  exit 1
fi
# Attempt to run the demo binary from common build locations
if [ -f "$BUILD_DIR/demo_assimp/demo_shader_cycle" ]; then
  "$BUILD_DIR/demo_assimp/demo_shader_cycle" ../assets/CesiumMan.glb
elif [ -f "$BUILD_DIR/demo_assimp/demo_shader_cycle.exe" ]; then
  "$BUILD_DIR/demo_assimp/demo_shader_cycle.exe" ../assets/CesiumMan.glb
else
  echo "Demo binary not found. Build the project and ensure demo_shader_cycle target is built."
  exit 1
fi
