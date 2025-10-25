#include "Scene/Scene.h"
#include "Resource/ResourceManager.h"
#include <iostream>

namespace coretech {

bool Scene::loadFromFile(const std::string& path) {
    auto r = coretech::res::ResourceManager::instance().loadText(path);
    if(!r) return false;
    loadedPath_ = path;
    std::cout << "Scene: loaded " << path << " (" << (r.use_count()) << " refs)\\n";
    return true;
}

void Scene::update(float dt) {
    // For now, nothing. Hook ECS update here.
    (void)dt;
}

void Scene::saveToFile(const std::string& path) {
    // simple save stub
    std::cout << "Scene: saved to " << path << "\\n";
}

} // namespace coretech
