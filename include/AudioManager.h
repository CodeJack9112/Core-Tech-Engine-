\
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "PhysicsComponent.h" // not required but included for common types
// Audio manager wrapper around miniaudio (or a stub).
// For full audio, replace third_party/miniaudio/miniaudio_stub.h with real miniaudio.h/.c
struct ma_engine;
struct ma_sound;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    // Initialize audio engine. Returns false on failure.
    bool initialize();

    // Shutdown audio engine.
    void shutdown();

    // Load a sound from file (wav, ogg, etc. when using real miniaudio)
    // Returns an integer handle (>=0) or -1 on failure.
    int loadSound(const std::string& path);

    // Play sound by handle. If loop is true, sound will loop (if backend supports it).
    void playSound(int handle, bool loop=false);

    // Unload sound
    void unloadSound(int handle);


private:
#ifdef USE_OPENAL
    coretech::audio::OpenALAudio* m_openal = nullptr;
    bool m_usingOpenAL = false;
    std::unordered_map<int, unsigned int> m_openalBuffers;
    std::unordered_map<int, unsigned int> m_openalSources;
#endif

    ma_engine* m_engine = nullptr;
    std::unordered_map<int, ma_sound*> m_sounds;
    int m_nextHandle = 1;
};
