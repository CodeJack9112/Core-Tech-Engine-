
#include "../include/AudioManager.h"
#include "../third_party/miniaudio/miniaudio_stub.h"
#include <iostream>
#include <unordered_map>
#include <string>

#ifdef USE_OPENAL
#include "audio_openal/OpenALAudio.h"
#endif

AudioManager::AudioManager(){}
AudioManager::~AudioManager(){ shutdown(); }

bool AudioManager::initialize(){
#ifdef USE_OPENAL
    if (!m_openal) m_openal = new coretech::audio::OpenALAudio();
    if (!m_openal->init()) {
        std::cerr << "AudioManager: OpenAL init failed, falling back to miniaudio stub\\n";
        // fallthrough to miniaudio stub init
    } else {
        m_usingOpenAL = true;
        return true;
    }
#endif

    // In real setup you'd configure ma_engine config; stub returns success.
    if(!m_engine){
        m_engine = new ma_engine();
        if(ma_engine_init(nullptr, m_engine) != 0){
            std::cerr << "AudioManager: failed to init audio engine\\n";
            delete m_engine; m_engine = nullptr;
            return false;
        }
    }
    return true;
}

void AudioManager::shutdown(){
#ifdef USE_OPENAL
    if(m_usingOpenAL){
        if(m_openal){
            m_openal->shutdown();
            delete m_openal; m_openal = nullptr;
        }
        m_usingOpenAL = false;
    }
#endif

    if(m_engine){
        ma_engine_uninit(m_engine);
        delete m_engine; m_engine = nullptr;
    }
    for(auto &kv : m_sounds){
#ifdef USE_OPENAL
        if(m_usingOpenAL){
            // OpenAL buffers are managed by OpenALAudio; we don't own them here.
        } else {
            if(kv.second) ma_sound_uninit(kv.second);
        }
#else
        if(kv.second) ma_sound_uninit(kv.second);
#endif
    }
    m_sounds.clear();
}

int AudioManager::loadSound(const std::string& path){
#ifdef USE_OPENAL
    if(m_usingOpenAL){
        coretech::audio::AudioBuffer buf;
        if(!m_openal->loadWAV(path, buf)){
            std::cerr << "AudioManager: OpenAL failed to load WAV: " << path << "\\n";
            return 0;
        }
        int handle = m_nextHandle++;
        // store buffer id in map by handle using negative id to indicate OpenAL buffer
        m_openalBuffers[handle] = buf.id;
        return handle;
    }
#endif
    // miniaudio fallback: use ma_sound stub
    ma_sound* sound = nullptr;
    if(ma_sound_init_from_file(m_engine, path.c_str(), nullptr, &sound) != 0){
        std::cerr << "AudioManager: failed to load sound via miniaudio: " << path << "\\n";
        return 0;
    }
    int handle = m_nextHandle++;
    m_sounds[handle] = sound;
    return handle;
}

void AudioManager::playSound(int handle, bool loop){
#ifdef USE_OPENAL
    if(m_usingOpenAL){
        auto it = m_openalBuffers.find(handle);
        if(it == m_openalBuffers.end()) return;
        unsigned int src = m_openal->playBuffer(coretech::audio::AudioBuffer{(unsigned int)it->second,0,0,0,{}} , loop);
        if(src) m_openalSources[handle] = src;
        return;
    }
#endif
    auto it = m_sounds.find(handle);
    if(it == m_sounds.end()) return;
    ma_engine_play_sound(m_engine, it->second, nullptr);
}

void AudioManager::unloadSound(int handle){
#ifdef USE_OPENAL
    if(m_usingOpenAL){
        auto it = m_openalBuffers.find(handle);
        if(it != m_openalBuffers.end()){
            // delete buffer
            ALuint buf = (ALuint)it->second;
            alDeleteBuffers(1, &buf);
            m_openalBuffers.erase(it);
        }
        auto it2 = m_openalSources.find(handle);
        if(it2 != m_openalSources.end()){
            unsigned int src = it2->second;
            m_openal->stopSource(src);
            m_openalSources.erase(it2);
        }
        return;
    }
#endif
    auto it = m_sounds.find(handle);
    if(it != m_sounds.end()){
        ma_sound_uninit(it->second);
        m_sounds.erase(it);
    }
}
