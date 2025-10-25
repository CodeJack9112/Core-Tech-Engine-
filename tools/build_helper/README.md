# Build Helper Bootstrap

This helper script is intended to be invoked automatically by CMake (if Python is available)
or run manually. It makes the project resilient to both online and offline builds by:

- Detecting network connectivity
- Detecting `prebuilt/` directories for third-party libraries and configuring CMake to use them
- Creating `cmake/local_toolchain.cmake` with cache variables such as `FETCH_DEPS` and `USE_PREBUILT_*`
- Optionally attempting to download prebuilt bundles (best-effort)

Usage (manual):
```bash
python3 tools/build_helper/bootstrap.py --download
```