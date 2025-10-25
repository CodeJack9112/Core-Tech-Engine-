#pragma once
#include <array>
#include <glm/glm.hpp>
struct AABB { glm::vec3 min, max; };
struct Frustum {
  // 6 planes: left, right, top, bottom, near, far
  std::array<glm::vec4,6> planes;
  // build from view-projection matrix
  static Frustum FromMatrix(const glm::mat4& vp);
  bool Intersects(const AABB& box) const;
};
