#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
namespace CoreTech {
struct SimpleMesh { std::vector<float> vertices; std::vector<unsigned int> indices; };
bool loadMeshFromFile(const std::string& path, SimpleMesh& out);
}