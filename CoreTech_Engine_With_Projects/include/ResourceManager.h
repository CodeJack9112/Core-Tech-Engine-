
#pragma once
#include <string>
#include <unordered_map>
#include <memory>

class ResourceManager {
public:
    ResourceManager() = default;
    const std::string& loadText(const std::string& path); // returns cached string (file contents)
private:
    std::unordered_map<std::string, std::shared_ptr<std::string>> m_cache;
};
