#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace coretech {

struct Mesh {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;
  std::vector<unsigned int> indices; // triangles (3-per)
};

// Simplify a mesh using vertex clustering. `gridSize` is the cell size in world units.
// Returns simplified mesh.
Mesh SimplifyByVertexClustering(const Mesh& inMesh, float gridSize);

// Convenience: load/save OBJ (simple, supports positions, normals, uvs, triangles)
bool LoadOBJ(const std::string& path, Mesh& outMesh);
bool SaveOBJ(const std::string& path, const Mesh& mesh);

} // namespace coretech
