#include "EngineModules/AudioModule.h"
#include <iostream>

#ifdef ENABLE_OPENAL
  #include <AL/al.h>
  #include <AL/alc.h>
#endif

namespace coretech {

struct AudioModule::Impl {
#ifdef ENABLE_OPENAL
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
#endif
    bool initialized = false;
};

bool AudioModule::init() {
    std::cout << "[AudioModule] init\n";
#ifdef ENABLE_OPENAL
    impl_ = new Impl();
    impl_->device = alcOpenDevice(NULL);
    if(!impl_->device) {
        std::cout << "OpenAL device open failed\n";
        delete impl_;
        impl_ = nullptr;
        return false;
    }
    impl_->context = alcCreateContext(impl_->device, NULL);
    alcMakeContextCurrent(impl_->context);
    impl_->initialized = true;
    std::cout << "[AudioModule] OpenAL initialized\n";
#else
    std::cout << "[AudioModule] OpenAL not enabled; audio disabled\n";
#endif
    return true;
}

void AudioModule::shutdown() {
    std::cout << "[AudioModule] shutdown\n";
#ifdef ENABLE_OPENAL
    if(impl_) {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(impl_->context);
        alcCloseDevice(impl_->device);
        delete impl_;
        impl_ = nullptr;
    }
#endif
}

void AudioModule::playTestSound() {
#ifdef ENABLE_OPENAL
    // Placeholder: would queue a buffer/source here
    std::cout << "[AudioModule] playTestSound (OpenAL)\n";
#else
    std::cout << "[AudioModule] playTestSound (stub)\n";
#endif
}

} // namespace coretech
