
# Using Prebuilt ENet & Lua with Code::Blocks (Auto-copy support)

This repository supports two modes for ENet and Lua:

1. **CMake fetch & build** (recommended) - uses FetchContent to download and build ENet and Lua during CMake configure.
2. **Prebuilt libraries** (for Code::Blocks/native builds) - you can provide prebuilt ENet and Lua installations and the project will link against them.

## How to use prebuilt libs with Code::Blocks (Windows)

1. Place prebuilt libraries under a folder inside the repo, for example `prebuilt/enet` and `prebuilt/lua` with the following structure:
   - `prebuilt/enet/include/...` (headers)
   - `prebuilt/enet/lib/enet.lib` (or `libenet.a`)
   - `prebuilt/lua/include/...`
   - `prebuilt/lua/lib/lua.lib` (or `liblua.a`)

2. Open `CoreTech_Engine_autogen.cbp` in Code::Blocks.
   - The project automatically includes `prebuilt` and `thirdparty` in its linker search directories.
   - If Code::Blocks still fails to find the libs, open Project -> Build options -> Linker settings and add the exact library filename.

3. After building, run the provided batch script to copy DLLs into the build output directory (for example `bin\Debug`):
   ```
   tools\scripts\copy_prebuilt.bat "prebuilt\enet;prebuilt\lua;thirdparty" "bin\Debug"
   ```

## How to use prebuilt libs with CMake (optional)

When running CMake, you can switch to prebuilt mode by specifying:
```bash
cmake -S . -B build -DUSE_PREBUILT_ENET=ON -DPREBUILT_ENET_DIR=prebuilt/enet -DUSE_PREBUILT_LUA=ON -DPREBUILT_LUA_DIR=prebuilt/lua
```
This will create IMPORTED targets `prebuilt_enet` and `prebuilt_lua` that the sample and other targets will link against.
