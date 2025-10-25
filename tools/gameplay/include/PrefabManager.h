#pragma once
#include <string>
#include <vector>
namespace CoreTech {
class Scene;
class PrefabManager {
public:
    static std::vector<std::string> listPrefabs(const std::string& folder="tools/prefabs");
    static bool savePrefabJson(const std::string& path, const std::string& jsonStr);
    static std::string loadPrefabJson(const std::string& path);
};
}