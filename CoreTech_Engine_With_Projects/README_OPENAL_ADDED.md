
# OpenAL Support Added (Assistant)

Files added:
- src/audio_openal/OpenALAudio.h
- src/audio_openal/OpenALAudio.cpp
- src/audio_openal/CMakeLists.txt

How it works:
- This provides a simple OpenAL audio backend with WAV loading, playback, and listener control.
- Build with the CMake option `-DUSE_OPENAL=ON`. If OpenAL is found, the code will be compiled and linked.
- The wrapper expects OpenAL headers `<AL/al.h>` and `<AL/alc.h>` to be available on the system.

Notes and limitations:
- The WAV loader is minimal and supports common PCM WAV files. It is not a full-featured audio file loader.
- You will still need to integrate calls into the engine (initialization, calling loadWAV, and managing sources).
- If the engine previously had an audio abstraction, connect this library to that audio interface.

Example CMake build (Linux):

    mkdir build && cd build
    cmake -DUSE_OPENAL=ON ..
    make -j

On Windows use vcpkg or install OpenAL Soft and ensure OpenAL include/lib paths are visible to CMake.
