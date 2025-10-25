
#pragma once
// Minimal stub of essential Bullet types to allow building without Bullet installed.
// This is NOT a physics implementation — only lightweight types to satisfy compilation.

#include <cmath>
#include <cstdint>

class btVector3 {
public:
    float m_x, m_y, m_z;
    btVector3(): m_x(0), m_y(0), m_z(0) {}
    btVector3(float x, float y, float z): m_x(x), m_y(y), m_z(z) {}
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getZ() const { return m_z; }
    void setValue(float x, float y, float z) { m_x=x; m_y=y; m_z=z; }
    btVector3 operator-(const btVector3& o) const { return btVector3(m_x-o.m_x, m_y-o.m_y, m_z-o.m_z); }
    float length() const { return std::sqrt(m_x*m_x + m_y*m_y + m_z*m_z); }
};

class btQuaternion {
public:
    float x,y,z,w;
    btQuaternion(): x(0),y(0),z(0),w(1) {}
};

class btTransform {
public:
    btQuaternion m_q;
    btVector3 m_v;
    btTransform(): m_q(), m_v() {}
    btVector3 getOrigin() const { return m_v; }
    void setOrigin(const btVector3& v) { m_v = v; }
    void setIdentity() { m_q = btQuaternion(); m_v = btVector3(); }
};

// Collision and rigid body placeholders
class btCollisionShape {};
class btCollisionObject {};
class btRigidBody {
public:
    btRigidBody(int, void*, btCollisionShape*) {}
    virtual ~btRigidBody() {}
    void setUserPointer(void*) {}
};

// Dispatcher / world placeholders
class btDefaultCollisionConfiguration {};
class btCollisionDispatcher {};
class btDbvtBroadphase {};
class btSequentialImpulseConstraintSolver {};
class btDiscreteDynamicsWorld {};
