#pragma once
#include "ecs/System.h"
#include "ecs/TransformComponent.h"

class MovementSystem : public System {
public:
    // moves entities with TransformComponent along +X axis
    void update(float dt, ComponentManager& comps) override;
};
