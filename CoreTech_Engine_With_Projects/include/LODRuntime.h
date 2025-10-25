#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
namespace coretech {
  struct LODLevel { std::string meshPath; float distanceThreshold; };
  // Register LODs for an entity (or model id). The system will pick the appropriate LOD based on camera distance.
  void LOD_Register(const std::string& id, const std::vector<LODLevel>& levels);
  void LOD_Unregister(const std::string& id);
  // Call every frame with camera position to update LODs. best-effort: will call renderer API if available.
  void LOD_Update(const glm::vec3& cameraPos);
}
