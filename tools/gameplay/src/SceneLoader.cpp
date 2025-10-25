#include "SceneLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace CoreTech {

std::shared_ptr<Scene> SceneLoader::loadFromFile(const std::string& path){
    auto scene = std::make_shared<Scene>();
    // try JSON first
    std::ifstream in(path);
    if(!in){
        std::cerr << "SceneLoader: failed to open: " << path << std::endl;
        return scene;
    }
    // read entire file
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    // detect JSON by leading '{' or '['
    std::string s = content;
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    if(!s.empty() && (s[0]=='{' || s[0]=='[')){
        try{
            auto j = json::parse(content);
            if(j.is_array()){ /* extended loader supports light/ai/prefab */
                for(auto &item : j){
                    std::string name = item.value("name","entity");
                    std::string type = item.value("type","entity");
                    float x = item.value("x", 0.0f);
                    float y = item.value("y", 0.0f);
                    float z = item.value("z", 0.0f);
                    auto e = scene->createEntity(name);
                    if(type=="camera") e->addComponent<CameraComponent>();
                    auto t = e->addComponent<TransformComponent>();
                    t->x = x; t->y = y; t->z = z;
                }
            } else if(j.is_object() && j.contains("entities")){
                for(auto &item : j["entities"]){
                    std::string name = item.value("name","entity");
                    std::string type = item.value("type","entity");
                    float x = item.value("x", 0.0f);
                    float y = item.value("y", 0.0f);
                    float z = item.value("z", 0.0f);
                    auto e = scene->createEntity(name);
                    if(type=="camera") e->addComponent<CameraComponent>();
                    auto t = e->addComponent<TransformComponent>();
                    t->x = x; t->y = y; t->z = z;
                }
            }
            return scene;
        }catch(std::exception &ex){
            std::cerr << "JSON parse error: " << ex.what() << std::endl;
            // fallthrough to legacy parser
        }
    }
    // legacy simple parser (line-based)
    std::istringstream iss(content);
    std::string line;
    while(std::getline(iss, line)){
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if(line.empty() || line[0]=='#') continue;
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string tok;
        while(std::getline(ss, tok, ':')) parts.push_back(tok);
        if(parts.size() < 2) continue;
        std::string name = parts[0];
        std::string type = parts[1];
        auto e = scene->createEntity(name);
        if(type == "camera"){
            e->addComponent<CameraComponent>();
            auto t = e->addComponent<TransformComponent>();
            if(parts.size()>=5){
                t->x = std::stof(parts[2]);
                t->y = std::stof(parts[3]);
                t->z = std::stof(parts[4]);
            }
        } else {
            auto t = e->addComponent<TransformComponent>();
            if(parts.size()>=5){
                t->x = std::stof(parts[2]);
                t->y = std::stof(parts[3]);
                t->z = std::stof(parts[4]);
            }
        }
    }
    return scene;
}


// Save scene to JSON file (overwrites)
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace CoreTech {

// Enhanced save that includes components' basic fields
bool saveSceneToFile(const std::shared_ptr<Scene>& scene, const std::string& path){
    if(!scene) return false;
    nlohmann::json j; j["entities"] = nlohmann::json::array();
    for(auto &e: scene->getAllEntities()){
        nlohmann::json item;
        item["id"] = e->getId();
        item["name"] = e->getName();
        auto t = e->getComponent<TransformComponent>();
        if(t){ item["transform"] = { {"x",t->x}, {"y",t->y}, {"z",t->z}, {"rx",t->rx}, {"ry",t->ry}, {"rz",t->rz}, {"sx",t->sx}, {"sy",t->sy}, {"sz",t->sz} }; }
        auto m = e->getComponent<MaterialComponent>();
        if(m){ item["material"] = { {"name", m->name}, {"albedo", {m->albedo.x,m->albedo.y,m->albedo.z}}, {"albedoTexture", m->albedoTexture} }; }
        auto p = e->getComponent<PhysicsComponent>();
        if(p){ item["physics"] = { {"mass", p->mass}, {"dynamic", p->dynamic}, {"shape", (int)p->shape} }; }
        j["entities"].push_back(item);
    }
    try{ std::ofstream o(path); o<<j.dump(2); return true; }catch(...){ return false; }
}
