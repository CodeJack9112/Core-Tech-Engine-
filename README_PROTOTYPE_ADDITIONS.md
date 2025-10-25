Core Tech Engine - Prototype Feature Additions

Added prototype implementations for the following systems (not production quality, but ready to extend):

- Scene graph (SceneNode)
- Prefab Manager and expanded prefab handling + browser UI
- Asset hot-reload watcher (tools/build_helper/hotreload.py)
- AnimationPlayer stub (clip/time/loop)
- Terrain heightmap loader and mesh generator stub (stb_image required)
- Physics API for raycasts and triggers (PhysicsAPI.h/.cpp)
- Directional light + ShadowMapper stub
- Post-processing framework stub + tonemap shader
- Editor UI enhancements: Hierarchy panel, Inspector, Prefab browser, Play/Pause toggle
- Plugin API header (include/PluginAPI.h)
- Lua binding stubs for getEntity, raycast, playSound (tools/gameplay/src/lua_bindings.cpp)

Build notes:
- Many features are prototypes and expect third-party libs (stb, tinyobj, Bullet, miniaudio).
- Use -DFETCH_DEPS=ON to fetch dependencies; or provide prebuilt libs in prebuilt/.
- Start hot-reload watcher during development: python3 tools/build_helper/hotreload.py
