
#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct MeshData {
    std::vector<float> vertices; // interleaved: pos(3), normal(3), uv(2)
    uint32_t vertexCount = 0;
};

class MeshLoader {
public:
    // Load a very simple OBJ (positions, normals, texcoords, faces). Returns true on success.
    static bool loadOBJ(const std::string& path, MeshData& out);
};
