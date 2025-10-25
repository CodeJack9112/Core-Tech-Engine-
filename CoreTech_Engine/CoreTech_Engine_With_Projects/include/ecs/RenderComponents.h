
#pragma once
#include "Component.h"
#include <string>
#include <glm/glm.hpp>

struct MeshComponent : public IComponent {
    std::string meshPath;
};

struct CameraComponent : public IComponent {
    glm::vec3 position{0,0,0};
    glm::vec3 forward{0,0,-1};
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
};
