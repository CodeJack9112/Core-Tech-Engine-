
#include "ResourceManager.h"
#include <fstream>
#include <iostream>

const std::string& ResourceManager::loadText(const std::string& path){
    auto it = m_cache.find(path);
    if(it != m_cache.end()) return *it->second;
    std::ifstream f(path);
    if(!f){ static std::string empty=\"\"; std::cerr << "[Res] Failed to load " << path << "\\n"; return empty; }
    auto buf = std::make_shared<std::string>(std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()));
    m_cache[path] = buf;
    return *buf;
}
