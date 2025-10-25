\
#pragma once
// miniaudio stub for compile without real audio library.
// For full audio support, replace this file with the real miniaudio.h from https://github.com/mackron/miniaudio
// The real miniaudio provides functions: ma_engine, ma_engine_init, ma_engine_uninit, ma_sound, ma_sound_init_from_file, ma_sound_uninit, ma_engine_play_sound, etc.
// This stub provides a tiny subset of types and no-op implementations to allow the engine to compile and run without sound.

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ma_engine ma_engine;
typedef struct ma_sound ma_sound;

struct ma_engine {
    int dummy;
};

struct ma_sound {
    int dummy;
};

static inline int ma_engine_init(void* pConfig, ma_engine* pEngine) {
    (void)pConfig; (void)pEngine; return 0;
}
static inline void ma_engine_uninit(ma_engine* pEngine) { (void)pEngine; }
static inline int ma_sound_init_from_file(ma_engine* pEngine, const char* filePath, ma_sound* sound) { (void)pEngine; (void)filePath; (void)sound; return 0; }
static inline void ma_sound_uninit(ma_sound* sound) { (void)sound; }
static inline int ma_engine_play_sound(ma_engine* pEngine, ma_sound* sound, void* options) { (void)pEngine; (void)sound; (void)options; return 0; }

#ifdef __cplusplus
}
#endif
