#pragma once
#include <string>
#include <unordered_map>

class ConfigLoader {
public:
    bool load(const std::string& path);
    std::string get(const std::string& key, const std::string& def="") const;
private:
    std::unordered_map<std::string,std::string> m_map;
};
