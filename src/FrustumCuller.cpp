    #include "FrustumCuller.h"
#include <glm/gtc/matrix_access.hpp>

Frustum Frustum::FromMatrix(const glm::mat4& m) {
    Frustum f;
    // extract planes (row-major assumed)
    // left
    f.planes[0] = glm::vec4(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]);
    // right
    f.planes[1] = glm::vec4(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]);
    // bottom
    f.planes[2] = glm::vec4(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]);
    // top
    f.planes[3] = glm::vec4(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]);
    // near
    f.planes[4] = glm::vec4(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]);
    // far
    f.planes[5] = glm::vec4(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]);
    // normalize
    for(int i=0;i<6;i++){ float l = glm::length(glm::vec3(f.planes[i])); f.planes[i] /= l; }
    return f;
}

bool Frustum::Intersects(const AABB& box) const {
    for(int i=0;i<6;i++){
        glm::vec3 n(planes[i]); float d = planes[i].w;
        glm::vec3 p = glm::vec3( (n.x>0?box.max.x:box.min.x), (n.y>0?box.max.y:box.min.y), (n.z>0?box.max.z:box.min.z) );
        if(glm::dot(n,p) + d < 0) return false;
    }
    return true;
}
