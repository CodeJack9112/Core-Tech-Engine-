#pragma once
#include <memory>
#include <btBulletDynamicsCommon.h>

// Lightweight wrapper for a btRigidBody bound to an Core Tech object.
// The Core Tech can store a shared_ptr<PhysicsComponent> and sync transforms.

class PhysicsComponent {
public:
    enum Tag { TAG_NONE = 0, TAG_PLAYER = 1, TAG_PICKUP = 2 };
public:
    PhysicsComponent(btCollisionShape* shape, btRigidBody* body);
    ~PhysicsComponent();

    btRigidBody* body() const { return m_body; }
    void setUserTag(int t) { m_userTag = t; }
    int getUserTag() const { return m_userTag; }
    btCollisionShape* shape() const { return m_shape; }

    // Helper: set kinematic (disable dynamics, set as kinematic object)
    void setKinematic(bool k);

    // Helper: set transform from Core Tech (when gizmo moved)
    void setTransform(const btTransform& t);

    // Helper: get transform to apply to Core Tech model matrix
    btTransform getTransform() const;

private:
    btCollisionShape* m_shape = nullptr;
    btRigidBody* m_body = nullptr;
    bool m_ownsShape = false;
    int m_userTag = 0;
};
