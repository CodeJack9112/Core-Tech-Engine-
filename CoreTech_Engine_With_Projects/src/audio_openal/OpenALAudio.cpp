#include "OpenALAudio.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <condition_variable>
#ifdef USE_OPENAL
extern "C" {
    typedef struct stb_vorbis stb_vorbis;
    stb_vorbis* stb_vorbis_open_filename(const char *filename, int *error, const void *alloc);
    int stb_vorbis_get_samples_short_interleaved(stb_vorbis *f, int channels, short *buffer, int num_shorts);
    int stb_vorbis_stream_length_in_samples(stb_vorbis *f);
    void stb_vorbis_close(stb_vorbis *f);
    typedef struct { int sample_rate; int channels; } stb_vorbis_info;
    stb_vorbis_info stb_vorbis_get_info(stb_vorbis *f);
    int stb_vorbis_get_sample_rate(stb_vorbis *f);
    void stb_vorbis_seek_start(stb_vorbis *f);
    int stb_vorbis_stream_length_in_samples(stb_vorbis *f);
    void stb_vorbis_close(stb_vorbis *f);
}
#endif


namespace coretech {
namespace audio {

OpenALAudio::OpenALAudio() = default;

// Map of streaming sources: source id -> StreamingSource
#ifdef USE_OPENAL
static std::map<unsigned int, StreamingSource> g_streamingSources;
#endif

OpenALAudio::~OpenALAudio() { shutdown(); }

bool OpenALAudio::init() {
#ifdef USE_OPENAL
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "[OpenAL] Failed to open device\n";
        return false;
    }
    context = alcCreateContext(device, nullptr);
    if (!context || alcMakeContextCurrent(context) == ALC_FALSE) {
        std::cerr << "[OpenAL] Failed to create or set context\n";
        if (context) alcDestroyContext(context);
        alcCloseDevice(device);
        device = nullptr;
        return false;
    }
    std::cout << "[OpenAL] Initialized\n";
    return true;
#else
    std::cerr << "[OpenAL] Built without USE_OPENAL - disabled\n";
    return false;
#endif
}

void OpenALAudio::shutdown() {
#ifdef USE_OPENAL
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }
    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
#endif
}

// Minimal WAV loader for PCM 16/8-bit. Not robust but works for typical PCM WAVs.
static bool readLE(std::ifstream& f, void* out, std::size_t bytes) {
    f.read(reinterpret_cast<char*>(out), bytes);
    return f.gcount() == bytes;
}

bool OpenALAudio::loadWAV(const std::string& filename, AudioBuffer& outBuffer) {
    // Support OGG via stb_vorbis if available
    std::string lower = filename;
    for(auto &c: lower) c = tolower(c);
    if(lower.size() >= 4 && lower.substr(lower.size()-4) == ".ogg"){
#ifdef USE_OPENAL
        // stb_vorbis provides a convenience to decode an entire file to signed 16-bit PCM
        extern "C" int stb_vorbis_decode_filename(const char *filename, int *channels, int *sample_rate, short **output);
        int channels=0, sampleRate=0;
        short* output = nullptr;
        int samples = stb_vorbis_decode_filename(filename, &channels, &sampleRate, &output);
        if(samples <= 0 || output==nullptr){
            std::cerr << "[OpenAL] stb_vorbis failed to decode OGG: " << filename << "\n";
            if(output) free(output);
            return false;
        }
        // samples is number of samples per channel
        outBuffer.channels = channels;
        outBuffer.sampleRate = sampleRate;
        outBuffer.bitsPerSample = 16;
        size_t totalSamples = (size_t)samples * channels;
        outBuffer.data.resize(totalSamples * sizeof(short));
        memcpy(outBuffer.data.data(), output, totalSamples * sizeof(short));
        free(output);
        // create OpenAL buffer
        ALenum format = 0;
        if(outBuffer.channels == 1) format = AL_FORMAT_MONO16;
        else if(outBuffer.channels == 2) format = AL_FORMAT_STEREO16;
        else { std::cerr << "[OpenAL] Unsupported channel count in OGG: " << outBuffer.channels << "\n"; return false; }
        alGenBuffers(1, reinterpret_cast<ALuint*>(&outBuffer.id));
        alBufferData(outBuffer.id, format, outBuffer.data.data(), (ALsizei)outBuffer.data.size(), outBuffer.sampleRate);
        ALenum err = alGetError();
        if(err != AL_NO_ERROR){ std::cerr << "[OpenAL] alBufferData error after OGG: "<<err<<"\n"; return false; }
        return true;
#else
        std::cerr << "[OpenAL] OGG support compiled out - USE_OPENAL not defined" << std::endl;
        return false;
#endif
    }

    std::ifstream f(filename, std::ios::binary);
    if (!f) {
        std::cerr << "[OpenAL] Failed to open WAV: " << filename << "\n";
        return false;
    }

    char riff[4];
    if (!readLE(f, riff, 4) || std::string(riff,4) != "RIFF") return false;
    uint32_t chunkSize=0; readLE(f, &chunkSize, 4);
    char wave[4]; readLE(f, wave, 4);
    if (std::string(wave,4) != "WAVE") return false;

    // Read fmt chunk
    char fmt[4]; readLE(f, fmt, 4);
    uint32_t fmtSize; readLE(f, &fmtSize, 4);
    uint16_t audioFormat=0; readLE(f, &audioFormat, 2);
    uint16_t numChannels=0; readLE(f, &numChannels, 2);
    uint32_t sampleRate=0; readLE(f, &sampleRate, 4);
    uint32_t byteRate=0; readLE(f, &byteRate, 4);
    uint16_t blockAlign=0; readLE(f, &blockAlign, 2);
    uint16_t bitsPerSample=0; readLE(f, &bitsPerSample, 2);
    if (fmtSize > 16) {
        // skip extra params
        f.seekg(fmtSize - 16, std::ios::cur);
    }

    // find data chunk
    char chunkId[4];
    uint32_t chunkLen=0;
    while (f && !(std::string(chunkId,4) == "data")) {
        if (!readLE(f, chunkId, 4)) break;
        readLE(f, &chunkLen, 4);
        if (std::string(chunkId,4) == "data") break;
        // skip unknown chunk
        f.seekg(chunkLen, std::ios::cur);
    }
    if (std::string(chunkId,4) != "data") {
        std::cerr << "[OpenAL] No data chunk in WAV\n";
        return false;
    }

    outBuffer.channels = numChannels;
    outBuffer.sampleRate = sampleRate;
    outBuffer.bitsPerSample = bitsPerSample;
    outBuffer.data.resize(chunkLen);
    f.read(reinterpret_cast<char*>(outBuffer.data.data()), chunkLen);
    if (f.gcount() != int(chunkLen)) {
        std::cerr << "[OpenAL] WAV read short\n";
        return false;
    }

#ifdef USE_OPENAL
    ALenum format = 0;
    if (outBuffer.channels == 1) {
        format = (outBuffer.bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else if (outBuffer.channels == 2) {
        format = (outBuffer.bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    } else {
        std::cerr << "[OpenAL] Unsupported channel count: " << outBuffer.channels << "\n";
        return false;
    }
    alGenBuffers(1, reinterpret_cast<ALuint*>(&outBuffer.id));
    alBufferData(outBuffer.id, format, outBuffer.data.data(), (ALsizei)outBuffer.data.size(), outBuffer.sampleRate);
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        std::cerr << "[OpenAL] alBufferData error: " << err << "\n";
        return false;
    }
#endif

    return true;
}

unsigned int OpenALAudio::playBuffer(const AudioBuffer& buffer, bool loop) {
#ifdef USE_OPENAL
    if (!buffer.id) return 0;
    ALuint src = 0;
    alGenSources(1, &src);
    alSourcei(src, AL_BUFFER, buffer.id);
    alSourcei(src, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(src);
    return (unsigned int)src;
#else
    return 0;
#endif
}

void OpenALAudio::stopSource(unsigned int source) {
#ifdef USE_OPENAL
    ALuint s = (ALuint)source;
    alSourceStop(s);
    alDeleteSources(1, &s);
#endif
}

void OpenALAudio::setListenerPosition(float x, float y, float z) {
#ifdef USE_OPENAL
    ALfloat pos[3] = {x,y,z};
    alListenerfv(AL_POSITION, pos);
#endif
}

} // namespace audio
} // namespace coretech



unsigned int OpenALAudio::startStreaming(const std::string& filename, bool loop){
#ifdef USE_OPENAL
    // open stb_vorbis stream
    int err = 0;
    stb_vorbis* v = stb_vorbis_open_filename(filename.c_str(), &err, nullptr);
    if(!v){
        std::cerr << "[OpenAL] stb_vorbis_open_filename failed: " << err << " file: " << filename << "\n";
        return 0;
    }
    // determine channels
    int channels = stb_vorbis_get_info(v).channels; // Note: this symbol may not be available in all stb builds; fallback will be 2
    if(channels <= 0) channels = 2;

    // create source and buffers
    ALuint src = 0;
    alGenSources(1, &src);
    const int NUM_BUFFERS = 4;
    const int CHUNK_SAMPLES = 4096; // samples per channel
    std::vector<ALuint> bufs(NUM_BUFFERS);
    alGenBuffers(NUM_BUFFERS, bufs.data());

    // prepare streaming source struct
    StreamingSource ss;
    ss.source = src;
    ss.buffers = std::vector<unsigned int>(bufs.begin(), bufs.end());
    ss.running = true;
    ss.stb_handle = v;

    // queue initial buffers
    std::vector<short> pcm(CHUNK_SAMPLES * channels);
    for(int i=0;i<NUM_BUFFERS;i++){
        int got = stb_vorbis_get_samples_short_interleaved(v, channels, pcm.data(), (int)pcm.size());
        if(got > 0){
            int samples = got * channels;
            alBufferData(bufs[i], (channels==1)?AL_FORMAT_MONO16:AL_FORMAT_STEREO16, pcm.data(), samples * sizeof(short), stb_vorbis_get_sample_rate(v));
        } else {
            // fill empty buffer with silence
            std::vector<short> silence(CHUNK_SAMPLES * channels);
            alBufferData(bufs[i], (channels==1)?AL_FORMAT_MONO16:AL_FORMAT_STEREO16, silence.data(), (int)(silence.size()*sizeof(short)), 44100);
        }
    }
    alSourceQueueBuffers(src, NUM_BUFFERS, bufs.data());
    alSourcePlay(src);

    // start streaming thread
    
ss.thread = std::thread([&ss, v]() mutable {
        const int CHUNK_SAMPLES = 4096;
        int channels = 2;
        // attempt to get channels/sample rate via stb_vorbis_get_info if available
        #ifdef USE_OPENAL
        try {
            channels = ss.stb_handle ? stb_vorbis_get_info((stb_vorbis*)ss.stb_handle).channels : 2;
        } catch(...) { channels = 2; }
        #endif
        std::vector<short> pcm(CHUNK_SAMPLES * channels);
        std::mutex local_mutex;
        std::condition_variable cv;
        bool local_running = true;
        while(true){
            {
                std::unique_lock<std::mutex> lk(ss.mutex);
                if(!ss.running){
                    break;
                }
            }
            // check processed buffers
            ALint processed = 0;
            alGetSourcei(ss.source, AL_BUFFERS_PROCESSED, &processed);
            if(processed > 0){
                std::vector<ALuint> recovered(processed);
                alSourceUnqueueBuffers(ss.source, processed, recovered.data());
                for(int i=0;i<processed;i++){
                    int got = stb_vorbis_get_samples_short_interleaved((stb_vorbis*)ss.stb_handle, channels, pcm.data(), (int)pcm.size());
                    if(got <= 0){
                        // reached end
                        if(!ss.running){
                            // do not loop and requested stop
                            // fill silence and requeue once to drain
                            std::vector<short> silence(CHUNK_SAMPLES * channels);
                            alBufferData(recovered[i], (channels==1)?AL_FORMAT_MONO16:AL_FORMAT_STEREO16, silence.data(), (int)(silence.size()*sizeof(short)), 44100);
                        } else {
                            // looping behavior: seek to start and decode again
                            stb_vorbis_seek_start((stb_vorbis*)ss.stb_handle);
                            got = stb_vorbis_get_samples_short_interleaved((stb_vorbis*)ss.stb_handle, channels, pcm.data(), (int)pcm.size());
                        }
                    }
                    int samples = std::max(0, got) * channels;
                    if(samples>0){
                        alBufferData(recovered[i], (channels==1)?AL_FORMAT_MONO16:AL_FORMAT_STEREO16, pcm.data(), samples * sizeof(short), stb_vorbis_get_sample_rate((stb_vorbis*)ss.stb_handle));
                    } else {
                        // fill with silence
                        std::vector<short> silence(CHUNK_SAMPLES * channels);
                        alBufferData(recovered[i], (channels==1)?AL_FORMAT_MONO16:AL_FORMAT_STEREO16, silence.data(), (int)(silence.size()*sizeof(short)), 44100);
                    }
                    alSourceQueueBuffers(ss.source, 1, &recovered[i]);
                }
            }
            ALint state;
            alGetSourcei(ss.source, AL_SOURCE_STATE, &state);
            if(state != AL_PLAYING){
                alSourcePlay(ss.source);
            }
            // wait for a short time or until stop notified
            {
                std::unique_lock<std::mutex> lk(ss.mutex);
                if(!ss.running) break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        // cleanup
        alSourceStop(ss.source);
        ALint queued=0;
        alGetSourcei(ss.source, AL_BUFFERS_QUEUED, &queued);
        std::vector<ALuint> qbufs(queued);
        if(queued>0) alSourceUnqueueBuffers(ss.source, queued, qbufs.data());
        alDeleteSources(1, &ss.source);
        for(ALuint b: ss.buffers) alDeleteBuffers(1, &b);
        if(ss.stb_handle) stb_vorbis_close((stb_vorbis*)ss.stb_handle);
    });


    // store in global map
    g_streamingSources[src] = std::move(ss);
    return src;
#else
    (void)filename; (void)loop;
    return 0;
#endif
}

void OpenALAudio::stopStreaming(unsigned int source){
#ifdef USE_OPENAL
    auto it = g_streamingSources.find(source);
    if(it == g_streamingSources.end()) return;
    StreamingSource &ss = it->second;
    ss.running = false;
    if(ss.thread.joinable()) ss.thread.join();
    // thread will cleanup buffers/sources
    g_streamingSources.erase(it);
#else
    (void)source;
#endif
}
