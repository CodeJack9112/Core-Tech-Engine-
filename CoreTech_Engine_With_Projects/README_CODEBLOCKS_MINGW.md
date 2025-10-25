
CoreTech Engine - Code::Blocks (MinGW) Project
---------------------------------------------

This repository now includes a ready-to-open Code::Blocks project file for Windows + MinGW:

  CoreTech_Engine_MinGW.cbp

How to use:
1. Install MinGW (GCC) and Code::Blocks (with MinGW) on Windows.
2. Ensure MinGW's `bin` directory is on your PATH so GCC and g++ are available.
3. Install OpenAL (OpenAL Soft) and place `OpenAL32.dll` in the same folder as the built executable or in Windows System32.
   Alternatively, install OpenAL via vcpkg and configure include/lib paths if necessary.
4. Open `CoreTech_Engine_MinGW.cbp` in Code::Blocks.
5. Build -> Build target 'Release' or 'Debug'.

Notes:
- The project assumes default MinGW include/search paths. If OpenAL headers/libs are in a custom location, update Project -> Build options -> Search directories and Linker settings.
- The CMake build system is still present and unchanged; advanced builds can continue to use CMake.
- If your MinGW toolchain uses a different name for OpenAL (e.g. `openal32` vs `openal`), adjust the linker flags in Code::Blocks project settings.

