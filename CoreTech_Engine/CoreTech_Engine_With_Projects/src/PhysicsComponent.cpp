#include "../include/PhysicsComponent.h"
#include <iostream>

PhysicsComponent::PhysicsComponent(btCollisionShape* shape, btRigidBody* body)
: m_shape(shape), m_body(body), m_ownsShape(true)
{
}

PhysicsComponent::~PhysicsComponent(){
    if(m_body){
        if(m_body->getMotionState()) delete m_body->getMotionState();
        delete m_body;
        m_body = nullptr;
    }
    if(m_ownsShape && m_shape){ delete m_shape; m_shape = nullptr; }
}

void PhysicsComponent::setKinematic(bool k){
    if(!m_body) return;
    if(k){
        m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        m_body->setActivationState(DISABLE_DEACTIVATION);
    } else {
        m_body->setCollisionFlags(m_body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        m_body->setActivationState(ACTIVE_TAG);
    }
}

void PhysicsComponent::setTransform(const btTransform& t){ if(m_body){ m_body->setWorldTransform(t); if(m_body->getMotionState()) m_body->getMotionState()->setWorldTransform(t); } }
btTransform PhysicsComponent::getTransform() const { if(m_body) return m_body->getWorldTransform(); return btTransform::getIdentity(); }
