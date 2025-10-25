# Core Tech Engine - Lightweight Runtime Glue & Stubs Added

This branch contains conservative, safe additions to make the engine runnable in any IDE without external native dependencies.

## What was added
- `Engine` facade (`include/Engine/Engine.h`, `src/Engine/Engine.cpp`)
- `ResourceManager` (text resource loader stub)
- `Scene` class (loads simple text files via ResourceManager)
- `RendererModule` and `ECSModule` as `EngineModule` implementations (stubs that print actions)
- `src/main.cpp` rewritten to register modules and run a safe 5-frame loop (no GPU required)

## Notes
- These additions are intentionally conservative: they provide scaffolding and safe behavior that run without OpenGL/Bullet/OpenAL.
- Real renderer, physics, audio, and editor still require proper integration with third-party libs. The modules included are stubs and meant to be replaced with full implementations.


## Optional Subsystems

You can enable optional subsystems via CMake options at configure time:

- `-DENABLE_OPENGL=ON` to enable OpenGL renderer (requires OpenGL headers/libs).
- `-DENABLE_GLFW=ON` to enable GLFW windowing (recommended with OpenGL).
- `-DENABLE_BULLET=ON` to enable Bullet physics (requires Bullet installed).
- `-DENABLE_OPENAL=ON` to enable OpenAL audio (requires OpenAL headers/libs).
- `-DENABLE_IMGUI=ON` to enable ImGui integration (will fetch if FETCH_DEPS=ON).

The engine compiles and runs in headless/stub mode with these options OFF. When enabling, CMake will attempt to find system packages; if not found, the features remain disabled or you must provide the libs.
