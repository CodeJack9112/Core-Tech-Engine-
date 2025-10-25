#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
namespace CoreTech {
struct SimpleMesh { std::vector<float> vertices; std::vector<unsigned int> indices; std::vector<float> normals; std::vector<float> uvs; std::vector<int> boneIds; std::vector<float> boneWeights; };
class AssetManager {
public:
    static bool loadOBJ(const std::string& path, SimpleMesh& out);
    static unsigned int loadTexture(const std::string& path); // returns GL texture or 0
};
}