#pragma once
#include <memory>
#include "Entity.h"

namespace CoreTech {

struct PhysicsComponent : public Component {
    float mass = 0.0f;
    bool dynamic = false;
    // simple shape hint
    enum Shape { BOX, SPHERE, CONVEX } shape = BOX;
    // user-facing API
    std::string getType() const override { return "Physics"; }
    PhysicsComponent() = default;
};

class PhysicsSystem {
public:
    PhysicsSystem();
    ~PhysicsSystem();
    void init();
    void step(float dt);
    void addEntity(std::shared_ptr<Entity> e);
private:
    struct Impl;
    Impl* p;
};

} // namespace CoreTech
