#pragma once
#include <vector>
#include <glm/glm.hpp>
struct HZB {
  int width, height; std::vector<float> data; // base level depth (0..1)
  std::vector<std::vector<float>> mips; // mip levels: each mip is width*height/(4^level)
  void BuildFromDepthBuffer(const std::vector<float>& depth, int w, int h);
  // Query: given projected AABB in NDC (x_min,y_min,x_max,y_max) and min/max depth in 0..1, return occluded or not
  bool IsOccludedNDCRange(float xmin,float ymin,float xmax,float ymax, float zmin, float zmax) const;
};
