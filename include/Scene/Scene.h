#pragma once
#include <vector>
#include <string>
#include <memory>

namespace coretech {

class Scene {
public:
    Scene() = default;
    bool loadFromFile(const std::string& path);
    void update(float dt);
    void saveToFile(const std::string& path);
private:
    std::string loadedPath_;
};

using ScenePtr = std::shared_ptr<Scene>;

} // namespace coretech
