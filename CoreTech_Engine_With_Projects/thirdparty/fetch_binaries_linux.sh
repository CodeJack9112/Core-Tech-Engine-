#!/usr/bin/env bash
# Fetch prebuilt-ish sources and SDKs for Linux. Edit URLs if you prefer distro packages.
set -euo pipefail
mkdir -p thirdparty/prebuilt
cd thirdparty/prebuilt

echo "Fetching suggested archives. These are source archives that may need building."
# Bullet (source)
if [ ! -d bullet3 ]; then
  curl -L -o bullet3.tar.gz https://github.com/bulletphysics/bullet3/archive/refs/tags/3.24.tar.gz ||       curl -L -o bullet3.tar.gz https://github.com/bulletphysics/bullet3/archive/refs/heads/master.tar.gz
  mkdir -p bullet3 && tar -xzf bullet3.tar.gz --strip-components=1 -C bullet3
fi

# SDL2
if [ ! -d SDL ]; then
  curl -L -o sdl2.tar.gz https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.28.2.tar.gz ||       curl -L -o sdl2.tar.gz https://github.com/libsdl-org/SDL/archive/refs/heads/main.tar.gz
  mkdir -p SDL && tar -xzf sdl2.tar.gz --strip-components=1 -C SDL
fi

# Assimp
if [ ! -d assimp ]; then
  curl -L -o assimp.tar.gz https://github.com/assimp/assimp/archive/refs/tags/v5.2.5.tar.gz ||       curl -L -o assimp.tar.gz https://github.com/assimp/assimp/archive/refs/heads/master.tar.gz
  mkdir -p assimp && tar -xzf assimp.tar.gz --strip-components=1 -C assimp
fi

# ImGui
if [ ! -d imgui ]; then
  curl -L -o imgui.tar.gz https://github.com/ocornut/imgui/archive/refs/heads/master.tar.gz
  mkdir -p imgui && tar -xzf imgui.tar.gz --strip-components=1 -C imgui
fi

echo "Downloaded sources into thirdparty/prebuilt/. You may now build them and point CMake to include/ and lib/."
