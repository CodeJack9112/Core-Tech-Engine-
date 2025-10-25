#include "../include/ConfigLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

bool ConfigLoader::load(const std::string& path){
    std::ifstream f(path);
    if(!f) return false;
    std::string line;
    while(std::getline(f, line)){
        auto pos = line.find('=');
        if(pos==std::string::npos) continue;
        std::string k = line.substr(0,pos);
        std::string v = line.substr(pos+1);
        // trim
        auto trim = [](std::string &s){ while(!s.empty() && isspace((unsigned char)s.back())) s.pop_back(); while(!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin()); };
        trim(k); trim(v);
        std::transform(k.begin(), k.end(), k.begin(), ::tolower);
        m_map[k]=v;
    }
    return true;
}

std::string ConfigLoader::get(const std::string& key, const std::string& def) const {
    std::string lk = key; std::transform(lk.begin(), lk.end(), lk.begin(), ::tolower);
    auto it = m_map.find(lk);
    if(it==m_map.end()) return def;
    return it->second;
}
