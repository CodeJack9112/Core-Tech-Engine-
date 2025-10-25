CoreTech - Thirdparty placeholders and FetchContent helper
---------------------------------------------------------

What this folder contains:
1) Guidance and CMake helper (FetchDependencies.cmake) that uses CMake's FetchContent
   to download and build required libraries automatically during CMake configure.
2) Small header-only stubs where appropriate (not real libraries).

IMPORTANT:
- I cannot bundle large external libraries (Bullet, SDL2, Assimp, OpenAL, ImGui, etc.)
  into this zip in this environment.
- Instead, the CMake helper below will download & build them on your machine when you run:
    mkdir build && cd build && cmake .. && cmake --build .
  (Requires internet access and standard build tools installed on your machine.)

If you want absolutely offline builds, you must download the libraries listed below and
place them in `CoreTech_source/thirdparty/` as instructed in each library's own README.

Libraries configured to be fetched by CMake (recommended):
- Bullet Physics
- SDL2 (for window/input on non-DirectX builds)
- GLM (math)
- stb (image stb_image)
- ImGui (editor/debug UI)
- Assimp (model loading)
- OpenAL-soft (audio backend) [optional]

After running CMake, check CMake output to see if FetchContent succeeded.
If any downloads fail, ensure your machine has network access and access to git/cmake downloads.
