#pragma once
#include <glm/glm.hpp>
#include <string>
namespace CoreTech {
struct DirectionalLightComponent : public Component {
    glm::vec3 direction = glm::vec3(-0.5f,-1.0f,-0.3f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    std::string getType() const override { return "DirectionalLight"; }
};
}