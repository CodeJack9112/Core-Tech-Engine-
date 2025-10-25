
# CoreTech Engine - No-manual-setup builds & contribution guide

## No manual setup
- CMake FetchContent automatically downloads GLFW, GLM, nlohmann/json, GLAD, ENet, and Lua by default.
- To build (recommended):
  ```bash
  cmake -S . -B build
  cmake --build build --config Debug -- -j$(nproc)
  ./build/path/to/gameplay_demo
  ```
- For Code::Blocks native builds, open the autogen project; if external libs are missing, the project will still build if you enable prebuilt mode or run CMake first to populate thirdparty builds.

## Extension points for contributors
- `tools/gameplay/include/Entity.h` — add new components derived from `Component` (e.g., MeshRenderer, RigidBody)
- `tools/gameplay/src/CameraController.cpp` — camera behavior; replace or extend for FPS/third-person
- `tools/game_app/src/GameApp.cpp` — window creation and input hookup; contributors can add ImGui, editor UI, or renderer integration here.
- `tools/gameplay/src/LuaBindings.cpp` — Lua<->engine glue; add bindings to new components using `lua_push...` helpers.
- `tools/enet_lua_sample` and `tools/gameplay` hold examples of integrating networking and scripting.
