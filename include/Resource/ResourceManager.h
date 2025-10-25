#pragma once
#include <string>
#include <unordered_map>
#include <memory>

namespace coretech::res {

class Resource {
public:
    virtual ~Resource() = default;
    std::string path;
};

using ResourcePtr = std::shared_ptr<Resource>;

class ResourceManager {
public:
    static ResourceManager& instance();
    ResourcePtr loadText(const std::string& path);
    ResourcePtr get(const std::string& path);
    void unload(const std::string& path);
private:
    ResourceManager() = default;
    std::unordered_map<std::string, ResourcePtr> cache_;
};

} // namespace coretech::res
