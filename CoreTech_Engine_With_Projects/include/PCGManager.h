#pragma once
#include <vector>
#include <memory>
#include <random>
#include <btBulletDynamicsCommon.h>
class PhysicsManager;

// Minimal PCG header (kept for compatibility)
class PCGManager {
public:
    PCGManager(unsigned int seed=0);
    ~PCGManager();

    std::vector<std::shared_ptr<void>> generateBoxGrid(int nx, int nz, float spacing, const btVector3& halfExtents, float baseHeight, float heightVariation, PhysicsManager& phys);
    void clear();

private:
    std::mt19937_64 m_rng;
};
