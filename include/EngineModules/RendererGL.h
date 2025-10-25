#pragma once
#include "Engine/Engine.h"
#include <memory>

namespace coretech {

class RendererGL : public EngineModule {
public:
    RendererGL();
    ~RendererGL();
    bool init() override;
    void shutdown() override;
    const char* name() const override { return "RendererGL"; }
    void onRender() override;
    bool hasWindow() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace coretech
