#include "../include/PCGManager.h"
#include "../include/PhysicsManager.h"
#include <chrono>
#include <iostream>

PCGManager::PCGManager(unsigned int seed){
    if(seed == 0) seed = (unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    m_rng.seed(seed);
}
PCGManager::~PCGManager(){ clear(); }
void PCGManager::clear(){}
std::vector<std::shared_ptr<void>> PCGManager::generateBoxGrid(int nx, int nz, float spacing, const btVector3& halfExtents, float baseHeight, float heightVariation, PhysicsManager& phys){
    std::vector<std::shared_ptr<void>> result;
    std::uniform_real_distribution<float> dist(-heightVariation, heightVariation);
    float startX = -((nx-1) * spacing) * 0.5f;
    float startZ = -((nz-1) * spacing) * 0.5f;
    for(int ix=0; ix<nx; ++ix){
        for(int iz=0; iz<nz; ++iz){
            float x = startX + ix * spacing;
            float z = startZ + iz * spacing;
            float y = baseHeight + dist(m_rng);
            btTransform tx; tx.setIdentity(); tx.setOrigin(btVector3(x,y,z));
            auto comp = phys.createDynamicBox(halfExtents, tx, 1.0f);
            result.push_back(std::static_pointer_cast<void>(comp));
        }
    }
    std::cout << "PCG: spawned " << result.size() << " boxes\n";
    return result;
}
