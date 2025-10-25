#pragma once
// Minimal miniaudio stub for offline builds
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ma_engine ma_engine;
typedef struct ma_sound ma_sound;
typedef struct ma_decoding_backend_config ma_decoding_backend_config;

static inline int ma_engine_init(void* pConfig, ma_engine* pEngine) { (void)pConfig; (void)pEngine; return 0; }
static inline void ma_engine_uninit(ma_engine* pEngine) { (void)pEngine; }
static inline int ma_sound_init_from_file(ma_engine* pEngine, const char* filePath, void* pConfig, ma_sound* sound) { (void)pEngine; (void)filePath; (void)pConfig; (void)sound; return 0; }
static inline void ma_sound_uninit(ma_sound* sound) { (void)sound; }
static inline int ma_engine_play_sound(ma_engine* pEngine, ma_sound* sound, const void* options) { (void)pEngine; (void)sound; (void)options; return 0; }

#ifdef __cplusplus
}
#endif
