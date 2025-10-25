#!/usr/bin/env bash
# Helper: configure and build CoreTech with FetchContent dependencies.
set -e
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc || 4)
echo "Build finished. Check build directory for targets."
