
# Building the CoreTech Engine with CMake, Code::Blocks, and VS Code

This repository includes multiple ways to build and work with the engine.

## CMake (recommended)
1. Create a build directory and configure:
   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build --config Debug -- -j4
   ```
2. Run demo or engine binaries from `build/`

## Code::Blocks (provided project)
A Code::Blocks project file `CoreTech_Engine.cbp` and workspace `CoreTech_Engine.workspace` are included.
They are configured to run CMake configure + build commands when you build the project inside Code::Blocks.
- Open `CoreTech_Engine.workspace` in Code::Blocks.
- Build the Debug or Release target (this will run cmake and build using your make/ninja toolchain).

If you prefer Code::Blocks to manage compilation natively, you can also generate a `.cbp` using CMake generator:
```bash
cmake -G "CodeBlocks - MinGW Makefiles" -S . -B build
```

## Visual Studio Code
A `.vscode` folder is included with tasks to configure and build using CMake:
- Open repository in VS Code.
- Run Tasks: "CMake: Configure (Debug)" then "CMake: Build (Debug)".
- Launch configuration expects `build/Debug/CoreTech_Engine.exe` — adjust `launch.json` if using different path or OS.

## Notes
- The provided Code::Blocks project uses custom build commands to run CMake. It assumes `cmake` and a build tool (make/ninja) are installed.
- On Windows with MSVC, you may prefer to use the Visual Studio solution provided in the repo instead.
