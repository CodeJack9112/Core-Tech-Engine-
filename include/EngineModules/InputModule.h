#pragma once
#include "Engine/Engine.h"
#include <functional>
#include <vector>

namespace coretech {

class InputModule : public EngineModule {
public:
    InputModule() = default;
    bool init() override;
    void shutdown() override;
    const char* name() const override { return "Input"; }
    using KeyCallback = std::function<void(int,int)>;
    void addKeyCallback(KeyCallback cb);
    void pollEvents();
private:
    struct Impl;
    Impl* impl_ = nullptr;
};

} // namespace coretech
