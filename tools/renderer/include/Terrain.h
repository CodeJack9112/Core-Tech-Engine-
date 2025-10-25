#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace CoreTech {
struct TerrainMesh { std::vector<float> vertices; std::vector<float> normals; std::vector<float> uvs; std::vector<unsigned int> indices; int width=0; int height=0; };
class Terrain {
public:
    static bool loadHeightmap(const std::string& path, int& w, int& h, std::vector<float>& data);
    static TerrainMesh generateMesh(const std::vector<float>& heightmap, int w, int h, float scale=1.0f, int skip=1);
    static void computeNormals(TerrainMesh& m);
};
}