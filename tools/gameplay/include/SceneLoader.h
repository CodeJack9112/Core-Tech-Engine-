#pragma once
#include "Entity.h"
#include <string>

namespace CoreTech {

class SceneLoader {
public:
    // Load a simple scene format:
    // Each line defines an entity:
    // name:type:x:y:z
    // e.g. Player:camera:0:1.6:0
    static std::shared_ptr<Scene> loadFromFile(const std::string& path);
};

} // namespace CoreTech
