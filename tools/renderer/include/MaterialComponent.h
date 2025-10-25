#pragma once
#include <string>
#include <glm/glm.hpp>

namespace CoreTech {
struct MaterialComponent : public Component {
    std::string shader = "default";
    glm::vec3 albedo = glm::vec3(1.0f);
    float metallic = 0.0f;
    float roughness = 1.0f;
    std::string albedoTexture;
    std::string name = "material";
    std::string getType() const override { return "Material"; }
};
}