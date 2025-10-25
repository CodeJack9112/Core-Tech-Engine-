#include "AudioManager.h"
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#ifdef __has_include
  #if __has_include("miniaudio.h")
    #define HAVE_MINIAUDIO 1
  #endif
#endif
#if HAVE_MINIAUDIO
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#endif

namespace CoreTech {

#if HAVE_MINIAUDIO
static ma_engine engine;
struct ManagedSound {
    ma_sound sound;
    bool initialized = false;
    ManagedSound(){}
    ~ManagedSound(){ if(initialized) ma_sound_uninit(&sound); }
};
static std::vector<std::unique_ptr<ManagedSound>> g_sounds;
static std::mutex g_sounds_mutex;
#endif

bool initAudio(){
#if HAVE_MINIAUDIO
    if(ma_engine_init(NULL, &engine) != MA_SUCCESS){
        std::cerr << "miniaudio init failed\n"; return false;
    }
    std::cout << "miniaudio initialized\n";
    return true;
#else
    std::cout << "miniaudio not available - audio functions are stubs\n";
    return true;
#endif
}

void shutdownAudio(){
#if HAVE_MINIAUDIO
    std::lock_guard<std::mutex> lk(g_sounds_mutex);
    g_sounds.clear(); // destructor will uninit sounds
    ma_engine_uninit(&engine);
    std::cout << "miniaudio shutdown\n";
#endif
}

bool playSoundFile(const char* path){
#if HAVE_MINIAUDIO
    try{
        auto ms = std::make_unique<ManagedSound>();
        if(ma_sound_init_from_file(&engine, path, MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, &ms->sound) != MA_SUCCESS){
            std::cerr << "Failed to init sound: " << path << std::endl;
            return false;
        }
        ms->initialized = true;
        // play sound
        if(ma_engine_play_sound(&engine, &ms->sound, NULL) != MA_SUCCESS){
            std::cerr << "Failed to play sound: " << path << std::endl;
            return false;
        }
        // store and let destructor handle cleanup later; we'll periodically compact finished sounds
        {
            std::lock_guard<std::mutex> lk(g_sounds_mutex);
            g_sounds.emplace_back(std::move(ms));
        }
        return true;
    }catch(...){
        return false;
    }
#else
    std::cout << "playSoundFile (stub) would play: " << path << std::endl;
    return false;
#endif
}

// Call periodically to clean finished sounds
void audioUpdate(){
#if HAVE_MINIAUDIO
    std::lock_guard<std::mutex> lk(g_sounds_mutex);
    // remove sounds that are no longer playing
    auto it = g_sounds.begin();
    while(it != g_sounds.end()){
        ManagedSound* ms = it->get();
        if(ms->initialized){
            if(!ma_sound_is_playing(&ms->sound)){
                // destructor will uninit
                it = g_sounds.erase(it);
                continue;
            }
        }
        ++it;
    }
#endif
}

} // namespace CoreTech
