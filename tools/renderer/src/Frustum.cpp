#include "Frustum.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>
namespace CoreTech {
Frustum Frustum::fromViewProj(const glm::mat4& vp){
    Frustum f;
    // extract planes from VP matrix (row-major access)
    // Left
    f.planes[0] = glm::vec4(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]);
    // Right
    f.planes[1] = glm::vec4(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]);
    // Bottom
    f.planes[2] = glm::vec4(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]);
    // Top
    f.planes[3] = glm::vec4(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]);
    // Near
    f.planes[4] = glm::vec4(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2]);
    // Far
    f.planes[5] = glm::vec4(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2]);
    // normalize planes
    for(int i=0;i<6;i++){
        float l = glm::length(glm::vec3(f.planes[i]));
        if(l>0.00001f) f.planes[i] /= l;
    }
    return f;
}
bool Frustum::sphereInFrustum(const glm::vec3& center, float radius) const{
    for(int i=0;i<6;i++){
        glm::vec4 p = planes[i];
        float d = p.x*center.x + p.y*center.y + p.z*center.z + p.w;
        if(d < -radius) return false;
    }
    return true;
}
}