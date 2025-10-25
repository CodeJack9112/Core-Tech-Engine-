#pragma once
#include <vector>
#include <glm/glm.hpp>
struct BVH {
  struct Node { glm::vec3 min, max; int left, right; int firstPrim, primCount; };
  std::vector<Node> nodes;
  void Build(const std::vector<glm::vec3>& primCenters);
};
