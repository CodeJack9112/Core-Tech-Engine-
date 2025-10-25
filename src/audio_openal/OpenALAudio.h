#pragma once
// Simple OpenAL audio backend for CoreTech Engine.
// Requires OpenAL headers and library (OpenAL Soft recommended).
// Build with -DUSE_OPENAL=ON and link with OpenAL (find_package or pkg-config).

#include <string>
#include <vector>
#include <cstdint>

#ifdef USE_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace coretech {
namespace audio {

struct AudioBuffer {
    unsigned int id = 0; // OpenAL buffer id
    int channels = 0;
    int sampleRate = 0;
    int bitsPerSample = 0;
    std::vector<uint8_t> data;
};

class OpenALAudio {
public:
    // Streaming OGG playback
    // returns source id (>0) on success
    unsigned int startStreaming(const std::string& filename, bool loop=false);
    void stopStreaming(unsigned int source);

    OpenALAudio();
    ~OpenALAudio();

    bool init();
    void shutdown();

    // load a simple WAV file (PCM) into an AudioBuffer
    bool loadWAV(const std::string& filename, AudioBuffer& outBuffer);

    // play a buffer, return source id (0 = failure)
    unsigned int playBuffer(const AudioBuffer& buffer, bool loop = false);

    void stopSource(unsigned int source);
    void setListenerPosition(float x, float y, float z);

#ifdef USE_OPENAL
#include <thread>
#include <atomic>
#include <mutex>
#include <map>

struct StreamingSource {
    unsigned int source = 0;
    std::vector<unsigned int> buffers;
    std::thread thread;
    std::atomic<bool> running{false};
    void* stb_handle = nullptr; // opaque pointer to stb_vorbis*
    std::mutex mutex;
};
#endif
private:
#ifdef USE_OPENAL
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
#endif
};

} // namespace audio
} // namespace coretech
