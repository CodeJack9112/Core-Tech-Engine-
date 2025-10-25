#pragma once
#include "Entity.h"
#include <glm/glm.hpp>
namespace CoreTech {
struct LightComponent : public Component {
    enum Type { DIRECTIONAL=0, POINT=1, SPOT=2 } type = DIRECTIONAL;
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float range = 10.0f; // for point/spot
    float spotAngle = 45.0f; // degrees for spot
    std::string getType() const override { return "Light"; }
};
}