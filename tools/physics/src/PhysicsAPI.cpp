#include "PhysicsAPI.h"
#include <iostream>
#ifdef HAVE_BULLET
#include <btBulletDynamicsCommon.h>
extern btDiscreteDynamicsWorld* g_dynamicsWorld; // assumed global from PhysicsSystem (prototype)
#endif
namespace CoreTech {
namespace PhysicsAPI {
    bool raycast(const glm::vec3& origin, const glm::vec3& dir, float maxDist, glm::vec3& hitPoint, unsigned int& hitEntityId){
#ifdef HAVE_BULLET
        if(!g_dynamicsWorld) return false;
        btVector3 o(origin.x, origin.y, origin.z);
        btVector3 d(dir.x, dir.y, dir.z);
        btVector3 to = o + d*maxDist;
        btCollisionWorld::ClosestRayResultCallback cb(o,to);
        g_dynamicsWorld->rayTest(o,to,cb);
        if(cb.hasHit()){
            hitPoint = glm::vec3(cb.m_hitPointWorld.x(), cb.m_hitPointWorld.y(), cb.m_hitPointWorld.z());
            // user pointer mapping to entity not implemented here
            hitEntityId = 0;
            return true;
        }
        return false;
#else
        (void)origin;(void)dir;(void)maxDist;(void)hitPoint;(void)hitEntityId;
        return false;
#endif
    }
    static void(*g_triggerCb)(unsigned int,bool) = nullptr;
    void setTriggerCallback(void(*cb)(unsigned int, bool)){ g_triggerCb = cb; }
}
}
