#pragma once
#include <vector>
#include <memory>
#include <random>
#include <btBulletDynamicsCommon.h>
#include "PhysicsComponent.h"
class PhysicsManager;

// Gameplay-oriented PCG: buildings, loot, enemy spawn zones, dynamic events.
class GameplayPCGManager {
public:
    GameplayPCGManager(unsigned int seed=0);
    ~GameplayPCGManager();

    std::shared_ptr<PhysicsComponent> generateBuilding(const btVector3& origin, int roomsX, int roomsZ, float roomSize, PhysicsManager& phys);
    std::vector<std::shared_ptr<PhysicsComponent>> placeLoot(const btVector3& minBounds, const btVector3& maxBounds, int count, PhysicsManager& phys);
    std::vector<btVector3> createEnemySpawns(const btVector3& minBounds, const btVector3& maxBounds, int count, PhysicsManager& phys);
    std::shared_ptr<PhysicsComponent> dropSupplyCrate(const btVector3& pos, PhysicsManager& phys);
    void clear();
    void destroyAll(PhysicsManager& phys);
    std::vector<btVector3> generateNavMeshNodes(const btVector3& minBounds, const btVector3& maxBounds, float cellSize, PhysicsManager& phys);

private:
    unsigned int m_seed;
    std::mt19937_64 m_rng;
    std::vector<std::shared_ptr<PhysicsComponent>> m_tracked;
};
