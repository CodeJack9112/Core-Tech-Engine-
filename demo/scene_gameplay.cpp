// Demo: gameplay PCG scene with building, loot, spawns, and a dynamic event.
#include "../include/GameplayPCGManager.h"
#include "../include/PhysicsManager.h"
#include <btBulletDynamicsCommon.h>
#include "../include/NavMesh.h"
#include "../include/PickupManager.h"
#include "../include/DebugUI.h"
#include <iostream>

void createGameplayScene(PhysicsManager& phys){
    // ground
    btTransform groundTx; groundTx.setIdentity(); groundTx.setOrigin(btVector3(0,-0.5,0));
    phys.createStaticBox(btVector3(50,0.5f,50), groundTx);

    GameplayPCGManager gpcg(12345);

    // create a simple kinematic player marker at origin
    btTransform ptx; ptx.setIdentity(); ptx.setOrigin(btVector3(0,1.0f,-5.0f));
    auto player = phys.createDynamicSphere(0.4f, ptx, 0.0f);
    if(player) player->setKinematic(true);
    if(player) player->setUserTag(PhysicsComponent::TAG_PLAYER);

    // register pickup callback to remove pickup when player collides
    phys.registerPickupCallback([&phys](std::shared_ptr<PhysicsComponent> pickup, std::shared_ptr<PhysicsComponent> other){
        std::cout << "Pickup collected!" << std::endl;
        if(pickup) phys.removeComponent(pickup);
    });

    // create a building at center
    gpcg.generateBuilding(btVector3(0,2,0), 4, 3, 4.0f, phys);
    // place loot inside building area
    gpcg.placeLoot(btVector3(-8,0.5f,-6), btVector3(8,3.0f,6), 12, phys);
    // create enemy spawn points
    auto sp = gpcg.createEnemySpawns(btVector3(-10,0.5f,-10), btVector3(10,3.0f,10), 6, phys);
    // trigger a supply drop near one spawn
    if(!sp.empty()) gpcg.dropSupplyCrate(sp[0], phys);

    // Example: generate navmesh nodes
    auto nodes = gpcg.generateNavMeshNodes(btVector3(-20,0,-20), btVector3(20,10,20), 2.0f, phys);
    std::cout << "Nav nodes: " << nodes.size() << std::endl;
    // build navmesh graph
    NavMesh nm;
    nm.build(nodes, 3.0f);
    // sample a path from player to a node (if nodes exist)
    if(!nodes.empty()){
        btVector3 start = player ? player->getTransform().getOrigin() : btVector3(0,0,0);
        auto path = nm.findPath(start, nodes.back());
        std::cout << "Path nodes: " << path.size() << std::endl;
    }
    // show debug UI info (prints if ImGui disabled)
    unsigned int seed = 0; bool regenRequested = false;
    DebugUI::showPickupLog(pm);
    DebugUI::showNavMesh(nm);

}
