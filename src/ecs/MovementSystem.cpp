
#include "ecs/System.h"
#include "ecs/TransformComponent.h"
#include <iostream>

class MovementSystem : public System {
public:
    // moves entities with TransformComponent along +X axis
    void update(float dt, ComponentManager& comps) override {
        comps.forEach<TransformComponent>([dt](uint32_t ent, TransformComponent& t){
            t.position.x += 1.0f * dt; // simple constant velocity
        });
    }
};
