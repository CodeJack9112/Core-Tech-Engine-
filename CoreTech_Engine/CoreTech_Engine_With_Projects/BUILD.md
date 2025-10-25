\
# Build Instructions — CoreTech (Core Tech)

This repository contains the CoreTech source code and a small demo. Below are platform-specific build steps using CMake.

## Prerequisites
- CMake 3.20+
- A C++17-capable compiler (g++/clang on Linux, MSVC on Windows)
- Bullet Physics development libraries (headers + libs). On Linux: `sudo apt install libbullet-dev` (Debian/Ubuntu).
- (Optional) If you want full ImGui UI, add Dear ImGui sources and adjust include paths. The project includes a minimal ImGui stub so it builds without external UI.

## Linux (g++/clang)
```bash
mkdir -p build/linux-release
cd build/linux-release
cmake -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . -- -j$(nproc)
# Run the demo binary (name depends on your CMake target; check CMakeLists)
./CoreTech
```

## Windows (Visual Studio)
- Open a Developer Command Prompt or use Visual Studio CMake support.
```powershell
mkdir build\vs-debug
cd build\vs-debug
cmake -G "Visual Studio 17 2022" ../..
cmake --build . --config Debug
# Run executable from the generated solution or output folder.
```

## Notes
- The CMake presets are included as `CMakePresets.json`. Use your IDE to select a preset, or use the `cmake` CLI with `--preset`.
- If you wish to use real Dear ImGui, replace `third_party/imgui/imgui_single.h` and `.cpp` with the real Dear ImGui files and link necessary backends (GLFW, SDL, OpenGL, DirectX) and remove the stub behavior.


## Audio
This project includes a **miniaudio stub** at `third_party/miniaudio/miniaudio_stub.h` so the project builds without the real library. For real audio playback:

1. Download `miniaudio.h` from https://github.com/mackron/miniaudio and place it in `third_party/miniaudio/` (replace the stub file).
2. Optionally include `miniaudio.c` or compile with `-DMAKING_MINIAUDIO` if you use the amalgamated C file.
3. Provide audio sample files (e.g. `sample_drop.wav`) in the runtime working directory, or modify the demo to point to a correct path.

Once miniaudio is present, rebuild the project and the AudioManager will use the real API for playback.


## To enable full ImGui-based visual editor (recommended)

This repository includes ImGui stubs so it builds without real Dear ImGui. To enable the full interactive editor, do the following:

1. Download Dear ImGui from https://github.com/ocornut/imgui and place sources in `third_party/imgui/`.
2. Download GLFW (or install via package manager) and place or point includes/libs to `third_party/glfw/` or system paths.
3. Add GLAD (or another GL loader) to `third_party/glad/` and include its source (e.g., `glad.c`).
4. Enable ImGui in CMake: `-DENABLE_IMGUI=ON` and ensure the ImGui/backend sources are added to the SOURCES list in `CMakeLists_bullet.txt`.
5. For JSON, you may replace the lightweight serializer with `nlohmann/json.hpp` (single header) in `third_party/nlohmann/`.

After adding these dependencies, rebuild the project. The ImGui-based visual editor and inspector will become fully interactive.
