#pragma once
#include "Engine/Engine.h"
#include <iostream>

namespace coretech {

class PhysicsModule : public EngineModule {
public:
    PhysicsModule() = default;
    bool init() override;
    void shutdown() override;
    const char* name() const override { return "Physics"; }
    void stepSimulation(float dt);
private:
    struct Impl;
    Impl* impl_ = nullptr;
};

} // namespace coretech
