#pragma once
#include <glm/glm.hpp>
namespace CoreTech {
struct Frustum {
    // 6 planes: each plane is (a,b,c,d) such that ax+by+cz+d >= 0 is inside
    glm::vec4 planes[6];
    static Frustum fromViewProj(const glm::mat4& vp);
    bool sphereInFrustum(const glm::vec3& center, float radius) const;
};
}