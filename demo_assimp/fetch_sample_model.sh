#!/usr/bin/env bash
set -euo pipefail
mkdir -p ../assets
cd ../assets
echo "Downloading sample model CesiumMan.glb (glTF Binary) from Khronos glTF-Sample-Models..."
URL="https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/CesiumMan/glTF-Binary/CesiumMan.glb"
if command -v curl >/dev/null 2>&1; then
  curl -L -o CesiumMan.glb "$URL"
elif command -v wget >/dev/null 2>&1; then
  wget -O CesiumMan.glb "$URL"
else
  echo "curl or wget required to download the sample model. Please download manually from $URL"
  exit 1
fi
echo "Downloaded ../assets/CesiumMan.glb"
