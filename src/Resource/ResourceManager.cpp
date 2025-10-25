#include "Resource/ResourceManager.h"
#include <fstream>
#include <iostream>

namespace coretech::res {

struct TextResource : public Resource {
    std::string data;
};

ResourceManager& ResourceManager::instance() {
    static ResourceManager rm;
    return rm;
}

ResourcePtr ResourceManager::loadText(const std::string& path) {
    auto it = cache_.find(path);
    if(it != cache_.end()) return it->second;
    std::ifstream ifs(path);
    if(!ifs) {
        std::cerr << "ResourceManager: failed to open " << path << "\\n";
        return nullptr;
    }
    auto r = std::make_shared<TextResource>();
    r->path = path;
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    r->data = std::move(content);
    cache_[path] = r;
    return r;
}

ResourcePtr ResourceManager::get(const std::string& path) {
    auto it = cache_.find(path);
    if(it!=cache_.end()) return it->second;
    return nullptr;
}

void ResourceManager::unload(const std::string& path) {
    cache_.erase(path);
}

} // namespace coretech::res
