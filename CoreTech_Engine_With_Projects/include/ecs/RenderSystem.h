
#pragma once
#include "System.h"
#include "RenderComponents.h"
#include "ComponentManager.h"

class RenderSystem : public System {
public:
    void update(float dt, ComponentManager& comps) override;
};
