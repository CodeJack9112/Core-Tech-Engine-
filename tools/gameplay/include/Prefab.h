#pragma once
#include <string>
#include <memory>
namespace CoreTech { class Scene; }
namespace CoreTech {
bool savePrefab(const std::shared_ptr<CoreTech::Entity>& e, const std::string& path);
std::shared_ptr<CoreTech::Entity> loadPrefab(const std::shared_ptr<CoreTech::Scene>& scene, const std::string& path);
}