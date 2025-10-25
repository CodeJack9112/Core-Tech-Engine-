#include "Prefab.h"
#include "SceneLoader.h"
#include "Entity.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
namespace CoreTech {
bool savePrefab(const std::shared_ptr<Entity>& e, const std::string& path){
    if(!e){ return false; }
    nlohmann::json j;
    j["name"]=e->getName();
    // transform
    if(auto t = e->getComponent<TransformComponent>()){
        j["transform"] = { {"x",t->x},{"y",t->y},{"z",t->z},{"rx",t->rx},{"ry",t->ry},{"rz",t->rz},{"sx",t->sx},{"sy",t->sy},{"sz",t->sz} };
    }
    // material simple
    if(auto m = e->getComponent<MaterialComponent>()){
        j["material"] = { {"name", m->name}, {"albedo", {m->albedo.x,m->albedo.y,m->albedo.z}}, {"albedoTexture", m->albedoTexture} };
    }
    try{ std::ofstream o(path); o<<j.dump(2); return true; }catch(...){ return false; }
}
std::shared_ptr<Entity> loadPrefab(const std::shared_ptr<Scene>& scene, const std::string& path){
    std::ifstream in(path);
    if(!in){ std::cerr<<"Prefab load failed: "<<path<<"\n"; return nullptr; }
    nlohmann::json j; in>>j;
    auto e = scene->createEntity(j.value("name","prefab"));
    if(j.contains("transform")){
        auto t = e->addComponent<TransformComponent>();
        t->x = j["transform"].value("x",0.0f);
        t->y = j["transform"].value("y",0.0f);
        t->z = j["transform"].value("z",0.0f);
        t->rx = j["transform"].value("rx",0.0f);
        t->ry = j["transform"].value("ry",0.0f);
        t->rz = j["transform"].value("rz",0.0f);
        t->sx = j["transform"].value("sx",1.0f);
        t->sy = j["transform"].value("sy",1.0f);
        t->sz = j["transform"].value("sz",1.0f);
    }
    if(j.contains("material")){
        auto m = e->addComponent<MaterialComponent>();
        m->name = j["material"].value("name","material");
        auto alb = j["material"]["albedo"];
        if(alb.is_array() && alb.size()==3) m->albedo = glm::vec3(alb[0], alb[1], alb[2]);
        m->albedoTexture = j["material"].value("albedoTexture","");
    }
    return e;
}