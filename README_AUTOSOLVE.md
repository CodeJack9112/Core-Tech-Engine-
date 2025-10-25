# Auto-setup system for CoreTech Engine builds

The repository includes an automatic bootstrap system that helps IDEs and CMake to build the engine
both online and offline. How it works:

- On configure, if `AUTO_SETUP` is ON and Python3 is available, CMake will run `tools/build_helper/bootstrap.py`.
- The bootstrap script will detect network availability and any prebuilt libraries under `prebuilt/`.
- It will generate `cmake/local_toolchain.cmake` which sets CMake cache variables:
  - `FETCH_DEPS` (ON to fetch third-party libs via FetchContent; OFF to use prebuilt or stubs)
  - `USE_PREBUILT_<NAME>` and `PREBUILT_<NAME>_DIR` for each known dependency
- This makes opening the project in VS Code, Code::Blocks, Visual Studio, or CLion more resilient.
- You can run the bootstrap manually: `python3 tools/build_helper/bootstrap.py --download` to try to fetch prebuilt bundles.