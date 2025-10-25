CoreTech - Dependency and Build Guide (Complete Package)
=======================================================

Target platforms: Windows (DirectX + OpenGL) and Linux (OpenGL)
Toolchains: Visual Studio (MSVC) and GCC/Clang (CMake + Ninja/Make)
Engine goal: Game Development Toolkit (ImGui editor, animations, physics, audio, rendering)

Recommended dependency versions (known-good starting points):
- Bullet Physics: bullet3 v3.24 (or latest stable release)
  Include: btBulletDynamicsCommon.h
  Source: https://github.com/bulletphysics/bullet3

- SDL2: libsdl-org/SDL tag 2.28.2 (for window/input on non-DirectX builds)
  Include: SDL.h
  Source: https://github.com/libsdl-org/SDL

- GLM: 0.9.9.8
  Include: glm/glm.hpp
  Source: https://github.com/g-truc/glm

- glad: latest release (or use system loader)
  Include: glad/glad.h
  Source: https://github.com/Dav1dde/glad

- GLFW: latest stable (if used in some code paths)
  Include: GLFW/glfw3.h
  Source: https://github.com/glfw/glfw

- Assimp (for model & animation import): assimp v5.2.5 (or master)
  Include: assimp/scene.h, assimp/Importer.hpp
  Source: https://github.com/assimp/assimp

- ImGui: latest master (for editor/debug UI)
  Include: imgui.h
  Source: https://github.com/ocornut/imgui

- OpenAL-soft (audio) or miniaudio for single-header option.
  Include: AL/al.h (OpenAL) or miniaudio.h (miniaudio)
  Source: https://github.com/kcat/openal-soft or https://github.com/mackron/miniaudio

- stb (stb_image.h) for texture loading (single-header)
  Source: https://github.com/nothings/stb

Important build tools:
- CMake >= 3.22 (FetchContent improvements and presets)
- Ninja (recommended on Linux) or Make
- Git (for FetchContent)
- Visual Studio 2022 with Desktop development workload (on Windows) OR GCC/Clang toolchain on Linux

Quick build (online FetchContent method):
  mkdir build && cd build
  cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
  cmake --build . -j$(nproc || 4)

Quick build (Windows - Visual Studio):
  mkdir build && cd build
  cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
  cmake --build . --config Release

Offline prebuilt integration:
- Place prebuilt SDKs under: thirdparty/prebuilt/<libname> with `include/` and `lib/` subfolders.
- The top-level CMakeLists.txt prefers thirdparty/prebuilt/ if present.

Notes on animations:
- Assimp will import skeletons, bones, and keyframe animations from FBX/DAE/GLTF files.
- You will need the engine's animation runtime (skeleton skinning + GPU upload). The source contains animation-related headers; ensure Assimp is enabled to build the demo importer.

If you'd like, upload your prebuilt SDK folder (thirdparty/prebuilt) and I will integrate binaries and repackage a single self-contained zip.
