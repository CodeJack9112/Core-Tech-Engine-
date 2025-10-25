// Example code showing how to create a ground plane and a falling cube using PhysicsManager API.
// Include this in your GameEngine initialization to spawn a demo scene.
#include "../include/PhysicsManager.h"
#include <btBulletDynamicsCommon.h>

void createDemoScene(PhysicsManager& phys){
    // Ground: static box centered at origin with large X,Z extents, thin Y
    btTransform groundTx; groundTx.setIdentity(); groundTx.setOrigin(btVector3(0,-0.5,0));
    phys.createStaticBox(btVector3(50,0.5f,50), groundTx);

    // Dynamic cube: half extents 0.5 (1x1 cube), placed above ground
    btTransform cubeTx; cubeTx.setIdentity(); cubeTx.setOrigin(btVector3(0,5,0));
    phys.createDynamicBox(btVector3(0.5f,0.5f,0.5f), cubeTx, 1.0f);
}
