    #pragma once
    #include <vector>

    struct MeshOptimizer {
    // Reorders triangle indices to improve post-transform vertex cache locality.
    // Implementation uses Tom Forsyth algorithm.
    static void OptimizeIndices(std::vector<unsigned int>& indices, unsigned int vertexCount, unsigned int cacheSize = 32);
};
