#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace coretech {
struct Mesh { std::vector<glm::vec3> positions; std::vector<glm::vec3> normals; std::vector<glm::vec2> uvs; std::vector<unsigned int> indices; };

// Simplify using a basic QEM (Quadric Error Metrics) implementation.
// targetVertexCount: desired number of vertices; returns simplified mesh.
Mesh SimplifyQEM(const Mesh& inMesh, unsigned int targetVertexCount);
Mesh SimplifyQEMOptimized(const Mesh& inMesh, unsigned int targetVertexCount);

}
