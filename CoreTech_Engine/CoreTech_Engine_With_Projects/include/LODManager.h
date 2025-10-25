#pragma once
#include <vector>
#include <glm/glm.hpp>
struct LODEntry { int modelId; float distanceThreshold; };
struct LODManager { std::vector<LODEntry> entries; int Select(float distance) const; };
