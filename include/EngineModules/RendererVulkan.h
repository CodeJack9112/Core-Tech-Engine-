#pragma once
#include "Engine/Engine.h"
#include <iostream>

namespace coretech {

class RendererVulkan : public EngineModule {
public:
    RendererVulkan() = default;
    ~RendererVulkan() = default;
    bool init() override;
    void shutdown() override;
    const char* name() const override { return "RendererVulkan"; }
    void onRender() override;
};

} // namespace coretech
