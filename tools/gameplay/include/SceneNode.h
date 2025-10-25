#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <string>

namespace CoreTech {
class Entity;
class Scene;
struct SceneNode {
    SceneNode() = default;
    SceneNode(const std::string& name): name(name) {}
    std::string name = "node";
    glm::vec3 localPosition = glm::vec3(0.0f);
    glm::vec3 localRotation = glm::vec3(0.0f); // Euler degrees
    glm::vec3 localScale = glm::vec3(1.0f);
    std::weak_ptr<SceneNode> parent;
    std::vector<std::shared_ptr<SceneNode>> children;
    std::weak_ptr<Entity> entity; // optional entity attached
    glm::mat4 worldMatrix() const; // computed transform
    void addChild(std::shared_ptr<SceneNode> child);
};
}