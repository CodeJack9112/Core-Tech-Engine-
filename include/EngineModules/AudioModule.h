#pragma once
#include "Engine/Engine.h"
#include <iostream>

namespace coretech {

class AudioModule : public EngineModule {
public:
    AudioModule() = default;
    bool init() override;
    void shutdown() override;
    const char* name() const override { return "Audio"; }
    void playTestSound(); // stub/play if OpenAL available
private:
    struct Impl;
    Impl* impl_ = nullptr;
};

} // namespace coretech
