#pragma once
// Simple Bullet physics manager wrapper
// Requires Bullet (find_package(BULLET REQUIRED) in CMake)
#include <memory>
#include <vector>
#include <glm/glm.hpp> // optional; if not used, math types can be replaced
#include <btBulletDynamicsCommon.h>
#include <functional>
#include <unordered_map>

class PhysicsComponent;

class PhysicsManager {
public:
    PhysicsManager();
    ~PhysicsManager();

    bool initialize();
    void shutdown();

    // step physics with fixed time-step. Call every frame.
    void stepSimulation(double dt);

    // create simple collision objects
    std::shared_ptr<PhysicsComponent> createStaticBox(const btVector3& halfExtents, const btTransform& transform);
    std::shared_ptr<PhysicsComponent> createDynamicBox(const btVector3& halfExtents, const btTransform& transform, float mass);

    // debug draw toggle
    void enableDebugDraw(bool enable);
    bool debugDrawEnabled() const;

    // access to world for advanced usage
    btDiscreteDynamicsWorld* dynamicsWorld() { return m_dynamicsWorld; }

    using PickupCallback = std::function<void(std::shared_ptr<PhysicsComponent> pickup, std::shared_ptr<PhysicsComponent> other)>;
    void registerPickupCallback(PickupCallback cb);
    void processCollisionCallbacks();
    std::vector<btVector3> generateNavMesh(const btVector3& minBounds, const btVector3& maxBounds, float cellSize);

private:
    btBroadphaseInterface* m_broadphase = nullptr;
    btDefaultCollisionConfiguration* m_collisionConfiguration = nullptr;
    btCollisionDispatcher* m_dispatcher = nullptr;
    btSequentialImpulseConstraintSolver* m_solver = nullptr;
    btDiscreteDynamicsWorld* m_dynamicsWorld = nullptr;

    bool m_debugDraw = false;

    std::unordered_map<PhysicsComponent*, std::weak_ptr<PhysicsComponent>> m_componentMap;
    PickupCallback m_pickupCallback;
};
