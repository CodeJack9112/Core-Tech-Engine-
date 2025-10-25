
#pragma once
#include "System.h"
#include "TransformComponent.h"
#include "ComponentManager.h"

class PhysicsSystem : public System {
public:
    PhysicsSystem() = default;
    void update(float dt, ComponentManager& comps) override;
    void setGravity(float g) { gravity = g; }
private:
    float gravity = -9.8f;
};
