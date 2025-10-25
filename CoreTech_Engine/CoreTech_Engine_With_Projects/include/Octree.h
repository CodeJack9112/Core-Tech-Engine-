#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <functional>
struct Octree {
  struct Node { glm::vec3 center; float halfSize; std::vector<int> objects; Node* children[8]; Node(); ~Node(); };
  Node* root = nullptr; int maxDepth=6; int maxObjects=8;
  Octree(const glm::vec3& center, float halfSize);
  ~Octree();
  void Insert(int objectId, const glm::vec3& pos);
  void QueryRange(const glm::vec3& center, float radius, std::vector<int>& out);
};
