#pragma once
#include "Engine/Engine.h"
#include <iostream>
#include "ecs/ComponentManager.h"
#include "ecs/RenderableComponent.h"
#include "ecs/TransformComponent.h"

namespace coretech {

class RendererModule : public EngineModule {
public:
    RendererModule() = default;
    bool init() override {
        std::cout << "[RendererModule] init\n";
        return true;
    }
    void shutdown() override {
        std::cout << "[RendererModule] shutdown\n";
    }
    const char* name() const override { return "Renderer"; }

    // Set a component manager to query renderables from
    void setComponentManager(ComponentManager* cm) { compMgr_ = cm; }

    void renderFrame() {
        if(!compMgr_) { std::cout << "[RendererModule] No ComponentManager set\n"; return; }
        // iterate renderable components and get their transform
        compMgr_->forEach<RenderableComponent>([this](uint32_t ent, RenderableComponent& rc){
            TransformComponent* tc = compMgr_->getComponent<TransformComponent>(ent);
            if(tc) {
                std::cout << "[Renderer] Entity " << ent << " mesh=" << rc.meshPath << " mat=" << rc.material
                          << " pos=(" << tc->position.x << "," << tc->position.y << "," << tc->position.z << ")\n";
            } else {
                std::cout << "[Renderer] Entity " << ent << " mesh=" << rc.meshPath << " mat=" << rc->material << " (no transform)\n";
            }
        });
    }

private:
    ComponentManager* compMgr_ = nullptr;
};

} // namespace coretech
