#include "../include/GameplayPCGManager.h"
#include "../include/PhysicsManager.h"
#include <chrono>
#include <random>
#include <iostream>

GameplayPCGManager::GameplayPCGManager(unsigned int seed){
    if(seed == 0) seed = (unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    m_seed = seed; m_rng.seed(seed);
}
GameplayPCGManager::~GameplayPCGManager(){ clear(); }

void GameplayPCGManager::clear(){ m_tracked.clear(); }

std::shared_ptr<PhysicsComponent> GameplayPCGManager::generateBuilding(const btVector3& origin, int roomsX, int roomsZ, float roomSize, PhysicsManager& phys){
    float halfW = (roomsX * roomSize) * 0.5f;
    float halfD = (roomsZ * roomSize) * 0.5f;
    btTransform tx; tx.setIdentity(); tx.setOrigin(origin);
    auto shell = phys.createStaticBox(btVector3(halfW + 1.0f, 2.5f, halfD + 1.0f), tx);
    m_tracked.push_back(shell);
    for(int x=0;x<roomsX;x++){
        for(int z=0;z<roomsZ;z++){
            btTransform rtx; rtx.setIdentity();
            float px = origin.getX() - halfW + x*roomSize + roomSize*0.5f;
            float pz = origin.getZ() - halfD + z*roomSize + roomSize*0.5f;
            rtx.setOrigin(btVector3(px, origin.getY() - 1.0f, pz));
            auto floor = phys.createStaticBox(btVector3(roomSize*0.45f, 0.1f, roomSize*0.45f), rtx);
            m_tracked.push_back(floor);
        }
    }
    return shell;
}

std::vector<std::shared_ptr<PhysicsComponent>> GameplayPCGManager::placeLoot(const btVector3& minBounds, const btVector3& maxBounds, int count, PhysicsManager& phys){
    std::vector<std::shared_ptr<PhysicsComponent>> result;
    std::uniform_real_distribution<float> dx(minBounds.getX(), maxBounds.getX());
    std::uniform_real_distribution<float> dy(minBounds.getY(), maxBounds.getY());
    std::uniform_real_distribution<float> dz(minBounds.getZ(), maxBounds.getZ());
    for(int i=0;i<count;i++){
        btVector3 pos(dx(m_rng), dy(m_rng), dz(m_rng));
        btTransform tx; tx.setIdentity(); tx.setOrigin(pos);
        auto crate = phys.createDynamicBox(btVector3(0.3f,0.3f,0.3f), tx, 1.0f);
        if(crate) crate->setUserTag(PhysicsComponent::TAG_PICKUP);
        result.push_back(crate);
        m_tracked.push_back(crate);
    }
    std::cout << "Placed " << result.size() << " loot crates\n";
    return result;
}

std::vector<btVector3> GameplayPCGManager::createEnemySpawns(const btVector3& minBounds, const btVector3& maxBounds, int count, PhysicsManager& phys){
    std::vector<btVector3> spawns;
    std::uniform_real_distribution<float> dx(minBounds.getX(), maxBounds.getX());
    std::uniform_real_distribution<float> dy(minBounds.getY(), maxBounds.getY());
    std::uniform_real_distribution<float> dz(minBounds.getZ(), maxBounds.getZ());
    for(int i=0;i<count;i++){
        btVector3 pos(dx(m_rng), dy(m_rng), dz(m_rng));
        btTransform tx; tx.setIdentity(); tx.setOrigin(pos);
        auto s = phys.createDynamicSphere(0.2f, tx, 0.0f);
        if(s){ s->setKinematic(true); m_tracked.push_back(s); }
        spawns.push_back(pos);
    }
    std::cout << "Created " << spawns.size() << " enemy spawn points\n";
    return spawns;
}

std::shared_ptr<PhysicsComponent> GameplayPCGManager::dropSupplyCrate(const btVector3& pos, PhysicsManager& phys){
    btTransform tx; tx.setIdentity();
    tx.setOrigin(btVector3(pos.getX(), pos.getY()+20.0f, pos.getZ()));
    auto crate = phys.createDynamicBox(btVector3(0.5f,0.5f,0.5f), tx, 5.0f);
    if(crate && crate->body()){
        crate->body()->applyCentralImpulse(btVector3(0, -10.0f, 0));
    }
    m_tracked.push_back(crate);
    std::cout << "Dropped supply crate at ("<<pos.getX()<<","<<pos.getY()<<","<<pos.getZ()<<")\n";
    return crate;
}

void GameplayPCGManager::destroyAll(PhysicsManager& phys){
    for(auto &c : m_tracked){
        if(c) phys.removeComponent(c);
    }
    m_tracked.clear();
}

std::vector<btVector3> GameplayPCGManager::generateNavMeshNodes(const btVector3& minBounds, const btVector3& maxBounds, float cellSize, PhysicsManager& phys){
    return phys.generateNavMesh(minBounds, maxBounds, cellSize);
}
