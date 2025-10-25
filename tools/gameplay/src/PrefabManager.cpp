#include "PrefabManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>
namespace CoreTech {
std::vector<std::string> PrefabManager::listPrefabs(const std::string& folder){
    std::vector<std::string> out;
    try{
        for(auto &p : std::filesystem::directory_iterator(folder)){
            if(p.is_regular_file()) out.push_back(p.path().string());
        }
    }catch(...){}
    return out;
}
bool PrefabManager::savePrefabJson(const std::string& path, const std::string& jsonStr){
    try{ std::ofstream o(path); o<<jsonStr; return true;}catch(...){ return false; }
}
std::string PrefabManager::loadPrefabJson(const std::string& path){
    try{ std::ifstream i(path); std::stringstream s; s<<i.rdbuf(); return s.str(); }catch(...){ return ""; }
}
}