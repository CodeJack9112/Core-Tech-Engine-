#include "EngineModules/PhysicsModule.h"
#include <iostream>

#ifdef ENABLE_BULLET
  #include <btBulletDynamicsCommon.h>
#endif

namespace coretech {

struct PhysicsModule::Impl {
#ifdef ENABLE_BULLET
    btDiscreteDynamicsWorld* world = nullptr;
    btBroadphaseInterface* broadphase = nullptr;
    btDefaultCollisionConfiguration* collisionConfig = nullptr;
    btCollisionDispatcher* dispatcher = nullptr;
    btSequentialImpulseConstraintSolver* solver = nullptr;
#endif
    bool initialized = false;
};

bool PhysicsModule::init() {
    std::cout << "[PhysicsModule] init\n";
#ifdef ENABLE_BULLET
    impl_ = new Impl();
    impl_->broadphase = new btDbvtBroadphase();
    impl_->collisionConfig = new btDefaultCollisionConfiguration();
    impl_->dispatcher = new btCollisionDispatcher(impl_->collisionConfig);
    impl_->solver = new btSequentialImpulseConstraintSolver();
    impl_->world = new btDiscreteDynamicsWorld(impl_->dispatcher, impl_->broadphase, impl_->solver, impl_->collisionConfig);
    impl_->initialized = true;
    std::cout << "[PhysicsModule] Bullet initialized\n";
#else
    std::cout << "[PhysicsModule] Bullet not enabled; physics disabled\n";
#endif
    return true;
}

void PhysicsModule::shutdown() {
    std::cout << "[PhysicsModule] shutdown\n";
#ifdef ENABLE_BULLET
    if(impl_) {
        delete impl_->world;
        delete impl_->solver;
        delete impl_->dispatcher;
        delete impl_->collisionConfig;
        delete impl_->broadphase;
        delete impl_;
        impl_ = nullptr;
    }
#endif
}

void PhysicsModule::stepSimulation(float dt) {
#ifdef ENABLE_BULLET
    if(impl_ && impl_->initialized) impl_->world->stepSimulation(dt);
#else
    (void)dt;
#endif
}

} // namespace coretech
