#pragma once
#include <vector>
#include <glm/glm.hpp>
struct InstanceData { glm::mat4 model; glm::vec4 color; };
struct GPUInstancer {
  std::vector<InstanceData> instances;
  void Clear(); void Add(const InstanceData& d); void Upload(); void BindAndDraw(unsigned meshVao, unsigned indexCount);
};
