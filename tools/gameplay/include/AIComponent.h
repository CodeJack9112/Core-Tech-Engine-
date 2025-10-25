#pragma once
#include "Entity.h"
#include <string>
namespace CoreTech {
struct AIComponent : public Component {
    enum State { IDLE=0, SEEK=1 } state = IDLE;
    unsigned int targetId = 0; // entity to seek
    float speed = 2.0f;
    std::string getType() const override { return "AI"; }
};
class AISystem {
public:
    void update(std::shared_ptr<Scene> scene, float dt);
};
}