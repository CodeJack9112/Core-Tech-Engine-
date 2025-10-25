#include "PhysicsComponent.h"
#include <vector>
#include <iostream>
#ifdef __has_include
  #if __has_include(<btBulletDynamicsCommon.h>)
    #define HAVE_BULLET 1
  #endif
#endif
#ifdef HAVE_BULLET
#include <btBulletDynamicsCommon.h>
#endif

namespace CoreTech {

struct PhysicsSystem::Impl {
#ifdef HAVE_BULLET
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> world;
    std::vector<std::unique_ptr<btCollisionShape>> shapes;
#endif
    std::vector<std::shared_ptr<Entity>> entities;
};

PhysicsSystem::PhysicsSystem(): p(new Impl()){}
PhysicsSystem::~PhysicsSystem(){}

void PhysicsSystem::init(){
#ifdef HAVE_BULLET
    p->broadphase = std::make_unique<btDbvtBroadphase>();
    p->collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
    p->dispatcher = std::make_unique<btCollisionDispatcher>(p->collisionConfig.get());
    p->solver = std::make_unique<btSequentialImpulseConstraintSolver>();
    p->world = std::make_unique<btDiscreteDynamicsWorld>(p->dispatcher.get(), p->broadphase.get(), p->solver.get(), p->collisionConfig.get());
    p->world->setGravity(btVector3(0,-9.81f,0));
    std::cout << "Bullet physics initialized\n";
#else
    std::cout << "Bullet not available - physics disabled at runtime\n";
#endif
}

void PhysicsSystem::step(float dt){
#ifdef HAVE_BULLET
    if(p->world) p->world->stepSimulation(dt, 10);
#endif
    // Update transforms from physics (not implemented in stub)
}


void PhysicsSystem::addEntity(std::shared_ptr<Entity> e){
    p->entities.push_back(e);
#ifdef HAVE_BULLET
    // create a rigid body based on PhysicsComponent and TransformComponent
    auto phys = e->getComponent<PhysicsComponent>();
    auto trans = e->getComponent<TransformComponent>();
    if(!phys){
        return;
    }
    float mass = phys->mass;
    bool isDynamic = phys->dynamic;
    btCollisionShape* shape = nullptr;
    if(phys->shape == PhysicsComponent::BOX){
        // box of size 1 scaled by transform scale
        btVector3 halfExtents(0.5f * trans->sx, 0.5f * trans->sy, 0.5f * trans->sz);
        shape = new btBoxShape(halfExtents);
    } else if(phys->shape == PhysicsComponent::SPHERE){
        float radius = 0.5f * std::max({trans->sx, trans->sy, trans->sz});
        shape = new btSphereShape(radius);
    } else {
        // fallback to box
        btVector3 halfExtents(0.5f * trans->sx, 0.5f * trans->sy, 0.5f * trans->sz);
        shape = new btBoxShape(halfExtents);
    }
    // store shape ownership
    p->shapes.emplace_back(shape);

    // transform
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(trans->x, trans->y, trans->z));

    btScalar btMass = isDynamic ? btScalar(mass) : btScalar(0.0);
    bool isDynamicBody = (btMass != 0.0);
    btVector3 localInertia(0,0,0);
    if(isDynamicBody){
        shape->calculateLocalInertia(btMass, localInertia);
    }

    // motion state
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(btMass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    body->setUserIndex(static_cast<int>(e->getId()));

    p->world->addRigidBody(body);

    // store entry
    auto entry = std::make_unique<RigidBodyEntry>();
    entry->shape.reset(shape);
    entry->motionState.reset(motionState);
    // Note: we wrap the raw pointer body into unique_ptr with custom deleter to call world->removeRigidBody before delete
    struct RBDeleter { btDiscreteDynamicsWorld* world; void operator()(btRigidBody* b){ if(world) world->removeRigidBody(b); delete b; } };
    entry->body.reset(body);
    entry->entityId = e->getId();
    p->rigidBodies[e->getId()] = std::move(entry);
#endif
}


} // namespace CoreTech
