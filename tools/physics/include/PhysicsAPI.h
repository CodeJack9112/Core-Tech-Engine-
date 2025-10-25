#pragma once
#include <glm/glm.hpp>
namespace CoreTech {
namespace PhysicsAPI {
    bool raycast(const glm::vec3& origin, const glm::vec3& dir, float maxDist, glm::vec3& hitPoint, unsigned int& hitEntityId);
    void setTriggerCallback(void(*cb)(unsigned int entityId, bool entered));
}
}