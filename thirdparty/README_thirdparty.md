CoreTech - Thirdparty requirements (auto-generated)
==================================================

The scanner found these external includes in the source. They indicate which third-party libraries
the engine expects to link against or use. Below is a prioritized list of libraries, suggested
version/commit hints, and the exact include locations found in your source tree.

=== Detected includes ===

- ../../include/VisualScripting.h
    - tools/visual_scripting_editor/create_demo_graph.cpp:3

- ../include/AudioManager.h
    - src/main.cpp:10
    - src/AudioManager.cpp:2
    - src/VisualScriptingNodes.cpp:2

- ../include/ConfigLoader.h
    - src/main.cpp:3
    - src/ConfigLoader.cpp:1
    - src/LauncherImGui.cpp:4

- ../include/DebugUI.h
    - demo/scene_gameplay.cpp:7
    - src/DebugUI.cpp:1

- ../include/DirectXRenderer.h
    - src/main.cpp:13
    - src/DirectXRenderer.cpp:4

- ../include/EditorInspector.h
    - src/VSEditorImGui.cpp:1
    - src/EditorInspector.cpp:2

- ../include/GameplayPCGManager.h
    - demo/scene_gameplay.cpp:2
    - src/GameplayPCGManager.cpp:1
    - src/main.cpp:7

- ../include/Launcher.h
    - src/main.cpp:4
    - src/LauncherWin32.cpp:3
    - src/LauncherWin32.cpp:122
    - src/LauncherImGui.cpp:3

- ../include/LauncherImGui.h
    - src/main.cpp:5
    - src/LauncherImGui.cpp:2
    - src/VSEditorImGui.cpp:3

- ../include/NavMesh.h
    - demo/scene_gameplay.cpp:5
    - src/NavMesh.cpp:1
    - src/main.cpp:8

- ../include/PCGManager.h
    - src/PCGManager.cpp:1

- ../include/PhysicsComponent.h
    - src/PhysicsManager.cpp:2
    - src/PhysicsComponent.cpp:1

- ../include/PhysicsManager.h
    - demo/scene_setup.cpp:3
    - demo/scene_gameplay.cpp:3
    - src/PhysicsManager.cpp:1
    - src/PCGManager.cpp:2
    - src/GameplayPCGManager.cpp:2
    - ... (+3 more occurrences)

- ../include/PickupManager.h
    - demo/scene_gameplay.cpp:6
    - src/PickupManager.cpp:1
    - src/main.cpp:9

- ../include/Renderer.h
    - src/main.cpp:11
    - src/Renderer.cpp:1

- ../include/RendererDX11.h
    - src/RendererDX11.cpp:3

- ../include/RendererGL.h
    - src/RendererGL.cpp:2

- ../include/VisualScripting.h
    - include/EditorInspector.h:3
    - src/VisualScripting.cpp:2
    - src/VisualScriptingNodes.cpp:1
    - src/VisualScriptingNodes.cpp:63
    - src/VSEditorImGui.cpp:2
    - ... (+1 more occurrences)

- ../include/imgui_stub.h
    - src/DebugUI.cpp:3
    - src/LauncherImGui.cpp:11
    - src/VSEditorImGui.cpp:4
    - src/EditorInspector.cpp:3

- ../third_party/miniaudio/miniaudio_stub.h
    - src/AudioManager.cpp:3

- GLFW/glfw3.h
    - src/RendererGL.cpp:10

- btBulletDynamicsCommon.h
    - demo/scene_setup.cpp:4
    - demo/scene_gameplay.cpp:4
    - include/PhysicsManager.h:7
    - include/PhysicsComponent.h:3
    - include/PCGManager.h:5
    - ... (+3 more occurrences)

- glad/glad.h
    - src/RendererGL.cpp:11

- glm/glm.hpp
    - include/PhysicsManager.h:6

- imgui.h
    - include/imgui_stub.h:10
    - src/visualscripting/EnhancedVisualScripting.cpp:4
    - tools/quick_profiler/ProfilerOverlay.cpp:8

- imgui_single.h
    - third_party/imgui/imgui_single.cpp:2

=== Suggested libraries & versions ===
- Bullet Physics (btBulletDynamicsCommon.h, etc.) -- recommended: bullet3 master or stable tag.
- SDL2 (SDL.h) -- libsdl.org or github libsdl-org/SDL main branch.
- GLM (glm/vec3.hpp, glm/... ) -- recommended tag 0.9.9.x or latest stable.
- stb (stb_image.h) -- single-header from nothings/stb.
- ImGui (imgui.h) -- ocornut/imgui master.
- Assimp (assimp/scene.h etc.) -- assimp master.
- OpenAL-soft (AL/al.h) -- kcat/openal-soft.
- Others: any custom or engine-specific header groups found above.

=== What I prepared for you ===
- cmake/FetchDependencies.cmake (FetchContent download of sources during CMake configure).
- build_deps.sh helper script to run CMake and build.
- fetch_binaries_linux.sh and fetch_binaries_win.ps1 - scripts to download prebuilt binaries (not run here).
- CMake now includes a small search routine to prefer `thirdparty/prebuilt/` if you place prebuilt libs there.

=== How to get a fully offline bundle (recommended workflow) ===
Option A (automated, online during build):
  - Ensure network access and run: `mkdir build && cd build && cmake .. && cmake --build .`.
  - CMake will use FetchContent to clone and build dependencies on your machine.

Option B (prepare prebuilt offline bundle yourself):
  - On a machine with internet, run the `fetch_binaries_*` scripts (linux or windows PowerShell).
  - Those scripts download prebuilt SDK zips into `thirdparty/prebuilt/` and extract them.
  - Then copy the entire `CoreTech_source/` directory to the target offline machine.

=== Notes ===
- I cannot download and embed the large libraries here due to environment/network restrictions,
  but the attached scripts will fetch recommended binaries when run on your machine.
- If you want, upload the prebuilt libraries here and I will integrate them into the project and repackage.
