#include "../include/PhysicsManager.h"
#include "../include/PhysicsComponent.h"
#include <unordered_map>
#include <sstream>
#include <iostream>

PhysicsManager::PhysicsManager(){}

PhysicsManager::~PhysicsManager(){ shutdown(); }

bool PhysicsManager::initialize(){
    m_broadphase = new btDbvtBroadphase();
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
    if(!m_dynamicsWorld){ std::cerr<<"Failed to create Bullet dynamics world"<<std::endl; return false; }
    m_dynamicsWorld->setGravity(btVector3(0,-9.81f,0));
    return true;
}

void PhysicsManager::shutdown(){
    if(m_dynamicsWorld){
        // remove bodies
        for(int i=m_dynamicsWorld->getNumCollisionObjects()-1;i>=0;--i){
            btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if(body && body->getMotionState()){
                delete body->getMotionState();
            }
            m_dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }
        delete m_dynamicsWorld; m_dynamicsWorld = nullptr;
    }
    delete m_solver; m_solver=nullptr;
    delete m_dispatcher; m_dispatcher=nullptr;
    delete m_collisionConfiguration; m_collisionConfiguration=nullptr;
    delete m_broadphase; m_broadphase=nullptr;
}

void PhysicsManager::stepSimulation(double dt){
    if(!m_dynamicsWorld) return;
    // fixed-step: use up to 10 substeps of 1/60
    m_dynamicsWorld->stepSimulation((btScalar)dt, 10, (btScalar)(1.0/60.0));
}

std::shared_ptr<PhysicsComponent> PhysicsManager::createStaticBox(const btVector3& halfExtents, const btTransform& transform){
    btCollisionShape* shape = new btBoxShape(halfExtents);
    btDefaultMotionState* motion = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion, shape, btVector3(0,0,0));
    btRigidBody* body = new btRigidBody(info);
    m_dynamicsWorld->addRigidBody(body);
    auto comp = std::make_shared<PhysicsComponent>(shape, body);
    if(body) body->setUserPointer((void*)comp.get());
    m_componentMap[comp.get()] = comp;
    return comp;
}

std::shared_ptr<PhysicsComponent> PhysicsManager::createDynamicBox(const btVector3& halfExtents, const btTransform& transform, float mass){
    btCollisionShape* shape = new btBoxShape(halfExtents);
    btVector3 localInertia(0,0,0);
    shape->calculateLocalInertia(mass, localInertia);
    btDefaultMotionState* motion = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, shape, localInertia);
    btRigidBody* body = new btRigidBody(info);
    m_dynamicsWorld->addRigidBody(body);
    auto comp = std::make_shared<PhysicsComponent>(shape, body);
    if(body) body->setUserPointer((void*)comp.get());
    m_componentMap[comp.get()] = comp;
    return comp;
}



void PhysicsManager::registerPickupCallback(PickupCallback cb){
    m_pickupCallback = cb;
}

void PhysicsManager::processCollisionCallbacks(){
    if(!m_dynamicsWorld || !m_pickupCallback) return;
    int numManifolds = m_dispatcher->getNumManifolds();
    for(int i=0;i<numManifolds;i++){
        btPersistentManifold* manifold = m_dispatcher->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = manifold->getBody0();
        const btCollisionObject* obB = manifold->getBody1();
        if(!obA || !obB) continue;
        auto ptrA_raw = (PhysicsComponent*)obA->getUserPointer();
        auto ptrB_raw = (PhysicsComponent*)obB->getUserPointer();
        if(!ptrA_raw || !ptrB_raw) continue;
        auto itA = m_componentMap.find(ptrA_raw);
        auto itB = m_componentMap.find(ptrB_raw);
        if(itA==m_componentMap.end() || itB==m_componentMap.end()) continue;
        auto a = itA->second.lock();
        auto b = itB->second.lock();
        if(!a || !b) continue;
        if(a->getUserTag() == PhysicsComponent::TAG_PICKUP && b->getUserTag() == PhysicsComponent::TAG_PLAYER){
            m_pickupCallback(a,b);
        } else if(b->getUserTag() == PhysicsComponent::TAG_PICKUP && a->getUserTag() == PhysicsComponent::TAG_PLAYER){
            m_pickupCallback(b,a);
        }
    }
}

std::vector<btVector3> PhysicsManager::generateNavMesh(const btVector3& minBounds, const btVector3& maxBounds, float cellSize){
    std::vector<btVector3> nodes;
    if(!m_dynamicsWorld) return nodes;
    int nx = (int)std::ceil((maxBounds.getX()-minBounds.getX())/cellSize);
    int nz = (int)std::ceil((maxBounds.getZ()-minBounds.getZ())/cellSize);
    for(int ix=0; ix<nx; ++ix){
        for(int iz=0; iz<nz; ++iz){
            float x = minBounds.getX() + (ix+0.5f)*cellSize;
            float z = minBounds.getZ() + (iz+0.5f)*cellSize;
            btVector3 from(x, maxBounds.getY()+10.0f, z);
            btVector3 to(x, minBounds.getY()-10.0f, z);
            btCollisionWorld::ClosestRayResultCallback raycb(from, to);
            m_dynamicsWorld->rayTest(from, to, raycb);
            if(raycb.hasHit()){
                btVector3 hit = raycb.m_hitPointWorld;
                btVector3 normal = raycb.m_hitNormalWorld;
                if(normal.getY() > 0.7f) nodes.push_back(hit);
            }
        }
    }
    return nodes;
}


void PhysicsManager::enableDebugDraw(bool enable){ m_debugDraw = enable; }
bool PhysicsManager::debugDrawEnabled() const { return m_debugDraw; }
