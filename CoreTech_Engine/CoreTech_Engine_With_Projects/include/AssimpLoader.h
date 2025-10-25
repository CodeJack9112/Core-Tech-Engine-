
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>

struct GLMesh {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    unsigned int indexCount = 0;
    int materialIndex = -1;
};

struct Material {
    unsigned int albedo = 0; // RGBA
    unsigned int normal = 0; // normal map
    unsigned int orm = 0;    // occlusion(R), roughness(G), metallic(B)
};

struct Model {
    std::vector<GLMesh> meshes;
    std::vector<Material> materials;
    std::string directory; // original model directory for resolving relative textures
};

class AssimpLoader {
public:
    AssimpLoader() = default;
    ~AssimpLoader();

    // Load model file and return pointer to cached Model (contains one or more GLMesh entries).
    Model* loadModel(const std::string& path);

    // Release all loaded models
    void clear();

private:
    std::unordered_map<std::string, Model> m_cache;
};
